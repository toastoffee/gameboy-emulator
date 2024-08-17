/**
  ******************************************************************************
  * @file           : serial.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/17
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_SERIAL_H
#define GAMEBOY_EMULATOR_SERIAL_H

#include "type.h"
#include "bit_oper.h"

#include <queue>

class Emulator;

class Serial {
    //! 0xFF01 Serial transfer data.
    u8 sb;

    //! 0xFF02 Serial transfer control.
    u8 sc;

    // Serial internal data.

    // whether data transferring is in progress.
    bool transferring;

    // Game Boy Serial output will be placed in this buffer.
    std::queue<u8> outputBuffer;

    // Current transfer out Byte;
    u8 outByte;

    // Transferring bit index (0~7)
    i8 transferBit;

    bool IsMaster() const { return bitTest(&sc, 0); }
    bool TransferEnable() const { return bitTest(&sc, 7); }

    void BeginTransfer();
    void ProcessTransfer(Emulator* emu);
    void EndTransfer(Emulator* emu);

    void Init() {
        sb = 0xFF;
        sc = 0x7C;
        transferring = false;
    }

    void Tick(Emulator* emu);
    u8 BusRead(u16 addr);
    void BusWrite(u16 addr, u8 data);
};


#endif //GAMEBOY_EMULATOR_SERIAL_H
