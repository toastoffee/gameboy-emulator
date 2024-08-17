/**
  ******************************************************************************
  * @file           : serial.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/17
  ******************************************************************************
  */



#include "serial.h"
#include "emulator.h"

void Serial::BeginTransfer() {
    transferring = true;
    outByte = sb;
    transferBit = 7;
}

void Serial::ProcessTransfer(Emulator *emu) {
    sb <<= 1;
    // Set lowest bit to 1.
    ++sb;
    --transferBit;
    if(transferBit < 0)
    {
        transferBit = 0;
        EndTransfer(emu);
    }
}

void Serial::EndTransfer(Emulator *emu) {
    outputBuffer.push(outByte);
    bitReset(&sc, 7);
    transferring = false;
    emu->intFlags |= INT_SERIAL;
}

void Serial::Tick(Emulator *emu) {
    if(!transferring && TransferEnable() && IsMaster())
    {
        BeginTransfer();
    }
    else if(transferring)
    {
        ProcessTransfer(emu);
    }
}

u8 Serial::BusRead(u16 addr) {
    assert(addr >= 0xFF01 && addr <= 0xFF02 && "serial register address illegal!");
    if(addr == 0xFF01) return sb;
    if(addr == 0xFF02) return sc;
    return 0;
}

void Serial::BusWrite(u16 addr, u8 data) {
    assert(addr >= 0xFF01 && addr <= 0xFF02 && "serial register address illegal!");
    if(addr == 0xFF01)
    {
        sb = data;
        return;
    }
    if(addr == 0xFF02)
    {
        sc = 0x7C | (data & 0x83);
        return;
    }
}
