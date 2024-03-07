int state = 2; int a = 0;
#define REPORTING_PERIOD_MS     1000
uint32_t tsLastReport = 0;

String ssid, pass, uid;

bool connection_status = false;
bool offline_mode = false;

unsigned long lastUpload = 0;
int counter = 1;

#include <PNGdec.h>
#include "Image.h" // Image is stored here in an 8 bit array

PNG png; // PNG decoder inatance

#include <SPI.h>
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

PulseOximeter pox;

#include "driver/rtc_io.h"
#include "Network.h"
#include "Fuzzy.h"

void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[240];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(57, 5 + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

void boot_init(){
  // tft.setFreeFont(&FreeSans9pt7b);
  tft.fillScreen(0x000000);
  int16_t rc = png.openFLASH((uint8_t *)logo, sizeof(logo), pngDraw);
  if (rc == PNG_SUCCESS) {
    tft.startWrite();
    rc = png.decode(NULL, 0);
    tft.endWrite();
  }
  delay(1500);
}
