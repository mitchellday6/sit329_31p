// timer_deakin.h
#ifndef TIMER_DEAKIN_H
#define TIMER_DEAKIN_H

#include <Arduino.h>

class TIMER_DEAKIN
{
private:
    volatile uint32_t tick_count; // Track the number of ticks (32bit as this could be a large number)
    volatile uint16_t tcNo; // the selected timer number
    TcCount16* TC; // the selected timer number
    volatile uint16_t res; //resolution of counter
    volatile uint32_t endTick; //the end tick (32bit as this could be a large number)

public:
    bool config_timer(uint8_t timer_num, uint16_t start_val, uint32_t end_val, uint16_t resolution);
    uint16_t getTC_count();
    void wait(uint32_t period_in_0_1ms);
};

#endif
