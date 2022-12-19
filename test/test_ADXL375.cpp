//
// Created by Tom Danvers on 19/12/2022.
//

#include <unity.h>
#include "ADXL375.h"

ADXL375 adxl = ADXL375(3, SPI, 5e6);

void setUp() {
    Serial.begin(115200);
    Serial.println("Test starting");

    adxl.begin();
}

void tearDown() {
    // clean stuff up here
}

/* -------------------------------- Tests -------------------------------- */

void test_device_id() {
    uint8_t address = adxl.who_am_i();
    Serial.printf("Device ID: %X (should be E5 for ADXL375)\n", address);
    TEST_ASSERT(address == 0xE5);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_device_id);
    UNITY_END();
    return 0;
}