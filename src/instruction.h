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

class Emulator;

using InstructionFunc = void(Emulator* emu);

extern InstructionFunc* instructionsMap[256];

#endif //GAMEBOY_EMULATOR_INSTRUCTION_H
