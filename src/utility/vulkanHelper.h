#pragma once
#include <system/config.h>
//
//  Imgui includes
//
#include <imgui.h>
//
//  Vulkan graphics includes
//
#include <vulkan/vulkan.hpp>

static void check_vk_result(VkResult err)
{
  if (err == 0)
    return;
  AKTION_LOG_ERROR("[vulkan] Error: VkResult = %d", err);
  if (err < 0)
    abort();
}
static bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension)
{
  for (const VkExtensionProperties& p : properties)
    if (strcmp(p.extensionName, extension) == 0)
      return true;
  return false;
}
static VkPhysicalDevice SetupVulkan_SelectPhysicalDevice(VkInstance g_Instance)
{
  uint32_t gpu_count;
  VkResult err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);
  AKTION_LOG_DEBUG("Detect GPU device count: %i", gpu_count);
  check_vk_result(err);
  IM_ASSERT(gpu_count > 0);

  ImVector<VkPhysicalDevice> gpus;
  gpus.resize(gpu_count);
  err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.Data);
  check_vk_result(err);

  // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
  // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
  // dedicated GPUs) is out of scope of this sample.
  VkPhysicalDevice result = VK_NULL_HANDLE;
  for (VkPhysicalDevice& device : gpus)
  {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
    {
      AKTION_LOG_INFO("Pick device: %s", properties.deviceName);
      result = device;
    }
    AKTION_LOG_DEBUG("\t Device name: %s", properties.deviceName);
    AKTION_LOG_DEBUG("\t Device ID: %u", properties.deviceID);
    AKTION_LOG_DEBUG("\t Device type: %i", properties.deviceType);
    AKTION_LOG_DEBUG("\t Driver version: %u", properties.driverVersion);
    AKTION_LOG_DEBUG("\t API version: %u", properties.apiVersion);
    AKTION_LOG_DEBUG("\t Vendor ID: %u", properties.vendorID);
  }
  if (result != VK_NULL_HANDLE) {
    return result;
  }
  

  // Use first GPU (Integrated) is a Discrete one is not available.
  if (gpu_count > 0)
  {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(gpus[0], &properties);
    AKTION_LOG_WARN("\t Detect no GPU is available, select the first one instead: %s", properties.deviceName);
    return gpus[0];
  }
  AKTION_LOG_FETAL("There is no device available...");
  return VK_NULL_HANDLE;
}