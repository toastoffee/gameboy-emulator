/**
  ******************************************************************************
  * @file           : instruction.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/16
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_INSTRUCTION_H
#define GAMEBOY_EMULATOR_INSTRUCTION_H

#include "type.h"

class Emulator;

using InstructionFunc = void(Emulator* emu);

extern InstructionFunc* instructionsMap[256];

const c8* GetOpcodeName(u8 opcode);

#endif //GAMEBOY_EMULATOR_INSTRUCTION_H
