#include "gui.h"
//
// Standard library
//
#include <iostream>
#include <filesystem>
#include <functional>
#include <fstream>
//
// Custom Include
//
#include <system/system.h>
#include <system/win.h>
#include <system/gui/logic.h>
#include <system/gui/comp.h>
#include <system/gui/menu.h>
#include <system/gui/panel.h>
#include <system/gui/setting.h>

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <external/stb_image/stb_image.h>
}

static bool m_GUIInit = false;
static Aktion::GUI::AktionGUI* current_gui = nullptr;

ImGuiWindowFlags GetWindowFlags()
{
  return
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_MenuBar |
    ImGuiWindowFlags_NoBringToFrontOnFocus;
}
void check_vk_result(VkResult err)
{
  if (err == 0)
    return;
  fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
  if (err < 0)
    abort();
}
void GUIInit(Aktion::GUI::AktionGUI& target) 
{
  target.m_Layout = Aktion::GUI::GUI_GenerateLayoutBuffer();
  Aktion::GUI::GUI_InitSetting(target);
}

void UpdateTouch(Aktion::GUI::AktionGUI& target)
{
  const auto deltaTime = ImGui::GetIO().DeltaTime;
  for (auto& entry : target.m_Logic.m_NodeTouchTime)
  {
    if (entry.second > 0.0f)
      entry.second -= deltaTime;
  }
}
void Register(Aktion::GUI::AktionGUI& target, SDL_Window* win, ImGui_ImplVulkan_InitInfo info, ImGui_ImplVulkanH_Window* _wd)
{
  target.m_InitInfo = info;
  target.wd = _wd;
  ImGui_ImplSDL3_InitForVulkan(win);
  ImGui_ImplVulkan_Init(&target.m_InitInfo);

  ImGuiIO& io = ImGui::GetIO();
  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != nullptr);

  /*
  IM_DELETE(io.Fonts);

  io.Fonts = IM_NEW(ImFontAtlas);

  ImFontConfig config;
  config.OversampleH = 4;
  config.OversampleV = 4;
  config.PixelSnapH = false;

  m_DefaultFont = io.Fonts->AddFontFromFileTTF("data/Play-Regular.ttf", 18.0f, &config);
  m_HeaderFont = io.Fonts->AddFontFromFileTTF("data/Cuprum-Bold.ttf", 20.0f, &config);

  io.Fonts->Build();
  */
}
void FrameRender(Aktion::GUI::AktionGUI& target, ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
{
  VkResult err;

  VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
  err = vkAcquireNextImageKHR(target.m_InitInfo.Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
  {
    target.m_WindowContext->m_SetSwapChainRebuild(true);
    return;
  }
  check_vk_result(err);

  ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
  {
    err = vkWaitForFences(target.m_InitInfo.Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
    check_vk_result(err);

    err = vkResetFences(target.m_InitInfo.Device, 1, &fd->Fence);
    check_vk_result(err);
  }
  {
    err = vkResetCommandPool(target.m_InitInfo.Device, fd->CommandPool, 0);
    check_vk_result(err);
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
    check_vk_result(err);
  }
  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = wd->RenderPass;
    info.framebuffer = fd->Framebuffer;
    info.renderArea.extent.width = wd->Width;
    info.renderArea.extent.height = wd->Height;
    info.clearValueCount = 1;
    info.pClearValues = &wd->ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  // Record dear imgui primitives into command buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

  // Submit command buffer
  vkCmdEndRenderPass(fd->CommandBuffer);
  {
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_acquired_semaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &fd->CommandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphore;

    err = vkEndCommandBuffer(fd->CommandBuffer);
    check_vk_result(err);
    err = vkQueueSubmit(target.m_InitInfo.Queue, 1, &info, fd->Fence);
    check_vk_result(err);
  }
}
void FramePresent(Aktion::GUI::AktionGUI& target, ImGui_ImplVulkanH_Window* wd)
{
  if (target.m_WindowContext->m_GetSwapChainRebuild())
    return;
  VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &wd->Swapchain;
  info.pImageIndices = &wd->FrameIndex;
  VkResult err = vkQueuePresentKHR(target.m_InitInfo.Queue, &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
  {
    target.m_WindowContext->m_SetSwapChainRebuild(true);
    return;
  }
  check_vk_result(err);
  wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
}
void StartDrawing(Aktion::GUI::AktionGUI& target, Aktion::Engine::Engine& engine, ImGuiIO& io)
{
  ImVec2 c_pos = ImGui::GetMainViewport()->Pos;
  ImVec2 c_size = io.DisplaySize;
  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::SetNextWindowSize(ImVec2(c_size.x, c_size.y));
  ImGui::SetNextWindowPos(ImVec2(c_pos.x, c_pos.y));
  if (ImGui::Begin("MainFrame", 0, GetWindowFlags()));
  {
    Aktion::GUI::GUI_DrawMainMenuBar(target);
    Aktion::GUI::GUI_DrawMainPanels(target, engine);
    ImGui::End();
  }

}

ImTextureID CreateTexture(const void* data, int width, int height)
{
  return nullptr;
  //return m_Renderer->CreateTexture(data, width, height);
}
ImTextureID LoadTexture(const char* path)
{
  int width = 0, height = 0, component = 0;
  if (auto data = stbi_load(path, &width, &height, &component, 4))
  {
    auto texture = CreateTexture(data, width, height);
    stbi_image_free(data);
    return texture;
  }
  else
    return nullptr;
}
void DestroyTexture(ImTextureID texture)
{
  //m_Renderer->DestroyTexture(texture);
}
int Aktion::GUI::GUI_GetTextureWidth(ImTextureID texture)
{
  // TODO
  return 0;
}
int Aktion::GUI::GUI_GetTextureHeight(ImTextureID texture)
{
  // TODO
  return 0;
}

void Aktion::GUI::GUI_Focus(AktionGUI& target)
{
  current_gui = &target;
}
void Aktion::GUI::GUI_RegisterWindowContext(Aktion::System::WindowContext* winContext)
{
  current_gui->m_WindowContext = winContext;
}
void Aktion::GUI::GUI_Init(Aktion::GUI::AktionGUI& target, SDL_Window* win, ImGui_ImplVulkan_InitInfo info, ImGui_ImplVulkanH_Window* _wd)
{
  if (!m_GUIInit) {
    IMGUI_CHECKVERSION();
    GUIInit(target);
  }
  m_GUIInit = true;
  current_gui = &target;
  target.m_GuiContext = ImGui::CreateContext();
  ImGui::SetCurrentContext(target.m_GuiContext);
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
  //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
  //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
  io.ConfigInputTrickleEventQueue = true;
  io.ConfigInputTextCursorBlink = true;
  io.ConfigWindowsResizeFromEdges = true;

  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
  io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
  io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
  io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  style = target.m_Setting.m_Style;
  io.FontGlobalScale = target.m_Setting.m_GlobalScale;

  GUI_Logic_InitLogicEditor(target.m_Logic);
  GUI_Comp_InitNodeEditor(target.m_Comp);

  target.m_Logic.m_HeaderBackground = LoadTexture("data/BlueprintBackground.png");
  target.m_Logic.m_SaveIcon = LoadTexture("data/ic_save_white_24dp.png");
  target.m_Logic.m_RestoreIcon = LoadTexture("data/ic_restore_white_24dp.png");
  target.m_Comp.m_HeaderBackground = LoadTexture("data/BlueprintBackground.png");
  target.m_Comp.m_SaveIcon = LoadTexture("data/ic_save_white_24dp.png");
  target.m_Comp.m_RestoreIcon = LoadTexture("data/ic_restore_white_24dp.png");
  Register(target, win, info, _wd);
}
bool Aktion::GUI::GUI_Update(Aktion::GUI::AktionGUI& target, System::AktionSystem& system)
{
  UpdateTouch(target);
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
  //ImGui::ShowDemoWindow();
  StartDrawing(target, system.m_Engine, io);

  ImGui::Render();
  ImDrawData* main_draw_data = ImGui::GetDrawData();
  const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
  target.wd->ClearValue.color.float32[0] = gui_clear_color[0] * gui_clear_color[3];
  target.wd->ClearValue.color.float32[1] = gui_clear_color[1] * gui_clear_color[3];
  target.wd->ClearValue.color.float32[2] = gui_clear_color[2] * gui_clear_color[3];
  target.wd->ClearValue.color.float32[3] = gui_clear_color[3];
  if (!main_is_minimized)
    FrameRender(target, target.wd, main_draw_data);

  // Update and Render additional Platform Windows
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  // Present Main Platform Window
  if (!main_is_minimized)
    FramePresent(target, target.wd);

  return true;
}
void Aktion::GUI::GUI_Destroy(Aktion::GUI::AktionGUI& target)
{
  ax::NodeEditor::DestroyEditor(target.m_Logic.m_EditorContext);
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}
