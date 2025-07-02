#include "comp.h"
#include <imgui.h>
#include <imgui_internal.h>
//
// Imgui and plugin
//
#include <imgui_node_editor.h>
#include <Imgui/node/node_database.h>
#include <Imgui/imgui_node_data.h>
#include <Imgui/utility/nodeutility.h>
#include <utilities/builders.h>
#include <system/gui.h>

std::vector<Aktion::GUI::NodeAddCategory> GetNodeCompAddList()
{
  std::vector<Aktion::GUI::NodeAddCategory> r = std::vector<Aktion::GUI::NodeAddCategory>();
  std::vector<Aktion::GUI::NodeAddType> export_list = std::vector<Aktion::GUI::NodeAddType>();
  export_list.push_back({ "Export View", "", Aktion::Database::Spawn_ExportView });
  r.push_back({ "Export", "", export_list });
  return r;
}

void Aktion::GUI::GUI_Comp_PopupAddNode(Aktion::GUI::AktionGUI& host, NodeEditorBuffer& target)
{
  ax::NodeEditor::SetCurrentEditor(target.m_EditorContext);
  NodeUtility_SetBuffer(target);
  Aktion::Database::Node_Database(&target.m_Nodes, NodeUtility_BuildNode, NodeUtility_GetNextId);
  auto openPopupPosition = ax::NodeEditor::ScreenToCanvas(target.m_CursorLastPos);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
  // Always center this window when appearing
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImVec2 size = ImGui::GetWindowSize();
  ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(size * 0.8f, ImGuiCond_Always);

  if (ImGui::BeginPopupModal("Create Comp Node", &target.m_ShowAddNode, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
  {
    auto newNodePostion = openPopupPosition;
    Node* node = nullptr;
    if (target.m_NodeAddBuffer.size() == 0 || target.m_UpdateNodeList)
      target.m_NodeAddBuffer = GetNodeCompAddList();

    for (auto i = target.m_NodeAddBuffer.begin(); i < target.m_NodeAddBuffer.end(); i++)
    {
      if (ImGui::TreeNode((*i).m_Title.c_str()))
      {
        for (auto j = (*i).m_List.begin(); j < (*i).m_List.end(); j++)
        {
          if (ImGui::Selectable((*j).m_Name.c_str()))
            node = (*j).m_Call();
        }
        ImGui::TreePop();
      }
    }

    if (node) {

      ImGui::CloseCurrentPopup();
      host.m_Comp.m_ShowAddNode = false;

      NodeUtility_BuildNodes();

      target.m_CreateNewNode = false;

      ax::NodeEditor::SetNodePosition(node->ID, newNodePostion);

      if (auto startPin = target.m_NewNodeLinkPin)
      {
        auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

        for (auto& pin : pins)
        {
          if (NodeUtility_CanCreateLink(startPin, &pin))
          {
            auto endPin = &pin;
            if (startPin->Kind == PinKind::Input)
              std::swap(startPin, endPin);

            target.m_Links.emplace_back(Link(NodeUtility_GetNextId(), startPin->ID, endPin->ID));
            target.m_Links.back().Color = Aktion::Database::GetIconColor(startPin->Type);

            break;
          }
        }
      }
    }
    ImGui::EndPopup();
  }
  else
  {
    target.m_CreateNewNode = false;
  }
  ImGui::PopStyleVar();
}
void Aktion::GUI::GUI_Comp_InitNodeEditor(NodeEditorBuffer& target)
{
  ax::NodeEditor::Config config;
  NodeUtility_SetBuffer(target);
  config.SettingsFile = "Simple.json";
  config.UserPointer = &target;
  config.LoadNodeSettings = [](ax::NodeEditor::NodeId nodeId, char* data, void* userPointer) -> size_t
    {
      auto self = static_cast<AktionGUI*>(userPointer);

      auto node = NodeUtility_FindNode(nodeId);
      if (!node)
        return 0;

      if (data != nullptr)
        memcpy(data, node->State.data(), node->State.size());
      return node->State.size();
    };
  config.SaveNodeSettings = [](ax::NodeEditor::NodeId nodeId, const char* data, size_t size, ax::NodeEditor::SaveReasonFlags reason, void* userPointer) -> bool
    {
      auto self = static_cast<AktionGUI*>(userPointer);

      auto node = NodeUtility_FindNode(nodeId);
      if (!node)
        return false;

      node->State.assign(data, size);

      NodeUtility_TouchNode(nodeId);

      return true;
    };
  target.m_EditorContext = ax::NodeEditor::CreateEditor(&config);
  ax::NodeEditor::SetCurrentEditor(target.m_EditorContext);

  target.m_Nodes = std::vector<Node>();
  target.m_Links = std::vector<Link>();
  Aktion::Database::Node_Database(&target.m_Nodes, NodeUtility_BuildNode, NodeUtility_GetNextId);

  Node* n = Aktion::Database::Spawn_ExportView();
  NodeUtility_BuildNode(n);
  ax::NodeEditor::SetNodePosition(target.m_Nodes.back().ID, ImVec2(0, 0));
  ax::NodeEditor::NavigateToContent();
}
void Aktion::GUI::GUI_Comp_DrawNodeEditor(Aktion::GUI::AktionGUI& target)
{
  NodeUtility_SetBuffer(target.m_Comp);
  Aktion::Database::Node_Database(&target.m_Comp.m_Nodes, NodeUtility_BuildNode, NodeUtility_GetNextId);
  ax::NodeEditor::Begin("My Editor");
  {
    auto cursorTopLeft = ImGui::GetCursorScreenPos();

    ax::NodeEditor::Utilities::BlueprintNodeBuilder builder(target.m_Comp.m_HeaderBackground, GUI_GetTextureWidth(target.m_Comp.m_HeaderBackground), GUI_GetTextureHeight(target.m_Comp.m_HeaderBackground));

    for (auto& node : target.m_Comp.m_Nodes)
    {
      if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple)
        continue;

      const auto isSimple = node.Type == NodeType::Simple;

      bool hasOutputDelegates = false;
      for (auto& output : node.Outputs)
        if (output.Type == PinType::Delegate)
          hasOutputDelegates = true;

      builder.Begin(node.ID);
      if (!isSimple)
      {
        builder.Header(node.Color);
        ImGui::Spring(0);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1);
        ImGui::Dummy(ImVec2(0, 28));
        if (hasOutputDelegates)
        {
          ImGui::BeginVertical("delegates", ImVec2(0, 28));
          ImGui::Spring(1, 0);
          for (auto& output : node.Outputs)
          {
            if (output.Type != PinType::Delegate)
              continue;

            auto alpha = ImGui::GetStyle().Alpha;
            if (target.m_Comp.m_NewLinkPin && !NodeUtility_CanCreateLink(target.m_Comp.m_NewLinkPin, &output) && &output != target.m_Comp.m_NewLinkPin)
              alpha = alpha * (48.0f / 255.0f);

            ax::NodeEditor::BeginPin(output.ID, ax::NodeEditor::PinKind::Output);
            ax::NodeEditor::PinPivotAlignment(ImVec2(1.0f, 0.5f));
            ax::NodeEditor::PinPivotSize(ImVec2(0, 0));
            ImGui::BeginHorizontal(output.ID.AsPointer());
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            if (!output.Name.empty())
            {
              ImGui::TextUnformatted(output.Name.c_str());
              ImGui::Spring(0);
            }
            GUI_DrawPinIcon(output, NodeUtility_IsPinLinked(output.ID), (int)(alpha * 255));
            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
            ImGui::EndHorizontal();
            ImGui::PopStyleVar();
            ax::NodeEditor::EndPin();

            //DrawItemRect(ImColor(255, 0, 0));
          }
          ImGui::Spring(1, 0);
          ImGui::EndVertical();
          ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
        }
        else
          ImGui::Spring(0);
        builder.EndHeader();
      }

      for (auto& input : node.Inputs)
      {
        auto alpha = ImGui::GetStyle().Alpha;
        if (target.m_Comp.m_NewLinkPin && !NodeUtility_CanCreateLink(target.m_Comp.m_NewLinkPin, &input) && &input != target.m_Comp.m_NewLinkPin)
          alpha = alpha * (48.0f / 255.0f);

        builder.Input(input.ID);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        GUI_DrawPinIcon(input, NodeUtility_IsPinLinked(input.ID), (int)(alpha * 255));
        ImGui::Spring(0);
        if (!input.Name.empty())
        {
          ImGui::TextUnformatted(input.Name.c_str());
          ImGui::Spring(0);
        }
        if (input.Type == PinType::Bool)
        {
          ImGui::Button("Hello");
          ImGui::Spring(0);
        }
        ImGui::PopStyleVar();
        builder.EndInput();
      }

      if (isSimple)
      {
        builder.Middle();

        ImGui::Spring(1, 0);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1, 0);
      }

      for (auto& output : node.Outputs)
      {
        if (!isSimple && output.Type == PinType::Delegate)
          continue;

        auto alpha = ImGui::GetStyle().Alpha;
        if (target.m_Comp.m_NewLinkPin && !NodeUtility_CanCreateLink(target.m_Comp.m_NewLinkPin, &output) && &output != target.m_Comp.m_NewLinkPin)
          alpha = alpha * (48.0f / 255.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        builder.Output(output.ID);
        if (output.Type == PinType::String)
        {
          static char buffer[128] = "Edit Me\nMultiline!";
          static bool wasActive = false;

          ImGui::PushItemWidth(100.0f);
          ImGui::InputText("##edit", buffer, 127);
          ImGui::PopItemWidth();
          if (ImGui::IsItemActive() && !wasActive)
          {
            ax::NodeEditor::EnableShortcuts(false);
            wasActive = true;
          }
          else if (!ImGui::IsItemActive() && wasActive)
          {
            ax::NodeEditor::EnableShortcuts(true);
            wasActive = false;
          }
          ImGui::Spring(0);
        }
        if (!output.Name.empty())
        {
          ImGui::Spring(0);
          ImGui::TextUnformatted(output.Name.c_str());
        }
        ImGui::Spring(0);
        GUI_DrawPinIcon(output, NodeUtility_IsPinLinked(output.ID), (int)(alpha * 255));
        ImGui::PopStyleVar();
        builder.EndOutput();
      }

      builder.End();
    }

    for (auto& node : target.m_Comp.m_Nodes)
    {
      if (node.Type != NodeType::Tree)
        continue;

      const float rounding = 5.0f;
      const float padding = 12.0f;

      const auto pinBackground = ax::NodeEditor::GetStyle().Colors[ax::NodeEditor::StyleColor_NodeBg];

      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_PinRect, ImColor(60, 180, 255, 150));
      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodeRounding, rounding);
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 0.0f);
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinBorderWidth, 1.0f);
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinRadius, 5.0f);
      ax::NodeEditor::BeginNode(node.ID);

      ImGui::BeginVertical(node.ID.AsPointer());
      ImGui::BeginHorizontal("inputs");
      ImGui::Spring(0, padding * 2);

      ImRect inputsRect;
      int inputAlpha = 200;
      if (!node.Inputs.empty())
      {
        auto& pin = node.Inputs[0];
        ImGui::Dummy(ImVec2(0, padding));
        ImGui::Spring(1, 0);
        inputsRect = GUI_ImGui_GetItemRect();

        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowSize, 10.0f);
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowWidth, 10.0f);
#if IMGUI_VERSION_NUM > 18101
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);
#else
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinCorners, 12);
#endif
        ax::NodeEditor::BeginPin(pin.ID, ax::NodeEditor::PinKind::Input);
        ax::NodeEditor::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
        ax::NodeEditor::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
        ax::NodeEditor::EndPin();
        ax::NodeEditor::PopStyleVar(3);

        if (target.m_Comp.m_NewLinkPin && !NodeUtility_CanCreateLink(target.m_Comp.m_NewLinkPin, &pin) && &pin != target.m_Comp.m_NewLinkPin)
          inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
      }
      else
        ImGui::Dummy(ImVec2(0, padding));

      ImGui::Spring(0, padding * 2);
      ImGui::EndHorizontal();

      ImGui::BeginHorizontal("content_frame");
      ImGui::Spring(1, padding);

      ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
      ImGui::Dummy(ImVec2(160, 0));
      ImGui::Spring(1);
      ImGui::TextUnformatted(node.Name.c_str());
      ImGui::Spring(1);
      ImGui::EndVertical();
      auto contentRect = GUI_ImGui_GetItemRect();

      ImGui::Spring(1, padding);
      ImGui::EndHorizontal();

      ImGui::BeginHorizontal("outputs");
      ImGui::Spring(0, padding * 2);

      ImRect outputsRect;
      int outputAlpha = 200;
      if (!node.Outputs.empty())
      {
        auto& pin = node.Outputs[0];
        ImGui::Dummy(ImVec2(0, padding));
        ImGui::Spring(1, 0);
        outputsRect = GUI_ImGui_GetItemRect();

#if IMGUI_VERSION_NUM > 18101
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);
#else
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinCorners, 3);
#endif
        ax::NodeEditor::BeginPin(pin.ID, ax::NodeEditor::PinKind::Output);
        ax::NodeEditor::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
        ax::NodeEditor::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
        ax::NodeEditor::EndPin();
        ax::NodeEditor::PopStyleVar();

        if (target.m_Comp.m_NewLinkPin && !NodeUtility_CanCreateLink(target.m_Comp.m_NewLinkPin, &pin) && &pin != target.m_Comp.m_NewLinkPin)
          outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
      }
      else
        ImGui::Dummy(ImVec2(0, padding));

      ImGui::Spring(0, padding * 2);
      ImGui::EndHorizontal();

      ImGui::EndVertical();

      ax::NodeEditor::EndNode();
      ax::NodeEditor::PopStyleVar(7);
      ax::NodeEditor::PopStyleColor(4);

      auto drawList = ax::NodeEditor::GetNodeBackgroundDrawList(node.ID);

      //const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
      //const auto unitSize    = 1.0f / fringeScale;

      //const auto ImDrawList_AddRect = [](ImDrawList* drawList, const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
      //{
      //    if ((col >> 24) == 0)
      //        return;
      //    drawList->PathRect(a, b, rounding, rounding_corners);
      //    drawList->PathStroke(col, true, thickness);
      //};

#if IMGUI_VERSION_NUM > 18101
      const auto    topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
      const auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;
#else
      const auto    topRoundCornersFlags = 1 | 2;
      const auto bottomRoundCornersFlags = 4 | 8;
#endif

      drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
      //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, bottomRoundCornersFlags);
      //ImGui::PopStyleVar();
      drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
      //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
        IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, topRoundCornersFlags);
      //ImGui::PopStyleVar();
      drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
      //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      drawList->AddRect(
        contentRect.GetTL(),
        contentRect.GetBR(),
        IM_COL32(48, 128, 255, 100), 0.0f);
      //ImGui::PopStyleVar();
    }

    for (auto& node : target.m_Comp.m_Nodes)
    {
      if (node.Type != NodeType::Houdini)
        continue;

      const float rounding = 10.0f;
      const float padding = 12.0f;


      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImColor(229, 229, 229, 200));
      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, ImColor(125, 125, 125, 200));
      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_PinRect, ImColor(229, 229, 229, 60));
      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_PinRectBorder, ImColor(125, 125, 125, 60));

      const auto pinBackground = ax::NodeEditor::GetStyle().Colors[ax::NodeEditor::StyleColor_NodeBg];

      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodeRounding, rounding);
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 0.0f);
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinBorderWidth, 1.0f);
      ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinRadius, 6.0f);
      ax::NodeEditor::BeginNode(node.ID);

      ImGui::BeginVertical(node.ID.AsPointer());
      if (!node.Inputs.empty())
      {
        ImGui::BeginHorizontal("inputs");
        ImGui::Spring(1, 0);

        ImRect inputsRect;
        int inputAlpha = 200;
        for (auto& pin : node.Inputs)
        {
          ImGui::Dummy(ImVec2(padding, padding));
          inputsRect = GUI_ImGui_GetItemRect();
          ImGui::Spring(1, 0);
          inputsRect.Min.y -= padding;
          inputsRect.Max.y -= padding;

#if IMGUI_VERSION_NUM > 18101
          const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
#else
          const auto allRoundCornersFlags = 15;
#endif
          //ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowSize, 10.0f);
          //ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowWidth, 10.0f);
          ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinCorners, allRoundCornersFlags);

          ax::NodeEditor::BeginPin(pin.ID, ax::NodeEditor::PinKind::Input);
          ax::NodeEditor::PinPivotRect(inputsRect.GetCenter(), inputsRect.GetCenter());
          ax::NodeEditor::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
          ax::NodeEditor::EndPin();
          //ax::NodeEditor::PopStyleVar(3);
          ax::NodeEditor::PopStyleVar(1);

          auto drawList = ImGui::GetWindowDrawList();
          drawList->AddRectFilled(inputsRect.GetTL(), inputsRect.GetBR(),
            IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, allRoundCornersFlags);
          drawList->AddRect(inputsRect.GetTL(), inputsRect.GetBR(),
            IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, allRoundCornersFlags);

          if (target.m_Comp.m_NewLinkPin && !NodeUtility_CanCreateLink(target.m_Comp.m_NewLinkPin, &pin) && &pin != target.m_Comp.m_NewLinkPin)
            inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }

        //ImGui::Spring(1, 0);
        ImGui::EndHorizontal();
      }

      ImGui::BeginHorizontal("content_frame");
      ImGui::Spring(1, padding);

      ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
      ImGui::Dummy(ImVec2(160, 0));
      ImGui::Spring(1);
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
      ImGui::TextUnformatted(node.Name.c_str());
      ImGui::PopStyleColor();
      ImGui::Spring(1);
      ImGui::EndVertical();
      auto contentRect = GUI_ImGui_GetItemRect();

      ImGui::Spring(1, padding);
      ImGui::EndHorizontal();

      if (!node.Outputs.empty())
      {
        ImGui::BeginHorizontal("outputs");
        ImGui::Spring(1, 0);

        ImRect outputsRect;
        int outputAlpha = 200;
        for (auto& pin : node.Outputs)
        {
          ImGui::Dummy(ImVec2(padding, padding));
          outputsRect = GUI_ImGui_GetItemRect();
          ImGui::Spring(1, 0);
          outputsRect.Min.y += padding;
          outputsRect.Max.y += padding;

#if IMGUI_VERSION_NUM > 18101
          const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
          const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
#else
          const auto allRoundCornersFlags = 15;
          const auto topRoundCornersFlags = 3;
#endif

          ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinCorners, topRoundCornersFlags);
          ax::NodeEditor::BeginPin(pin.ID, ax::NodeEditor::PinKind::Output);
          ax::NodeEditor::PinPivotRect(outputsRect.GetCenter(), outputsRect.GetCenter());
          ax::NodeEditor::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
          ax::NodeEditor::EndPin();
          ax::NodeEditor::PopStyleVar();


          auto drawList = ImGui::GetWindowDrawList();
          drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR(),
            IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, allRoundCornersFlags);
          drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR(),
            IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, allRoundCornersFlags);


          if (target.m_Comp.m_NewLinkPin && !NodeUtility_CanCreateLink(target.m_Comp.m_NewLinkPin, &pin) && &pin != target.m_Comp.m_NewLinkPin)
            outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }

        ImGui::EndHorizontal();
      }

      ImGui::EndVertical();

      ax::NodeEditor::EndNode();
      ax::NodeEditor::PopStyleVar(7);
      ax::NodeEditor::PopStyleColor(4);

      // auto drawList = ax::NodeEditor::GetNodeBackgroundDrawList(node.ID);

      //const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
      //const auto unitSize    = 1.0f / fringeScale;

      //const auto ImDrawList_AddRect = [](ImDrawList* drawList, const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
      //{
      //    if ((col >> 24) == 0)
      //        return;
      //    drawList->PathRect(a, b, rounding, rounding_corners);
      //    drawList->PathStroke(col, true, thickness);
      //};

      //drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
      //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
      //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      //drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
      //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
      //ImGui::PopStyleVar();
      //drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
      //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
      ////ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      //drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
      //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
      ////ImGui::PopStyleVar();
      //drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
      //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      //drawList->AddRect(
      //    contentRect.GetTL(),
      //    contentRect.GetBR(),
      //    IM_COL32(48, 128, 255, 100), 0.0f);
      //ImGui::PopStyleVar();
    }

    for (auto& node : target.m_Comp.m_Nodes)
    {
      if (node.Type != NodeType::Comment)
        continue;

      const float commentAlpha = 0.75f;

      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
      ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
      ax::NodeEditor::BeginNode(node.ID);
      ImGui::PushID(node.ID.AsPointer());
      ImGui::BeginVertical("content");
      ImGui::BeginHorizontal("horizontal");
      ImGui::Spring(1);
      ImGui::TextUnformatted(node.Name.c_str());
      ImGui::Spring(1);
      ImGui::EndHorizontal();
      ax::NodeEditor::Group(node.Size);
      ImGui::EndVertical();
      ImGui::PopID();
      ax::NodeEditor::EndNode();
      ax::NodeEditor::PopStyleColor(2);
      ImGui::PopStyleVar();

      if (ax::NodeEditor::BeginGroupHint(node.ID))
      {
        //auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
        auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

        //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

        auto min = ax::NodeEditor::GetGroupMin();
        //auto max = ax::NodeEditor::GetGroupMax();

        ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
        ImGui::BeginGroup();
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::EndGroup();

        auto drawList = ax::NodeEditor::GetHintBackgroundDrawList();

        auto hintBounds = GUI_ImGui_GetItemRect();
        auto hintFrameBounds = GUI_ImRect_Expanded(hintBounds, 8, 4);

        drawList->AddRectFilled(
          hintFrameBounds.GetTL(),
          hintFrameBounds.GetBR(),
          IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

        drawList->AddRect(
          hintFrameBounds.GetTL(),
          hintFrameBounds.GetBR(),
          IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

        //ImGui::PopStyleVar();
      }
      ax::NodeEditor::EndGroupHint();
    }

    for (auto& link : target.m_Comp.m_Links)
      ax::NodeEditor::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

    if (!target.m_Comp.m_CreateNewNode)
    {
      if (ax::NodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f))
      {
        auto showLabel = [](const char* label, ImColor color)
          {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
            auto size = ImGui::CalcTextSize(label);

            auto padding = ImGui::GetStyle().FramePadding;
            auto spacing = ImGui::GetStyle().ItemSpacing;

            ImVec2 cursor = ImGui::GetCursorPos();
            ImVec2 scursor = ImGui::GetCursorScreenPos();
            ImGui::SetCursorPos(ImVec2(cursor.x + spacing.x, cursor.y + -spacing.y));

            auto rectMin = ImVec2(scursor.x - padding.x, scursor.y - padding.y);
            auto rectMax = ImVec2(scursor.x + size.x + padding.x, scursor.y + size.y + padding.y);

            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
            ImGui::TextUnformatted(label);
          };

        ax::NodeEditor::PinId startPinId = 0, endPinId = 0;
        if (ax::NodeEditor::QueryNewLink(&startPinId, &endPinId))
        {
          auto startPin = NodeUtility_FindPin(startPinId);
          auto endPin = NodeUtility_FindPin(endPinId);

          target.m_Comp.m_NewLinkPin = startPin ? startPin : endPin;

          if (startPin->Kind == PinKind::Input)
          {
            std::swap(startPin, endPin);
            std::swap(startPinId, endPinId);
          }

          if (startPin && endPin)
          {
            if (endPin == startPin)
            {
              ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            }
            else if (endPin->Kind == startPin->Kind)
            {
              showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
              ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            }
            //else if (endPin->Node == startPin->Node)
            //{
            //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
            //    ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
            //}
            else if (endPin->Type != startPin->Type)
            {
              showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
              ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
            }
            else
            {
              showLabel("+ Create Link", ImColor(32, 45, 32, 180));
              if (ax::NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
              {
                target.m_Comp.m_Links.emplace_back(Link(NodeUtility_GetNextId(), startPinId, endPinId));
                target.m_Comp.m_Links.back().Color = Aktion::Database::GetIconColor(startPin->Type);
              }
            }
          }
        }

        ax::NodeEditor::PinId pinId = 0;
        if (ax::NodeEditor::QueryNewNode(&pinId))
        {
          target.m_Comp.m_NewLinkPin = NodeUtility_FindPin(pinId);
          if (target.m_Comp.m_NewLinkPin)
            showLabel("+ Create Node", ImColor(32, 45, 32, 180));

          if (ax::NodeEditor::AcceptNewItem())
          {
            target.m_Comp.m_CreateNewNode = true;
            target.m_Comp.m_NewNodeLinkPin = NodeUtility_FindPin(pinId);
            target.m_Comp.m_NewLinkPin = nullptr;
            ax::NodeEditor::Suspend();
            ImGui::OpenPopup("Create New Node");
            ax::NodeEditor::Resume();
          }
        }
      }
      else
        target.m_Comp.m_NewLinkPin = nullptr;

      ax::NodeEditor::EndCreate();

      if (ax::NodeEditor::BeginDelete())
      {
        ax::NodeEditor::NodeId nodeId = 0;
        while (ax::NodeEditor::QueryDeletedNode(&nodeId))
        {
          if (ax::NodeEditor::AcceptDeletedItem())
          {
            auto id = std::find_if(target.m_Comp.m_Nodes.begin(), target.m_Comp.m_Nodes.end(), [nodeId](auto& node) { return node.ID == nodeId; });
            if (id != target.m_Comp.m_Nodes.end())
              target.m_Comp.m_Nodes.erase(id);
          }
        }

        ax::NodeEditor::LinkId linkId = 0;
        while (ax::NodeEditor::QueryDeletedLink(&linkId))
        {
          if (ax::NodeEditor::AcceptDeletedItem())
          {
            auto id = std::find_if(target.m_Comp.m_Links.begin(), target.m_Comp.m_Links.end(), [linkId](auto& link) { return link.ID == linkId; });
            if (id != target.m_Comp.m_Links.end())
              target.m_Comp.m_Links.erase(id);
          }
        }
      }
      ax::NodeEditor::EndDelete();
    }

    ImGui::SetCursorScreenPos(cursorTopLeft);
  }

  ax::NodeEditor::Suspend();
  if (ax::NodeEditor::ShowBackgroundContextMenu()) {
    target.m_Comp.m_ShowAddNode = true;
    target.m_Comp.m_CursorLastPos = ImGui::GetMousePos();
  }
  ax::NodeEditor::Resume();

  ax::NodeEditor::End();
}