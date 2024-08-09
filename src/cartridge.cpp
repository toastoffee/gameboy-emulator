/**
  ******************************************************************************
  * @file           : cartridge.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/9
  ******************************************************************************
  */



#include "cartridge.h"

CartridgeHeader *GetCartridgeHeader(byte *romData) {
    return (CartridgeHeader*)(romData + 0x0100);
}