#include "system.h"
#include <system/display.h>

Aktion::System::AktionSystem* current_system = nullptr;

Aktion::System::AktionDisplay* Call_CreateDisplay()
{
	current_system->m_Displays.push_back(Aktion::System::AktionDisplay());
	Aktion::System::AktionDisplay* target = &current_system->m_Displays.back();
	Display_Init(*target);
	return target;
}
bool Call_DestroyDisplay(Aktion::System::AktionDisplay* display)
{
	for (int i = 0; i < current_system->m_Displays.size(); i++) 
	{
		if (&current_system->m_Displays[i] == display) {
			Display_Destroy(current_system->m_Displays[i]);
			current_system->m_Displays.erase(current_system->m_Displays.begin() + i);
			return true;
		}
	}
	return false;
}
int32_t Call_DisplayCount()
{
	return current_system->m_Displays.size();
}
Aktion::System::AktionDisplay* Call_GetDisplay(uint32_t index) 
{
	uint32_t size = Call_DisplayCount();
	if (index >= 0 && index < size)
		return &current_system->m_Displays[index];
	else
		return nullptr;
}

void Aktion::System::ApplyArgument(AktionSystem& target, int argc, char* argv[])
{
	if (argc % 2) argc--;
	if (argc <= 0) 
	{
		AKTION_LOG_DEBUG("No arguments has been received");
		return;
	}
	AKTION_LOG_DEBUG("Apply argument count: %i", argc);

	for (int i = 0; i < argc; i += 2) 
	{
		AKTION_LOG_DEBUG("Append argument: [%s, %s]", argv[i], argv[i + i]);
		target.m_Engine.m_Parameters.insert({
			argv[i],
			argv[i + 1],
		});
	}
}
void Aktion::System::InitMainSystem(AktionSystem& target)
{
	AKTION_LOG_INFO("Main system initialization start...");
	current_system = &target;

	Engine::Engine_Init(target.m_Engine);
	Window_Init(target.m_Win);
	Window_RegisterDisplayManager(target.m_Win, GetDisplayManager());
	AKTION_LOG_INFO("Main system initialization end");
}
void Aktion::System::RunMainSystem(AktionSystem& target)
{
	while (Window_IsRunning(target.m_Win)) {
		// Engine Logic mainloop
		Aktion::Engine::Engine_MainLoop(target.m_Engine);
		// Editor window mainloop
		Window_MainLoop(target.m_Win, target);
		// Display window mainloop
		for (int i = 0; i < target.m_Displays.size(); i++) {
			Display_MainLoop(target.m_Displays[i]);
		}
	}
}
Aktion::System::InterfaceDisplayManager Aktion::System::GetDisplayManager()
{
	Aktion::System::InterfaceDisplayManager result = {
		Call_CreateDisplay,
		Call_DestroyDisplay,
		Call_GetDisplay,
		Call_DisplayCount
	};
	return result;
}