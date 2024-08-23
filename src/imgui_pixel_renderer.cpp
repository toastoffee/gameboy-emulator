/**
  ******************************************************************************
  * @file           : imgui_pixel_renderer.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/22
  ******************************************************************************
  */


#include "glad/glad.h"
#include "imgui_pixel_renderer.h"
#include "stbi/stb_image.h"
#include "file_helper.h"

ImGuiPixelRenderer::ImGuiPixelRenderer() {
    _tex    = 0;
    _width  = 0;
    _height = 0;
}

bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

void ImGuiPixelRenderer::GeneTex(const unsigned char *data, int width, int height, ColorMode mode) {
    assert(mode != ColorMode::NONE && "color mode is none!");

    _width = width;
    _height = height;

    char ppmHeader[100] = {0};
    snprintf(ppmHeader, 100, "P6 %d %d 255 ", width, height);
    int headerSize = (int) strlen(ppmHeader);
    int texDataSize = headerSize + width * height * 3;

    unsigned char *texData;
    texData = (unsigned char *) malloc(texDataSize);

    // 1. write in the ppm header
    memcpy(texData, ppmHeader, strlen(ppmHeader));

    int pixelSize = 0;
    switch (mode) {
        case ColorMode::RGB:
            pixelSize = 3;
            break;
        case ColorMode::RGBA:
            pixelSize = 4;
            break;
        default:
            return;
    }

    // 2. write in the ppm pixel colors
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int idx = i * width + j;
            memcpy(texData + headerSize + idx * 3, data + idx * pixelSize, 3);
        }
    }

    LoadTextureFromMemory((const void*)texData, texDataSize, &_tex, &_width, &_height);

    free(texData);
}

void ImGuiPixelRenderer::GeneTex(const char *fileName) {
    void *data = nullptr;
    size_t fileSize = LoadFile(fileName, data);

    LoadTextureFromMemory(data, fileSize, &_tex, &_width, &_height);

    free(data);
}

void ImGuiPixelRenderer::Render() {
    ImGui::Image((void*)(intptr_t)_tex, ImVec2(_width * 2, _height * 2));
}

