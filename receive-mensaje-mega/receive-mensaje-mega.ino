
#include "Arduino.h"
#include "LoRa_E32.h"

LoRa_E32 e32ttl(&Serial2, 18, 21, 19); //  RX AUX M0 M1

unsigned char buffer[58];
int count = 0;


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

  //  e32ttl.resetModule();
  // After set configuration comment set M0 and M1 to low
  // and reboot if you directly set HIGH M0 and M1 to program
  ResponseStructContainer c;
  c = e32ttl.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
  //  configuration.ADDL = 3;
  //  configuration.ADDH = 0;
  //  configuration.CHAN = 0x04;
  //  configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
  //  e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  printParameters(configuration);
  c.close();
  // ---------------------------
  Serial.println();
  Serial.println("Start listening!");
}

// The loop function is called in an endless loop
void loop()
{
  if (e32ttl.available()  > 1) {
    ResponseContainer rs = e32ttl.receiveMessage();
    // First of all get the data
    String message = rs.data;

    Serial.println(rs.status.getResponseDescription());
    Serial.println(message);
  }
}

void serialEvent() {
  while (Serial.available()) {
    buffer[count++] = Serial.read();
    if (count == 58)break;
  }
  Serial.write(buffer, count);
  Serial.println("Broadcast message to channel 02");

  ResponseStatus rs = e32ttl.sendFixedMessage(0x00, 0x01, 0x19, buffer, count);
  Serial.println(rs.getResponseDescription());
  clearBufferArray();
  count = 0;
}

void clearBufferArray()
{
  for (int i = 0; i < count; i++)
  {
    buffer[i] = NULL;
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
