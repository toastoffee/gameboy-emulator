/**
  ******************************************************************************
  * @file           : file_helper.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/22
  ******************************************************************************
  */



#include <cstdio>
#include <cstdlib>
#include "file_helper.h"

size_t LoadFile(const char* fileName, void* &data)
{
    FILE* f = fopen(fileName, "rb");
    if (f == NULL)
        return 0;
    fseek(f, 0, SEEK_END);
    auto fileSize = (size_t)ftell(f);
    if (fileSize == -1)
        return 0;
    fseek(f, 0, SEEK_SET);

    data = malloc(fileSize);

    fread(data, 1, fileSize, f);

    return fileSize;
}