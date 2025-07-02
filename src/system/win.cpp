#include "win.h"
#include <fstream>
#include <system/config.h>
#include <utility/vulkanHelper.h>
#include <system/system.h>

uint32_t minImageCount = 2;
bool swapChainRebuild = false;
static SDL_Window* m_CurrentWindow = nullptr;

Aktion::GUI::AktionSetting localSettingBuffer = Aktion::GUI::AktionSetting();
Aktion::GUI::SetSetting localSettingLoadBuffer = nullptr;
const SDL_DialogFileFilter settingFilter[] = {
  { "Binary files","bin"},
  { "All files","*" }
};

void SetupVulkanWindow(Aktion::System::AktionWindow& win, ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
{
  wd->Surface = surface;

  // Check for WSI support
  VkBool32 res;
  vkGetPhysicalDeviceSurfaceSupportKHR(win.m_VKPhysicalDevice, win.m_VKGraphicsQueueIndex, wd->Surface, &res);
  if (res != VK_TRUE)
  {
    fprintf(stderr, "Error no WSI support on physical device 0\n");
    exit(-1);
  }

  // Select Surface Format
  const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
  const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(win.m_VKPhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

  // Select Present Mode
#ifdef APP_UNLIMITED_FRAME_RATE
  VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
  VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
  wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(win.m_VKPhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
  //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

  // Create SwapChain, RenderPass, Framebuffer, etc.
  IM_ASSERT(minImageCount >= 2);
  ImGui_ImplVulkanH_CreateOrResizeWindow(win.m_VKInst, win.m_VKPhysicalDevice, win.m_VKDevice, wd, win.m_VKGraphicsQueueIndex, win.m_VKAllocator, width, height, minImageCount);
}
#ifdef APP_USE_VULKAN_DEBUG_REPORT
VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
  (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
  fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
  return VK_FALSE;
}
#endif // APP_USE_VULKAN_DEBUG_REPORT

void SaveSettingCallback(void* userdata, const char* const* filelist, int filter) {
  if (!filelist) {
    SDL_Log("An error occured: %s", SDL_GetError());
    return;
  }
  else if (!*filelist) {
    SDL_Log("The user did not select any file.");
    SDL_Log("Most likely, the dialog was canceled.");
    return;
  }

  if (filelist != NULL) {
    const char* path = filelist[0];
    std::ofstream fout;
    fout.open(path, std::ios::binary | std::ios::out);
    fout.write((char*)&localSettingBuffer, sizeof(localSettingBuffer));
    fout.close();
  }
}
void LoadSettingCallback(void* userdata, const char* const* filelist, int filter) {
  if (!filelist) {
    SDL_Log("An error occured: %s", SDL_GetError());
    return;
  }
  else if (!*filelist) {
    SDL_Log("The user did not select any file.");
    SDL_Log("Most likely, the dialog was canceled.");
    return;
  }

  if (filelist != NULL) {
    Aktion::GUI::AktionSetting buffer = Aktion::GUI::AktionSetting();
    const char* path = filelist[0];
    std::ifstream fin;
    fin.open(path, std::ios::binary | std::ios::out);
    fin.read(reinterpret_cast<char*>(&buffer), sizeof(buffer));
    fin.close();
    localSettingLoadBuffer(buffer);
  }
}

uint32_t Call_GetMinImageCount() { return minImageCount; };
bool Call_GetSwapChainRebuild() { return swapChainRebuild; };
void Call_SetSwapChainRebuild(bool v) { swapChainRebuild = v; };
void Call_SaveSetting(Aktion::GUI::AktionSetting setting) {
  localSettingBuffer = setting;
  SDL_ShowSaveFileDialog(SaveSettingCallback, NULL, m_CurrentWindow, settingFilter, 2, NULL);
}
void Call_LoadSetting(Aktion::GUI::SetSetting setter) {
  localSettingLoadBuffer = setter;
  SDL_ShowOpenFileDialog(LoadSettingCallback, NULL, m_CurrentWindow, settingFilter, 2, NULL, false);
}

bool CreateWin(Aktion::System::AktionWindow& win)
{
  // Setup SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) != 0)
  {
    printf("Error: SDL_Init(): %s\n", SDL_GetError());
    return false;
  }
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (WIN_USE_FULLSCREEN == 1) 
  {
    AKTION_LOG_DEBUG("Detect that use maximized");
    window_flags |= SDL_WINDOW_MAXIMIZED;
  }
  else if (WIN_USE_FULLSCREEN == 2) 
  {
    AKTION_LOG_DEBUG("Detect that use fullscreen");
    window_flags |= SDL_WINDOW_FULLSCREEN;
  }
  win.m_SDLWin = SDL_CreateWindow(APPNAME, WIN_WIDTH, WIN_HEIGHT, window_flags);
  if (win.m_SDLWin == nullptr)
  {
    AKTION_LOG_FETAL("SDL_CreateWindow(): %s\n", SDL_GetError());
    return false;
  }
  AKTION_LOG_DEBUG("Create editor window w:%i, h:%i, fullscreen:%i ", WIN_WIDTH, WIN_HEIGHT, WIN_USE_FULLSCREEN);
  m_CurrentWindow = win.m_SDLWin;
  return true;
}
bool GetVKExtension(Aktion::System::AktionWindow& win)
{
  ImVector<const char*> instance_extensions;
  {
    uint32_t sdl_extensions_count = 0;
    const char* const* sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_extensions_count);
    for (uint32_t n = 0; n < sdl_extensions_count; n++) {
      AKTION_LOG_DEBUG("Extension detected: %s", sdl_extensions[n]);
      instance_extensions.push_back(sdl_extensions[n]);
    }
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
  win.m_VKError = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.Data);
  check_vk_result(win.m_VKError);

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
  AKTION_LOG_DEBUG("Use Debug layer: VK_EXT_debug_report");
#endif

  // Create Vulkan Instance
  create_info.enabledExtensionCount = (uint32_t)instance_extensions.Size;
  create_info.ppEnabledExtensionNames = instance_extensions.Data;
  win.m_VKError = vkCreateInstance(&create_info, win.m_VKAllocator, &win.m_VKInst);
  AKTION_LOG_DEBUG("Create vulkan instance !");
  check_vk_result(win.m_VKError);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
  volkLoadInstance(display.vkInst);
#endif
  return true;
}
bool DebugReport(Aktion::System::AktionWindow& win)
{
#ifdef APP_USE_VULKAN_DEBUG_REPORT
  auto f_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(win.m_VKInst, "vkCreateDebugReportCallbackEXT");
  IM_ASSERT(f_vkCreateDebugReportCallbackEXT != nullptr);
  VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
  debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
  debug_report_ci.pfnCallback = debug_report;
  debug_report_ci.pUserData = nullptr;
  AKTION_LOG_DEBUG("Added Debug report callback");
  VkResult err = f_vkCreateDebugReportCallbackEXT(win.m_VKInst, &debug_report_ci, win.m_VKAllocator, &win.m_VKDebugReport);
  check_vk_result(err);
#endif
  return true;
}
bool GetVKPhysicsDevices(Aktion::System::AktionWindow& win)
{
  win.m_VKPhysicalDevice = SetupVulkan_SelectPhysicalDevice(win.m_VKInst);
  if (win.m_VKPhysicalDevice == VK_NULL_HANDLE) 
  {
    return false;
  }

  uint32_t count;
  vkGetPhysicalDeviceQueueFamilyProperties(win.m_VKPhysicalDevice, &count, nullptr);
  VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
  vkGetPhysicalDeviceQueueFamilyProperties(win.m_VKPhysicalDevice, &count, queues);
  for (uint32_t i = 0; i < count; i++)
    if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      win.m_VKGraphicsQueueIndex = i;
      break;
    }
  free(queues);
  IM_ASSERT(win.m_VKGraphicsQueueIndex != (uint32_t)-1);
  return true;
}
bool DeviceSetup(Aktion::System::AktionWindow& win)
{
  AKTION_LOG_DEBUG("Create Device");
  ImVector<const char*> device_extensions;
  device_extensions.push_back("VK_KHR_swapchain");

  // Enumerate physical device extension
  uint32_t properties_count;
  ImVector<VkExtensionProperties> properties;
  vkEnumerateDeviceExtensionProperties(win.m_VKPhysicalDevice, nullptr, &properties_count, nullptr);
  properties.resize(properties_count);
  vkEnumerateDeviceExtensionProperties(win.m_VKPhysicalDevice, nullptr, &properties_count, properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
  if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
    device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

  const float queue_priority[] = { 1.0f };
  VkDeviceQueueCreateInfo queue_info[1] = {};
  queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info[0].queueFamilyIndex = win.m_VKGraphicsQueueIndex;
  queue_info[0].queueCount = 1;
  queue_info[0].pQueuePriorities = queue_priority;
  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
  create_info.pQueueCreateInfos = queue_info;
  create_info.enabledExtensionCount = (uint32_t)device_extensions.Size;
  create_info.ppEnabledExtensionNames = device_extensions.Data;
  win.m_VKError = vkCreateDevice(win.m_VKPhysicalDevice, &create_info, win.m_VKAllocator, &win.m_VKDevice);
  check_vk_result(win.m_VKError);
  vkGetDeviceQueue(win.m_VKDevice, win.m_VKGraphicsQueueIndex, 0, &win.m_VKGraphicsQueue);
  return true;
}
bool DescriptorPool(Aktion::System::AktionWindow& win)
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
  win.m_VKError = vkCreateDescriptorPool(win.m_VKDevice, &pool_info, win.m_VKAllocator, &win.m_VKDescriptor);
  check_vk_result(win.m_VKError);
  return true;
}
bool CreateVKSurface(Aktion::System::AktionWindow& win)
{
  if (SDL_Vulkan_CreateSurface(win.m_SDLWin, win.m_VKInst, win.m_VKAllocator, &win.m_VKSurface) == 0)
  {
    printf("Failed to create Vulkan surface.\n");
    return false;
  }
  return true;
}
bool GUISetup(Aktion::System::AktionWindow& win)
{
  AKTION_LOG_DEBUG("ImGui Setup");
  SDL_GetWindowSize(win.m_SDLWin, &win.m_Width, &win.m_Height);
  ImGui_ImplVulkanH_Window* wd = &win.m_MainWindowData;
  SetupVulkanWindow(win, wd, win.m_VKSurface, win.m_Width, win.m_Height);

  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = win.m_VKInst;
  init_info.PhysicalDevice = win.m_VKPhysicalDevice;
  init_info.Device = win.m_VKDevice;
  init_info.QueueFamily = win.m_VKGraphicsQueueIndex;
  init_info.Queue = win.m_VKGraphicsQueue;
  init_info.PipelineCache = win.m_VKPipeline;
  init_info.DescriptorPool = win.m_VKDescriptor;
  init_info.RenderPass = wd->RenderPass;
  init_info.Subpass = 0;
  init_info.MinImageCount = minImageCount;
  init_info.ImageCount = wd->ImageCount;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = win.m_VKAllocator;
  init_info.CheckVkResultFn = check_vk_result;

  AKTION_LOG_DEBUG("\t MSAASample: %i", init_info.MSAASamples);

  Aktion::System::WindowContext* ctx = new Aktion::System::WindowContext{
    Call_GetMinImageCount,
    Call_GetSwapChainRebuild,
    Call_SetSwapChainRebuild,
    Call_SaveSetting,
    Call_LoadSetting,
  };
  Aktion::GUI::GUI_Focus(win.m_GUI);
  Aktion::GUI::GUI_RegisterWindowContext(ctx);
  Aktion::GUI::GUI_Init(win.m_GUI, win.m_SDLWin, init_info, wd);
  return true;
}
bool Shutdown(Aktion::System::AktionWindow& win)
{
  vkDestroyDevice(win.m_VKDevice, nullptr);
  vkDestroyInstance(win.m_VKInst, nullptr);
  SDL_DestroyWindow(win.m_SDLWin);
  SDL_Vulkan_UnloadLibrary();
  SDL_Quit();
  return true;
}

void Aktion::System::DestroyWindow(Aktion::System::AktionWindow& target)
{
  ImGui_ImplVulkanH_DestroyWindow(target.m_VKInst, target.m_VKDevice, &target.m_MainWindowData, target.m_VKAllocator);
  Shutdown(target);
}
void Aktion::System::Window_Init(Aktion::System::AktionWindow& target) 
{
  AKTION_LOG_INFO("Win initialization start...");
  bool process;
  process = CreateWin(target);
  if (process) GetVKExtension(target);
  if (process) DebugReport(target);
  if (process) GetVKPhysicsDevices(target);
  if (process) DeviceSetup(target);
  if (process) DescriptorPool(target);
  if (process) CreateVKSurface(target);
  if (process) GUISetup(target);
  AKTION_LOG_INFO("Win initialization end");

  if (!process)
  {
    AKTION_LOG_FETAL("Win initialization failed");
  }
}
void Aktion::System::Window_RegisterDisplayManager(Aktion::System::AktionWindow& target, Aktion::System::InterfaceDisplayManager manager)
{
  target.m_DisplayManager = manager;
}
bool Aktion::System::Window_IsRunning(Aktion::System::AktionWindow& target) 
{
  return target.m_Running;
}
void Aktion::System::Window_MainLoop(Aktion::System::AktionWindow& target, Aktion::System::AktionSystem& system)
{
  SDL_Event windowEvent;
  while (SDL_PollEvent(&windowEvent))
  {
    ImGui_ImplSDL3_ProcessEvent(&windowEvent);
    if (windowEvent.type == SDL_EVENT_QUIT)
      target.m_Running = false;
    if (windowEvent.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && windowEvent.window.windowID == SDL_GetWindowID(target.m_SDLWin))
      target.m_Running = false;
  }
  if (SDL_GetWindowFlags(target.m_SDLWin) & SDL_WINDOW_MINIMIZED)
  {
    SDL_Delay(10);
    return;
  }

  // Resize swap chain?
  int fb_width, fb_height;
  SDL_GetWindowSize(target.m_SDLWin, &fb_width, &fb_height);
  if (fb_width > 0 && fb_height > 0 && (swapChainRebuild || target.m_MainWindowData.Width != fb_width || target.m_MainWindowData.Height != fb_height))
  {
    ImGui_ImplVulkan_SetMinImageCount(minImageCount);
    ImGui_ImplVulkanH_CreateOrResizeWindow(target.m_VKInst, target.m_VKPhysicalDevice, target.m_VKDevice, &target.m_MainWindowData, target.m_VKGraphicsQueueIndex, target.m_VKAllocator, fb_width, fb_height, minImageCount);
    target.m_MainWindowData.FrameIndex = 0;
    swapChainRebuild = false;
  }

  GUI_Update(target.m_GUI, system);
  return;
}
