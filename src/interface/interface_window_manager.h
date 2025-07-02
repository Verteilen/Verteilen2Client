#pragma once
#include <cinttypes>

namespace Aktion::System {
	typedef class AktionWindow;
	typedef void (*CreateWindow)(AktionWindow* win);
	typedef void (*DestroyWindow)(AktionWindow* win);
	typedef size_t(*WindowCount)(AktionWindow* win);
	typedef AktionWindow* (*GetWindowByIndex)(int32_t index);

	/// <summary>
	/// The display management interface
	/// </summary>
	struct InterfaceDisplayManager {
	public:
		CreateWindow m_CreateWindow;
		DestroyWindow m_DestroyWindow;
		WindowCount m_WindowCount;
		GetWindowByIndex m_GetWindowByIndex;
	};
}