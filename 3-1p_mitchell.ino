#include "TIMER_DEAKIN.h"
#include "GPIO_DEAKIN.h"

GPIO_DEAKIN myGpio;
TIMER_DEAKIN myTimer;
bool isOn;
const uint FLASH_TIME = 50000; //Flash times in 10ths of a millisecond
double iTimer; //tracks the timers current time in 10ths of a millisecond

void setup() {
  Serial.begin(9600);
  myGpio.Config_GPIO('A', 17, OUTPUT);
  // iTimer = 0; // init to 0
}

void loop() {
  Serial.println(1);
  myGpio.Write_GPIO('A', 17, HIGH);
  isOn = myGpio.Read_GPIO('A', 17);
  
  bool finished = myTimer.wait(FLASH_TIME);
  Serial.println(finished);
  myGpio.Write_GPIO('A', 17, LOW);
  isOn = myGpio.Read_GPIO('A', 17);
  myTimer.wait(FLASH_TIME);
}

