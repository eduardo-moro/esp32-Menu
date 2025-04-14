cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS
    components
    components/arduino
    lib/Adafruit_BusIO
    lib/Adafruit_GFX_Library
    lib/PubSubClient
    lib/QRcodeDisplay
    lib/Adafruit_ST7789
    lib/Adafruit_ST77xx
    lib/qrcode_st7789
)

project(hello)