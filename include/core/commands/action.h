#ifndef CORE_COMMANDS_ACTION_H
#define CORE_COMMANDS_ACTION_H

#include <windows.h>
#include <FL/Fl_Window.H>
#include "core/extract/execution/thread_hijack.h"

namespace command {

class Action {
private:
    static HANDLE g_current_thread;
    static HANDLE g_target_thread;
    static bool g_gui_initialized;
    static Fl_Window* g_window;
    static HANDLE g_thread_ready_event;

public:
    // Initialize/cleanup
    static bool init_sync();
    static void cleanup_sync();
    static void cleanup_threads();
    static void cleanup_all();

    // Main action functions
    static bool gui_init_trigger();
    static HANDLE apc_init();
    static void on_image_error(DWORD threadId);
    static bool stealth_prepare();
};

} // namespace command

#endif // CORE_COMMANDS_ACTION_H