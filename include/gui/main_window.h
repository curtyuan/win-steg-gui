#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <memory>
#include "gui/image_tab.h"
#include "gui/examiner_tab.h"

namespace gui {

// Forward declarations
class ImageTab;
class ExaminerTab;

class MainWindow : public Fl_Double_Window {
public:
    MainWindow(int w, int h, const char* title);
    ~MainWindow() override = default;

    void resize(int X, int Y, int W, int H) override;

    // Get examiner tab for test setup
    ExaminerTab* get_examiner_tab() { return secondTab.get(); }

private:
    // GUI components
    std::unique_ptr<Fl_Tabs> tabs;
    std::unique_ptr<ImageTab> firstTab;
    std::unique_ptr<ExaminerTab> secondTab;

    // Setup methods
    void createTabs();
    void updateLayout();
};

} // namespace gui

#endif // MAIN_WINDOW_H 