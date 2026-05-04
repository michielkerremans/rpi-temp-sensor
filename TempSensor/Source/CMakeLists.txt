cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(TempSensor)

add_executable(TempSensor main.c gpio/gpio.c gpio/gpiod.c i2c/i2c1.c i2c/tc74.c util/logger.c mqtt/mqtt.c)

target_link_libraries(TempSensor PJ_RPI gpiod paho-mqtt3c)
