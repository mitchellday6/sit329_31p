#pragma once

#ifndef TIMER_DEAKIN_H
#define TIMER_DEAKIN_H

#include <Arduino.h>


class TIMER_DEAKIN {
    private:
        //current count value of each timer
        uint16_t count3;
        uint16_t count4;
        uint16_t count5;
        //the last count value before reset
        uint16_t endCount3;
        uint16_t endCount4;
        uint16_t endCount5;
    public:
        TIMER_DEAKIN();
        bool config_timer(int timerNo, int countStart, int countEnd, int timerResolution);
        uint getTC3_count();
        uint getTC4_count();
        uint getTC5_count();
        bool wait(uint waitTime);
};

#endif