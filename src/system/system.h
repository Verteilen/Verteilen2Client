#pragma once
#include <vector>
#include <system/config.h>
#include <system/win.h>
#include <system/display.h>
#include <engine/engine.h>
#include <interface/interface_display_manager.h>

namespace Aktion::System {
	/// <summary>
	/// The main manage system on top of everything. this handles the window creation
	/// </summary>
	struct AktionSystem {
		AktionWindow m_Win;
		std::vector<AktionDisplay> m_Displays;
		Engine::Engine m_Engine;
	};

	void ApplyArgument(AktionSystem& target, int argc, char* argv[]);
	void InitMainSystem(AktionSystem& target);
	void RunMainSystem(AktionSystem& target);
	InterfaceDisplayManager GetDisplayManager();
}