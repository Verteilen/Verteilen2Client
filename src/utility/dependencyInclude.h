#pragma once
//
//  C++ header includes
//
#include <iostream>
#include <vector>
//
//  Windows handler includes
//
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
//
//  Vulkan graphics includes
//
#include <vulkan/vulkan.hpp>
//
//  Volk headers
//
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif