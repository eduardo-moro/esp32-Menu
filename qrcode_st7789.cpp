#include <Arduino.h>
#include "qrencode.h"
#include "qrcode_st7789.h"


QRcode_ST7789::QRcode_ST7789(Adafruit_ST7789 *display) {
    this->display = display;
}


void QRcode_ST7789::init(int rotation, int width, int height) {
    display->setRotation(rotation);
    this->screenwidth = width;
    this->screenheight = height;
    display->fillScreen(ST77XX_WHITE);
    int min = width;
    if (height<width)
        min = height;
    multiply = min/WD;
    offsetsX = (display->width()-(WD*multiply))/2;
    offsetsY = (display->height()-(WD*multiply))/2;
}

void QRcode_ST7789::screenwhite() {
    display->fillScreen(ST77XX_WHITE);
}

void QRcode_ST7789::screenupdate() {
    // No hay que hacer nada
}

void QRcode_ST7789::drawPixel(int x, int y, int color) {
    if(color==1) {
        color = ST77XX_BLACK;
    } else {
        color = ST77XX_WHITE;
    }
    display->fillRect(x,y,multiply,multiply,color);
}
