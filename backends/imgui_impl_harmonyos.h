// dear imgui: Platform Binding for HarmonyOS native app
// This needs to be used along with the OpenGL 3 Renderer (imgui_impl_opengl3)

// Implemented features:
//  [X] Platform: Keyboard support.
//  [X] Platform: Mouse support. Can discriminate Mouse/TouchScreen.
// Missing features or Issues:
//  [ ] Platform: IME support.
//  [ ] Platform: Clipboard support.
//  [ ] Platform: Gamepad support.
//  [ ] Platform: Mouse cursor shape and visibility (ImGuiBackendFlags_HasMouseCursors). Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'. FIXME: Check if this is even possible with HarmonyOS.
// Important:
//  - Consider using SDL or GLFW backend on HarmonyOS, which will be more full-featured than this.

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API
#ifndef IMGUI_DISABLE

#include <ace/xcomponent/native_interface_xcomponent.h>

// Follow "Getting Started" link and check examples/ folder to learn about using backends!
IMGUI_IMPL_API bool     ImGui_ImplHarmonyOS_Init(void* window);
IMGUI_IMPL_API void     ImGui_ImplHarmonyOS_UpdateWindowInfo(int32_t x, int32_t y);
IMGUI_IMPL_API int32_t  ImGui_ImplHarmonyOS_HandleKeyEvent(OH_NativeXComponent_KeyEvent* key_event);
IMGUI_IMPL_API int32_t  ImGui_ImplHarmonyOS_HandleTouchEvent(OH_NativeXComponent_TouchEvent* touch_event);
IMGUI_IMPL_API int32_t  ImGui_ImplHarmonyOS_HandleMouseEvent(OH_NativeXComponent_MouseEvent* mouse_event);
IMGUI_IMPL_API int32_t  ImGui_ImplHarmonyOS_HandleUIInputEvent(ArkUI_UIInputEvent* key_event);
IMGUI_IMPL_API int32_t  ImGui_ImplHarmonyOS_HandleMouseWheelEvent(double x, double y);
IMGUI_IMPL_API int32_t  ImGui_ImplHarmonyOS_HandleFocusEvent();
IMGUI_IMPL_API int32_t  ImGui_ImplHarmonyOS_HandleBlurEvent();
IMGUI_IMPL_API void     ImGui_ImplHarmonyOS_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplHarmonyOS_NewFrame(int32_t window_width, int32_t window_height);

#endif // #ifndef IMGUI_DISABLE
