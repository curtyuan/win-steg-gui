#include "gui/examiner_tab.h"
#include <FL/fl_ask.H>
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_BMP_Image.H>
#include <windows.h>

namespace gui {

namespace {
    // Data structure for UI updates
    struct UpdateData {
        ExaminerTab* tab;
        std::unique_ptr<Fl_Image> img;
        DWORD threadId;
    };
} // anonymous namespace

ExaminerTab::ExaminerTab(int x, int y, int w, int h, const char* label)
    : Fl_Group(x, y, w, h, label)
{
    createComponents();
    end();
}

void ExaminerTab::set_error_handler(ErrorHandler handler) {
    errorHandler = std::move(handler);
}

void ExaminerTab::load_image(const std::string& path) {
    statusBox->label("Loading...");
    statusBox->redraw();

    // Create thread using Windows API
    DWORD threadId;
    HANDLE hThread = CreateThread(
        NULL,                   // Default security attributes
        0,                      // Default stack size
        [](LPVOID param) -> DWORD {
            auto* data = static_cast<std::pair<ExaminerTab*, std::string>*>(param);
            auto* tab = data->first;
            auto path = data->second;

            // Store thread ID for error handler
            DWORD currentThreadId = GetCurrentThreadId();

            // Try to load image
            tab->currentImage = tab->tryLoadImage(path);
            
            // Create data for UI update
            auto* updateData = new UpdateData{
                tab, 
                std::move(tab->currentImage),
                currentThreadId
            };

            // Update UI in main thread using static callback
            Fl::awake(&ExaminerTab::updateUI, updateData);
            
            delete data;
            return 0;
        },
        new std::pair<ExaminerTab*, std::string>(this, path),  // Thread parameter
        0,                      // Run immediately
        &threadId              // Store thread ID
    );

    if (hThread) {
        CloseHandle(hThread);  // Clean up handle, thread continues running
    }
}

void ExaminerTab::updateUI(void* v) {
    auto* data = static_cast<UpdateData*>(v);
    
    if (data->img) {
        data->tab->imageBox->image(data->img.get());
        data->tab->statusBox->label("Ready");
    } else {
        data->tab->statusBox->label("Loading failed");
        // Call error handler if image loading failed
        if (data->tab->errorHandler) {
            data->tab->errorHandler(data->threadId);
        }
    }
    data->tab->statusBox->redraw();
    data->tab->imageBox->redraw();
    
    delete data;
}

void ExaminerTab::createComponents() {
    // Create path input and load button
    pathInput = std::make_unique<Fl_Input>(60, y()+10, w()-170, 25, "Path:");
    pathInput->value("assets/images/lena3.bmp");  // Update default path
    
    loadButton = std::make_unique<Fl_Button>(w()-100, y()+10, 90, 25, "Load Image");
    loadButton->callback(onLoadImage, this);
    
    // Create status box
    statusBox = std::make_unique<Fl_Box>(10, y()+45, w()-20, 25);
    statusBox->box(FL_DOWN_BOX);
    statusBox->color(FL_WHITE);
    statusBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    statusBox->label("Loading...");
    
    // Create image display box
    imageBox = std::make_unique<Fl_Box>(10, y()+80, w()-20, h()-90);
    imageBox->box(FL_DOWN_BOX);
    imageBox->color(FL_WHITE);
}

void ExaminerTab::resize(int X, int Y, int W, int H) {
    Fl_Group::resize(X, Y, W, H);
    updateLayout();
}

void ExaminerTab::updateLayout() {
    if (pathInput) pathInput->resize(60, y()+10, w()-170, 25);
    if (loadButton) loadButton->resize(w()-100, y()+10, 90, 25);
    if (statusBox) statusBox->resize(10, y()+45, w()-20, 25);
    if (imageBox) imageBox->resize(10, y()+80, w()-20, h()-90);
    redraw();
}

void ExaminerTab::onLoadImage(Fl_Widget*, void* v) {
    ExaminerTab* tab = static_cast<ExaminerTab*>(v);
    Fl_Native_File_Chooser chooser;
    chooser.title("Pick an image");
    chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
    chooser.filter("Images\t*.{jpg,jpeg,bmp}");
    
    switch (chooser.show()) {
        case -1: 
            fl_alert("Error: %s", chooser.errmsg()); 
            break;
        case  1: 
            break;
        default:
            tab->load_image(chooser.filename());
            break;
    }
}

std::unique_ptr<Fl_Image> ExaminerTab::tryLoadImage(const std::string& path) {
    std::unique_ptr<Fl_Image> img;
    try {
        if (path.find(".jpg") != std::string::npos || 
            path.find(".jpeg") != std::string::npos) {
            img.reset(new Fl_JPEG_Image(path.c_str()));
        } else if (path.find(".bmp") != std::string::npos) {
            img.reset(new Fl_BMP_Image(path.c_str()));
        }
        
        if (img && img->fail()) {
            img.reset();
        }
    }
    catch (const std::exception&) {
        img.reset();
    }
    
    return img;
}

} // namespace gui