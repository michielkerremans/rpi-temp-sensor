cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(TempSensor)

add_executable(TempSensor main.c gpio.c gpiod.c logger.c)

target_link_libraries(TempSensor PJ_RPI gpiod)
