/*
 * Sensor de puerta
 * Para ESP32 TTGOv2
 * por Greencore Solutions
 * Usa pin 34 en puerta1 en modo trigger
 * 
 * Debe definir APPEUI, DEVEUI, APPKEY
 */
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include<U8g2lib.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <Arduino_SNMP.h>
#include <EasyButton.h>


#define BUTTON_PIN 34
EasyButton sensorPuerta(BUTTON_PIN);


#include "config.h"

#ifdef USE_IAS
#define COMPDATE __DATE__ __TIME__
#define MODEBUTTON 0

#include <IOTAppStory.h>
IOTAppStory IAS(COMPDATE, MODEBUTTON);
#endif

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); 

WiFiUDP udp;
SNMPAgent snmp = SNMPAgent("greencore");  // Starts an SMMPAgent instance with the community string 'public'

char* refreshDisplay = "5000";
unsigned long lastDisplayRefresh;

int snmpDoorCount = 0;
int snmpDoorOpen = 0;
int lastNumber = 0;
uint8_t mydata[] = "0000";

static osjob_t sendjob;

void onPressedForDuration() {
  snmpDoorCount++;
  Serial.print("count: "); Serial.print(snmpDoorCount);
  Serial.print("open: "); Serial.print(sensorPuerta.isPressed());
  Serial.println("Button has been pressed!");

}

void logo(){
    // LOGO
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawXBMP(39,0,50,50,greenfoot_bits);
    u8g2.drawStr(5,64,"GreenCore Solutions");
    u8g2.sendBuffer();
}

void muestraDatos(int doorCount){
    // Muestra cantidad de activaciones (triggers) al activarse el sensor de puerta
    char actString[10];
    dtostrf(doorCount,9,0,actString);
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(15, 10, "Activaciones ");
    u8g2.drawStr(25, 40,actString);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.sendBuffer();     
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    int contador = 0;
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            Serial.println(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if(LMIC.dataLen) {
                // data received in rx slot after tx
                Serial.print(F("Data Received: "));
                Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
                Serial.println();
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);

            // Muestra datos de activaciones/triggers
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
         default:
            Serial.println(F("Unknown event"));
            break;
    }
}


void do_send(osjob_t* j){
  if (sensorPuerta.isPressed()) {
    mydata[1] = 0x1;
  } else {
    mydata[1] = 0x0;
  }
//    mydata[2] = lowByte(doorCount);
//    mydata[3] = highByte(doorCount);
    
    unsigned long currentMillis = millis();

    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
           // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
    }
}

void setup() {
    u8g2.begin();
    logo();
    #ifdef USE_IAS
    IAS.begin('P');
    IAS.setCallHome(true);                  // Set to true to enable calling home frequently (disabled by default)
    IAS.setCallHomeInterval(300);            // Call home interval in seconds, use 60s only for development. Please change it to at least 2 hours in production
    IAS.callHome(true);
    IAS.addField(refreshDisplay, "RefreshDisplay(mS)", 5, 'N');
    #else
    Serial.begin(115200);
    #endif
    sensorPuerta.begin();
    sensorPuerta.onPressedFor(200, onPressedForDuration);
    Serial.println(F("Starting"));
    WiFi.setHostname(hostname);
    WiFi.begin(ssid, password);
    MDNS.begin(hostname);
    MDNS.enableWorkstation();
    MDNS.addService("snmp", "tcp", 161);
    snmp.setUDP(&udp);
    snmp.begin();
    snmp.addIntegerHandler(".1.3.6.1.4.1.5.0", &snmpDoorCount);
    snmp.addIntegerHandler(".1.3.6.1.4.1.5.1", &snmpDoorOpen);
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    #ifndef OTAA
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
    #endif

    #ifdef CFG_us915
    LMIC_selectSubBand(1);

    //Disable FSB2-8, channels 16-72
    for (int i = 16; i < 73; i++) {
      if (i != 10)
        LMIC_disableChannel(i);
    }
    #endif
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

    // código sensor
    mydata[0] = 0x16;

    // Start job
    do_send(&sendjob);
}

void loop() {
  #ifdef USE_IAS
  IAS.loop();
  #endif
  os_runloop_once();
  snmp.loop();
  sensorPuerta.read();
  


//      logo();
//      muestraDatos(doorCount);
   

}
