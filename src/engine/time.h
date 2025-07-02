#pragma once
#include <cinttypes>
#include <chrono>

namespace Aktion::Engine 
{
	struct AktionTime 
	{
		uint32_t m_Frame;
		std::chrono::steady_clock m_Start;
		std::chrono::time_point<std::chrono::steady_clock> m_StartPoint;
		std::chrono::time_point<std::chrono::steady_clock> m_DeltaPoint;
		double m_Time;
		double m_Delta;
	};
}