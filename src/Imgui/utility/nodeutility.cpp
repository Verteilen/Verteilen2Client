#include "nodeutility.h"
//
// Imgui and plugin
//
#include <imgui_node_editor.h>
#include <Imgui/node/node_database.h>
#include <Imgui/imgui_node_data.h>
#include <utilities/builders.h>
#include <utilities/widgets.h>
#include <imgui_internal.h>
//
// Lua Helper
//
#ifdef __cplusplus
extern "C" {

}
#endif

void Aktion::GUI::NodeUtility_BuildNode(Node* node)
{
  for (auto& input : node->Inputs)
  {
    input.Node = node;
    input.Kind = PinKind::Input;
  }

  for (auto& output : node->Outputs)
  {
    output.Node = node;
    output.Kind = PinKind::Output;
  }
}
void Aktion::GUI::NodeUtility_BuildNodes()
{
  for (auto& node : m_CurrentEditorBuffer->m_Nodes)
    NodeUtility_BuildNode(&node);
}
int Aktion::GUI::NodeUtility_GetNextId()
{
  return m_CurrentEditorBuffer->m_NextId++;
}
ax::NodeEditor::LinkId Aktion::GUI::NodeUtility_GetNextLinkId()
{
  return ax::NodeEditor::LinkId(NodeUtility_GetNextId());
}
void Aktion::GUI::NodeUtility_TouchNode(ax::NodeEditor::NodeId id)
{
  m_CurrentEditorBuffer->m_NodeTouchTime[id] = m_CurrentEditorBuffer->m_TouchTime;
}
float Aktion::GUI::NodeUtility_GetTouchProgress(ax::NodeEditor::NodeId id)
{
  auto it = m_CurrentEditorBuffer->m_NodeTouchTime.find(id);
  if (it != m_CurrentEditorBuffer->m_NodeTouchTime.end() && it->second > 0.0f)
    return (m_CurrentEditorBuffer->m_TouchTime - it->second) / m_CurrentEditorBuffer->m_TouchTime;
  else
    return 0.0f;
}
Node* Aktion::GUI::NodeUtility_FindNode(ax::NodeEditor::NodeId id)
{
  for (auto& node : m_CurrentEditorBuffer->m_Nodes)
    if (node.ID == id)
      return &node;

  return nullptr;
}

Link* Aktion::GUI::NodeUtility_FindLink(ax::NodeEditor::LinkId id)
{
  for (auto& link : m_CurrentEditorBuffer->m_Links)
    if (link.ID == id)
      return &link;

  return nullptr;
}
Pin* Aktion::GUI::NodeUtility_FindPin(ax::NodeEditor::PinId id)
{
  if (!id)
    return nullptr;

  for (auto& node : m_CurrentEditorBuffer->m_Nodes)
  {
    for (auto& pin : node.Inputs)
      if (pin.ID == id)
        return &pin;

    for (auto& pin : node.Outputs)
      if (pin.ID == id)
        return &pin;
  }

  return nullptr;
}
bool Aktion::GUI::NodeUtility_IsPinLinked(ax::NodeEditor::PinId id)
{
  if (!id)
    return false;

  for (auto& link : m_CurrentEditorBuffer->m_Links)
    if (link.StartPinID == id || link.EndPinID == id)
      return true;

  return false;
}
bool Aktion::GUI::NodeUtility_CanCreateLink(Pin* a, Pin* b)
{
  if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
    return false;

  return true;
}
void Aktion::GUI::NodeUtility_SetBuffer(NodeEditorBuffer& target)
{
  m_CurrentEditorBuffer = &target;
}

void Aktion::GUI::GUI_DrawPinIcon(const Pin& pin, bool connected, int alpha)
{
  ax::Drawing::IconType iconType;
  ImColor  color = Aktion::Database::GetIconColor(pin.Type);
  color.Value.w = alpha / 255.0f;
  switch (pin.Type)
  {
  case PinType::Flow:     iconType = ax::Drawing::IconType::Flow;   break;
  case PinType::Bool:     iconType = ax::Drawing::IconType::Circle; break;
  case PinType::Int:      iconType = ax::Drawing::IconType::Circle; break;
  case PinType::Float:    iconType = ax::Drawing::IconType::Circle; break;
  case PinType::String:   iconType = ax::Drawing::IconType::Circle; break;
  case PinType::Object:   iconType = ax::Drawing::IconType::Circle; break;
  case PinType::Function: iconType = ax::Drawing::IconType::Circle; break;
  case PinType::Delegate: iconType = ax::Drawing::IconType::Square; break;
  default:
    return;
  }

  ax::Widgets::Icon(ImVec2(static_cast<float>(PIN_ICON_SIZE), static_cast<float>(PIN_ICON_SIZE)), iconType, connected, color, ImColor(32, 32, 32, alpha));
};
ImRect Aktion::GUI::GUI_ImGui_GetItemRect()
{
  return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}
ImRect Aktion::GUI::GUI_ImRect_Expanded(const ImRect& rect, float x, float y)
{
  auto result = rect;
  result.Min.x -= x;
  result.Min.y -= y;
  result.Max.x += x;
  result.Max.y += y;
  return result;
}


void Aktion::GUI::ExecuteLogicNode()
{
  std::vector<Node*> EventTypeNodes = std::vector<Node*>();
  for (auto i = m_CurrentEditorBuffer->m_Nodes.begin(); i < m_CurrentEditorBuffer->m_Nodes.end(); i++) 
  {
    if ((*i).Meta == "Event") {
      EventTypeNodes.push_back(i.base());
    }
  }


}