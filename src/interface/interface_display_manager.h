#pragma once
#include <cinttypes>

namespace Aktion::System {
	typedef class AktionDisplay;
	typedef AktionDisplay* (*CreateDisplay)();
	typedef bool (*DestroyDisplay)(AktionDisplay* display);
	typedef AktionDisplay* (*GetDisplay)(uint32_t index);
	typedef int32_t(*DisplayCount)();

	/// <summary>
	/// The display management interface
	/// </summary>
	struct InterfaceDisplayManager {
		CreateDisplay m_CreateDisplay;
		DestroyDisplay m_DestroyDisplay;
		GetDisplay m_GetDisplay;
		DisplayCount m_DisplayCount;
	};
}