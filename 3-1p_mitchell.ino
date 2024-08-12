#include "TIMER_DEAKIN.h"
#include "GPIO_DEAKIN.h"

GPIO_DEAKIN myGpio;
TIMER_DEAKIN myTimer;
bool isOn;
uint32_t FLASH_TIME; //Flash times in 10ths of a millisecond
double iTimer; //tracks the timers current time in 10ths of a millisecond

void setup() {
  Serial.begin(9600);
  myGpio.Config_GPIO('A', 17, OUTPUT);
  FLASH_TIME = 65005;
}

void loop() {
  myGpio.Write_GPIO('A', 17, HIGH);
  isOn = myGpio.Read_GPIO('A', 17);
  myTimer.wait(FLASH_TIME);
  myGpio.Write_GPIO('A', 17, LOW);
  isOn = myGpio.Read_GPIO('A', 17);
  myTimer.wait(FLASH_TIME);
}
