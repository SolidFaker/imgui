// dear imgui: Platform Binding for HarmonyOS native app
// This needs to be used along with the OpenGL 3 Renderer (imgui_impl_opengl3)

// Implemented features:
//  [X] Platform: Keyboard support.
//  [X] Platform: Mouse support. Can discriminate Mouse/TouchScreen.
// Missing features or Issues:
//  [ ] Platform: IME support.
//  [ ] Platform: Clipboard support.
//  [ ] Platform: Gamepad support.
//  [ ] Platform: Mouse cursor shape and visibility (ImGuiBackendFlags_HasMouseCursors). Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.

#include "imgui.h"
#include <codecvt>
#include <cstdint>
#include <inputmethod/inputmethod_types_capi.h>
#include <locale>
#include <unordered_map>
#ifndef IMGUI_DISABLE
#include "imgui_impl_harmonyos.h"
#include <time.h>
#include <arkui/native_key_event.h>
#include <ace/xcomponent/native_xcomponent_key_event.h>
#include <ace/xcomponent/native_interface_xcomponent.h>

#include <inputmethod/inputmethod_attach_options_capi.h>
#include <inputmethod/inputmethod_text_editor_proxy_capi.h>
#include <inputmethod/inputmethod_inputmethod_proxy_capi.h>
#include <inputmethod/inputmethod_controller_capi.h>

// HarmonyOS data
static double                                   g_Time = 0.0;
static void*                                    g_Window;
static char                                     g_LogTag[] = "ImGuiExample";

int32_t x_pos;
int32_t y_pos;
InputMethod_InputMethodProxy *ime_proxy = nullptr;
InputMethod_TextEditorProxy *ime_editor_proxy = nullptr;
InputMethod_AttachOptions *ime_options = nullptr;

static std::unordered_map<char, char> shift_key_char_map= {
    {'`', '~'},
    {'1', '!'},
    {'2', '@'},
    {'3', '#'},
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
    {'-', '_'},
    {'=', '+'},
    {'[', '{'},
    {']', '}'},
    {';', ':'},
    {'\'', '"'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    {'\\', '|'},
    {'\t', '\t'},
    {' ', ' '},
};

static ImGuiKey ImGui_ImplHarmonyOS_KeyCodeToImGuiKey(OH_NativeXComponent_KeyCode key_code)
{
    switch (key_code)
    {
        case KEY_TAB:                  return ImGuiKey_Tab;
        case KEY_DPAD_LEFT:            return ImGuiKey_LeftArrow;
        case KEY_DPAD_RIGHT:           return ImGuiKey_RightArrow;
        case KEY_DPAD_UP:              return ImGuiKey_UpArrow;
        case KEY_DPAD_DOWN:            return ImGuiKey_DownArrow;
        case KEY_PAGE_UP:              return ImGuiKey_PageUp;
        case KEY_PAGE_DOWN:            return ImGuiKey_PageDown;
        case KEY_MOVE_HOME:            return ImGuiKey_Home;
        case KEY_MOVE_END:             return ImGuiKey_End;
        case KEY_INSERT:               return ImGuiKey_Insert;
        case KEY_FORWARD_DEL:          return ImGuiKey_Delete;
        case KEY_DEL:                  return ImGuiKey_Backspace;
        case KEY_SPACE:                return ImGuiKey_Space;
        case KEY_ENTER:                return ImGuiKey_Enter;
        case KEY_ESCAPE:               return ImGuiKey_Escape;
        case KEY_APOSTROPHE:           return ImGuiKey_Apostrophe;
        case KEY_COMMA:                return ImGuiKey_Comma;
        case KEY_MINUS:                return ImGuiKey_Minus;
        case KEY_PERIOD:               return ImGuiKey_Period;
        case KEY_SLASH:                return ImGuiKey_Slash;
        case KEY_SEMICOLON:            return ImGuiKey_Semicolon;
        case KEY_EQUALS:               return ImGuiKey_Equal;
        case KEY_LEFT_BRACKET:         return ImGuiKey_LeftBracket;
        case KEY_BACKSLASH:            return ImGuiKey_Backslash;
        case KEY_RIGHT_BRACKET:        return ImGuiKey_RightBracket;
        case KEY_GRAVE:                return ImGuiKey_GraveAccent;
        case KEY_CAPS_LOCK:            return ImGuiKey_CapsLock;
        case KEY_SCROLL_LOCK:          return ImGuiKey_ScrollLock;
        case KEY_NUM_LOCK:             return ImGuiKey_NumLock;
        case KEY_SYSRQ:                return ImGuiKey_PrintScreen;
        case KEY_BREAK:                return ImGuiKey_Pause;
        case KEY_NUMPAD_0:             return ImGuiKey_Keypad0;
        case KEY_NUMPAD_1:             return ImGuiKey_Keypad1;
        case KEY_NUMPAD_2:             return ImGuiKey_Keypad2;
        case KEY_NUMPAD_3:             return ImGuiKey_Keypad3;
        case KEY_NUMPAD_4:             return ImGuiKey_Keypad4;
        case KEY_NUMPAD_5:             return ImGuiKey_Keypad5;
        case KEY_NUMPAD_6:             return ImGuiKey_Keypad6;
        case KEY_NUMPAD_7:             return ImGuiKey_Keypad7;
        case KEY_NUMPAD_8:             return ImGuiKey_Keypad8;
        case KEY_NUMPAD_9:             return ImGuiKey_Keypad9;
        case KEY_NUMPAD_DOT:           return ImGuiKey_KeypadDecimal;
        case KEY_NUMPAD_DIVIDE:        return ImGuiKey_KeypadDivide;
        case KEY_NUMPAD_MULTIPLY:      return ImGuiKey_KeypadMultiply;
        case KEY_NUMPAD_SUBTRACT:      return ImGuiKey_KeypadSubtract;
        case KEY_NUMPAD_ADD:           return ImGuiKey_KeypadAdd;
        case KEY_NUMPAD_ENTER:         return ImGuiKey_KeypadEnter;
        case KEY_NUMPAD_EQUALS:        return ImGuiKey_KeypadEqual;
        case KEY_CTRL_LEFT:            return ImGuiKey_LeftCtrl;
        case KEY_SHIFT_LEFT:           return ImGuiKey_LeftShift;
        case KEY_ALT_LEFT:             return ImGuiKey_LeftAlt;
        case KEY_META_LEFT:            return ImGuiKey_LeftSuper;
        case KEY_CTRL_RIGHT:           return ImGuiKey_RightCtrl;
        case KEY_SHIFT_RIGHT:          return ImGuiKey_RightShift;
        case KEY_ALT_RIGHT:            return ImGuiKey_RightAlt;
        case KEY_META_RIGHT:           return ImGuiKey_RightSuper;
        case KEY_MENU:                 return ImGuiKey_Menu;
        case KEY_0:                    return ImGuiKey_0;
        case KEY_1:                    return ImGuiKey_1;
        case KEY_2:                    return ImGuiKey_2;
        case KEY_3:                    return ImGuiKey_3;
        case KEY_4:                    return ImGuiKey_4;
        case KEY_5:                    return ImGuiKey_5;
        case KEY_6:                    return ImGuiKey_6;
        case KEY_7:                    return ImGuiKey_7;
        case KEY_8:                    return ImGuiKey_8;
        case KEY_9:                    return ImGuiKey_9;
        case KEY_A:                    return ImGuiKey_A;
        case KEY_B:                    return ImGuiKey_B;
        case KEY_C:                    return ImGuiKey_C;
        case KEY_D:                    return ImGuiKey_D;
        case KEY_E:                    return ImGuiKey_E;
        case KEY_F:                    return ImGuiKey_F;
        case KEY_G:                    return ImGuiKey_G;
        case KEY_H:                    return ImGuiKey_H;
        case KEY_I:                    return ImGuiKey_I;
        case KEY_J:                    return ImGuiKey_J;
        case KEY_K:                    return ImGuiKey_K;
        case KEY_L:                    return ImGuiKey_L;
        case KEY_M:                    return ImGuiKey_M;
        case KEY_N:                    return ImGuiKey_N;
        case KEY_O:                    return ImGuiKey_O;
        case KEY_P:                    return ImGuiKey_P;
        case KEY_Q:                    return ImGuiKey_Q;
        case KEY_R:                    return ImGuiKey_R;
        case KEY_S:                    return ImGuiKey_S;
        case KEY_T:                    return ImGuiKey_T;
        case KEY_U:                    return ImGuiKey_U;
        case KEY_V:                    return ImGuiKey_V;
        case KEY_W:                    return ImGuiKey_W;
        case KEY_X:                    return ImGuiKey_X;
        case KEY_Y:                    return ImGuiKey_Y;
        case KEY_Z:                    return ImGuiKey_Z;
        case KEY_F1:                   return ImGuiKey_F1;
        case KEY_F2:                   return ImGuiKey_F2;
        case KEY_F3:                   return ImGuiKey_F3;
        case KEY_F4:                   return ImGuiKey_F4;
        case KEY_F5:                   return ImGuiKey_F5;
        case KEY_F6:                   return ImGuiKey_F6;
        case KEY_F7:                   return ImGuiKey_F7;
        case KEY_F8:                   return ImGuiKey_F8;
        case KEY_F9:                   return ImGuiKey_F9;
        case KEY_F10:                  return ImGuiKey_F10;
        case KEY_F11:                  return ImGuiKey_F11;
        case KEY_F12:                  return ImGuiKey_F12;
        default:                            return ImGuiKey_None;
    }
}

static char ImGui_ImplHarmonyOS_KeyCodeToChar(OH_NativeXComponent_KeyCode key_code)
{
    switch (key_code)
    {
        case KEY_TAB:                  return '\t';
        case KEY_SPACE:                return ' ';
        case KEY_APOSTROPHE:           return '\'';
        case KEY_COMMA:                return ',';
        case KEY_MINUS:                return '-';
        case KEY_PERIOD:               return '.';
        case KEY_SLASH:                return '/';
        case KEY_SEMICOLON:            return ';';
        case KEY_EQUALS:               return '=';
        case KEY_LEFT_BRACKET:         return '[';
        case KEY_BACKSLASH:            return '\\';
        case KEY_RIGHT_BRACKET:        return ']';
        case KEY_GRAVE:                return '`';
        //case KEY_ENTER:                return '\n';
        case KEY_NUMPAD_0:             return '0';
        case KEY_NUMPAD_1:             return '1';
        case KEY_NUMPAD_2:             return '2';
        case KEY_NUMPAD_3:             return '3';
        case KEY_NUMPAD_4:             return '4';
        case KEY_NUMPAD_5:             return '5';
        case KEY_NUMPAD_6:             return '6';
        case KEY_NUMPAD_7:             return '7';
        case KEY_NUMPAD_8:             return '8';
        case KEY_NUMPAD_9:             return '9';
        case KEY_NUMPAD_DOT:           return '.';
        case KEY_NUMPAD_DIVIDE:        return '/';
        case KEY_NUMPAD_MULTIPLY:      return '*';
        case KEY_NUMPAD_SUBTRACT:      return '-';
        case KEY_NUMPAD_ADD:           return '+';
        case KEY_NUMPAD_EQUALS:        return '=';
        case KEY_0:                    return '0';
        case KEY_1:                    return '1';
        case KEY_2:                    return '2';
        case KEY_3:                    return '3';
        case KEY_4:                    return '4';
        case KEY_5:                    return '5';
        case KEY_6:                    return '6';
        case KEY_7:                    return '7';
        case KEY_8:                    return '8';
        case KEY_9:                    return '9';
        case KEY_A:                    return 'a';
        case KEY_B:                    return 'b';
        case KEY_C:                    return 'c';
        case KEY_D:                    return 'd';
        case KEY_E:                    return 'e';
        case KEY_F:                    return 'f';
        case KEY_G:                    return 'g';
        case KEY_H:                    return 'h';
        case KEY_I:                    return 'i';
        case KEY_J:                    return 'j';
        case KEY_K:                    return 'k';
        case KEY_L:                    return 'l';
        case KEY_M:                    return 'm';
        case KEY_N:                    return 'n';
        case KEY_O:                    return 'o';
        case KEY_P:                    return 'p';
        case KEY_Q:                    return 'q';
        case KEY_R:                    return 'r';
        case KEY_S:                    return 's';
        case KEY_T:                    return 't';
        case KEY_U:                    return 'u';
        case KEY_V:                    return 'v';
        case KEY_W:                    return 'w';
        case KEY_X:                    return 'x';
        case KEY_Y:                    return 'y';
        case KEY_Z:                    return 'z';
        default:                            return 0;
    }
}

int32_t ImGui_ImplHarmonyOS_HandleUIInputEvent(ArkUI_UIInputEvent* key_event)
{
    ImGuiIO& io = ImGui::GetIO();
    const char key_char = *OH_ArkUI_KeyEvent_GetKeyText(key_event);
    io.AddInputCharacter(key_char);
}

int32_t ImGui_ImplHarmonyOS_HandleKeyEvent(OH_NativeXComponent_KeyEvent* key_event)
{
    ImGuiIO& io = ImGui::GetIO();
    OH_NativeXComponent_KeyCode event_key_code;
    OH_NativeXComponent_KeyAction event_action;
    char key_char;
    
    OH_NativeXComponent_GetKeyEventCode(key_event, &event_key_code);
    OH_NativeXComponent_GetKeyEventAction(key_event, &event_action);
    key_char = ImGui_ImplHarmonyOS_KeyCodeToChar(event_key_code);
    switch (event_action)
    {
    case OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN: {
        if (event_key_code >= KEY_A && event_key_code <= KEY_Z) {
            if (io.KeyShift) {
                key_char += 'A' - 'a';
            }
            io.AddInputCharacter(key_char);
        } else if (key_char > 0) {
            if (io.KeyShift) {
                io.AddInputCharacter(shift_key_char_map[key_char]);
            } else {
                io.AddInputCharacter(key_char);
            }
        }
    }
    case OH_NATIVEXCOMPONENT_KEY_ACTION_UP: {
        ImGuiKey key = ImGui_ImplHarmonyOS_KeyCodeToImGuiKey(event_key_code);
        
        if (key != ImGuiKey_None) {
            io.AddKeyEvent(key, event_action == OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN);
        }
        
        /*
        if (key == ImGuiKey_Backspace || key == ImGuiKey_Delete) {
            io.AddKeyEvent(key, true);
            io.AddKeyEvent(key, false);
        }
        */
        
        if (key == ImGuiKey_LeftShift || key == ImGuiKey_RightShift) {
            io.AddKeyEvent(ImGuiMod_Shift, event_action == OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN);
        }
        if (key == ImGuiKey_LeftCtrl || key == ImGuiKey_RightCtrl) {
            io.AddKeyEvent(ImGuiMod_Ctrl, event_action == OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN);
        }
        if (key == ImGuiKey_LeftAlt || key == ImGuiKey_RightAlt) {
            io.AddKeyEvent(ImGuiMod_Alt, event_action == OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN);
        }
        if (key == ImGuiKey_LeftSuper) {
            io.AddKeyEvent(ImGuiMod_Super, event_action == OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN);
        }
        break;
    }default:
        break;
    }
    return 0;
}

int32_t ImGui_ImplHarmonyOS_HandleFocusEvent()
{
    return 0;
}

int32_t ImGui_ImplHarmonyOS_HandleBlurEvent()
{
    ImGuiIO& io = ImGui::GetIO();
    io.ClearInputKeys();
    return 0;
}

int32_t ImGui_ImplHarmonyOS_HandleMouseEvent(OH_NativeXComponent_MouseEvent* mouse_event)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    OH_NativeXComponent_MouseEventAction event_action = mouse_event->action;
    OH_NativeXComponent_MouseEventButton button_state = mouse_event->button;
    if (event_action == OH_NATIVEXCOMPONENT_MOUSE_MOVE) {
        io.AddMousePosEvent(mouse_event->x, mouse_event->y);
    }
    switch (button_state) {
        case OH_NATIVEXCOMPONENT_LEFT_BUTTON:
            io.AddMouseButtonEvent(0, (event_action != OH_NATIVEXCOMPONENT_MOUSE_RELEASE));
            break;
        case OH_NATIVEXCOMPONENT_RIGHT_BUTTON:
            io.AddMouseButtonEvent(1, (event_action != OH_NATIVEXCOMPONENT_MOUSE_RELEASE));
            break;
        case OH_NATIVEXCOMPONENT_MIDDLE_BUTTON:
            io.AddMouseButtonEvent(2, (event_action != OH_NATIVEXCOMPONENT_MOUSE_RELEASE));
            break;
        default:
            break;
    }
    return 0;
}

int32_t ImGui_ImplHarmonyOS_HandleMouseWheelEvent(double x, double y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
    io.AddMouseWheelEvent(x, y);
    return 0;
}

int32_t ImGui_ImplHarmonyOS_HandleTouchEvent(OH_NativeXComponent_TouchEvent* touch_event)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
    OH_NativeXComponent_TouchEventType event_action = touch_event->type;
    switch (event_action) {
        case OH_NATIVEXCOMPONENT_UP:
        case OH_NATIVEXCOMPONENT_DOWN:
            io.AddMousePosEvent(touch_event->x, touch_event->y);
            io.AddMouseButtonEvent(0, (event_action == OH_NATIVEXCOMPONENT_DOWN));
            break;
        case OH_NATIVEXCOMPONENT_MOVE:
            io.AddMousePosEvent(touch_event->x, touch_event->y);
            break;
        default:
            break;
    }
    return 0;
}

void ImGui_ImplHarmonyOS_UpdateWindowInfo(int32_t x, int32_t y)
{
    x_pos = x;
    y_pos = y;
}

static void ImGui_ImplHarmonyOS_PlatformSetImeData(ImGuiContext*, ImGuiViewport* viewport, ImGuiPlatformImeData* data)
{
    // TODO: IME related
    OH_InputMethodController_Attach(ime_editor_proxy, ime_options, &ime_proxy);
    //OH_InputMethodProxy_ShowKeyboard(ime_proxy);
    InputMethod_CursorInfo *cursor = OH_CursorInfo_Create(x_pos + data->InputPos.x, y_pos + data->InputPos.y, 1, data->InputLineHeight);
    OH_InputMethodProxy_NotifyCursorUpdate(ime_proxy, cursor);
}

static const char* ImGui_ImplHarmonyOS_GetClipboardText(ImGuiContext*)
{
    return nullptr;
}

static void ImGui_ImplHarmonyOS_SetClipboardText(ImGuiContext*, const char* text)
{
}

bool ImGui_ImplHarmonyOS_Init(void* window)
{
    IMGUI_CHECKVERSION();

    g_Window = window;
    g_Time = 0.0;

    // Setup backend capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl_harmonyos";

    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Platform_SetClipboardTextFn = ImGui_ImplHarmonyOS_SetClipboardText;
    platform_io.Platform_GetClipboardTextFn = ImGui_ImplHarmonyOS_GetClipboardText;
    platform_io.Platform_SetImeDataFn = ImGui_ImplHarmonyOS_PlatformSetImeData;
    platform_io.Platform_OpenInShellFn = [](ImGuiContext*, const char* url) { return true; };
    
    io.Fonts->AddFontFromFileTTF("/system/fonts/HarmonyOS_Sans_SC.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    
    ime_editor_proxy = OH_TextEditorProxy_Create();
    ime_options = OH_AttachOptions_Create(true);
    
    OH_TextEditorProxy_SetGetTextConfigFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, InputMethod_TextConfig *config) {
        OH_TextConfig_SetInputType(config, IME_TEXT_INPUT_TYPE_TEXT);
        OH_TextConfig_SetPreviewTextSupport(config, false);
    });
    OH_TextEditorProxy_SetInsertTextFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, const char16_t *text, size_t length){
        ImGuiIO& io = ImGui::GetIO();
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
        // Convert from utf16 to utf8
        std::string str = conv.to_bytes(text);
        io.AddInputCharactersUTF8(str.c_str());
    });
    OH_TextEditorProxy_SetDeleteForwardFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, int32_t length){
        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(ImGuiKey_Delete, true);
        io.AddKeyEvent(ImGuiKey_Delete, false);
    });
    OH_TextEditorProxy_SetDeleteBackwardFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, int32_t length){
        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(ImGuiKey_Backspace, true);
        io.AddKeyEvent(ImGuiKey_Backspace, false);
    });
    OH_TextEditorProxy_SetSendKeyboardStatusFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, InputMethod_KeyboardStatus keyboardStatus){});
    OH_TextEditorProxy_SetSendEnterKeyFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, InputMethod_EnterKeyType enterKeyType){
        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(ImGuiKey_Enter, true);
        io.AddKeyEvent(ImGuiKey_Enter, false);
    });
    OH_TextEditorProxy_SetMoveCursorFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, InputMethod_Direction direction){
        ImGuiIO& io = ImGui::GetIO();
        switch (direction) {
            case IME_DIRECTION_UP:
                io.AddKeyEvent(ImGuiKey_UpArrow, true);
                io.AddKeyEvent(ImGuiKey_UpArrow, false);
                break;
            case IME_DIRECTION_DOWN:
                io.AddKeyEvent(ImGuiKey_DownArrow, true);
                io.AddKeyEvent(ImGuiKey_DownArrow, false);
                break;
            case IME_DIRECTION_RIGHT:
                io.AddKeyEvent(ImGuiKey_RightArrow, true);
                io.AddKeyEvent(ImGuiKey_RightArrow, false);
                break;
            case IME_DIRECTION_LEFT:
                io.AddKeyEvent(ImGuiKey_LeftArrow, true);
                io.AddKeyEvent(ImGuiKey_LeftArrow, false);
                break;
            default: 
                break;
        }
    });
    OH_TextEditorProxy_SetHandleSetSelectionFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, int32_t start, int32_t end){});
    OH_TextEditorProxy_SetHandleExtendActionFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, InputMethod_ExtendAction action){});
    OH_TextEditorProxy_SetGetLeftTextOfCursorFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, int32_t number, char16_t text[], size_t *length){});
    OH_TextEditorProxy_SetGetRightTextOfCursorFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, int32_t number, char16_t text[], size_t *length){});
    OH_TextEditorProxy_SetGetTextIndexAtCursorFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy){ return 0; });
    OH_TextEditorProxy_SetReceivePrivateCommandFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, InputMethod_PrivateCommand *privateCommand[], size_t size){ return 0; });
    OH_TextEditorProxy_SetSetPreviewTextFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy, const char16_t text[], size_t length, int32_t start, int32_t end){ return 0; });
    OH_TextEditorProxy_SetFinishTextPreviewFunc(ime_editor_proxy, [](InputMethod_TextEditorProxy *textEditorProxy){});
    
    return true;
}

void ImGui_ImplHarmonyOS_Shutdown()
{
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = nullptr;
    OH_InputMethodController_Detach(ime_proxy);
    OH_TextEditorProxy_Destroy(ime_editor_proxy);
    OH_AttachOptions_Destroy(ime_options);
}

void ImGui_ImplHarmonyOS_NewFrame(int32_t window_width, int32_t window_height)
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int display_width = window_width;
    int display_height = window_height;

    io.DisplaySize = ImVec2((float)window_width, (float)window_height);
    if (window_width > 0 && window_height > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_width / window_width, (float)display_height / window_height);

    // Setup time step
    struct timespec current_timespec;
    clock_gettime(CLOCK_MONOTONIC, &current_timespec);
    double current_time = (double)(current_timespec.tv_sec) + (current_timespec.tv_nsec / 1000000000.0);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;
}

#endif // #ifndef IMGUI_DISABLE
