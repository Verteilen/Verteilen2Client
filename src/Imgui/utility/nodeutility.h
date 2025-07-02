#pragma once
#include <system/config.h>
//
// Make imgui imvec can be use math operaetors
//
#include <vector>
//
// Imgui and plugin
//
#include <imgui.h>
#include <imgui_internal.h>
//
// Imgui and plugin
//
#include <system/gui/logic.h>
#include <system/gui/comp.h>
#include <system/gui/setting.h>

namespace Aktion::GUI
{
  static NodeEditorBuffer* m_CurrentEditorBuffer = nullptr;

  void NodeUtility_BuildNode(Node* node);
  void NodeUtility_BuildNodes();
  int NodeUtility_GetNextId();
  ax::NodeEditor::LinkId NodeUtility_GetNextLinkId();
  void NodeUtility_TouchNode(ax::NodeEditor::NodeId id);
  float NodeUtility_GetTouchProgress(ax::NodeEditor::NodeId id);
  Node* NodeUtility_FindNode(ax::NodeEditor::NodeId id);

  Link* NodeUtility_FindLink(ax::NodeEditor::LinkId id);
  Pin* NodeUtility_FindPin(ax::NodeEditor::PinId id);
  bool NodeUtility_IsPinLinked(ax::NodeEditor::PinId id);
  bool NodeUtility_CanCreateLink(Pin* a, Pin* b);
  void NodeUtility_SetBuffer(NodeEditorBuffer& target);

  void GUI_DrawPinIcon(const Pin& pin, bool connected, int alpha);
  ImRect GUI_ImGui_GetItemRect();
  ImRect GUI_ImRect_Expanded(const ImRect& rect, float x, float y);
 
  void ExecuteLogicNode();
}