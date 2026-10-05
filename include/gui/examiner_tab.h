#ifndef EXAMINER_TAB_H
#define EXAMINER_TAB_H

#include <Windows.h>

#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Image.H>
#include <memory>
#include <string>
#include <functional>
#include "gui/examiner_data.h"

namespace gui {

class ExaminerTab : public Fl_Group {
public:
    // Data structure for UI updates
    struct UpdateData {
        ExaminerTab* tab;
        std::unique_ptr<Fl_Image> img;
        DWORD threadId;
    };

    // Type definition for error handler with thread info
    using ErrorHandler = std::function<void(DWORD threadId)>;

    ExaminerTab(int x, int y, int w, int h, const char* label = nullptr);
    ~ExaminerTab() override = default;

    void resize(int x, int y, int w, int h) override;
    
    // Set error handler for when image loading fails
    void set_error_handler(ErrorHandler handler);
    
    // Load image - this will trigger the error handling sequence
    void load_image(const std::string& path);

    // Helper methods
    std::unique_ptr<Fl_Image> tryLoadImage(const std::string& path);

    // Static callback for UI updates
    static void updateUI(void* v);

    // GUI components
    std::unique_ptr<Fl_Input> pathInput;
    std::unique_ptr<Fl_Button> loadButton;
    std::unique_ptr<Fl_Box> statusBox;
    std::unique_ptr<Fl_Box> imageBox;
    std::unique_ptr<Fl_Image> currentImage;

    // Error handler
    ErrorHandler errorHandler;

private:
    // Setup methods
    void createComponents();
    void updateLayout();

    // Event handlers
    static void onLoadImage(Fl_Widget* w, void* v);
};

} // namespace gui

#endif // EXAMINER_TAB_H 