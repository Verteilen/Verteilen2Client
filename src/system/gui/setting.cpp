#include "setting.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>

#include <imgui_internal.h>

#include <system/gui.h>
#include <system/win.h>

bool bytes_compare(const char* s, const char* t, int length)
{
  for (int i = 0; i < length; i++)
    if (s[i] != t[i]) return false;
  return true;
}
const char* GetThemeName(uint32_t index) {
  switch (index) {
  case 0: return "Classic";
  case 1: return "White";
  case 2: return "Dark";
  };
  return "Custom";
}
void ApplyTheme(Aktion::GUI::AktionGUI& target, ImGuiIO& io, ImGuiStyle& style, int i, std::function<void(ImGuiStyle*)> dest) 
{
  dest(&style);
  dest(&target.m_Setting.m_Style);
  target.m_Theme.m_Current = i;
}
void HelpMarker(const char* desc)
{
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip())
  {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}
void StyleUpdate(Aktion::GUI::AktionGUI& target)
{
  if (bytes_compare((char*)&target.m_Setting.m_Style, (char*)&target.m_Theme.m_ThemeClassic, sizeof(ImGuiStyle)))
  {
    target.m_Theme.m_Current = 0;
    return;
  }
  else if (bytes_compare((char*)&target.m_Setting.m_Style, (char*)&target.m_Theme.m_ThemeWhite, sizeof(ImGuiStyle)))
  {
    target.m_Theme.m_Current = 1;
    return;
  }
  else if (bytes_compare((char*)&target.m_Setting.m_Style, (char*)&target.m_Theme.m_ThemeDark, sizeof(ImGuiStyle)))
  {
    target.m_Theme.m_Current = 2;
    return;
  }
  else 
  {
    target.m_Theme.m_Current = 3;
  }
  
}

void Aktion::GUI::GUI_InitSetting(AktionGUI& target)
{
  target.m_Theme.m_ThemeClassic = ImGui::StyleColorsClassic;
  target.m_Theme.m_ThemeWhite = ImGui::StyleColorsLight;
  target.m_Theme.m_ThemeDark = ImGui::StyleColorsDark;

  if (std::filesystem::exists(THEME_FILE)) {
    std::ifstream fin;
    fin.open(THEME_FILE, std::ios::binary);
    fin.read(reinterpret_cast<char*>(&target.m_Setting), sizeof(Aktion::GUI::AktionSetting));
    fin.close();
    StyleUpdate(target);
  }
  else {
    target.m_Theme.m_ThemeDark(&target.m_Setting.m_Style);
    target.m_Setting.m_GlobalScale = 3.0f;
    target.m_Setting.m_WindowScale = 1.0f;
    std::ofstream fout;
    fout.open(THEME_FILE, std::ios::binary | std::ios::out);
    fout.write((char*)&target.m_Setting, sizeof(Aktion::GUI::AktionSetting));
    fout.close();
    target.m_Theme.m_Current = 2;
  }
}
void Aktion::GUI::GUI_DrawSettingView(AktionGUI& target, Engine::Engine& engine, ImGuiIO& io, ImGuiStyle* ref)
{
  ImGuiStyle& style = ImGui::GetStyle();
  static ImGuiStyle ref_saved_style;

  // Default to using internal storage as reference
  static bool init = true;
  if (init && ref == NULL)
    ref_saved_style = style;
  init = false;
  if (ref == NULL)
    ref = &ref_saved_style;

  if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
  {
    if (ImGui::BeginTabItem("Theme"))
    {
      if (ImGui::BeginCombo("##theme", GetThemeName(target.m_Theme.m_Current)))
      {
        for (int i = 0; i < THEMECOUNT + 1; i++) {
          bool isSelect = target.m_Theme.m_Current == i;
          if (ImGui::Selectable(GetThemeName(i), isSelect))
          {
            switch (i) {
            case 0: ApplyTheme(target, io, style, i, target.m_Theme.m_ThemeClassic); break;
            case 1: ApplyTheme(target, io, style, i, target.m_Theme.m_ThemeWhite); break;
            case 2: ApplyTheme(target, io, style, i, target.m_Theme.m_ThemeDark); break;
            case 3: target.m_Theme.m_Current = 3; break;
            }
          }

          if (isSelect)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }
      if (ImGui::Button("Save"))
      {
        if (std::filesystem::exists(THEME_FILE))
          std::filesystem::remove(THEME_FILE);
        AktionSetting& sett = target.m_Setting;
        std::ofstream fout;
        fout.open(THEME_FILE, std::ios::binary | std::ios::out);
        fout.write((char*)&sett, sizeof(sett));
        fout.close();
        StyleUpdate(target);
      }
      if (ImGui::Button("Import"))
      {
        SetSetting setter = [&](AktionSetting setting) -> void {
          std::cout << "Load new setting from select file!" << std::endl;
          std::cout << "sizeof the file: " << sizeof(setting) << std::endl;
          target.m_Setting = setting;
          style = target.m_Setting.m_Style;
          io.FontGlobalScale = target.m_Setting.m_GlobalScale;
          StyleUpdate(target);
        };
        target.m_WindowContext->m_LoadSettingFunc(setter);
      }
      if (ImGui::Button("Export"))
      {
        target.m_WindowContext->m_SaveSettingFunc(target.m_Setting);
      }
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Sizes"))
    {
      ImGui::SeparatorText("Main");
      ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
      ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
      ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
      ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
      ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
      ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
      ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
      ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");

      ImGui::SeparatorText("Borders");
      ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
      ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
      ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
      ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
      ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
      ImGui::SliderFloat("TabBarBorderSize", &style.TabBarBorderSize, 0.0f, 2.0f, "%.0f");

      ImGui::SeparatorText("Rounding");
      ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
      ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
      ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
      ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
      ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
      ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
      ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");

      ImGui::SeparatorText("Tables");
      ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
      ImGui::SliderAngle("TableAngledHeadersAngle", &style.TableAngledHeadersAngle, -50.0f, +50.0f);
      ImGui::SliderFloat2("TableAngledHeadersTextAlign", (float*)&style.TableAngledHeadersTextAlign, 0.0f, 1.0f, "%.2f");

      ImGui::SeparatorText("Widgets");
      ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
      int window_menu_button_position = style.WindowMenuButtonPosition + 1;
      if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
        style.WindowMenuButtonPosition = (ImGuiDir)(window_menu_button_position - 1);
      ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
      ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
      ImGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
      ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
      ImGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
      ImGui::SliderFloat("SeparatorTextBorderSize", &style.SeparatorTextBorderSize, 0.0f, 10.0f, "%.0f");
      ImGui::SliderFloat2("SeparatorTextAlign", (float*)&style.SeparatorTextAlign, 0.0f, 1.0f, "%.2f");
      ImGui::SliderFloat2("SeparatorTextPadding", (float*)&style.SeparatorTextPadding, 0.0f, 40.0f, "%.0f");
      ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");

      ImGui::SeparatorText("Docking");
      ImGui::SliderFloat("DockingSplitterSize", &style.DockingSeparatorSize, 0.0f, 12.0f, "%.0f");

      ImGui::SeparatorText("Tooltips");
      for (int n = 0; n < 2; n++)
        if (ImGui::TreeNodeEx(n == 0 ? "HoverFlagsForTooltipMouse" : "HoverFlagsForTooltipNav"))
        {
          ImGuiHoveredFlags* p = (n == 0) ? &style.HoverFlagsForTooltipMouse : &style.HoverFlagsForTooltipNav;
          ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNone", p, ImGuiHoveredFlags_DelayNone);
          ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayShort", p, ImGuiHoveredFlags_DelayShort);
          ImGui::CheckboxFlags("ImGuiHoveredFlags_DelayNormal", p, ImGuiHoveredFlags_DelayNormal);
          ImGui::CheckboxFlags("ImGuiHoveredFlags_Stationary", p, ImGuiHoveredFlags_Stationary);
          ImGui::CheckboxFlags("ImGuiHoveredFlags_NoSharedDelay", p, ImGuiHoveredFlags_NoSharedDelay);
          ImGui::TreePop();
        }

      ImGui::SeparatorText("Misc");
      ImGui::SliderFloat2("DisplayWindowPadding", (float*)&style.DisplayWindowPadding, 0.0f, 30.0f, "%.0f"); ImGui::SameLine(); HelpMarker("Apply to regular windows: amount which we enforce to keep visible when moving near edges of your screen.");
      ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f"); ImGui::SameLine(); HelpMarker("Apply to every windows, menus, popups, tooltips: amount where we avoid displaying contents. Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Colors"))
    {
      static int output_dest = 0;
      static bool output_only_modified = true;
      if (ImGui::Button("Export"))
      {
        if (output_dest == 0)
          ImGui::LogToClipboard();
        else
          ImGui::LogToTTY();
        ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
          const ImVec4& col = style.Colors[i];
          const char* name = ImGui::GetStyleColorName(i);
          if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
            ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
              name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
        }
        ImGui::LogFinish();
      }
      ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
      ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

      static ImGuiTextFilter filter;
      filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

      static ImGuiColorEditFlags alpha_flags = 0;
      if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
      if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
      if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
      HelpMarker(
        "In the color list:\n"
        "Left-click on color square to open color picker,\n"
        "Right-click to open edit options menu.");

      ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), ImVec2(FLT_MAX, FLT_MAX));
      ImGui::BeginChild("##colors", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
      ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
      for (int i = 0; i < ImGuiCol_COUNT; i++)
      {
        const char* name = ImGui::GetStyleColorName(i);
        if (!filter.PassFilter(name))
          continue;
        ImGui::PushID(i);
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
        if (ImGui::Button("?"))
          ImGui::DebugFlashStyleColor((ImGuiCol)i);
        ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
        ImGui::SameLine();
#endif
        ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
        if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
        {
          // Tips: in a real user application, you may want to merge and use an icon font into the main font,
          // so instead of "Save"/"Revert" you'd use icons!
          // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
          ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
          ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
        }
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        ImGui::TextUnformatted(name);
        ImGui::PopID();
      }
      ImGui::PopItemWidth();
      ImGui::EndChild();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Fonts"))
    {
      ImGuiIO& io = ImGui::GetIO();
      ImFontAtlas* atlas = io.Fonts;
      HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
      ImGui::ShowFontAtlas(atlas);

      // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
      // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
      HelpMarker(
        "Those are old settings provided for convenience.\n"
        "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
        "rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
        "Using those settings here will give you poor quality results.");
      ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
      if (ImGui::DragFloat("window scale", &target.m_Setting.m_WindowScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp)) // Scale only this window
        ImGui::SetWindowFontScale(target.m_Setting.m_WindowScale);
      ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything
      ImGui::PopItemWidth();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Rendering"))
    {
      ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
      ImGui::SameLine();
      HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

      ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
      ImGui::SameLine();
      HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

      ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
      ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
      ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
      if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

      // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
      ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError, 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
      const bool show_samples = ImGui::IsItemActive();
      if (show_samples)
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
      if (show_samples && ImGui::BeginTooltip())
      {
        ImGui::TextUnformatted("(R = radius, N = approx number of segments)");
        ImGui::Spacing();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const float min_widget_width = ImGui::CalcTextSize("R: MMM\nN: MMM").x;
        for (int n = 0; n < 8; n++)
        {
          const float RAD_MIN = 5.0f;
          const float RAD_MAX = 70.0f;
          const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

          ImGui::BeginGroup();

          // N is not always exact here due to how PathArcTo() function work internally
          ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

          const float canvas_width = IM_MAX(min_widget_width, rad * 2.0f);
          const float offset_x = floorf(canvas_width * 0.5f);
          const float offset_y = floorf(RAD_MAX);

          const ImVec2 p1 = ImGui::GetCursorScreenPos();
          draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
          ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

          /*
          const ImVec2 p2 = ImGui::GetCursorScreenPos();
          draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
          ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));
          */

          ImGui::EndGroup();
          ImGui::SameLine();
        }
        ImGui::EndTooltip();
      }
      ImGui::SameLine();
      HelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");

      ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
      ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); HelpMarker("Additional alpha multiplier for disabled items (multiply over current value of Alpha).");
      ImGui::PopItemWidth();

      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Editor")) 
    {
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Project Setting"))
    {
      bool update = ImGui::Checkbox("Default No Editor", &engine.m_ProjectInfo.m_Setting.m_DefaultNoEditor);
      bool hover = ImGui::IsItemHovered();
      if (hover) 
      {
        ImGui::SetTooltip("When project file is directly open, should the editor show up?");
      }

      if (update) 
      {

      }
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
}
