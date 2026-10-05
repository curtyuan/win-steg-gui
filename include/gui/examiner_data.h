#ifndef EXAMINER_DATA_H
#define EXAMINER_DATA_H

#include <Windows.h>
#include <FL/Fl_Image.H>
#include <memory>

namespace gui {

class ExaminerTab;  // Forward declaration

// Data structure for UI updates
struct ExaminerUpdateData {
    ExaminerTab* tab;
    std::unique_ptr<Fl_Image> img;
    DWORD threadId;
};

} // namespace gui

#endif // EXAMINER_DATA_H 