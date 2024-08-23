/**
  ******************************************************************************
  * @file           : imgui_pixel_renderer.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/22
  ******************************************************************************
  */



#ifndef INC_2D_PIXEL_RENDERER_IMGUI_PIXEL_RENDERER_H
#define INC_2D_PIXEL_RENDERER_IMGUI_PIXEL_RENDERER_H

#include "imgui.h"

enum class ColorMode : unsigned int {
    NONE    = 0,
    RGB     = 1,
    RGBA    = 2
};

class ImGuiPixelRenderer {
private:
    unsigned int    _tex;
    int             _width, _height;

public:
    ImGuiPixelRenderer();

    void GeneTex(const unsigned char *data, int width, int height, ColorMode mode);

    void GeneTex(const char* fileName);

    void Render();

    void Render(float pixelWidth);
};


#endif //INC_2D_PIXEL_RENDERER_IMGUI_PIXEL_RENDERER_H
