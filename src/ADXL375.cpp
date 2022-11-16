#include "ADXL375.h"
ADXL375::ADXL375(TwoWire *pipe, uint32_t freq) { // constructor for I2C protocol
    device = new I2CProtocol(ADXL375_DEFAULT_I2C_ADDRESS, pipe, freq);
    acc_conversion_factor = 0.00981 * 49;
}

ADXL375::ADXL375(byte chipSelect, SPIClass& spi, uint32_t freq){
SPISettings settings = SPISettings(freq, MSBFIRST, SPI_MODE3);
    device = new SPIProtocol(chipSelect, spi, settings, READ_BYTE, WRITE_BYTE);
    acc_conversion_factor = 0.00981 * 49;
}

uint8_t ADXL375::set_fifo_mode(FIFO_MODES mode) { //
    byte data = device->read_reg(ADXL375_REGISTER::FIFO_CTL);
    data = data & 0b01111000; // bit 3 must be zero, clear bottom 3 bits
    data = mode | data;
    return device->write_reg(ADXL375_REGISTER::FIFO_CTL, data);
}
uint8_t ADXL375::set_spi_mode(bool enable) { // 1 for 3-wire spi and 0 for 4 wire spi
    byte spi_wires = device->read_reg(ADXL375_REGISTER::DATA_FORMAT);
    setBit(&spi_wires, 7, enable);
    return device->write_reg(ADXL375_REGISTER::DATA_FORMAT, spi_wires);
}

uint8_t ADXL375::set_INTERRUPT(int bit_num,bool set_pin, INTERRUPTS interrupt, bool enable) {

    byte map_data = device->read_reg(ADXL375_REGISTER::INT_MAP) ;
    setBit(&map_data, bit_num, set_pin);//bit num chooses which bit we wanna alter, set pin is 0 or 1 based on interrupt pin you wanna select.
    int a =  device->write_reg(ADXL375_REGISTER::INT_MAP, map_data);
    byte data = device->read_reg(ADXL375_REGISTER::INT_ENABLE);
    setBit(&data, interrupt, enable);
    int b = device->write_reg(ADXL375_REGISTER::INT_ENABLE, data);
    return a | b;
//A setting of 1 for any bit in the INT_ENABLE register enables the specified function to generate interrupts;
// A setting of 0 for any bit in this register prevents the function from generating interrupts
}


uint8_t ADXL375::set_low_DATA_RATE(Low_power_ODR rate){

    byte Rate_data = device->read_reg(ADXL375_REGISTER::BW_RATE);
    //setBit(&Rate_data,5,1);
    Rate_data = Rate_data & 0b00010000; // setting bit 5 here to 1 to enable low power mode. there is no need for the above commented line right?
    Rate_data = rate | Rate_data;
    return device->write_reg(ADXL375_REGISTER::BW_RATE, Rate_data);
}

uint8_t ADXL375::set_DATA_RATE(ODR rate){

    byte Rate_data = device->read_reg(ADXL375_REGISTER::BW_RATE);
    //setBit(&Rate_data,5,1);
    Rate_data = Rate_data & 0b00000000; // setting bit 5 here to 0 to disable low power mode. there is no need for the above commented line right?
    Rate_data = rate | Rate_data;
    return device->write_reg(ADXL375_REGISTER::BW_RATE, Rate_data);
}

//USURE HOW TO GO FORWARD
// uint8_t ADXL375::set_autosleep_mode(bool enable){
//     byte Rate_data = device->read_reg(ADXL375_REGISTER::);
//     setBit(&Rate_data,5,1);
//     Rate_data = Rate_data & 0b00000000; // setting bit 5 here to 0 to disable low power mode. there is no need for the above commented line right?
//     Rate_data = rate | Rate_data;
//     return device->write_reg(ADXL375_REGISTER::BW_RATE, Rate_data);
// }

uint8_t ADXL375::set_standby_mode(bool enable){//0 for setting to standby. Sets measure bit in powerctl register to 0 to put it into standby mode
    byte Measure_bit = device->read_reg(ADXL375_REGISTER::POWER_CTL);
    setBit(&Measure_bit , 4, enable);
    return device->write_reg(ADXL375_REGISTER::BW_RATE, Measure_bit);
}


//uint8_t ADXL375::Self_test(bool enable, Self_test_ODR rate){//1 for self testing. the SELF_TEST bit in the DATA_FORMAT register, Address 0x31 is set to 1
//
////     byte Self_test_2 = device->read_reg(ADXL375_REGISTER::BW_RATE);
////     Self_test_2 = Self_test_2 & 0b00000000; // setting bit 5 here to 0 to disable low power mode. there is no need for the above commented line right?
////     Self_test_2 = rate | Self_test_2;
////     return device->write_reg(ADXL375_REGISTER::BW_RATE, Self_test_2);
//
//    byte Self_test = device->read_reg(ADXL375_REGISTER::DATA_FORMAT);
//    setBit(&Self_test , 7, enable); //Should i do set bit or do bit manipulation to ensure all the other bits remain the same while changing only this bit
//     return device->write_reg(ADXL375_REGISTER::DATA_FORMAT, Self_test);
// }

uint8_t ADXL375::ac_coupled_mode(bool enable){//0 for setting to standby. Sets measure bit in powerctl register to 0 to put it into standby mode
    byte Measure_bit = device->read_reg(ADXL375_REGISTER::POWER_CTL);
    setBit(&Measure_bit , 4, enable);
    return device->write_reg(ADXL375_REGISTER::BW_RATE, Measure_bit);
}

uint8_t ADXL375::default_configuration() {
    set_low_DATA_RATE(Low_ODR_400_HZ);
    set_standby_mode(0);
}

byte ADXL375::who_am_i() {
    return device->read_reg(ADXL375_REGISTER::DEVID);
}
uint8_t ADXL375::MSB_or_LSB(bool enable){//1 is for MSB and 0 is LSB
    byte msb_or_lsb = device->read_reg(ADXL375_REGISTER::DATA_FORMAT);
    setBit(&msb_or_lsb, 3, enable);
    return device->write_reg(ADXL375_REGISTER::DATA_FORMAT, msb_or_lsb);
}

Vector<double, 3> ADXL375::read_acc_data(){

    Vector<double, 3> acceleration = {0};
    byte data[6]= {};
    device->read_regs(ADXL375_REGISTER::DATAX0, data, 6);

    acceleration[0] =  ((double)((data[1]<<8)|data[0]))* acc_conversion_factor;
    acceleration[1] = ((double)((data[3]<<8)|data[2]))* acc_conversion_factor;
    acceleration[2] =  ((double)((data[5]<<8)|data[4]))* acc_conversion_factor;
    //acc_fifo.push(Vector<double, 3> {x_axis,_axis,z_axis});

    return acceleration;
}




