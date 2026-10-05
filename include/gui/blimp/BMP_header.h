/**
 * @file BMP_header.h
 * @brief Defines the BMP file format header structure
 * @copyright Copyright 2020, Sage Farrenholz
 *
 * This file is separated from the main BMP class implementation to:
 * 1. Maintain a clear separation between file format specification and manipulation logic
 * 2. Make it easier to update the header structure if the BMP format changes
 * 3. Allow potential reuse of the header structure in other contexts without pulling in the entire BMP class
 * 4. Follow the single responsibility principle - this file only defines what a BMP file looks like
 *
 * The BMP file format consists of:
 * - File Header: Basic file information (signature, size, data offset)
 * - DIB Header: Bitmap specific information (dimensions, color depth, compression)
 * - Optional Color Palette: For indexed color modes
 * - Pixel Data: The actual image data
 */

#ifndef GUI_BLIMP_BMP_HEADER_H_
#define GUI_BLIMP_BMP_HEADER_H_

#include <cstdint>

namespace gui {

#pragma pack(push, 1)

// BMP file header structure
struct BMP_Header {

    //File Header Structure
    char id[2] = {'B', 'M'};
    uint32_t size;
    uint32_t reserved = 0;
    uint32_t offset;

    //DIB Header Structure
    uint32_t header_size = 40;
    int32_t width;
    int32_t height;
    uint16_t color_planes = 1;
    uint16_t bit_depth;
    uint32_t compress = 0;
    uint32_t image_size = 0;
    int32_t x_res = 2835; //72dpi ~= 2835 ppm
    int32_t y_res = 2835; //72dpi ~= 2835 ppm
    uint32_t color_count = 0;
    uint32_t important_colors = 0;

};

// DIB header structure (Windows BITMAPINFOHEADER)
struct DIB_Header {
    uint32_t header_size = 40;
    int32_t width;
    int32_t height;
    uint16_t color_planes = 1;
    uint16_t bit_depth;
    uint32_t compress = 0;
    uint32_t image_size = 0;
    int32_t x_res = 2835; //72dpi ~= 2835 ppm
    int32_t y_res = 2835; //72dpi ~= 2835 ppm
    uint32_t color_count = 0;
    uint32_t important_colors = 0;
};

#pragma pack(pop)

} // namespace gui

#endif // GUI_BLIMP_BMP_HEADER_H_