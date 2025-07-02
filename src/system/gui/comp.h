#pragma once
#include <system/config.h>
#include <system/gui/logic.h>

namespace Aktion::GUI 
{
	void GUI_Comp_PopupAddNode(Aktion::GUI::AktionGUI& host, NodeEditorBuffer& target);
	void GUI_Comp_InitNodeEditor(NodeEditorBuffer& target);
	void GUI_Comp_DrawNodeEditor(AktionGUI& target);
}