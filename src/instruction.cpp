/**
  ******************************************************************************
  * @file           : instruction.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/16
  ******************************************************************************
  */



#include "instruction.h"
#include "emulator.h"

// combines one 16-bit value from two 8-bit values.
inline constexpr u16 MakeU16(u8 low, u8 high) {
    return (u16)low | ((u16)high << 8);
}

// reads 16-bit immediate data.
// cpu of Game Boy is little endian (low Byte data at low, high Byte data at high)
inline u16 ReadU16(Emulator* emu) {
    u16 r = MakeU16(emu->BusRead(emu->cpu.pc), emu->BusRead(emu->cpu.pc+1));
    emu->cpu.pc += 2;
    return r;
}

// reads 8-bit immediate data.
inline u8 ReadU8(Emulator* emu) {
    u8 r = emu->BusRead(emu->cpu.pc);
    ++emu->cpu.pc;
    return r;
}

// NOP : Do nothing
void x00_nop(Emulator *emu) {
    // do nothing
    emu->Tick(1);
}

// LD instructions (0x40~0x7F)



//! LD B,B : load B to B
void x40_ld_b_b(Emulator * emu) {
    emu->cpu.b = emu->cpu.b;
    emu->Tick(1);
}

//! LD B,C : load C to B
void x41_ld_b_c(Emulator * emu) {
    emu->cpu.b = emu->cpu.c;
    emu->Tick(1);
}

//! LD B,D : load D to B
void x42_ld_b_d(Emulator * emu) {
    emu->cpu.b = emu->cpu.d;
    emu->Tick(1);
}

//! LD B,E : load E to B
void x43_ld_b_e(Emulator * emu) {
    emu->cpu.b = emu->cpu.e;
    emu->Tick(1);
}

//! LD B,H : load H to B
void x44_ld_b_h(Emulator * emu) {
    emu->cpu.b = emu->cpu.h;
    emu->Tick(1);
}

//! LD B,L : load L to B
void x45_ld_b_l(Emulator * emu) {
    emu->cpu.b = emu->cpu.l;
    emu->Tick(1);
}

//! LD B,A : load A to B
void x47_ld_b_a(Emulator * emu) {
    emu->cpu.b = emu->cpu.a;
    emu->Tick(1);
}

//! LD C, B : Loads B to C.
void x48_ld_c_b(Emulator* emu)
{
    emu->cpu.c = emu->cpu.b;
    emu->Tick(1);
}
//! LD C, C : Loads C to C.
void x49_ld_c_c(Emulator* emu)
{
    emu->cpu.c = emu->cpu.c;
    emu->Tick(1);
}
//! LD C, D : Loads D to C.
void x4a_ld_c_d(Emulator* emu)
{
    emu->cpu.c = emu->cpu.d;
    emu->Tick(1);
}
//! LD C, E : Loads E to C.
void x4b_ld_c_e(Emulator* emu)
{
    emu->cpu.c = emu->cpu.e;
    emu->Tick(1);
}
//! LD C, H : Loads H to C.
void x4c_ld_c_h(Emulator* emu)
{
    emu->cpu.c = emu->cpu.h;
    emu->Tick(1);
}
//! LD C, L : Loads L to C.
void x4d_ld_c_l(Emulator* emu)
{
    emu->cpu.c = emu->cpu.l;
    emu->Tick(1);
}
//! LD C, (HL) : Loads 8-bit data pointed by HL to C.
void x4e_ld_c_mhl(Emulator* emu)
{
    emu->cpu.c = emu->BusRead(emu->cpu.hl());
    emu->Tick(2);
}
//! LD C, A : Loads A to C.
void x4f_ld_c_a(Emulator* emu)
{
    emu->cpu.c = emu->cpu.a;
    emu->Tick(1);
}
//! LD D, B : Loads B to D.
void x50_ld_d_b(Emulator* emu)
{
    emu->cpu.d = emu->cpu.b;
    emu->Tick(1);
}
//! LD D, C : Loads C to D.
void x51_ld_d_c(Emulator* emu)
{
    emu->cpu.d = emu->cpu.c;
    emu->Tick(1);
}
//! LD D, D : Loads D to D.
void x52_ld_d_d(Emulator* emu)
{
    emu->cpu.d = emu->cpu.d;
    emu->Tick(1);
}
//! LD D, E : Loads E to D.
void x53_ld_d_e(Emulator* emu)
{
    emu->cpu.d = emu->cpu.e;
    emu->Tick(1);
}
//! LD D, H : Loads H to D.
void x54_ld_d_h(Emulator* emu)
{
    emu->cpu.d = emu->cpu.h;
    emu->Tick(1);
}
//! LD D, L : Loads L to D.
void x55_ld_d_l(Emulator* emu)
{
    emu->cpu.d = emu->cpu.l;
    emu->Tick(1);
}
//! LD D, (HL) : Loads 8-bit data pointed by HL to D.
void x56_ld_d_mhl(Emulator* emu)
{
    emu->cpu.d = emu->BusRead(emu->cpu.hl());
    emu->Tick(2);
}
//! LD D, A : Loads A to D.
void x57_ld_d_a(Emulator* emu)
{
    emu->cpu.d = emu->cpu.a;
    emu->Tick(1);
}
//! LD E, B : Loads B to E.
void x58_ld_e_b(Emulator* emu)
{
    emu->cpu.e = emu->cpu.b;
    emu->Tick(1);
}
//! LD E, C : Loads C to E.
void x59_ld_e_c(Emulator* emu)
{
    emu->cpu.e = emu->cpu.c;
    emu->Tick(1);
}
//! LD E, D : Loads D to E.
void x5a_ld_e_d(Emulator* emu)
{
    emu->cpu.e = emu->cpu.d;
    emu->Tick(1);
}
//! LD E, E : Loads E to E.
void x5b_ld_e_e(Emulator* emu)
{
    emu->cpu.e = emu->cpu.e;
    emu->Tick(1);
}
//! LD E, H : Loads H to E.
void x5c_ld_e_h(Emulator* emu)
{
    emu->cpu.e = emu->cpu.h;
    emu->Tick(1);
}
//! LD E, L : Loads L to E.
void x5d_ld_e_l(Emulator* emu)
{
    emu->cpu.e = emu->cpu.l;
    emu->Tick(1);
}
//! LD E, (HL) : Loads 8-bit data pointed by HL to E.
void x5e_ld_e_mhl(Emulator* emu)
{
    emu->cpu.e = emu->BusRead(emu->cpu.hl());
    emu->Tick(2);
}
//! LD E, A : Loads A to E.
void x5f_ld_e_a(Emulator* emu)
{
    emu->cpu.e = emu->cpu.a;
    emu->Tick(1);
}
//! LD H, B : Loads B to H.
void x60_ld_h_b(Emulator* emu)
{
    emu->cpu.h = emu->cpu.b;
    emu->Tick(1);
}
//! LD H, C : Loads C to H.
void x61_ld_h_c(Emulator* emu)
{
    emu->cpu.h = emu->cpu.c;
    emu->Tick(1);
}
//! LD H, D : Loads D to H.
void x62_ld_h_d(Emulator* emu)
{
    emu->cpu.h = emu->cpu.d;
    emu->Tick(1);
}
//! LD H, E : Loads E to H.
void x63_ld_h_e(Emulator* emu)
{
    emu->cpu.h = emu->cpu.e;
    emu->Tick(1);
}
//! LD H, H : Loads H to H.
void x64_ld_h_h(Emulator* emu)
{
    emu->cpu.h = emu->cpu.h;
    emu->Tick(1);
}
//! LD H, L : Loads L to H.
void x65_ld_h_l(Emulator* emu)
{
    emu->cpu.h = emu->cpu.l;
    emu->Tick(1);
}
//! LD H, (HL) : Loads 8-bit data pointed by HL to H.
void x66_ld_h_mhl(Emulator* emu)
{
    emu->cpu.h = emu->BusRead(emu->cpu.hl());
    emu->Tick(2);
}
//! LD H, A : Loads A to H.
void x67_ld_h_a(Emulator* emu)
{
    emu->cpu.h = emu->cpu.a;
    emu->Tick(1);
}
//! LD L, B : Loads B to L.
void x68_ld_l_b(Emulator* emu)
{
    emu->cpu.l = emu->cpu.b;
    emu->Tick(1);
}
//! LD L, C : Loads C to L.
void x69_ld_l_c(Emulator* emu)
{
    emu->cpu.l = emu->cpu.c;
    emu->Tick(1);
}
//! LD L, D : Loads D to L.
void x6a_ld_l_d(Emulator* emu)
{
    emu->cpu.l = emu->cpu.d;
    emu->Tick(1);
}
//! LD L, E : Loads E to L.
void x6b_ld_l_e(Emulator* emu)
{
    emu->cpu.l = emu->cpu.e;
    emu->Tick(1);
}
//! LD L, H : Loads H to L.
void x6c_ld_l_h(Emulator* emu)
{
    emu->cpu.l = emu->cpu.h;
    emu->Tick(1);
}
//! LD L, L : Loads L to L.
void x6d_ld_l_l(Emulator* emu)
{
    emu->cpu.l = emu->cpu.l;
    emu->Tick(1);
}
//! LD L, (HL) : Loads 8-bit data pointed by HL to L.
void x6e_ld_l_mhl(Emulator* emu)
{
    emu->cpu.l = emu->BusRead(emu->cpu.hl());
    emu->Tick(2);
}
//! LD L, A : Loads A to L.
void x6f_ld_l_a(Emulator* emu)
{
    emu->cpu.l = emu->cpu.a;
    emu->Tick(1);
}
//! LD (HL), B : Stores B to the memory pointed by HL.
void x70_ld_mhl_b(Emulator* emu)
{
    emu->BusWrite(emu->cpu.hl(), emu->cpu.b);
    emu->Tick(2);
}
//! LD (HL), C : Stores C to the memory pointed by HL.
void x71_ld_mhl_c(Emulator* emu)
{
    emu->BusWrite(emu->cpu.hl(), emu->cpu.c);
    emu->Tick(2);
}
//! LD (HL), D : Stores D to the memory pointed by HL.
void x72_ld_mhl_d(Emulator* emu)
{
    emu->BusWrite(emu->cpu.hl(), emu->cpu.d);
    emu->Tick(2);
}
//! LD (HL), E : Stores E to the memory pointed by HL.
void x73_ld_mhl_e(Emulator* emu)
{
    emu->BusWrite(emu->cpu.hl(), emu->cpu.e);
    emu->Tick(2);
}
//! LD (HL), H : Stores H to the memory pointed by HL.
void x74_ld_mhl_h(Emulator* emu)
{
    emu->BusWrite(emu->cpu.hl(), emu->cpu.h);
    emu->Tick(2);
}
//! LD (HL), L : Stores L to the memory pointed by HL.
void x75_ld_mhl_l(Emulator* emu)
{
    emu->BusWrite(emu->cpu.hl(), emu->cpu.l);
    emu->Tick(2);
}
//! LD (HL), A : Stores A to the memory pointed by HL.
void x77_ld_mhl_a(Emulator* emu)
{
    emu->BusWrite(emu->cpu.hl(), emu->cpu.a);
    emu->Tick(2);
}
//! LD A, B : Loads B to A.
void x78_ld_a_b(Emulator* emu)
{
    emu->cpu.a = emu->cpu.b;
    emu->Tick(1);
}
//! LD A, C : Loads C to A.
void x79_ld_a_c(Emulator* emu)
{
    emu->cpu.a = emu->cpu.c;
    emu->Tick(1);
}
//! LD A, D : Loads D to A.
void x7a_ld_a_d(Emulator* emu)
{
    emu->cpu.a = emu->cpu.d;
    emu->Tick(1);
}
//! LD A, E : Loads E to A.
void x7b_ld_a_e(Emulator* emu)
{
    emu->cpu.a = emu->cpu.e;
    emu->Tick(1);
}
//! LD A, H : Loads H to A.
void x7c_ld_a_h(Emulator* emu)
{
    emu->cpu.a = emu->cpu.h;
    emu->Tick(1);
}
//! LD A, L : Loads L to A.
void x7d_ld_a_l(Emulator* emu)
{
    emu->cpu.a = emu->cpu.l;
    emu->Tick(1);
}
//! LD A, (HL) : Loads 8-bit data pointed by HL to A.
void x7e_ld_a_mhl(Emulator* emu)
{
    emu->cpu.a = emu->BusRead(emu->cpu.hl());
    emu->Tick(2);
}
//! LD A, A : Loads A to A.
void x7f_ld_a_a(Emulator* emu)
{
    emu->cpu.a = emu->cpu.a;
    emu->Tick(1);
}



InstructionFunc* instructionsMap[256] = {
        x00_nop, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};