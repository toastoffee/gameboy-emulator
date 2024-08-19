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
#include "bit_oper.h"
#include "log-min.h"

// combines one 16-bit value from two 8-bit values.
inline constexpr u16 MakeU16(u8 low, u8 high) {
    return (u16)low | ((u16)high << 8);
}

// reads 16-bit immediate data.
// cpu of Game Boy is little endian (low Byte data at low, high Byte data at high)
inline u16 ReadU16(Emulator* emu) {
    u16 r = MakeU16(emu->BusRead(emu->cpu.pc), emu->BusRead(emu->cpu.pc + 1));
    emu->cpu.pc += 2;
    return r;
}

// reads 8-bit immediate data.
inline u8 ReadU8(Emulator* emu) {
    u8 r = emu->BusRead(emu->cpu.pc);
    ++emu->cpu.pc;
    return r;
}

// sets the zero flag if 'v' is 0, otherwise resets the zero flag.
inline void SetZeroFlag(Emulator* emu, u8 v) {
    if(v) {
        emu->cpu.reset_fz();
    }
    else {
        emu->cpu.set_fz();
    }
}

// compares v1 with v2, and sets flags based on results.
inline void Cmp8(Emulator* emu, u16 v1, u16 v2) {
    u8 r = (u8)v1 - (u8)v2;
    SetZeroFlag(emu, r);
    emu->cpu.set_fn();

    if((v1 & 0x0F) < (v2 & 0x0F)) {
        emu->cpu.set_fh();
    } else {
        emu->cpu.reset_fh();
    }

    if(v1 < v2) {
        emu->cpu.set_fc();
    } else {
        emu->cpu.reset_fc();
    }
}

// push 16-bit data into stack
// Game Boy stack from low to high
inline void Push16(Emulator* emu, u16 v) {
    emu->cpu.sp -= 2;
    emu->BusWrite(emu->cpu.sp + 1, (u8)((v >> 8) & 0xFF));
    emu->BusWrite(emu->cpu.sp, (u8)(v & 0xFF));
}

// pops 16-bit data from stack
inline u16 Pop16(Emulator* emu) {
    u8 lo = emu->BusRead(emu->cpu.sp);
    u8 hi = emu->BusRead(emu->cpu.sp + 1);
    emu->cpu.sp += 2;
    return MakeU16(lo, hi);
}

// 8-bit increase
// if overflowed, then set Z to 1, else 0
// set N to 0
// if carry happends at 3-bit to 4-bit, then set H to 1, else 0
inline void Inc8(Emulator* emu, u8& v) {
    ++v;
    SetZeroFlag(emu, v);
    emu->cpu.reset_fn();
    if((v & 0x0F) == 0x00) {
        emu->cpu.set_fh();
    }
    else {
        emu->cpu.reset_fh();
    }
}

// 8-bit increase
// if overflowed, then set Z to 1, else 0
// set N to 1
// if carry happens at 4-bit to 3-bit, then set H to 1, else 0
inline void Dec8(Emulator* emu, u8& v) {
    --v;
    SetZeroFlag(emu, v);
    emu->cpu.set_fn();
    if((v & 0x0F) == 0x0F) {
        emu->cpu.set_fh();
    }
    else {
        emu->cpu.reset_fh();
    }
}

// 8-bit add
// if overflowed, then set Z to 1, else 0
// set N to 0
// if carry happens at 3-bit to 4-bit, then set H to 1, else 0
// if carry happens at 7-bit to 8-bit, then set C to 1, else 0
inline u8 Add8(Emulator* emu, u16 v1, u16 v2) {
    u8 r = (u8)(v1 + v2);
    SetZeroFlag(emu, r);
    emu->cpu.reset_fn();
    if((v1 & 0x0F) + (v2 & 0x0F) > 0x0F) {
        emu->cpu.set_fh();
    }
    else {
        emu->cpu.reset_fh();
    }

    if(v1 + v2 > 0xFF) {
        emu->cpu.set_fc();
    }
    else {
        emu->cpu.reset_fc();
    }
    return r;
}

// 16-bit add
// set N to 0
// if carry happens at 11-bit to 12-bit, then set H to 1, else 0
// if carry happens at 15-bit to 16-bit, then set C to 1, else 0
// don't modify Z
inline u16 Add16(Emulator* emu, u32 v1, u32 v2) {
    emu->cpu.reset_fn();
    if((v1 & 0xFFF) + (v2 & 0xFFF) > 0xFFF) {
        emu->cpu.set_fh();
    }
    else {
        emu->cpu.reset_fh();
    }

    if(v1 + v2 > 0xFFFF) {
        emu->cpu.set_fc();
    }
    else {
        emu->cpu.reset_fc();
    }

    return (u16)(v1 + v2);
}

// 8-bit ADC (add with carry)
inline u8 Adc8(Emulator* emu, u16 v1, u16 v2) {
    u16 c = emu->cpu.fc() ? 1 : 0;
    u8 r = (u8)(v1 + v2 + c);
    SetZeroFlag(emu, r);
    emu->cpu.reset_fn();
    if(((v1 & 0x0F) + (v2 & 0x0F) + c) > 0x0F) {
        emu->cpu.set_fh();
    }
    else {
        emu->cpu.reset_fh();
    }

    if((v1 + v2 + c) > 0xFF) {
        emu->cpu.set_fc();
    }
    else {
        emu->cpu.reset_fc();
    }
    return r;
}

// 8-bit Sub
inline u8 Sub8(Emulator* emu, u16 v1, u16 v2) {
    u8 r = (u8)v1 - (u8)v2;
    SetZeroFlag(emu, r);
    emu->cpu.set_fn();
    if((v1 & 0x0F) < (v2 & 0x0F)) {
        emu->cpu.set_fh();
    }
    else {
        emu->cpu.reset_fh();
    }

    if(v1 < v2) {
        emu->cpu.set_fc();
    }
    else {
        emu->cpu.reset_fc();
    }
    return r;
}

// 8-bit SBC(subtract with carry)
inline u8 Sbc8(Emulator* emu, u16 v1, u16 v2) {
    u8 c = emu->cpu.fc() ? 1 : 0;
    u8 r = (u8)v1 - (u8)v2 - c;
    SetZeroFlag(emu, r);
    emu->cpu.set_fn();
    if((v1 & 0x0F) < ((v2 & 0x0F) + c)) {
        emu->cpu.set_fh();
    }
    else {
        emu->cpu.reset_fh();
    }

    if(v1 < (v2 + c)) {
        emu->cpu.set_fc();
    }
    else {
        emu->cpu.reset_fc();
    }
    return r;
}

// 8-bit AND
// set N to 0
// set H to 1
// set C to 0
inline u8 And8(Emulator* emu, u8 v1, u8 v2) {
    u8 r = v1 & v2;
    SetZeroFlag(emu, r);
    emu->cpu.reset_fn();
    emu->cpu.set_fh();
    emu->cpu.reset_fc();
    return r;
}

// 8-bit XOR
inline u8 Xor8(Emulator* emu, u8 v1, u8 v2) {
    u8 r = v1 ^ v2;
    SetZeroFlag(emu, r);
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
    emu->cpu.reset_fc();
    return r;
}

// 8-bit OR
inline u8 Or8(Emulator* emu, u8 v1, u8 v2) {
    u8 r = v1 | v2;
    SetZeroFlag(emu, r);
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
    emu->cpu.reset_fc();
    return r;
}

inline void rlc_8(Emulator* emu, u8& v)
{
    bool carry = (v & 0x80) != 0;
    v <<= 1;
    if(carry)
    {
        v |= 0x01;
        emu->cpu.set_fc();
    }
    else
    {
        emu->cpu.reset_fc();
    }
    SetZeroFlag(emu, v);
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
}
inline void rrc_8(Emulator* emu, u8& v)
{
    bool carry = (v & 0x01) != 0;
    v >>= 1;
    if(carry)
    {
        v |= 0x80;
        emu->cpu.set_fc();
    }
    else
    {
        emu->cpu.reset_fc();
    }
    SetZeroFlag(emu, v);
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
}
inline void rl_8(Emulator* emu, u8& v)
{
    bool carry = (v & 0x80) != 0;
    v <<= 1;
    if(emu->cpu.fc())
    {
        v |= 0x01;
    }
    if(carry)
    {
        emu->cpu.set_fc();
    }
    else
    {
        emu->cpu.reset_fc();
    }
    SetZeroFlag(emu, v);
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
}
inline void rr_8(Emulator* emu, u8& v)
{
    bool carry = (v & 0x01) != 0;
    v >>= 1;
    if(emu->cpu.fc())
    {
        v |= 0x80;
    }
    if(carry)
    {
        emu->cpu.set_fc();
    }
    else
    {
        emu->cpu.reset_fc();
    }
    SetZeroFlag(emu, v);
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
}
inline void sla_8(Emulator* emu, u8& v)
{
    bool carry = (v & 0x80) != 0;
    v <<= 1;
    if(carry)
    {
        emu->cpu.set_fc();
    }
    else
    {
        emu->cpu.reset_fc();
    }
    SetZeroFlag(emu, v);
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
}
inline void sra_8(Emulator* emu, u8& v)
{
    bool carry = (v & 0x01) != 0;
    v = (v & 0x80) | ((v >> 1) & 0x7F);
    if(carry)
    {
        emu->cpu.set_fc();
    }
    else
    {
        emu->cpu.reset_fc();
    }
    SetZeroFlag(emu, v);
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
}
inline void srl_8(Emulator* emu, u8& v)
{
    bool carry = (v & 0x01) != 0;
    v = (v >> 1) & 0x7F;
    if(carry)
    {
        emu->cpu.set_fc();
    }
    else
    {
        emu->cpu.reset_fc();
    }
    SetZeroFlag(emu, v);
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
}
inline void swap_8(Emulator* emu, u8& v)
{
    v = ((v >> 4) & 0x0F) + ((v << 4) & 0xF0);
    SetZeroFlag(emu, v);
    emu->cpu.reset_fc();
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
}
inline void bit_8(Emulator* emu, u8 v, u8 bit)
{
    if(bitTest(&v, bit))
    {
        emu->cpu.reset_fz();
    }
    else
    {
        emu->cpu.set_fz();
    }
    emu->cpu.reset_fn();
    emu->cpu.set_fh();
}
inline void res_8(Emulator* emu, u8& v, u8 bit)
{
    bitReset(&v, bit);
}
inline void set_8(Emulator* emu, u8& v, u8 bit)
{
    bitSet(&v, bit);
}


//! NOP : Do nothing.
void x00_nop(Emulator* emu)
{
    // Do nothing.
    emu->Tick(1);
}
//! LD BC, d16 : Loads 16-bit immediate data to BC.
void x01_ld_bc_d16(Emulator* emu)
{
    emu->cpu.bc(ReadU16(emu));
    emu->Tick(3);
}
//! LD (BC), A : Stores A to the memory pointed by BC.
void x02_ld_mbc_a(Emulator* emu)
{
    emu->BusWrite(emu->cpu.bc(), emu->cpu.a);
    emu->Tick(2);
}
//! INC BC : Increases BC.
void x03_inc_bc(Emulator* emu)
{
    emu->cpu.bc(emu->cpu.bc() + 1);
    emu->Tick(2);
}
//! INC B : Increases B.
void x04_inc_b(Emulator* emu)
{
    Inc8(emu, emu->cpu.b);
    emu->Tick(1);
}
//! DEC B : Decreases B.
void x05_dec_b(Emulator* emu)
{
    Dec8(emu, emu->cpu.b);
    emu->Tick(1);
}
//! LD B, d8 : Loads 8-bit immediate data to B.
void x06_ld_b_d8(Emulator* emu)
{
    emu->cpu.b = ReadU8(emu);
    emu->Tick(2);
}
//! RLCA : Rotates A left.
void x07_rlca(Emulator* emu)
{
    rlc_8(emu, emu->cpu.a);
    emu->cpu.reset_fz();
    emu->Tick(1);
}
//! LD (a16), SP : Stores SP to the specified address.
void x08_ld_a16_sp(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    emu->Tick(2);
    emu->BusWrite(addr, (u8)(emu->cpu.sp & 0xFF));
    emu->Tick(1);
    emu->BusWrite(addr + 1, (u8)(emu->cpu.sp >> 8));
    emu->Tick(2);
}
//! ADD HL, BC : Adds BC to HL.
void x09_add_hl_bc(Emulator* emu)
{
    emu->cpu.hl(Add16(emu, emu->cpu.hl(), emu->cpu.bc()));
    emu->Tick(2);
}
//! LD A (BC) : Loads value from memory pointed by BC to A.
void x0a_ld_a_mbc(Emulator* emu)
{
    emu->cpu.a = emu->BusRead(emu->cpu.bc());
    emu->Tick(2);
}
//! DEC BC : Decreases BC.
void x0b_dec_bc(Emulator* emu)
{
    emu->cpu.bc(emu->cpu.bc() - 1);
    emu->Tick(2);
}
//! INC C : Increases C.
void x0c_inc_c(Emulator* emu)
{
    Inc8(emu, emu->cpu.c);
    emu->Tick(1);
}
//! DEC C : Decreases C.
void x0d_dec_c(Emulator* emu)
{
    Dec8(emu, emu->cpu.c);
    emu->Tick(1);
}
//! LD C, d8 : Loads 8-bit immediate data to C.
void x0e_ld_c_d8(Emulator* emu)
{
    emu->cpu.c = ReadU8(emu);
    emu->Tick(2);
}
//! RRCA : Rotates A right.
void x0f_rrca(Emulator* emu)
{
    rrc_8(emu, emu->cpu.a);
    emu->cpu.reset_fz();
    emu->Tick(1);
}
//! STOP : Stops CPU.
void x10_stop(Emulator* emu)
{
    ReadU8(emu); // always 0x00.
    emu->isPaused = true;
    emu->Tick(1);
}
//! LD DE, d16 : Loads 16-bit immediate data to DE.
void x11_ld_de_d16(Emulator* emu)
{
    emu->cpu.de(ReadU16(emu));
    emu->Tick(3);
}
//! LD (DE), A : Stores A to the memory pointed by DE.
void x12_ld_mde_a(Emulator* emu)
{
    emu->BusWrite(emu->cpu.de(), emu->cpu.a);
    emu->Tick(2);
}
//! INC DE : Increases DE.
void x13_inc_de(Emulator* emu)
{
    emu->cpu.de(emu->cpu.de() + 1);
    emu->Tick(2);
}
//! INC D : Increases D.
void x14_inc_d(Emulator* emu)
{
    Inc8(emu, emu->cpu.d);
    emu->Tick(1);
}
//! DEC D : Decreases D.
void x15_dec_d(Emulator* emu)
{
    Dec8(emu, emu->cpu.d);
    emu->Tick(1);
}
//! LD D, d8 : Loads 8-bit immediate data to D.
void x16_ld_d_d8(Emulator* emu)
{
    emu->cpu.d = ReadU8(emu);
    emu->Tick(2);
}
//! RLA : Rotates A left through carry.
void x17_rla(Emulator* emu)
{
    rl_8(emu, emu->cpu.a);
    emu->cpu.reset_fz();
    emu->Tick(1);
}
//! JR r8 : Jumps to PC + r8.
void x18_jr_r8(Emulator* emu)
{
    i8 offset = (i8)ReadU8(emu);
    emu->cpu.pc += (i16)offset;
    emu->Tick(3);
}
//! ADD HL, DE : Adds DE to HL.
void x19_add_hl_de(Emulator* emu)
{
    emu->cpu.hl(Add16(emu, emu->cpu.hl(), emu->cpu.de()));
    emu->Tick(2);
}
//! LD A (DE) : Loads value from memory pointed by DE to A.
void x1a_ld_a_mde(Emulator* emu)
{
    emu->cpu.a = emu->BusRead(emu->cpu.de());
    emu->Tick(2);
}
//! DEC DE : Decreases DE.
void x1b_dec_de(Emulator* emu)
{
    emu->cpu.de(emu->cpu.de() - 1);
    emu->Tick(2);
}
//! INC E : Increases E.
void x1c_inc_e(Emulator* emu)
{
    Inc8(emu, emu->cpu.e);
    emu->Tick(1);
}
//! DEC E : Decreases E.
void x1d_dec_e(Emulator* emu)
{
    Dec8(emu, emu->cpu.e);
    emu->Tick(1);
}
//! LD E, d8 : Loads 8-bit immediate data to E.
void x1e_ld_e_d8(Emulator* emu)
{
    emu->cpu.e = ReadU8(emu);
    emu->Tick(2);
}
//! RRA : Rotates A right through carry.
void x1f_rra(Emulator* emu)
{
    rr_8(emu, emu->cpu.a);
    emu->cpu.reset_fz();
    emu->Tick(1);
}
//! JR NZ, r8 : Jumps to PC + r8 if Z is 0.
void x20_jr_nz_r8(Emulator* emu)
{
    i8 offset = (i8)ReadU8(emu);
    if(!emu->cpu.fz())
    {
        emu->cpu.pc += (i16)offset;
        emu->Tick(3);
    }
    else
    {
        emu->Tick(2);
    }
}
//! LD HL, d16 : Loads 16-bit immediate data to HL.
void x21_ld_hl_d16(Emulator* emu)
{
    emu->cpu.hl(ReadU16(emu));
    emu->Tick(3);
}
//! LD (HL+), A : Stores A to the memory pointed by HL, then increases HL.
void x22_ldi_mhl_a(Emulator* emu)
{
    emu->BusWrite(emu->cpu.hl(), emu->cpu.a);
    emu->cpu.hl(emu->cpu.hl() + 1);
    emu->Tick(2);
}
//! INC HL : Increases HL.
void x23_inc_hl(Emulator* emu)
{
    emu->cpu.hl(emu->cpu.hl() + 1);
    emu->Tick(2);
}
//! INC H : Increases H.
void x24_inc_h(Emulator* emu)
{
    Inc8(emu, emu->cpu.h);
    emu->Tick(1);
}
//! DEC H : Decreases H.
void x25_dec_h(Emulator* emu)
{
    Dec8(emu, emu->cpu.h);
    emu->Tick(1);
}
//! LD H, d8 : Loads 8-bit immediate data to H.
void x26_ld_h_d8(Emulator* emu)
{
    emu->cpu.h = ReadU8(emu);
    emu->Tick(2);
}
//! DAA : Decimal adjust for A.
void x27_daa(Emulator* emu)
{
    if(emu->cpu.fn())
    {
        if(emu->cpu.fc())
        {
            if(emu->cpu.fh()) emu->cpu.a += 0x9A;
            else emu->cpu.a += 0xA0;
        }
        else
        {
            if(emu->cpu.fh()) emu->cpu.a += 0xFA;
        }
    }
    else
    {
        if(emu->cpu.fc() || (emu->cpu.a > 0x99))
        {
            if(emu->cpu.fh() || ((emu->cpu.a & 0x0F) > 0x09)) emu->cpu.a += 0x66;
            else emu->cpu.a += 0x60;
            emu->cpu.set_fc();
        }
        else
        {
            if(emu->cpu.fh() || ((emu->cpu.a & 0x0F) > 0x09)) emu->cpu.a += 0x06;
        }
    }
    SetZeroFlag(emu, emu->cpu.a);
    emu->cpu.reset_fh();
    emu->Tick(1);
}
//! JR Z, r8 : Jumps to PC + r8 if Z is 1.
void x28_jr_z_r8(Emulator* emu)
{
    i8 offset = (i8)ReadU8(emu);
    if(emu->cpu.fz())
    {
        emu->cpu.pc += (i16)offset;
        emu->Tick(3);
    }
    else
    {
        emu->Tick(2);
    }
}
//! ADD HL, HL : Adds HL to HL.
void x29_add_hl_hl(Emulator* emu)
{
    emu->cpu.hl(Add16(emu, emu->cpu.hl(), emu->cpu.hl()));
    emu->Tick(2);
}
//! LD A (HL+) : Loads value from memory pointed by HL to A, then increases HL.
void x2a_ldi_a_mhl(Emulator* emu)
{
    emu->cpu.a = emu->BusRead(emu->cpu.hl());
    emu->cpu.hl(emu->cpu.hl() + 1);
    emu->Tick(2);
}
//! DEC HL : Decreases HL.
void x2b_dec_hl(Emulator* emu)
{
    emu->cpu.hl(emu->cpu.hl() - 1);
    emu->Tick(2);
}
//! INC L : Increases L.
void x2c_inc_l(Emulator* emu)
{
    Inc8(emu, emu->cpu.l);
    emu->Tick(1);
}
//! DEC L : Decreases L.
void x2d_dec_l(Emulator* emu)
{
    Dec8(emu, emu->cpu.l);
    emu->Tick(1);
}
//! LD L, d8 : Loads 8-bit immediate data to L.
void x2e_ld_l_d8(Emulator* emu)
{
    emu->cpu.l = ReadU8(emu);
    emu->Tick(2);
}
//! CPL : Takes complements of A (inverts every bit of A).
void x2f_cpl(Emulator* emu)
{
    emu->cpu.a = emu->cpu.a ^ 0xFF;
    emu->cpu.set_fn();
    emu->cpu.set_fh();
    emu->Tick(1);
}
//! JR NC, r8 : Jumps to PC + r8 if C is 0.
void x30_jr_nc_r8(Emulator* emu)
{
    i8 offset = (i8)ReadU8(emu);
    if(!emu->cpu.fc())
    {
        emu->cpu.pc += (i16)offset;
        emu->Tick(3);
    }
    else
    {
        emu->Tick(2);
    }
}
//! LD SP, d16 : Loads 16-bit immediate data to SP.
void x31_ld_sp_d16(Emulator* emu)
{
    emu->cpu.sp = ReadU16(emu);
    emu->Tick(3);
}
//! LD (HL-) A : Stores A to the memory pointed by HL, then decreases HL.
void x32_ldd_mhl_a(Emulator* emu)
{
    emu->BusWrite(emu->cpu.hl(), emu->cpu.a);
    emu->cpu.hl(emu->cpu.hl() - 1);
    emu->Tick(2);
}
//! INC SP : Increases SP.
void x33_inc_sp(Emulator* emu)
{
    ++emu->cpu.sp;
    emu->Tick(2);
}
//! INC (HL) : Increases data in memory pointed by HL.
void x34_inc_mhl(Emulator* emu)
{
    u8 data = emu->BusRead(emu->cpu.hl());
    emu->Tick(1);
    Inc8(emu, data);
    emu->BusWrite(emu->cpu.hl(), data);
    emu->Tick(2);
}
//! DEC (HL) : Decreases data in memory pointed by HL.
void x35_dec_mhl(Emulator* emu)
{
    u8 data = emu->BusRead(emu->cpu.hl());
    emu->Tick(1);
    Dec8(emu, data);
    emu->BusWrite(emu->cpu.hl(), data);
    emu->Tick(2);
}
//! LD (HL), d8 : Stores 8-bit immediate data to memory pointed by HL.
void x36_ld_mhl_d8(Emulator* emu)
{
    u8 data = ReadU8(emu);
    emu->Tick(1);
    emu->BusWrite(emu->cpu.hl(), data);
    emu->Tick(2);
}
//! SCF : Sets C to 1.
void x37_scf(Emulator* emu)
{
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
    emu->cpu.set_fc();
    emu->Tick(1);
}
//! JR C, r8 : Jumps to PC + r8 if C is 1.
void x38_jr_c_r8(Emulator* emu)
{
    i8 offset = (i8)ReadU8(emu);
    if(emu->cpu.fc())
    {
        emu->cpu.pc += (i16)offset;
        emu->Tick(3);
    }
    else
    {
        emu->Tick(2);
    }
}
//! ADD HL, SP : Adds SP to HL.
void x39_add_hl_sp(Emulator* emu)
{
    emu->cpu.hl(Add16(emu, emu->cpu.hl(), emu->cpu.sp));
    emu->Tick(2);
}
//! LD A (HL-) : Loads value from memory pointed by HL to A, then decreases HL.
void x3a_ldd_a_mhl(Emulator* emu)
{
    emu->cpu.a = emu->BusRead(emu->cpu.hl());
    emu->cpu.hl(emu->cpu.hl() - 1);
    emu->Tick(2);
}
//! DEC SP : Decreases SP.
void x3b_dec_sp(Emulator* emu)
{
    --emu->cpu.sp;
    emu->Tick(2);
}
//! INC A : Increases A.
void x3c_inc_a(Emulator* emu)
{
    Inc8(emu, emu->cpu.a);
    emu->Tick(1);
}
//! DEC A : Decreases A.
void x3d_dec_a(Emulator* emu)
{
    Dec8(emu, emu->cpu.a);
    emu->Tick(1);
}
//! LD A, d8 : Loads 8-bit immediate data to A.
void x3e_ld_a_d8(Emulator* emu)
{
    emu->cpu.a = ReadU8(emu);
    emu->Tick(2);
}
//! CCF : Flips C.
void x3f_ccf(Emulator* emu)
{
    emu->cpu.reset_fn();
    emu->cpu.reset_fh();
    if(emu->cpu.fc())
    {
        emu->cpu.reset_fc();
    }
    else
    {
        emu->cpu.set_fc();
    }
    emu->Tick(1);
}
//! LD B, B : Loads B to B.
void x40_ld_b_b(Emulator* emu)
{
    emu->cpu.b = emu->cpu.b;
    emu->Tick(1);
}
//! LD B, C : Loads C to B.
void x41_ld_b_c(Emulator* emu)
{
    emu->cpu.b = emu->cpu.c;
    emu->Tick(1);
}
//! LD B, D : Loads D to B.
void x42_ld_b_d(Emulator* emu)
{
    emu->cpu.b = emu->cpu.d;
    emu->Tick(1);
}
//! LD B, E : Loads E to B.
void x43_ld_b_e(Emulator* emu)
{
    emu->cpu.b = emu->cpu.e;
    emu->Tick(1);
}
//! LD B, H : Loads H to B.
void x44_ld_b_h(Emulator* emu)
{
    emu->cpu.b = emu->cpu.h;
    emu->Tick(1);
}
//! LD B, L : Loads L to B.
void x45_ld_b_l(Emulator* emu)
{
    emu->cpu.b = emu->cpu.l;
    emu->Tick(1);
}
//! LD B, (HL) : Loads 8-bit data pointed by HL to B.
void x46_ld_b_mhl(Emulator* emu)
{
    emu->cpu.b = emu->BusRead(emu->cpu.hl());
    emu->Tick(2);
}
//! LD B, A : Loads A to B.
void x47_ld_b_a(Emulator* emu)
{
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
//! HALT : Pauses the CPU.
void x76_halt(Emulator* emu)
{
    emu->cpu.halted = true;
    emu->Tick(1);
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
//! ADD A, B : Adds B to A.
void x80_add_a_b(Emulator* emu)
{
    emu->cpu.a = Add8(emu, emu->cpu.a, emu->cpu.b);
    emu->Tick(1);
}
//! ADD A, C : Adds C to A.
void x81_add_a_c(Emulator* emu)
{
    emu->cpu.a = Add8(emu, emu->cpu.a, emu->cpu.c);
    emu->Tick(1);
}
//! ADD A, D : Adds D to A.
void x82_add_a_d(Emulator* emu)
{
    emu->cpu.a = Add8(emu, emu->cpu.a, emu->cpu.d);
    emu->Tick(1);
}
//! ADD A, E : Adds E to A.
void x83_add_a_e(Emulator* emu)
{
    emu->cpu.a = Add8(emu, emu->cpu.a, emu->cpu.e);
    emu->Tick(1);
}
//! ADD A, H : Adds H to A.
void x84_add_a_h(Emulator* emu)
{
    emu->cpu.a = Add8(emu, emu->cpu.a, emu->cpu.h);
    emu->Tick(1);
}
//! ADD A, L : Adds L to A.
void x85_add_a_l(Emulator* emu)
{
    emu->cpu.a = Add8(emu, emu->cpu.a, emu->cpu.l);
    emu->Tick(1);
}
//! ADD A, (HL) : Adds data pointed by HL to A.
void x86_add_a_mhl(Emulator* emu)
{
    u8 data = emu->BusRead(emu->cpu.hl());
    emu->Tick(1);
    emu->cpu.a = Add8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! ADD A, A : Adds A to A.
void x87_add_a_a(Emulator* emu)
{
    emu->cpu.a = Add8(emu, emu->cpu.a, emu->cpu.a);
    emu->Tick(1);
}
//! ADC A, B : Adds B to A with carry.
void x88_adc_a_b(Emulator* emu)
{
    emu->cpu.a = Adc8(emu, emu->cpu.a, emu->cpu.b);
    emu->Tick(1);
}
//! ADC A, C : Adds C to A with carry.
void x89_adc_a_c(Emulator* emu)
{
    emu->cpu.a = Adc8(emu, emu->cpu.a, emu->cpu.c);
    emu->Tick(1);
}
//! ADC A, D : Adds D to A with carry.
void x8a_adc_a_d(Emulator* emu)
{
    emu->cpu.a = Adc8(emu, emu->cpu.a, emu->cpu.d);
    emu->Tick(1);
}
//! ADC A, E : Adds E to A with carry.
void x8b_adc_a_e(Emulator* emu)
{
    emu->cpu.a = Adc8(emu, emu->cpu.a, emu->cpu.e);
    emu->Tick(1);
}
//! ADC A, H : Adds H to A with carry.
void x8c_adc_a_h(Emulator* emu)
{
    emu->cpu.a = Adc8(emu, emu->cpu.a, emu->cpu.h);
    emu->Tick(1);
}
//! ADC A, L : Adds L to A with carry.
void x8d_adc_a_l(Emulator* emu)
{
    emu->cpu.a = Adc8(emu, emu->cpu.a, emu->cpu.l);
    emu->Tick(1);
}
//! ADC A, (HL) : Adds data pointed by HL to A with carry.
void x8e_adc_a_mhl(Emulator* emu)
{
    u8 data = emu->BusRead(emu->cpu.hl());
    emu->Tick(1);
    emu->cpu.a = Adc8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! ADC A, A : Adds A to A with carry.
void x8f_adc_a_a(Emulator* emu)
{
    emu->cpu.a = Adc8(emu, emu->cpu.a, emu->cpu.a);
    emu->Tick(1);
}
//! SUB B : Subtracts B from A.
void x90_sub_b(Emulator* emu)
{
    emu->cpu.a = Sub8(emu, emu->cpu.a, emu->cpu.b);
    emu->Tick(1);
}
//! SUB C : Subtracts C from A.
void x91_sub_c(Emulator* emu)
{
    emu->cpu.a = Sub8(emu, emu->cpu.a, emu->cpu.c);
    emu->Tick(1);
}
//! SUB D : Subtracts D from A.
void x92_sub_d(Emulator* emu)
{
    emu->cpu.a = Sub8(emu, emu->cpu.a, emu->cpu.d);
    emu->Tick(1);
}
//! SUB E : Subtracts E from A.
void x93_sub_e(Emulator* emu)
{
    emu->cpu.a = Sub8(emu, emu->cpu.a, emu->cpu.e);
    emu->Tick(1);
}
//! SUB H : Subtracts H from A.
void x94_sub_h(Emulator* emu)
{
    emu->cpu.a = Sub8(emu, emu->cpu.a, emu->cpu.h);
    emu->Tick(1);
}
//! SUB L : Subtracts L from A.
void x95_sub_l(Emulator* emu)
{
    emu->cpu.a = Sub8(emu, emu->cpu.a, emu->cpu.l);
    emu->Tick(1);
}
//! SUB (HL) : Subtracts data pointed by HL from A.
void x96_sub_mhl(Emulator* emu)
{
    u8 data = emu->BusRead(emu->cpu.hl());
    emu->Tick(1);
    emu->cpu.a = Sub8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! SUB A : Subtracts A from A.
void x97_sub_a(Emulator* emu)
{
    emu->cpu.a = Sub8(emu, emu->cpu.a, emu->cpu.a);
    emu->Tick(1);
}
//! SBC A, B : Subtracts B from A with carry.
void x98_sbc_a_b(Emulator* emu)
{
    emu->cpu.a = Sbc8(emu, emu->cpu.a, emu->cpu.b);
    emu->Tick(1);
}
//! SBC A, C : Subtracts C from A with carry.
void x99_sbc_a_c(Emulator* emu)
{
    emu->cpu.a = Sbc8(emu, emu->cpu.a, emu->cpu.c);
    emu->Tick(1);
}
//! SBC A, D : Subtracts D from A with carry.
void x9a_sbc_a_d(Emulator* emu)
{
    emu->cpu.a = Sbc8(emu, emu->cpu.a, emu->cpu.d);
    emu->Tick(1);
}
//! SBC A, E : Subtracts E from A with carry.
void x9b_sbc_a_e(Emulator* emu)
{
    emu->cpu.a = Sbc8(emu, emu->cpu.a, emu->cpu.e);
    emu->Tick(1);
}
//! SBC A, H : Subtracts H from A with carry.
void x9c_sbc_a_h(Emulator* emu)
{
    emu->cpu.a = Sbc8(emu, emu->cpu.a, emu->cpu.h);
    emu->Tick(1);
}
//! SBC A, L : Subtracts L from A with carry.
void x9d_sbc_a_l(Emulator* emu)
{
    emu->cpu.a = Sbc8(emu, emu->cpu.a, emu->cpu.l);
    emu->Tick(1);
}
//! SBC A, (HL) : Subtracts data pointed by HL from A with carry.
void x9e_sbc_a_mhl(Emulator* emu)
{
    u8 data = emu->BusRead(emu->cpu.hl());
    emu->Tick(1);
    emu->cpu.a = Sbc8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! SBC A, A : Subtracts A from A with carry.
void x9f_sbc_a_a(Emulator* emu)
{
    emu->cpu.a = Sbc8(emu, emu->cpu.a, emu->cpu.a);
    emu->Tick(1);
}
//! AND B : Performs bitwise AND on A and B.
void xa0_and_b(Emulator* emu)
{
    emu->cpu.a = And8(emu, emu->cpu.a, emu->cpu.b);
    emu->Tick(1);
}
//! AND C : Performs bitwise AND on A and C.
void xa1_and_c(Emulator* emu)
{
    emu->cpu.a = And8(emu, emu->cpu.a, emu->cpu.c);
    emu->Tick(1);
}
//! AND D : Performs bitwise AND on A and D.
void xa2_and_d(Emulator* emu)
{
    emu->cpu.a = And8(emu, emu->cpu.a, emu->cpu.d);
    emu->Tick(1);
}
//! AND E : Performs bitwise AND on A and E.
void xa3_and_e(Emulator* emu)
{
    emu->cpu.a = And8(emu, emu->cpu.a, emu->cpu.e);
    emu->Tick(1);
}
//! AND H : Performs bitwise AND on A and H.
void xa4_and_h(Emulator* emu)
{
    emu->cpu.a = And8(emu, emu->cpu.a, emu->cpu.h);
    emu->Tick(1);
}
//! AND L : Performs bitwise AND on A and L.
void xa5_and_l(Emulator* emu)
{
    emu->cpu.a = And8(emu, emu->cpu.a, emu->cpu.l);
    emu->Tick(1);
}
//! AND (HL) : Performs bitwise AND on A and data pointed by HL.
void xa6_and_mhl(Emulator* emu)
{
    u8 data = emu->BusRead(emu->cpu.hl());
    emu->Tick(1);
    emu->cpu.a = And8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! AND A : Performs bitwise AND on A and A.
void xa7_and_a(Emulator* emu)
{
    emu->cpu.a = And8(emu, emu->cpu.a, emu->cpu.a);
    emu->Tick(1);
}
//! XOR B : Performs bitwise XOR on A and B.
void xa8_xor_b(Emulator* emu)
{
    emu->cpu.a = Xor8(emu, emu->cpu.a, emu->cpu.b);
    emu->Tick(1);
}
//! XOR C : Performs bitwise XOR on A and C.
void xa9_xor_c(Emulator* emu)
{
    emu->cpu.a = Xor8(emu, emu->cpu.a, emu->cpu.c);
    emu->Tick(1);
}
//! XOR D : Performs bitwise XOR on A and D.
void xaa_xor_d(Emulator* emu)
{
    emu->cpu.a = Xor8(emu, emu->cpu.a, emu->cpu.d);
    emu->Tick(1);
}
//! XOR E : Performs bitwise XOR on A and E.
void xab_xor_e(Emulator* emu)
{
    emu->cpu.a = Xor8(emu, emu->cpu.a, emu->cpu.e);
    emu->Tick(1);
}
//! XOR H : Performs bitwise XOR on A and H.
void xac_xor_h(Emulator* emu)
{
    emu->cpu.a = Xor8(emu, emu->cpu.a, emu->cpu.h);
    emu->Tick(1);
}
//! XOR L : Performs bitwise XOR on A and L.
void xad_xor_l(Emulator* emu)
{
    emu->cpu.a = Xor8(emu, emu->cpu.a, emu->cpu.l);
    emu->Tick(1);
}
//! XOR (HL) : Performs bitwise XOR on A and data pointed by HL.
void xae_xor_mhl(Emulator* emu)
{
    u8 data = emu->BusRead(emu->cpu.hl());
    emu->Tick(1);
    emu->cpu.a = Xor8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! XOR A : Performs bitwise XOR on A and A.
void xaf_xor_a(Emulator* emu)
{
    emu->cpu.a = Xor8(emu, emu->cpu.a, emu->cpu.a);
    emu->Tick(1);
}
//! OR B : Performs bitwise OR on A and B.
void xb0_or_b(Emulator* emu)
{
    emu->cpu.a = Or8(emu, emu->cpu.a, emu->cpu.b);
    emu->Tick(1);
}
//! OR C : Performs bitwise OR on A and C.
void xb1_or_c(Emulator* emu)
{
    emu->cpu.a = Or8(emu, emu->cpu.a, emu->cpu.c);
    emu->Tick(1);
}
//! OR D : Performs bitwise OR on A and D.
void xb2_or_d(Emulator* emu)
{
    emu->cpu.a = Or8(emu, emu->cpu.a, emu->cpu.d);
    emu->Tick(1);
}
//! OR E : Performs bitwise OR on A and E.
void xb3_or_e(Emulator* emu)
{
    emu->cpu.a = Or8(emu, emu->cpu.a, emu->cpu.e);
    emu->Tick(1);
}
//! OR H : Performs bitwise OR on A and H.
void xb4_or_h(Emulator* emu)
{
    emu->cpu.a = Or8(emu, emu->cpu.a, emu->cpu.h);
    emu->Tick(1);
}
//! OR L : Performs bitwise OR on A and L.
void xb5_or_l(Emulator* emu)
{
    emu->cpu.a = Or8(emu, emu->cpu.a, emu->cpu.l);
    emu->Tick(1);
}
//! OR (HL) : Performs bitwise OR on A and data pointed by HL.
void xb6_or_mhl(Emulator* emu)
{
    u8 data = emu->BusRead(emu->cpu.hl());
    emu->Tick(1);
    emu->cpu.a = Or8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! OR A : Performs bitwise OR on A and A.
void xb7_or_a(Emulator* emu)
{
    emu->cpu.a = Or8(emu, emu->cpu.a, emu->cpu.a);
    emu->Tick(1);
}
//! CP B : Compares B with A.
void xb8_cp_b(Emulator* emu)
{
    Cmp8(emu, emu->cpu.a, emu->cpu.b);
    emu->Tick(1);
}
//! CP C : Compares C with A.
void xb9_cp_c(Emulator* emu)
{
    Cmp8(emu, emu->cpu.a, emu->cpu.c);
    emu->Tick(1);
}
//! CP D : Compares D with A.
void xba_cp_d(Emulator* emu)
{
    Cmp8(emu, emu->cpu.a, emu->cpu.d);
    emu->Tick(1);
}
//! CP E : Compares E with A.
void xbb_cp_e(Emulator* emu)
{
    Cmp8(emu, emu->cpu.a, emu->cpu.e);
    emu->Tick(1);
}
//! CP H : Compares H with A.
void xbc_cp_h(Emulator* emu)
{
    Cmp8(emu, emu->cpu.a, emu->cpu.h);
    emu->Tick(1);
}
//! CP L : Compares L with A.
void xbd_cp_l(Emulator* emu)
{
    Cmp8(emu, emu->cpu.a, emu->cpu.l);
    emu->Tick(1);
}
//! CP (HL) : Compares data pointed by HL with A.
void xbe_cp_mhl(Emulator* emu)
{
    Cmp8(emu, emu->cpu.a, emu->BusRead(emu->cpu.hl()));
    emu->Tick(2);
}
//! CP A : Compares A with A.
void xbf_cp_a(Emulator* emu)
{
    Cmp8(emu, emu->cpu.a, emu->cpu.a);
    emu->Tick(1);
}
//! RET NZ : Returns the function if Z is 0.
void xc0_ret_nz(Emulator* emu)
{
    if(!emu->cpu.fz())
    {
        emu->cpu.pc = Pop16(emu);
        emu->Tick(5);
    }
    else
    {
        emu->Tick(2);
    }
}
//! POP BC : Pops 16-bit data from stack to BC.
void xc1_pop_bc(Emulator* emu)
{
    emu->cpu.bc(Pop16(emu));
    emu->Tick(3);
}
//! JP NZ, a16 : Jumps to the 16-bit address if Z is 0.
void xc2_jp_nz_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    if(!emu->cpu.fz())
    {
        emu->cpu.pc = addr;
        emu->Tick(4);
    }
    else
    {
        emu->Tick(3);
    }
}
//! JP a16 : Jumps to the 16-bit address.
void xc3_jp_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    emu->cpu.pc = addr;
    emu->Tick(4);
}
//! CALL NZ, a16 : Calls the function if Z is 0.
void xc4_call_nz_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    emu->Tick(2);
    if(!emu->cpu.fz())
    {
        Push16(emu, emu->cpu.pc);
        emu->cpu.pc = addr;
        emu->Tick(4);
    }
    else
    {
        emu->Tick(1);
    }
}
//! PUSH BC : Pushes BC to the stack.
void xc5_push_bc(Emulator* emu)
{
    Push16(emu, emu->cpu.bc());
    emu->Tick(4);
}
//! ADD A, d8 : Adds 8-bit immediate data to A.
void xc6_add_a_d8(Emulator* emu)
{
    u8 data = ReadU8(emu);
    emu->Tick(1);
    emu->cpu.a = Add8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! RST 00H : Pushes PC to stack and resets PC to 0x00.
void xc7_rst_00h(Emulator* emu)
{
    Push16(emu, emu->cpu.pc);
    emu->cpu.pc = 0x0000;
    emu->Tick(4);
}
//! RET Z : Returns the function if Z is 1.
void xc8_ret_z(Emulator* emu)
{
    if(emu->cpu.fz())
    {
        emu->cpu.pc = Pop16(emu);
        emu->Tick(5);
    }
    else
    {
        emu->Tick(2);
    }
}
//! RET : Returns the function.
void xc9_ret(Emulator* emu)
{
    emu->cpu.pc = Pop16(emu);
    emu->Tick(4);
}
//! JP Z, a16 : Jumps to the 16-bit address if Z is 1.
void xca_jp_z_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    if(emu->cpu.fz())
    {
        emu->cpu.pc = addr;
        emu->Tick(4);
    }
    else
    {
        emu->Tick(3);
    }
}
//! PREFIX CB : Invokes CB instructions.
void xcb_prefix_cb(Emulator* emu)
{
    u8 op = ReadU8(emu);
    emu->Tick(1);
    u8 data_bits = op & 0x07;
    u8 data;
    // Load data.
    switch(data_bits)
    {
        case 0: data = emu->cpu.b; break;
        case 1: data = emu->cpu.c; break;
        case 2: data = emu->cpu.d; break;
        case 3: data = emu->cpu.e; break;
        case 4: data = emu->cpu.h; break;
        case 5: data = emu->cpu.l; break;
        case 6: data = emu->BusRead(emu->cpu.hl()); emu->Tick(1); break;
        case 7: data = emu->cpu.a; break;
        default: ERROR("wrong XCB instruction"); break;
    }
    // Modify data.
    u8 op_bits = (op & 0xF8) >> 3;
    if(op_bits == 0) rlc_8(emu, data);
    else if(op_bits == 1) rrc_8(emu, data);
    else if(op_bits == 2) rl_8(emu, data);
    else if(op_bits == 3) rr_8(emu, data);
    else if(op_bits == 4) sla_8(emu, data);
    else if(op_bits == 5) sra_8(emu, data);
    else if(op_bits == 6) swap_8(emu, data);
    else if(op_bits == 7) srl_8(emu, data);
    else if(op_bits <= 0x0F) bit_8(emu, data, op_bits - 0x08);
    else if(op_bits <= 0x17) res_8(emu, data, op_bits - 0x10);
    else if(op_bits <= 0x1F) set_8(emu, data, op_bits - 0x18);
    else ERROR("wrong XCB instruction");
    // Store data if op is not BIT (which does not modify data).
    if(op_bits <= 0x07 || op_bits >= 0x10)
    {
        switch(data_bits)
        {
            case 0: emu->cpu.b = data; break;
            case 1: emu->cpu.c = data; break;
            case 2: emu->cpu.d = data; break;
            case 3: emu->cpu.e = data; break;
            case 4: emu->cpu.h = data; break;
            case 5: emu->cpu.l = data; break;
            case 6: emu->BusWrite(emu->cpu.hl(), data); emu->Tick(1); break;
            case 7: emu->cpu.a = data; break;
            default: ERROR("wrong XCB instruction"); break;
        }
    }
    emu->Tick(1);
}
//! CALL Z, a16 : Calls the function if Z is 1.
void xcc_call_z_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    emu->Tick(2);
    if(emu->cpu.fz())
    {
        Push16(emu, emu->cpu.pc);
        emu->cpu.pc = addr;
        emu->Tick(4);
    }
    else
    {
        emu->Tick(1);
    }
}
//! CALL a16 : Calls the function.
void xcd_call_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    emu->Tick(2);
    Push16(emu, emu->cpu.pc);
    emu->cpu.pc = addr;
    emu->Tick(4);
}
//! ADC A, d8 : Adds 8-bit immediate data to A with carry.
void xce_adc_a_d8(Emulator* emu)
{
    u8 data = ReadU8(emu);
    emu->Tick(1);
    emu->cpu.a = Adc8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! RST 08H : Pushes PC to stack and resets PC to 0x08.
void xcf_rst_08h(Emulator* emu)
{
    Push16(emu, emu->cpu.pc);
    emu->cpu.pc = 0x0008;
    emu->Tick(4);
}
//! RET NC : Returns the function if C is 0.
void xd0_ret_nc(Emulator* emu)
{
    if(!emu->cpu.fc())
    {
        emu->cpu.pc = Pop16(emu);
        emu->Tick(5);
    }
    else
    {
        emu->Tick(2);
    }
}
//! POP DE : Pops 16-bit data from stack to DE.
void xd1_pop_de(Emulator* emu)
{
    emu->cpu.de(Pop16(emu));
    emu->Tick(3);
}
//! JP NC, a16 : Jumps to the 16-bit address if C is 0.
void xd2_jp_nc_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    if(!emu->cpu.fc())
    {
        emu->cpu.pc = addr;
        emu->Tick(4);
    }
    else
    {
        emu->Tick(3);
    }
}
//! CALL NC, a16 : Calls the function if C is 0.
void xd4_call_nc_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    emu->Tick(2);
    if(!emu->cpu.fc())
    {
        Push16(emu, emu->cpu.pc);
        emu->cpu.pc = addr;
        emu->Tick(4);
    }
    else
    {
        emu->Tick(1);
    }
}
//! PUSH DE : Pushes DE to the stack.
void xd5_push_de(Emulator* emu)
{
    Push16(emu, emu->cpu.de());
    emu->Tick(4);
}
//! SUB d8 : Subtracts 8-bit immediate data from A.
void xd6_sub_d8(Emulator* emu)
{
    u8 data = ReadU8(emu);
    emu->Tick(1);
    emu->cpu.a = Sub8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! RST 10H : Pushes PC to stack and resets PC to 0x10.
void xd7_rst_10h(Emulator* emu)
{
    Push16(emu, emu->cpu.pc);
    emu->cpu.pc = 0x0010;
    emu->Tick(4);
}
//! RET C : Returns the function if C is 1.
void xd8_ret_c(Emulator* emu)
{
    if(emu->cpu.fc())
    {
        emu->cpu.pc = Pop16(emu);
        emu->Tick(5);
    }
    else
    {
        emu->Tick(2);
    }
}
//! RETI : Returns and enables interruption.
void xd9_reti(Emulator* emu)
{
    emu->cpu.EnableInterruptMaster();
    xc9_ret(emu);
}
//! JP C, a16 : Jumps to the 16-bit address if C is 1.
void xda_jp_c_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    if(emu->cpu.fc())
    {
        emu->cpu.pc = addr;
        emu->Tick(4);
    }
    else
    {
        emu->Tick(3);
    }
}
//! CALL C, a16 : Calls the function if C is 1.
void xdc_call_c_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    emu->Tick(2);
    if(emu->cpu.fc())
    {
        Push16(emu, emu->cpu.pc);
        emu->cpu.pc = addr;
        emu->Tick(4);
    }
    else
    {
        emu->Tick(1);
    }
}
//! SBC A, d8 : Subtracts 8-bit immediate data from A.
void xde_sbc_a_d8(Emulator* emu)
{
    u8 data = ReadU8(emu);
    emu->Tick(1);
    emu->cpu.a = Sbc8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! RST 18H : Pushes PC to stack and resets PC to 0x18.
void xdf_rst_18h(Emulator* emu)
{
    Push16(emu, emu->cpu.pc);
    emu->cpu.pc = 0x0018;
    emu->Tick(4);
}
//! LDH (a8) A : Stores A to high memory address (0xFF00 + a8).
void xe0_ldh_m8_a(Emulator* emu)
{
    u8 addr = ReadU8(emu);
    emu->Tick(1);
    emu->BusWrite(0xFF00 + (u16)addr, emu->cpu.a);
    emu->Tick(2);
}
//! POP HL : Pops 16-bit data from stack to HL.
void xe1_pop_hl(Emulator* emu)
{
    emu->cpu.hl(Pop16(emu));
    emu->Tick(3);
}
//! LD (C), A : Stores A to memory at 0xFF00 + C.
void xe2_ld_mc_a(Emulator* emu)
{
    emu->BusWrite(0xFF00 + (u16)emu->cpu.c, emu->cpu.a);
    emu->Tick(2);
}
//! PUSH HL : Pushes HL to the stack.
void xe5_push_hl(Emulator* emu)
{
    Push16(emu, emu->cpu.hl());
    emu->Tick(4);
}
//! AND d8 : Performs bitwise AND between A and 8-bit immediate data.
void xe6_and_d8(Emulator* emu)
{
    u8 data = ReadU8(emu);
    emu->Tick(1);
    emu->cpu.a = And8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! RST 20H : Pushes PC to stack and resets PC to 0x20.
void xe7_rst_20h(Emulator* emu)
{
    Push16(emu, emu->cpu.pc);
    emu->cpu.pc = 0x0020;
    emu->Tick(4);
}
//! ADD SP, r8 : Adds 8-bit immediate signed integer to SP.
void xe8_add_sp_r8(Emulator* emu)
{
    emu->cpu.reset_fz();
    emu->cpu.reset_fn();
    u16 v1 = emu->cpu.sp;
    i16 v2 = (i16)((i8)ReadU8(emu));
    emu->Tick(1);
    u16 r = v1 + v2;
    u16 check = v1 ^ v2 ^ r;
    if(check & 0x10) emu->cpu.set_fh();
    else emu->cpu.reset_fh();
    if(check & 0x100) emu->cpu.set_fc();
    else emu->cpu.reset_fc();
    emu->cpu.sp = r;
    emu->Tick(3);
}
//! JP HL : Jumps to HL.
void xe9_jp_hl(Emulator* emu)
{
    emu->cpu.pc = emu->cpu.hl();
    emu->Tick(1);
}
//! LD (a16), A : Stores A to the memory address.
void xea_ld_a16_a(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    emu->Tick(2);
    emu->BusWrite(addr, emu->cpu.a);
    emu->Tick(2);
}
//! XOR d8 : Performs bitwise XOR between A and 8-bit immediate data.
void xee_xor_d8(Emulator* emu)
{
    u8 data = ReadU8(emu);
    emu->Tick(1);
    emu->cpu.a = Xor8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! RST 28H : Pushes PC to stack and resets PC to 0x28.
void xef_rst_28h(Emulator* emu)
{
    Push16(emu, emu->cpu.pc);
    emu->cpu.pc = 0x0028;
    emu->Tick(4);
}
//! LDH A, (a8) : Loads data in high memory address (0xFF00 + a8) to A.
void xf0_ldh_a_m8(Emulator* emu)
{
    u8 addr = ReadU8(emu);
    emu->Tick(1);
    emu->cpu.a = emu->BusRead(0xFF00 + (u16)addr);
    emu->Tick(2);
}
//! POP AF : Pops 16-bit data from stack to AF.
void xf1_pop_af(Emulator* emu)
{
    emu->cpu.af(Pop16(emu));
    emu->Tick(3);
}
//! LD A, (C) : Loads data at memory 0xFF00 + C to A.
void xf2_ld_a_mc(Emulator* emu)
{
    emu->cpu.a = emu->BusRead(0xFF00 + (u16)emu->cpu.c);
    emu->Tick(2);
}
//! DI : Disable interrupts.
void xf3_di(Emulator* emu)
{
    emu->cpu.DisableInterruptMaster();
    emu->Tick(1);
}
//! PUSH AF : Pushes AF to the stack.
void xf5_push_af(Emulator* emu)
{
    Push16(emu, emu->cpu.af());
    emu->Tick(4);
}
//! OR d8 : Performs bitwise OR between A and 8-bit immediate data.
void xf6_or_d8(Emulator* emu)
{
    u8 data = ReadU8(emu);
    emu->Tick(1);
    emu->cpu.a = Or8(emu, emu->cpu.a, data);
    emu->Tick(1);
}
//! RST 30H : Pushes PC to stack and resets PC to 0x30.
void xf7_rst_30h(Emulator* emu)
{
    Push16(emu, emu->cpu.pc);
    emu->cpu.pc = 0x0030;
    emu->Tick(4);
}
//! LD HL, SP + r8 : Loads SP + r8 to HL.
void xf8_ld_hl_sp_r8(Emulator* emu)
{
    emu->cpu.reset_fz();
    emu->cpu.reset_fn();
    u16 v1 = emu->cpu.sp;
    i16 v2 = (i16)((i8)ReadU8(emu));
    emu->Tick(1);
    u16 r = v1 + v2;
    u16 check = v1 ^ v2 ^ r;
    if(check & 0x10) emu->cpu.set_fh();
    else emu->cpu.reset_fh();
    if(check & 0x100) emu->cpu.set_fc();
    else emu->cpu.reset_fc();
    emu->cpu.hl(r);
    emu->Tick(2);
}
//! LD SP, HL : Loads HL to SP.
void xf9_ld_sp_hl(Emulator* emu)
{
    emu->cpu.sp = emu->cpu.hl();
    emu->Tick(2);
}
//! LD A, (a16) : Loads data at memory address to A.
void xfa_ld_a_a16(Emulator* emu)
{
    u16 addr = ReadU16(emu);
    emu->Tick(2);
    emu->cpu.a = emu->BusRead(addr);
    emu->Tick(2);
}
//! EI : Enable interruption.
void xfb_ei(Emulator* emu)
{
    emu->cpu.EnableInterruptMaster();
    emu->Tick(1);
}
//! CP d8 : Compares A with 8-bit immediate data.
void xfe_cp_d8(Emulator* emu)
{
    Cmp8(emu, emu->cpu.a, ReadU8(emu));
    emu->Tick(2);
}
//! RST 38H : Pushes PC to stack and resets PC to 0x38.
void xff_rst_38h(Emulator* emu)
{
    Push16(emu, emu->cpu.pc);
    emu->cpu.pc = 0x0038;
    emu->Tick(4);
}


InstructionFunc* instructionsMap[256] = {
        x00_nop,      x01_ld_bc_d16, x02_ld_mbc_a,  x03_inc_bc, x04_inc_b,   x05_dec_b,   x06_ld_b_d8,   x07_rlca, x08_ld_a16_sp, x09_add_hl_bc, x0a_ld_a_mbc,  x0b_dec_bc, x0c_inc_c, x0d_dec_c, x0e_ld_c_d8, x0f_rrca,
        x10_stop,     x11_ld_de_d16, x12_ld_mde_a,  x13_inc_de, x14_inc_d,   x15_dec_d,   x16_ld_d_d8,   x17_rla,  x18_jr_r8,     x19_add_hl_de, x1a_ld_a_mde,  x1b_dec_de, x1c_inc_e, x1d_dec_e, x1e_ld_e_d8, x1f_rra,
        x20_jr_nz_r8, x21_ld_hl_d16, x22_ldi_mhl_a, x23_inc_hl, x24_inc_h,   x25_dec_h,   x26_ld_h_d8,   x27_daa, x28_jr_z_r8,   x29_add_hl_hl, x2a_ldi_a_mhl, x2b_dec_hl, x2c_inc_l, x2d_dec_l, x2e_ld_l_d8, x2f_cpl,
        x30_jr_nc_r8, x31_ld_sp_d16, x32_ldd_mhl_a, x33_inc_sp, x34_inc_mhl, x35_dec_mhl, x36_ld_mhl_d8, x37_scf, x38_jr_c_r8,   x39_add_hl_sp, x3a_ldd_a_mhl, x3b_dec_sp, x3c_inc_a, x3d_dec_a, x3e_ld_a_d8, x3f_ccf,
        x40_ld_b_b,   x41_ld_b_c,   x42_ld_b_d,   x43_ld_b_e,   x44_ld_b_h,   x45_ld_b_l,   x46_ld_b_mhl, x47_ld_b_a,   x48_ld_c_b, x49_ld_c_c, x4a_ld_c_d, x4b_ld_c_e, x4c_ld_c_h, x4d_ld_c_l, x4e_ld_c_mhl, x4f_ld_c_a,
        x50_ld_d_b,   x51_ld_d_c,   x52_ld_d_d,   x53_ld_d_e,   x54_ld_d_h,   x55_ld_d_l,   x56_ld_d_mhl, x57_ld_d_a,   x58_ld_e_b, x59_ld_e_c, x5a_ld_e_d, x5b_ld_e_e, x5c_ld_e_h, x5d_ld_e_l, x5e_ld_e_mhl, x5f_ld_e_a,
        x60_ld_h_b,   x61_ld_h_c,   x62_ld_h_d,   x63_ld_h_e,   x64_ld_h_h,   x65_ld_h_l,   x66_ld_h_mhl, x67_ld_h_a,   x68_ld_l_b, x69_ld_l_c, x6a_ld_l_d, x6b_ld_l_e, x6c_ld_l_h, x6d_ld_l_l, x6e_ld_l_mhl, x6f_ld_l_a,
        x70_ld_mhl_b, x71_ld_mhl_c, x72_ld_mhl_d, x73_ld_mhl_e, x74_ld_mhl_h, x75_ld_mhl_l, x76_halt,     x77_ld_mhl_a, x78_ld_a_b, x79_ld_a_c, x7a_ld_a_d, x7b_ld_a_e, x7c_ld_a_h, x7d_ld_a_l, x7e_ld_a_mhl, x7f_ld_a_a,
        x80_add_a_b, x81_add_a_c, x82_add_a_d, x83_add_a_e, x84_add_a_h, x85_add_a_l, x86_add_a_mhl, x87_add_a_a, x88_adc_a_b, x89_adc_a_c, x8a_adc_a_d, x8b_adc_a_e, x8c_adc_a_h, x8d_adc_a_l, x8e_adc_a_mhl, x8f_adc_a_a,
        x90_sub_b, x91_sub_c, x92_sub_d, x93_sub_e, x94_sub_h, x95_sub_l, x96_sub_mhl, x97_sub_a, x98_sbc_a_b, x99_sbc_a_c, x9a_sbc_a_d, x9b_sbc_a_e, x9c_sbc_a_h, x9d_sbc_a_l, x9e_sbc_a_mhl, x9f_sbc_a_a,
        xa0_and_b, xa1_and_c, xa2_and_d, xa3_and_e, xa4_and_h, xa5_and_l, xa6_and_mhl, xa7_and_a, xa8_xor_b, xa9_xor_c, xaa_xor_d, xab_xor_e, xac_xor_h, xad_xor_l, xae_xor_mhl, xaf_xor_a,
        xb0_or_b,  xb1_or_c,  xb2_or_d,  xb3_or_e,  xb4_or_h,  xb5_or_l,  xb6_or_mhl,  xb7_or_a,  xb8_cp_b, xb9_cp_c, xba_cp_d, xbb_cp_e, xbc_cp_h, xbd_cp_l, xbe_cp_mhl, xbf_cp_a,
        xc0_ret_nz,   xc1_pop_bc, xc2_jp_nz_a16, xc3_jp_a16, xc4_call_nz_a16, xc5_push_bc, xc6_add_a_d8, xc7_rst_00h, xc8_ret_z,       xc9_ret,      xca_jp_z_a16, xcb_prefix_cb, xcc_call_z_a16, xcd_call_a16, xce_adc_a_d8, xcf_rst_08h,
        xd0_ret_nc,   xd1_pop_de, xd2_jp_nc_a16, nullptr,    xd4_call_nc_a16, xd5_push_de, xd6_sub_d8,   xd7_rst_10h, xd8_ret_c,       xd9_reti,     xda_jp_c_a16, nullptr,       xdc_call_c_a16, nullptr,      xde_sbc_a_d8, xdf_rst_18h,
        xe0_ldh_m8_a, xe1_pop_hl, xe2_ld_mc_a,   nullptr,    nullptr,         xe5_push_hl, xe6_and_d8,   xe7_rst_20h, xe8_add_sp_r8,   xe9_jp_hl,    xea_ld_a16_a, nullptr,       nullptr,        nullptr,      xee_xor_d8,   xef_rst_28h,
        xf0_ldh_a_m8, xf1_pop_af, xf2_ld_a_mc,   xf3_di,     nullptr,         xf5_push_af, xf6_or_d8,    xf7_rst_30h, xf8_ld_hl_sp_r8, xf9_ld_sp_hl, xfa_ld_a_a16, xfb_ei,        nullptr,        nullptr,      xfe_cp_d8,    xff_rst_38h
};

const c8* instructionNames[256] = {
        "x00 NOP",
        "x01 LD BC, d16",
        "x02 LD (BC), A",
        "x03 INC BC",
        "x04 INC B",
        "x05 DEC B",
        "x06 LD B, d8",
        "x07 RLCA",
        "x08 LD (a16), SP",
        "x09 ADD HL, BC",
        "x0A LD A, (BC)",
        "x0B DEC BC",
        "x0C INC C",
        "x0D DEC C",
        "x0E LD C, d8",
        "x0F RRCA",
        "x10 STOP 0",
        "x11 LD DE, d16",
        "x12 LD (DE), A",
        "x13 INC DE",
        "x14 INC D",
        "x15 DEC D",
        "x16 LD D, d8",
        "x17 RLA",
        "x18 JR r8",
        "x19 ADD HL, DE",
        "x1A LD A, (DE)",
        "x1B DEC DE",
        "x1C INC E",
        "x1D DEC E",
        "x1E LD E, d8",
        "x1F RRA",
        "x20 JR NZ, r8",
        "x21 LD HL, d16",
        "x22 LD (HL+), A",
        "x23 INC HL",
        "x24 INC H",
        "x25 DEC H",
        "x26 LD H, d8",
        "x27 DAA",
        "x28 JR Z, r8",
        "x29 ADD HL, HL",
        "x2A LD A, (HL+)",
        "x2B DEC HL",
        "x2C INC L",
        "x2D DEC L",
        "x2E LD L, d8",
        "x2F CPL",
        "x30 JR NC, r8",
        "x31 LD SP, d16",
        "x32 LD (HL-), A",
        "x33 INC SP",
        "x34 INC (HL)",
        "x35 DEC (HL)",
        "x36 LD (HL), d8",
        "x37 SCF",
        "x38 JR C, r8",
        "x39 ADD HL, SP",
        "x3A LD A, (HL-)",
        "x3B DEC SP",
        "x3C INC A",
        "x3D DEC A",
        "x3E LD A, d8",
        "x3F CCF",
        "x40 LD B, B",
        "x41 LD B, C",
        "x42 LD B, D",
        "x43 LD B, E",
        "x44 LD B, H",
        "x45 LD B, L",
        "x46 LD B, (HL)",
        "x47 LD B, A",
        "x48 LD C, B",
        "x49 LD C, C",
        "x4A LD C, D",
        "x4B LD C, E",
        "x4C LD C, H",
        "x4D LD C, L",
        "x4E LD C, (HL)",
        "x4F LD C, A",
        "x50 LD D, B",
        "x51 LD D, C",
        "x52 LD D, D",
        "x53 LD D, E",
        "x54 LD D, H",
        "x55 LD D, L",
        "x56 LD D, (HL)",
        "x57 LD D, A",
        "x58 LD E, B",
        "x59 LD E, C",
        "x5A LD E, D",
        "x5B LD E, E",
        "x5C LD E, H",
        "x5D LD E, L",
        "x5E LD E, (HL)",
        "x5F LD E, A",
        "x60 LD H, B",
        "x61 LD H, C",
        "x62 LD H, D",
        "x63 LD H, E",
        "x64 LD H, H",
        "x65 LD H, L",
        "x66 LD H, (HL)",
        "x67 LD H, A",
        "x68 LD L, B",
        "x69 LD L, C",
        "x6A LD L, D",
        "x6B LD L, E",
        "x6C LD L, H",
        "x6D LD L, L",
        "x6E LD L, (HL)",
        "x6F LD L, A",
        "x70 LD (HL), B",
        "x71 LD (HL), C",
        "x72 LD (HL), D",
        "x73 LD (HL), E",
        "x74 LD (HL), H",
        "x75 LD (HL), L",
        "x76 HALT",
        "x77 LD (HL), A",
        "x78 LD A, B",
        "x79 LD A, C",
        "x7A LD A, D",
        "x7B LD A, E",
        "x7C LD A, H",
        "x7D LD A, L",
        "x7E LD A, (HL)",
        "x7F LD A, A",
        "x80 ADD A, B",
        "x81 ADD A, C",
        "x82 ADD A, D",
        "x83 ADD A, E",
        "x84 ADD A, H",
        "x85 ADD A, L",
        "x86 ADD A, (HL)",
        "x87 ADD A, A",
        "x88 ADC A, B",
        "x89 ADC A, C",
        "x8A ADC A, D",
        "x8B ADC A, E",
        "x8C ADC A, H",
        "x8D ADC A, L",
        "x8E ADC A, (HL)",
        "x8F ADC A, A",
        "x90 SUB B",
        "x91 SUB C",
        "x92 SUB D",
        "x93 SUB E",
        "x94 SUB H",
        "x95 SUB L",
        "x96 SUB (HL)",
        "x97 SUB A",
        "x98 SBC A, B",
        "x99 SBC A, C",
        "x9A SBC A, D",
        "x9B SBC A, E",
        "x9C SBC A, H",
        "x9D SBC A, L",
        "x9E SBC A, (HL)",
        "x9F SBC A, A",
        "xA0 AND B",
        "xA1 AND C",
        "xA2 AND D",
        "xA3 AND E",
        "xA4 AND H",
        "xA5 AND L",
        "xA6 AND (HL)",
        "xA7 AND A",
        "xA8 XOR B",
        "xA9 XOR C",
        "xAA XOR D",
        "xAB XOR E",
        "xAC XOR H",
        "xAD XOR L",
        "xAE XOR (HL)",
        "xAF XOR A",
        "xB0 OR B",
        "xB1 OR C",
        "xB2 OR D",
        "xB3 OR E",
        "xB4 OR H",
        "xB5 OR L",
        "xB6 OR (HL)",
        "xB7 OR A",
        "xB8 CP B",
        "xB9 CP C",
        "xBA CP D",
        "xBB CP E",
        "xBC CP H",
        "xBD CP L",
        "xBE CP (HL)",
        "xBF CP A",
        "xC0 RET NZ",
        "xC1 POP BC",
        "xC2 JP NZ, a16",
        "xC3 JP a16",
        "xC4 CALL NZ, a16",
        "xC5 PUSH BC",
        "xC6 ADD A, d8",
        "xC7 RST 00H",
        "xC8 RET Z",
        "xC9 RET",
        "xCA JP Z, a16",
        "xCB PREFIX CB",
        "xCC CALL Z, a16",
        "xCD CALL a16",
        "xCE ADC A, d8",
        "xCF RST 08H",
        "xD0 RET NC",
        "xD1 POP DE",
        "xD2 JP NC, a16",
        "xD3 NULL",
        "xD4 CALL NC, a16",
        "xD5 PUSH DE",
        "xD6 SUB d8",
        "xD7 RST 10H",
        "xD8 RET C",
        "xD9 RETI",
        "xDA JP C, a16",
        "xDB NULL",
        "xDC CALL C, a16",
        "xDD NULL",
        "xDE SBC A, d8",
        "xDF RST 18H",
        "xE0 LDH (a8), A",
        "xE1 POP HL",
        "xE2 LD (C), A",
        "xE3 NULL",
        "xE4 NULL",
        "xE5 PUSH HL",
        "xE6 AND d8",
        "xE7 RST 20H",
        "xE8 ADD SP, r8",
        "xE9 JP (HL)",
        "xEA LD (a16), A",
        "xEB NULL",
        "xEC NULL",
        "xED NULL",
        "xEE XOR d8",
        "xEF RST 28H",
        "xF0 LDH A, (a8)",
        "xF1 POP AF",
        "xF2 LD A, (C)",
        "xF3 DI",
        "xF4 NULL",
        "xF5 PUSH AF",
        "xF6 OR d8",
        "xF7 RST 30H",
        "xF8 LD HL, SP+r8",
        "xF9 LD SP, HL",
        "xFA LD A, (a16)",
        "xFB EI",
        "xFC NULL",
        "xFD NULL",
        "xFE CP d8",
        "xFF RST 38H"
};

const c8* GetOpcodeName(u8 opcode)
{
    return instructionNames[opcode];
}