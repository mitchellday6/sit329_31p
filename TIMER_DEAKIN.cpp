#include "TIMER_DEAKIN.h"
#include "wiring_private.h"
#include <stdint.h>

volatile uint16_t count3 = 0, count4 = 0, count5 = 0;
uint16_t endCount3 = 0, endCount4 = 0, endCount5 = 0;

TIMER_DEAKIN::TIMER_DEAKIN() : count3(0), count4(0), count5(0), endCount3(0), endCount4(0), endCount5(0)
{
    
}

// Configures specific PIN on PORT to OUTPUT/INPUT
bool TIMER_DEAKIN::config_timer(int timerNo, int countStart, int countEnd, int timerresolution)
{
    TcCount16 *TC;
    uint timerId;
    Serial.println(timerNo);

    switch (timerNo)
    {
    case 3:
        TC = (TcCount16 *) TC3;
        PM->APBCMASK.reg |= PM_APBCMASK_TC3;
        timerId = GCLK_CLKCTRL_ID_TCC2_TC3;
        NVIC_EnableIRQ(TC3_IRQn);
        count3 = countStart;
        endCount3 = countEnd;
        Serial.println(count3);
        Serial.println(endCount3);
        break;
    case 4:
        TC = (TcCount16 *) TC4;
        PM->APBCMASK.reg |= PM_APBCMASK_TC4;
        timerId = GCLK_CLKCTRL_ID_TC4_TC5;
        NVIC_EnableIRQ(TC4_IRQn);
        count4 = countStart;
        endCount4 = countEnd;
        break;
    case 5:
        TC = (TcCount16 *) TC5;
        PM->APBCMASK.reg |= PM_APBCMASK_TC5;
        timerId = GCLK_CLKCTRL_ID_TC4_TC5;
        NVIC_EnableIRQ(TC5_IRQn);
        count5 = countStart;
        endCount5 = countEnd;
        break;
    default:
        return false;
        break;
    }

    //set clock values
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) | GCLK_GENDIV_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY);

    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY);

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4 | timerId;
    while (GCLK->STATUS.bit.SYNCBUSY);

    //reset the selected timer
    TC->CTRLA.reg = TC_CTRLA_SWRST;
    while (TC->STATUS.bit.SYNCBUSY);

    TC->CTRLA.reg = TC_CTRLA_MODE_COUNT16;
    while (TC->STATUS.bit.SYNCBUSY);

    TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_NFRQ;
    while (TC->STATUS.bit.SYNCBUSY);

    TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV256;
    while (TC->STATUS.bit.SYNCBUSY);

    uint scalar = 256;
    float freq = 48000000.0;
    float interval = scalar/freq;
    uint16_t compareVal = round(timerresolution/interval);


    TC->COUNT.reg = 0;
    TC->CC[0].reg = compareVal;
    while (TC->STATUS.bit.SYNCBUSY);

    TC->INTENSET.reg = 0;
    TC->INTENSET.bit.MC0 = 1;

    TC->CTRLA.reg |= TC_CTRLA_ENABLE;
}

uint TIMER_DEAKIN::getTC3_count()
{
    return count3;
};
uint TIMER_DEAKIN::getTC4_count()
{
    return count4;
};
uint TIMER_DEAKIN::getTC5_count()
{
    return count5;
};

// input waitTime is in 10ths of a millisecond
bool TIMER_DEAKIN::wait(uint waitTime) {
    uint resolution = 1; //10ths of ms
    
    bool configured = config_timer(3, 0, waitTime/resolution, resolution);
    if(!configured){
        Serial.println("Bad Timer Configuration");
        return false;
    }

    while(getTC3_count() < waitTime){
        
    }
    return true;
}

void TC3_Handler() {

    Serial.println(TC3->COUNT16.INTFLAG.bit.MC0);
    if (TC3->COUNT16.INTFLAG.bit.MC0) {
        TC3->COUNT16.INTFLAG.bit.MC0 = 1; // Clear the interrupt flag
        if(count3 == endCount3){
            count3 = 0;
        } else {
            count3++;
        }
    }
}

void TC4_Handler() {
    if (TC4->COUNT16.INTFLAG.bit.MC0) {
        TC4->COUNT16.INTFLAG.bit.MC0 = 1; // Clear the interrupt flag
        if(count4 == endCount4){
            count4 = 0;
        } else {
            count4++;
        }
    }
}

void TC5_Handler() {
    if (TC5->COUNT16.INTFLAG.bit.MC0) {
        TC5->COUNT16.INTFLAG.bit.MC0 = 1; // Clear the interrupt flag
        if(count5 == endCount5){
            count5 = 0;
        } else {
            count5++;
        }
    }
}