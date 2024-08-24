/**
  ******************************************************************************
  * @file           : RTC.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/24
  ******************************************************************************
  */



#include <cstring>
#include "RTC.h"


void RTC::init()
{
    memset(this, 0, sizeof(RTC));
}

void RTC::update(f64 delta_time)
{
    if(!halted())
    {
        time += delta_time;
        if(!time_latched)
        {
            update_time_registers();
        }
    }
}

void RTC::update_time_registers()
{
    s = ((u64)time) % 60;
    m = (((u64)time) / 60) % 60;
    h = (((u64)time) / 3600) % 24;
    u16 days = (u16)(((u64)time) / 86400);
    dl = (u8)(days & 0xFF);
    if(days & 0x100) bitSet(&dh, 0);
    else bitReset(&dh, 0);
    if(days >= 512) bitSet(&dh, 7);
    else bitReset(&dh, 7);
}

void RTC::update_timestamp()
{
    time = s + ((u64)m) * 60 + ((u64)h) * 3600 + ((u64)days()) * 86400;
    if(day_overflow())
    {
        time += 86400 * 512;
    }
}
void RTC::latch()
{
    if(!time_latched)
    {
        time_latched = true;
    }
    else
    {
        time_latched = false;
        update_time_registers();
    }
}