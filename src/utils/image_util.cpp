#include "utils/image_util.h"
#include "utils/file_logger.h"

namespace utils {

DWORD loadImage() {
    // Try to open a non-existent image file
    HANDLE hFile = CreateFileA(
        "assets/images/lena9.bmp",  // Non-existent file
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        utils::FileLogger::log("[GUI] Image file not found");
        return 1;  // Return error
    }

    CloseHandle(hFile);
    return 0;
}

} // namespace utils 