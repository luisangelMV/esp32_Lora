
#include "Arduino.h"
#include "LoRa_E32.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Fonts/FreeMonoBold12pt7b.h>
#include <SPI.h>

// LoRa_E32 e32ttl(&Serial2, 18, 21, 19); //  RX AUX M0 M1
LoRa_E32 e32ttl(&Serial2, 18, 14, 15); //  RX AUX M0 M1

unsigned char buffer[58];
unsigned long lastMessage, lastCount;
int count = 0;
int16_t  x1, y1;
uint16_t w, h;
String SlastMessage;

#define TFT_SCLK   3 // 7  set these to be whatever pins you like!
#define TFT_MOSI   4 // 6  set these to be whatever pins you like!
#define TFT_DC     5 // 4
#define TFT_RST    6 // 3  you can also connect this to the Arduino reset
#define TFT_CS     7  // 5

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);
//The setup function is called once at startup of the sketch
void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  delay(100);
  e32ttl.begin();
  bool change = false;

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_WHITE);
  tft.setRotation(3); // set rotation 3
  tft.setFont(&FreeMonoBold12pt7b);
  testdrawtext("mensage:", ST77XX_BLACK, 0, 15);
  testdrawtext("last msg:", ST77XX_BLACK, 0, 65);

  ResponseStructContainer c;
  c = e32ttl.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
  if (configuration.ADDL != 0x03) {
    Serial.println("cambiando ADDL");
    configuration.ADDL = 0x03;
    change = true;
  }
  if (configuration.ADDH != 0) {
    Serial.println("cambiando ADDH");
    configuration.ADDH = 0;
    change = true;
  }
  if (configuration.CHAN != 0x19) {
    Serial.println("cambiando CHANEL");
    configuration.CHAN = 0x19;
    change = true;
  }
  if (configuration.OPTION.fixedTransmission != FT_FIXED_TRANSMISSION) {
    Serial.println("cambiando FT_FIXED_TRANSMISSION");
    configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
    change = true;
  }
  if (change == true) {
    configuration.OPTION.fec = FEC_1_ON;
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
    configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
    configuration.SPED.uartBaudRate = UART_BPS_9600;
    e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
    e32ttl.resetModule();
    printParameters(configuration);
    c.close();
  } else {
    printParameters(configuration);
    c.close();
  }
}

void testdrawtext(char *text, uint16_t color, uint16_t x, uint16_t y) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void loop()
{
  if (e32ttl.available()) {
    ResponseContainer rs = e32ttl.receiveMessage();
    // First of all get the data
    String message = rs.data;

    Serial.println(rs.status.getResponseDescription());
    Serial.println(message);
    char charBuf[6];
    message.toCharArray(charBuf, 50);
    tft.fillRect(6, 26, 70, 15, ST77XX_WHITE);
    testdrawtext(charBuf, ST77XX_BLACK, 5, 40);
    lastMessage = millis();
    tft.fillRect(7, 76, 75, 15, ST77XX_WHITE);
  }

  if ((millis() - lastCount > 1000) && (millis() - lastMessage > 5000)) {
    SlastMessage = (millis() - lastMessage) / 1000;
    byte lengthMessage = SlastMessage.length() + 1;
    char charBuf[lengthMessage];
    SlastMessage.toCharArray(charBuf, lengthMessage);
    tft.fillRect(7, 76, 75, 15, ST77XX_WHITE);
    testdrawtext(charBuf, ST77XX_BLACK, 5, 90);
    lastCount = millis();
  }
}

void printParameters(struct Configuration configuration) {
  Serial.println("----------------------------------------");

  Serial.print(F("HEAD : "));  Serial.print(configuration.HEAD, BIN); Serial.print(" "); Serial.print(configuration.HEAD, DEC); Serial.print(" "); Serial.println(configuration.HEAD, HEX);
  Serial.println(F(" "));
  Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, DEC);
  Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, DEC);
  Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
  Serial.println(F(" "));
  Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN); Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
  Serial.print(F("SpeedUARTDatte  : "));  Serial.print(configuration.SPED.uartBaudRate, BIN); Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRate());
  Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN); Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRate());

  Serial.print(F("OptionTrans        : "));  Serial.print(configuration.OPTION.fixedTransmission, BIN); Serial.print(" -> "); Serial.println(configuration.OPTION.getFixedTransmissionDescription());
  Serial.print(F("OptionPullup       : "));  Serial.print(configuration.OPTION.ioDriveMode, BIN); Serial.print(" -> "); Serial.println(configuration.OPTION.getIODroveModeDescription());
  Serial.print(F("OptionWakeup       : "));  Serial.print(configuration.OPTION.wirelessWakeupTime, BIN); Serial.print(" -> "); Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
  Serial.print(F("OptionFEC          : "));  Serial.print(configuration.OPTION.fec, BIN); Serial.print(" -> "); Serial.println(configuration.OPTION.getFECDescription());
  Serial.print(F("OptionPower        : "));  Serial.print(configuration.OPTION.transmissionPower, BIN); Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());

  Serial.println("----------------------------------------");

}
void printModuleInformation(struct ModuleInformation moduleInformation) {
  Serial.println("----------------------------------------");
  Serial.print(F("HEAD BIN: "));  Serial.print(moduleInformation.HEAD, BIN); Serial.print(" "); Serial.print(moduleInformation.HEAD, DEC); Serial.print(" "); Serial.println(moduleInformation.HEAD, HEX);

  Serial.print(F("Freq.: "));  Serial.println(moduleInformation.frequency, HEX);
  Serial.print(F("Version  : "));  Serial.println(moduleInformation.version, HEX);
  Serial.print(F("Features : "));  Serial.println(moduleInformation.features, HEX);
  Serial.println("----------------------------------------");

}
