// timer_deakin.cpp

#include "timer_deakin.h"
#include <Arduino.h>
#include "sam.h"

bool TIMER_DEAKIN::config_timer(uint8_t timer_num, uint16_t start_val, uint32_t end_val, uint16_t resolution) {
    if (timer_num < 3 || timer_num > 5) {
        return false; // Only TC3, TC4, and TC5 are valid for SAMD21G18A
    }

    tick_count = start_val;
    tcNo = timer_num;
    res = resolution;
    endTick = end_val;

    //choose the correct Timer and power values
    switch (timer_num) {
        case 3: 
            TC = (TcCount16*) TC3;
            PM->APBCMASK.reg |= PM_APBCMASK_TC3; 
            break;
        case 4: 
            TC = (TcCount16*) TC4; 
            PM->APBCMASK.reg |= PM_APBCMASK_TC4;
            break;
        case 5: 
            TC = (TcCount16*) TC5; 
            PM->APBCMASK.reg |= PM_APBCMASK_TC5;
            break;
        default: 
            return false; // Invalid timer number
    }

  
    // Configure GCLK for the timer
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(GCM_TC4_TC5 + (timer_num - 4)) | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY);


    //msTcStep is the number of milliseconds in a single timer tick after prescalar
    const float msTcStep = 64.0f/48000.0f;

    //stepDivider is the number of timer ticks it takes to reach the step resolution value
    const uint16_t stepDivider = round(res/(msTcStep*10));

    //this finds how many times the tick step value divides into 65535 (max compare)
    //it then gets the floor of that so the compare value is divisible by step and less then 16bit
    uint16_t compare = stepDivider*(floor(65535/stepDivider));

    //set the COUNT and compare registers for the timer
    TC->COUNT.reg = start_val;
    TC->CC[0].reg = compare;
    while (TC->STATUS.bit.SYNCBUSY);

    //configure timer for 16 bit with a prescalar of 64
    TC->CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV64;
    TC->CTRLA.reg |= TC_CTRLA_ENABLE;
    while (TC->STATUS.bit.SYNCBUSY);

    return true;
}

//method returns the current timer wrapper count number
uint16_t TIMER_DEAKIN::getTC_count() {
    return tick_count;
}

//waits for a period of time defined by the input in 10ths of a millisecond
void TIMER_DEAKIN::wait(uint32_t period_in_0_1ms) {

    uint16_t start_val = 0;
    uint16_t resolution = 1; //in 10ths of a millisecond
    uint32_t end_val = period_in_0_1ms/resolution; //total ticks to wait
    const float msTcStep = 64.0f/48000.0f;

    const uint16_t stepDivider = round(resolution/(10*msTcStep));

    bool configured = config_timer(4, start_val, end_val, resolution); // Using timer 3 for wait
    
    unsigned long startMillis = millis();

    //will perform tick increment while the tick_count is still less the the endTick value
    while(tick_count < endTick){
        if(TC->COUNT.reg >= stepDivider){
            tick_count++; //increment tick count
            TC->COUNT.reg = 0; //set the timers register count back to 0
        }
    }

    //used for testing timing functionality
    unsigned long elapsedMillis = millis() - startMillis;
    Serial.print("Elapsed Time (ms): "); Serial.println(elapsedMillis);
    tick_count = 0;
}
