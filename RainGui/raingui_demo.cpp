// dear raingui, v1.83
// (demo code)

// Help:
// - Read FAQ at http://dearraingui.org/faq
// - Newcomers, read 'Programmer guide' in raingui.cpp for notes on how to setup Dear RainGui in your codebase.
// - Call and read RainGui::ShowDemoWindow() in raingui_demo.cpp. All applications in examples/ are doing that.
// Read raingui.cpp for more details, documentation and comments.
// Get the latest version at https://github.com/ocornut/raingui

// Message to the person tempted to delete this file when integrating Dear RainGui into their codebase:
// Do NOT remove this file from your project! Think again! It is the most useful reference code that you and other
// coders will want to refer to and call. Have the RainGui::ShowDemoWindow() function wired in an always-available
// debug menu of your game/app! Removing this file from your project is hindering access to documentation for everyone
// in your team, likely leading you to poorer usage of the library.
// Everything in this file will be stripped out by the linker if you don't call RainGui::ShowDemoWindow().
// If you want to link core Dear RainGui in your shipped builds but want a thorough guarantee that the demo will not be
// linked, you can setup your imconfig.h with #define RAINGUI_DISABLE_DEMO_WINDOWS and those functions will be empty.
// In another situation, whenever you have Dear RainGui available you probably want this to be available for reference.
// Thank you,
// -Your beloved friend, raingui_demo.cpp (which you won't delete)

// Message to beginner C/C++ programmers about the meaning of the 'static' keyword:
// In this demo code, we frequently use 'static' variables inside functions. A static variable persists across calls,
// so it is essentially like a global variable but declared inside the scope of the function. We do this as a way to
// gather code and data in the same place, to make the demo source code faster to read, faster to write, and smaller
// in size. It also happens to be a convenient way of storing simple UI related information as long as your function
// doesn't need to be reentrant or used in multiple threads. This might be a pattern you will want to use in your code,
// but most of the real data you would be editing is likely going to be stored outside your functions.

// The Demo code in this file is designed to be easy to copy-and-paste into your application!
// Because of this:
// - We never omit the RainGui:: prefix when calling functions, even though most code here is in the same namespace.
// - We try to declare static variables in the local scope, as close as possible to the code using them.
// - We never use any of the helpers/facilities used internally by Dear RainGui, unless available in the public API.
// - We never use maths operators on ImVec2/ImVec4. For our other sources files we use them, and they are provided
//   by raingui_internal.h using the RAINGUI_DEFINE_MATH_OPERATORS define. For your own sources file they are optional
//   and require you either enable those, either provide your own via IM_VEC2_CLASS_EXTRA in imconfig.h.
//   Because we can't assume anything about your support of maths operators, we cannot use them in raingui_demo.cpp.

// Navigating this file:
// - In Visual Studio IDE: CTRL+comma ("Edit.NavigateTo") can follow symbols in comments, whereas CTRL+F12 ("Edit.GoToImplementation") cannot.
// - With Visual Assist installed: ALT+G ("VAssistX.GoToImplementation") can also follow symbols in comments.

/*

Index of this file:

// [SECTION] Forward Declarations, Helpers
// [SECTION] Demo Window / ShowDemoWindow()
// - sub section: ShowDemoWindowWidgets()
// - sub section: ShowDemoWindowLayout()
// - sub section: ShowDemoWindowPopups()
// - sub section: ShowDemoWindowTables()
// - sub section: ShowDemoWindowMisc()
// [SECTION] About Window / ShowAboutWindow()
// [SECTION] Font Viewer / ShowFontAtlas()
// [SECTION] Style Editor / ShowStyleEditor()
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
// [SECTION] Example App: Simple overlay / ShowExampleAppSimpleOverlay()
// [SECTION] Example App: Fullscreen window / ShowExampleAppFullscreen()
// [SECTION] Example App: Manipulating window titles / ShowExampleAppWindowTitles()
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "raingui.h"
#ifndef RAINGUI_DISABLE

// System includes
#include <ctype.h>          // toupper
#include <limits.h>         // INT_MIN, INT_MAX
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>         // intptr_t
#else
#include <stdint.h>         // intptr_t
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#pragma warning (disable: 26451)    // [Static Analyzer] Arithmetic overflow : Using operator 'xxx' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator 'xxx' to avoid overflow(io.2).
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'                     // not all warnings are known by all Clang versions and they tend to be rename-happy.. so ignoring warnings triggers new warnings on some configuration. Great!
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"                 // warning: use of old-style cast                           // yes, they are more terse.
#pragma clang diagnostic ignored "-Wdeprecated-declarations"        // warning: 'xx' is deprecated: The POSIX name for this..   // for strdup used in demo code (so user can copy & paste the code)
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"       // warning: cast to 'void *' from smaller integer type
#pragma clang diagnostic ignored "-Wformat-security"                // warning: format string is not a string literal
#pragma clang diagnostic ignored "-Wexit-time-destructors"          // warning: declaration requires an exit-time destructor    // exit-time destruction order is undefined. if MemFree() leads to users code that has been disabled before exit it might cause problems. RainGui coding style welcomes static/globals.
#pragma clang diagnostic ignored "-Wunused-macros"                  // warning: macro is not used                               // we define snprintf/vsnprintf on Windows so they are available, but not always used.
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant                   // some standard header variations use #define NULL 0
#pragma clang diagnostic ignored "-Wdouble-promotion"               // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#pragma clang diagnostic ignored "-Wreserved-id-macro"              // warning: macro name is a reserved identifier
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"      // warning: cast to pointer from integer of different size
#pragma GCC diagnostic ignored "-Wformat-security"          // warning: format string is not a string literal (potentially insecure)
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wmisleading-indentation"   // [__GNUC__ >= 6] warning: this 'if' clause does not guard this statement      // GCC 6.0+ only. See #883 on GitHub.
#endif

// Play it nice with Windows users (Update: May 2018, Notepad now supports Unix-style carriage returns!)
#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#else
#define IM_NEWLINE  "\n"
#endif

// Helpers
#if defined(_MSC_VER) && !defined(snprintf)
#define snprintf    _snprintf
#endif
#if defined(_MSC_VER) && !defined(vsnprintf)
#define vsnprintf   _vsnprintf
#endif

// Format specifiers, printing 64-bit hasn't been decently standardized...
// In a real application you should be using PRId64 and PRIu64 from <inttypes.h> (non-windows) and on Windows define them yourself.
#ifdef _MSC_VER
#define IM_PRId64   "I64d"
#define IM_PRIu64   "I64u"
#else
#define IM_PRId64   "lld"
#define IM_PRIu64   "llu"
#endif

// Helpers macros
// We normally try to not use many helpers in raingui_demo.cpp in order to make code easier to copy and paste,
// but making an exception here as those are largely simplifying code...
// In other raingui sources we can use nicer internal functions from raingui_internal.h (ImMin/ImMax) but not in the demo.
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

// Enforce cdecl calling convention for functions called by the standard library, in case compilation settings changed the default to e.g. __vectorcall
#ifndef RAINGUI_CDECL
#ifdef _MSC_VER
#define RAINGUI_CDECL __cdecl
#else
#define RAINGUI_CDECL
#endif
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward Declarations, Helpers
//-----------------------------------------------------------------------------

#if !defined(RAINGUI_DISABLE_DEMO_WINDOWS)

// Forward Declarations
static void ShowExampleAppDocuments(bool* p_open);
static void ShowExampleAppMainMenuBar();
static void ShowExampleAppConsole(bool* p_open);
static void ShowExampleAppLog(bool* p_open);
static void ShowExampleAppLayout(bool* p_open);
static void ShowExampleAppPropertyEditor(bool* p_open);
static void ShowExampleAppLongText(bool* p_open);
static void ShowExampleAppAutoResize(bool* p_open);
static void ShowExampleAppConstrainedResize(bool* p_open);
static void ShowExampleAppSimpleOverlay(bool* p_open);
static void ShowExampleAppFullscreen(bool* p_open);
static void ShowExampleAppWindowTitles(bool* p_open);
static void ShowExampleAppCustomRendering(bool* p_open);
static void ShowExampleMenuFile();

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    RainGui::TextDisabled("(?)");
    if (RainGui::IsItemHovered())
    {
        RainGui::BeginTooltip();
        RainGui::PushTextWrapPos(RainGui::GetFontSize() * 35.0f);
        RainGui::TextUnformatted(desc);
        RainGui::PopTextWrapPos();
        RainGui::EndTooltip();
    }
}

// Helper to display basic user controls.
void RainGui::ShowUserGuide()
{
    RainGuiIO& io = RainGui::GetIO();
    RainGui::BulletText("Double-click on title bar to collapse window.");
    RainGui::BulletText(
        "Click and drag on lower corner to resize window\n"
        "(double-click to auto fit window to its contents).");
    RainGui::BulletText("CTRL+Click on a slider or drag box to input value as text.");
    RainGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
    if (io.FontAllowUserScaling)
        RainGui::BulletText("CTRL+Mouse Wheel to zoom window contents.");
    RainGui::BulletText("While inputing text:\n");
    RainGui::Indent();
    RainGui::BulletText("CTRL+Left/Right to word jump.");
    RainGui::BulletText("CTRL+A or double-click to select all.");
    RainGui::BulletText("CTRL+X/C/V to use clipboard cut/copy/paste.");
    RainGui::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
    RainGui::BulletText("ESCAPE to revert.");
    RainGui::BulletText("You can apply arithmetic operators +,*,/ on numerical values.\nUse +- to subtract.");
    RainGui::Unindent();
    RainGui::BulletText("With keyboard navigation enabled:");
    RainGui::Indent();
    RainGui::BulletText("Arrow keys to navigate.");
    RainGui::BulletText("Space to activate a widget.");
    RainGui::BulletText("Return to input text into a widget.");
    RainGui::BulletText("Escape to deactivate a widget, close popup, exit child window.");
    RainGui::BulletText("Alt to jump to the menu layer of a window.");
    RainGui::BulletText("CTRL+Tab to select a window.");
    RainGui::Unindent();
}

//-----------------------------------------------------------------------------
// [SECTION] Demo Window / ShowDemoWindow()
//-----------------------------------------------------------------------------
// - ShowDemoWindowWidgets()
// - ShowDemoWindowLayout()
// - ShowDemoWindowPopups()
// - ShowDemoWindowTables()
// - ShowDemoWindowColumns()
// - ShowDemoWindowMisc()
//-----------------------------------------------------------------------------

// We split the contents of the big ShowDemoWindow() function into smaller functions
// (because the link time of very large functions grow non-linearly)
static void ShowDemoWindowWidgets();
static void ShowDemoWindowLayout();
static void ShowDemoWindowPopups();
static void ShowDemoWindowTables();
static void ShowDemoWindowColumns();
static void ShowDemoWindowMisc();

// Demonstrate most Dear RainGui features (this is big function!)
// You may execute this function to experiment with the UI and understand what it does.
// You may then search for keywords in the code when you are interested by a specific feature.
void RainGui::ShowDemoWindow(bool* p_open)
{
    // Exceptionally add an extra assert here for people confused about initial Dear RainGui setup
    // Most RainGui functions would normally just crash if the context is missing.
    IM_ASSERT(RainGui::GetCurrentContext() != NULL && "Missing dear raingui context. Refer to examples app!");

    // Examples Apps (accessible from the "Examples" menu)
    static bool show_app_main_menu_bar = false;
    static bool show_app_documents = false;

    static bool show_app_console = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_long_text = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_fullscreen = false;
    static bool show_app_window_titles = false;
    static bool show_app_custom_rendering = false;

    if (show_app_main_menu_bar)       ShowExampleAppMainMenuBar();
    if (show_app_documents)           ShowExampleAppDocuments(&show_app_documents);

    if (show_app_console)             ShowExampleAppConsole(&show_app_console);
    if (show_app_log)                 ShowExampleAppLog(&show_app_log);
    if (show_app_layout)              ShowExampleAppLayout(&show_app_layout);
    if (show_app_property_editor)     ShowExampleAppPropertyEditor(&show_app_property_editor);
    if (show_app_long_text)           ShowExampleAppLongText(&show_app_long_text);
    if (show_app_auto_resize)         ShowExampleAppAutoResize(&show_app_auto_resize);
    if (show_app_constrained_resize)  ShowExampleAppConstrainedResize(&show_app_constrained_resize);
    if (show_app_simple_overlay)      ShowExampleAppSimpleOverlay(&show_app_simple_overlay);
    if (show_app_fullscreen)          ShowExampleAppFullscreen(&show_app_fullscreen);
    if (show_app_window_titles)       ShowExampleAppWindowTitles(&show_app_window_titles);
    if (show_app_custom_rendering)    ShowExampleAppCustomRendering(&show_app_custom_rendering);

    // Dear RainGui Apps (accessible from the "Tools" menu)
    static bool show_app_metrics = false;
    static bool show_app_style_editor = false;
    static bool show_app_about = false;

    if (show_app_metrics)       { RainGui::ShowMetricsWindow(&show_app_metrics); }
    if (show_app_about)         { RainGui::ShowAboutWindow(&show_app_about); }
    if (show_app_style_editor)
    {
        RainGui::Begin("Dear RainGui Style Editor", &show_app_style_editor);
        RainGui::ShowStyleEditor();
        RainGui::End();
    }

    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;

    RainGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= RainGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= RainGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= RainGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= RainGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= RainGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= RainGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= RainGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= RainGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= RainGuiWindowFlags_NoBringToFrontOnFocus;
    if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const RainGuiViewport* main_viewport = RainGui::GetMainViewport();
    RainGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), RainGuiCond_FirstUseEver);
    RainGui::SetNextWindowSize(ImVec2(550, 680), RainGuiCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!RainGui::Begin("Dear RainGui Demo", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        RainGui::End();
        return;
    }

    // Most "big" widgets share a common width settings by default. See 'Demo->Layout->Widgets Width' for details.

    // e.g. Use 2/3 of the space for widgets and 1/3 for labels (right align)
    //RainGui::PushItemWidth(-RainGui::GetWindowWidth() * 0.35f);

    // e.g. Leave a fixed amount of width for labels (by passing a negative value), the rest goes to widgets.
    RainGui::PushItemWidth(RainGui::GetFontSize() * -12);

    // Menu Bar
    if (RainGui::BeginMenuBar())
    {
        if (RainGui::BeginMenu("Menu"))
        {
            ShowExampleMenuFile();
            RainGui::EndMenu();
        }
        if (RainGui::BeginMenu("Examples"))
        {
            RainGui::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
            RainGui::MenuItem("Console", NULL, &show_app_console);
            RainGui::MenuItem("Log", NULL, &show_app_log);
            RainGui::MenuItem("Simple layout", NULL, &show_app_layout);
            RainGui::MenuItem("Property editor", NULL, &show_app_property_editor);
            RainGui::MenuItem("Long text display", NULL, &show_app_long_text);
            RainGui::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            RainGui::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
            RainGui::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            RainGui::MenuItem("Fullscreen window", NULL, &show_app_fullscreen);
            RainGui::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
            RainGui::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            RainGui::MenuItem("Documents", NULL, &show_app_documents);
            RainGui::EndMenu();
        }
        if (RainGui::BeginMenu("Tools"))
        {
            RainGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
            RainGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
            RainGui::MenuItem("About Dear RainGui", NULL, &show_app_about);
            RainGui::EndMenu();
        }
        RainGui::EndMenuBar();
    }

    RainGui::Text("dear raingui says hello. (%s)", RAINGUI_VERSION);
    RainGui::Spacing();

    if (RainGui::CollapsingHeader("Help"))
    {
        RainGui::Text("ABOUT THIS DEMO:");
        RainGui::BulletText("Sections below are demonstrating many aspects of the library.");
        RainGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
        RainGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
                          "and Metrics/Debugger (general purpose Dear RainGui debugging tool).");
        RainGui::Separator();

        RainGui::Text("PROGRAMMER GUIDE:");
        RainGui::BulletText("See the ShowDemoWindow() code in raingui_demo.cpp. <- you are here!");
        RainGui::BulletText("See comments in raingui.cpp.");
        RainGui::BulletText("See example applications in the examples/ folder.");
        RainGui::BulletText("Read the FAQ at http://www.dearraingui.org/faq/");
        RainGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        RainGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
        RainGui::Separator();

        RainGui::Text("USER GUIDE:");
        RainGui::ShowUserGuide();
    }

    if (RainGui::CollapsingHeader("Configuration"))
    {
        RainGuiIO& io = RainGui::GetIO();

        if (RainGui::TreeNode("Configuration##2"))
        {
            RainGui::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard",    &io.ConfigFlags, RainGuiConfigFlags_NavEnableKeyboard);
            RainGui::SameLine(); HelpMarker("Enable keyboard controls.");
            RainGui::CheckboxFlags("io.ConfigFlags: NavEnableGamepad",     &io.ConfigFlags, RainGuiConfigFlags_NavEnableGamepad);
            RainGui::SameLine(); HelpMarker("Enable gamepad controls. Require backend to set io.BackendFlags |= RainGuiBackendFlags_HasGamepad.\n\nRead instructions in raingui.cpp for details.");
            RainGui::CheckboxFlags("io.ConfigFlags: NavEnableSetMousePos", &io.ConfigFlags, RainGuiConfigFlags_NavEnableSetMousePos);
            RainGui::SameLine(); HelpMarker("Instruct navigation to move the mouse cursor. See comment for RainGuiConfigFlags_NavEnableSetMousePos.");
            RainGui::CheckboxFlags("io.ConfigFlags: NoMouse",              &io.ConfigFlags, RainGuiConfigFlags_NoMouse);
            if (io.ConfigFlags & RainGuiConfigFlags_NoMouse)
            {
                // The "NoMouse" option can get us stuck with a disabled mouse! Let's provide an alternative way to fix it:
                if (fmodf((float)RainGui::GetTime(), 0.40f) < 0.20f)
                {
                    RainGui::SameLine();
                    RainGui::Text("<<PRESS SPACE TO DISABLE>>");
                }
                if (RainGui::IsKeyPressed(RainGui::GetKeyIndex(RainGuiKey_Space)))
                    io.ConfigFlags &= ~RainGuiConfigFlags_NoMouse;
            }
            RainGui::CheckboxFlags("io.ConfigFlags: NoMouseCursorChange", &io.ConfigFlags, RainGuiConfigFlags_NoMouseCursorChange);
            RainGui::SameLine(); HelpMarker("Instruct backend to not alter mouse cursor shape and visibility.");
            RainGui::Checkbox("io.ConfigInputTextCursorBlink", &io.ConfigInputTextCursorBlink);
            RainGui::SameLine(); HelpMarker("Enable blinking cursor (optional as some users consider it to be distracting)");
            RainGui::Checkbox("io.ConfigDragClickToInputText", &io.ConfigDragClickToInputText);
            RainGui::SameLine(); HelpMarker("Enable turning DragXXX widgets into text input with a simple mouse click-release (without moving).");
            RainGui::Checkbox("io.ConfigWindowsResizeFromEdges", &io.ConfigWindowsResizeFromEdges);
            RainGui::SameLine(); HelpMarker("Enable resizing of windows from their edges and from the lower-left corner.\nThis requires (io.BackendFlags & RainGuiBackendFlags_HasMouseCursors) because it needs mouse cursor feedback.");
            RainGui::Checkbox("io.ConfigWindowsMoveFromTitleBarOnly", &io.ConfigWindowsMoveFromTitleBarOnly);
            RainGui::Checkbox("io.MouseDrawCursor", &io.MouseDrawCursor);
            RainGui::SameLine(); HelpMarker("Instruct Dear RainGui to render a mouse cursor itself. Note that a mouse cursor rendered via your application GPU rendering path will feel more laggy than hardware cursor, but will be more in sync with your other visuals.\n\nSome desktop applications may use both kinds of cursors (e.g. enable software cursor only when resizing/dragging something).");
            RainGui::Text("Also see Style->Rendering for rendering options.");
            RainGui::TreePop();
            RainGui::Separator();
        }

        if (RainGui::TreeNode("Backend Flags"))
        {
            HelpMarker(
                "Those flags are set by the backends (raingui_impl_xxx files) to specify their capabilities.\n"
                "Here we expose then as read-only fields to avoid breaking interactions with your backend.");

            // Make a local copy to avoid modifying actual backend flags.
            RainGuiBackendFlags backend_flags = io.BackendFlags;
            RainGui::CheckboxFlags("io.BackendFlags: HasGamepad",           &backend_flags, RainGuiBackendFlags_HasGamepad);
            RainGui::CheckboxFlags("io.BackendFlags: HasMouseCursors",      &backend_flags, RainGuiBackendFlags_HasMouseCursors);
            RainGui::CheckboxFlags("io.BackendFlags: HasSetMousePos",       &backend_flags, RainGuiBackendFlags_HasSetMousePos);
            RainGui::CheckboxFlags("io.BackendFlags: RendererHasVtxOffset", &backend_flags, RainGuiBackendFlags_RendererHasVtxOffset);
            RainGui::TreePop();
            RainGui::Separator();
        }

        if (RainGui::TreeNode("Style"))
        {
            HelpMarker("The same contents can be accessed in 'Tools->Style Editor' or by calling the ShowStyleEditor() function.");
            RainGui::ShowStyleEditor();
            RainGui::TreePop();
            RainGui::Separator();
        }

        if (RainGui::TreeNode("Capture/Logging"))
        {
            HelpMarker(
                "The logging API redirects all text output so you can easily capture the content of "
                "a window or a block. Tree nodes can be automatically expanded.\n"
                "Try opening any of the contents below in this window and then click one of the \"Log To\" button.");
            RainGui::LogButtons();

            HelpMarker("You can also call RainGui::LogText() to output directly to the log without a visual output.");
            if (RainGui::Button("Copy \"Hello, world!\" to clipboard"))
            {
                RainGui::LogToClipboard();
                RainGui::LogText("Hello, world!");
                RainGui::LogFinish();
            }
            RainGui::TreePop();
        }
    }

    if (RainGui::CollapsingHeader("Window options"))
    {
        if (RainGui::BeginTable("split", 3))
        {
            RainGui::TableNextColumn(); RainGui::Checkbox("No titlebar", &no_titlebar);
            RainGui::TableNextColumn(); RainGui::Checkbox("No scrollbar", &no_scrollbar);
            RainGui::TableNextColumn(); RainGui::Checkbox("No menu", &no_menu);
            RainGui::TableNextColumn(); RainGui::Checkbox("No move", &no_move);
            RainGui::TableNextColumn(); RainGui::Checkbox("No resize", &no_resize);
            RainGui::TableNextColumn(); RainGui::Checkbox("No collapse", &no_collapse);
            RainGui::TableNextColumn(); RainGui::Checkbox("No close", &no_close);
            RainGui::TableNextColumn(); RainGui::Checkbox("No nav", &no_nav);
            RainGui::TableNextColumn(); RainGui::Checkbox("No background", &no_background);
            RainGui::TableNextColumn(); RainGui::Checkbox("No bring to front", &no_bring_to_front);
            RainGui::EndTable();
        }
    }

    // All demo contents
    ShowDemoWindowWidgets();
    ShowDemoWindowLayout();
    ShowDemoWindowPopups();
    ShowDemoWindowTables();
    ShowDemoWindowMisc();

    // End of ShowDemoWindow()
    RainGui::PopItemWidth();
    RainGui::End();
}

static void ShowDemoWindowWidgets()
{
    if (!RainGui::CollapsingHeader("Widgets"))
        return;

    if (RainGui::TreeNode("Basic"))
    {
        static int clicked = 0;
        if (RainGui::Button("Button"))
            clicked++;
        if (clicked & 1)
        {
            RainGui::SameLine();
            RainGui::Text("Thanks for clicking me!");
        }

        static bool check = true;
        RainGui::Checkbox("checkbox", &check);

        static int e = 0;
        RainGui::RadioButton("radio a", &e, 0); RainGui::SameLine();
        RainGui::RadioButton("radio b", &e, 1); RainGui::SameLine();
        RainGui::RadioButton("radio c", &e, 2);

        // Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
        for (int i = 0; i < 7; i++)
        {
            if (i > 0)
                RainGui::SameLine();
            RainGui::PushID(i);
            RainGui::PushStyleColor(RainGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            RainGui::PushStyleColor(RainGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            RainGui::PushStyleColor(RainGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
            RainGui::Button("Click");
            RainGui::PopStyleColor(3);
            RainGui::PopID();
        }

        // Use AlignTextToFramePadding() to align text baseline to the baseline of framed widgets elements
        // (otherwise a Text+SameLine+Button sequence will have the text a little too high by default!)
        // See 'Demo->Layout->Text Baseline Alignment' for details.
        RainGui::AlignTextToFramePadding();
        RainGui::Text("Hold to repeat:");
        RainGui::SameLine();

        // Arrow buttons with Repeater
        static int counter = 0;
        float spacing = RainGui::GetStyle().ItemInnerSpacing.x;
        RainGui::PushButtonRepeat(true);
        if (RainGui::ArrowButton("##left", RainGuiDir_Left)) { counter--; }
        RainGui::SameLine(0.0f, spacing);
        if (RainGui::ArrowButton("##right", RainGuiDir_Right)) { counter++; }
        RainGui::PopButtonRepeat();
        RainGui::SameLine();
        RainGui::Text("%d", counter);

        RainGui::Text("Hover over me");
        if (RainGui::IsItemHovered())
            RainGui::SetTooltip("I am a tooltip");

        RainGui::SameLine();
        RainGui::Text("- or me");
        if (RainGui::IsItemHovered())
        {
            RainGui::BeginTooltip();
            RainGui::Text("I am a fancy tooltip");
            static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
            RainGui::PlotLines("Curve", arr, IM_ARRAYSIZE(arr));
            RainGui::EndTooltip();
        }

        RainGui::Separator();

        RainGui::LabelText("label", "Value");

        {
            // Using the _simplified_ one-liner Combo() api here
            // See "Combo" section for examples of how to use the more flexible BeginCombo()/EndCombo() api.
            const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIIIIII", "JJJJ", "KKKKKKK" };
            static int item_current = 0;
            RainGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
            RainGui::SameLine(); HelpMarker(
                "Using the simplified one-liner Combo API here.\nRefer to the \"Combo\" section below for an explanation of how to use the more flexible and general BeginCombo/EndCombo API.");
        }

        {
            // To wire InputText() with std::string or any other custom string type,
            // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/raingui_stdlib.h file.
            static char str0[128] = "Hello, world!";
            RainGui::InputText("input text", str0, IM_ARRAYSIZE(str0));
            RainGui::SameLine(); HelpMarker(
                "USER:\n"
                "Hold SHIFT or use mouse to select text.\n"
                "CTRL+Left/Right to word jump.\n"
                "CTRL+A or double-click to select all.\n"
                "CTRL+X,CTRL+C,CTRL+V clipboard.\n"
                "CTRL+Z,CTRL+Y undo/redo.\n"
                "ESCAPE to revert.\n\n"
                "PROGRAMMER:\n"
                "You can use the RainGuiInputTextFlags_CallbackResize facility if you need to wire InputText() "
                "to a dynamic string type. See misc/cpp/raingui_stdlib.h for an example (this is not demonstrated "
                "in raingui_demo.cpp).");

            static char str1[128] = "";
            RainGui::InputTextWithHint("input text (w/ hint)", "enter text here", str1, IM_ARRAYSIZE(str1));

            static int i0 = 123;
            RainGui::InputInt("input int", &i0);
            RainGui::SameLine(); HelpMarker(
                "You can apply arithmetic operators +,*,/ on numerical values.\n"
                "  e.g. [ 100 ], input \'*2\', result becomes [ 200 ]\n"
                "Use +- to subtract.");

            static float f0 = 0.001f;
            RainGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");

            static double d0 = 999999.00000001;
            RainGui::InputDouble("input double", &d0, 0.01f, 1.0f, "%.8f");

            static float f1 = 1.e10f;
            RainGui::InputFloat("input scientific", &f1, 0.0f, 0.0f, "%e");
            RainGui::SameLine(); HelpMarker(
                "You can input value using the scientific notation,\n"
                "  e.g. \"1e+8\" becomes \"100000000\".");

            static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            RainGui::InputFloat3("input float3", vec4a);
        }

        {
            static int i1 = 50, i2 = 42;
            RainGui::DragInt("drag int", &i1, 1);
            RainGui::SameLine(); HelpMarker(
                "Click and drag to edit value.\n"
                "Hold SHIFT/ALT for faster/slower edit.\n"
                "Double-click or CTRL+click to input value.");

            RainGui::DragInt("drag int 0..100", &i2, 1, 0, 100, "%d%%", RainGuiSliderFlags_AlwaysClamp);

            static float f1 = 1.00f, f2 = 0.0067f;
            RainGui::DragFloat("drag float", &f1, 0.005f);
            RainGui::DragFloat("drag small float", &f2, 0.0001f, 0.0f, 0.0f, "%.06f ns");
        }

        {
            static int i1 = 0;
            RainGui::SliderInt("slider int", &i1, -1, 3);
            RainGui::SameLine(); HelpMarker("CTRL+click to input value.");

            static float f1 = 0.123f, f2 = 0.0f;
            RainGui::SliderFloat("slider float", &f1, 0.0f, 1.0f, "ratio = %.3f");
            RainGui::SliderFloat("slider float (log)", &f2, -10.0f, 10.0f, "%.4f", RainGuiSliderFlags_Logarithmic);

            static float angle = 0.0f;
            RainGui::SliderAngle("slider angle", &angle);

            // Using the format string to display a name instead of an integer.
            // Here we completely omit '%d' from the format string, so it'll only display a name.
            // This technique can also be used with DragInt().
            enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };
            static int elem = Element_Fire;
            const char* elems_names[Element_COUNT] = { "Fire", "Earth", "Air", "Water" };
            const char* elem_name = (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
            RainGui::SliderInt("slider enum", &elem, 0, Element_COUNT - 1, elem_name);
            RainGui::SameLine(); HelpMarker("Using the format string parameter to display a name instead of the underlying integer.");
        }

        {
            static float col1[3] = { 1.0f, 0.0f, 0.2f };
            static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            RainGui::ColorEdit3("color 1", col1);
            RainGui::SameLine(); HelpMarker(
                "Click on the color square to open a color picker.\n"
                "Click and hold to use drag and drop.\n"
                "Right-click on the color square to show options.\n"
                "CTRL+click on individual component to input value.\n");

            RainGui::ColorEdit4("color 2", col2);
        }

        {
            // Using the _simplified_ one-liner ListBox() api here
            // See "List boxes" section for examples of how to use the more flexible BeginListBox()/EndListBox() api.
            const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
            static int item_current = 1;
            RainGui::ListBox("listbox", &item_current, items, IM_ARRAYSIZE(items), 4);
            RainGui::SameLine(); HelpMarker(
                "Using the simplified one-liner ListBox API here.\nRefer to the \"List boxes\" section below for an explanation of how to use the more flexible and general BeginListBox/EndListBox API.");
        }

        RainGui::TreePop();
    }

    // Testing RainGuiOnceUponAFrame helper.
    //static RainGuiOnceUponAFrame once;
    //for (int i = 0; i < 5; i++)
    //    if (once)
    //        RainGui::Text("This will be displayed only once.");

    if (RainGui::TreeNode("Trees"))
    {
        if (RainGui::TreeNode("Basic trees"))
        {
            for (int i = 0; i < 5; i++)
            {
                // Use SetNextItemOpen() so set the default state of a node to be open. We could
                // also use TreeNodeEx() with the RainGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
                if (i == 0)
                    RainGui::SetNextItemOpen(true, RainGuiCond_Once);

                if (RainGui::TreeNode((void*)(intptr_t)i, "Child %d", i))
                {
                    RainGui::Text("blah blah");
                    RainGui::SameLine();
                    if (RainGui::SmallButton("button")) {}
                    RainGui::TreePop();
                }
            }
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Advanced, with Selectable nodes"))
        {
            HelpMarker(
                "This is a more typical looking tree with selectable nodes.\n"
                "Click to select, CTRL+Click to toggle, click on arrows or double-click to open.");
            static RainGuiTreeNodeFlags base_flags = RainGuiTreeNodeFlags_OpenOnArrow | RainGuiTreeNodeFlags_OpenOnDoubleClick | RainGuiTreeNodeFlags_SpanAvailWidth;
            static bool align_label_with_current_x_position = false;
            static bool test_drag_and_drop = false;
            RainGui::CheckboxFlags("RainGuiTreeNodeFlags_OpenOnArrow",       &base_flags, RainGuiTreeNodeFlags_OpenOnArrow);
            RainGui::CheckboxFlags("RainGuiTreeNodeFlags_OpenOnDoubleClick", &base_flags, RainGuiTreeNodeFlags_OpenOnDoubleClick);
            RainGui::CheckboxFlags("RainGuiTreeNodeFlags_SpanAvailWidth",    &base_flags, RainGuiTreeNodeFlags_SpanAvailWidth); RainGui::SameLine(); HelpMarker("Extend hit area to all available width instead of allowing more items to be laid out after the node.");
            RainGui::CheckboxFlags("RainGuiTreeNodeFlags_SpanFullWidth",     &base_flags, RainGuiTreeNodeFlags_SpanFullWidth);
            RainGui::Checkbox("Align label with current X position", &align_label_with_current_x_position);
            RainGui::Checkbox("Test tree node as drag source", &test_drag_and_drop);
            RainGui::Text("Hello!");
            if (align_label_with_current_x_position)
                RainGui::Unindent(RainGui::GetTreeNodeToLabelSpacing());

            // 'selection_mask' is dumb representation of what may be user-side selection state.
            //  You may retain selection state inside or outside your objects in whatever format you see fit.
            // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
            /// of the loop. May be a pointer to your own node type, etc.
            static int selection_mask = (1 << 2);
            int node_clicked = -1;
            for (int i = 0; i < 6; i++)
            {
                // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
                RainGuiTreeNodeFlags node_flags = base_flags;
                const bool is_selected = (selection_mask & (1 << i)) != 0;
                if (is_selected)
                    node_flags |= RainGuiTreeNodeFlags_Selected;
                if (i < 3)
                {
                    // Items 0..2 are Tree Node
                    bool node_open = RainGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                    if (RainGui::IsItemClicked())
                        node_clicked = i;
                    if (test_drag_and_drop && RainGui::BeginDragDropSource())
                    {
                        RainGui::SetDragDropPayload("_TREENODE", NULL, 0);
                        RainGui::Text("This is a drag and drop source");
                        RainGui::EndDragDropSource();
                    }
                    if (node_open)
                    {
                        RainGui::BulletText("Blah blah\nBlah Blah");
                        RainGui::TreePop();
                    }
                }
                else
                {
                    // Items 3..5 are Tree Leaves
                    // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
                    // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
                    node_flags |= RainGuiTreeNodeFlags_Leaf | RainGuiTreeNodeFlags_NoTreePushOnOpen; // RainGuiTreeNodeFlags_Bullet
                    RainGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
                    if (RainGui::IsItemClicked())
                        node_clicked = i;
                    if (test_drag_and_drop && RainGui::BeginDragDropSource())
                    {
                        RainGui::SetDragDropPayload("_TREENODE", NULL, 0);
                        RainGui::Text("This is a drag and drop source");
                        RainGui::EndDragDropSource();
                    }
                }
            }
            if (node_clicked != -1)
            {
                // Update selection state
                // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                if (RainGui::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select
            }
            if (align_label_with_current_x_position)
                RainGui::Indent(RainGui::GetTreeNodeToLabelSpacing());
            RainGui::TreePop();
        }
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Collapsing Headers"))
    {
        static bool closable_group = true;
        RainGui::Checkbox("Show 2nd header", &closable_group);
        if (RainGui::CollapsingHeader("Header", RainGuiTreeNodeFlags_None))
        {
            RainGui::Text("IsItemHovered: %d", RainGui::IsItemHovered());
            for (int i = 0; i < 5; i++)
                RainGui::Text("Some content %d", i);
        }
        if (RainGui::CollapsingHeader("Header with a close button", &closable_group))
        {
            RainGui::Text("IsItemHovered: %d", RainGui::IsItemHovered());
            for (int i = 0; i < 5; i++)
                RainGui::Text("More content %d", i);
        }
        /*
        if (RainGui::CollapsingHeader("Header with a bullet", RainGuiTreeNodeFlags_Bullet))
            RainGui::Text("IsItemHovered: %d", RainGui::IsItemHovered());
        */
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Bullets"))
    {
        RainGui::BulletText("Bullet point 1");
        RainGui::BulletText("Bullet point 2\nOn multiple lines");
        if (RainGui::TreeNode("Tree node"))
        {
            RainGui::BulletText("Another bullet point");
            RainGui::TreePop();
        }
        RainGui::Bullet(); RainGui::Text("Bullet point 3 (two calls)");
        RainGui::Bullet(); RainGui::SmallButton("Button");
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Text"))
    {
        if (RainGui::TreeNode("Colorful Text"))
        {
            // Using shortcut. You can use PushStyleColor()/PopStyleColor() for more flexibility.
            RainGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Pink");
            RainGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Yellow");
            RainGui::TextDisabled("Disabled");
            RainGui::SameLine(); HelpMarker("The TextDisabled color is stored in RainGuiStyle.");
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Word Wrapping"))
        {
            // Using shortcut. You can use PushTextWrapPos()/PopTextWrapPos() for more flexibility.
            RainGui::TextWrapped(
                "This text should automatically wrap on the edge of the window. The current implementation "
                "for text wrapping follows simple rules suitable for English and possibly other languages.");
            RainGui::Spacing();

            static float wrap_width = 200.0f;
            RainGui::SliderFloat("Wrap width", &wrap_width, -20, 600, "%.0f");

            ImDrawList* draw_list = RainGui::GetWindowDrawList();
            for (int n = 0; n < 2; n++)
            {
                RainGui::Text("Test paragraph %d:", n);
                ImVec2 pos = RainGui::GetCursorScreenPos();
                ImVec2 marker_min = ImVec2(pos.x + wrap_width, pos.y);
                ImVec2 marker_max = ImVec2(pos.x + wrap_width + 10, pos.y + RainGui::GetTextLineHeight());
                RainGui::PushTextWrapPos(RainGui::GetCursorPos().x + wrap_width);
                if (n == 0)
                    RainGui::Text("The lazy dog is a good dog. This paragraph should fit within %.0f pixels. Testing a 1 character word. The quick brown fox jumps over the lazy dog.", wrap_width);
                else
                    RainGui::Text("aaaaaaaa bbbbbbbb, c cccccccc,dddddddd. d eeeeeeee   ffffffff. gggggggg!hhhhhhhh");

                // Draw actual text bounding box, following by marker of our expected limit (should not overlap!)
                draw_list->AddRect(RainGui::GetItemRectMin(), RainGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
                draw_list->AddRectFilled(marker_min, marker_max, IM_COL32(255, 0, 255, 255));
                RainGui::PopTextWrapPos();
            }

            RainGui::TreePop();
        }

        if (RainGui::TreeNode("UTF-8 Text"))
        {
            // UTF-8 test with Japanese characters
            // (Needs a suitable font? Try "Google Noto" or "Arial Unicode". See docs/FONTS.md for details.)
            // - From C++11 you can use the u8"my text" syntax to encode literal strings as UTF-8
            // - For earlier compiler, you may be able to encode your sources as UTF-8 (e.g. in Visual Studio, you
            //   can save your source files as 'UTF-8 without signature').
            // - FOR THIS DEMO FILE ONLY, BECAUSE WE WANT TO SUPPORT OLD COMPILERS, WE ARE *NOT* INCLUDING RAW UTF-8
            //   CHARACTERS IN THIS SOURCE FILE. Instead we are encoding a few strings with hexadecimal constants.
            //   Don't do this in your application! Please use u8"text in any language" in your application!
            // Note that characters values are preserved even by InputText() if the font cannot be displayed,
            // so you can safely copy & paste garbled characters into another application.
            RainGui::TextWrapped(
                "CJK text will only appears if the font was loaded with the appropriate CJK character ranges. "
                "Call io.Fonts->AddFontFromFileTTF() manually to load extra character ranges. "
                "Read docs/FONTS.md for details.");
            RainGui::Text("Hiragana: \xe3\x81\x8b\xe3\x81\x8d\xe3\x81\x8f\xe3\x81\x91\xe3\x81\x93 (kakikukeko)"); // Normally we would use u8"blah blah" with the proper characters directly in the string.
            RainGui::Text("Kanjis: \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e (nihongo)");
            static char buf[32] = "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
            //static char buf[32] = u8"NIHONGO"; // <- this is how you would write it with C++11, using real kanjis
            RainGui::InputText("UTF-8 input", buf, IM_ARRAYSIZE(buf));
            RainGui::TreePop();
        }
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Images"))
    {
        RainGuiIO& io = RainGui::GetIO();
        RainGui::TextWrapped(
            "Below we are displaying the font texture (which is the only texture we have access to in this demo). "
            "Use the 'ImTextureID' type as storage to pass pointers or identifier to your own texture data. "
            "Hover the texture for a zoomed view!");

        // Below we are displaying the font texture because it is the only texture we have access to inside the demo!
        // Remember that ImTextureID is just storage for whatever you want it to be. It is essentially a value that
        // will be passed to the rendering backend via the ImDrawCmd structure.
        // If you use one of the default raingui_impl_XXXX.cpp rendering backend, they all have comments at the top
        // of their respective source file to specify what they expect to be stored in ImTextureID, for example:
        // - The raingui_impl_dx11.cpp renderer expect a 'ID3D11ShaderResourceView*' pointer
        // - The raingui_impl_opengl3.cpp renderer expect a GLuint OpenGL texture identifier, etc.
        // More:
        // - If you decided that ImTextureID = MyEngineTexture*, then you can pass your MyEngineTexture* pointers
        //   to RainGui::Image(), and gather width/height through your own functions, etc.
        // - You can use ShowMetricsWindow() to inspect the draw data that are being passed to your renderer,
        //   it will help you debug issues if you are confused about it.
        // - Consider using the lower-level ImDrawList::AddImage() API, via RainGui::GetWindowDrawList()->AddImage().
        // - Read https://github.com/ocornut/raingui/blob/master/docs/FAQ.md
        // - Read https://github.com/ocornut/raingui/wiki/Image-Loading-and-Displaying-Examples
        ImTextureID my_tex_id = io.Fonts->TexID;
        float my_tex_w = (float)io.Fonts->TexWidth;
        float my_tex_h = (float)io.Fonts->TexHeight;
        {
            RainGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
            ImVec2 pos = RainGui::GetCursorScreenPos();
            ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
            ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
            ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
            RainGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            if (RainGui::IsItemHovered())
            {
                RainGui::BeginTooltip();
                float region_sz = 32.0f;
                float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
                float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
                float zoom = 4.0f;
                if (region_x < 0.0f) { region_x = 0.0f; }
                else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
                if (region_y < 0.0f) { region_y = 0.0f; }
                else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
                RainGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
                RainGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
                ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
                ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
                RainGui::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
                RainGui::EndTooltip();
            }
        }
        RainGui::TextWrapped("And now some textured buttons..");
        static int pressed_count = 0;
        for (int i = 0; i < 8; i++)
        {
            RainGui::PushID(i);
            int frame_padding = -1 + i;                             // -1 == uses default padding (style.FramePadding)
            ImVec2 size = ImVec2(32.0f, 32.0f);                     // Size of the image we want to make visible
            ImVec2 uv0 = ImVec2(0.0f, 0.0f);                        // UV coordinates for lower-left
            ImVec2 uv1 = ImVec2(32.0f / my_tex_w, 32.0f / my_tex_h);// UV coordinates for (32,32) in our texture
            ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint
            if (RainGui::ImageButton(my_tex_id, size, uv0, uv1, frame_padding, bg_col, tint_col))
                pressed_count += 1;
            RainGui::PopID();
            RainGui::SameLine();
        }
        RainGui::NewLine();
        RainGui::Text("Pressed %d times.", pressed_count);
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Combo"))
    {
        // Expose flags as checkbox for the demo
        static RainGuiComboFlags flags = 0;
        RainGui::CheckboxFlags("RainGuiComboFlags_PopupAlignLeft", &flags, RainGuiComboFlags_PopupAlignLeft);
        RainGui::SameLine(); HelpMarker("Only makes a difference if the popup is larger than the combo");
        if (RainGui::CheckboxFlags("RainGuiComboFlags_NoArrowButton", &flags, RainGuiComboFlags_NoArrowButton))
            flags &= ~RainGuiComboFlags_NoPreview;     // Clear the other flag, as we cannot combine both
        if (RainGui::CheckboxFlags("RainGuiComboFlags_NoPreview", &flags, RainGuiComboFlags_NoPreview))
            flags &= ~RainGuiComboFlags_NoArrowButton; // Clear the other flag, as we cannot combine both

        // Using the generic BeginCombo() API, you have full control over how to display the combo contents.
        // (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
        // stored in the object itself, etc.)
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static int item_current_idx = 0; // Here we store our selection data as an index.
        const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically it could be anything)
        if (RainGui::BeginCombo("combo 1", combo_label, flags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (RainGui::Selectable(items[n], is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    RainGui::SetItemDefaultFocus();
            }
            RainGui::EndCombo();
        }

        // Simplified one-liner Combo() API, using values packed in a single constant string
        static int item_current_2 = 0;
        RainGui::Combo("combo 2 (one-liner)", &item_current_2, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

        // Simplified one-liner Combo() using an array of const char*
        static int item_current_3 = -1; // If the selection isn't within 0..count, Combo won't display a preview
        RainGui::Combo("combo 3 (array)", &item_current_3, items, IM_ARRAYSIZE(items));

        // Simplified one-liner Combo() using an accessor function
        struct Funcs { static bool ItemGetter(void* data, int n, const char** out_str) { *out_str = ((const char**)data)[n]; return true; } };
        static int item_current_4 = 0;
        RainGui::Combo("combo 4 (function)", &item_current_4, &Funcs::ItemGetter, items, IM_ARRAYSIZE(items));

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("List boxes"))
    {
        // Using the generic BeginListBox() API, you have full control over how to display the combo contents.
        // (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
        // stored in the object itself, etc.)
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static int item_current_idx = 0; // Here we store our selection data as an index.
        if (RainGui::BeginListBox("listbox 1"))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (RainGui::Selectable(items[n], is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    RainGui::SetItemDefaultFocus();
            }
            RainGui::EndListBox();
        }

        // Custom size: use all width, 5 items tall
        RainGui::Text("Full-width:");
        if (RainGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 5 * RainGui::GetTextLineHeightWithSpacing())))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (RainGui::Selectable(items[n], is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    RainGui::SetItemDefaultFocus();
            }
            RainGui::EndListBox();
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Selectables"))
    {
        // Selectable() has 2 overloads:
        // - The one taking "bool selected" as a read-only selection information.
        //   When Selectable() has been clicked it returns true and you can alter selection state accordingly.
        // - The one taking "bool* p_selected" as a read-write selection information (convenient in some cases)
        // The earlier is more flexible, as in real application your selection may be stored in many different ways
        // and not necessarily inside a bool value (e.g. in flags within objects, as an external list, etc).
        if (RainGui::TreeNode("Basic"))
        {
            static bool selection[5] = { false, true, false, false, false };
            RainGui::Selectable("1. I am selectable", &selection[0]);
            RainGui::Selectable("2. I am selectable", &selection[1]);
            RainGui::Text("3. I am not selectable");
            RainGui::Selectable("4. I am selectable", &selection[3]);
            if (RainGui::Selectable("5. I am double clickable", selection[4], RainGuiSelectableFlags_AllowDoubleClick))
                if (RainGui::IsMouseDoubleClicked(0))
                    selection[4] = !selection[4];
            RainGui::TreePop();
        }
        if (RainGui::TreeNode("Selection State: Single Selection"))
        {
            static int selected = -1;
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (RainGui::Selectable(buf, selected == n))
                    selected = n;
            }
            RainGui::TreePop();
        }
        if (RainGui::TreeNode("Selection State: Multiple Selection"))
        {
            HelpMarker("Hold CTRL and click to select multiple items.");
            static bool selection[5] = { false, false, false, false, false };
            for (int n = 0; n < 5; n++)
            {
                char buf[32];
                sprintf(buf, "Object %d", n);
                if (RainGui::Selectable(buf, selection[n]))
                {
                    if (!RainGui::GetIO().KeyCtrl)    // Clear selection when CTRL is not held
                        memset(selection, 0, sizeof(selection));
                    selection[n] ^= 1;
                }
            }
            RainGui::TreePop();
        }
        if (RainGui::TreeNode("Rendering more text into the same line"))
        {
            // Using the Selectable() override that takes "bool* p_selected" parameter,
            // this function toggle your bool value automatically.
            static bool selected[3] = { false, false, false };
            RainGui::Selectable("main.c",    &selected[0]); RainGui::SameLine(300); RainGui::Text(" 2,345 bytes");
            RainGui::Selectable("Hello.cpp", &selected[1]); RainGui::SameLine(300); RainGui::Text("12,345 bytes");
            RainGui::Selectable("Hello.h",   &selected[2]); RainGui::SameLine(300); RainGui::Text(" 2,345 bytes");
            RainGui::TreePop();
        }
        if (RainGui::TreeNode("In columns"))
        {
            static bool selected[10] = {};

            if (RainGui::BeginTable("split1", 3, RainGuiTableFlags_Resizable | RainGuiTableFlags_NoSavedSettings))
            {
                for (int i = 0; i < 10; i++)
                {
                    char label[32];
                    sprintf(label, "Item %d", i);
                    RainGui::TableNextColumn();
                    RainGui::Selectable(label, &selected[i]); // FIXME-TABLE: Selection overlap
                }
                RainGui::EndTable();
            }
            RainGui::Separator();
            if (RainGui::BeginTable("split2", 3, RainGuiTableFlags_Resizable | RainGuiTableFlags_NoSavedSettings))
            {
                for (int i = 0; i < 10; i++)
                {
                    char label[32];
                    sprintf(label, "Item %d", i);
                    RainGui::TableNextRow();
                    RainGui::TableNextColumn();
                    RainGui::Selectable(label, &selected[i], RainGuiSelectableFlags_SpanAllColumns);
                    RainGui::TableNextColumn();
                    RainGui::Text("Some other contents");
                    RainGui::TableNextColumn();
                    RainGui::Text("123456");
                }
                RainGui::EndTable();
            }
            RainGui::TreePop();
        }
        if (RainGui::TreeNode("Grid"))
        {
            static char selected[4][4] = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

            // Add in a bit of silly fun...
            const float time = (float)RainGui::GetTime();
            const bool winning_state = memchr(selected, 0, sizeof(selected)) == NULL; // If all cells are selected...
            if (winning_state)
                RainGui::PushStyleVar(RainGuiStyleVar_SelectableTextAlign, ImVec2(0.5f + 0.5f * cosf(time * 2.0f), 0.5f + 0.5f * sinf(time * 3.0f)));

            for (int y = 0; y < 4; y++)
                for (int x = 0; x < 4; x++)
                {
                    if (x > 0)
                        RainGui::SameLine();
                    RainGui::PushID(y * 4 + x);
                    if (RainGui::Selectable("Sailor", selected[y][x] != 0, 0, ImVec2(50, 50)))
                    {
                        // Toggle clicked cell + toggle neighbors
                        selected[y][x] ^= 1;
                        if (x > 0) { selected[y][x - 1] ^= 1; }
                        if (x < 3) { selected[y][x + 1] ^= 1; }
                        if (y > 0) { selected[y - 1][x] ^= 1; }
                        if (y < 3) { selected[y + 1][x] ^= 1; }
                    }
                    RainGui::PopID();
                }

            if (winning_state)
                RainGui::PopStyleVar();
            RainGui::TreePop();
        }
        if (RainGui::TreeNode("Alignment"))
        {
            HelpMarker(
                "By default, Selectables uses style.SelectableTextAlign but it can be overridden on a per-item "
                "basis using PushStyleVar(). You'll probably want to always keep your default situation to "
                "left-align otherwise it becomes difficult to layout multiple items on a same line");
            static bool selected[3 * 3] = { true, false, true, false, true, false, true, false, true };
            for (int y = 0; y < 3; y++)
            {
                for (int x = 0; x < 3; x++)
                {
                    ImVec2 alignment = ImVec2((float)x / 2.0f, (float)y / 2.0f);
                    char name[32];
                    sprintf(name, "(%.1f,%.1f)", alignment.x, alignment.y);
                    if (x > 0) RainGui::SameLine();
                    RainGui::PushStyleVar(RainGuiStyleVar_SelectableTextAlign, alignment);
                    RainGui::Selectable(name, &selected[3 * y + x], RainGuiSelectableFlags_None, ImVec2(80, 80));
                    RainGui::PopStyleVar();
                }
            }
            RainGui::TreePop();
        }
        RainGui::TreePop();
    }

    // To wire InputText() with std::string or any other custom string type,
    // see the "Text Input > Resize Callback" section of this demo, and the misc/cpp/raingui_stdlib.h file.
    if (RainGui::TreeNode("Text Input"))
    {
        if (RainGui::TreeNode("Multi-line Text Input"))
        {
            // Note: we are using a fixed-sized buffer for simplicity here. See RainGuiInputTextFlags_CallbackResize
            // and the code in misc/cpp/raingui_stdlib.h for how to setup InputText() for dynamically resizing strings.
            static char text[1024 * 16] =
                "/*\n"
                " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
                " the hexadecimal encoding of one offending instruction,\n"
                " more formally, the invalid operand with locked CMPXCHG8B\n"
                " instruction bug, is a design flaw in the majority of\n"
                " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
                " processors (all in the P5 microarchitecture).\n"
                "*/\n\n"
                "label:\n"
                "\tlock cmpxchg8b eax\n";

            static RainGuiInputTextFlags flags = RainGuiInputTextFlags_AllowTabInput;
            HelpMarker("You can use the RainGuiInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/raingui_stdlib.h for an example. (This is not demonstrated in raingui_demo.cpp because we don't want to include <string> in here)");
            RainGui::CheckboxFlags("RainGuiInputTextFlags_ReadOnly", &flags, RainGuiInputTextFlags_ReadOnly);
            RainGui::CheckboxFlags("RainGuiInputTextFlags_AllowTabInput", &flags, RainGuiInputTextFlags_AllowTabInput);
            RainGui::CheckboxFlags("RainGuiInputTextFlags_CtrlEnterForNewLine", &flags, RainGuiInputTextFlags_CtrlEnterForNewLine);
            RainGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, RainGui::GetTextLineHeight() * 16), flags);
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Filtered Text Input"))
        {
            struct TextFilters
            {
                // Return 0 (pass) if the character is 'i' or 'm' or 'g' or 'u' or 'i'
                static int FilterRainGuiLetters(RainGuiInputTextCallbackData* data)
                {
                    if (data->EventChar < 256 && strchr("raingui", (char)data->EventChar))
                        return 0;
                    return 1;
                }
            };

            static char buf1[64] = ""; RainGui::InputText("default",     buf1, 64);
            static char buf2[64] = ""; RainGui::InputText("decimal",     buf2, 64, RainGuiInputTextFlags_CharsDecimal);
            static char buf3[64] = ""; RainGui::InputText("hexadecimal", buf3, 64, RainGuiInputTextFlags_CharsHexadecimal | RainGuiInputTextFlags_CharsUppercase);
            static char buf4[64] = ""; RainGui::InputText("uppercase",   buf4, 64, RainGuiInputTextFlags_CharsUppercase);
            static char buf5[64] = ""; RainGui::InputText("no blank",    buf5, 64, RainGuiInputTextFlags_CharsNoBlank);
            static char buf6[64] = ""; RainGui::InputText("\"raingui\" letters", buf6, 64, RainGuiInputTextFlags_CallbackCharFilter, TextFilters::FilterRainGuiLetters);
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Password Input"))
        {
            static char password[64] = "password123";
            RainGui::InputText("password", password, IM_ARRAYSIZE(password), RainGuiInputTextFlags_Password);
            RainGui::SameLine(); HelpMarker("Display all characters as '*'.\nDisable clipboard cut and copy.\nDisable logging.\n");
            RainGui::InputTextWithHint("password (w/ hint)", "<password>", password, IM_ARRAYSIZE(password), RainGuiInputTextFlags_Password);
            RainGui::InputText("password (clear)", password, IM_ARRAYSIZE(password));
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Completion, History, Edit Callbacks"))
        {
            struct Funcs
            {
                static int MyCallback(RainGuiInputTextCallbackData* data)
                {
                    if (data->EventFlag == RainGuiInputTextFlags_CallbackCompletion)
                    {
                        data->InsertChars(data->CursorPos, "..");
                    }
                    else if (data->EventFlag == RainGuiInputTextFlags_CallbackHistory)
                    {
                        if (data->EventKey == RainGuiKey_UpArrow)
                        {
                            data->DeleteChars(0, data->BufTextLen);
                            data->InsertChars(0, "Pressed Up!");
                            data->SelectAll();
                        }
                        else if (data->EventKey == RainGuiKey_DownArrow)
                        {
                            data->DeleteChars(0, data->BufTextLen);
                            data->InsertChars(0, "Pressed Down!");
                            data->SelectAll();
                        }
                    }
                    else if (data->EventFlag == RainGuiInputTextFlags_CallbackEdit)
                    {
                        // Toggle casing of first character
                        char c = data->Buf[0];
                        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) data->Buf[0] ^= 32;
                        data->BufDirty = true;

                        // Increment a counter
                        int* p_int = (int*)data->UserData;
                        *p_int = *p_int + 1;
                    }
                    return 0;
                }
            };
            static char buf1[64];
            RainGui::InputText("Completion", buf1, 64, RainGuiInputTextFlags_CallbackCompletion, Funcs::MyCallback);
            RainGui::SameLine(); HelpMarker("Here we append \"..\" each time Tab is pressed. See 'Examples>Console' for a more meaningful demonstration of using this callback.");

            static char buf2[64];
            RainGui::InputText("History", buf2, 64, RainGuiInputTextFlags_CallbackHistory, Funcs::MyCallback);
            RainGui::SameLine(); HelpMarker("Here we replace and select text each time Up/Down are pressed. See 'Examples>Console' for a more meaningful demonstration of using this callback.");

            static char buf3[64];
            static int edit_count = 0;
            RainGui::InputText("Edit", buf3, 64, RainGuiInputTextFlags_CallbackEdit, Funcs::MyCallback, (void*)&edit_count);
            RainGui::SameLine(); HelpMarker("Here we toggle the casing of the first character on every edits + count edits.");
            RainGui::SameLine(); RainGui::Text("(%d)", edit_count);

            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Resize Callback"))
        {
            // To wire InputText() with std::string or any other custom string type,
            // you can use the RainGuiInputTextFlags_CallbackResize flag + create a custom RainGui::InputText() wrapper
            // using your preferred type. See misc/cpp/raingui_stdlib.h for an implementation of this using std::string.
            HelpMarker(
                "Using RainGuiInputTextFlags_CallbackResize to wire your custom string type to InputText().\n\n"
                "See misc/cpp/raingui_stdlib.h for an implementation of this for std::string.");
            struct Funcs
            {
                static int MyResizeCallback(RainGuiInputTextCallbackData* data)
                {
                    if (data->EventFlag == RainGuiInputTextFlags_CallbackResize)
                    {
                        ImVector<char>* my_str = (ImVector<char>*)data->UserData;
                        IM_ASSERT(my_str->begin() == data->Buf);
                        my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
                        data->Buf = my_str->begin();
                    }
                    return 0;
                }

                // Note: Because RainGui:: is a namespace you would typically add your own function into the namespace.
                // For example, you code may declare a function 'RainGui::InputText(const char* label, MyString* my_str)'
                static bool MyInputTextMultiline(const char* label, ImVector<char>* my_str, const ImVec2& size = ImVec2(0, 0), RainGuiInputTextFlags flags = 0)
                {
                    IM_ASSERT((flags & RainGuiInputTextFlags_CallbackResize) == 0);
                    return RainGui::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | RainGuiInputTextFlags_CallbackResize, Funcs::MyResizeCallback, (void*)my_str);
                }
            };

            // For this demo we are using ImVector as a string container.
            // Note that because we need to store a terminating zero character, our size/capacity are 1 more
            // than usually reported by a typical string class.
            static ImVector<char> my_str;
            if (my_str.empty())
                my_str.push_back(0);
            Funcs::MyInputTextMultiline("##MyStr", &my_str, ImVec2(-FLT_MIN, RainGui::GetTextLineHeight() * 16));
            RainGui::Text("Data: %p\nSize: %d\nCapacity: %d", (void*)my_str.begin(), my_str.size(), my_str.capacity());
            RainGui::TreePop();
        }

        RainGui::TreePop();
    }

    // Tabs
    if (RainGui::TreeNode("Tabs"))
    {
        if (RainGui::TreeNode("Basic"))
        {
            RainGuiTabBarFlags tab_bar_flags = RainGuiTabBarFlags_None;
            if (RainGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (RainGui::BeginTabItem("Avocado"))
                {
                    RainGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
                    RainGui::EndTabItem();
                }
                if (RainGui::BeginTabItem("Broccoli"))
                {
                    RainGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                    RainGui::EndTabItem();
                }
                if (RainGui::BeginTabItem("Cucumber"))
                {
                    RainGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
                    RainGui::EndTabItem();
                }
                RainGui::EndTabBar();
            }
            RainGui::Separator();
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Advanced & Close Button"))
        {
            // Expose a couple of the available flags. In most cases you may just call BeginTabBar() with no flags (0).
            static RainGuiTabBarFlags tab_bar_flags = RainGuiTabBarFlags_Reorderable;
            RainGui::CheckboxFlags("RainGuiTabBarFlags_Reorderable", &tab_bar_flags, RainGuiTabBarFlags_Reorderable);
            RainGui::CheckboxFlags("RainGuiTabBarFlags_AutoSelectNewTabs", &tab_bar_flags, RainGuiTabBarFlags_AutoSelectNewTabs);
            RainGui::CheckboxFlags("RainGuiTabBarFlags_TabListPopupButton", &tab_bar_flags, RainGuiTabBarFlags_TabListPopupButton);
            RainGui::CheckboxFlags("RainGuiTabBarFlags_NoCloseWithMiddleMouseButton", &tab_bar_flags, RainGuiTabBarFlags_NoCloseWithMiddleMouseButton);
            if ((tab_bar_flags & RainGuiTabBarFlags_FittingPolicyMask_) == 0)
                tab_bar_flags |= RainGuiTabBarFlags_FittingPolicyDefault_;
            if (RainGui::CheckboxFlags("RainGuiTabBarFlags_FittingPolicyResizeDown", &tab_bar_flags, RainGuiTabBarFlags_FittingPolicyResizeDown))
                tab_bar_flags &= ~(RainGuiTabBarFlags_FittingPolicyMask_ ^ RainGuiTabBarFlags_FittingPolicyResizeDown);
            if (RainGui::CheckboxFlags("RainGuiTabBarFlags_FittingPolicyScroll", &tab_bar_flags, RainGuiTabBarFlags_FittingPolicyScroll))
                tab_bar_flags &= ~(RainGuiTabBarFlags_FittingPolicyMask_ ^ RainGuiTabBarFlags_FittingPolicyScroll);

            // Tab Bar
            const char* names[4] = { "Artichoke", "Beetroot", "Celery", "Daikon" };
            static bool opened[4] = { true, true, true, true }; // Persistent user state
            for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
            {
                if (n > 0) { RainGui::SameLine(); }
                RainGui::Checkbox(names[n], &opened[n]);
            }

            // Passing a bool* to BeginTabItem() is similar to passing one to Begin():
            // the underlying bool will be set to false when the tab is closed.
            if (RainGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(opened); n++)
                    if (opened[n] && RainGui::BeginTabItem(names[n], &opened[n], RainGuiTabItemFlags_None))
                    {
                        RainGui::Text("This is the %s tab!", names[n]);
                        if (n & 1)
                            RainGui::Text("I am an odd tab.");
                        RainGui::EndTabItem();
                    }
                RainGui::EndTabBar();
            }
            RainGui::Separator();
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("TabItemButton & Leading/Trailing flags"))
        {
            static ImVector<int> active_tabs;
            static int next_tab_id = 0;
            if (next_tab_id == 0) // Initialize with some default tabs
                for (int i = 0; i < 3; i++)
                    active_tabs.push_back(next_tab_id++);

            // TabItemButton() and Leading/Trailing flags are distinct features which we will demo together.
            // (It is possible to submit regular tabs with Leading/Trailing flags, or TabItemButton tabs without Leading/Trailing flags...
            // but they tend to make more sense together)
            static bool show_leading_button = true;
            static bool show_trailing_button = true;
            RainGui::Checkbox("Show Leading TabItemButton()", &show_leading_button);
            RainGui::Checkbox("Show Trailing TabItemButton()", &show_trailing_button);

            // Expose some other flags which are useful to showcase how they interact with Leading/Trailing tabs
            static RainGuiTabBarFlags tab_bar_flags = RainGuiTabBarFlags_AutoSelectNewTabs | RainGuiTabBarFlags_Reorderable | RainGuiTabBarFlags_FittingPolicyResizeDown;
            RainGui::CheckboxFlags("RainGuiTabBarFlags_TabListPopupButton", &tab_bar_flags, RainGuiTabBarFlags_TabListPopupButton);
            if (RainGui::CheckboxFlags("RainGuiTabBarFlags_FittingPolicyResizeDown", &tab_bar_flags, RainGuiTabBarFlags_FittingPolicyResizeDown))
                tab_bar_flags &= ~(RainGuiTabBarFlags_FittingPolicyMask_ ^ RainGuiTabBarFlags_FittingPolicyResizeDown);
            if (RainGui::CheckboxFlags("RainGuiTabBarFlags_FittingPolicyScroll", &tab_bar_flags, RainGuiTabBarFlags_FittingPolicyScroll))
                tab_bar_flags &= ~(RainGuiTabBarFlags_FittingPolicyMask_ ^ RainGuiTabBarFlags_FittingPolicyScroll);

            if (RainGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                // Demo a Leading TabItemButton(): click the "?" button to open a menu
                if (show_leading_button)
                    if (RainGui::TabItemButton("?", RainGuiTabItemFlags_Leading | RainGuiTabItemFlags_NoTooltip))
                        RainGui::OpenPopup("MyHelpMenu");
                if (RainGui::BeginPopup("MyHelpMenu"))
                {
                    RainGui::Selectable("Hello!");
                    RainGui::EndPopup();
                }

                // Demo Trailing Tabs: click the "+" button to add a new tab (in your app you may want to use a font icon instead of the "+")
                // Note that we submit it before the regular tabs, but because of the RainGuiTabItemFlags_Trailing flag it will always appear at the end.
                if (show_trailing_button)
                    if (RainGui::TabItemButton("+", RainGuiTabItemFlags_Trailing | RainGuiTabItemFlags_NoTooltip))
                        active_tabs.push_back(next_tab_id++); // Add new tab

                // Submit our regular tabs
                for (int n = 0; n < active_tabs.Size; )
                {
                    bool open = true;
                    char name[16];
                    snprintf(name, IM_ARRAYSIZE(name), "%04d", active_tabs[n]);
                    if (RainGui::BeginTabItem(name, &open, RainGuiTabItemFlags_None))
                    {
                        RainGui::Text("This is the %s tab!", name);
                        RainGui::EndTabItem();
                    }

                    if (!open)
                        active_tabs.erase(active_tabs.Data + n);
                    else
                        n++;
                }

                RainGui::EndTabBar();
            }
            RainGui::Separator();
            RainGui::TreePop();
        }
        RainGui::TreePop();
    }

    // Plot/Graph widgets are not very good.
    // Consider writing your own, or using a third-party one, see:
    // - ImPlot https://github.com/epezent/implot
    // - others https://github.com/ocornut/raingui/wiki/Useful-Extensions
    if (RainGui::TreeNode("Plots Widgets"))
    {
        static bool animate = true;
        RainGui::Checkbox("Animate", &animate);

        static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        RainGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

        // Fill an array of contiguous float values to plot
        // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float
        // and the sizeof() of your structure in the "stride" parameter.
        static float values[90] = {};
        static int values_offset = 0;
        static double refresh_time = 0.0;
        if (!animate || refresh_time == 0.0)
            refresh_time = RainGui::GetTime();
        while (refresh_time < RainGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
        {
            static float phase = 0.0f;
            values[values_offset] = cosf(phase);
            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            phase += 0.10f * values_offset;
            refresh_time += 1.0f / 60.0f;
        }

        // Plots can display overlay texts
        // (in this example, we will display an average value)
        {
            float average = 0.0f;
            for (int n = 0; n < IM_ARRAYSIZE(values); n++)
                average += values[n];
            average /= (float)IM_ARRAYSIZE(values);
            char overlay[32];
            sprintf(overlay, "avg %f", average);
            RainGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));
        }
        RainGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80.0f));

        // Use functions to generate output
        // FIXME: This is rather awkward because current plot API only pass in indices.
        // We probably want an API passing floats and user provide sample rate/count.
        struct Funcs
        {
            static float Sin(void*, int i) { return sinf(i * 0.1f); }
            static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
        };
        static int func_type = 0, display_count = 70;
        RainGui::Separator();
        RainGui::SetNextItemWidth(RainGui::GetFontSize() * 8);
        RainGui::Combo("func", &func_type, "Sin\0Saw\0");
        RainGui::SameLine();
        RainGui::SliderInt("Sample count", &display_count, 1, 400);
        float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
        RainGui::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        RainGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
        RainGui::Separator();

        // Animate a simple progress bar
        static float progress = 0.0f, progress_dir = 1.0f;
        if (animate)
        {
            progress += progress_dir * 0.4f * RainGui::GetIO().DeltaTime;
            if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
            if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
        }

        // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
        // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
        RainGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        RainGui::SameLine(0.0f, RainGui::GetStyle().ItemInnerSpacing.x);
        RainGui::Text("Progress Bar");

        float progress_saturated = IM_CLAMP(progress, 0.0f, 1.0f);
        char buf[32];
        sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
        RainGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Color/Picker Widgets"))
    {
        static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        RainGui::Checkbox("With Alpha Preview", &alpha_preview);
        RainGui::Checkbox("With Half Alpha Preview", &alpha_half_preview);
        RainGui::Checkbox("With Drag and Drop", &drag_and_drop);
        RainGui::Checkbox("With Options Menu", &options_menu); RainGui::SameLine(); HelpMarker("Right-click on the individual color widget to show options.");
        RainGui::Checkbox("With HDR", &hdr); RainGui::SameLine(); HelpMarker("Currently all this does is to lift the 0..1 limits on dragging widgets.");
        RainGuiColorEditFlags misc_flags = (hdr ? RainGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : RainGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? RainGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? RainGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : RainGuiColorEditFlags_NoOptions);

        RainGui::Text("Color widget:");
        RainGui::SameLine(); HelpMarker(
            "Click on the color square to open a color picker.\n"
            "CTRL+click on individual component to input value.\n");
        RainGui::ColorEdit3("MyColor##1", (float*)&color, misc_flags);

        RainGui::Text("Color widget HSV with Alpha:");
        RainGui::ColorEdit4("MyColor##2", (float*)&color, RainGuiColorEditFlags_DisplayHSV | misc_flags);

        RainGui::Text("Color widget with Float Display:");
        RainGui::ColorEdit4("MyColor##2f", (float*)&color, RainGuiColorEditFlags_Float | misc_flags);

        RainGui::Text("Color button with Picker:");
        RainGui::SameLine(); HelpMarker(
            "With the RainGuiColorEditFlags_NoInputs flag you can hide all the slider/text inputs.\n"
            "With the RainGuiColorEditFlags_NoLabel flag you can pass a non-empty label which will only "
            "be used for the tooltip and picker popup.");
        RainGui::ColorEdit4("MyColor##3", (float*)&color, RainGuiColorEditFlags_NoInputs | RainGuiColorEditFlags_NoLabel | misc_flags);

        RainGui::Text("Color button with Custom Picker Popup:");

        // Generate a default palette. The palette will persist and can be edited.
        static bool saved_palette_init = true;
        static ImVec4 saved_palette[32] = {};
        if (saved_palette_init)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                RainGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                    saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
            saved_palette_init = false;
        }

        static ImVec4 backup_color;
        bool open_popup = RainGui::ColorButton("MyColor##3b", color, misc_flags);
        RainGui::SameLine(0, RainGui::GetStyle().ItemInnerSpacing.x);
        open_popup |= RainGui::Button("Palette");
        if (open_popup)
        {
            RainGui::OpenPopup("mypicker");
            backup_color = color;
        }
        if (RainGui::BeginPopup("mypicker"))
        {
            RainGui::Text("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
            RainGui::Separator();
            RainGui::ColorPicker4("##picker", (float*)&color, misc_flags | RainGuiColorEditFlags_NoSidePreview | RainGuiColorEditFlags_NoSmallPreview);
            RainGui::SameLine();

            RainGui::BeginGroup(); // Lock X position
            RainGui::Text("Current");
            RainGui::ColorButton("##current", color, RainGuiColorEditFlags_NoPicker | RainGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
            RainGui::Text("Previous");
            if (RainGui::ColorButton("##previous", backup_color, RainGuiColorEditFlags_NoPicker | RainGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                color = backup_color;
            RainGui::Separator();
            RainGui::Text("Palette");
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                RainGui::PushID(n);
                if ((n % 8) != 0)
                    RainGui::SameLine(0.0f, RainGui::GetStyle().ItemSpacing.y);

                RainGuiColorEditFlags palette_button_flags = RainGuiColorEditFlags_NoAlpha | RainGuiColorEditFlags_NoPicker | RainGuiColorEditFlags_NoTooltip;
                if (RainGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
                    color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

                // Allow user to drop colors into each palette entry. Note that ColorButton() is already a
                // drag source by default, unless specifying the RainGuiColorEditFlags_NoDragDrop flag.
                if (RainGui::BeginDragDropTarget())
                {
                    if (const RainGuiPayload* payload = RainGui::AcceptDragDropPayload(RAINGUI_PAYLOAD_TYPE_COLOR_3F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                    if (const RainGuiPayload* payload = RainGui::AcceptDragDropPayload(RAINGUI_PAYLOAD_TYPE_COLOR_4F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                    RainGui::EndDragDropTarget();
                }

                RainGui::PopID();
            }
            RainGui::EndGroup();
            RainGui::EndPopup();
        }

        RainGui::Text("Color button only:");
        static bool no_border = false;
        RainGui::Checkbox("RainGuiColorEditFlags_NoBorder", &no_border);
        RainGui::ColorButton("MyColor##3c", *(ImVec4*)&color, misc_flags | (no_border ? RainGuiColorEditFlags_NoBorder : 0), ImVec2(80, 80));

        RainGui::Text("Color picker:");
        static bool alpha = true;
        static bool alpha_bar = true;
        static bool side_preview = true;
        static bool ref_color = false;
        static ImVec4 ref_color_v(1.0f, 0.0f, 1.0f, 0.5f);
        static int display_mode = 0;
        static int picker_mode = 0;
        RainGui::Checkbox("With Alpha", &alpha);
        RainGui::Checkbox("With Alpha Bar", &alpha_bar);
        RainGui::Checkbox("With Side Preview", &side_preview);
        if (side_preview)
        {
            RainGui::SameLine();
            RainGui::Checkbox("With Ref Color", &ref_color);
            if (ref_color)
            {
                RainGui::SameLine();
                RainGui::ColorEdit4("##RefColor", &ref_color_v.x, RainGuiColorEditFlags_NoInputs | misc_flags);
            }
        }
        RainGui::Combo("Display Mode", &display_mode, "Auto/Current\0None\0RGB Only\0HSV Only\0Hex Only\0");
        RainGui::SameLine(); HelpMarker(
            "ColorEdit defaults to displaying RGB inputs if you don't specify a display mode, "
            "but the user can change it with a right-click.\n\nColorPicker defaults to displaying RGB+HSV+Hex "
            "if you don't specify a display mode.\n\nYou can change the defaults using SetColorEditOptions().");
        RainGui::Combo("Picker Mode", &picker_mode, "Auto/Current\0Hue bar + SV rect\0Hue wheel + SV triangle\0");
        RainGui::SameLine(); HelpMarker("User can right-click the picker to change mode.");
        RainGuiColorEditFlags flags = misc_flags;
        if (!alpha)            flags |= RainGuiColorEditFlags_NoAlpha;        // This is by default if you call ColorPicker3() instead of ColorPicker4()
        if (alpha_bar)         flags |= RainGuiColorEditFlags_AlphaBar;
        if (!side_preview)     flags |= RainGuiColorEditFlags_NoSidePreview;
        if (picker_mode == 1)  flags |= RainGuiColorEditFlags_PickerHueBar;
        if (picker_mode == 2)  flags |= RainGuiColorEditFlags_PickerHueWheel;
        if (display_mode == 1) flags |= RainGuiColorEditFlags_NoInputs;       // Disable all RGB/HSV/Hex displays
        if (display_mode == 2) flags |= RainGuiColorEditFlags_DisplayRGB;     // Override display mode
        if (display_mode == 3) flags |= RainGuiColorEditFlags_DisplayHSV;
        if (display_mode == 4) flags |= RainGuiColorEditFlags_DisplayHex;
        RainGui::ColorPicker4("MyColor##4", (float*)&color, flags, ref_color ? &ref_color_v.x : NULL);

        RainGui::Text("Set defaults in code:");
        RainGui::SameLine(); HelpMarker(
            "SetColorEditOptions() is designed to allow you to set boot-time default.\n"
            "We don't have Push/Pop functions because you can force options on a per-widget basis if needed,"
            "and the user can change non-forced ones with the options menu.\nWe don't have a getter to avoid"
            "encouraging you to persistently save values that aren't forward-compatible.");
        if (RainGui::Button("Default: Uint8 + HSV + Hue Bar"))
            RainGui::SetColorEditOptions(RainGuiColorEditFlags_Uint8 | RainGuiColorEditFlags_DisplayHSV | RainGuiColorEditFlags_PickerHueBar);
        if (RainGui::Button("Default: Float + HDR + Hue Wheel"))
            RainGui::SetColorEditOptions(RainGuiColorEditFlags_Float | RainGuiColorEditFlags_HDR | RainGuiColorEditFlags_PickerHueWheel);

        // HSV encoded support (to avoid RGB<>HSV round trips and singularities when S==0 or V==0)
        static ImVec4 color_hsv(0.23f, 1.0f, 1.0f, 1.0f); // Stored as HSV!
        RainGui::Spacing();
        RainGui::Text("HSV encoded colors");
        RainGui::SameLine(); HelpMarker(
            "By default, colors are given to ColorEdit and ColorPicker in RGB, but RainGuiColorEditFlags_InputHSV"
            "allows you to store colors as HSV and pass them to ColorEdit and ColorPicker as HSV. This comes with the"
            "added benefit that you can manipulate hue values with the picker even when saturation or value are zero.");
        RainGui::Text("Color widget with InputHSV:");
        RainGui::ColorEdit4("HSV shown as RGB##1", (float*)&color_hsv, RainGuiColorEditFlags_DisplayRGB | RainGuiColorEditFlags_InputHSV | RainGuiColorEditFlags_Float);
        RainGui::ColorEdit4("HSV shown as HSV##1", (float*)&color_hsv, RainGuiColorEditFlags_DisplayHSV | RainGuiColorEditFlags_InputHSV | RainGuiColorEditFlags_Float);
        RainGui::DragFloat4("Raw HSV values", (float*)&color_hsv, 0.01f, 0.0f, 1.0f);

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Drag/Slider Flags"))
    {
        // Demonstrate using advanced flags for DragXXX and SliderXXX functions. Note that the flags are the same!
        static RainGuiSliderFlags flags = RainGuiSliderFlags_None;
        RainGui::CheckboxFlags("RainGuiSliderFlags_AlwaysClamp", &flags, RainGuiSliderFlags_AlwaysClamp);
        RainGui::SameLine(); HelpMarker("Always clamp value to min/max bounds (if any) when input manually with CTRL+Click.");
        RainGui::CheckboxFlags("RainGuiSliderFlags_Logarithmic", &flags, RainGuiSliderFlags_Logarithmic);
        RainGui::SameLine(); HelpMarker("Enable logarithmic editing (more precision for small values).");
        RainGui::CheckboxFlags("RainGuiSliderFlags_NoRoundToFormat", &flags, RainGuiSliderFlags_NoRoundToFormat);
        RainGui::SameLine(); HelpMarker("Disable rounding underlying value to match precision of the format string (e.g. %.3f values are rounded to those 3 digits).");
        RainGui::CheckboxFlags("RainGuiSliderFlags_NoInput", &flags, RainGuiSliderFlags_NoInput);
        RainGui::SameLine(); HelpMarker("Disable CTRL+Click or Enter key allowing to input text directly into the widget.");

        // Drags
        static float drag_f = 0.5f;
        static int drag_i = 50;
        RainGui::Text("Underlying float value: %f", drag_f);
        RainGui::DragFloat("DragFloat (0 -> 1)", &drag_f, 0.005f, 0.0f, 1.0f, "%.3f", flags);
        RainGui::DragFloat("DragFloat (0 -> +inf)", &drag_f, 0.005f, 0.0f, FLT_MAX, "%.3f", flags);
        RainGui::DragFloat("DragFloat (-inf -> 1)", &drag_f, 0.005f, -FLT_MAX, 1.0f, "%.3f", flags);
        RainGui::DragFloat("DragFloat (-inf -> +inf)", &drag_f, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
        RainGui::DragInt("DragInt (0 -> 100)", &drag_i, 0.5f, 0, 100, "%d", flags);

        // Sliders
        static float slider_f = 0.5f;
        static int slider_i = 50;
        RainGui::Text("Underlying float value: %f", slider_f);
        RainGui::SliderFloat("SliderFloat (0 -> 1)", &slider_f, 0.0f, 1.0f, "%.3f", flags);
        RainGui::SliderInt("SliderInt (0 -> 100)", &slider_i, 0, 100, "%d", flags);

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Range Widgets"))
    {
        static float begin = 10, end = 90;
        static int begin_i = 100, end_i = 1000;
        RainGui::DragFloatRange2("range float", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%", RainGuiSliderFlags_AlwaysClamp);
        RainGui::DragIntRange2("range int", &begin_i, &end_i, 5, 0, 1000, "Min: %d units", "Max: %d units");
        RainGui::DragIntRange2("range int (no bounds)", &begin_i, &end_i, 5, 0, 0, "Min: %d units", "Max: %d units");
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Data Types"))
    {
        // DragScalar/InputScalar/SliderScalar functions allow various data types
        // - signed/unsigned
        // - 8/16/32/64-bits
        // - integer/float/double
        // To avoid polluting the public API with all possible combinations, we use the RainGuiDataType enum
        // to pass the type, and passing all arguments by pointer.
        // This is the reason the test code below creates local variables to hold "zero" "one" etc. for each types.
        // In practice, if you frequently use a given type that is not covered by the normal API entry points,
        // you can wrap it yourself inside a 1 line function which can take typed argument as value instead of void*,
        // and then pass their address to the generic function. For example:
        //   bool MySliderU64(const char *label, u64* value, u64 min = 0, u64 max = 0, const char* format = "%lld")
        //   {
        //      return SliderScalar(label, RainGuiDataType_U64, value, &min, &max, format);
        //   }

        // Setup limits (as helper variables so we can take their address, as explained above)
        // Note: SliderScalar() functions have a maximum usable range of half the natural type maximum, hence the /2.
        #ifndef LLONG_MIN
        ImS64 LLONG_MIN = -9223372036854775807LL - 1;
        ImS64 LLONG_MAX = 9223372036854775807LL;
        ImU64 ULLONG_MAX = (2ULL * 9223372036854775807LL + 1);
        #endif
        const char    s8_zero  = 0,   s8_one  = 1,   s8_fifty  = 50, s8_min  = -128,        s8_max = 127;
        const ImU8    u8_zero  = 0,   u8_one  = 1,   u8_fifty  = 50, u8_min  = 0,           u8_max = 255;
        const short   s16_zero = 0,   s16_one = 1,   s16_fifty = 50, s16_min = -32768,      s16_max = 32767;
        const ImU16   u16_zero = 0,   u16_one = 1,   u16_fifty = 50, u16_min = 0,           u16_max = 65535;
        const ImS32   s32_zero = 0,   s32_one = 1,   s32_fifty = 50, s32_min = INT_MIN/2,   s32_max = INT_MAX/2,    s32_hi_a = INT_MAX/2 - 100,    s32_hi_b = INT_MAX/2;
        const ImU32   u32_zero = 0,   u32_one = 1,   u32_fifty = 50, u32_min = 0,           u32_max = UINT_MAX/2,   u32_hi_a = UINT_MAX/2 - 100,   u32_hi_b = UINT_MAX/2;
        const ImS64   s64_zero = 0,   s64_one = 1,   s64_fifty = 50, s64_min = LLONG_MIN/2, s64_max = LLONG_MAX/2,  s64_hi_a = LLONG_MAX/2 - 100,  s64_hi_b = LLONG_MAX/2;
        const ImU64   u64_zero = 0,   u64_one = 1,   u64_fifty = 50, u64_min = 0,           u64_max = ULLONG_MAX/2, u64_hi_a = ULLONG_MAX/2 - 100, u64_hi_b = ULLONG_MAX/2;
        const float   f32_zero = 0.f, f32_one = 1.f, f32_lo_a = -10000000000.0f, f32_hi_a = +10000000000.0f;
        const double  f64_zero = 0.,  f64_one = 1.,  f64_lo_a = -1000000000000000.0, f64_hi_a = +1000000000000000.0;

        // State
        static char   s8_v  = 127;
        static ImU8   u8_v  = 255;
        static short  s16_v = 32767;
        static ImU16  u16_v = 65535;
        static ImS32  s32_v = -1;
        static ImU32  u32_v = (ImU32)-1;
        static ImS64  s64_v = -1;
        static ImU64  u64_v = (ImU64)-1;
        static float  f32_v = 0.123f;
        static double f64_v = 90000.01234567890123456789;

        const float drag_speed = 0.2f;
        static bool drag_clamp = false;
        RainGui::Text("Drags:");
        RainGui::Checkbox("Clamp integers to 0..50", &drag_clamp);
        RainGui::SameLine(); HelpMarker(
            "As with every widgets in dear raingui, we never modify values unless there is a user interaction.\n"
            "You can override the clamping limits by using CTRL+Click to input a value.");
        RainGui::DragScalar("drag s8",        RainGuiDataType_S8,     &s8_v,  drag_speed, drag_clamp ? &s8_zero  : NULL, drag_clamp ? &s8_fifty  : NULL);
        RainGui::DragScalar("drag u8",        RainGuiDataType_U8,     &u8_v,  drag_speed, drag_clamp ? &u8_zero  : NULL, drag_clamp ? &u8_fifty  : NULL, "%u ms");
        RainGui::DragScalar("drag s16",       RainGuiDataType_S16,    &s16_v, drag_speed, drag_clamp ? &s16_zero : NULL, drag_clamp ? &s16_fifty : NULL);
        RainGui::DragScalar("drag u16",       RainGuiDataType_U16,    &u16_v, drag_speed, drag_clamp ? &u16_zero : NULL, drag_clamp ? &u16_fifty : NULL, "%u ms");
        RainGui::DragScalar("drag s32",       RainGuiDataType_S32,    &s32_v, drag_speed, drag_clamp ? &s32_zero : NULL, drag_clamp ? &s32_fifty : NULL);
        RainGui::DragScalar("drag u32",       RainGuiDataType_U32,    &u32_v, drag_speed, drag_clamp ? &u32_zero : NULL, drag_clamp ? &u32_fifty : NULL, "%u ms");
        RainGui::DragScalar("drag s64",       RainGuiDataType_S64,    &s64_v, drag_speed, drag_clamp ? &s64_zero : NULL, drag_clamp ? &s64_fifty : NULL);
        RainGui::DragScalar("drag u64",       RainGuiDataType_U64,    &u64_v, drag_speed, drag_clamp ? &u64_zero : NULL, drag_clamp ? &u64_fifty : NULL);
        RainGui::DragScalar("drag float",     RainGuiDataType_Float,  &f32_v, 0.005f,  &f32_zero, &f32_one, "%f");
        RainGui::DragScalar("drag float log", RainGuiDataType_Float,  &f32_v, 0.005f,  &f32_zero, &f32_one, "%f", RainGuiSliderFlags_Logarithmic);
        RainGui::DragScalar("drag double",    RainGuiDataType_Double, &f64_v, 0.0005f, &f64_zero, NULL,     "%.10f grams");
        RainGui::DragScalar("drag double log",RainGuiDataType_Double, &f64_v, 0.0005f, &f64_zero, &f64_one, "0 < %.10f < 1", RainGuiSliderFlags_Logarithmic);

        RainGui::Text("Sliders");
        RainGui::SliderScalar("slider s8 full",       RainGuiDataType_S8,     &s8_v,  &s8_min,   &s8_max,   "%d");
        RainGui::SliderScalar("slider u8 full",       RainGuiDataType_U8,     &u8_v,  &u8_min,   &u8_max,   "%u");
        RainGui::SliderScalar("slider s16 full",      RainGuiDataType_S16,    &s16_v, &s16_min,  &s16_max,  "%d");
        RainGui::SliderScalar("slider u16 full",      RainGuiDataType_U16,    &u16_v, &u16_min,  &u16_max,  "%u");
        RainGui::SliderScalar("slider s32 low",       RainGuiDataType_S32,    &s32_v, &s32_zero, &s32_fifty,"%d");
        RainGui::SliderScalar("slider s32 high",      RainGuiDataType_S32,    &s32_v, &s32_hi_a, &s32_hi_b, "%d");
        RainGui::SliderScalar("slider s32 full",      RainGuiDataType_S32,    &s32_v, &s32_min,  &s32_max,  "%d");
        RainGui::SliderScalar("slider u32 low",       RainGuiDataType_U32,    &u32_v, &u32_zero, &u32_fifty,"%u");
        RainGui::SliderScalar("slider u32 high",      RainGuiDataType_U32,    &u32_v, &u32_hi_a, &u32_hi_b, "%u");
        RainGui::SliderScalar("slider u32 full",      RainGuiDataType_U32,    &u32_v, &u32_min,  &u32_max,  "%u");
        RainGui::SliderScalar("slider s64 low",       RainGuiDataType_S64,    &s64_v, &s64_zero, &s64_fifty,"%" IM_PRId64);
        RainGui::SliderScalar("slider s64 high",      RainGuiDataType_S64,    &s64_v, &s64_hi_a, &s64_hi_b, "%" IM_PRId64);
        RainGui::SliderScalar("slider s64 full",      RainGuiDataType_S64,    &s64_v, &s64_min,  &s64_max,  "%" IM_PRId64);
        RainGui::SliderScalar("slider u64 low",       RainGuiDataType_U64,    &u64_v, &u64_zero, &u64_fifty,"%" IM_PRIu64 " ms");
        RainGui::SliderScalar("slider u64 high",      RainGuiDataType_U64,    &u64_v, &u64_hi_a, &u64_hi_b, "%" IM_PRIu64 " ms");
        RainGui::SliderScalar("slider u64 full",      RainGuiDataType_U64,    &u64_v, &u64_min,  &u64_max,  "%" IM_PRIu64 " ms");
        RainGui::SliderScalar("slider float low",     RainGuiDataType_Float,  &f32_v, &f32_zero, &f32_one);
        RainGui::SliderScalar("slider float low log", RainGuiDataType_Float,  &f32_v, &f32_zero, &f32_one,  "%.10f", RainGuiSliderFlags_Logarithmic);
        RainGui::SliderScalar("slider float high",    RainGuiDataType_Float,  &f32_v, &f32_lo_a, &f32_hi_a, "%e");
        RainGui::SliderScalar("slider double low",    RainGuiDataType_Double, &f64_v, &f64_zero, &f64_one,  "%.10f grams");
        RainGui::SliderScalar("slider double low log",RainGuiDataType_Double, &f64_v, &f64_zero, &f64_one,  "%.10f", RainGuiSliderFlags_Logarithmic);
        RainGui::SliderScalar("slider double high",   RainGuiDataType_Double, &f64_v, &f64_lo_a, &f64_hi_a, "%e grams");

        RainGui::Text("Sliders (reverse)");
        RainGui::SliderScalar("slider s8 reverse",    RainGuiDataType_S8,   &s8_v,  &s8_max,    &s8_min,   "%d");
        RainGui::SliderScalar("slider u8 reverse",    RainGuiDataType_U8,   &u8_v,  &u8_max,    &u8_min,   "%u");
        RainGui::SliderScalar("slider s32 reverse",   RainGuiDataType_S32,  &s32_v, &s32_fifty, &s32_zero, "%d");
        RainGui::SliderScalar("slider u32 reverse",   RainGuiDataType_U32,  &u32_v, &u32_fifty, &u32_zero, "%u");
        RainGui::SliderScalar("slider s64 reverse",   RainGuiDataType_S64,  &s64_v, &s64_fifty, &s64_zero, "%" IM_PRId64);
        RainGui::SliderScalar("slider u64 reverse",   RainGuiDataType_U64,  &u64_v, &u64_fifty, &u64_zero, "%" IM_PRIu64 " ms");

        static bool inputs_step = true;
        RainGui::Text("Inputs");
        RainGui::Checkbox("Show step buttons", &inputs_step);
        RainGui::InputScalar("input s8",      RainGuiDataType_S8,     &s8_v,  inputs_step ? &s8_one  : NULL, NULL, "%d");
        RainGui::InputScalar("input u8",      RainGuiDataType_U8,     &u8_v,  inputs_step ? &u8_one  : NULL, NULL, "%u");
        RainGui::InputScalar("input s16",     RainGuiDataType_S16,    &s16_v, inputs_step ? &s16_one : NULL, NULL, "%d");
        RainGui::InputScalar("input u16",     RainGuiDataType_U16,    &u16_v, inputs_step ? &u16_one : NULL, NULL, "%u");
        RainGui::InputScalar("input s32",     RainGuiDataType_S32,    &s32_v, inputs_step ? &s32_one : NULL, NULL, "%d");
        RainGui::InputScalar("input s32 hex", RainGuiDataType_S32,    &s32_v, inputs_step ? &s32_one : NULL, NULL, "%08X", RainGuiInputTextFlags_CharsHexadecimal);
        RainGui::InputScalar("input u32",     RainGuiDataType_U32,    &u32_v, inputs_step ? &u32_one : NULL, NULL, "%u");
        RainGui::InputScalar("input u32 hex", RainGuiDataType_U32,    &u32_v, inputs_step ? &u32_one : NULL, NULL, "%08X", RainGuiInputTextFlags_CharsHexadecimal);
        RainGui::InputScalar("input s64",     RainGuiDataType_S64,    &s64_v, inputs_step ? &s64_one : NULL);
        RainGui::InputScalar("input u64",     RainGuiDataType_U64,    &u64_v, inputs_step ? &u64_one : NULL);
        RainGui::InputScalar("input float",   RainGuiDataType_Float,  &f32_v, inputs_step ? &f32_one : NULL);
        RainGui::InputScalar("input double",  RainGuiDataType_Double, &f64_v, inputs_step ? &f64_one : NULL);

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Multi-component Widgets"))
    {
        static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
        static int vec4i[4] = { 1, 5, 100, 255 };

        RainGui::InputFloat2("input float2", vec4f);
        RainGui::DragFloat2("drag float2", vec4f, 0.01f, 0.0f, 1.0f);
        RainGui::SliderFloat2("slider float2", vec4f, 0.0f, 1.0f);
        RainGui::InputInt2("input int2", vec4i);
        RainGui::DragInt2("drag int2", vec4i, 1, 0, 255);
        RainGui::SliderInt2("slider int2", vec4i, 0, 255);
        RainGui::Spacing();

        RainGui::InputFloat3("input float3", vec4f);
        RainGui::DragFloat3("drag float3", vec4f, 0.01f, 0.0f, 1.0f);
        RainGui::SliderFloat3("slider float3", vec4f, 0.0f, 1.0f);
        RainGui::InputInt3("input int3", vec4i);
        RainGui::DragInt3("drag int3", vec4i, 1, 0, 255);
        RainGui::SliderInt3("slider int3", vec4i, 0, 255);
        RainGui::Spacing();

        RainGui::InputFloat4("input float4", vec4f);
        RainGui::DragFloat4("drag float4", vec4f, 0.01f, 0.0f, 1.0f);
        RainGui::SliderFloat4("slider float4", vec4f, 0.0f, 1.0f);
        RainGui::InputInt4("input int4", vec4i);
        RainGui::DragInt4("drag int4", vec4i, 1, 0, 255);
        RainGui::SliderInt4("slider int4", vec4i, 0, 255);

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Vertical Sliders"))
    {
        const float spacing = 4;
        RainGui::PushStyleVar(RainGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

        static int int_value = 0;
        RainGui::VSliderInt("##int", ImVec2(18, 160), &int_value, 0, 5);
        RainGui::SameLine();

        static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
        RainGui::PushID("set1");
        for (int i = 0; i < 7; i++)
        {
            if (i > 0) RainGui::SameLine();
            RainGui::PushID(i);
            RainGui::PushStyleColor(RainGuiCol_FrameBg, (ImVec4)ImColor::HSV(i / 7.0f, 0.5f, 0.5f));
            RainGui::PushStyleColor(RainGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.5f));
            RainGui::PushStyleColor(RainGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.5f));
            RainGui::PushStyleColor(RainGuiCol_SliderGrab, (ImVec4)ImColor::HSV(i / 7.0f, 0.9f, 0.9f));
            RainGui::VSliderFloat("##v", ImVec2(18, 160), &values[i], 0.0f, 1.0f, "");
            if (RainGui::IsItemActive() || RainGui::IsItemHovered())
                RainGui::SetTooltip("%.3f", values[i]);
            RainGui::PopStyleColor(4);
            RainGui::PopID();
        }
        RainGui::PopID();

        RainGui::SameLine();
        RainGui::PushID("set2");
        static float values2[4] = { 0.20f, 0.80f, 0.40f, 0.25f };
        const int rows = 3;
        const ImVec2 small_slider_size(18, (float)(int)((160.0f - (rows - 1) * spacing) / rows));
        for (int nx = 0; nx < 4; nx++)
        {
            if (nx > 0) RainGui::SameLine();
            RainGui::BeginGroup();
            for (int ny = 0; ny < rows; ny++)
            {
                RainGui::PushID(nx * rows + ny);
                RainGui::VSliderFloat("##v", small_slider_size, &values2[nx], 0.0f, 1.0f, "");
                if (RainGui::IsItemActive() || RainGui::IsItemHovered())
                    RainGui::SetTooltip("%.3f", values2[nx]);
                RainGui::PopID();
            }
            RainGui::EndGroup();
        }
        RainGui::PopID();

        RainGui::SameLine();
        RainGui::PushID("set3");
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) RainGui::SameLine();
            RainGui::PushID(i);
            RainGui::PushStyleVar(RainGuiStyleVar_GrabMinSize, 40);
            RainGui::VSliderFloat("##v", ImVec2(40, 160), &values[i], 0.0f, 1.0f, "%.2f\nsec");
            RainGui::PopStyleVar();
            RainGui::PopID();
        }
        RainGui::PopID();
        RainGui::PopStyleVar();
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Drag and Drop"))
    {
        if (RainGui::TreeNode("Drag and drop in standard widgets"))
        {
            // ColorEdit widgets automatically act as drag source and drag target.
            // They are using standardized payload strings RAINGUI_PAYLOAD_TYPE_COLOR_3F and RAINGUI_PAYLOAD_TYPE_COLOR_4F
            // to allow your own widgets to use colors in their drag and drop interaction.
            // Also see 'Demo->Widgets->Color/Picker Widgets->Palette' demo.
            HelpMarker("You can drag from the color squares.");
            static float col1[3] = { 1.0f, 0.0f, 0.2f };
            static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            RainGui::ColorEdit3("color 1", col1);
            RainGui::ColorEdit4("color 2", col2);
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Drag and drop to copy/swap items"))
        {
            enum Mode
            {
                Mode_Copy,
                Mode_Move,
                Mode_Swap
            };
            static int mode = 0;
            if (RainGui::RadioButton("Copy", mode == Mode_Copy)) { mode = Mode_Copy; } RainGui::SameLine();
            if (RainGui::RadioButton("Move", mode == Mode_Move)) { mode = Mode_Move; } RainGui::SameLine();
            if (RainGui::RadioButton("Swap", mode == Mode_Swap)) { mode = Mode_Swap; }
            static const char* names[9] =
            {
                "Bobby", "Beatrice", "Betty",
                "Brianna", "Barry", "Bernard",
                "Bibi", "Blaine", "Bryn"
            };
            for (int n = 0; n < IM_ARRAYSIZE(names); n++)
            {
                RainGui::PushID(n);
                if ((n % 3) != 0)
                    RainGui::SameLine();
                RainGui::Button(names[n], ImVec2(60, 60));

                // Our buttons are both drag sources and drag targets here!
                if (RainGui::BeginDragDropSource(RainGuiDragDropFlags_None))
                {
                    // Set payload to carry the index of our item (could be anything)
                    RainGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

                    // Display preview (could be anything, e.g. when dragging an image we could decide to display
                    // the filename and a small preview of the image, etc.)
                    if (mode == Mode_Copy) { RainGui::Text("Copy %s", names[n]); }
                    if (mode == Mode_Move) { RainGui::Text("Move %s", names[n]); }
                    if (mode == Mode_Swap) { RainGui::Text("Swap %s", names[n]); }
                    RainGui::EndDragDropSource();
                }
                if (RainGui::BeginDragDropTarget())
                {
                    if (const RainGuiPayload* payload = RainGui::AcceptDragDropPayload("DND_DEMO_CELL"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        if (mode == Mode_Copy)
                        {
                            names[n] = names[payload_n];
                        }
                        if (mode == Mode_Move)
                        {
                            names[n] = names[payload_n];
                            names[payload_n] = "";
                        }
                        if (mode == Mode_Swap)
                        {
                            const char* tmp = names[n];
                            names[n] = names[payload_n];
                            names[payload_n] = tmp;
                        }
                    }
                    RainGui::EndDragDropTarget();
                }
                RainGui::PopID();
            }
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Drag to reorder items (simple)"))
        {
            // Simple reordering
            HelpMarker(
                "We don't use the drag and drop api at all here! "
                "Instead we query when the item is held but not hovered, and order items accordingly.");
            static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
            for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
            {
                const char* item = item_names[n];
                RainGui::Selectable(item);

                if (RainGui::IsItemActive() && !RainGui::IsItemHovered())
                {
                    int n_next = n + (RainGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (n_next >= 0 && n_next < IM_ARRAYSIZE(item_names))
                    {
                        item_names[n] = item_names[n_next];
                        item_names[n_next] = item;
                        RainGui::ResetMouseDragDelta();
                    }
                }
            }
            RainGui::TreePop();
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Querying Status (Edited/Active/Focused/Hovered etc.)"))
    {
        // Select an item type
        const char* item_names[] =
        {
            "Text", "Button", "Button (w/ repeat)", "Checkbox", "SliderFloat", "InputText", "InputFloat",
            "InputFloat3", "ColorEdit4", "MenuItem", "TreeNode", "TreeNode (w/ double-click)", "Combo", "ListBox"
        };
        static int item_type = 1;
        RainGui::Combo("Item Type", &item_type, item_names, IM_ARRAYSIZE(item_names), IM_ARRAYSIZE(item_names));
        RainGui::SameLine();
        HelpMarker("Testing how various types of items are interacting with the IsItemXXX functions. Note that the bool return value of most RainGui function is generally equivalent to calling RainGui::IsItemHovered().");

        // Submit selected item item so we can query their status in the code following it.
        bool ret = false;
        static bool b = false;
        static float col4f[4] = { 1.0f, 0.5, 0.0f, 1.0f };
        static char str[16] = {};
        if (item_type == 0) { RainGui::Text("ITEM: Text"); }                                              // Testing text items with no identifier/interaction
        if (item_type == 1) { ret = RainGui::Button("ITEM: Button"); }                                    // Testing button
        if (item_type == 2) { RainGui::PushButtonRepeat(true); ret = RainGui::Button("ITEM: Button"); RainGui::PopButtonRepeat(); } // Testing button (with repeater)
        if (item_type == 3) { ret = RainGui::Checkbox("ITEM: Checkbox", &b); }                            // Testing checkbox
        if (item_type == 4) { ret = RainGui::SliderFloat("ITEM: SliderFloat", &col4f[0], 0.0f, 1.0f); }   // Testing basic item
        if (item_type == 5) { ret = RainGui::InputText("ITEM: InputText", &str[0], IM_ARRAYSIZE(str)); }  // Testing input text (which handles tabbing)
        if (item_type == 6) { ret = RainGui::InputFloat("ITEM: InputFloat", col4f, 1.0f); }               // Testing +/- buttons on scalar input
        if (item_type == 7) { ret = RainGui::InputFloat3("ITEM: InputFloat3", col4f); }                   // Testing multi-component items (IsItemXXX flags are reported merged)
        if (item_type == 8) { ret = RainGui::ColorEdit4("ITEM: ColorEdit4", col4f); }                     // Testing multi-component items (IsItemXXX flags are reported merged)
        if (item_type == 9) { ret = RainGui::MenuItem("ITEM: MenuItem"); }                                // Testing menu item (they use RainGuiButtonFlags_PressedOnRelease button policy)
        if (item_type == 10){ ret = RainGui::TreeNode("ITEM: TreeNode"); if (ret) RainGui::TreePop(); }     // Testing tree node
        if (item_type == 11){ ret = RainGui::TreeNodeEx("ITEM: TreeNode w/ RainGuiTreeNodeFlags_OpenOnDoubleClick", RainGuiTreeNodeFlags_OpenOnDoubleClick | RainGuiTreeNodeFlags_NoTreePushOnOpen); } // Testing tree node with RainGuiButtonFlags_PressedOnDoubleClick button policy.
        if (item_type == 12){ const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi" }; static int current = 1; ret = RainGui::Combo("ITEM: Combo", &current, items, IM_ARRAYSIZE(items)); }
        if (item_type == 13){ const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi" }; static int current = 1; ret = RainGui::ListBox("ITEM: ListBox", &current, items, IM_ARRAYSIZE(items), IM_ARRAYSIZE(items)); }

        // Display the values of IsItemHovered() and other common item state functions.
        // Note that the RainGuiHoveredFlags_XXX flags can be combined.
        // Because BulletText is an item itself and that would affect the output of IsItemXXX functions,
        // we query every state in a single call to avoid storing them and to simplify the code.
        RainGui::BulletText(
            "Return value = %d\n"
            "IsItemFocused() = %d\n"
            "IsItemHovered() = %d\n"
            "IsItemHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsItemHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsItemHovered(_AllowWhenOverlapped) = %d\n"
            "IsItemHovered(_RectOnly) = %d\n"
            "IsItemActive() = %d\n"
            "IsItemEdited() = %d\n"
            "IsItemActivated() = %d\n"
            "IsItemDeactivated() = %d\n"
            "IsItemDeactivatedAfterEdit() = %d\n"
            "IsItemVisible() = %d\n"
            "IsItemClicked() = %d\n"
            "IsItemToggledOpen() = %d\n"
            "GetItemRectMin() = (%.1f, %.1f)\n"
            "GetItemRectMax() = (%.1f, %.1f)\n"
            "GetItemRectSize() = (%.1f, %.1f)",
            ret,
            RainGui::IsItemFocused(),
            RainGui::IsItemHovered(),
            RainGui::IsItemHovered(RainGuiHoveredFlags_AllowWhenBlockedByPopup),
            RainGui::IsItemHovered(RainGuiHoveredFlags_AllowWhenBlockedByActiveItem),
            RainGui::IsItemHovered(RainGuiHoveredFlags_AllowWhenOverlapped),
            RainGui::IsItemHovered(RainGuiHoveredFlags_RectOnly),
            RainGui::IsItemActive(),
            RainGui::IsItemEdited(),
            RainGui::IsItemActivated(),
            RainGui::IsItemDeactivated(),
            RainGui::IsItemDeactivatedAfterEdit(),
            RainGui::IsItemVisible(),
            RainGui::IsItemClicked(),
            RainGui::IsItemToggledOpen(),
            RainGui::GetItemRectMin().x, RainGui::GetItemRectMin().y,
            RainGui::GetItemRectMax().x, RainGui::GetItemRectMax().y,
            RainGui::GetItemRectSize().x, RainGui::GetItemRectSize().y
        );

        static bool embed_all_inside_a_child_window = false;
        RainGui::Checkbox("Embed everything inside a child window (for additional testing)", &embed_all_inside_a_child_window);
        if (embed_all_inside_a_child_window)
            RainGui::BeginChild("outer_child", ImVec2(0, RainGui::GetFontSize() * 20.0f), true);

        // Testing IsWindowFocused() function with its various flags.
        // Note that the RainGuiFocusedFlags_XXX flags can be combined.
        RainGui::BulletText(
            "IsWindowFocused() = %d\n"
            "IsWindowFocused(_ChildWindows) = %d\n"
            "IsWindowFocused(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowFocused(_RootWindow) = %d\n"
            "IsWindowFocused(_AnyWindow) = %d\n",
            RainGui::IsWindowFocused(),
            RainGui::IsWindowFocused(RainGuiFocusedFlags_ChildWindows),
            RainGui::IsWindowFocused(RainGuiFocusedFlags_ChildWindows | RainGuiFocusedFlags_RootWindow),
            RainGui::IsWindowFocused(RainGuiFocusedFlags_RootWindow),
            RainGui::IsWindowFocused(RainGuiFocusedFlags_AnyWindow));

        // Testing IsWindowHovered() function with its various flags.
        // Note that the RainGuiHoveredFlags_XXX flags can be combined.
        RainGui::BulletText(
            "IsWindowHovered() = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_AllowWhenBlockedByActiveItem) = %d\n"
            "IsWindowHovered(_ChildWindows) = %d\n"
            "IsWindowHovered(_ChildWindows|_RootWindow) = %d\n"
            "IsWindowHovered(_ChildWindows|_AllowWhenBlockedByPopup) = %d\n"
            "IsWindowHovered(_RootWindow) = %d\n"
            "IsWindowHovered(_AnyWindow) = %d\n",
            RainGui::IsWindowHovered(),
            RainGui::IsWindowHovered(RainGuiHoveredFlags_AllowWhenBlockedByPopup),
            RainGui::IsWindowHovered(RainGuiHoveredFlags_AllowWhenBlockedByActiveItem),
            RainGui::IsWindowHovered(RainGuiHoveredFlags_ChildWindows),
            RainGui::IsWindowHovered(RainGuiHoveredFlags_ChildWindows | RainGuiHoveredFlags_RootWindow),
            RainGui::IsWindowHovered(RainGuiHoveredFlags_ChildWindows | RainGuiHoveredFlags_AllowWhenBlockedByPopup),
            RainGui::IsWindowHovered(RainGuiHoveredFlags_RootWindow),
            RainGui::IsWindowHovered(RainGuiHoveredFlags_AnyWindow));

        RainGui::BeginChild("child", ImVec2(0, 50), true);
        RainGui::Text("This is another child window for testing the _ChildWindows flag.");
        RainGui::EndChild();
        if (embed_all_inside_a_child_window)
            RainGui::EndChild();

        static char unused_str[] = "This widget is only here to be able to tab-out of the widgets above.";
        RainGui::InputText("unused", unused_str, IM_ARRAYSIZE(unused_str), RainGuiInputTextFlags_ReadOnly);

        // Calling IsItemHovered() after begin returns the hovered status of the title bar.
        // This is useful in particular if you want to create a context menu associated to the title bar of a window.
        static bool test_window = false;
        RainGui::Checkbox("Hovered/Active tests after Begin() for title bar testing", &test_window);
        if (test_window)
        {
            RainGui::Begin("Title bar Hovered/Active tests", &test_window);
            if (RainGui::BeginPopupContextItem()) // <-- This is using IsItemHovered()
            {
                if (RainGui::MenuItem("Close")) { test_window = false; }
                RainGui::EndPopup();
            }
            RainGui::Text(
                "IsItemHovered() after begin = %d (== is title bar hovered)\n"
                "IsItemActive() after begin = %d (== is window being clicked/moved)\n",
                RainGui::IsItemHovered(), RainGui::IsItemActive());
            RainGui::End();
        }

        RainGui::TreePop();
    }
}

static void ShowDemoWindowLayout()
{
    if (!RainGui::CollapsingHeader("Layout & Scrolling"))
        return;

    if (RainGui::TreeNode("Child windows"))
    {
        HelpMarker("Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window.");
        static bool disable_mouse_wheel = false;
        static bool disable_menu = false;
        RainGui::Checkbox("Disable Mouse Wheel", &disable_mouse_wheel);
        RainGui::Checkbox("Disable Menu", &disable_menu);

        // Child 1: no border, enable horizontal scrollbar
        {
            RainGuiWindowFlags window_flags = RainGuiWindowFlags_HorizontalScrollbar;
            if (disable_mouse_wheel)
                window_flags |= RainGuiWindowFlags_NoScrollWithMouse;
            RainGui::BeginChild("ChildL", ImVec2(RainGui::GetWindowContentRegionWidth() * 0.5f, 260), false, window_flags);
            for (int i = 0; i < 100; i++)
                RainGui::Text("%04d: scrollable region", i);
            RainGui::EndChild();
        }

        RainGui::SameLine();

        // Child 2: rounded border
        {
            RainGuiWindowFlags window_flags = RainGuiWindowFlags_None;
            if (disable_mouse_wheel)
                window_flags |= RainGuiWindowFlags_NoScrollWithMouse;
            if (!disable_menu)
                window_flags |= RainGuiWindowFlags_MenuBar;
            RainGui::PushStyleVar(RainGuiStyleVar_ChildRounding, 5.0f);
            RainGui::BeginChild("ChildR", ImVec2(0, 260), true, window_flags);
            if (!disable_menu && RainGui::BeginMenuBar())
            {
                if (RainGui::BeginMenu("Menu"))
                {
                    ShowExampleMenuFile();
                    RainGui::EndMenu();
                }
                RainGui::EndMenuBar();
            }
            if (RainGui::BeginTable("split", 2, RainGuiTableFlags_Resizable | RainGuiTableFlags_NoSavedSettings))
            {
                for (int i = 0; i < 100; i++)
                {
                    char buf[32];
                    sprintf(buf, "%03d", i);
                    RainGui::TableNextColumn();
                    RainGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                }
                RainGui::EndTable();
            }
            RainGui::EndChild();
            RainGui::PopStyleVar();
        }

        RainGui::Separator();

        // Demonstrate a few extra things
        // - Changing RainGuiCol_ChildBg (which is transparent black in default styles)
        // - Using SetCursorPos() to position child window (the child window is an item from the POV of parent window)
        //   You can also call SetNextWindowPos() to position the child window. The parent window will effectively
        //   layout from this position.
        // - Using RainGui::GetItemRectMin/Max() to query the "item" state (because the child window is an item from
        //   the POV of the parent window). See 'Demo->Querying Status (Active/Focused/Hovered etc.)' for details.
        {
            static int offset_x = 0;
            RainGui::SetNextItemWidth(RainGui::GetFontSize() * 8);
            RainGui::DragInt("Offset X", &offset_x, 1.0f, -1000, 1000);

            RainGui::SetCursorPosX(RainGui::GetCursorPosX() + (float)offset_x);
            RainGui::PushStyleColor(RainGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
            RainGui::BeginChild("Red", ImVec2(200, 100), true, RainGuiWindowFlags_None);
            for (int n = 0; n < 50; n++)
                RainGui::Text("Some test %d", n);
            RainGui::EndChild();
            bool child_is_hovered = RainGui::IsItemHovered();
            ImVec2 child_rect_min = RainGui::GetItemRectMin();
            ImVec2 child_rect_max = RainGui::GetItemRectMax();
            RainGui::PopStyleColor();
            RainGui::Text("Hovered: %d", child_is_hovered);
            RainGui::Text("Rect of child window is: (%.0f,%.0f) (%.0f,%.0f)", child_rect_min.x, child_rect_min.y, child_rect_max.x, child_rect_max.y);
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Widgets Width"))
    {
        static float f = 0.0f;
        static bool show_indented_items = true;
        RainGui::Checkbox("Show indented items", &show_indented_items);

        // Use SetNextItemWidth() to set the width of a single upcoming item.
        // Use PushItemWidth()/PopItemWidth() to set the width of a group of items.
        // In real code use you'll probably want to choose width values that are proportional to your font size
        // e.g. Using '20.0f * GetFontSize()' as width instead of '200.0f', etc.

        RainGui::Text("SetNextItemWidth/PushItemWidth(100)");
        RainGui::SameLine(); HelpMarker("Fixed width.");
        RainGui::PushItemWidth(100);
        RainGui::DragFloat("float##1b", &f);
        if (show_indented_items)
        {
            RainGui::Indent();
            RainGui::DragFloat("float (indented)##1b", &f);
            RainGui::Unindent();
        }
        RainGui::PopItemWidth();

        RainGui::Text("SetNextItemWidth/PushItemWidth(-100)");
        RainGui::SameLine(); HelpMarker("Align to right edge minus 100");
        RainGui::PushItemWidth(-100);
        RainGui::DragFloat("float##2a", &f);
        if (show_indented_items)
        {
            RainGui::Indent();
            RainGui::DragFloat("float (indented)##2b", &f);
            RainGui::Unindent();
        }
        RainGui::PopItemWidth();

        RainGui::Text("SetNextItemWidth/PushItemWidth(GetContentRegionAvail().x * 0.5f)");
        RainGui::SameLine(); HelpMarker("Half of available width.\n(~ right-cursor_pos)\n(works within a column set)");
        RainGui::PushItemWidth(RainGui::GetContentRegionAvail().x * 0.5f);
        RainGui::DragFloat("float##3a", &f);
        if (show_indented_items)
        {
            RainGui::Indent();
            RainGui::DragFloat("float (indented)##3b", &f);
            RainGui::Unindent();
        }
        RainGui::PopItemWidth();

        RainGui::Text("SetNextItemWidth/PushItemWidth(-GetContentRegionAvail().x * 0.5f)");
        RainGui::SameLine(); HelpMarker("Align to right edge minus half");
        RainGui::PushItemWidth(-RainGui::GetContentRegionAvail().x * 0.5f);
        RainGui::DragFloat("float##4a", &f);
        if (show_indented_items)
        {
            RainGui::Indent();
            RainGui::DragFloat("float (indented)##4b", &f);
            RainGui::Unindent();
        }
        RainGui::PopItemWidth();

        // Demonstrate using PushItemWidth to surround three items.
        // Calling SetNextItemWidth() before each of them would have the same effect.
        RainGui::Text("SetNextItemWidth/PushItemWidth(-FLT_MIN)");
        RainGui::SameLine(); HelpMarker("Align to right edge");
        RainGui::PushItemWidth(-FLT_MIN);
        RainGui::DragFloat("##float5a", &f);
        if (show_indented_items)
        {
            RainGui::Indent();
            RainGui::DragFloat("float (indented)##5b", &f);
            RainGui::Unindent();
        }
        RainGui::PopItemWidth();

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Basic Horizontal Layout"))
    {
        RainGui::TextWrapped("(Use RainGui::SameLine() to keep adding items to the right of the preceding item)");

        // Text
        RainGui::Text("Two items: Hello"); RainGui::SameLine();
        RainGui::TextColored(ImVec4(1,1,0,1), "Sailor");

        // Adjust spacing
        RainGui::Text("More spacing: Hello"); RainGui::SameLine(0, 20);
        RainGui::TextColored(ImVec4(1,1,0,1), "Sailor");

        // Button
        RainGui::AlignTextToFramePadding();
        RainGui::Text("Normal buttons"); RainGui::SameLine();
        RainGui::Button("Banana"); RainGui::SameLine();
        RainGui::Button("Apple"); RainGui::SameLine();
        RainGui::Button("Corniflower");

        // Button
        RainGui::Text("Small buttons"); RainGui::SameLine();
        RainGui::SmallButton("Like this one"); RainGui::SameLine();
        RainGui::Text("can fit within a text block.");

        // Aligned to arbitrary position. Easy/cheap column.
        RainGui::Text("Aligned");
        RainGui::SameLine(150); RainGui::Text("x=150");
        RainGui::SameLine(300); RainGui::Text("x=300");
        RainGui::Text("Aligned");
        RainGui::SameLine(150); RainGui::SmallButton("x=150");
        RainGui::SameLine(300); RainGui::SmallButton("x=300");

        // Checkbox
        static bool c1 = false, c2 = false, c3 = false, c4 = false;
        RainGui::Checkbox("My", &c1); RainGui::SameLine();
        RainGui::Checkbox("Tailor", &c2); RainGui::SameLine();
        RainGui::Checkbox("Is", &c3); RainGui::SameLine();
        RainGui::Checkbox("Rich", &c4);

        // Various
        static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
        RainGui::PushItemWidth(80);
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD" };
        static int item = -1;
        RainGui::Combo("Combo", &item, items, IM_ARRAYSIZE(items)); RainGui::SameLine();
        RainGui::SliderFloat("X", &f0, 0.0f, 5.0f); RainGui::SameLine();
        RainGui::SliderFloat("Y", &f1, 0.0f, 5.0f); RainGui::SameLine();
        RainGui::SliderFloat("Z", &f2, 0.0f, 5.0f);
        RainGui::PopItemWidth();

        RainGui::PushItemWidth(80);
        RainGui::Text("Lists:");
        static int selection[4] = { 0, 1, 2, 3 };
        for (int i = 0; i < 4; i++)
        {
            if (i > 0) RainGui::SameLine();
            RainGui::PushID(i);
            RainGui::ListBox("", &selection[i], items, IM_ARRAYSIZE(items));
            RainGui::PopID();
            //if (RainGui::IsItemHovered()) RainGui::SetTooltip("ListBox %d hovered", i);
        }
        RainGui::PopItemWidth();

        // Dummy
        ImVec2 button_sz(40, 40);
        RainGui::Button("A", button_sz); RainGui::SameLine();
        RainGui::Dummy(button_sz); RainGui::SameLine();
        RainGui::Button("B", button_sz);

        // Manually wrapping
        // (we should eventually provide this as an automatic layout feature, but for now you can do it manually)
        RainGui::Text("Manually wrapping:");
        RainGuiStyle& style = RainGui::GetStyle();
        int buttons_count = 20;
        float window_visible_x2 = RainGui::GetWindowPos().x + RainGui::GetWindowContentRegionMax().x;
        for (int n = 0; n < buttons_count; n++)
        {
            RainGui::PushID(n);
            RainGui::Button("Box", button_sz);
            float last_button_x2 = RainGui::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
            if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
                RainGui::SameLine();
            RainGui::PopID();
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Groups"))
    {
        HelpMarker(
            "BeginGroup() basically locks the horizontal position for new line. "
            "EndGroup() bundles the whole group so that you can use \"item\" functions such as "
            "IsItemHovered()/IsItemActive() or SameLine() etc. on the whole group.");
        RainGui::BeginGroup();
        {
            RainGui::BeginGroup();
            RainGui::Button("AAA");
            RainGui::SameLine();
            RainGui::Button("BBB");
            RainGui::SameLine();
            RainGui::BeginGroup();
            RainGui::Button("CCC");
            RainGui::Button("DDD");
            RainGui::EndGroup();
            RainGui::SameLine();
            RainGui::Button("EEE");
            RainGui::EndGroup();
            if (RainGui::IsItemHovered())
                RainGui::SetTooltip("First group hovered");
        }
        // Capture the group size and create widgets using the same size
        ImVec2 size = RainGui::GetItemRectSize();
        const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
        RainGui::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

        RainGui::Button("ACTION", ImVec2((size.x - RainGui::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        RainGui::SameLine();
        RainGui::Button("REACTION", ImVec2((size.x - RainGui::GetStyle().ItemSpacing.x) * 0.5f, size.y));
        RainGui::EndGroup();
        RainGui::SameLine();

        RainGui::Button("LEVERAGE\nBUZZWORD", size);
        RainGui::SameLine();

        if (RainGui::BeginListBox("List", size))
        {
            RainGui::Selectable("Selected", true);
            RainGui::Selectable("Not Selected", false);
            RainGui::EndListBox();
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Text Baseline Alignment"))
    {
        {
            RainGui::BulletText("Text baseline:");
            RainGui::SameLine(); HelpMarker(
                "This is testing the vertical alignment that gets applied on text to keep it aligned with widgets. "
                "Lines only composed of text or \"small\" widgets use less vertical space than lines with framed widgets.");
            RainGui::Indent();

            RainGui::Text("KO Blahblah"); RainGui::SameLine();
            RainGui::Button("Some framed item"); RainGui::SameLine();
            HelpMarker("Baseline of button will look misaligned with text..");

            // If your line starts with text, call AlignTextToFramePadding() to align text to upcoming widgets.
            // (because we don't know what's coming after the Text() statement, we need to move the text baseline
            // down by FramePadding.y ahead of time)
            RainGui::AlignTextToFramePadding();
            RainGui::Text("OK Blahblah"); RainGui::SameLine();
            RainGui::Button("Some framed item"); RainGui::SameLine();
            HelpMarker("We call AlignTextToFramePadding() to vertically align the text baseline by +FramePadding.y");

            // SmallButton() uses the same vertical padding as Text
            RainGui::Button("TEST##1"); RainGui::SameLine();
            RainGui::Text("TEST"); RainGui::SameLine();
            RainGui::SmallButton("TEST##2");

            // If your line starts with text, call AlignTextToFramePadding() to align text to upcoming widgets.
            RainGui::AlignTextToFramePadding();
            RainGui::Text("Text aligned to framed item"); RainGui::SameLine();
            RainGui::Button("Item##1"); RainGui::SameLine();
            RainGui::Text("Item"); RainGui::SameLine();
            RainGui::SmallButton("Item##2"); RainGui::SameLine();
            RainGui::Button("Item##3");

            RainGui::Unindent();
        }

        RainGui::Spacing();

        {
            RainGui::BulletText("Multi-line text:");
            RainGui::Indent();
            RainGui::Text("One\nTwo\nThree"); RainGui::SameLine();
            RainGui::Text("Hello\nWorld"); RainGui::SameLine();
            RainGui::Text("Banana");

            RainGui::Text("Banana"); RainGui::SameLine();
            RainGui::Text("Hello\nWorld"); RainGui::SameLine();
            RainGui::Text("One\nTwo\nThree");

            RainGui::Button("HOP##1"); RainGui::SameLine();
            RainGui::Text("Banana"); RainGui::SameLine();
            RainGui::Text("Hello\nWorld"); RainGui::SameLine();
            RainGui::Text("Banana");

            RainGui::Button("HOP##2"); RainGui::SameLine();
            RainGui::Text("Hello\nWorld"); RainGui::SameLine();
            RainGui::Text("Banana");
            RainGui::Unindent();
        }

        RainGui::Spacing();

        {
            RainGui::BulletText("Misc items:");
            RainGui::Indent();

            // SmallButton() sets FramePadding to zero. Text baseline is aligned to match baseline of previous Button.
            RainGui::Button("80x80", ImVec2(80, 80));
            RainGui::SameLine();
            RainGui::Button("50x50", ImVec2(50, 50));
            RainGui::SameLine();
            RainGui::Button("Button()");
            RainGui::SameLine();
            RainGui::SmallButton("SmallButton()");

            // Tree
            const float spacing = RainGui::GetStyle().ItemInnerSpacing.x;
            RainGui::Button("Button##1");
            RainGui::SameLine(0.0f, spacing);
            if (RainGui::TreeNode("Node##1"))
            {
                // Placeholder tree data
                for (int i = 0; i < 6; i++)
                    RainGui::BulletText("Item %d..", i);
                RainGui::TreePop();
            }

            // Vertically align text node a bit lower so it'll be vertically centered with upcoming widget.
            // Otherwise you can use SmallButton() (smaller fit).
            RainGui::AlignTextToFramePadding();

            // Common mistake to avoid: if we want to SameLine after TreeNode we need to do it before we add
            // other contents below the node.
            bool node_open = RainGui::TreeNode("Node##2");
            RainGui::SameLine(0.0f, spacing); RainGui::Button("Button##2");
            if (node_open)
            {
                // Placeholder tree data
                for (int i = 0; i < 6; i++)
                    RainGui::BulletText("Item %d..", i);
                RainGui::TreePop();
            }

            // Bullet
            RainGui::Button("Button##3");
            RainGui::SameLine(0.0f, spacing);
            RainGui::BulletText("Bullet text");

            RainGui::AlignTextToFramePadding();
            RainGui::BulletText("Node");
            RainGui::SameLine(0.0f, spacing); RainGui::Button("Button##4");
            RainGui::Unindent();
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Scrolling"))
    {
        // Vertical scroll functions
        HelpMarker("Use SetScrollHereY() or SetScrollFromPosY() to scroll to a given vertical position.");

        static int track_item = 50;
        static bool enable_track = true;
        static bool enable_extra_decorations = false;
        static float scroll_to_off_px = 0.0f;
        static float scroll_to_pos_px = 200.0f;

        RainGui::Checkbox("Decoration", &enable_extra_decorations);

        RainGui::Checkbox("Track", &enable_track);
        RainGui::PushItemWidth(100);
        RainGui::SameLine(140); enable_track |= RainGui::DragInt("##item", &track_item, 0.25f, 0, 99, "Item = %d");

        bool scroll_to_off = RainGui::Button("Scroll Offset");
        RainGui::SameLine(140); scroll_to_off |= RainGui::DragFloat("##off", &scroll_to_off_px, 1.00f, 0, FLT_MAX, "+%.0f px");

        bool scroll_to_pos = RainGui::Button("Scroll To Pos");
        RainGui::SameLine(140); scroll_to_pos |= RainGui::DragFloat("##pos", &scroll_to_pos_px, 1.00f, -10, FLT_MAX, "X/Y = %.0f px");
        RainGui::PopItemWidth();

        if (scroll_to_off || scroll_to_pos)
            enable_track = false;

        RainGuiStyle& style = RainGui::GetStyle();
        float child_w = (RainGui::GetContentRegionAvail().x - 4 * style.ItemSpacing.x) / 5;
        if (child_w < 1.0f)
            child_w = 1.0f;
        RainGui::PushID("##VerticalScrolling");
        for (int i = 0; i < 5; i++)
        {
            if (i > 0) RainGui::SameLine();
            RainGui::BeginGroup();
            const char* names[] = { "Top", "25%", "Center", "75%", "Bottom" };
            RainGui::TextUnformatted(names[i]);

            const RainGuiWindowFlags child_flags = enable_extra_decorations ? RainGuiWindowFlags_MenuBar : 0;
            const RainGuiID child_id = RainGui::GetID((void*)(intptr_t)i);
            const bool child_is_visible = RainGui::BeginChild(child_id, ImVec2(child_w, 200.0f), true, child_flags);
            if (RainGui::BeginMenuBar())
            {
                RainGui::TextUnformatted("abc");
                RainGui::EndMenuBar();
            }
            if (scroll_to_off)
                RainGui::SetScrollY(scroll_to_off_px);
            if (scroll_to_pos)
                RainGui::SetScrollFromPosY(RainGui::GetCursorStartPos().y + scroll_to_pos_px, i * 0.25f);
            if (child_is_visible) // Avoid calling SetScrollHereY when running with culled items
            {
                for (int item = 0; item < 100; item++)
                {
                    if (enable_track && item == track_item)
                    {
                        RainGui::TextColored(ImVec4(1, 1, 0, 1), "Item %d", item);
                        RainGui::SetScrollHereY(i * 0.25f); // 0.0f:top, 0.5f:center, 1.0f:bottom
                    }
                    else
                    {
                        RainGui::Text("Item %d", item);
                    }
                }
            }
            float scroll_y = RainGui::GetScrollY();
            float scroll_max_y = RainGui::GetScrollMaxY();
            RainGui::EndChild();
            RainGui::Text("%.0f/%.0f", scroll_y, scroll_max_y);
            RainGui::EndGroup();
        }
        RainGui::PopID();

        // Horizontal scroll functions
        RainGui::Spacing();
        HelpMarker(
            "Use SetScrollHereX() or SetScrollFromPosX() to scroll to a given horizontal position.\n\n"
            "Because the clipping rectangle of most window hides half worth of WindowPadding on the "
            "left/right, using SetScrollFromPosX(+1) will usually result in clipped text whereas the "
            "equivalent SetScrollFromPosY(+1) wouldn't.");
        RainGui::PushID("##HorizontalScrolling");
        for (int i = 0; i < 5; i++)
        {
            float child_height = RainGui::GetTextLineHeight() + style.ScrollbarSize + style.WindowPadding.y * 2.0f;
            RainGuiWindowFlags child_flags = RainGuiWindowFlags_HorizontalScrollbar | (enable_extra_decorations ? RainGuiWindowFlags_AlwaysVerticalScrollbar : 0);
            RainGuiID child_id = RainGui::GetID((void*)(intptr_t)i);
            bool child_is_visible = RainGui::BeginChild(child_id, ImVec2(-100, child_height), true, child_flags);
            if (scroll_to_off)
                RainGui::SetScrollX(scroll_to_off_px);
            if (scroll_to_pos)
                RainGui::SetScrollFromPosX(RainGui::GetCursorStartPos().x + scroll_to_pos_px, i * 0.25f);
            if (child_is_visible) // Avoid calling SetScrollHereY when running with culled items
            {
                for (int item = 0; item < 100; item++)
                {
                    if (item > 0)
                        RainGui::SameLine();
                    if (enable_track && item == track_item)
                    {
                        RainGui::TextColored(ImVec4(1, 1, 0, 1), "Item %d", item);
                        RainGui::SetScrollHereX(i * 0.25f); // 0.0f:left, 0.5f:center, 1.0f:right
                    }
                    else
                    {
                        RainGui::Text("Item %d", item);
                    }
                }
            }
            float scroll_x = RainGui::GetScrollX();
            float scroll_max_x = RainGui::GetScrollMaxX();
            RainGui::EndChild();
            RainGui::SameLine();
            const char* names[] = { "Left", "25%", "Center", "75%", "Right" };
            RainGui::Text("%s\n%.0f/%.0f", names[i], scroll_x, scroll_max_x);
            RainGui::Spacing();
        }
        RainGui::PopID();

        // Miscellaneous Horizontal Scrolling Demo
        HelpMarker(
            "Horizontal scrolling for a window is enabled via the RainGuiWindowFlags_HorizontalScrollbar flag.\n\n"
            "You may want to also explicitly specify content width by using SetNextWindowContentWidth() before Begin().");
        static int lines = 7;
        RainGui::SliderInt("Lines", &lines, 1, 15);
        RainGui::PushStyleVar(RainGuiStyleVar_FrameRounding, 3.0f);
        RainGui::PushStyleVar(RainGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
        ImVec2 scrolling_child_size = ImVec2(0, RainGui::GetFrameHeightWithSpacing() * 7 + 30);
        RainGui::BeginChild("scrolling", scrolling_child_size, true, RainGuiWindowFlags_HorizontalScrollbar);
        for (int line = 0; line < lines; line++)
        {
            // Display random stuff. For the sake of this trivial demo we are using basic Button() + SameLine()
            // If you want to create your own time line for a real application you may be better off manipulating
            // the cursor position yourself, aka using SetCursorPos/SetCursorScreenPos to position the widgets
            // yourself. You may also want to use the lower-level ImDrawList API.
            int num_buttons = 10 + ((line & 1) ? line * 9 : line * 3);
            for (int n = 0; n < num_buttons; n++)
            {
                if (n > 0) RainGui::SameLine();
                RainGui::PushID(n + line * 1000);
                char num_buf[16];
                sprintf(num_buf, "%d", n);
                const char* label = (!(n % 15)) ? "FizzBuzz" : (!(n % 3)) ? "Fizz" : (!(n % 5)) ? "Buzz" : num_buf;
                float hue = n * 0.05f;
                RainGui::PushStyleColor(RainGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
                RainGui::PushStyleColor(RainGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue, 0.7f, 0.7f));
                RainGui::PushStyleColor(RainGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue, 0.8f, 0.8f));
                RainGui::Button(label, ImVec2(40.0f + sinf((float)(line + n)) * 20.0f, 0.0f));
                RainGui::PopStyleColor(3);
                RainGui::PopID();
            }
        }
        float scroll_x = RainGui::GetScrollX();
        float scroll_max_x = RainGui::GetScrollMaxX();
        RainGui::EndChild();
        RainGui::PopStyleVar(2);
        float scroll_x_delta = 0.0f;
        RainGui::SmallButton("<<");
        if (RainGui::IsItemActive())
            scroll_x_delta = -RainGui::GetIO().DeltaTime * 1000.0f;
        RainGui::SameLine();
        RainGui::Text("Scroll from code"); RainGui::SameLine();
        RainGui::SmallButton(">>");
        if (RainGui::IsItemActive())
            scroll_x_delta = +RainGui::GetIO().DeltaTime * 1000.0f;
        RainGui::SameLine();
        RainGui::Text("%.0f/%.0f", scroll_x, scroll_max_x);
        if (scroll_x_delta != 0.0f)
        {
            // Demonstrate a trick: you can use Begin to set yourself in the context of another window
            // (here we are already out of your child window)
            RainGui::BeginChild("scrolling");
            RainGui::SetScrollX(RainGui::GetScrollX() + scroll_x_delta);
            RainGui::EndChild();
        }
        RainGui::Spacing();

        static bool show_horizontal_contents_size_demo_window = false;
        RainGui::Checkbox("Show Horizontal contents size demo window", &show_horizontal_contents_size_demo_window);

        if (show_horizontal_contents_size_demo_window)
        {
            static bool show_h_scrollbar = true;
            static bool show_button = true;
            static bool show_tree_nodes = true;
            static bool show_text_wrapped = false;
            static bool show_columns = true;
            static bool show_tab_bar = true;
            static bool show_child = false;
            static bool explicit_content_size = false;
            static float contents_size_x = 300.0f;
            if (explicit_content_size)
                RainGui::SetNextWindowContentSize(ImVec2(contents_size_x, 0.0f));
            RainGui::Begin("Horizontal contents size demo window", &show_horizontal_contents_size_demo_window, show_h_scrollbar ? RainGuiWindowFlags_HorizontalScrollbar : 0);
            RainGui::PushStyleVar(RainGuiStyleVar_ItemSpacing, ImVec2(2, 0));
            RainGui::PushStyleVar(RainGuiStyleVar_FramePadding, ImVec2(2, 0));
            HelpMarker("Test of different widgets react and impact the work rectangle growing when horizontal scrolling is enabled.\n\nUse 'Metrics->Tools->Show windows rectangles' to visualize rectangles.");
            RainGui::Checkbox("H-scrollbar", &show_h_scrollbar);
            RainGui::Checkbox("Button", &show_button);            // Will grow contents size (unless explicitly overwritten)
            RainGui::Checkbox("Tree nodes", &show_tree_nodes);    // Will grow contents size and display highlight over full width
            RainGui::Checkbox("Text wrapped", &show_text_wrapped);// Will grow and use contents size
            RainGui::Checkbox("Columns", &show_columns);          // Will use contents size
            RainGui::Checkbox("Tab bar", &show_tab_bar);          // Will use contents size
            RainGui::Checkbox("Child", &show_child);              // Will grow and use contents size
            RainGui::Checkbox("Explicit content size", &explicit_content_size);
            RainGui::Text("Scroll %.1f/%.1f %.1f/%.1f", RainGui::GetScrollX(), RainGui::GetScrollMaxX(), RainGui::GetScrollY(), RainGui::GetScrollMaxY());
            if (explicit_content_size)
            {
                RainGui::SameLine();
                RainGui::SetNextItemWidth(100);
                RainGui::DragFloat("##csx", &contents_size_x);
                ImVec2 p = RainGui::GetCursorScreenPos();
                RainGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 10, p.y + 10), IM_COL32_WHITE);
                RainGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x + contents_size_x - 10, p.y), ImVec2(p.x + contents_size_x, p.y + 10), IM_COL32_WHITE);
                RainGui::Dummy(ImVec2(0, 10));
            }
            RainGui::PopStyleVar(2);
            RainGui::Separator();
            if (show_button)
            {
                RainGui::Button("this is a 300-wide button", ImVec2(300, 0));
            }
            if (show_tree_nodes)
            {
                bool open = true;
                if (RainGui::TreeNode("this is a tree node"))
                {
                    if (RainGui::TreeNode("another one of those tree node..."))
                    {
                        RainGui::Text("Some tree contents");
                        RainGui::TreePop();
                    }
                    RainGui::TreePop();
                }
                RainGui::CollapsingHeader("CollapsingHeader", &open);
            }
            if (show_text_wrapped)
            {
                RainGui::TextWrapped("This text should automatically wrap on the edge of the work rectangle.");
            }
            if (show_columns)
            {
                RainGui::Text("Tables:");
                if (RainGui::BeginTable("table", 4, RainGuiTableFlags_Borders))
                {
                    for (int n = 0; n < 4; n++)
                    {
                        RainGui::TableNextColumn();
                        RainGui::Text("Width %.2f", RainGui::GetContentRegionAvail().x);
                    }
                    RainGui::EndTable();
                }
                RainGui::Text("Columns:");
                RainGui::Columns(4);
                for (int n = 0; n < 4; n++)
                {
                    RainGui::Text("Width %.2f", RainGui::GetColumnWidth());
                    RainGui::NextColumn();
                }
                RainGui::Columns(1);
            }
            if (show_tab_bar && RainGui::BeginTabBar("Hello"))
            {
                if (RainGui::BeginTabItem("OneOneOne")) { RainGui::EndTabItem(); }
                if (RainGui::BeginTabItem("TwoTwoTwo")) { RainGui::EndTabItem(); }
                if (RainGui::BeginTabItem("ThreeThreeThree")) { RainGui::EndTabItem(); }
                if (RainGui::BeginTabItem("FourFourFour")) { RainGui::EndTabItem(); }
                RainGui::EndTabBar();
            }
            if (show_child)
            {
                RainGui::BeginChild("child", ImVec2(0, 0), true);
                RainGui::EndChild();
            }
            RainGui::End();
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Clipping"))
    {
        static ImVec2 size(100.0f, 100.0f);
        static ImVec2 offset(30.0f, 30.0f);
        RainGui::DragFloat2("size", (float*)&size, 0.5f, 1.0f, 200.0f, "%.0f");
        RainGui::TextWrapped("(Click and drag to scroll)");

        for (int n = 0; n < 3; n++)
        {
            if (n > 0)
                RainGui::SameLine();
            RainGui::PushID(n);
            RainGui::BeginGroup(); // Lock X position

            RainGui::InvisibleButton("##empty", size);
            if (RainGui::IsItemActive() && RainGui::IsMouseDragging(RainGuiMouseButton_Left))
            {
                offset.x += RainGui::GetIO().MouseDelta.x;
                offset.y += RainGui::GetIO().MouseDelta.y;
            }
            const ImVec2 p0 = RainGui::GetItemRectMin();
            const ImVec2 p1 = RainGui::GetItemRectMax();
            const char* text_str = "Line 1 hello\nLine 2 clip me!";
            const ImVec2 text_pos = ImVec2(p0.x + offset.x, p0.y + offset.y);
            ImDrawList* draw_list = RainGui::GetWindowDrawList();

            switch (n)
            {
            case 0:
                HelpMarker(
                    "Using RainGui::PushClipRect():\n"
                    "Will alter RainGui hit-testing logic + ImDrawList rendering.\n"
                    "(use this if you want your clipping rectangle to affect interactions)");
                RainGui::PushClipRect(p0, p1, true);
                draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
                draw_list->AddText(text_pos, IM_COL32_WHITE, text_str);
                RainGui::PopClipRect();
                break;
            case 1:
                HelpMarker(
                    "Using ImDrawList::PushClipRect():\n"
                    "Will alter ImDrawList rendering only.\n"
                    "(use this as a shortcut if you are only using ImDrawList calls)");
                draw_list->PushClipRect(p0, p1, true);
                draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
                draw_list->AddText(text_pos, IM_COL32_WHITE, text_str);
                draw_list->PopClipRect();
                break;
            case 2:
                HelpMarker(
                    "Using ImDrawList::AddText() with a fine ClipRect:\n"
                    "Will alter only this specific ImDrawList::AddText() rendering.\n"
                    "(this is often used internally to avoid altering the clipping rectangle and minimize draw calls)");
                ImVec4 clip_rect(p0.x, p0.y, p1.x, p1.y); // AddText() takes a ImVec4* here so let's convert.
                draw_list->AddRectFilled(p0, p1, IM_COL32(90, 90, 120, 255));
                draw_list->AddText(RainGui::GetFont(), RainGui::GetFontSize(), text_pos, IM_COL32_WHITE, text_str, NULL, 0.0f, &clip_rect);
                break;
            }
            RainGui::EndGroup();
            RainGui::PopID();
        }

        RainGui::TreePop();
    }
}

static void ShowDemoWindowPopups()
{
    if (!RainGui::CollapsingHeader("Popups & Modal windows"))
        return;

    // The properties of popups windows are:
    // - They block normal mouse hovering detection outside them. (*)
    // - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    // - Their visibility state (~bool) is held internally by Dear RainGui instead of being held by the programmer as
    //   we are used to with regular Begin() calls. User can manipulate the visibility state by calling OpenPopup().
    // (*) One can use IsItemHovered(RainGuiHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even
    //     when normally blocked by a popup.
    // Those three properties are connected. The library needs to hold their visibility state BECAUSE it can close
    // popups at any time.

    // Typical use for regular windows:
    //   bool my_tool_is_active = false; if (RainGui::Button("Open")) my_tool_is_active = true; [...] if (my_tool_is_active) Begin("My Tool", &my_tool_is_active) { [...] } End();
    // Typical use for popups:
    //   if (RainGui::Button("Open")) RainGui::OpenPopup("MyPopup"); if (RainGui::BeginPopup("MyPopup") { [...] EndPopup(); }

    // With popups we have to go through a library call (here OpenPopup) to manipulate the visibility state.
    // This may be a bit confusing at first but it should quickly make sense. Follow on the examples below.

    if (RainGui::TreeNode("Popups"))
    {
        RainGui::TextWrapped(
            "When a popup is active, it inhibits interacting with windows that are behind the popup. "
            "Clicking outside the popup closes it.");

        static int selected_fish = -1;
        const char* names[] = { "Bream", "Haddock", "Mackerel", "Pollock", "Tilefish" };
        static bool toggles[] = { true, false, false, false, false };

        // Simple selection popup (if you want to show the current selection inside the Button itself,
        // you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
        if (RainGui::Button("Select.."))
            RainGui::OpenPopup("my_select_popup");
        RainGui::SameLine();
        RainGui::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
        if (RainGui::BeginPopup("my_select_popup"))
        {
            RainGui::Text("Aquarium");
            RainGui::Separator();
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                if (RainGui::Selectable(names[i]))
                    selected_fish = i;
            RainGui::EndPopup();
        }

        // Showing a menu with toggles
        if (RainGui::Button("Toggle.."))
            RainGui::OpenPopup("my_toggle_popup");
        if (RainGui::BeginPopup("my_toggle_popup"))
        {
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                RainGui::MenuItem(names[i], "", &toggles[i]);
            if (RainGui::BeginMenu("Sub-menu"))
            {
                RainGui::MenuItem("Click me");
                RainGui::EndMenu();
            }

            RainGui::Separator();
            RainGui::Text("Tooltip here");
            if (RainGui::IsItemHovered())
                RainGui::SetTooltip("I am a tooltip over a popup");

            if (RainGui::Button("Stacked Popup"))
                RainGui::OpenPopup("another popup");
            if (RainGui::BeginPopup("another popup"))
            {
                for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                    RainGui::MenuItem(names[i], "", &toggles[i]);
                if (RainGui::BeginMenu("Sub-menu"))
                {
                    RainGui::MenuItem("Click me");
                    if (RainGui::Button("Stacked Popup"))
                        RainGui::OpenPopup("another popup");
                    if (RainGui::BeginPopup("another popup"))
                    {
                        RainGui::Text("I am the last one here.");
                        RainGui::EndPopup();
                    }
                    RainGui::EndMenu();
                }
                RainGui::EndPopup();
            }
            RainGui::EndPopup();
        }

        // Call the more complete ShowExampleMenuFile which we use in various places of this demo
        if (RainGui::Button("File Menu.."))
            RainGui::OpenPopup("my_file_popup");
        if (RainGui::BeginPopup("my_file_popup"))
        {
            ShowExampleMenuFile();
            RainGui::EndPopup();
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Context menus"))
    {
        HelpMarker("\"Context\" functions are simple helpers to associate a Popup to a given Item or Window identifier.");

        // BeginPopupContextItem() is a helper to provide common/simple popup behavior of essentially doing:
        //     if (id == 0)
        //         id = GetItemID(); // Use last item id
        //     if (IsItemHovered() && IsMouseReleased(RainGuiMouseButton_Right))
        //         OpenPopup(id);
        //     return BeginPopup(id);
        // For advanced advanced uses you may want to replicate and customize this code.
        // See more details in BeginPopupContextItem().

        // Example 1
        // When used after an item that has an ID (e.g. Button), we can skip providing an ID to BeginPopupContextItem(),
        // and BeginPopupContextItem() will use the last item ID as the popup ID.
        {
            const char* names[5] = { "Label1", "Label2", "Label3", "Label4", "Label5" };
            for (int n = 0; n < 5; n++)
            {
                RainGui::Selectable(names[n]);
                if (RainGui::BeginPopupContextItem()) // <-- use last item id as popup id
                {
                    RainGui::Text("This a popup for \"%s\"!", names[n]);
                    if (RainGui::Button("Close"))
                        RainGui::CloseCurrentPopup();
                    RainGui::EndPopup();
                }
                if (RainGui::IsItemHovered())
                    RainGui::SetTooltip("Right-click to open popup");
            }
        }

        // Example 2
        // Popup on a Text() element which doesn't have an identifier: we need to provide an identifier to BeginPopupContextItem().
        // Using an explicit identifier is also convenient if you want to activate the popups from different locations.
        {
            HelpMarker("Text() elements don't have stable identifiers so we need to provide one.");
            static float value = 0.5f;
            RainGui::Text("Value = %.3f <-- (1) right-click this value", value);
            if (RainGui::BeginPopupContextItem("my popup"))
            {
                if (RainGui::Selectable("Set to zero")) value = 0.0f;
                if (RainGui::Selectable("Set to PI")) value = 3.1415f;
                RainGui::SetNextItemWidth(-FLT_MIN);
                RainGui::DragFloat("##Value", &value, 0.1f, 0.0f, 0.0f);
                RainGui::EndPopup();
            }

            // We can also use OpenPopupOnItemClick() to toggle the visibility of a given popup.
            // Here we make it that right-clicking this other text element opens the same popup as above.
            // The popup itself will be submitted by the code above.
            RainGui::Text("(2) Or right-click this text");
            RainGui::OpenPopupOnItemClick("my popup", RainGuiPopupFlags_MouseButtonRight);

            // Back to square one: manually open the same popup.
            if (RainGui::Button("(3) Or click this button"))
                RainGui::OpenPopup("my popup");
        }

        // Example 3
        // When using BeginPopupContextItem() with an implicit identifier (NULL == use last item ID),
        // we need to make sure your item identifier is stable.
        // In this example we showcase altering the item label while preserving its identifier, using the ### operator (see FAQ).
        {
            HelpMarker("Showcase using a popup ID linked to item ID, with the item having a changing label + stable ID using the ### operator.");
            static char name[32] = "Label1";
            char buf[64];
            sprintf(buf, "Button: %s###Button", name); // ### operator override ID ignoring the preceding label
            RainGui::Button(buf);
            if (RainGui::BeginPopupContextItem())
            {
                RainGui::Text("Edit name:");
                RainGui::InputText("##edit", name, IM_ARRAYSIZE(name));
                if (RainGui::Button("Close"))
                    RainGui::CloseCurrentPopup();
                RainGui::EndPopup();
            }
            RainGui::SameLine(); RainGui::Text("(<-- right-click here)");
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Modals"))
    {
        RainGui::TextWrapped("Modal windows are like popups but the user cannot close them by clicking outside.");

        if (RainGui::Button("Delete.."))
            RainGui::OpenPopup("Delete?");

        // Always center this window when appearing
        ImVec2 center = RainGui::GetMainViewport()->GetCenter();
        RainGui::SetNextWindowPos(center, RainGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (RainGui::BeginPopupModal("Delete?", NULL, RainGuiWindowFlags_AlwaysAutoResize))
        {
            RainGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
            RainGui::Separator();

            //static int unused_i = 0;
            //RainGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

            static bool dont_ask_me_next_time = false;
            RainGui::PushStyleVar(RainGuiStyleVar_FramePadding, ImVec2(0, 0));
            RainGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
            RainGui::PopStyleVar();

            if (RainGui::Button("OK", ImVec2(120, 0))) { RainGui::CloseCurrentPopup(); }
            RainGui::SetItemDefaultFocus();
            RainGui::SameLine();
            if (RainGui::Button("Cancel", ImVec2(120, 0))) { RainGui::CloseCurrentPopup(); }
            RainGui::EndPopup();
        }

        if (RainGui::Button("Stacked modals.."))
            RainGui::OpenPopup("Stacked 1");
        if (RainGui::BeginPopupModal("Stacked 1", NULL, RainGuiWindowFlags_MenuBar))
        {
            if (RainGui::BeginMenuBar())
            {
                if (RainGui::BeginMenu("File"))
                {
                    if (RainGui::MenuItem("Some menu item")) {}
                    RainGui::EndMenu();
                }
                RainGui::EndMenuBar();
            }
            RainGui::Text("Hello from Stacked The First\nUsing style.Colors[RainGuiCol_ModalWindowDimBg] behind it.");

            // Testing behavior of widgets stacking their own regular popups over the modal.
            static int item = 1;
            static float color[4] = { 0.4f, 0.7f, 0.0f, 0.5f };
            RainGui::Combo("Combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
            RainGui::ColorEdit4("color", color);

            if (RainGui::Button("Add another modal.."))
                RainGui::OpenPopup("Stacked 2");

            // Also demonstrate passing a bool* to BeginPopupModal(), this will create a regular close button which
            // will close the popup. Note that the visibility state of popups is owned by raingui, so the input value
            // of the bool actually doesn't matter here.
            bool unused_open = true;
            if (RainGui::BeginPopupModal("Stacked 2", &unused_open))
            {
                RainGui::Text("Hello from Stacked The Second!");
                if (RainGui::Button("Close"))
                    RainGui::CloseCurrentPopup();
                RainGui::EndPopup();
            }

            if (RainGui::Button("Close"))
                RainGui::CloseCurrentPopup();
            RainGui::EndPopup();
        }

        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Menus inside a regular window"))
    {
        RainGui::TextWrapped("Below we are testing adding menu items to a regular window. It's rather unusual but should work!");
        RainGui::Separator();

        // Note: As a quirk in this very specific example, we want to differentiate the parent of this menu from the
        // parent of the various popup menus above. To do so we are encloding the items in a PushID()/PopID() block
        // to make them two different menusets. If we don't, opening any popup above and hovering our menu here would
        // open it. This is because once a menu is active, we allow to switch to a sibling menu by just hovering on it,
        // which is the desired behavior for regular menus.
        RainGui::PushID("foo");
        RainGui::MenuItem("Menu item", "CTRL+M");
        if (RainGui::BeginMenu("Menu inside a regular window"))
        {
            ShowExampleMenuFile();
            RainGui::EndMenu();
        }
        RainGui::PopID();
        RainGui::Separator();
        RainGui::TreePop();
    }
}

// Dummy data structure that we use for the Table demo.
// (pre-C++11 doesn't allow us to instantiate ImVector<MyItem> template if this structure if defined inside the demo function)
namespace
{
// We are passing our own identifier to TableSetupColumn() to facilitate identifying columns in the sorting code.
// This identifier will be passed down into RainGuiTableSortSpec::ColumnUserID.
// But it is possible to omit the user id parameter of TableSetupColumn() and just use the column index instead! (RainGuiTableSortSpec::ColumnIndex)
// If you don't use sorting, you will generally never care about giving column an ID!
enum MyItemColumnID
{
    MyItemColumnID_ID,
    MyItemColumnID_Name,
    MyItemColumnID_Action,
    MyItemColumnID_Quantity,
    MyItemColumnID_Description
};

struct MyItem
{
    int         ID;
    const char* Name;
    int         Quantity;

    // We have a problem which is affecting _only this demo_ and should not affect your code:
    // As we don't rely on std:: or other third-party library to compile dear raingui, we only have reliable access to qsort(),
    // however qsort doesn't allow passing user data to comparing function.
    // As a workaround, we are storing the sort specs in a static/global for the comparing function to access.
    // In your own use case you would probably pass the sort specs to your sorting/comparing functions directly and not use a global.
    // We could technically call RainGui::TableGetSortSpecs() in CompareWithSortSpecs(), but considering that this function is called
    // very often by the sorting algorithm it would be a little wasteful.
    static const RainGuiTableSortSpecs* s_current_sort_specs;

    // Compare function to be used by qsort()
    static int RAINGUI_CDECL CompareWithSortSpecs(const void* lhs, const void* rhs)
    {
        const MyItem* a = (const MyItem*)lhs;
        const MyItem* b = (const MyItem*)rhs;
        for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
        {
            // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
            // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
            const RainGuiTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
            int delta = 0;
            switch (sort_spec->ColumnUserID)
            {
            case MyItemColumnID_ID:             delta = (a->ID - b->ID);                break;
            case MyItemColumnID_Name:           delta = (strcmp(a->Name, b->Name));     break;
            case MyItemColumnID_Quantity:       delta = (a->Quantity - b->Quantity);    break;
            case MyItemColumnID_Description:    delta = (strcmp(a->Name, b->Name));     break;
            default: IM_ASSERT(0); break;
            }
            if (delta > 0)
                return (sort_spec->SortDirection == RainGuiSortDirection_Ascending) ? +1 : -1;
            if (delta < 0)
                return (sort_spec->SortDirection == RainGuiSortDirection_Ascending) ? -1 : +1;
        }

        // qsort() is instable so always return a way to differenciate items.
        // Your own compare function may want to avoid fallback on implicit sort specs e.g. a Name compare if it wasn't already part of the sort specs.
        return (a->ID - b->ID);
    }
};
const RainGuiTableSortSpecs* MyItem::s_current_sort_specs = NULL;
}

// Make the UI compact because there are so many fields
static void PushStyleCompact()
{
    RainGuiStyle& style = RainGui::GetStyle();
    RainGui::PushStyleVar(RainGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
    RainGui::PushStyleVar(RainGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

static void PopStyleCompact()
{
    RainGui::PopStyleVar(2);
}

// Show a combo box with a choice of sizing policies
static void EditTableSizingFlags(RainGuiTableFlags* p_flags)
{
    struct EnumDesc { RainGuiTableFlags Value; const char* Name; const char* Tooltip; };
    static const EnumDesc policies[] =
    {
        { RainGuiTableFlags_None,               "Default",                            "Use default sizing policy:\n- RainGuiTableFlags_SizingFixedFit if ScrollX is on or if host window has RainGuiWindowFlags_AlwaysAutoResize.\n- RainGuiTableFlags_SizingStretchSame otherwise." },
        { RainGuiTableFlags_SizingFixedFit,     "RainGuiTableFlags_SizingFixedFit",     "Columns default to _WidthFixed (if resizable) or _WidthAuto (if not resizable), matching contents width." },
        { RainGuiTableFlags_SizingFixedSame,    "RainGuiTableFlags_SizingFixedSame",    "Columns are all the same width, matching the maximum contents width.\nImplicitly disable RainGuiTableFlags_Resizable and enable RainGuiTableFlags_NoKeepColumnsVisible." },
        { RainGuiTableFlags_SizingStretchProp,  "RainGuiTableFlags_SizingStretchProp",  "Columns default to _WidthStretch with weights proportional to their widths." },
        { RainGuiTableFlags_SizingStretchSame,  "RainGuiTableFlags_SizingStretchSame",  "Columns default to _WidthStretch with same weights." }
    };
    int idx;
    for (idx = 0; idx < IM_ARRAYSIZE(policies); idx++)
        if (policies[idx].Value == (*p_flags & RainGuiTableFlags_SizingMask_))
            break;
    const char* preview_text = (idx < IM_ARRAYSIZE(policies)) ? policies[idx].Name + (idx > 0 ? strlen("RainGuiTableFlags") : 0) : "";
    if (RainGui::BeginCombo("Sizing Policy", preview_text))
    {
        for (int n = 0; n < IM_ARRAYSIZE(policies); n++)
            if (RainGui::Selectable(policies[n].Name, idx == n))
                *p_flags = (*p_flags & ~RainGuiTableFlags_SizingMask_) | policies[n].Value;
        RainGui::EndCombo();
    }
    RainGui::SameLine();
    RainGui::TextDisabled("(?)");
    if (RainGui::IsItemHovered())
    {
        RainGui::BeginTooltip();
        RainGui::PushTextWrapPos(RainGui::GetFontSize() * 50.0f);
        for (int m = 0; m < IM_ARRAYSIZE(policies); m++)
        {
            RainGui::Separator();
            RainGui::Text("%s:", policies[m].Name);
            RainGui::Separator();
            RainGui::SetCursorPosX(RainGui::GetCursorPosX() + RainGui::GetStyle().IndentSpacing * 0.5f);
            RainGui::TextUnformatted(policies[m].Tooltip);
        }
        RainGui::PopTextWrapPos();
        RainGui::EndTooltip();
    }
}

static void EditTableColumnsFlags(RainGuiTableColumnFlags* p_flags)
{
    RainGui::CheckboxFlags("_DefaultHide", p_flags, RainGuiTableColumnFlags_DefaultHide);
    RainGui::CheckboxFlags("_DefaultSort", p_flags, RainGuiTableColumnFlags_DefaultSort);
    if (RainGui::CheckboxFlags("_WidthStretch", p_flags, RainGuiTableColumnFlags_WidthStretch))
        *p_flags &= ~(RainGuiTableColumnFlags_WidthMask_ ^ RainGuiTableColumnFlags_WidthStretch);
    if (RainGui::CheckboxFlags("_WidthFixed", p_flags, RainGuiTableColumnFlags_WidthFixed))
        *p_flags &= ~(RainGuiTableColumnFlags_WidthMask_ ^ RainGuiTableColumnFlags_WidthFixed);
    RainGui::CheckboxFlags("_NoResize", p_flags, RainGuiTableColumnFlags_NoResize);
    RainGui::CheckboxFlags("_NoReorder", p_flags, RainGuiTableColumnFlags_NoReorder);
    RainGui::CheckboxFlags("_NoHide", p_flags, RainGuiTableColumnFlags_NoHide);
    RainGui::CheckboxFlags("_NoClip", p_flags, RainGuiTableColumnFlags_NoClip);
    RainGui::CheckboxFlags("_NoSort", p_flags, RainGuiTableColumnFlags_NoSort);
    RainGui::CheckboxFlags("_NoSortAscending", p_flags, RainGuiTableColumnFlags_NoSortAscending);
    RainGui::CheckboxFlags("_NoSortDescending", p_flags, RainGuiTableColumnFlags_NoSortDescending);
    RainGui::CheckboxFlags("_NoHeaderWidth", p_flags, RainGuiTableColumnFlags_NoHeaderWidth);
    RainGui::CheckboxFlags("_PreferSortAscending", p_flags, RainGuiTableColumnFlags_PreferSortAscending);
    RainGui::CheckboxFlags("_PreferSortDescending", p_flags, RainGuiTableColumnFlags_PreferSortDescending);
    RainGui::CheckboxFlags("_IndentEnable", p_flags, RainGuiTableColumnFlags_IndentEnable); RainGui::SameLine(); HelpMarker("Default for column 0");
    RainGui::CheckboxFlags("_IndentDisable", p_flags, RainGuiTableColumnFlags_IndentDisable); RainGui::SameLine(); HelpMarker("Default for column >0");
}

static void ShowTableColumnsStatusFlags(RainGuiTableColumnFlags flags)
{
    RainGui::CheckboxFlags("_IsEnabled", &flags, RainGuiTableColumnFlags_IsEnabled);
    RainGui::CheckboxFlags("_IsVisible", &flags, RainGuiTableColumnFlags_IsVisible);
    RainGui::CheckboxFlags("_IsSorted", &flags, RainGuiTableColumnFlags_IsSorted);
    RainGui::CheckboxFlags("_IsHovered", &flags, RainGuiTableColumnFlags_IsHovered);
}

static void ShowDemoWindowTables()
{
    //RainGui::SetNextItemOpen(true, RainGuiCond_Once);
    if (!RainGui::CollapsingHeader("Tables & Columns"))
        return;

    // Using those as a base value to create width/height that are factor of the size of our font
    const float TEXT_BASE_WIDTH = RainGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = RainGui::GetTextLineHeightWithSpacing();

    RainGui::PushID("Tables");

    int open_action = -1;
    if (RainGui::Button("Open all"))
        open_action = 1;
    RainGui::SameLine();
    if (RainGui::Button("Close all"))
        open_action = 0;
    RainGui::SameLine();

    // Options
    static bool disable_indent = false;
    RainGui::Checkbox("Disable tree indentation", &disable_indent);
    RainGui::SameLine();
    HelpMarker("Disable the indenting of tree nodes so demo tables can use the full window width.");
    RainGui::Separator();
    if (disable_indent)
        RainGui::PushStyleVar(RainGuiStyleVar_IndentSpacing, 0.0f);

    // About Styling of tables
    // Most settings are configured on a per-table basis via the flags passed to BeginTable() and TableSetupColumns APIs.
    // There are however a few settings that a shared and part of the RainGuiStyle structure:
    //   style.CellPadding                          // Padding within each cell
    //   style.Colors[RainGuiCol_TableHeaderBg]       // Table header background
    //   style.Colors[RainGuiCol_TableBorderStrong]   // Table outer and header borders
    //   style.Colors[RainGuiCol_TableBorderLight]    // Table inner borders
    //   style.Colors[RainGuiCol_TableRowBg]          // Table row background when RainGuiTableFlags_RowBg is enabled (even rows)
    //   style.Colors[RainGuiCol_TableRowBgAlt]       // Table row background when RainGuiTableFlags_RowBg is enabled (odds rows)

    // Demos
    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Basic"))
    {
        // Here we will showcase three different ways to output a table.
        // They are very simple variations of a same thing!

        // [Method 1] Using TableNextRow() to create a new row, and TableSetColumnIndex() to select the column.
        // In many situations, this is the most flexible and easy to use pattern.
        HelpMarker("Using TableNextRow() + calling TableSetColumnIndex() _before_ each cell, in a loop.");
        if (RainGui::BeginTable("table1", 3))
        {
            for (int row = 0; row < 4; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("Row %d Column %d", row, column);
                }
            }
            RainGui::EndTable();
        }

        // [Method 2] Using TableNextColumn() called multiple times, instead of using a for loop + TableSetColumnIndex().
        // This is generally more convenient when you have code manually submitting the contents of each columns.
        HelpMarker("Using TableNextRow() + calling TableNextColumn() _before_ each cell, manually.");
        if (RainGui::BeginTable("table2", 3))
        {
            for (int row = 0; row < 4; row++)
            {
                RainGui::TableNextRow();
                RainGui::TableNextColumn();
                RainGui::Text("Row %d", row);
                RainGui::TableNextColumn();
                RainGui::Text("Some contents");
                RainGui::TableNextColumn();
                RainGui::Text("123.456");
            }
            RainGui::EndTable();
        }

        // [Method 3] We call TableNextColumn() _before_ each cell. We never call TableNextRow(),
        // as TableNextColumn() will automatically wrap around and create new roes as needed.
        // This is generally more convenient when your cells all contains the same type of data.
        HelpMarker(
            "Only using TableNextColumn(), which tends to be convenient for tables where every cells contains the same type of contents.\n"
            "This is also more similar to the old NextColumn() function of the Columns API, and provided to facilitate the Columns->Tables API transition.");
        if (RainGui::BeginTable("table3", 3))
        {
            for (int item = 0; item < 14; item++)
            {
                RainGui::TableNextColumn();
                RainGui::Text("Item %d", item);
            }
            RainGui::EndTable();
        }

        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Borders, background"))
    {
        // Expose a few Borders related flags interactively
        enum ContentsType { CT_Text, CT_FillButton };
        static RainGuiTableFlags flags = RainGuiTableFlags_Borders | RainGuiTableFlags_RowBg;
        static bool display_headers = false;
        static int contents_type = CT_Text;

        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_RowBg", &flags, RainGuiTableFlags_RowBg);
        RainGui::CheckboxFlags("RainGuiTableFlags_Borders", &flags, RainGuiTableFlags_Borders);
        RainGui::SameLine(); HelpMarker("RainGuiTableFlags_Borders\n = RainGuiTableFlags_BordersInnerV\n | RainGuiTableFlags_BordersOuterV\n | RainGuiTableFlags_BordersInnerV\n | RainGuiTableFlags_BordersOuterH");
        RainGui::Indent();

        RainGui::CheckboxFlags("RainGuiTableFlags_BordersH", &flags, RainGuiTableFlags_BordersH);
        RainGui::Indent();
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersOuterH", &flags, RainGuiTableFlags_BordersOuterH);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersInnerH", &flags, RainGuiTableFlags_BordersInnerH);
        RainGui::Unindent();

        RainGui::CheckboxFlags("RainGuiTableFlags_BordersV", &flags, RainGuiTableFlags_BordersV);
        RainGui::Indent();
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersOuterV", &flags, RainGuiTableFlags_BordersOuterV);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersInnerV", &flags, RainGuiTableFlags_BordersInnerV);
        RainGui::Unindent();

        RainGui::CheckboxFlags("RainGuiTableFlags_BordersOuter", &flags, RainGuiTableFlags_BordersOuter);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersInner", &flags, RainGuiTableFlags_BordersInner);
        RainGui::Unindent();

        RainGui::AlignTextToFramePadding(); RainGui::Text("Cell contents:");
        RainGui::SameLine(); RainGui::RadioButton("Text", &contents_type, CT_Text);
        RainGui::SameLine(); RainGui::RadioButton("FillButton", &contents_type, CT_FillButton);
        RainGui::Checkbox("Display headers", &display_headers);
        RainGui::CheckboxFlags("RainGuiTableFlags_NoBordersInBody", &flags, RainGuiTableFlags_NoBordersInBody); RainGui::SameLine(); HelpMarker("Disable vertical borders in columns Body (borders will always appears in Headers");
        PopStyleCompact();

        if (RainGui::BeginTable("table1", 3, flags))
        {
            // Display headers so we can inspect their interaction with borders.
            // (Headers are not the main purpose of this section of the demo, so we are not elaborating on them too much. See other sections for details)
            if (display_headers)
            {
                RainGui::TableSetupColumn("One");
                RainGui::TableSetupColumn("Two");
                RainGui::TableSetupColumn("Three");
                RainGui::TableHeadersRow();
            }

            for (int row = 0; row < 5; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    char buf[32];
                    sprintf(buf, "Hello %d,%d", column, row);
                    if (contents_type == CT_Text)
                        RainGui::TextUnformatted(buf);
                    else if (contents_type)
                        RainGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                }
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Resizable, stretch"))
    {
        // By default, if we don't enable ScrollX the sizing policy for each columns is "Stretch"
        // Each columns maintain a sizing weight, and they will occupy all available width.
        static RainGuiTableFlags flags = RainGuiTableFlags_SizingStretchSame | RainGuiTableFlags_Resizable | RainGuiTableFlags_BordersOuter | RainGuiTableFlags_BordersV | RainGuiTableFlags_ContextMenuInBody;
        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_Resizable", &flags, RainGuiTableFlags_Resizable);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersV", &flags, RainGuiTableFlags_BordersV);
        RainGui::SameLine(); HelpMarker("Using the _Resizable flag automatically enables the _BordersInnerV flag as well, this is why the resize borders are still showing when unchecking this.");
        PopStyleCompact();

        if (RainGui::BeginTable("table1", 3, flags))
        {
            for (int row = 0; row < 5; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("Hello %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Resizable, fixed"))
    {
        // Here we use RainGuiTableFlags_SizingFixedFit (even though _ScrollX is not set)
        // So columns will adopt the "Fixed" policy and will maintain a fixed width regardless of the whole available width (unless table is small)
        // If there is not enough available width to fit all columns, they will however be resized down.
        // FIXME-TABLE: Providing a stretch-on-init would make sense especially for tables which don't have saved settings
        HelpMarker(
            "Using _Resizable + _SizingFixedFit flags.\n"
            "Fixed-width columns generally makes more sense if you want to use horizontal scrolling.\n\n"
            "Double-click a column border to auto-fit the column to its contents.");
        PushStyleCompact();
        static RainGuiTableFlags flags = RainGuiTableFlags_SizingFixedFit | RainGuiTableFlags_Resizable | RainGuiTableFlags_BordersOuter | RainGuiTableFlags_BordersV | RainGuiTableFlags_ContextMenuInBody;
        RainGui::CheckboxFlags("RainGuiTableFlags_NoHostExtendX", &flags, RainGuiTableFlags_NoHostExtendX);
        PopStyleCompact();

        if (RainGui::BeginTable("table1", 3, flags))
        {
            for (int row = 0; row < 5; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("Hello %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Resizable, mixed"))
    {
        HelpMarker(
            "Using TableSetupColumn() to alter resizing policy on a per-column basis.\n\n"
            "When combining Fixed and Stretch columns, generally you only want one, maybe two trailing columns to use _WidthStretch.");
        static RainGuiTableFlags flags = RainGuiTableFlags_SizingFixedFit | RainGuiTableFlags_RowBg | RainGuiTableFlags_Borders | RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable;

        if (RainGui::BeginTable("table1", 3, flags))
        {
            RainGui::TableSetupColumn("AAA", RainGuiTableColumnFlags_WidthFixed);
            RainGui::TableSetupColumn("BBB", RainGuiTableColumnFlags_WidthFixed);
            RainGui::TableSetupColumn("CCC", RainGuiTableColumnFlags_WidthStretch);
            RainGui::TableHeadersRow();
            for (int row = 0; row < 5; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("%s %d,%d", (column == 2) ? "Stretch" : "Fixed", column, row);
                }
            }
            RainGui::EndTable();
        }
        if (RainGui::BeginTable("table2", 6, flags))
        {
            RainGui::TableSetupColumn("AAA", RainGuiTableColumnFlags_WidthFixed);
            RainGui::TableSetupColumn("BBB", RainGuiTableColumnFlags_WidthFixed);
            RainGui::TableSetupColumn("CCC", RainGuiTableColumnFlags_WidthFixed | RainGuiTableColumnFlags_DefaultHide);
            RainGui::TableSetupColumn("DDD", RainGuiTableColumnFlags_WidthStretch);
            RainGui::TableSetupColumn("EEE", RainGuiTableColumnFlags_WidthStretch);
            RainGui::TableSetupColumn("FFF", RainGuiTableColumnFlags_WidthStretch | RainGuiTableColumnFlags_DefaultHide);
            RainGui::TableHeadersRow();
            for (int row = 0; row < 5; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 6; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("%s %d,%d", (column >= 3) ? "Stretch" : "Fixed", column, row);
                }
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Reorderable, hideable, with headers"))
    {
        HelpMarker(
            "Click and drag column headers to reorder columns.\n\n"
            "Right-click on a header to open a context menu.");
        static RainGuiTableFlags flags = RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable | RainGuiTableFlags_BordersOuter | RainGuiTableFlags_BordersV;
        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_Resizable", &flags, RainGuiTableFlags_Resizable);
        RainGui::CheckboxFlags("RainGuiTableFlags_Reorderable", &flags, RainGuiTableFlags_Reorderable);
        RainGui::CheckboxFlags("RainGuiTableFlags_Hideable", &flags, RainGuiTableFlags_Hideable);
        RainGui::CheckboxFlags("RainGuiTableFlags_NoBordersInBody", &flags, RainGuiTableFlags_NoBordersInBody);
        RainGui::CheckboxFlags("RainGuiTableFlags_NoBordersInBodyUntilResize", &flags, RainGuiTableFlags_NoBordersInBodyUntilResize); RainGui::SameLine(); HelpMarker("Disable vertical borders in columns Body until hovered for resize (borders will always appears in Headers)");
        PopStyleCompact();

        if (RainGui::BeginTable("table1", 3, flags))
        {
            // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
            // (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
            RainGui::TableSetupColumn("One");
            RainGui::TableSetupColumn("Two");
            RainGui::TableSetupColumn("Three");
            RainGui::TableHeadersRow();
            for (int row = 0; row < 6; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("Hello %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }

        // Use outer_size.x == 0.0f instead of default to make the table as tight as possible (only valid when no scrolling and no stretch column)
        if (RainGui::BeginTable("table2", 3, flags | RainGuiTableFlags_SizingFixedFit, ImVec2(0.0f, 0.0f)))
        {
            RainGui::TableSetupColumn("One");
            RainGui::TableSetupColumn("Two");
            RainGui::TableSetupColumn("Three");
            RainGui::TableHeadersRow();
            for (int row = 0; row < 6; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("Fixed %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Padding"))
    {
        // First example: showcase use of padding flags and effect of BorderOuterV/BorderInnerV on X padding.
        // We don't expose BorderOuterH/BorderInnerH here because they have no effect on X padding.
        HelpMarker(
            "We often want outer padding activated when any using features which makes the edges of a column visible:\n"
            "e.g.:\n"
            "- BorderOuterV\n"
            "- any form of row selection\n"
            "Because of this, activating BorderOuterV sets the default to PadOuterX. Using PadOuterX or NoPadOuterX you can override the default.\n\n"
            "Actual padding values are using style.CellPadding.\n\n"
            "In this demo we don't show horizontal borders to emphasis how they don't affect default horizontal padding.");

        static RainGuiTableFlags flags1 = RainGuiTableFlags_BordersV;
        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_PadOuterX", &flags1, RainGuiTableFlags_PadOuterX);
        RainGui::SameLine(); HelpMarker("Enable outer-most padding (default if RainGuiTableFlags_BordersOuterV is set)");
        RainGui::CheckboxFlags("RainGuiTableFlags_NoPadOuterX", &flags1, RainGuiTableFlags_NoPadOuterX);
        RainGui::SameLine(); HelpMarker("Disable outer-most padding (default if RainGuiTableFlags_BordersOuterV is not set)");
        RainGui::CheckboxFlags("RainGuiTableFlags_NoPadInnerX", &flags1, RainGuiTableFlags_NoPadInnerX);
        RainGui::SameLine(); HelpMarker("Disable inner padding between columns (double inner padding if BordersOuterV is on, single inner padding if BordersOuterV is off)");
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersOuterV", &flags1, RainGuiTableFlags_BordersOuterV);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersInnerV", &flags1, RainGuiTableFlags_BordersInnerV);
        static bool show_headers = false;
        RainGui::Checkbox("show_headers", &show_headers);
        PopStyleCompact();

        if (RainGui::BeginTable("table_padding", 3, flags1))
        {
            if (show_headers)
            {
                RainGui::TableSetupColumn("One");
                RainGui::TableSetupColumn("Two");
                RainGui::TableSetupColumn("Three");
                RainGui::TableHeadersRow();
            }

            for (int row = 0; row < 5; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    if (row == 0)
                    {
                        RainGui::Text("Avail %.2f", RainGui::GetContentRegionAvail().x);
                    }
                    else
                    {
                        char buf[32];
                        sprintf(buf, "Hello %d,%d", column, row);
                        RainGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
                    }
                    //if (RainGui::TableGetColumnFlags() & RainGuiTableColumnFlags_IsHovered)
                    //    RainGui::TableSetBgColor(RainGuiTableBgTarget_CellBg, IM_COL32(0, 100, 0, 255));
                }
            }
            RainGui::EndTable();
        }

        // Second example: set style.CellPadding to (0.0) or a custom value.
        // FIXME-TABLE: Vertical border effectively not displayed the same way as horizontal one...
        HelpMarker("Setting style.CellPadding to (0,0) or a custom value.");
        static RainGuiTableFlags flags2 = RainGuiTableFlags_Borders | RainGuiTableFlags_RowBg;
        static ImVec2 cell_padding(0.0f, 0.0f);
        static bool show_widget_frame_bg = true;

        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_Borders", &flags2, RainGuiTableFlags_Borders);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersH", &flags2, RainGuiTableFlags_BordersH);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersV", &flags2, RainGuiTableFlags_BordersV);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersInner", &flags2, RainGuiTableFlags_BordersInner);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersOuter", &flags2, RainGuiTableFlags_BordersOuter);
        RainGui::CheckboxFlags("RainGuiTableFlags_RowBg", &flags2, RainGuiTableFlags_RowBg);
        RainGui::CheckboxFlags("RainGuiTableFlags_Resizable", &flags2, RainGuiTableFlags_Resizable);
        RainGui::Checkbox("show_widget_frame_bg", &show_widget_frame_bg);
        RainGui::SliderFloat2("CellPadding", &cell_padding.x, 0.0f, 10.0f, "%.0f");
        PopStyleCompact();

        RainGui::PushStyleVar(RainGuiStyleVar_CellPadding, cell_padding);
        if (RainGui::BeginTable("table_padding_2", 3, flags2))
        {
            static char text_bufs[3 * 5][16]; // Mini text storage for 3x5 cells
            static bool init = true;
            if (!show_widget_frame_bg)
                RainGui::PushStyleColor(RainGuiCol_FrameBg, 0);
            for (int cell = 0; cell < 3 * 5; cell++)
            {
                RainGui::TableNextColumn();
                if (init)
                    strcpy(text_bufs[cell], "edit me");
                RainGui::SetNextItemWidth(-FLT_MIN);
                RainGui::PushID(cell);
                RainGui::InputText("##cell", text_bufs[cell], IM_ARRAYSIZE(text_bufs[cell]));
                RainGui::PopID();
            }
            if (!show_widget_frame_bg)
                RainGui::PopStyleColor();
            init = false;
            RainGui::EndTable();
        }
        RainGui::PopStyleVar();

        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Sizing policies"))
    {
        static RainGuiTableFlags flags1 = RainGuiTableFlags_BordersV | RainGuiTableFlags_BordersOuterH | RainGuiTableFlags_RowBg | RainGuiTableFlags_ContextMenuInBody;
        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_Resizable", &flags1, RainGuiTableFlags_Resizable);
        RainGui::CheckboxFlags("RainGuiTableFlags_NoHostExtendX", &flags1, RainGuiTableFlags_NoHostExtendX);
        PopStyleCompact();

        static RainGuiTableFlags sizing_policy_flags[4] = { RainGuiTableFlags_SizingFixedFit, RainGuiTableFlags_SizingFixedSame, RainGuiTableFlags_SizingStretchProp, RainGuiTableFlags_SizingStretchSame };
        for (int table_n = 0; table_n < 4; table_n++)
        {
            RainGui::PushID(table_n);
            RainGui::SetNextItemWidth(TEXT_BASE_WIDTH * 30);
            EditTableSizingFlags(&sizing_policy_flags[table_n]);

            // To make it easier to understand the different sizing policy,
            // For each policy: we display one table where the columns have equal contents width, and one where the columns have different contents width.
            if (RainGui::BeginTable("table1", 3, sizing_policy_flags[table_n] | flags1))
            {
                for (int row = 0; row < 3; row++)
                {
                    RainGui::TableNextRow();
                    RainGui::TableNextColumn(); RainGui::Text("Oh dear");
                    RainGui::TableNextColumn(); RainGui::Text("Oh dear");
                    RainGui::TableNextColumn(); RainGui::Text("Oh dear");
                }
                RainGui::EndTable();
            }
            if (RainGui::BeginTable("table2", 3, sizing_policy_flags[table_n] | flags1))
            {
                for (int row = 0; row < 3; row++)
                {
                    RainGui::TableNextRow();
                    RainGui::TableNextColumn(); RainGui::Text("AAAA");
                    RainGui::TableNextColumn(); RainGui::Text("BBBBBBBB");
                    RainGui::TableNextColumn(); RainGui::Text("CCCCCCCCCCCC");
                }
                RainGui::EndTable();
            }
            RainGui::PopID();
        }

        RainGui::Spacing();
        RainGui::TextUnformatted("Advanced");
        RainGui::SameLine();
        HelpMarker("This section allows you to interact and see the effect of various sizing policies depending on whether Scroll is enabled and the contents of your columns.");

        enum ContentsType { CT_ShowWidth, CT_ShortText, CT_LongText, CT_Button, CT_FillButton, CT_InputText };
        static RainGuiTableFlags flags = RainGuiTableFlags_ScrollY | RainGuiTableFlags_Borders | RainGuiTableFlags_RowBg | RainGuiTableFlags_Resizable;
        static int contents_type = CT_ShowWidth;
        static int column_count = 3;

        PushStyleCompact();
        RainGui::PushID("Advanced");
        RainGui::PushItemWidth(TEXT_BASE_WIDTH * 30);
        EditTableSizingFlags(&flags);
        RainGui::Combo("Contents", &contents_type, "Show width\0Short Text\0Long Text\0Button\0Fill Button\0InputText\0");
        if (contents_type == CT_FillButton)
        {
            RainGui::SameLine();
            HelpMarker("Be mindful that using right-alignment (e.g. size.x = -FLT_MIN) creates a feedback loop where contents width can feed into auto-column width can feed into contents width.");
        }
        RainGui::DragInt("Columns", &column_count, 0.1f, 1, 64, "%d", RainGuiSliderFlags_AlwaysClamp);
        RainGui::CheckboxFlags("RainGuiTableFlags_Resizable", &flags, RainGuiTableFlags_Resizable);
        RainGui::CheckboxFlags("RainGuiTableFlags_PreciseWidths", &flags, RainGuiTableFlags_PreciseWidths);
        RainGui::SameLine(); HelpMarker("Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.");
        RainGui::CheckboxFlags("RainGuiTableFlags_ScrollX", &flags, RainGuiTableFlags_ScrollX);
        RainGui::CheckboxFlags("RainGuiTableFlags_ScrollY", &flags, RainGuiTableFlags_ScrollY);
        RainGui::CheckboxFlags("RainGuiTableFlags_NoClip", &flags, RainGuiTableFlags_NoClip);
        RainGui::PopItemWidth();
        RainGui::PopID();
        PopStyleCompact();

        if (RainGui::BeginTable("table2", column_count, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 7)))
        {
            for (int cell = 0; cell < 10 * column_count; cell++)
            {
                RainGui::TableNextColumn();
                int column = RainGui::TableGetColumnIndex();
                int row = RainGui::TableGetRowIndex();

                RainGui::PushID(cell);
                char label[32];
                static char text_buf[32] = "";
                sprintf(label, "Hello %d,%d", column, row);
                switch (contents_type)
                {
                case CT_ShortText:  RainGui::TextUnformatted(label); break;
                case CT_LongText:   RainGui::Text("Some %s text %d,%d\nOver two lines..", column == 0 ? "long" : "longeeer", column, row); break;
                case CT_ShowWidth:  RainGui::Text("W: %.1f", RainGui::GetContentRegionAvail().x); break;
                case CT_Button:     RainGui::Button(label); break;
                case CT_FillButton: RainGui::Button(label, ImVec2(-FLT_MIN, 0.0f)); break;
                case CT_InputText:  RainGui::SetNextItemWidth(-FLT_MIN); RainGui::InputText("##", text_buf, IM_ARRAYSIZE(text_buf)); break;
                }
                RainGui::PopID();
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Vertical scrolling, with clipping"))
    {
        HelpMarker("Here we activate ScrollY, which will create a child window container to allow hosting scrollable contents.\n\nWe also demonstrate using RainGuiListClipper to virtualize the submission of many items.");
        static RainGuiTableFlags flags = RainGuiTableFlags_ScrollY | RainGuiTableFlags_RowBg | RainGuiTableFlags_BordersOuter | RainGuiTableFlags_BordersV | RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable;

        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_ScrollY", &flags, RainGuiTableFlags_ScrollY);
        PopStyleCompact();

        // When using ScrollX or ScrollY we need to specify a size for our table container!
        // Otherwise by default the table will fit all available space, like a BeginChild() call.
        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
        if (RainGui::BeginTable("table_scrolly", 3, flags, outer_size))
        {
            RainGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
            RainGui::TableSetupColumn("One", RainGuiTableColumnFlags_None);
            RainGui::TableSetupColumn("Two", RainGuiTableColumnFlags_None);
            RainGui::TableSetupColumn("Three", RainGuiTableColumnFlags_None);
            RainGui::TableHeadersRow();

            // Demonstrate using clipper for large vertical lists
            RainGuiListClipper clipper;
            clipper.Begin(1000);
            while (clipper.Step())
            {
                for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                {
                    RainGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        RainGui::TableSetColumnIndex(column);
                        RainGui::Text("Hello %d,%d", column, row);
                    }
                }
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Horizontal scrolling"))
    {
        HelpMarker(
            "When ScrollX is enabled, the default sizing policy becomes RainGuiTableFlags_SizingFixedFit, "
            "as automatically stretching columns doesn't make much sense with horizontal scrolling.\n\n"
            "Also note that as of the current version, you will almost always want to enable ScrollY along with ScrollX,"
            "because the container window won't automatically extend vertically to fix contents (this may be improved in future versions).");
        static RainGuiTableFlags flags = RainGuiTableFlags_ScrollX | RainGuiTableFlags_ScrollY | RainGuiTableFlags_RowBg | RainGuiTableFlags_BordersOuter | RainGuiTableFlags_BordersV | RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable;
        static int freeze_cols = 1;
        static int freeze_rows = 1;

        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_Resizable", &flags, RainGuiTableFlags_Resizable);
        RainGui::CheckboxFlags("RainGuiTableFlags_ScrollX", &flags, RainGuiTableFlags_ScrollX);
        RainGui::CheckboxFlags("RainGuiTableFlags_ScrollY", &flags, RainGuiTableFlags_ScrollY);
        RainGui::SetNextItemWidth(RainGui::GetFrameHeight());
        RainGui::DragInt("freeze_cols", &freeze_cols, 0.2f, 0, 9, NULL, RainGuiSliderFlags_NoInput);
        RainGui::SetNextItemWidth(RainGui::GetFrameHeight());
        RainGui::DragInt("freeze_rows", &freeze_rows, 0.2f, 0, 9, NULL, RainGuiSliderFlags_NoInput);
        PopStyleCompact();

        // When using ScrollX or ScrollY we need to specify a size for our table container!
        // Otherwise by default the table will fit all available space, like a BeginChild() call.
        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
        if (RainGui::BeginTable("table_scrollx", 7, flags, outer_size))
        {
            RainGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
            RainGui::TableSetupColumn("Line #", RainGuiTableColumnFlags_NoHide); // Make the first column not hideable to match our use of TableSetupScrollFreeze()
            RainGui::TableSetupColumn("One");
            RainGui::TableSetupColumn("Two");
            RainGui::TableSetupColumn("Three");
            RainGui::TableSetupColumn("Four");
            RainGui::TableSetupColumn("Five");
            RainGui::TableSetupColumn("Six");
            RainGui::TableHeadersRow();
            for (int row = 0; row < 20; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 7; column++)
                {
                    // Both TableNextColumn() and TableSetColumnIndex() return true when a column is visible or performing width measurement.
                    // Because here we know that:
                    // - A) all our columns are contributing the same to row height
                    // - B) column 0 is always visible,
                    // We only always submit this one column and can skip others.
                    // More advanced per-column clipping behaviors may benefit from polling the status flags via TableGetColumnFlags().
                    if (!RainGui::TableSetColumnIndex(column) && column > 0)
                        continue;
                    if (column == 0)
                        RainGui::Text("Line %d", row);
                    else
                        RainGui::Text("Hello world %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }

        RainGui::Spacing();
        RainGui::TextUnformatted("Stretch + ScrollX");
        RainGui::SameLine();
        HelpMarker(
            "Showcase using Stretch columns + ScrollX together: "
            "this is rather unusual and only makes sense when specifying an 'inner_width' for the table!\n"
            "Without an explicit value, inner_width is == outer_size.x and therefore using Stretch columns + ScrollX together doesn't make sense.");
        static RainGuiTableFlags flags2 = RainGuiTableFlags_SizingStretchSame | RainGuiTableFlags_ScrollX | RainGuiTableFlags_ScrollY | RainGuiTableFlags_BordersOuter | RainGuiTableFlags_RowBg | RainGuiTableFlags_ContextMenuInBody;
        static float inner_width = 1000.0f;
        PushStyleCompact();
        RainGui::PushID("flags3");
        RainGui::PushItemWidth(TEXT_BASE_WIDTH * 30);
        RainGui::CheckboxFlags("RainGuiTableFlags_ScrollX", &flags2, RainGuiTableFlags_ScrollX);
        RainGui::DragFloat("inner_width", &inner_width, 1.0f, 0.0f, FLT_MAX, "%.1f");
        RainGui::PopItemWidth();
        RainGui::PopID();
        PopStyleCompact();
        if (RainGui::BeginTable("table2", 7, flags2, outer_size, inner_width))
        {
            for (int cell = 0; cell < 20 * 7; cell++)
            {
                RainGui::TableNextColumn();
                RainGui::Text("Hello world %d,%d", RainGui::TableGetColumnIndex(), RainGui::TableGetRowIndex());
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Columns flags"))
    {
        // Create a first table just to show all the options/flags we want to make visible in our example!
        const int column_count = 3;
        const char* column_names[column_count] = { "One", "Two", "Three" };
        static RainGuiTableColumnFlags column_flags[column_count] = { RainGuiTableColumnFlags_DefaultSort, RainGuiTableColumnFlags_None, RainGuiTableColumnFlags_DefaultHide };
        static RainGuiTableColumnFlags column_flags_out[column_count] = { 0, 0, 0 }; // Output from TableGetColumnFlags()

        if (RainGui::BeginTable("table_columns_flags_checkboxes", column_count, RainGuiTableFlags_None))
        {
            PushStyleCompact();
            for (int column = 0; column < column_count; column++)
            {
                RainGui::TableNextColumn();
                RainGui::PushID(column);
                RainGui::AlignTextToFramePadding(); // FIXME-TABLE: Workaround for wrong text baseline propagation
                RainGui::Text("'%s'", column_names[column]);
                RainGui::Spacing();
                RainGui::Text("Input flags:");
                EditTableColumnsFlags(&column_flags[column]);
                RainGui::Spacing();
                RainGui::Text("Output flags:");
                ShowTableColumnsStatusFlags(column_flags_out[column]);
                RainGui::PopID();
            }
            PopStyleCompact();
            RainGui::EndTable();
        }

        // Create the real table we care about for the example!
        // We use a scrolling table to be able to showcase the difference between the _IsEnabled and _IsVisible flags above, otherwise in
        // a non-scrolling table columns are always visible (unless using RainGuiTableFlags_NoKeepColumnsVisible + resizing the parent window down)
        const RainGuiTableFlags flags
            = RainGuiTableFlags_SizingFixedFit | RainGuiTableFlags_ScrollX | RainGuiTableFlags_ScrollY
            | RainGuiTableFlags_RowBg | RainGuiTableFlags_BordersOuter | RainGuiTableFlags_BordersV
            | RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable | RainGuiTableFlags_Sortable;
        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 9);
        if (RainGui::BeginTable("table_columns_flags", column_count, flags, outer_size))
        {
            for (int column = 0; column < column_count; column++)
                RainGui::TableSetupColumn(column_names[column], column_flags[column]);
            RainGui::TableHeadersRow();
            for (int column = 0; column < column_count; column++)
                column_flags_out[column] = RainGui::TableGetColumnFlags(column);
            float indent_step = (float)((int)TEXT_BASE_WIDTH / 2);
            for (int row = 0; row < 8; row++)
            {
                RainGui::Indent(indent_step); // Add some indentation to demonstrate usage of per-column IndentEnable/IndentDisable flags.
                RainGui::TableNextRow();
                for (int column = 0; column < column_count; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("%s %s", (column == 0) ? "Indented" : "Hello", RainGui::TableGetColumnName(column));
                }
            }
            RainGui::Unindent(indent_step * 8.0f);

            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Columns widths"))
    {
        HelpMarker("Using TableSetupColumn() to setup default width.");

        static RainGuiTableFlags flags1 = RainGuiTableFlags_Borders | RainGuiTableFlags_NoBordersInBodyUntilResize;
        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_Resizable", &flags1, RainGuiTableFlags_Resizable);
        RainGui::CheckboxFlags("RainGuiTableFlags_NoBordersInBodyUntilResize", &flags1, RainGuiTableFlags_NoBordersInBodyUntilResize);
        PopStyleCompact();
        if (RainGui::BeginTable("table1", 3, flags1))
        {
            // We could also set RainGuiTableFlags_SizingFixedFit on the table and all columns will default to RainGuiTableColumnFlags_WidthFixed.
            RainGui::TableSetupColumn("one", RainGuiTableColumnFlags_WidthFixed, 100.0f); // Default to 100.0f
            RainGui::TableSetupColumn("two", RainGuiTableColumnFlags_WidthFixed, 200.0f); // Default to 200.0f
            RainGui::TableSetupColumn("three", RainGuiTableColumnFlags_WidthFixed);       // Default to auto
            RainGui::TableHeadersRow();
            for (int row = 0; row < 4; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    if (row == 0)
                        RainGui::Text("(w: %5.1f)", RainGui::GetContentRegionAvail().x);
                    else
                        RainGui::Text("Hello %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }

        HelpMarker("Using TableSetupColumn() to setup explicit width.\n\nUnless _NoKeepColumnsVisible is set, fixed columns with set width may still be shrunk down if there's not enough space in the host.");

        static RainGuiTableFlags flags2 = RainGuiTableFlags_None;
        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_NoKeepColumnsVisible", &flags2, RainGuiTableFlags_NoKeepColumnsVisible);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersInnerV", &flags2, RainGuiTableFlags_BordersInnerV);
        RainGui::CheckboxFlags("RainGuiTableFlags_BordersOuterV", &flags2, RainGuiTableFlags_BordersOuterV);
        PopStyleCompact();
        if (RainGui::BeginTable("table2", 4, flags2))
        {
            // We could also set RainGuiTableFlags_SizingFixedFit on the table and all columns will default to RainGuiTableColumnFlags_WidthFixed.
            RainGui::TableSetupColumn("", RainGuiTableColumnFlags_WidthFixed, 100.0f);
            RainGui::TableSetupColumn("", RainGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);
            RainGui::TableSetupColumn("", RainGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 30.0f);
            RainGui::TableSetupColumn("", RainGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 15.0f);
            for (int row = 0; row < 5; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 4; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    if (row == 0)
                        RainGui::Text("(w: %5.1f)", RainGui::GetContentRegionAvail().x);
                    else
                        RainGui::Text("Hello %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Nested tables"))
    {
        HelpMarker("This demonstrate embedding a table into another table cell.");

        if (RainGui::BeginTable("table_nested1", 2, RainGuiTableFlags_Borders | RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable))
        {
            RainGui::TableSetupColumn("A0");
            RainGui::TableSetupColumn("A1");
            RainGui::TableHeadersRow();

            RainGui::TableNextColumn();
            RainGui::Text("A0 Row 0");
            {
                float rows_height = TEXT_BASE_HEIGHT * 2;
                if (RainGui::BeginTable("table_nested2", 2, RainGuiTableFlags_Borders | RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable))
                {
                    RainGui::TableSetupColumn("B0");
                    RainGui::TableSetupColumn("B1");
                    RainGui::TableHeadersRow();

                    RainGui::TableNextRow(RainGuiTableRowFlags_None, rows_height);
                    RainGui::TableNextColumn();
                    RainGui::Text("B0 Row 0");
                    RainGui::TableNextColumn();
                    RainGui::Text("B1 Row 0");
                    RainGui::TableNextRow(RainGuiTableRowFlags_None, rows_height);
                    RainGui::TableNextColumn();
                    RainGui::Text("B0 Row 1");
                    RainGui::TableNextColumn();
                    RainGui::Text("B1 Row 1");

                    RainGui::EndTable();
                }
            }
            RainGui::TableNextColumn(); RainGui::Text("A1 Row 0");
            RainGui::TableNextColumn(); RainGui::Text("A0 Row 1");
            RainGui::TableNextColumn(); RainGui::Text("A1 Row 1");
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Row height"))
    {
        HelpMarker("You can pass a 'min_row_height' to TableNextRow().\n\nRows are padded with 'style.CellPadding.y' on top and bottom, so effectively the minimum row height will always be >= 'style.CellPadding.y * 2.0f'.\n\nWe cannot honor a _maximum_ row height as that would requires a unique clipping rectangle per row.");
        if (RainGui::BeginTable("table_row_height", 1, RainGuiTableFlags_BordersOuter | RainGuiTableFlags_BordersInnerV))
        {
            for (int row = 0; row < 10; row++)
            {
                float min_row_height = (float)(int)(TEXT_BASE_HEIGHT * 0.30f * row);
                RainGui::TableNextRow(RainGuiTableRowFlags_None, min_row_height);
                RainGui::TableNextColumn();
                RainGui::Text("min_row_height = %.2f", min_row_height);
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Outer size"))
    {
        // Showcasing use of RainGuiTableFlags_NoHostExtendX and RainGuiTableFlags_NoHostExtendY
        // Important to that note how the two flags have slightly different behaviors!
        RainGui::Text("Using NoHostExtendX and NoHostExtendY:");
        PushStyleCompact();
        static RainGuiTableFlags flags = RainGuiTableFlags_Borders | RainGuiTableFlags_Resizable | RainGuiTableFlags_ContextMenuInBody | RainGuiTableFlags_RowBg | RainGuiTableFlags_SizingFixedFit | RainGuiTableFlags_NoHostExtendX;
        RainGui::CheckboxFlags("RainGuiTableFlags_NoHostExtendX", &flags, RainGuiTableFlags_NoHostExtendX);
        RainGui::SameLine(); HelpMarker("Make outer width auto-fit to columns, overriding outer_size.x value.\n\nOnly available when ScrollX/ScrollY are disabled and Stretch columns are not used.");
        RainGui::CheckboxFlags("RainGuiTableFlags_NoHostExtendY", &flags, RainGuiTableFlags_NoHostExtendY);
        RainGui::SameLine(); HelpMarker("Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit).\n\nOnly available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.");
        PopStyleCompact();

        ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 5.5f);
        if (RainGui::BeginTable("table1", 3, flags, outer_size))
        {
            for (int row = 0; row < 10; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableNextColumn();
                    RainGui::Text("Cell %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }
        RainGui::SameLine();
        RainGui::Text("Hello!");

        RainGui::Spacing();

        RainGui::Text("Using explicit size:");
        if (RainGui::BeginTable("table2", 3, RainGuiTableFlags_Borders | RainGuiTableFlags_RowBg, ImVec2(TEXT_BASE_WIDTH * 30, 0.0f)))
        {
            for (int row = 0; row < 5; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableNextColumn();
                    RainGui::Text("Cell %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }
        RainGui::SameLine();
        if (RainGui::BeginTable("table3", 3, RainGuiTableFlags_Borders | RainGuiTableFlags_RowBg, ImVec2(TEXT_BASE_WIDTH * 30, 0.0f)))
        {
            for (int row = 0; row < 3; row++)
            {
                RainGui::TableNextRow(0, TEXT_BASE_HEIGHT * 1.5f);
                for (int column = 0; column < 3; column++)
                {
                    RainGui::TableNextColumn();
                    RainGui::Text("Cell %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }

        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Background color"))
    {
        static RainGuiTableFlags flags = RainGuiTableFlags_RowBg;
        static int row_bg_type = 1;
        static int row_bg_target = 1;
        static int cell_bg_type = 1;

        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_Borders", &flags, RainGuiTableFlags_Borders);
        RainGui::CheckboxFlags("RainGuiTableFlags_RowBg", &flags, RainGuiTableFlags_RowBg);
        RainGui::SameLine(); HelpMarker("RainGuiTableFlags_RowBg automatically sets RowBg0 to alternative colors pulled from the Style.");
        RainGui::Combo("row bg type", (int*)&row_bg_type, "None\0Red\0Gradient\0");
        RainGui::Combo("row bg target", (int*)&row_bg_target, "RowBg0\0RowBg1\0"); RainGui::SameLine(); HelpMarker("Target RowBg0 to override the alternating odd/even colors,\nTarget RowBg1 to blend with them.");
        RainGui::Combo("cell bg type", (int*)&cell_bg_type, "None\0Blue\0"); RainGui::SameLine(); HelpMarker("We are colorizing cells to B1->C2 here.");
        IM_ASSERT(row_bg_type >= 0 && row_bg_type <= 2);
        IM_ASSERT(row_bg_target >= 0 && row_bg_target <= 1);
        IM_ASSERT(cell_bg_type >= 0 && cell_bg_type <= 1);
        PopStyleCompact();

        if (RainGui::BeginTable("table1", 5, flags))
        {
            for (int row = 0; row < 6; row++)
            {
                RainGui::TableNextRow();

                // Demonstrate setting a row background color with 'RainGui::TableSetBgColor(RainGuiTableBgTarget_RowBgX, ...)'
                // We use a transparent color so we can see the one behind in case our target is RowBg1 and RowBg0 was already targeted by the RainGuiTableFlags_RowBg flag.
                if (row_bg_type != 0)
                {
                    ImU32 row_bg_color = RainGui::GetColorU32(row_bg_type == 1 ? ImVec4(0.7f, 0.3f, 0.3f, 0.65f) : ImVec4(0.2f + row * 0.1f, 0.2f, 0.2f, 0.65f)); // Flat or Gradient?
                    RainGui::TableSetBgColor(RainGuiTableBgTarget_RowBg0 + row_bg_target, row_bg_color);
                }

                // Fill cells
                for (int column = 0; column < 5; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("%c%c", 'A' + row, '0' + column);

                    // Change background of Cells B1->C2
                    // Demonstrate setting a cell background color with 'RainGui::TableSetBgColor(RainGuiTableBgTarget_CellBg, ...)'
                    // (the CellBg color will be blended over the RowBg and ColumnBg colors)
                    // We can also pass a column number as a third parameter to TableSetBgColor() and do this outside the column loop.
                    if (row >= 1 && row <= 2 && column >= 1 && column <= 2 && cell_bg_type == 1)
                    {
                        ImU32 cell_bg_color = RainGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                        RainGui::TableSetBgColor(RainGuiTableBgTarget_CellBg, cell_bg_color);
                    }
                }
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Tree view"))
    {
        static RainGuiTableFlags flags = RainGuiTableFlags_BordersV | RainGuiTableFlags_BordersOuterH | RainGuiTableFlags_Resizable | RainGuiTableFlags_RowBg | RainGuiTableFlags_NoBordersInBody;

        if (RainGui::BeginTable("3ways", 3, flags))
        {
            // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
            RainGui::TableSetupColumn("Name", RainGuiTableColumnFlags_NoHide);
            RainGui::TableSetupColumn("Size", RainGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            RainGui::TableSetupColumn("Type", RainGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            RainGui::TableHeadersRow();

            // Simple storage to output a dummy file-system.
            struct MyTreeNode
            {
                const char*     Name;
                const char*     Type;
                int             Size;
                int             ChildIdx;
                int             ChildCount;
                static void DisplayNode(const MyTreeNode* node, const MyTreeNode* all_nodes)
                {
                    RainGui::TableNextRow();
                    RainGui::TableNextColumn();
                    const bool is_folder = (node->ChildCount > 0);
                    if (is_folder)
                    {
                        bool open = RainGui::TreeNodeEx(node->Name, RainGuiTreeNodeFlags_SpanFullWidth);
                        RainGui::TableNextColumn();
                        RainGui::TextDisabled("--");
                        RainGui::TableNextColumn();
                        RainGui::TextUnformatted(node->Type);
                        if (open)
                        {
                            for (int child_n = 0; child_n < node->ChildCount; child_n++)
                                DisplayNode(&all_nodes[node->ChildIdx + child_n], all_nodes);
                            RainGui::TreePop();
                        }
                    }
                    else
                    {
                        RainGui::TreeNodeEx(node->Name, RainGuiTreeNodeFlags_Leaf | RainGuiTreeNodeFlags_Bullet | RainGuiTreeNodeFlags_NoTreePushOnOpen | RainGuiTreeNodeFlags_SpanFullWidth);
                        RainGui::TableNextColumn();
                        RainGui::Text("%d", node->Size);
                        RainGui::TableNextColumn();
                        RainGui::TextUnformatted(node->Type);
                    }
                }
            };
            static const MyTreeNode nodes[] =
            {
                { "Root",                         "Folder",       -1,       1, 3    }, // 0
                { "Music",                        "Folder",       -1,       4, 2    }, // 1
                { "Textures",                     "Folder",       -1,       6, 3    }, // 2
                { "desktop.ini",                  "System file",  1024,    -1,-1    }, // 3
                { "File1_a.wav",                  "Audio file",   123000,  -1,-1    }, // 4
                { "File1_b.wav",                  "Audio file",   456000,  -1,-1    }, // 5
                { "Image001.png",                 "Image file",   203128,  -1,-1    }, // 6
                { "Copy of Image001.png",         "Image file",   203256,  -1,-1    }, // 7
                { "Copy of Image001 (Final2).png","Image file",   203512,  -1,-1    }, // 8
            };

            MyTreeNode::DisplayNode(&nodes[0], nodes);

            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Item width"))
    {
        HelpMarker(
            "Showcase using PushItemWidth() and how it is preserved on a per-column basis.\n\n"
            "Note that on auto-resizing non-resizable fixed columns, querying the content width for e.g. right-alignment doesn't make sense.");
        if (RainGui::BeginTable("table_item_width", 3, RainGuiTableFlags_Borders))
        {
            RainGui::TableSetupColumn("small");
            RainGui::TableSetupColumn("half");
            RainGui::TableSetupColumn("right-align");
            RainGui::TableHeadersRow();

            for (int row = 0; row < 3; row++)
            {
                RainGui::TableNextRow();
                if (row == 0)
                {
                    // Setup ItemWidth once (instead of setting up every time, which is also possible but less efficient)
                    RainGui::TableSetColumnIndex(0);
                    RainGui::PushItemWidth(TEXT_BASE_WIDTH * 3.0f); // Small
                    RainGui::TableSetColumnIndex(1);
                    RainGui::PushItemWidth(-RainGui::GetContentRegionAvail().x * 0.5f);
                    RainGui::TableSetColumnIndex(2);
                    RainGui::PushItemWidth(-FLT_MIN); // Right-aligned
                }

                // Draw our contents
                static float dummy_f = 0.0f;
                RainGui::PushID(row);
                RainGui::TableSetColumnIndex(0);
                RainGui::SliderFloat("float0", &dummy_f, 0.0f, 1.0f);
                RainGui::TableSetColumnIndex(1);
                RainGui::SliderFloat("float1", &dummy_f, 0.0f, 1.0f);
                RainGui::TableSetColumnIndex(2);
                RainGui::SliderFloat("float2", &dummy_f, 0.0f, 1.0f);
                RainGui::PopID();
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    // Demonstrate using TableHeader() calls instead of TableHeadersRow()
    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Custom headers"))
    {
        const int COLUMNS_COUNT = 3;
        if (RainGui::BeginTable("table_custom_headers", COLUMNS_COUNT, RainGuiTableFlags_Borders | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable))
        {
            RainGui::TableSetupColumn("Apricot");
            RainGui::TableSetupColumn("Banana");
            RainGui::TableSetupColumn("Cherry");

            // Dummy entire-column selection storage
            // FIXME: It would be nice to actually demonstrate full-featured selection using those checkbox.
            static bool column_selected[3] = {};

            // Instead of calling TableHeadersRow() we'll submit custom headers ourselves
            RainGui::TableNextRow(RainGuiTableRowFlags_Headers);
            for (int column = 0; column < COLUMNS_COUNT; column++)
            {
                RainGui::TableSetColumnIndex(column);
                const char* column_name = RainGui::TableGetColumnName(column); // Retrieve name passed to TableSetupColumn()
                RainGui::PushID(column);
                RainGui::PushStyleVar(RainGuiStyleVar_FramePadding, ImVec2(0, 0));
                RainGui::Checkbox("##checkall", &column_selected[column]);
                RainGui::PopStyleVar();
                RainGui::SameLine(0.0f, RainGui::GetStyle().ItemInnerSpacing.x);
                RainGui::TableHeader(column_name);
                RainGui::PopID();
            }

            for (int row = 0; row < 5; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    char buf[32];
                    sprintf(buf, "Cell %d,%d", column, row);
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Selectable(buf, column_selected[column]);
                }
            }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    // Demonstrate creating custom context menus inside columns, while playing it nice with context menus provided by TableHeadersRow()/TableHeader()
    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Context menus"))
    {
        HelpMarker("By default, right-clicking over a TableHeadersRow()/TableHeader() line will open the default context-menu.\nUsing RainGuiTableFlags_ContextMenuInBody we also allow right-clicking over columns body.");
        static RainGuiTableFlags flags1 = RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable | RainGuiTableFlags_Borders | RainGuiTableFlags_ContextMenuInBody;

        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_ContextMenuInBody", &flags1, RainGuiTableFlags_ContextMenuInBody);
        PopStyleCompact();

        // Context Menus: first example
        // [1.1] Right-click on the TableHeadersRow() line to open the default table context menu.
        // [1.2] Right-click in columns also open the default table context menu (if RainGuiTableFlags_ContextMenuInBody is set)
        const int COLUMNS_COUNT = 3;
        if (RainGui::BeginTable("table_context_menu", COLUMNS_COUNT, flags1))
        {
            RainGui::TableSetupColumn("One");
            RainGui::TableSetupColumn("Two");
            RainGui::TableSetupColumn("Three");

            // [1.1]] Right-click on the TableHeadersRow() line to open the default table context menu.
            RainGui::TableHeadersRow();

            // Submit dummy contents
            for (int row = 0; row < 4; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < COLUMNS_COUNT; column++)
                {
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("Cell %d,%d", column, row);
                }
            }
            RainGui::EndTable();
        }

        // Context Menus: second example
        // [2.1] Right-click on the TableHeadersRow() line to open the default table context menu.
        // [2.2] Right-click on the ".." to open a custom popup
        // [2.3] Right-click in columns to open another custom popup
        HelpMarker("Demonstrate mixing table context menu (over header), item context button (over button) and custom per-colum context menu (over column body).");
        RainGuiTableFlags flags2 = RainGuiTableFlags_Resizable | RainGuiTableFlags_SizingFixedFit | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable | RainGuiTableFlags_Borders;
        if (RainGui::BeginTable("table_context_menu_2", COLUMNS_COUNT, flags2))
        {
            RainGui::TableSetupColumn("One");
            RainGui::TableSetupColumn("Two");
            RainGui::TableSetupColumn("Three");

            // [2.1] Right-click on the TableHeadersRow() line to open the default table context menu.
            RainGui::TableHeadersRow();
            for (int row = 0; row < 4; row++)
            {
                RainGui::TableNextRow();
                for (int column = 0; column < COLUMNS_COUNT; column++)
                {
                    // Submit dummy contents
                    RainGui::TableSetColumnIndex(column);
                    RainGui::Text("Cell %d,%d", column, row);
                    RainGui::SameLine();

                    // [2.2] Right-click on the ".." to open a custom popup
                    RainGui::PushID(row * COLUMNS_COUNT + column);
                    RainGui::SmallButton("..");
                    if (RainGui::BeginPopupContextItem())
                    {
                        RainGui::Text("This is the popup for Button(\"..\") in Cell %d,%d", column, row);
                        if (RainGui::Button("Close"))
                            RainGui::CloseCurrentPopup();
                        RainGui::EndPopup();
                    }
                    RainGui::PopID();
                }
            }

            // [2.3] Right-click anywhere in columns to open another custom popup
            // (instead of testing for !IsAnyItemHovered() we could also call OpenPopup() with RainGuiPopupFlags_NoOpenOverExistingPopup
            // to manage popup priority as the popups triggers, here "are we hovering a column" are overlapping)
            int hovered_column = -1;
            for (int column = 0; column < COLUMNS_COUNT + 1; column++)
            {
                RainGui::PushID(column);
                if (RainGui::TableGetColumnFlags(column) & RainGuiTableColumnFlags_IsHovered)
                    hovered_column = column;
                if (hovered_column == column && !RainGui::IsAnyItemHovered() && RainGui::IsMouseReleased(1))
                    RainGui::OpenPopup("MyPopup");
                if (RainGui::BeginPopup("MyPopup"))
                {
                    if (column == COLUMNS_COUNT)
                        RainGui::Text("This is a custom popup for unused space after the last column.");
                    else
                        RainGui::Text("This is a custom popup for Column %d", column);
                    if (RainGui::Button("Close"))
                        RainGui::CloseCurrentPopup();
                    RainGui::EndPopup();
                }
                RainGui::PopID();
            }

            RainGui::EndTable();
            RainGui::Text("Hovered column: %d", hovered_column);
        }
        RainGui::TreePop();
    }

    // Demonstrate creating multiple tables with the same ID
    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Synced instances"))
    {
        HelpMarker("Multiple tables with the same identifier will share their settings, width, visibility, order etc.");
        for (int n = 0; n < 3; n++)
        {
            char buf[32];
            sprintf(buf, "Synced Table %d", n);
            bool open = RainGui::CollapsingHeader(buf, RainGuiTreeNodeFlags_DefaultOpen);
            if (open && RainGui::BeginTable("Table", 3, RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable | RainGuiTableFlags_Borders | RainGuiTableFlags_SizingFixedFit | RainGuiTableFlags_NoSavedSettings))
            {
                RainGui::TableSetupColumn("One");
                RainGui::TableSetupColumn("Two");
                RainGui::TableSetupColumn("Three");
                RainGui::TableHeadersRow();
                for (int cell = 0; cell < 9; cell++)
                {
                    RainGui::TableNextColumn();
                    RainGui::Text("this cell %d", cell);
                }
                RainGui::EndTable();
            }
        }
        RainGui::TreePop();
    }

    // Demonstrate using Sorting facilities
    // This is a simplified version of the "Advanced" example, where we mostly focus on the code necessary to handle sorting.
    // Note that the "Advanced" example also showcase manually triggering a sort (e.g. if item quantities have been modified)
    static const char* template_items_names[] =
    {
        "Banana", "Apple", "Cherry", "Watermelon", "Grapefruit", "Strawberry", "Mango",
        "Kiwi", "Orange", "Pineapple", "Blueberry", "Plum", "Coconut", "Pear", "Apricot"
    };
    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Sorting"))
    {
        // Create item list
        static ImVector<MyItem> items;
        if (items.Size == 0)
        {
            items.resize(50, MyItem());
            for (int n = 0; n < items.Size; n++)
            {
                const int template_n = n % IM_ARRAYSIZE(template_items_names);
                MyItem& item = items[n];
                item.ID = n;
                item.Name = template_items_names[template_n];
                item.Quantity = (n * n - n) % 20; // Assign default quantities
            }
        }

        // Options
        static RainGuiTableFlags flags =
            RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable | RainGuiTableFlags_Sortable | RainGuiTableFlags_SortMulti
            | RainGuiTableFlags_RowBg | RainGuiTableFlags_BordersOuter | RainGuiTableFlags_BordersV | RainGuiTableFlags_NoBordersInBody
            | RainGuiTableFlags_ScrollY;
        PushStyleCompact();
        RainGui::CheckboxFlags("RainGuiTableFlags_SortMulti", &flags, RainGuiTableFlags_SortMulti);
        RainGui::SameLine(); HelpMarker("When sorting is enabled: hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).");
        RainGui::CheckboxFlags("RainGuiTableFlags_SortTristate", &flags, RainGuiTableFlags_SortTristate);
        RainGui::SameLine(); HelpMarker("When sorting is enabled: allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).");
        PopStyleCompact();

        if (RainGui::BeginTable("table_sorting", 4, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 15), 0.0f))
        {
            // Declare columns
            // We use the "user_id" parameter of TableSetupColumn() to specify a user id that will be stored in the sort specifications.
            // This is so our sort function can identify a column given our own identifier. We could also identify them based on their index!
            // Demonstrate using a mixture of flags among available sort-related flags:
            // - RainGuiTableColumnFlags_DefaultSort
            // - RainGuiTableColumnFlags_NoSort / RainGuiTableColumnFlags_NoSortAscending / RainGuiTableColumnFlags_NoSortDescending
            // - RainGuiTableColumnFlags_PreferSortAscending / RainGuiTableColumnFlags_PreferSortDescending
            RainGui::TableSetupColumn("ID",       RainGuiTableColumnFlags_DefaultSort          | RainGuiTableColumnFlags_WidthFixed,   0.0f, MyItemColumnID_ID);
            RainGui::TableSetupColumn("Name",                                                  RainGuiTableColumnFlags_WidthFixed,   0.0f, MyItemColumnID_Name);
            RainGui::TableSetupColumn("Action",   RainGuiTableColumnFlags_NoSort               | RainGuiTableColumnFlags_WidthFixed,   0.0f, MyItemColumnID_Action);
            RainGui::TableSetupColumn("Quantity", RainGuiTableColumnFlags_PreferSortDescending | RainGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Quantity);
            RainGui::TableSetupScrollFreeze(0, 1); // Make row always visible
            RainGui::TableHeadersRow();

            // Sort our data if sort specs have been changed!
            if (RainGuiTableSortSpecs* sorts_specs = RainGui::TableGetSortSpecs())
                if (sorts_specs->SpecsDirty)
                {
                    MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                    if (items.Size > 1)
                        qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
                    MyItem::s_current_sort_specs = NULL;
                    sorts_specs->SpecsDirty = false;
                }

            // Demonstrate using clipper for large vertical lists
            RainGuiListClipper clipper;
            clipper.Begin(items.Size);
            while (clipper.Step())
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    // Display a data item
                    MyItem* item = &items[row_n];
                    RainGui::PushID(item->ID);
                    RainGui::TableNextRow();
                    RainGui::TableNextColumn();
                    RainGui::Text("%04d", item->ID);
                    RainGui::TableNextColumn();
                    RainGui::TextUnformatted(item->Name);
                    RainGui::TableNextColumn();
                    RainGui::SmallButton("None");
                    RainGui::TableNextColumn();
                    RainGui::Text("%d", item->Quantity);
                    RainGui::PopID();
                }
            RainGui::EndTable();
        }
        RainGui::TreePop();
    }

    // In this example we'll expose most table flags and settings.
    // For specific flags and settings refer to the corresponding section for more detailed explanation.
    // This section is mostly useful to experiment with combining certain flags or settings with each others.
    //RainGui::SetNextItemOpen(true, RainGuiCond_Once); // [DEBUG]
    if (open_action != -1)
        RainGui::SetNextItemOpen(open_action != 0);
    if (RainGui::TreeNode("Advanced"))
    {
        static RainGuiTableFlags flags =
            RainGuiTableFlags_Resizable | RainGuiTableFlags_Reorderable | RainGuiTableFlags_Hideable
            | RainGuiTableFlags_Sortable | RainGuiTableFlags_SortMulti
            | RainGuiTableFlags_RowBg | RainGuiTableFlags_Borders | RainGuiTableFlags_NoBordersInBody
            | RainGuiTableFlags_ScrollX | RainGuiTableFlags_ScrollY
            | RainGuiTableFlags_SizingFixedFit;

        enum ContentsType { CT_Text, CT_Button, CT_SmallButton, CT_FillButton, CT_Selectable, CT_SelectableSpanRow };
        static int contents_type = CT_SelectableSpanRow;
        const char* contents_type_names[] = { "Text", "Button", "SmallButton", "FillButton", "Selectable", "Selectable (span row)" };
        static int freeze_cols = 1;
        static int freeze_rows = 1;
        static int items_count = IM_ARRAYSIZE(template_items_names) * 2;
        static ImVec2 outer_size_value = ImVec2(0.0f, TEXT_BASE_HEIGHT * 12);
        static float row_min_height = 0.0f; // Auto
        static float inner_width_with_scroll = 0.0f; // Auto-extend
        static bool outer_size_enabled = true;
        static bool show_headers = true;
        static bool show_wrapped_text = false;
        //static RainGuiTextFilter filter;
        //RainGui::SetNextItemOpen(true, RainGuiCond_Once); // FIXME-TABLE: Enabling this results in initial clipped first pass on table which tend to affects column sizing
        if (RainGui::TreeNode("Options"))
        {
            // Make the UI compact because there are so many fields
            PushStyleCompact();
            RainGui::PushItemWidth(TEXT_BASE_WIDTH * 28.0f);

            if (RainGui::TreeNodeEx("Features:", RainGuiTreeNodeFlags_DefaultOpen))
            {
                RainGui::CheckboxFlags("RainGuiTableFlags_Resizable", &flags, RainGuiTableFlags_Resizable);
                RainGui::CheckboxFlags("RainGuiTableFlags_Reorderable", &flags, RainGuiTableFlags_Reorderable);
                RainGui::CheckboxFlags("RainGuiTableFlags_Hideable", &flags, RainGuiTableFlags_Hideable);
                RainGui::CheckboxFlags("RainGuiTableFlags_Sortable", &flags, RainGuiTableFlags_Sortable);
                RainGui::CheckboxFlags("RainGuiTableFlags_NoSavedSettings", &flags, RainGuiTableFlags_NoSavedSettings);
                RainGui::CheckboxFlags("RainGuiTableFlags_ContextMenuInBody", &flags, RainGuiTableFlags_ContextMenuInBody);
                RainGui::TreePop();
            }

            if (RainGui::TreeNodeEx("Decorations:", RainGuiTreeNodeFlags_DefaultOpen))
            {
                RainGui::CheckboxFlags("RainGuiTableFlags_RowBg", &flags, RainGuiTableFlags_RowBg);
                RainGui::CheckboxFlags("RainGuiTableFlags_BordersV", &flags, RainGuiTableFlags_BordersV);
                RainGui::CheckboxFlags("RainGuiTableFlags_BordersOuterV", &flags, RainGuiTableFlags_BordersOuterV);
                RainGui::CheckboxFlags("RainGuiTableFlags_BordersInnerV", &flags, RainGuiTableFlags_BordersInnerV);
                RainGui::CheckboxFlags("RainGuiTableFlags_BordersH", &flags, RainGuiTableFlags_BordersH);
                RainGui::CheckboxFlags("RainGuiTableFlags_BordersOuterH", &flags, RainGuiTableFlags_BordersOuterH);
                RainGui::CheckboxFlags("RainGuiTableFlags_BordersInnerH", &flags, RainGuiTableFlags_BordersInnerH);
                RainGui::CheckboxFlags("RainGuiTableFlags_NoBordersInBody", &flags, RainGuiTableFlags_NoBordersInBody); RainGui::SameLine(); HelpMarker("Disable vertical borders in columns Body (borders will always appears in Headers");
                RainGui::CheckboxFlags("RainGuiTableFlags_NoBordersInBodyUntilResize", &flags, RainGuiTableFlags_NoBordersInBodyUntilResize); RainGui::SameLine(); HelpMarker("Disable vertical borders in columns Body until hovered for resize (borders will always appears in Headers)");
                RainGui::TreePop();
            }

            if (RainGui::TreeNodeEx("Sizing:", RainGuiTreeNodeFlags_DefaultOpen))
            {
                EditTableSizingFlags(&flags);
                RainGui::SameLine(); HelpMarker("In the Advanced demo we override the policy of each column so those table-wide settings have less effect that typical.");
                RainGui::CheckboxFlags("RainGuiTableFlags_NoHostExtendX", &flags, RainGuiTableFlags_NoHostExtendX);
                RainGui::SameLine(); HelpMarker("Make outer width auto-fit to columns, overriding outer_size.x value.\n\nOnly available when ScrollX/ScrollY are disabled and Stretch columns are not used.");
                RainGui::CheckboxFlags("RainGuiTableFlags_NoHostExtendY", &flags, RainGuiTableFlags_NoHostExtendY);
                RainGui::SameLine(); HelpMarker("Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit).\n\nOnly available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.");
                RainGui::CheckboxFlags("RainGuiTableFlags_NoKeepColumnsVisible", &flags, RainGuiTableFlags_NoKeepColumnsVisible);
                RainGui::SameLine(); HelpMarker("Only available if ScrollX is disabled.");
                RainGui::CheckboxFlags("RainGuiTableFlags_PreciseWidths", &flags, RainGuiTableFlags_PreciseWidths);
                RainGui::SameLine(); HelpMarker("Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.");
                RainGui::CheckboxFlags("RainGuiTableFlags_NoClip", &flags, RainGuiTableFlags_NoClip);
                RainGui::SameLine(); HelpMarker("Disable clipping rectangle for every individual columns (reduce draw command count, items will be able to overflow into other columns). Generally incompatible with ScrollFreeze options.");
                RainGui::TreePop();
            }

            if (RainGui::TreeNodeEx("Padding:", RainGuiTreeNodeFlags_DefaultOpen))
            {
                RainGui::CheckboxFlags("RainGuiTableFlags_PadOuterX", &flags, RainGuiTableFlags_PadOuterX);
                RainGui::CheckboxFlags("RainGuiTableFlags_NoPadOuterX", &flags, RainGuiTableFlags_NoPadOuterX);
                RainGui::CheckboxFlags("RainGuiTableFlags_NoPadInnerX", &flags, RainGuiTableFlags_NoPadInnerX);
                RainGui::TreePop();
            }

            if (RainGui::TreeNodeEx("Scrolling:", RainGuiTreeNodeFlags_DefaultOpen))
            {
                RainGui::CheckboxFlags("RainGuiTableFlags_ScrollX", &flags, RainGuiTableFlags_ScrollX);
                RainGui::SameLine();
                RainGui::SetNextItemWidth(RainGui::GetFrameHeight());
                RainGui::DragInt("freeze_cols", &freeze_cols, 0.2f, 0, 9, NULL, RainGuiSliderFlags_NoInput);
                RainGui::CheckboxFlags("RainGuiTableFlags_ScrollY", &flags, RainGuiTableFlags_ScrollY);
                RainGui::SameLine();
                RainGui::SetNextItemWidth(RainGui::GetFrameHeight());
                RainGui::DragInt("freeze_rows", &freeze_rows, 0.2f, 0, 9, NULL, RainGuiSliderFlags_NoInput);
                RainGui::TreePop();
            }

            if (RainGui::TreeNodeEx("Sorting:", RainGuiTreeNodeFlags_DefaultOpen))
            {
                RainGui::CheckboxFlags("RainGuiTableFlags_SortMulti", &flags, RainGuiTableFlags_SortMulti);
                RainGui::SameLine(); HelpMarker("When sorting is enabled: hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).");
                RainGui::CheckboxFlags("RainGuiTableFlags_SortTristate", &flags, RainGuiTableFlags_SortTristate);
                RainGui::SameLine(); HelpMarker("When sorting is enabled: allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).");
                RainGui::TreePop();
            }

            if (RainGui::TreeNodeEx("Other:", RainGuiTreeNodeFlags_DefaultOpen))
            {
                RainGui::Checkbox("show_headers", &show_headers);
                RainGui::Checkbox("show_wrapped_text", &show_wrapped_text);

                RainGui::DragFloat2("##OuterSize", &outer_size_value.x);
                RainGui::SameLine(0.0f, RainGui::GetStyle().ItemInnerSpacing.x);
                RainGui::Checkbox("outer_size", &outer_size_enabled);
                RainGui::SameLine();
                HelpMarker("If scrolling is disabled (ScrollX and ScrollY not set):\n"
                    "- The table is output directly in the parent window.\n"
                    "- OuterSize.x < 0.0f will right-align the table.\n"
                    "- OuterSize.x = 0.0f will narrow fit the table unless there are any Stretch column.\n"
                    "- OuterSize.y then becomes the minimum size for the table, which will extend vertically if there are more rows (unless NoHostExtendY is set).");

                // From a user point of view we will tend to use 'inner_width' differently depending on whether our table is embedding scrolling.
                // To facilitate toying with this demo we will actually pass 0.0f to the BeginTable() when ScrollX is disabled.
                RainGui::DragFloat("inner_width (when ScrollX active)", &inner_width_with_scroll, 1.0f, 0.0f, FLT_MAX);

                RainGui::DragFloat("row_min_height", &row_min_height, 1.0f, 0.0f, FLT_MAX);
                RainGui::SameLine(); HelpMarker("Specify height of the Selectable item.");

                RainGui::DragInt("items_count", &items_count, 0.1f, 0, 9999);
                RainGui::Combo("items_type (first column)", &contents_type, contents_type_names, IM_ARRAYSIZE(contents_type_names));
                //filter.Draw("filter");
                RainGui::TreePop();
            }

            RainGui::PopItemWidth();
            PopStyleCompact();
            RainGui::Spacing();
            RainGui::TreePop();
        }

        // Update item list if we changed the number of items
        static ImVector<MyItem> items;
        static ImVector<int> selection;
        static bool items_need_sort = false;
        if (items.Size != items_count)
        {
            items.resize(items_count, MyItem());
            for (int n = 0; n < items_count; n++)
            {
                const int template_n = n % IM_ARRAYSIZE(template_items_names);
                MyItem& item = items[n];
                item.ID = n;
                item.Name = template_items_names[template_n];
                item.Quantity = (template_n == 3) ? 10 : (template_n == 4) ? 20 : 0; // Assign default quantities
            }
        }

        const ImDrawList* parent_draw_list = RainGui::GetWindowDrawList();
        const int parent_draw_list_draw_cmd_count = parent_draw_list->CmdBuffer.Size;
        ImVec2 table_scroll_cur, table_scroll_max; // For debug display
        const ImDrawList* table_draw_list = NULL;  // "

        // Submit table
        const float inner_width_to_use = (flags & RainGuiTableFlags_ScrollX) ? inner_width_with_scroll : 0.0f;
        if (RainGui::BeginTable("table_advanced", 6, flags, outer_size_enabled ? outer_size_value : ImVec2(0, 0), inner_width_to_use))
        {
            // Declare columns
            // We use the "user_id" parameter of TableSetupColumn() to specify a user id that will be stored in the sort specifications.
            // This is so our sort function can identify a column given our own identifier. We could also identify them based on their index!
            RainGui::TableSetupColumn("ID",           RainGuiTableColumnFlags_DefaultSort | RainGuiTableColumnFlags_WidthFixed | RainGuiTableColumnFlags_NoHide, 0.0f, MyItemColumnID_ID);
            RainGui::TableSetupColumn("Name",         RainGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
            RainGui::TableSetupColumn("Action",       RainGuiTableColumnFlags_NoSort | RainGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
            RainGui::TableSetupColumn("Quantity",     RainGuiTableColumnFlags_PreferSortDescending, 0.0f, MyItemColumnID_Quantity);
            RainGui::TableSetupColumn("Description",  (flags & RainGuiTableFlags_NoHostExtendX) ? 0 : RainGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Description);
            RainGui::TableSetupColumn("Hidden",       RainGuiTableColumnFlags_DefaultHide | RainGuiTableColumnFlags_NoSort);
            RainGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);

            // Sort our data if sort specs have been changed!
            RainGuiTableSortSpecs* sorts_specs = RainGui::TableGetSortSpecs();
            if (sorts_specs && sorts_specs->SpecsDirty)
                items_need_sort = true;
            if (sorts_specs && items_need_sort && items.Size > 1)
            {
                MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
                MyItem::s_current_sort_specs = NULL;
                sorts_specs->SpecsDirty = false;
            }
            items_need_sort = false;

            // Take note of whether we are currently sorting based on the Quantity field,
            // we will use this to trigger sorting when we know the data of this column has been modified.
            const bool sorts_specs_using_quantity = (RainGui::TableGetColumnFlags(3) & RainGuiTableColumnFlags_IsSorted) != 0;

            // Show headers
            if (show_headers)
                RainGui::TableHeadersRow();

            // Show data
            // FIXME-TABLE FIXME-NAV: How we can get decent up/down even though we have the buttons here?
            RainGui::PushButtonRepeat(true);
#if 1
            // Demonstrate using clipper for large vertical lists
            RainGuiListClipper clipper;
            clipper.Begin(items.Size);
            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
#else
            // Without clipper
            {
                for (int row_n = 0; row_n < items.Size; row_n++)
#endif
                {
                    MyItem* item = &items[row_n];
                    //if (!filter.PassFilter(item->Name))
                    //    continue;

                    const bool item_is_selected = selection.contains(item->ID);
                    RainGui::PushID(item->ID);
                    RainGui::TableNextRow(RainGuiTableRowFlags_None, row_min_height);

                    // For the demo purpose we can select among different type of items submitted in the first column
                    RainGui::TableSetColumnIndex(0);
                    char label[32];
                    sprintf(label, "%04d", item->ID);
                    if (contents_type == CT_Text)
                        RainGui::TextUnformatted(label);
                    else if (contents_type == CT_Button)
                        RainGui::Button(label);
                    else if (contents_type == CT_SmallButton)
                        RainGui::SmallButton(label);
                    else if (contents_type == CT_FillButton)
                        RainGui::Button(label, ImVec2(-FLT_MIN, 0.0f));
                    else if (contents_type == CT_Selectable || contents_type == CT_SelectableSpanRow)
                    {
                        RainGuiSelectableFlags selectable_flags = (contents_type == CT_SelectableSpanRow) ? RainGuiSelectableFlags_SpanAllColumns | RainGuiSelectableFlags_AllowItemOverlap : RainGuiSelectableFlags_None;
                        if (RainGui::Selectable(label, item_is_selected, selectable_flags, ImVec2(0, row_min_height)))
                        {
                            if (RainGui::GetIO().KeyCtrl)
                            {
                                if (item_is_selected)
                                    selection.find_erase_unsorted(item->ID);
                                else
                                    selection.push_back(item->ID);
                            }
                            else
                            {
                                selection.clear();
                                selection.push_back(item->ID);
                            }
                        }
                    }

                    if (RainGui::TableSetColumnIndex(1))
                        RainGui::TextUnformatted(item->Name);

                    // Here we demonstrate marking our data set as needing to be sorted again if we modified a quantity,
                    // and we are currently sorting on the column showing the Quantity.
                    // To avoid triggering a sort while holding the button, we only trigger it when the button has been released.
                    // You will probably need a more advanced system in your code if you want to automatically sort when a specific entry changes.
                    if (RainGui::TableSetColumnIndex(2))
                    {
                        if (RainGui::SmallButton("Chop")) { item->Quantity += 1; }
                        if (sorts_specs_using_quantity && RainGui::IsItemDeactivated()) { items_need_sort = true; }
                        RainGui::SameLine();
                        if (RainGui::SmallButton("Eat")) { item->Quantity -= 1; }
                        if (sorts_specs_using_quantity && RainGui::IsItemDeactivated()) { items_need_sort = true; }
                    }

                    if (RainGui::TableSetColumnIndex(3))
                        RainGui::Text("%d", item->Quantity);

                    RainGui::TableSetColumnIndex(4);
                    if (show_wrapped_text)
                        RainGui::TextWrapped("Lorem ipsum dolor sit amet");
                    else
                        RainGui::Text("Lorem ipsum dolor sit amet");

                    if (RainGui::TableSetColumnIndex(5))
                        RainGui::Text("1234");

                    RainGui::PopID();
                }
            }
            RainGui::PopButtonRepeat();

            // Store some info to display debug details below
            table_scroll_cur = ImVec2(RainGui::GetScrollX(), RainGui::GetScrollY());
            table_scroll_max = ImVec2(RainGui::GetScrollMaxX(), RainGui::GetScrollMaxY());
            table_draw_list = RainGui::GetWindowDrawList();
            RainGui::EndTable();
        }
        static bool show_debug_details = false;
        RainGui::Checkbox("Debug details", &show_debug_details);
        if (show_debug_details && table_draw_list)
        {
            RainGui::SameLine(0.0f, 0.0f);
            const int table_draw_list_draw_cmd_count = table_draw_list->CmdBuffer.Size;
            if (table_draw_list == parent_draw_list)
                RainGui::Text(": DrawCmd: +%d (in same window)",
                    table_draw_list_draw_cmd_count - parent_draw_list_draw_cmd_count);
            else
                RainGui::Text(": DrawCmd: +%d (in child window), Scroll: (%.f/%.f) (%.f/%.f)",
                    table_draw_list_draw_cmd_count - 1, table_scroll_cur.x, table_scroll_max.x, table_scroll_cur.y, table_scroll_max.y);
        }
        RainGui::TreePop();
    }

    RainGui::PopID();

    ShowDemoWindowColumns();

    if (disable_indent)
        RainGui::PopStyleVar();
}

// Demonstrate old/legacy Columns API!
// [2020: Columns are under-featured and not maintained. Prefer using the more flexible and powerful BeginTable() API!]
static void ShowDemoWindowColumns()
{
    bool open = RainGui::TreeNode("Legacy Columns API");
    RainGui::SameLine();
    HelpMarker("Columns() is an old API! Prefer using the more flexible and powerful BeginTable() API!");
    if (!open)
        return;

    // Basic columns
    if (RainGui::TreeNode("Basic"))
    {
        RainGui::Text("Without border:");
        RainGui::Columns(3, "mycolumns3", false);  // 3-ways, no border
        RainGui::Separator();
        for (int n = 0; n < 14; n++)
        {
            char label[32];
            sprintf(label, "Item %d", n);
            if (RainGui::Selectable(label)) {}
            //if (RainGui::Button(label, ImVec2(-FLT_MIN,0.0f))) {}
            RainGui::NextColumn();
        }
        RainGui::Columns(1);
        RainGui::Separator();

        RainGui::Text("With border:");
        RainGui::Columns(4, "mycolumns"); // 4-ways, with border
        RainGui::Separator();
        RainGui::Text("ID"); RainGui::NextColumn();
        RainGui::Text("Name"); RainGui::NextColumn();
        RainGui::Text("Path"); RainGui::NextColumn();
        RainGui::Text("Hovered"); RainGui::NextColumn();
        RainGui::Separator();
        const char* names[3] = { "One", "Two", "Three" };
        const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
        static int selected = -1;
        for (int i = 0; i < 3; i++)
        {
            char label[32];
            sprintf(label, "%04d", i);
            if (RainGui::Selectable(label, selected == i, RainGuiSelectableFlags_SpanAllColumns))
                selected = i;
            bool hovered = RainGui::IsItemHovered();
            RainGui::NextColumn();
            RainGui::Text(names[i]); RainGui::NextColumn();
            RainGui::Text(paths[i]); RainGui::NextColumn();
            RainGui::Text("%d", hovered); RainGui::NextColumn();
        }
        RainGui::Columns(1);
        RainGui::Separator();
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Borders"))
    {
        // NB: Future columns API should allow automatic horizontal borders.
        static bool h_borders = true;
        static bool v_borders = true;
        static int columns_count = 4;
        const int lines_count = 3;
        RainGui::SetNextItemWidth(RainGui::GetFontSize() * 8);
        RainGui::DragInt("##columns_count", &columns_count, 0.1f, 2, 10, "%d columns");
        if (columns_count < 2)
            columns_count = 2;
        RainGui::SameLine();
        RainGui::Checkbox("horizontal", &h_borders);
        RainGui::SameLine();
        RainGui::Checkbox("vertical", &v_borders);
        RainGui::Columns(columns_count, NULL, v_borders);
        for (int i = 0; i < columns_count * lines_count; i++)
        {
            if (h_borders && RainGui::GetColumnIndex() == 0)
                RainGui::Separator();
            RainGui::Text("%c%c%c", 'a' + i, 'a' + i, 'a' + i);
            RainGui::Text("Width %.2f", RainGui::GetColumnWidth());
            RainGui::Text("Avail %.2f", RainGui::GetContentRegionAvail().x);
            RainGui::Text("Offset %.2f", RainGui::GetColumnOffset());
            RainGui::Text("Long text that is likely to clip");
            RainGui::Button("Button", ImVec2(-FLT_MIN, 0.0f));
            RainGui::NextColumn();
        }
        RainGui::Columns(1);
        if (h_borders)
            RainGui::Separator();
        RainGui::TreePop();
    }

    // Create multiple items in a same cell before switching to next column
    if (RainGui::TreeNode("Mixed items"))
    {
        RainGui::Columns(3, "mixed");
        RainGui::Separator();

        RainGui::Text("Hello");
        RainGui::Button("Banana");
        RainGui::NextColumn();

        RainGui::Text("RainGui");
        RainGui::Button("Apple");
        static float foo = 1.0f;
        RainGui::InputFloat("red", &foo, 0.05f, 0, "%.3f");
        RainGui::Text("An extra line here.");
        RainGui::NextColumn();

        RainGui::Text("Sailor");
        RainGui::Button("Corniflower");
        static float bar = 1.0f;
        RainGui::InputFloat("blue", &bar, 0.05f, 0, "%.3f");
        RainGui::NextColumn();

        if (RainGui::CollapsingHeader("Category A")) { RainGui::Text("Blah blah blah"); } RainGui::NextColumn();
        if (RainGui::CollapsingHeader("Category B")) { RainGui::Text("Blah blah blah"); } RainGui::NextColumn();
        if (RainGui::CollapsingHeader("Category C")) { RainGui::Text("Blah blah blah"); } RainGui::NextColumn();
        RainGui::Columns(1);
        RainGui::Separator();
        RainGui::TreePop();
    }

    // Word wrapping
    if (RainGui::TreeNode("Word-wrapping"))
    {
        RainGui::Columns(2, "word-wrapping");
        RainGui::Separator();
        RainGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
        RainGui::TextWrapped("Hello Left");
        RainGui::NextColumn();
        RainGui::TextWrapped("The quick brown fox jumps over the lazy dog.");
        RainGui::TextWrapped("Hello Right");
        RainGui::Columns(1);
        RainGui::Separator();
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Horizontal Scrolling"))
    {
        RainGui::SetNextWindowContentSize(ImVec2(1500.0f, 0.0f));
        ImVec2 child_size = ImVec2(0, RainGui::GetFontSize() * 20.0f);
        RainGui::BeginChild("##ScrollingRegion", child_size, false, RainGuiWindowFlags_HorizontalScrollbar);
        RainGui::Columns(10);

        // Also demonstrate using clipper for large vertical lists
        int ITEMS_COUNT = 2000;
        RainGuiListClipper clipper;
        clipper.Begin(ITEMS_COUNT);
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                for (int j = 0; j < 10; j++)
                {
                    RainGui::Text("Line %d Column %d...", i, j);
                    RainGui::NextColumn();
                }
        }
        RainGui::Columns(1);
        RainGui::EndChild();
        RainGui::TreePop();
    }

    if (RainGui::TreeNode("Tree"))
    {
        RainGui::Columns(2, "tree", true);
        for (int x = 0; x < 3; x++)
        {
            bool open1 = RainGui::TreeNode((void*)(intptr_t)x, "Node%d", x);
            RainGui::NextColumn();
            RainGui::Text("Node contents");
            RainGui::NextColumn();
            if (open1)
            {
                for (int y = 0; y < 3; y++)
                {
                    bool open2 = RainGui::TreeNode((void*)(intptr_t)y, "Node%d.%d", x, y);
                    RainGui::NextColumn();
                    RainGui::Text("Node contents");
                    if (open2)
                    {
                        RainGui::Text("Even more contents");
                        if (RainGui::TreeNode("Tree in column"))
                        {
                            RainGui::Text("The quick brown fox jumps over the lazy dog");
                            RainGui::TreePop();
                        }
                    }
                    RainGui::NextColumn();
                    if (open2)
                        RainGui::TreePop();
                }
                RainGui::TreePop();
            }
        }
        RainGui::Columns(1);
        RainGui::TreePop();
    }

    RainGui::TreePop();
}

static void ShowDemoWindowMisc()
{
    if (RainGui::CollapsingHeader("Filtering"))
    {
        // Helper class to easy setup a text filter.
        // You may want to implement a more feature-full filtering scheme in your own application.
        static RainGuiTextFilter filter;
        RainGui::Text("Filter usage:\n"
                    "  \"\"         display all lines\n"
                    "  \"xxx\"      display lines containing \"xxx\"\n"
                    "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                    "  \"-xxx\"     hide lines containing \"xxx\"");
        filter.Draw();
        const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
        for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
            if (filter.PassFilter(lines[i]))
                RainGui::BulletText("%s", lines[i]);
    }

    if (RainGui::CollapsingHeader("Inputs, Navigation & Focus"))
    {
        RainGuiIO& io = RainGui::GetIO();

        // Display RainGuiIO output flags
        RainGui::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
        RainGui::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
        RainGui::Text("WantTextInput: %d", io.WantTextInput);
        RainGui::Text("WantSetMousePos: %d", io.WantSetMousePos);
        RainGui::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

        // Display Mouse state
        if (RainGui::TreeNode("Mouse State"))
        {
            if (RainGui::IsMousePosValid())
                RainGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                RainGui::Text("Mouse pos: <INVALID>");
            RainGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
            RainGui::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (RainGui::IsMouseDown(i))         { RainGui::SameLine(); RainGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            RainGui::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (RainGui::IsMouseClicked(i))      { RainGui::SameLine(); RainGui::Text("b%d", i); }
            RainGui::Text("Mouse dblclick:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (RainGui::IsMouseDoubleClicked(i)){ RainGui::SameLine(); RainGui::Text("b%d", i); }
            RainGui::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (RainGui::IsMouseReleased(i))     { RainGui::SameLine(); RainGui::Text("b%d", i); }
            RainGui::Text("Mouse wheel: %.1f", io.MouseWheel);
            RainGui::Text("Pen Pressure: %.1f", io.PenPressure); // Note: currently unused
            RainGui::TreePop();
        }

        // Display Keyboard/Mouse state
        if (RainGui::TreeNode("Keyboard & Navigation State"))
        {
            RainGui::Text("Keys down:");          for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (RainGui::IsKeyDown(i))        { RainGui::SameLine(); RainGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
            RainGui::Text("Keys pressed:");       for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (RainGui::IsKeyPressed(i))     { RainGui::SameLine(); RainGui::Text("%d (0x%X)", i, i); }
            RainGui::Text("Keys release:");       for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (RainGui::IsKeyReleased(i))    { RainGui::SameLine(); RainGui::Text("%d (0x%X)", i, i); }
            RainGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
            RainGui::Text("Chars queue:");        for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; RainGui::SameLine();  RainGui::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

            RainGui::Text("NavInputs down:");     for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputs[i] > 0.0f)              { RainGui::SameLine(); RainGui::Text("[%d] %.2f (%.02f secs)", i, io.NavInputs[i], io.NavInputsDownDuration[i]); }
            RainGui::Text("NavInputs pressed:");  for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] == 0.0f) { RainGui::SameLine(); RainGui::Text("[%d]", i); }

            RainGui::Button("Hovering me sets the\nkeyboard capture flag");
            if (RainGui::IsItemHovered())
                RainGui::CaptureKeyboardFromApp(true);
            RainGui::SameLine();
            RainGui::Button("Holding me clears the\nthe keyboard capture flag");
            if (RainGui::IsItemActive())
                RainGui::CaptureKeyboardFromApp(false);
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Tabbing"))
        {
            RainGui::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "hello";
            RainGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            RainGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            RainGui::InputText("3", buf, IM_ARRAYSIZE(buf));
            RainGui::PushAllowKeyboardFocus(false);
            RainGui::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            RainGui::SameLine(); HelpMarker("Item won't be cycled through when using TAB or Shift+Tab.");
            RainGui::PopAllowKeyboardFocus();
            RainGui::InputText("5", buf, IM_ARRAYSIZE(buf));
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Focus from code"))
        {
            bool focus_1 = RainGui::Button("Focus on 1"); RainGui::SameLine();
            bool focus_2 = RainGui::Button("Focus on 2"); RainGui::SameLine();
            bool focus_3 = RainGui::Button("Focus on 3");
            int has_focus = 0;
            static char buf[128] = "click on a button to set focus";

            if (focus_1) RainGui::SetKeyboardFocusHere();
            RainGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            if (RainGui::IsItemActive()) has_focus = 1;

            if (focus_2) RainGui::SetKeyboardFocusHere();
            RainGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            if (RainGui::IsItemActive()) has_focus = 2;

            RainGui::PushAllowKeyboardFocus(false);
            if (focus_3) RainGui::SetKeyboardFocusHere();
            RainGui::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
            if (RainGui::IsItemActive()) has_focus = 3;
            RainGui::SameLine(); HelpMarker("Item won't be cycled through when using TAB or Shift+Tab.");
            RainGui::PopAllowKeyboardFocus();

            if (has_focus)
                RainGui::Text("Item with focus: %d", has_focus);
            else
                RainGui::Text("Item with focus: <none>");

            // Use >= 0 parameter to SetKeyboardFocusHere() to focus an upcoming item
            static float f3[3] = { 0.0f, 0.0f, 0.0f };
            int focus_ahead = -1;
            if (RainGui::Button("Focus on X")) { focus_ahead = 0; } RainGui::SameLine();
            if (RainGui::Button("Focus on Y")) { focus_ahead = 1; } RainGui::SameLine();
            if (RainGui::Button("Focus on Z")) { focus_ahead = 2; }
            if (focus_ahead != -1) RainGui::SetKeyboardFocusHere(focus_ahead);
            RainGui::SliderFloat3("Float3", &f3[0], 0.0f, 1.0f);

            RainGui::TextWrapped("NB: Cursor & selection are preserved when refocusing last used item in code.");
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Dragging"))
        {
            RainGui::TextWrapped("You can use RainGui::GetMouseDragDelta(0) to query for the dragged amount on any widget.");
            for (int button = 0; button < 3; button++)
            {
                RainGui::Text("IsMouseDragging(%d):", button);
                RainGui::Text("  w/ default threshold: %d,", RainGui::IsMouseDragging(button));
                RainGui::Text("  w/ zero threshold: %d,", RainGui::IsMouseDragging(button, 0.0f));
                RainGui::Text("  w/ large threshold: %d,", RainGui::IsMouseDragging(button, 20.0f));
            }

            RainGui::Button("Drag Me");
            if (RainGui::IsItemActive())
                RainGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, RainGui::GetColorU32(RainGuiCol_Button), 4.0f); // Draw a line between the button and the mouse cursor

            // Drag operations gets "unlocked" when the mouse has moved past a certain threshold
            // (the default threshold is stored in io.MouseDragThreshold). You can request a lower or higher
            // threshold using the second parameter of IsMouseDragging() and GetMouseDragDelta().
            ImVec2 value_raw = RainGui::GetMouseDragDelta(0, 0.0f);
            ImVec2 value_with_lock_threshold = RainGui::GetMouseDragDelta(0);
            ImVec2 mouse_delta = io.MouseDelta;
            RainGui::Text("GetMouseDragDelta(0):");
            RainGui::Text("  w/ default threshold: (%.1f, %.1f)", value_with_lock_threshold.x, value_with_lock_threshold.y);
            RainGui::Text("  w/ zero threshold: (%.1f, %.1f)", value_raw.x, value_raw.y);
            RainGui::Text("io.MouseDelta: (%.1f, %.1f)", mouse_delta.x, mouse_delta.y);
            RainGui::TreePop();
        }

        if (RainGui::TreeNode("Mouse cursors"))
        {
            const char* mouse_cursors_names[] = { "Arrow", "TextInput", "ResizeAll", "ResizeNS", "ResizeEW", "ResizeNESW", "ResizeNWSE", "Hand", "NotAllowed" };
            IM_ASSERT(IM_ARRAYSIZE(mouse_cursors_names) == RainGuiMouseCursor_COUNT);

            RainGuiMouseCursor current = RainGui::GetMouseCursor();
            RainGui::Text("Current mouse cursor = %d: %s", current, mouse_cursors_names[current]);
            RainGui::Text("Hover to see mouse cursors:");
            RainGui::SameLine(); HelpMarker(
                "Your application can render a different mouse cursor based on what RainGui::GetMouseCursor() returns. "
                "If software cursor rendering (io.MouseDrawCursor) is set RainGui will draw the right cursor for you, "
                "otherwise your backend needs to handle it.");
            for (int i = 0; i < RainGuiMouseCursor_COUNT; i++)
            {
                char label[32];
                sprintf(label, "Mouse cursor %d: %s", i, mouse_cursors_names[i]);
                RainGui::Bullet(); RainGui::Selectable(label, false);
                if (RainGui::IsItemHovered())
                    RainGui::SetMouseCursor(i);
            }
            RainGui::TreePop();
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] About Window / ShowAboutWindow()
// Access from Dear RainGui Demo -> Tools -> About
//-----------------------------------------------------------------------------

void RainGui::ShowAboutWindow(bool* p_open)
{
    if (!RainGui::Begin("About Dear RainGui", p_open, RainGuiWindowFlags_AlwaysAutoResize))
    {
        RainGui::End();
        return;
    }
    RainGui::Text("Dear RainGui %s", RainGui::GetVersion());
    RainGui::Separator();
    RainGui::Text("By Omar Cornut and all Dear RainGui contributors.");
    RainGui::Text("Dear RainGui is licensed under the MIT License, see LICENSE for more information.");

    static bool show_config_info = false;
    RainGui::Checkbox("Config/Build Information", &show_config_info);
    if (show_config_info)
    {
        RainGuiIO& io = RainGui::GetIO();
        RainGuiStyle& style = RainGui::GetStyle();

        bool copy_to_clipboard = RainGui::Button("Copy to clipboard");
        ImVec2 child_size = ImVec2(0, RainGui::GetTextLineHeightWithSpacing() * 18);
        RainGui::BeginChildFrame(RainGui::GetID("cfg_infos"), child_size, RainGuiWindowFlags_NoMove);
        if (copy_to_clipboard)
        {
            RainGui::LogToClipboard();
            RainGui::LogText("```\n"); // Back quotes will make text appears without formatting when pasting on GitHub
        }

        RainGui::Text("Dear RainGui %s (%d)", RAINGUI_VERSION, RAINGUI_VERSION_NUM);
        RainGui::Separator();
        RainGui::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
        RainGui::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef RAINGUI_DISABLE_OBSOLETE_FUNCTIONS
        RainGui::Text("define: RAINGUI_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef RAINGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
        RainGui::Text("define: RAINGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef RAINGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
        RainGui::Text("define: RAINGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef RAINGUI_DISABLE_WIN32_FUNCTIONS
        RainGui::Text("define: RAINGUI_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef RAINGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
        RainGui::Text("define: RAINGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS");
#endif
#ifdef RAINGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
        RainGui::Text("define: RAINGUI_DISABLE_DEFAULT_MATH_FUNCTIONS");
#endif
#ifdef RAINGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
        RainGui::Text("define: RAINGUI_DISABLE_DEFAULT_FILE_FUNCTIONS");
#endif
#ifdef RAINGUI_DISABLE_FILE_FUNCTIONS
        RainGui::Text("define: RAINGUI_DISABLE_FILE_FUNCTIONS");
#endif
#ifdef RAINGUI_DISABLE_DEFAULT_ALLOCATORS
        RainGui::Text("define: RAINGUI_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef RAINGUI_USE_BGRA_PACKED_COLOR
        RainGui::Text("define: RAINGUI_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
        RainGui::Text("define: _WIN32");
#endif
#ifdef _WIN64
        RainGui::Text("define: _WIN64");
#endif
#ifdef __linux__
        RainGui::Text("define: __linux__");
#endif
#ifdef __APPLE__
        RainGui::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
        RainGui::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef _MSVC_LANG
        RainGui::Text("define: _MSVC_LANG=%d", (int)_MSVC_LANG);
#endif
#ifdef __MINGW32__
        RainGui::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
        RainGui::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
        RainGui::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
        RainGui::Text("define: __clang_version__=%s", __clang_version__);
#endif
        RainGui::Separator();
        RainGui::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
        RainGui::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
        RainGui::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
        if (io.ConfigFlags & RainGuiConfigFlags_NavEnableKeyboard)        RainGui::Text(" NavEnableKeyboard");
        if (io.ConfigFlags & RainGuiConfigFlags_NavEnableGamepad)         RainGui::Text(" NavEnableGamepad");
        if (io.ConfigFlags & RainGuiConfigFlags_NavEnableSetMousePos)     RainGui::Text(" NavEnableSetMousePos");
        if (io.ConfigFlags & RainGuiConfigFlags_NavNoCaptureKeyboard)     RainGui::Text(" NavNoCaptureKeyboard");
        if (io.ConfigFlags & RainGuiConfigFlags_NoMouse)                  RainGui::Text(" NoMouse");
        if (io.ConfigFlags & RainGuiConfigFlags_NoMouseCursorChange)      RainGui::Text(" NoMouseCursorChange");
        if (io.MouseDrawCursor)                                         RainGui::Text("io.MouseDrawCursor");
        if (io.ConfigMacOSXBehaviors)                                   RainGui::Text("io.ConfigMacOSXBehaviors");
        if (io.ConfigInputTextCursorBlink)                              RainGui::Text("io.ConfigInputTextCursorBlink");
        if (io.ConfigWindowsResizeFromEdges)                            RainGui::Text("io.ConfigWindowsResizeFromEdges");
        if (io.ConfigWindowsMoveFromTitleBarOnly)                       RainGui::Text("io.ConfigWindowsMoveFromTitleBarOnly");
        if (io.ConfigMemoryCompactTimer >= 0.0f)                        RainGui::Text("io.ConfigMemoryCompactTimer = %.1f", io.ConfigMemoryCompactTimer);
        RainGui::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
        if (io.BackendFlags & RainGuiBackendFlags_HasGamepad)             RainGui::Text(" HasGamepad");
        if (io.BackendFlags & RainGuiBackendFlags_HasMouseCursors)        RainGui::Text(" HasMouseCursors");
        if (io.BackendFlags & RainGuiBackendFlags_HasSetMousePos)         RainGui::Text(" HasSetMousePos");
        if (io.BackendFlags & RainGuiBackendFlags_RendererHasVtxOffset)   RainGui::Text(" RendererHasVtxOffset");
        RainGui::Separator();
        RainGui::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
        RainGui::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        RainGui::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        RainGui::Separator();
        RainGui::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
        RainGui::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
        RainGui::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
        RainGui::Text("style.FrameRounding: %.2f", style.FrameRounding);
        RainGui::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
        RainGui::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
        RainGui::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

        if (copy_to_clipboard)
        {
            RainGui::LogText("\n```\n");
            RainGui::LogFinish();
        }
        RainGui::EndChildFrame();
    }
    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Font viewer / ShowFontAtlas()
//-----------------------------------------------------------------------------
// - ShowFontSelector()
// - ShowFont()
// - ShowFontAtlas()
//-----------------------------------------------------------------------------

// This isn't worth putting in public API but we want Metrics to use it
namespace RainGui { void ShowFontAtlas(ImFontAtlas* atlas); }

// Demo helper function to select among loaded fonts.
// Here we use the regular BeginCombo()/EndCombo() api which is more the more flexible one.
void RainGui::ShowFontSelector(const char* label)
{
    RainGuiIO& io = RainGui::GetIO();
    ImFont* font_current = RainGui::GetFont();
    if (RainGui::BeginCombo(label, font_current->GetDebugName()))
    {
        for (int n = 0; n < io.Fonts->Fonts.Size; n++)
        {
            ImFont* font = io.Fonts->Fonts[n];
            RainGui::PushID((void*)font);
            if (RainGui::Selectable(font->GetDebugName(), font == font_current))
                io.FontDefault = font;
            RainGui::PopID();
        }
        RainGui::EndCombo();
    }
    RainGui::SameLine();
    HelpMarker(
        "- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
        "- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
        "- Read FAQ and docs/FONTS.md for more details.\n"
        "- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");
}

// [Internal] Display details for a single font, called by ShowStyleEditor().
static void ShowFont(ImFont* font)
{
    RainGuiIO& io = RainGui::GetIO();
    RainGuiStyle& style = RainGui::GetStyle();
    bool font_details_opened = RainGui::TreeNode(font, "Font: \"%s\"\n%.2f px, %d glyphs, %d file(s)",
        font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size, font->ConfigDataCount);
    RainGui::SameLine(); if (RainGui::SmallButton("Set as default")) { io.FontDefault = font; }
    if (!font_details_opened)
        return;

    // Display preview text
    RainGui::PushFont(font);
    RainGui::Text("The quick brown fox jumps over the lazy dog");
    RainGui::PopFont();

    // Display details
    RainGui::SetNextItemWidth(RainGui::GetFontSize() * 8);
    RainGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
    RainGui::SameLine(); HelpMarker(
        "Note than the default embedded font is NOT meant to be scaled.\n\n"
        "Font are currently rendered into bitmaps at a given size at the time of building the atlas. "
        "You may oversample them to get some flexibility with scaling. "
        "You can also render at multiple sizes and select which one to use at runtime.\n\n"
        "(Glimmer of hope: the atlas system will be rewritten in the future to make scaling more flexible.)");
    RainGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
    RainGui::Text("Fallback character: '%c' (U+%04X)", font->FallbackChar, font->FallbackChar);
    RainGui::Text("Ellipsis character: '%c' (U+%04X)", font->EllipsisChar, font->EllipsisChar);
    const int surface_sqrt = (int)sqrtf((float)font->MetricsTotalSurface);
    RainGui::Text("Texture Area: about %d px ~%dx%d px", font->MetricsTotalSurface, surface_sqrt, surface_sqrt);
    for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
        if (font->ConfigData)
            if (const ImFontConfig* cfg = &font->ConfigData[config_i])
                RainGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d, Offset: (%.1f,%.1f)",
                    config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH, cfg->GlyphOffset.x, cfg->GlyphOffset.y);

    // Display all glyphs of the fonts in separate pages of 256 characters
    if (RainGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
    {
        const ImU32 glyph_col = RainGui::GetColorU32(RainGuiCol_Text);
        for (unsigned int base = 0; base <= IM_UNICODE_CODEPOINT_MAX; base += 256)
        {
            // Skip ahead if a large bunch of glyphs are not present in the font (test in chunks of 4k)
            // This is only a small optimization to reduce the number of iterations when IM_UNICODE_MAX_CODEPOINT
            // is large // (if ImWchar==ImWchar32 we will do at least about 272 queries here)
            if (!(base & 4095) && font->IsGlyphRangeUnused(base, base + 4095))
            {
                base += 4096 - 256;
                continue;
            }

            int count = 0;
            for (unsigned int n = 0; n < 256; n++)
                if (font->FindGlyphNoFallback((ImWchar)(base + n)))
                    count++;
            if (count <= 0)
                continue;
            if (!RainGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                continue;
            float cell_size = font->FontSize * 1;
            float cell_spacing = style.ItemSpacing.y;
            ImVec2 base_pos = RainGui::GetCursorScreenPos();
            ImDrawList* draw_list = RainGui::GetWindowDrawList();
            for (unsigned int n = 0; n < 256; n++)
            {
                // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions
                // available here and thus cannot easily generate a zero-terminated UTF-8 encoded string.
                ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                const ImFontGlyph* glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
                draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                if (glyph)
                    font->RenderChar(draw_list, cell_size, cell_p1, glyph_col, (ImWchar)(base + n));
                if (glyph && RainGui::IsMouseHoveringRect(cell_p1, cell_p2))
                {
                    RainGui::BeginTooltip();
                    RainGui::Text("Codepoint: U+%04X", base + n);
                    RainGui::Separator();
                    RainGui::Text("Visible: %d", glyph->Visible);
                    RainGui::Text("AdvanceX: %.1f", glyph->AdvanceX);
                    RainGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
                    RainGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
                    RainGui::EndTooltip();
                }
            }
            RainGui::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
            RainGui::TreePop();
        }
        RainGui::TreePop();
    }
    RainGui::TreePop();
}

void RainGui::ShowFontAtlas(ImFontAtlas* atlas)
{
    for (int i = 0; i < atlas->Fonts.Size; i++)
    {
        ImFont* font = atlas->Fonts[i];
        RainGui::PushID(font);
        ShowFont(font);
        RainGui::PopID();
    }
    if (RainGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
    {
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
        RainGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
        RainGui::TreePop();
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Style Editor / ShowStyleEditor()
//-----------------------------------------------------------------------------
// - ShowStyleSelector()
// - ShowStyleEditor()
//-----------------------------------------------------------------------------

// Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
// Here we use the simplified Combo() api that packs items into a single literal string.
// Useful for quick combo boxes where the choices are known locally.
bool RainGui::ShowStyleSelector(const char* label)
{
    static int style_idx = -1;
    if (RainGui::Combo(label, &style_idx, "Dark\0Light\0Classic\0"))
    {
        switch (style_idx)
        {
        case 0: RainGui::StyleColorsDark(); break;
        case 1: RainGui::StyleColorsLight(); break;
        case 2: RainGui::StyleColorsClassic(); break;
        }
        return true;
    }
    return false;
}

void RainGui::ShowStyleEditor(RainGuiStyle* ref)
{
    // You can pass in a reference RainGuiStyle structure to compare to, revert to and save to
    // (without a reference style pointer, we will use one compared locally as a reference)
    RainGuiStyle& style = RainGui::GetStyle();
    static RainGuiStyle ref_saved_style;

    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == NULL)
        ref_saved_style = style;
    init = false;
    if (ref == NULL)
        ref = &ref_saved_style;

    RainGui::PushItemWidth(RainGui::GetWindowWidth() * 0.50f);

    if (RainGui::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;
    RainGui::ShowFontSelector("Fonts##Selector");

    // Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
    if (RainGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
        style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
    { bool border = (style.WindowBorderSize > 0.0f); if (RainGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
    RainGui::SameLine();
    { bool border = (style.FrameBorderSize > 0.0f);  if (RainGui::Checkbox("FrameBorder",  &border)) { style.FrameBorderSize  = border ? 1.0f : 0.0f; } }
    RainGui::SameLine();
    { bool border = (style.PopupBorderSize > 0.0f);  if (RainGui::Checkbox("PopupBorder",  &border)) { style.PopupBorderSize  = border ? 1.0f : 0.0f; } }

    // Save/Revert button
    if (RainGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    RainGui::SameLine();
    if (RainGui::Button("Revert Ref"))
        style = *ref;
    RainGui::SameLine();
    HelpMarker(
        "Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
        "Use \"Export\" below to save them somewhere.");

    RainGui::Separator();

    if (RainGui::BeginTabBar("##tabs", RainGuiTabBarFlags_None))
    {
        if (RainGui::BeginTabItem("Sizes"))
        {
            RainGui::Text("Main");
            RainGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            RainGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            RainGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
            RainGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            RainGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            RainGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            RainGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            RainGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            RainGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            RainGui::Text("Borders");
            RainGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            RainGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            RainGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            RainGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            RainGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
            RainGui::Text("Rounding");
            RainGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            RainGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
            RainGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
            RainGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
            RainGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            RainGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
            RainGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
            RainGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
            RainGui::Text("Alignment");
            RainGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = style.WindowMenuButtonPosition + 1;
            if (RainGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                style.WindowMenuButtonPosition = window_menu_button_position - 1;
            RainGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
            RainGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            RainGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
            RainGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            RainGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
            RainGui::Text("Safe Area Padding");
            RainGui::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            RainGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            RainGui::EndTabItem();
        }

        if (RainGui::BeginTabItem("Colors"))
        {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (RainGui::Button("Export"))
            {
                if (output_dest == 0)
                    RainGui::LogToClipboard();
                else
                    RainGui::LogToTTY();
                RainGui::LogText("ImVec4* colors = RainGui::GetStyle().Colors;" IM_NEWLINE);
                for (int i = 0; i < RainGuiCol_COUNT; i++)
                {
                    const ImVec4& col = style.Colors[i];
                    const char* name = RainGui::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        RainGui::LogText("colors[RainGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
                            name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                }
                RainGui::LogFinish();
            }
            RainGui::SameLine(); RainGui::SetNextItemWidth(120); RainGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            RainGui::SameLine(); RainGui::Checkbox("Only Modified Colors", &output_only_modified);

            static RainGuiTextFilter filter;
            filter.Draw("Filter colors", RainGui::GetFontSize() * 16);

            static RainGuiColorEditFlags alpha_flags = 0;
            if (RainGui::RadioButton("Opaque", alpha_flags == RainGuiColorEditFlags_None))             { alpha_flags = RainGuiColorEditFlags_None; } RainGui::SameLine();
            if (RainGui::RadioButton("Alpha",  alpha_flags == RainGuiColorEditFlags_AlphaPreview))     { alpha_flags = RainGuiColorEditFlags_AlphaPreview; } RainGui::SameLine();
            if (RainGui::RadioButton("Both",   alpha_flags == RainGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = RainGuiColorEditFlags_AlphaPreviewHalf; } RainGui::SameLine();
            HelpMarker(
                "In the color list:\n"
                "Left-click on color square to open color picker,\n"
                "Right-click to open edit options menu.");

            RainGui::BeginChild("##colors", ImVec2(0, 0), true, RainGuiWindowFlags_AlwaysVerticalScrollbar | RainGuiWindowFlags_AlwaysHorizontalScrollbar | RainGuiWindowFlags_NavFlattened);
            RainGui::PushItemWidth(-160);
            for (int i = 0; i < RainGuiCol_COUNT; i++)
            {
                const char* name = RainGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                RainGui::PushID(i);
                RainGui::ColorEdit4("##color", (float*)&style.Colors[i], RainGuiColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                    // so instead of "Save"/"Revert" you'd use icons!
                    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                    RainGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (RainGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                    RainGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (RainGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
                }
                RainGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                RainGui::TextUnformatted(name);
                RainGui::PopID();
            }
            RainGui::PopItemWidth();
            RainGui::EndChild();

            RainGui::EndTabItem();
        }

        if (RainGui::BeginTabItem("Fonts"))
        {
            RainGuiIO& io = RainGui::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
            RainGui::ShowFontAtlas(atlas);

            // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
            // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
            const float MIN_SCALE = 0.3f;
            const float MAX_SCALE = 2.0f;
            HelpMarker(
                "Those are old settings provided for convenience.\n"
                "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
                "rebuild the font atlas, and call style.ScaleAllSizes() on a reference RainGuiStyle structure.\n"
                "Using those settings here will give you poor quality results.");
            static float window_scale = 1.0f;
            RainGui::PushItemWidth(RainGui::GetFontSize() * 8);
            if (RainGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", RainGuiSliderFlags_AlwaysClamp)) // Scale only this window
                RainGui::SetWindowFontScale(window_scale);
            RainGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", RainGuiSliderFlags_AlwaysClamp); // Scale everything
            RainGui::PopItemWidth();

            RainGui::EndTabItem();
        }

        if (RainGui::BeginTabItem("Rendering"))
        {
            RainGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            RainGui::SameLine();
            HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

            RainGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
            RainGui::SameLine();
            HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

            RainGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
            RainGui::PushItemWidth(RainGui::GetFontSize() * 8);
            RainGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
            if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

            // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
            RainGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", RainGuiSliderFlags_AlwaysClamp);
            if (RainGui::IsItemActive())
            {
                RainGui::SetNextWindowPos(RainGui::GetCursorScreenPos());
                RainGui::BeginTooltip();
                RainGui::TextUnformatted("(R = radius, N = number of segments)");
                RainGui::Spacing();
                ImDrawList* draw_list = RainGui::GetWindowDrawList();
                const float min_widget_width = RainGui::CalcTextSize("N: MMM\nR: MMM").x;
                for (int n = 0; n < 8; n++)
                {
                    const float RAD_MIN = 5.0f;
                    const float RAD_MAX = 70.0f;
                    const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

                    RainGui::BeginGroup();

                    RainGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

                    const float canvas_width = IM_MAX(min_widget_width, rad * 2.0f);
                    const float offset_x     = floorf(canvas_width * 0.5f);
                    const float offset_y     = floorf(RAD_MAX);

                    const ImVec2 p1 = RainGui::GetCursorScreenPos();
                    draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, RainGui::GetColorU32(RainGuiCol_Text));
                    RainGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

                    /*
                    const ImVec2 p2 = RainGui::GetCursorScreenPos();
                    draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, RainGui::GetColorU32(RainGuiCol_Text));
                    RainGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));
                    */

                    RainGui::EndGroup();
                    RainGui::SameLine();
                }
                RainGui::EndTooltip();
            }
            RainGui::SameLine();
            HelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");

            RainGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
            RainGui::PopItemWidth();

            RainGui::EndTabItem();
        }

        RainGui::EndTabBar();
    }

    RainGui::PopItemWidth();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Main Menu Bar / ShowExampleAppMainMenuBar()
//-----------------------------------------------------------------------------
// - ShowExampleAppMainMenuBar()
// - ShowExampleMenuFile()
//-----------------------------------------------------------------------------

// Demonstrate creating a "main" fullscreen menu bar and populating it.
// Note the difference between BeginMainMenuBar() and BeginMenuBar():
// - BeginMenuBar() = menu-bar inside current window (which needs the RainGuiWindowFlags_MenuBar flag!)
// - BeginMainMenuBar() = helper to create menu-bar-sized window at the top of the main viewport + call BeginMenuBar() into it.
static void ShowExampleAppMainMenuBar()
{
    if (RainGui::BeginMainMenuBar())
    {
        if (RainGui::BeginMenu("File"))
        {
            ShowExampleMenuFile();
            RainGui::EndMenu();
        }
        if (RainGui::BeginMenu("Edit"))
        {
            if (RainGui::MenuItem("Undo", "CTRL+Z")) {}
            if (RainGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            RainGui::Separator();
            if (RainGui::MenuItem("Cut", "CTRL+X")) {}
            if (RainGui::MenuItem("Copy", "CTRL+C")) {}
            if (RainGui::MenuItem("Paste", "CTRL+V")) {}
            RainGui::EndMenu();
        }
        RainGui::EndMainMenuBar();
    }
}

// Note that shortcuts are currently provided for display only
// (future version will add explicit flags to BeginMenu() to request processing shortcuts)
static void ShowExampleMenuFile()
{
    RainGui::MenuItem("(demo menu)", NULL, false, false);
    if (RainGui::MenuItem("New")) {}
    if (RainGui::MenuItem("Open", "Ctrl+O")) {}
    if (RainGui::BeginMenu("Open Recent"))
    {
        RainGui::MenuItem("fish_hat.c");
        RainGui::MenuItem("fish_hat.inl");
        RainGui::MenuItem("fish_hat.h");
        if (RainGui::BeginMenu("More.."))
        {
            RainGui::MenuItem("Hello");
            RainGui::MenuItem("Sailor");
            if (RainGui::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                RainGui::EndMenu();
            }
            RainGui::EndMenu();
        }
        RainGui::EndMenu();
    }
    if (RainGui::MenuItem("Save", "Ctrl+S")) {}
    if (RainGui::MenuItem("Save As..")) {}

    RainGui::Separator();
    if (RainGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        RainGui::MenuItem("Enabled", "", &enabled);
        RainGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            RainGui::Text("Scrolling Text %d", i);
        RainGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        RainGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        RainGui::InputFloat("Input", &f, 0.1f);
        RainGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        RainGui::EndMenu();
    }

    if (RainGui::BeginMenu("Colors"))
    {
        float sz = RainGui::GetTextLineHeight();
        for (int i = 0; i < RainGuiCol_COUNT; i++)
        {
            const char* name = RainGui::GetStyleColorName((RainGuiCol)i);
            ImVec2 p = RainGui::GetCursorScreenPos();
            RainGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), RainGui::GetColorU32((RainGuiCol)i));
            RainGui::Dummy(ImVec2(sz, sz));
            RainGui::SameLine();
            RainGui::MenuItem(name);
        }
        RainGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (RainGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        RainGui::Checkbox("SomeOption", &b);
        RainGui::EndMenu();
    }

    if (RainGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (RainGui::MenuItem("Checked", NULL, true)) {}
    if (RainGui::MenuItem("Quit", "Alt+F4")) {}
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, we are using a more C++ like approach of declaring a class to hold both data and functions.
struct ExampleAppConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    RainGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    ExampleAppConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        Commands.push_back("HELP");
        Commands.push_back("HISTORY");
        Commands.push_back("CLEAR");
        Commands.push_back("CLASSIFY");
        AutoScroll = true;
        ScrollToBottom = false;
        AddLog("Welcome to Dear RainGui!");
    }
    ~ExampleAppConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    void    Draw(const char* title, bool* p_open)
    {
        RainGui::SetNextWindowSize(ImVec2(520, 600), RainGuiCond_FirstUseEver);
        if (!RainGui::Begin(title, p_open))
        {
            RainGui::End();
            return;
        }

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (RainGui::BeginPopupContextItem())
        {
            if (RainGui::MenuItem("Close Console"))
                *p_open = false;
            RainGui::EndPopup();
        }

        RainGui::TextWrapped(
            "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
            "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
        RainGui::TextWrapped("Enter 'HELP' for help.");

        // TODO: display items starting from the bottom

        if (RainGui::SmallButton("Add Debug Text"))  { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); }
        RainGui::SameLine();
        if (RainGui::SmallButton("Add Debug Error")) { AddLog("[error] something went wrong"); }
        RainGui::SameLine();
        if (RainGui::SmallButton("Clear"))           { ClearLog(); }
        RainGui::SameLine();
        bool copy_to_clipboard = RainGui::SmallButton("Copy");
        //static float t = 0.0f; if (RainGui::GetTime() - t > 0.02f) { t = RainGui::GetTime(); AddLog("Spam %f", t); }

        RainGui::Separator();

        // Options menu
        if (RainGui::BeginPopup("Options"))
        {
            RainGui::Checkbox("Auto-scroll", &AutoScroll);
            RainGui::EndPopup();
        }

        // Options, Filter
        if (RainGui::Button("Options"))
            RainGui::OpenPopup("Options");
        RainGui::SameLine();
        Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        RainGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = RainGui::GetStyle().ItemSpacing.y + RainGui::GetFrameHeightWithSpacing();
        RainGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, RainGuiWindowFlags_HorizontalScrollbar);
        if (RainGui::BeginPopupContextWindow())
        {
            if (RainGui::Selectable("Clear")) ClearLog();
            RainGui::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use RainGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      RainGuiListClipper clipper;
        //      clipper.Begin(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        RainGui::PushStyleVar(RainGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            RainGui::LogToClipboard();
        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            if (!Filter.PassFilter(item))
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[error]"))          { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                RainGui::PushStyleColor(RainGuiCol_Text, color);
            RainGui::TextUnformatted(item);
            if (has_color)
                RainGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            RainGui::LogFinish();

        if (ScrollToBottom || (AutoScroll && RainGui::GetScrollY() >= RainGui::GetScrollMaxY()))
            RainGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        RainGui::PopStyleVar();
        RainGui::EndChild();
        RainGui::Separator();

        // Command-line
        bool reclaim_focus = false;
        RainGuiInputTextFlags input_text_flags = RainGuiInputTextFlags_EnterReturnsTrue | RainGuiInputTextFlags_CallbackCompletion | RainGuiInputTextFlags_CallbackHistory;
        if (RainGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
        {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0])
                ExecCommand(s);
            strcpy(s, "");
            reclaim_focus = true;
        }

        // Auto-focus on window apparition
        RainGui::SetItemDefaultFocus();
        if (reclaim_focus)
            RainGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        RainGui::End();
    }

    void    ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            ClearLog();
        }
        else if (Stricmp(command_line, "HELP") == 0)
        {
            AddLog("Commands:");
            for (int i = 0; i < Commands.Size; i++)
                AddLog("- %s", Commands[i]);
        }
        else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = History.Size - 10;
            for (int i = first > 0 ? first : 0; i < History.Size; i++)
                AddLog("%3d: %s\n", i, History[i]);
        }
        else
        {
            AddLog("Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(RainGuiInputTextCallbackData* data)
    {
        ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int     TextEditCallback(RainGuiInputTextCallbackData* data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
        case RainGuiInputTextFlags_CallbackCompletion:
            {
                // Example of TEXT COMPLETION

                // Locate beginning of current word
                const char* word_end = data->Buf + data->CursorPos;
                const char* word_start = word_end;
                while (word_start > data->Buf)
                {
                    const char c = word_start[-1];
                    if (c == ' ' || c == '\t' || c == ',' || c == ';')
                        break;
                    word_start--;
                }

                // Build a list of candidates
                ImVector<const char*> candidates;
                for (int i = 0; i < Commands.Size; i++)
                    if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                        candidates.push_back(Commands[i]);

                if (candidates.Size == 0)
                {
                    // No match
                    AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
                }
                else if (candidates.Size == 1)
                {
                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0]);
                    data->InsertChars(data->CursorPos, " ");
                }
                else
                {
                    // Multiple matches. Complete as much as we can..
                    // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                    int match_len = (int)(word_end - word_start);
                    for (;;)
                    {
                        int c = 0;
                        bool all_candidates_matches = true;
                        for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                            if (i == 0)
                                c = toupper(candidates[i][match_len]);
                            else if (c == 0 || c != toupper(candidates[i][match_len]))
                                all_candidates_matches = false;
                        if (!all_candidates_matches)
                            break;
                        match_len++;
                    }

                    if (match_len > 0)
                    {
                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                        data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                    }

                    // List matches
                    AddLog("Possible matches:\n");
                    for (int i = 0; i < candidates.Size; i++)
                        AddLog("- %s\n", candidates[i]);
                }

                break;
            }
        case RainGuiInputTextFlags_CallbackHistory:
            {
                // Example of HISTORY
                const int prev_history_pos = HistoryPos;
                if (data->EventKey == RainGuiKey_UpArrow)
                {
                    if (HistoryPos == -1)
                        HistoryPos = History.Size - 1;
                    else if (HistoryPos > 0)
                        HistoryPos--;
                }
                else if (data->EventKey == RainGuiKey_DownArrow)
                {
                    if (HistoryPos != -1)
                        if (++HistoryPos >= History.Size)
                            HistoryPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if (prev_history_pos != HistoryPos)
                {
                    const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, history_str);
                }
            }
        }
        return 0;
    }
};

static void ShowExampleAppConsole(bool* p_open)
{
    static ExampleAppConsole console;
    console.Draw("Example: Console", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
    RainGuiTextBuffer     Buf;
    RainGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!RainGui::Begin(title, p_open))
        {
            RainGui::End();
            return;
        }

        // Options menu
        if (RainGui::BeginPopup("Options"))
        {
            RainGui::Checkbox("Auto-scroll", &AutoScroll);
            RainGui::EndPopup();
        }

        // Main window
        if (RainGui::Button("Options"))
            RainGui::OpenPopup("Options");
        RainGui::SameLine();
        bool clear = RainGui::Button("Clear");
        RainGui::SameLine();
        bool copy = RainGui::Button("Copy");
        RainGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        RainGui::Separator();
        RainGui::BeginChild("scrolling", ImVec2(0, 0), false, RainGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            RainGui::LogToClipboard();

        RainGui::PushStyleVar(RainGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char* line_start = buf + LineOffsets[line_no];
                const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    RainGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   RainGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using RainGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            RainGuiListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    RainGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        RainGui::PopStyleVar();

        if (AutoScroll && RainGui::GetScrollY() >= RainGui::GetScrollMaxY())
            RainGui::SetScrollHereY(1.0f);

        RainGui::EndChild();
        RainGui::End();
    }
};

// Demonstrate creating a simple log window with basic filtering.
static void ShowExampleAppLog(bool* p_open)
{
    static ExampleAppLog log;

    // For the demo: add a debug button _BEFORE_ the normal log window contents
    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    RainGui::SetNextWindowSize(ImVec2(500, 400), RainGuiCond_FirstUseEver);
    RainGui::Begin("Example: Log", p_open);
    if (RainGui::SmallButton("[Debug] Add 5 entries"))
    {
        static int counter = 0;
        const char* categories[3] = { "info", "warn", "error" };
        const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
        for (int n = 0; n < 5; n++)
        {
            const char* category = categories[counter % IM_ARRAYSIZE(categories)];
            const char* word = words[counter % IM_ARRAYSIZE(words)];
            log.AddLog("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
                RainGui::GetFrameCount(), category, RainGui::GetTime(), word);
            counter++;
        }
    }
    RainGui::End();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    log.Draw("Example: Log", p_open);
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple Layout / ShowExampleAppLayout()
//-----------------------------------------------------------------------------

// Demonstrate create a window with multiple child windows.
static void ShowExampleAppLayout(bool* p_open)
{
    RainGui::SetNextWindowSize(ImVec2(500, 440), RainGuiCond_FirstUseEver);
    if (RainGui::Begin("Example: Simple layout", p_open, RainGuiWindowFlags_MenuBar))
    {
        if (RainGui::BeginMenuBar())
        {
            if (RainGui::BeginMenu("File"))
            {
                if (RainGui::MenuItem("Close")) *p_open = false;
                RainGui::EndMenu();
            }
            RainGui::EndMenuBar();
        }

        // Left
        static int selected = 0;
        {
            RainGui::BeginChild("left pane", ImVec2(150, 0), true);
            for (int i = 0; i < 100; i++)
            {
                char label[128];
                sprintf(label, "MyObject %d", i);
                if (RainGui::Selectable(label, selected == i))
                    selected = i;
            }
            RainGui::EndChild();
        }
        RainGui::SameLine();

        // Right
        {
            RainGui::BeginGroup();
            RainGui::BeginChild("item view", ImVec2(0, -RainGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            RainGui::Text("MyObject: %d", selected);
            RainGui::Separator();
            if (RainGui::BeginTabBar("##Tabs", RainGuiTabBarFlags_None))
            {
                if (RainGui::BeginTabItem("Description"))
                {
                    RainGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                    RainGui::EndTabItem();
                }
                if (RainGui::BeginTabItem("Details"))
                {
                    RainGui::Text("ID: 0123456789");
                    RainGui::EndTabItem();
                }
                RainGui::EndTabBar();
            }
            RainGui::EndChild();
            if (RainGui::Button("Revert")) {}
            RainGui::SameLine();
            if (RainGui::Button("Save")) {}
            RainGui::EndGroup();
        }
    }
    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Property Editor / ShowExampleAppPropertyEditor()
//-----------------------------------------------------------------------------

static void ShowPlaceholderObject(const char* prefix, int uid)
{
    // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
    RainGui::PushID(uid);

    // Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
    RainGui::TableNextRow();
    RainGui::TableSetColumnIndex(0);
    RainGui::AlignTextToFramePadding();
    bool node_open = RainGui::TreeNode("Object", "%s_%u", prefix, uid);
    RainGui::TableSetColumnIndex(1);
    RainGui::Text("my sailor is rich");

    if (node_open)
    {
        static float placeholder_members[8] = { 0.0f, 0.0f, 1.0f, 3.1416f, 100.0f, 999.0f };
        for (int i = 0; i < 8; i++)
        {
            RainGui::PushID(i); // Use field index as identifier.
            if (i < 2)
            {
                ShowPlaceholderObject("Child", 424242);
            }
            else
            {
                // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                RainGui::TableNextRow();
                RainGui::TableSetColumnIndex(0);
                RainGui::AlignTextToFramePadding();
                RainGuiTreeNodeFlags flags = RainGuiTreeNodeFlags_Leaf | RainGuiTreeNodeFlags_NoTreePushOnOpen | RainGuiTreeNodeFlags_Bullet;
                RainGui::TreeNodeEx("Field", flags, "Field_%d", i);

                RainGui::TableSetColumnIndex(1);
                RainGui::SetNextItemWidth(-FLT_MIN);
                if (i >= 5)
                    RainGui::InputFloat("##value", &placeholder_members[i], 1.0f);
                else
                    RainGui::DragFloat("##value", &placeholder_members[i], 0.01f);
                RainGui::NextColumn();
            }
            RainGui::PopID();
        }
        RainGui::TreePop();
    }
    RainGui::PopID();
}

// Demonstrate create a simple property editor.
static void ShowExampleAppPropertyEditor(bool* p_open)
{
    RainGui::SetNextWindowSize(ImVec2(430, 450), RainGuiCond_FirstUseEver);
    if (!RainGui::Begin("Example: Property editor", p_open))
    {
        RainGui::End();
        return;
    }

    HelpMarker(
        "This example shows how you may implement a property editor using two columns.\n"
        "All objects/fields data are dummies here.\n"
        "Remember that in many simple cases, you can use RainGui::SameLine(xxx) to position\n"
        "your cursor horizontally instead of using the Columns() API.");

    RainGui::PushStyleVar(RainGuiStyleVar_FramePadding, ImVec2(2, 2));
    if (RainGui::BeginTable("split", 2, RainGuiTableFlags_BordersOuter | RainGuiTableFlags_Resizable))
    {
        // Iterate placeholder objects (all the same data)
        for (int obj_i = 0; obj_i < 4; obj_i++)
        {
            ShowPlaceholderObject("Object", obj_i);
            //RainGui::Separator();
        }
        RainGui::EndTable();
    }
    RainGui::PopStyleVar();
    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Long Text / ShowExampleAppLongText()
//-----------------------------------------------------------------------------

// Demonstrate/test rendering huge amount of text, and the incidence of clipping.
static void ShowExampleAppLongText(bool* p_open)
{
    RainGui::SetNextWindowSize(ImVec2(520, 600), RainGuiCond_FirstUseEver);
    if (!RainGui::Begin("Example: Long text display", p_open))
    {
        RainGui::End();
        return;
    }

    static int test_type = 0;
    static RainGuiTextBuffer log;
    static int lines = 0;
    RainGui::Text("Printing unusually long amount of text.");
    RainGui::Combo("Test type", &test_type,
        "Single call to TextUnformatted()\0"
        "Multiple calls to Text(), clipped\0"
        "Multiple calls to Text(), not clipped (slow)\0");
    RainGui::Text("Buffer contents: %d lines, %d bytes", lines, log.size());
    if (RainGui::Button("Clear")) { log.clear(); lines = 0; }
    RainGui::SameLine();
    if (RainGui::Button("Add 1000 lines"))
    {
        for (int i = 0; i < 1000; i++)
            log.appendf("%i The quick brown fox jumps over the lazy dog\n", lines + i);
        lines += 1000;
    }
    RainGui::BeginChild("Log");
    switch (test_type)
    {
    case 0:
        // Single call to TextUnformatted() with a big buffer
        RainGui::TextUnformatted(log.begin(), log.end());
        break;
    case 1:
        {
            // Multiple calls to Text(), manually coarsely clipped - demonstrate how to use the RainGuiListClipper helper.
            RainGui::PushStyleVar(RainGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            RainGuiListClipper clipper;
            clipper.Begin(lines);
            while (clipper.Step())
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                    RainGui::Text("%i The quick brown fox jumps over the lazy dog", i);
            RainGui::PopStyleVar();
            break;
        }
    case 2:
        // Multiple calls to Text(), not clipped (slow)
        RainGui::PushStyleVar(RainGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        for (int i = 0; i < lines; i++)
            RainGui::Text("%i The quick brown fox jumps over the lazy dog", i);
        RainGui::PopStyleVar();
        break;
    }
    RainGui::EndChild();
    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Auto Resize / ShowExampleAppAutoResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window which gets auto-resized according to its content.
static void ShowExampleAppAutoResize(bool* p_open)
{
    if (!RainGui::Begin("Example: Auto-resizing window", p_open, RainGuiWindowFlags_AlwaysAutoResize))
    {
        RainGui::End();
        return;
    }

    static int lines = 10;
    RainGui::TextUnformatted(
        "Window will resize every-frame to the size of its content.\n"
        "Note that you probably don't want to query the window size to\n"
        "output your content because that would create a feedback loop.");
    RainGui::SliderInt("Number of lines", &lines, 1, 20);
    for (int i = 0; i < lines; i++)
        RainGui::Text("%*sThis is line %d", i * 4, "", i); // Pad with space to extend size horizontally
    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Constrained Resize / ShowExampleAppConstrainedResize()
//-----------------------------------------------------------------------------

// Demonstrate creating a window with custom resize constraints.
static void ShowExampleAppConstrainedResize(bool* p_open)
{
    struct CustomConstraints
    {
        // Helper functions to demonstrate programmatic constraints
        static void Square(RainGuiSizeCallbackData* data) { data->DesiredSize.x = data->DesiredSize.y = IM_MAX(data->DesiredSize.x, data->DesiredSize.y); }
        static void Step(RainGuiSizeCallbackData* data)   { float step = (float)(int)(intptr_t)data->UserData; data->DesiredSize = ImVec2((int)(data->DesiredSize.x / step + 0.5f) * step, (int)(data->DesiredSize.y / step + 0.5f) * step); }
    };

    const char* test_desc[] =
    {
        "Resize vertical only",
        "Resize horizontal only",
        "Width > 100, Height > 100",
        "Width 400-500",
        "Height 400-500",
        "Custom: Always Square",
        "Custom: Fixed Steps (100)",
    };

    static bool auto_resize = false;
    static int type = 0;
    static int display_lines = 10;
    if (type == 0) RainGui::SetNextWindowSizeConstraints(ImVec2(-1, 0),    ImVec2(-1, FLT_MAX));      // Vertical only
    if (type == 1) RainGui::SetNextWindowSizeConstraints(ImVec2(0, -1),    ImVec2(FLT_MAX, -1));      // Horizontal only
    if (type == 2) RainGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(FLT_MAX, FLT_MAX)); // Width > 100, Height > 100
    if (type == 3) RainGui::SetNextWindowSizeConstraints(ImVec2(400, -1),  ImVec2(500, -1));          // Width 400-500
    if (type == 4) RainGui::SetNextWindowSizeConstraints(ImVec2(-1, 400),  ImVec2(-1, 500));          // Height 400-500
    if (type == 5) RainGui::SetNextWindowSizeConstraints(ImVec2(0, 0),     ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Square);                     // Always Square
    if (type == 6) RainGui::SetNextWindowSizeConstraints(ImVec2(0, 0),     ImVec2(FLT_MAX, FLT_MAX), CustomConstraints::Step, (void*)(intptr_t)100); // Fixed Step

    RainGuiWindowFlags flags = auto_resize ? RainGuiWindowFlags_AlwaysAutoResize : 0;
    if (RainGui::Begin("Example: Constrained Resize", p_open, flags))
    {
        if (RainGui::Button("200x200")) { RainGui::SetWindowSize(ImVec2(200, 200)); } RainGui::SameLine();
        if (RainGui::Button("500x500")) { RainGui::SetWindowSize(ImVec2(500, 500)); } RainGui::SameLine();
        if (RainGui::Button("800x200")) { RainGui::SetWindowSize(ImVec2(800, 200)); }
        RainGui::SetNextItemWidth(200);
        RainGui::Combo("Constraint", &type, test_desc, IM_ARRAYSIZE(test_desc));
        RainGui::SetNextItemWidth(200);
        RainGui::DragInt("Lines", &display_lines, 0.2f, 1, 100);
        RainGui::Checkbox("Auto-resize", &auto_resize);
        for (int i = 0; i < display_lines; i++)
            RainGui::Text("%*sHello, sailor! Making this line long enough for the example.", i * 4, "");
    }
    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Simple overlay / ShowExampleAppSimpleOverlay()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple static window with no decoration
// + a context-menu to choose which corner of the screen to use.
static void ShowExampleAppSimpleOverlay(bool* p_open)
{
    const float PAD = 10.0f;
    static int corner = 0;
    RainGuiIO& io = RainGui::GetIO();
    RainGuiWindowFlags window_flags = RainGuiWindowFlags_NoDecoration | RainGuiWindowFlags_AlwaysAutoResize | RainGuiWindowFlags_NoSavedSettings | RainGuiWindowFlags_NoFocusOnAppearing | RainGuiWindowFlags_NoNav;
    if (corner != -1)
    {
        const RainGuiViewport* viewport = RainGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        RainGui::SetNextWindowPos(window_pos, RainGuiCond_Always, window_pos_pivot);
        window_flags |= RainGuiWindowFlags_NoMove;
    }
    RainGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (RainGui::Begin("Example: Simple overlay", p_open, window_flags))
    {
        RainGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
        RainGui::Separator();
        if (RainGui::IsMousePosValid())
            RainGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            RainGui::Text("Mouse Position: <invalid>");
        if (RainGui::BeginPopupContextWindow())
        {
            if (RainGui::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
            if (RainGui::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
            if (RainGui::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
            if (RainGui::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
            if (RainGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && RainGui::MenuItem("Close")) *p_open = false;
            RainGui::EndPopup();
        }
    }
    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Fullscreen window / ShowExampleAppFullscreen()
//-----------------------------------------------------------------------------

// Demonstrate creating a window covering the entire screen/viewport
static void ShowExampleAppFullscreen(bool* p_open)
{
    static bool use_work_area = true;
    static RainGuiWindowFlags flags = RainGuiWindowFlags_NoDecoration | RainGuiWindowFlags_NoMove | RainGuiWindowFlags_NoResize | RainGuiWindowFlags_NoSavedSettings;

    // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
    // Based on your use case you may want one of the other.
    const RainGuiViewport* viewport = RainGui::GetMainViewport();
    RainGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    RainGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

    if (RainGui::Begin("Example: Fullscreen window", p_open, flags))
    {
        RainGui::Checkbox("Use work area instead of main area", &use_work_area);
        RainGui::SameLine();
        HelpMarker("Main Area = entire viewport,\nWork Area = entire viewport minus sections used by the main menu bars, task bars etc.\n\nEnable the main-menu bar in Examples menu to see the difference.");

        RainGui::CheckboxFlags("RainGuiWindowFlags_NoBackground", &flags, RainGuiWindowFlags_NoBackground);
        RainGui::CheckboxFlags("RainGuiWindowFlags_NoDecoration", &flags, RainGuiWindowFlags_NoDecoration);
        RainGui::Indent();
        RainGui::CheckboxFlags("RainGuiWindowFlags_NoTitleBar", &flags, RainGuiWindowFlags_NoTitleBar);
        RainGui::CheckboxFlags("RainGuiWindowFlags_NoCollapse", &flags, RainGuiWindowFlags_NoCollapse);
        RainGui::CheckboxFlags("RainGuiWindowFlags_NoScrollbar", &flags, RainGuiWindowFlags_NoScrollbar);
        RainGui::Unindent();

        if (p_open && RainGui::Button("Close this window"))
            *p_open = false;
    }
    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Manipulating Window Titles / ShowExampleAppWindowTitles()
//-----------------------------------------------------------------------------

// Demonstrate using "##" and "###" in identifiers to manipulate ID generation.
// This apply to all regular items as well.
// Read FAQ section "How can I have multiple widgets with the same label?" for details.
static void ShowExampleAppWindowTitles(bool*)
{
    const RainGuiViewport* viewport = RainGui::GetMainViewport();
    const ImVec2 base_pos = viewport->Pos;

    // By default, Windows are uniquely identified by their title.
    // You can use the "##" and "###" markers to manipulate the display/ID.

    // Using "##" to display same title but have unique identifier.
    RainGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 100), RainGuiCond_FirstUseEver);
    RainGui::Begin("Same title as another window##1");
    RainGui::Text("This is window 1.\nMy title is the same as window 2, but my identifier is unique.");
    RainGui::End();

    RainGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 200), RainGuiCond_FirstUseEver);
    RainGui::Begin("Same title as another window##2");
    RainGui::Text("This is window 2.\nMy title is the same as window 1, but my identifier is unique.");
    RainGui::End();

    // Using "###" to display a changing title but keep a static identifier "AnimatedTitle"
    char buf[128];
    sprintf(buf, "Animated title %c %d###AnimatedTitle", "|/-\\"[(int)(RainGui::GetTime() / 0.25f) & 3], RainGui::GetFrameCount());
    RainGui::SetNextWindowPos(ImVec2(base_pos.x + 100, base_pos.y + 300), RainGuiCond_FirstUseEver);
    RainGui::Begin(buf);
    RainGui::Text("This window has a changing title.");
    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Custom Rendering using ImDrawList API / ShowExampleAppCustomRendering()
//-----------------------------------------------------------------------------

// Demonstrate using the low-level ImDrawList to draw custom shapes.
static void ShowExampleAppCustomRendering(bool* p_open)
{
    if (!RainGui::Begin("Example: Custom rendering", p_open))
    {
        RainGui::End();
        return;
    }

    // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of
    // overloaded operators, etc. Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your
    // types and ImVec2/ImVec4. Dear RainGui defines overloaded operators but they are internal to raingui.cpp and not
    // exposed outside (to avoid messing with your types) In this example we are not using the maths operators!

    if (RainGui::BeginTabBar("##TabBar"))
    {
        if (RainGui::BeginTabItem("Primitives"))
        {
            RainGui::PushItemWidth(-RainGui::GetFontSize() * 15);
            ImDrawList* draw_list = RainGui::GetWindowDrawList();

            // Draw gradients
            // (note that those are currently exacerbating our sRGB/Linear issues)
            // Calling RainGui::GetColorU32() multiplies the given colors by the current Style Alpha, but you may pass the IM_COL32() directly as well..
            RainGui::Text("Gradients");
            ImVec2 gradient_size = ImVec2(RainGui::CalcItemWidth(), RainGui::GetFrameHeight());
            {
                ImVec2 p0 = RainGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = RainGui::GetColorU32(IM_COL32(0, 0, 0, 255));
                ImU32 col_b = RainGui::GetColorU32(IM_COL32(255, 255, 255, 255));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                RainGui::InvisibleButton("##gradient1", gradient_size);
            }
            {
                ImVec2 p0 = RainGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + gradient_size.x, p0.y + gradient_size.y);
                ImU32 col_a = RainGui::GetColorU32(IM_COL32(0, 255, 0, 255));
                ImU32 col_b = RainGui::GetColorU32(IM_COL32(255, 0, 0, 255));
                draw_list->AddRectFilledMultiColor(p0, p1, col_a, col_b, col_b, col_a);
                RainGui::InvisibleButton("##gradient2", gradient_size);
            }

            // Draw a bunch of primitives
            RainGui::Text("All primitives");
            static float sz = 36.0f;
            static float thickness = 3.0f;
            static int ngon_sides = 6;
            static bool circle_segments_override = false;
            static int circle_segments_override_v = 12;
            static bool curve_segments_override = false;
            static int curve_segments_override_v = 8;
            static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
            RainGui::DragFloat("Size", &sz, 0.2f, 2.0f, 100.0f, "%.0f");
            RainGui::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
            RainGui::SliderInt("N-gon sides", &ngon_sides, 3, 12);
            RainGui::Checkbox("##circlesegmentoverride", &circle_segments_override);
            RainGui::SameLine(0.0f, RainGui::GetStyle().ItemInnerSpacing.x);
            circle_segments_override |= RainGui::SliderInt("Circle segments override", &circle_segments_override_v, 3, 40);
            RainGui::Checkbox("##curvessegmentoverride", &curve_segments_override);
            RainGui::SameLine(0.0f, RainGui::GetStyle().ItemInnerSpacing.x);
            curve_segments_override |= RainGui::SliderInt("Curves segments override", &curve_segments_override_v, 3, 40);
            RainGui::ColorEdit4("Color", &colf.x);

            const ImVec2 p = RainGui::GetCursorScreenPos();
            const ImU32 col = ImColor(colf);
            const float spacing = 10.0f;
            const ImDrawFlags corners_tl_br = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomRight;
            const float rounding = sz / 5.0f;
            const int circle_segments = circle_segments_override ? circle_segments_override_v : 0;
            const int curve_segments = curve_segments_override ? curve_segments_override_v : 0;
            float x = p.x + 4.0f;
            float y = p.y + 4.0f;
            for (int n = 0; n < 2; n++)
            {
                // First line uses a thickness of 1.0f, second line uses the configurable thickness
                float th = (n == 0) ? 1.0f : thickness;
                draw_list->AddNgon(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, ngon_sides, th);                 x += sz + spacing;  // N-gon
                draw_list->AddCircle(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, circle_segments, th);          x += sz + spacing;  // Circle
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 0.0f, ImDrawFlags_None, th);          x += sz + spacing;  // Square
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, rounding, ImDrawFlags_None, th);      x += sz + spacing;  // Square with all rounded corners
                draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col, rounding, corners_tl_br, th);         x += sz + spacing;  // Square with two rounded corners
                draw_list->AddTriangle(ImVec2(x+sz*0.5f,y), ImVec2(x+sz, y+sz-0.5f), ImVec2(x, y+sz-0.5f), col, th);x += sz + spacing;  // Triangle
                //draw_list->AddTriangle(ImVec2(x+sz*0.2f,y), ImVec2(x, y+sz-0.5f), ImVec2(x+sz*0.4f, y+sz-0.5f), col, th);x+= sz*0.4f + spacing; // Thin triangle
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col, th);                                       x += sz + spacing;  // Horizontal line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col, th);                                       x += spacing;       // Vertical line (note: drawing a filled rectangle will be faster!)
                draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y + sz), col, th);                                  x += sz + spacing;  // Diagonal line

                // Quadratic Bezier Curve (3 control points)
                ImVec2 cp3[3] = { ImVec2(x, y + sz * 0.6f), ImVec2(x + sz * 0.5f, y - sz * 0.4f), ImVec2(x + sz, y + sz) };
                draw_list->AddBezierQuadratic(cp3[0], cp3[1], cp3[2], col, th, curve_segments); x += sz + spacing;

                // Cubic Bezier Curve (4 control points)
                ImVec2 cp4[4] = { ImVec2(x, y), ImVec2(x + sz * 1.3f, y + sz * 0.3f), ImVec2(x + sz - sz * 1.3f, y + sz - sz * 0.3f), ImVec2(x + sz, y + sz) };
                draw_list->AddBezierCubic(cp4[0], cp4[1], cp4[2], cp4[3], col, th, curve_segments);

                x = p.x + 4;
                y += sz + spacing;
            }
            draw_list->AddNgonFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz*0.5f, col, ngon_sides);               x += sz + spacing;  // N-gon
            draw_list->AddCircleFilled(ImVec2(x + sz*0.5f, y + sz*0.5f), sz*0.5f, col, circle_segments);            x += sz + spacing;  // Circle
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col);                                    x += sz + spacing;  // Square
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f);                             x += sz + spacing;  // Square with all rounded corners
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col, 10.0f, corners_tl_br);              x += sz + spacing;  // Square with two rounded corners
            draw_list->AddTriangleFilled(ImVec2(x+sz*0.5f,y), ImVec2(x+sz, y+sz-0.5f), ImVec2(x, y+sz-0.5f), col);  x += sz + spacing;  // Triangle
            //draw_list->AddTriangleFilled(ImVec2(x+sz*0.2f,y), ImVec2(x, y+sz-0.5f), ImVec2(x+sz*0.4f, y+sz-0.5f), col); x += sz*0.4f + spacing; // Thin triangle
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + thickness), col);                             x += sz + spacing;  // Horizontal line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y + sz), col);                             x += spacing * 2.0f;// Vertical line (faster than AddLine, but only handle integer thickness)
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col);                                      x += sz;            // Pixel (faster than AddLine)
            draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));

            RainGui::Dummy(ImVec2((sz + spacing) * 10.2f, (sz + spacing) * 3.0f));
            RainGui::PopItemWidth();
            RainGui::EndTabItem();
        }

        if (RainGui::BeginTabItem("Canvas"))
        {
            static ImVector<ImVec2> points;
            static ImVec2 scrolling(0.0f, 0.0f);
            static bool opt_enable_grid = true;
            static bool opt_enable_context_menu = true;
            static bool adding_line = false;

            RainGui::Checkbox("Enable grid", &opt_enable_grid);
            RainGui::Checkbox("Enable context menu", &opt_enable_context_menu);
            RainGui::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, click for context menu.");

            // Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping region + own scrolling.
            // Here we demonstrate that this can be replaced by simple offsetting + custom drawing + PushClipRect/PopClipRect() calls.
            // To use a child window instead we could use, e.g:
            //      RainGui::PushStyleVar(RainGuiStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
            //      RainGui::PushStyleColor(RainGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
            //      RainGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), true, RainGuiWindowFlags_NoMove);
            //      RainGui::PopStyleColor();
            //      RainGui::PopStyleVar();
            //      [...]
            //      RainGui::EndChild();

            // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
            ImVec2 canvas_p0 = RainGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
            ImVec2 canvas_sz = RainGui::GetContentRegionAvail();   // Resize canvas to what's available
            if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
            if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
            ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

            // Draw border and background color
            RainGuiIO& io = RainGui::GetIO();
            ImDrawList* draw_list = RainGui::GetWindowDrawList();
            draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
            draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

            // This will catch our interactions
            RainGui::InvisibleButton("canvas", canvas_sz, RainGuiButtonFlags_MouseButtonLeft | RainGuiButtonFlags_MouseButtonRight);
            const bool is_hovered = RainGui::IsItemHovered(); // Hovered
            const bool is_active = RainGui::IsItemActive();   // Held
            const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
            const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

            // Add first and second point
            if (is_hovered && !adding_line && RainGui::IsMouseClicked(RainGuiMouseButton_Left))
            {
                points.push_back(mouse_pos_in_canvas);
                points.push_back(mouse_pos_in_canvas);
                adding_line = true;
            }
            if (adding_line)
            {
                points.back() = mouse_pos_in_canvas;
                if (!RainGui::IsMouseDown(RainGuiMouseButton_Left))
                    adding_line = false;
            }

            // Pan (we use a zero mouse threshold when there's no context menu)
            // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
            const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
            if (is_active && RainGui::IsMouseDragging(RainGuiMouseButton_Right, mouse_threshold_for_pan))
            {
                scrolling.x += io.MouseDelta.x;
                scrolling.y += io.MouseDelta.y;
            }

            // Context menu (under default mouse threshold)
            ImVec2 drag_delta = RainGui::GetMouseDragDelta(RainGuiMouseButton_Right);
            if (opt_enable_context_menu && RainGui::IsMouseReleased(RainGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
                RainGui::OpenPopupOnItemClick("context");
            if (RainGui::BeginPopup("context"))
            {
                if (adding_line)
                    points.resize(points.size() - 2);
                adding_line = false;
                if (RainGui::MenuItem("Remove one", NULL, false, points.Size > 0)) { points.resize(points.size() - 2); }
                if (RainGui::MenuItem("Remove all", NULL, false, points.Size > 0)) { points.clear(); }
                RainGui::EndPopup();
            }

            // Draw grid + all lines in the canvas
            draw_list->PushClipRect(canvas_p0, canvas_p1, true);
            if (opt_enable_grid)
            {
                const float GRID_STEP = 64.0f;
                for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
                for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
            }
            for (int n = 0; n < points.Size; n += 2)
                draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
            draw_list->PopClipRect();

            RainGui::EndTabItem();
        }

        if (RainGui::BeginTabItem("BG/FG draw lists"))
        {
            static bool draw_bg = true;
            static bool draw_fg = true;
            RainGui::Checkbox("Draw in Background draw list", &draw_bg);
            RainGui::SameLine(); HelpMarker("The Background draw list will be rendered below every Dear RainGui windows.");
            RainGui::Checkbox("Draw in Foreground draw list", &draw_fg);
            RainGui::SameLine(); HelpMarker("The Foreground draw list will be rendered over every Dear RainGui windows.");
            ImVec2 window_pos = RainGui::GetWindowPos();
            ImVec2 window_size = RainGui::GetWindowSize();
            ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
            if (draw_bg)
                RainGui::GetBackgroundDrawList()->AddCircle(window_center, window_size.x * 0.6f, IM_COL32(255, 0, 0, 200), 0, 10 + 4);
            if (draw_fg)
                RainGui::GetForegroundDrawList()->AddCircle(window_center, window_size.y * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);
            RainGui::EndTabItem();
        }

        RainGui::EndTabBar();
    }

    RainGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Documents Handling / ShowExampleAppDocuments()
//-----------------------------------------------------------------------------

// Simplified structure to mimic a Document model
struct MyDocument
{
    const char* Name;       // Document title
    bool        Open;       // Set when open (we keep an array of all available documents to simplify demo code!)
    bool        OpenPrev;   // Copy of Open from last update.
    bool        Dirty;      // Set when the document has been modified
    bool        WantClose;  // Set when the document
    ImVec4      Color;      // An arbitrary variable associated to the document

    MyDocument(const char* name, bool open = true, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
    {
        Name = name;
        Open = OpenPrev = open;
        Dirty = false;
        WantClose = false;
        Color = color;
    }
    void DoOpen()       { Open = true; }
    void DoQueueClose() { WantClose = true; }
    void DoForceClose() { Open = false; Dirty = false; }
    void DoSave()       { Dirty = false; }

    // Display placeholder contents for the Document
    static void DisplayContents(MyDocument* doc)
    {
        RainGui::PushID(doc);
        RainGui::Text("Document \"%s\"", doc->Name);
        RainGui::PushStyleColor(RainGuiCol_Text, doc->Color);
        RainGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
        RainGui::PopStyleColor();
        if (RainGui::Button("Modify", ImVec2(100, 0)))
            doc->Dirty = true;
        RainGui::SameLine();
        if (RainGui::Button("Save", ImVec2(100, 0)))
            doc->DoSave();
        RainGui::ColorEdit3("color", &doc->Color.x);  // Useful to test drag and drop and hold-dragged-to-open-tab behavior.
        RainGui::PopID();
    }

    // Display context menu for the Document
    static void DisplayContextMenu(MyDocument* doc)
    {
        if (!RainGui::BeginPopupContextItem())
            return;

        char buf[256];
        sprintf(buf, "Save %s", doc->Name);
        if (RainGui::MenuItem(buf, "CTRL+S", false, doc->Open))
            doc->DoSave();
        if (RainGui::MenuItem("Close", "CTRL+W", false, doc->Open))
            doc->DoQueueClose();
        RainGui::EndPopup();
    }
};

struct ExampleAppDocuments
{
    ImVector<MyDocument> Documents;

    ExampleAppDocuments()
    {
        Documents.push_back(MyDocument("Lettuce",             true,  ImVec4(0.4f, 0.8f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("Eggplant",            true,  ImVec4(0.8f, 0.5f, 1.0f, 1.0f)));
        Documents.push_back(MyDocument("Carrot",              true,  ImVec4(1.0f, 0.8f, 0.5f, 1.0f)));
        Documents.push_back(MyDocument("Tomato",              false, ImVec4(1.0f, 0.3f, 0.4f, 1.0f)));
        Documents.push_back(MyDocument("A Rather Long Title", false));
        Documents.push_back(MyDocument("Some Document",       false));
    }
};

// [Optional] Notify the system of Tabs/Windows closure that happened outside the regular tab interface.
// If a tab has been closed programmatically (aka closed from another source such as the Checkbox() in the demo,
// as opposed to clicking on the regular tab closing button) and stops being submitted, it will take a frame for
// the tab bar to notice its absence. During this frame there will be a gap in the tab bar, and if the tab that has
// disappeared was the selected one, the tab bar will report no selected tab during the frame. This will effectively
// give the impression of a flicker for one frame.
// We call SetTabItemClosed() to manually notify the Tab Bar or Docking system of removed tabs to avoid this glitch.
// Note that this completely optional, and only affect tab bars with the RainGuiTabBarFlags_Reorderable flag.
static void NotifyOfDocumentsClosedElsewhere(ExampleAppDocuments& app)
{
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (!doc->Open && doc->OpenPrev)
            RainGui::SetTabItemClosed(doc->Name);
        doc->OpenPrev = doc->Open;
    }
}

void ShowExampleAppDocuments(bool* p_open)
{
    static ExampleAppDocuments app;

    // Options
    static bool opt_reorderable = true;
    static RainGuiTabBarFlags opt_fitting_flags = RainGuiTabBarFlags_FittingPolicyDefault_;

    bool window_contents_visible = RainGui::Begin("Example: Documents", p_open, RainGuiWindowFlags_MenuBar);
    if (!window_contents_visible)
    {
        RainGui::End();
        return;
    }

    // Menu
    if (RainGui::BeginMenuBar())
    {
        if (RainGui::BeginMenu("File"))
        {
            int open_count = 0;
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                open_count += app.Documents[doc_n].Open ? 1 : 0;

            if (RainGui::BeginMenu("Open", open_count < app.Documents.Size))
            {
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                {
                    MyDocument* doc = &app.Documents[doc_n];
                    if (!doc->Open)
                        if (RainGui::MenuItem(doc->Name))
                            doc->DoOpen();
                }
                RainGui::EndMenu();
            }
            if (RainGui::MenuItem("Close All Documents", NULL, false, open_count > 0))
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
                    app.Documents[doc_n].DoQueueClose();
            if (RainGui::MenuItem("Exit", "Alt+F4")) {}
            RainGui::EndMenu();
        }
        RainGui::EndMenuBar();
    }

    // [Debug] List documents with one checkbox for each
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
    {
        MyDocument* doc = &app.Documents[doc_n];
        if (doc_n > 0)
            RainGui::SameLine();
        RainGui::PushID(doc);
        if (RainGui::Checkbox(doc->Name, &doc->Open))
            if (!doc->Open)
                doc->DoForceClose();
        RainGui::PopID();
    }

    RainGui::Separator();

    // Submit Tab Bar and Tabs
    {
        RainGuiTabBarFlags tab_bar_flags = (opt_fitting_flags) | (opt_reorderable ? RainGuiTabBarFlags_Reorderable : 0);
        if (RainGui::BeginTabBar("##tabs", tab_bar_flags))
        {
            if (opt_reorderable)
                NotifyOfDocumentsClosedElsewhere(app);

            // [DEBUG] Stress tests
            //if ((RainGui::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g. dragging with this on.
            //if (RainGui::GetIO().KeyCtrl) RainGui::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not very useful as-is anyway..

            // Submit Tabs
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
            {
                MyDocument* doc = &app.Documents[doc_n];
                if (!doc->Open)
                    continue;

                RainGuiTabItemFlags tab_flags = (doc->Dirty ? RainGuiTabItemFlags_UnsavedDocument : 0);
                bool visible = RainGui::BeginTabItem(doc->Name, &doc->Open, tab_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc->Open && doc->Dirty)
                {
                    doc->Open = true;
                    doc->DoQueueClose();
                }

                MyDocument::DisplayContextMenu(doc);
                if (visible)
                {
                    MyDocument::DisplayContents(doc);
                    RainGui::EndTabItem();
                }
            }

            RainGui::EndTabBar();
        }
    }

    // Update closing queue
    static ImVector<MyDocument*> close_queue;
    if (close_queue.empty())
    {
        // Close queue is locked once we started a popup
        for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++)
        {
            MyDocument* doc = &app.Documents[doc_n];
            if (doc->WantClose)
            {
                doc->WantClose = false;
                close_queue.push_back(doc);
            }
        }
    }

    // Display closing confirmation UI
    if (!close_queue.empty())
    {
        int close_queue_unsaved_documents = 0;
        for (int n = 0; n < close_queue.Size; n++)
            if (close_queue[n]->Dirty)
                close_queue_unsaved_documents++;

        if (close_queue_unsaved_documents == 0)
        {
            // Close documents when all are unsaved
            for (int n = 0; n < close_queue.Size; n++)
                close_queue[n]->DoForceClose();
            close_queue.clear();
        }
        else
        {
            if (!RainGui::IsPopupOpen("Save?"))
                RainGui::OpenPopup("Save?");
            if (RainGui::BeginPopupModal("Save?", NULL, RainGuiWindowFlags_AlwaysAutoResize))
            {
                RainGui::Text("Save change to the following items?");
                float item_height = RainGui::GetTextLineHeightWithSpacing();
                if (RainGui::BeginChildFrame(RainGui::GetID("frame"), ImVec2(-FLT_MIN, 6.25f * item_height)))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                        if (close_queue[n]->Dirty)
                            RainGui::Text("%s", close_queue[n]->Name);
                    RainGui::EndChildFrame();
                }

                ImVec2 button_size(RainGui::GetFontSize() * 7.0f, 0.0f);
                if (RainGui::Button("Yes", button_size))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                    {
                        if (close_queue[n]->Dirty)
                            close_queue[n]->DoSave();
                        close_queue[n]->DoForceClose();
                    }
                    close_queue.clear();
                    RainGui::CloseCurrentPopup();
                }
                RainGui::SameLine();
                if (RainGui::Button("No", button_size))
                {
                    for (int n = 0; n < close_queue.Size; n++)
                        close_queue[n]->DoForceClose();
                    close_queue.clear();
                    RainGui::CloseCurrentPopup();
                }
                RainGui::SameLine();
                if (RainGui::Button("Cancel", button_size))
                {
                    close_queue.clear();
                    RainGui::CloseCurrentPopup();
                }
                RainGui::EndPopup();
            }
        }
    }

    RainGui::End();
}

// End of Demo code
#else

void RainGui::ShowAboutWindow(bool*) {}
void RainGui::ShowDemoWindow(bool*) {}
void RainGui::ShowUserGuide() {}
void RainGui::ShowStyleEditor(RainGuiStyle*) {}

#endif

#endif // #ifndef RAINGUI_DISABLE
