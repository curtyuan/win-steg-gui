#ifndef IMAGE_TAB_H
#define IMAGE_TAB_H

#include <Windows.h>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Image.H>
#include <memory>
#include <string>

namespace gui {

class ImageTab : public Fl_Group {
public:
    ImageTab(int x, int y, int w, int h, const char* label = nullptr);
    ~ImageTab() override = default;

    void resize(int x, int y, int w, int h) override;

private:
    // GUI components
    std::unique_ptr<Fl_Button> loadButton;
    std::unique_ptr<Fl_Button> reloadButton;
    std::unique_ptr<Fl_Scroll> scrollArea;
    std::unique_ptr<Fl_Box> imageBox;
    std::unique_ptr<Fl_Image> currentImage;

    // Setup methods
    void createToolbar();
    void setupImageDisplay();
    void loadDefaultImage();
    void resizeImageBox(int imgW, int imgH);
    void updateLayout();

    // Event handlers
    static void onLoadImage(Fl_Widget* w, void* v);
    static void onReloadImage(Fl_Widget* w, void* v);

    // Helper methods
    std::unique_ptr<Fl_Image> loadImage(const std::string& path);
};

} // namespace gui

#endif // IMAGE_TAB_H 