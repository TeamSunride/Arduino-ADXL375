#include <Arduino.h>
#include "ADXL375.h"

ADXL375 sensor(&Wire, 1000000);


void setup() {
  // put your setup code here, to run once:
  Serial. begin(9600);
  sensor.begin();
  sensor.default_configuration();
}

void loop() {
    byte id = sensor.who_am_i(); // put your main code here, to run repeatedly:
    if (sensor.who_am_i() == 0xE5) {
        printf("ADXL found!");

    }
    else{
        printf("Mission abort!*static*");
    }

    uint8_t Standby = sensor.set_standby_mode(0);


    uint8_t  Measure = sensor.set_standby_mode(1);
    uint8_t MSB_mode = sensor.MSB_or_LSB(1);
    uint8_t Data_Rate = sensor.set_DATA_RATE(ODR_800_HZ);
    Vector<double, 3> acceleration = sensor.read_acc_data();

}