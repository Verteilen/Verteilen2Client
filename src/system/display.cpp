#include "display.h"
#include <system/config.h>
#include <utility/vulkanHelper.h>

bool CreateWin(Aktion::System::AktionDisplay& display)
{
  // Setup SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) != 0)
  {
    printf("Error: SDL_Init(): %s\n", SDL_GetError());
    return false;
  }
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  display.m_SDLWin = SDL_CreateWindow("ActionDesigner", 1280, 720, window_flags);
  SDL_Log("Finished create SDL display");
  if (display.m_SDLWin == nullptr)
  {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return false;
  }
  return true;
}
bool GetVKExtension(Aktion::System::AktionDisplay& display)
{
  ImVector<const char*> instance_extensions;
  {
    uint32_t sdl_extensions_count = 0;
    const char* const* sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_extensions_count);
    for (uint32_t n = 0; n < sdl_extensions_count; n++)
      instance_extensions.push_back(sdl_extensions[n]);
  }
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
  volkInitialize();
#endif

  // Create Vulkan Instance
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

  // Enumerate available extensions
  uint32_t properties_count;
  ImVector<VkExtensionProperties> properties;
  vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
  properties.resize(properties_count);
  display.m_VKError = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.Data);
  check_vk_result(display.m_VKError);

  // Enable required extensions
  if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
  if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
  {
    instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  }
#endif

  // Enabling validation layers
#ifdef APP_USE_VULKAN_DEBUG_REPORT
  const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
  create_info.enabledLayerCount = 1;
  create_info.ppEnabledLayerNames = layers;
  instance_extensions.push_back("VK_EXT_debug_report");
#endif

  // Create Vulkan Instance
  create_info.enabledExtensionCount = (uint32_t)instance_extensions.Size;
  create_info.ppEnabledExtensionNames = instance_extensions.Data;
  display.m_VKError = vkCreateInstance(&create_info, display.m_VKAllocator, &display.m_VKInst);
  check_vk_result(display.m_VKError);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
  volkLoadInstance(display.vkInst);
#endif
  return true;
}

bool GetVKPhysicsDevices(Aktion::System::AktionDisplay& display)
{
  display.m_VKPhysicalDevice = SetupVulkan_SelectPhysicalDevice(display.m_VKInst);

  uint32_t count;
  vkGetPhysicalDeviceQueueFamilyProperties(display.m_VKPhysicalDevice, &count, nullptr);
  VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
  vkGetPhysicalDeviceQueueFamilyProperties(display.m_VKPhysicalDevice, &count, queues);
  for (uint32_t i = 0; i < count; i++)
    if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      display.m_VKGraphicsQueueIndex = i;
      break;
    }
  free(queues);
  IM_ASSERT(display.m_VKGraphicsQueueIndex != (uint32_t)-1);
  return true;
}
bool DeviceSetup(Aktion::System::AktionDisplay& display)
{
  ImVector<const char*> device_extensions;
  device_extensions.push_back("VK_KHR_swapchain");

  // Enumerate physical device extension
  uint32_t properties_count;
  ImVector<VkExtensionProperties> properties;
  vkEnumerateDeviceExtensionProperties(display.m_VKPhysicalDevice, nullptr, &properties_count, nullptr);
  properties.resize(properties_count);
  vkEnumerateDeviceExtensionProperties(display.m_VKPhysicalDevice, nullptr, &properties_count, properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
  if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
    device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

  const float queue_priority[] = { 1.0f };
  VkDeviceQueueCreateInfo queue_info[1] = {};
  queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info[0].queueFamilyIndex = display.m_VKGraphicsQueueIndex;
  queue_info[0].queueCount = 1;
  queue_info[0].pQueuePriorities = queue_priority;
  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
  create_info.pQueueCreateInfos = queue_info;
  create_info.enabledExtensionCount = (uint32_t)device_extensions.Size;
  create_info.ppEnabledExtensionNames = device_extensions.Data;
  display.m_VKError = vkCreateDevice(display.m_VKPhysicalDevice, &create_info, display.m_VKAllocator, &display.m_VKDevice);
  check_vk_result(display.m_VKError);
  vkGetDeviceQueue(display.m_VKDevice, display.m_VKGraphicsQueueIndex, 0, &display.m_VKGraphicsQueue);
  return true;
}
bool DescriptorPool(Aktion::System::AktionDisplay& display)
{
  VkDescriptorPoolSize pool_sizes[] =
  {
      { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
  };
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1;
  pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;
  display.m_VKError = vkCreateDescriptorPool(display.m_VKDevice, &pool_info, display.m_VKAllocator, &display.m_VKDescriptor);
  check_vk_result(display.m_VKError);
  return true;
}
bool CreateVKSurface(Aktion::System::AktionDisplay& display)
{
  if (SDL_Vulkan_CreateSurface(display.m_SDLWin, display.m_VKInst, display.m_VKAllocator, &display.m_VKSurface) == 0)
  {
    printf("Failed to create Vulkan surface.\n");
    return false;
  }
  return true;
}
bool Shutdown(Aktion::System::AktionDisplay& display)
{
  vkDestroyDevice(display.m_VKDevice, nullptr);
  vkDestroyInstance(display.m_VKInst, nullptr);
  SDL_DestroyWindow(display.m_SDLWin);
  SDL_Vulkan_UnloadLibrary();
  SDL_Quit();
  return true;
}

void Aktion::System::Display_Destroy(AktionDisplay& display)
{
  Shutdown(display);
}
void Aktion::System::Display_Init(AktionDisplay& display)
{
  SDL_Log("Start running display init");
  CreateWin(display);
  GetVKExtension(display);
  GetVKPhysicsDevices(display);
  DeviceSetup(display);
  DescriptorPool(display);
  CreateVKSurface(display);
}
bool Aktion::System::Display_IsRunning(AktionDisplay& display)
{
	return display.m_Running;
}
void Aktion::System::Display_MainLoop(AktionDisplay& display)
{
}
