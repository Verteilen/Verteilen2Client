#pragma once
#include <system/config.h>
#include <map>
#include <vector>
#include <functional>
//
// Imgui and plugin
//
#include <Imgui/imgui_node_data.h>

namespace Aktion::GUI 
{
	typedef struct AktionGUI;

  struct NodeAddType
  {
    std::string m_Name;
    std::string m_Description;
    std::function<Node* (void)> m_Call;
  };

  struct NodeAddCategory
  {
    std::string m_Title;
    std::string m_Description;
    std::vector<NodeAddType> m_List;
  };

	struct NodeEditorBuffer {
    ax::NodeEditor::EditorContext* m_EditorContext;
    int m_NextId = 1;
    ax::NodeEditor::NodeId m_ContextNodeId = 0;
    ax::NodeEditor::LinkId m_ContextLinkId = 0;
    ax::NodeEditor::PinId  m_ContextPinId = 0;
    std::vector<Node> m_Nodes;
    std::vector<Link> m_Links;
    ImFont* m_DefaultFont = nullptr;
    ImFont* m_HeaderFont = nullptr;
    ImTextureID m_HeaderBackground = NULL;
    ImTextureID m_SaveIcon = NULL;
    ImTextureID m_RestoreIcon = NULL;
    const float m_TouchTime = 1.0f;
    std::map<ax::NodeEditor::NodeId, float, NodeIdLess> m_NodeTouchTime;
    bool m_ShowOrdinals = false;

    bool m_CreateNewNode;
    ImVec2 m_CursorTopLeft;
    ImVec2 m_CursorLastPos;
    Pin* m_NewNodeLinkPin;
    Pin* m_NewLinkPin;

    bool m_ShowAddNode;
    bool m_UpdateNodeList;
  
    std::vector<NodeAddCategory> m_NodeAddBuffer = std::vector<NodeAddCategory>();
	};

  void GUI_Logic_PopupAddNode(Aktion::GUI::AktionGUI& host, NodeEditorBuffer& target);
  void GUI_Logic_InitLogicEditor(NodeEditorBuffer& target);
	void GUI_Logic_DrawLogicEditor(AktionGUI& target);
}