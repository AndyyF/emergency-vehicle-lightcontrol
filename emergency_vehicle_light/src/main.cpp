///////////////////////////////
// emergency_vehicle_light   //
// author: AndyyF            //
// date:   November 11. 2022 //
///////////////////////////////

// ****** timing table for one pin group ******
//
// Scheinwerfer + Rücklichter: +3,3 V
//
// flashlight 1 & 2
//       : _____            _____                                                                       : _____            _____
// LED1  :|     |          |     |                                                                      :|     |          |     |
//       :|  #  |    2#    |  #  |                                   14#                                :|  #  |    2#    |  #  |                  14#
//   ____:|     |__________|     |______________________________________________________________________:|     |__________|     |___________________________
//       :                                              _____            _____                          :                                               : _____
// LED2  :                                             |     |          |     |                         :                                               :|     |
//       :                  9#                         |  #  |     2#   |  #  |                         :          14#                                  :|  #  |
//   ____:_____________________________________________|     |__________|     |_________________________:_______________________________________________:|     |_
//       :                                                                                              :
// blue-light-bar 1 & hazard light 1:                                                                   :
//       : ______________________________                                ______________________________ :
// LED3  :|                              |                              |                              |:
//       :|              6#              |              6#              |              6#              |:
//   ____:|                              |______________________________|                              |:______________________________
//       :                                                                                              :
// blue-light-bar 2 & hazard light 2:                                                                   :
//   ____:                                ______________________________                                :______________________________
// LED4  :|                              |                              |                              |:                              |
//       :|              6#              |              6#              |              6#              |:              6#              |
//       :|______________________________|                              |______________________________|:                              |
//
// Legend:     #   stands for  50ms
//            2#   stands for 100ms
//            6#   stands for 300ms
//            9#   stands for 450ms
//           14#   stands for 700ms

#include <Arduino.h>
#include "soc/rtc_wdt.h"
#include "esp_task_wdt.h"

// #define DEBUG

#define totalGroups 4

#ifdef DEBUG
u_int8_t cnt_flashlight[totalGroups];
u_int8_t cnt_bar[totalGroups];
#endif // DEBUG

#define g0LED1 19 // pins for emergency light group 0
#define g0LED2 20
#define g0LED3 21
#define g0LED4 34

#define g1LED1 35 // pins for emergency light group 1
#define g1LED2 36
#define g1LED3 37
#define g1LED4 38

#define g2LED1 17 // pins for emergency light group 2
#define g2LED2 16
#define g2LED3 15
#define g2LED4 14

#define g3LED1 13 // pins for emergency light group 3
#define g3LED2 12
#define g3LED3 11
#define g3LED4 10

hw_timer_t *Timer0 = NULL; // pointer for the timers
hw_timer_t *Timer1 = NULL;
hw_timer_t *Timer2 = NULL;
hw_timer_t *Timer3 = NULL;

void IRAM_ATTR onTimer_g0(); // function prototype
void IRAM_ATTR onTimer_g1();
void IRAM_ATTR onTimer_g2();
void IRAM_ATTR onTimer_g3();

void ISR_handler(u_int8_t timer);

u_int16_t brightness = 4094;
u_int16_t brightness2 = brightness;
u_int16_t brightness3_inital = brightness;
u_int16_t brightness4_inital = brightness;

#if (totalGroups == 4)

u_int16_t brightness3[totalGroups][2] = {{brightness3_inital, 0},
                                         {brightness3_inital, 0},
                                         {brightness3_inital, 0},
                                         {brightness3_inital, 0}};

#elif (totalGroups == 3)

u_int16_t brightness3[totalGroups][2] = {{brightness3_inital, 0},
                                         {brightness3_inital, 0},
                                         {brightness3_inital, 0}};

#elif (totalGroups == 2)

u_int16_t brightness3[totalGroups][2] = {{brightness3_inital, 0},
                                         {brightness3_inital, 0}};

#elif (totalGroups == 1)

u_int16_t brightness3[totalGroups][2] = {{brightness3_inital, 0}};

#endif

u_int8_t pin_number[16] = {
    g0LED1, // group 0
    g0LED2,
    g0LED3,
    g0LED4,
    g1LED1, // group 1
    g1LED2,
    g1LED3,
    g1LED4,
    g2LED1, // group 2
    g2LED2,
    g2LED3,
    g2LED4,
    g3LED1, // group 3
    g3LED2,
    g3LED3,
    g3LED4}; // pin numbers in the order: g0, g1, g2 & g3

void swap_values(u_int16_t *array);

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
  {
  };
#endif // DEBUG

  for (u_int8_t i = 0; i < totalGroups * 4; i++)
  {
    ledcSetup(i, 9000, 12); // setup PWM: f = 9 kHz, resolution 12 Bit
    ledcAttachPin(pin_number[i], i);
    ledcWrite(i, 0);
    delay(5);

#ifdef DEBUG
    Serial.println((String) "PWM-channel: " + i + "\t regards to pin: " + pin_number[i]);
#endif // DEBUG
  }

#if (totalGroups == 4)

  Timer0 = timerBegin(0, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer0, &onTimer_g0, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer0, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer0);                        // Enable Timer

  delay(250); // delay between groups

  Timer1 = timerBegin(1, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer1, &onTimer_g1, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer1, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer1);                        // Enable Timer

  delay(150); // delay between groups

  Timer2 = timerBegin(2, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer2, &onTimer_g2, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer2, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer2);                        // Enable Timer

  delay(200); // delay between groups

  Timer3 = timerBegin(3, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer3, &onTimer_g3, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer3, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer3);                        // Enable Timer

#elif (totalGroups == 3)

  Timer0 = timerBegin(0, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer0, &onTimer_g0, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer0, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer0);                        // Enable Timer

  delay(250); // delay between groups

  Timer1 = timerBegin(1, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer1, &onTimer_g1, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer1, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer1);                        // Enable Timer

  delay(150); // delay between groups

  Timer2 = timerBegin(2, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer2, &onTimer_g2, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer2, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer2);                        // Enable Timer

#elif (totalGroups == 2)

  Timer0 = timerBegin(0, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer0, &onTimer_g0, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer0, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer0);                        // Enable Timer

  delay(250); // delay between groups

  Timer1 = timerBegin(1, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer1, &onTimer_g1, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer1, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer1);                        // Enable Timer

#elif (totalGroups == 1)

  Timer0 = timerBegin(0, 80, true);                // timerBegin(timer number, CLK divider, counter mode)
  timerAttachInterrupt(Timer0, &onTimer_g0, true); // timerAttachInterrupt(ISR-function, time in us (-> CLK divider), edge)
  timerAlarmWrite(Timer0, 50000, true);            // timerAlarmWrite(hw_timer_t *timer, uint64_t alarm_value, bool autoreload)
  timerAlarmEnable(Timer0);                        // Enable Timer

#endif
}

void loop()
{
#ifdef DEBUG
  for (u_int8_t i = 0; i < totalGroups; i++)
  {
    Serial.println((String) "timer: " + i + "\tcnt-flasch-value: " + cnt_flashlight[i] + "\tcnt-bar-value: " + cnt_bar[i]);
  }
#endif // DEBUG
}

void IRAM_ATTR onTimer_g0()
{
  ISR_handler(0);
}

void IRAM_ATTR onTimer_g1()
{
  ISR_handler(1);
}

void IRAM_ATTR onTimer_g2()
{
  ISR_handler(2);
}

void IRAM_ATTR onTimer_g3()
{
  ISR_handler(3);
}

void ISR_handler(u_int8_t timer)
{
#ifndef DEBUG
  static u_int8_t cnt_flashlight[totalGroups];
  static u_int8_t cnt_bar[totalGroups];
#endif // DEBUG

  switch (cnt_flashlight[timer])
  {
  case 0:
    ledcWrite(timer * 4, brightness);
    break;

  case 1:
    ledcWrite(timer * 4, 0);
    break;

  case 3:
    ledcWrite(timer * 4, brightness);
    break;

  case 4:
    ledcWrite(timer * 4, 0);
    break;

  case 7:
    ledcWrite(timer * 4 + 1, brightness2);
    break;

  case 8:
    ledcWrite(timer * 4 + 1, 0);
    break;

  case 10:
    ledcWrite(timer * 4 + 1, brightness2);
    break;

  case 11:
    ledcWrite(timer * 4 + 1, 0);
    break;

  default:
    break;
  }

  if (cnt_bar[timer] == 0 || cnt_bar[timer] == 6)
  {
    ledcWrite(timer * 4 + 2, brightness3[timer][1]);
    ledcWrite(timer * 4 + 3, brightness3[timer][2]);

    swap_values(&brightness3[timer][1]);
  }

  cnt_flashlight[timer]++;
  cnt_bar[timer]++;

  if (cnt_flashlight[timer] > 17)
  {
    cnt_flashlight[timer] = 0;
  }

  if (cnt_bar[timer] >= 11)
  {
    cnt_bar[timer] = 0;
  }
}

void swap_values(u_int16_t *array)
{
  u_int16_t tmp = array[0];

  array[0] = array[1];
  array[1] = tmp;
}