# Project File Structure

```
win-gui-steg/
├── include/                     # Header files
│   ├── core/                   # Core functionality headers
│   │   ├── aes/               # AES encryption/decryption
│   │   │   └── aes_decryptor.h  # AES decryption implementation
│   │   ├── commands/          # High-level operation headers
│   │   │   └── action.h       # Main action controller
│   │   └── extract/           # Extraction functionality
│   │       ├── execution/     # Process/thread execution headers
│   │       │   ├── apc.h      # APC execution interface
│   │       │   └── executor.h # Base execution interface
│   │       ├── lsb_extract.h  # LSB extraction implementation
│   │       └── mapping.h      # Memory mapping utilities
│   ├── gui/                   # GUI components
│   │   ├── examiner_tab.h    # Image examination tab
│   │   └── main_window.h     # Main application window
│   └── utils/                # Utility functions
│       └── file_logger.h     # File logging utilities
├── src/                      # Source implementations
│   ├── core/                # Core functionality implementations
│   │   ├── aes/            # AES implementations
│   │   │   └── aes_decryptor.cpp
│   │   ├── commands/       # High-level operations
│   │   │   └── action.cpp  # Main action implementation
│   │   └── extract/        # Extraction implementations
│   │       ├── execution/  # Process/thread execution
│   │       │   ├── apc.cpp # APC execution implementation
│   │       │   └── executor.cpp # Execution implementation
│   │       ├── lsb_extract.cpp # LSB extraction implementation
│   │       └── mapping.cpp # Memory mapping implementation
│   ├── gui/               # GUI implementations
│   │   ├── examiner_tab.cpp
│   │   └── main_window.cpp
│   └── utils/            # Utility implementations
│       └── file_logger.cpp
├── tests/               # Test files
├── assets/             # Resource files
│   ├── images/         # Test images
│   └── log/           # Log output
├── docs/              # Documentation
├── CMakeLists.txt    # Main CMake configuration
└── README.md         # Project readme
```

# Function Reference

## Core Functionality

### Action Controller (action.h)
- `command::Action::gui_init_trigger()` - Initialize GUI with trigger
- `command::Action::apc_init()` - Initialize APC operations
- `command::Action::stealth_prepare()` - Prepare stealth operations
- `command::Action::suspend_thread_load_image()` - Suspend and load image
- `command::Action::on_image_error()` - Handle image loading errors

### AES (aes_decryptor.h)
- `aes::AesDecryptor::init_from_mapped_data()` - Initialize with mapped data
- `aes::AesDecryptor::decrypt()` - Decrypt data

### LSB Extraction (lsb_extract.h)
- `extract::LsbExtractor::extract()` - Extract data from image

### Memory Mapping (mapping.h)
- `extract::map_file_readonly()` - Map file to memory read-only

### APC Execution (apc.h)
- `extract::APCExecutor::queue_apc_thread()` - Queue APC thread
- `extract::APCExecutor::resume_apc_thread()` - Resume APC thread

## GUI Components

### Main Window (main_window.h)
- `gui::MainWindow::get_examiner_tab()` - Get examiner tab

### Examiner Tab (examiner_tab.h)
- `gui::ExaminerTab::set_error_handler()` - Set error handler
- `gui::ExaminerTab::load_image()` - Load image for examination

## Utilities

### File Logger (file_logger.h)
- `utils::FileLogger::log()` - Log general message
- `utils::FileLogger::log_apc()` - Log APC-specific message
```
