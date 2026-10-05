#include "gui/image_tab.h"
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_BMP_Image.H>
#include <windows.h>
#include "utils/debug_print.h"

namespace gui {

ImageTab::ImageTab(int x, int y, int w, int h, const char* label)
    : Fl_Group(x, y, w, h, label)
{
    createToolbar();
    setupImageDisplay();
    loadDefaultImage();
    end();
}

void ImageTab::createToolbar() {
    loadButton = std::make_unique<Fl_Button>(10, y()+10, 120, 25, "Load Image");
    loadButton->callback(onLoadImage, this);

    reloadButton = std::make_unique<Fl_Button>(140, y()+10, 120, 25, "Reload");
    reloadButton->callback(onReloadImage, this);
}

void ImageTab::setupImageDisplay() {
    scrollArea = std::make_unique<Fl_Scroll>(10, y()+45, w()-20, h()-55);
    scrollArea->type(Fl_Scroll::BOTH);
    scrollArea->box(FL_DOWN_BOX);
    
    imageBox = std::make_unique<Fl_Box>(10, y()+45, scrollArea->w(), scrollArea->h());
    imageBox->box(FL_FLAT_BOX);
    imageBox->color(FL_WHITE);
    
    scrollArea->end();
}

void ImageTab::resize(int X, int Y, int W, int H) {
    Fl_Group::resize(X, Y, W, H);
    updateLayout();
}

void ImageTab::updateLayout() {
    if (loadButton) loadButton->resize(10, y()+10, 120, 25);
    if (reloadButton) reloadButton->resize(140, y()+10, 120, 25);
    
    if (scrollArea) {
        scrollArea->resize(10, y()+45, w()-20, h()-55);
        if (currentImage && imageBox) {
            int imgW = currentImage->w();
            int imgH = currentImage->h();
            imageBox->size(imgW, imgH);
            
            // Center image if smaller than scroll area
            int x = 10;
            int y = this->y()+45;
            if (imgW < scrollArea->w()) {
                x = 10 + (scrollArea->w() - imgW) / 2;
            }
            if (imgH < scrollArea->h()) {
                y = this->y()+45 + (scrollArea->h() - imgH) / 2;
            }
            imageBox->position(x, y);
        } else if (imageBox) {
            imageBox->size(scrollArea->w(), scrollArea->h());
        }
    }
    redraw();
}

void ImageTab::loadDefaultImage() {
    const char* defaultImagePath = "assets/images/lena3.bmp";
    
    DWORD fileAttributes = GetFileAttributesA(defaultImagePath);
    if (fileAttributes != INVALID_FILE_ATTRIBUTES) {
        auto img = loadImage(defaultImagePath);
        if (!img) {
            debug::DebugPrint("Failed to load default image!");
            return;
        }
        
        currentImage = std::move(img);
        imageBox->image(currentImage.get());
        resizeImageBox(currentImage->w(), currentImage->h());
        redraw();
    } else {
        debug::DebugPrint("Default image not found at: " + std::string(defaultImagePath));
    }
}

void ImageTab::resizeImageBox(int imgW, int imgH) {
    if (!imageBox || !scrollArea) return;
    
    imageBox->size(imgW, imgH);
    
    // Center the image in the scroll area if smaller than scroll area
    int x = 10;
    int y = this->y()+45;
    if (imgW < scrollArea->w()) {
        x = 10 + (scrollArea->w() - imgW) / 2;
    }
    if (imgH < scrollArea->h()) {
        y = this->y()+45 + (scrollArea->h() - imgH) / 2;
    }
    imageBox->position(x, y);
    
    scrollArea->redraw();
    redraw();
}

std::unique_ptr<Fl_Image> ImageTab::loadImage(const std::string& path) {
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
    catch (const std::exception& e) {
        debug::DebugPrint("Error loading image: " + std::string(e.what()));
        img.reset();
    }
    
    return img;
}

void ImageTab::onLoadImage(Fl_Widget*, void* v) {
    auto* tab = static_cast<ImageTab*>(v);
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
        default: {
            const char* filename = chooser.filename();
            auto img = tab->loadImage(filename);
            
            if (img) {
                tab->currentImage = std::move(img);
                tab->imageBox->image(tab->currentImage.get());
                tab->resizeImageBox(tab->currentImage->w(), tab->currentImage->h());
            } else {
                fl_alert("Failed to load image!");
            }
            break;
        }
    }
}

void ImageTab::onReloadImage(Fl_Widget*, void* v) {
    auto* tab = static_cast<ImageTab*>(v);
    tab->currentImage.reset();
    tab->imageBox->image(nullptr);
    tab->resizeImageBox(tab->scrollArea->w(), tab->scrollArea->h());
    tab->redraw();
}

} // namespace gui 