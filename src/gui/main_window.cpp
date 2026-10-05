#include "gui/main_window.h"
#include "gui/image_tab.h"
#include "gui/examiner_tab.h"
#include <FL/Fl.H>

namespace gui {

MainWindow::MainWindow(int w, int h, const char* title) 
    : Fl_Double_Window(w, h, title)
{
    resizable(this);
    size_range(400, 300);
    
    createTabs();
}

void MainWindow::createTabs() {
    tabs = std::make_unique<Fl_Tabs>(0, 0, w(), h());
    tabs->resizable(tabs.get());
    
    // Create first tab for default image
    firstTab = std::make_unique<ImageTab>(0, 25, w(), h()-25, "Image View");
    
    // Create second tab for examiner
    secondTab = std::make_unique<ExaminerTab>(0, 25, w(), h()-25, "2th Examiner");
    
    tabs->end();
}

void MainWindow::resize(int X, int Y, int W, int H) {
    Fl_Double_Window::resize(X, Y, W, H);
    updateLayout();
}

void MainWindow::updateLayout() {
    if (tabs) {
        tabs->resize(0, 0, w(), h());
    }
    redraw();
}

} // namespace gui