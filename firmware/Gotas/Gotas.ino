/*
 * Sensor de gotas de agua
 * Para ESP32 TTGOv2
 * por Greencore Solutions
 * Usa pin 34 en goteo1 en modo trigger
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
int snmpGotas = 0;

unsigned long previousMillis = 0;
const long interval = 300000;
unsigned int paqCont = 0;
uint8_t mydata[] = "000";

static osjob_t sendjob;

struct Trigger {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};
Trigger goteo1 = {34, 0, false};

void IRAM_ATTR isr() {
    goteo1.numberKeyPresses += 1;
    goteo1.pressed = true;
    //Serial.println(goteo1.numberKeyPresses);
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

void muestraDatos(){
    char actString[10]; 
    // Muestra cantidad de activaciones (triggers) al activarse el sensor de gotas
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(15, 10, "Activaciones ");
    dtostrf(goteo1.numberKeyPresses,9,0,actString);
    u8g2.drawStr(25, 40,actString);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    // No se han detecado gotas
    if (goteo1.numberKeyPresses ==0){
      u8g2.drawStr(20, 60,"OK no hay gotas");   
    }
    else {
      u8g2.drawStr(0, 60,"WARN gotas detectadas");
    }
    u8g2.sendBuffer();   
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
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
            Serial.println("Paquete enviado");
            Serial.println("numberKeyPresses: ");
            Serial.print(goteo1.numberKeyPresses);
            if(LMIC.dataLen) {
                // data received in rx slot after tx
                Serial.print(F("Data Received: "));
                Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
                Serial.println();
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            
            // Muestra datos de activaciones/triggers
            muestraDatos();
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
    unsigned long currentMillis = millis();

    mydata[1] = highByte(goteo1.numberKeyPresses);
    mydata[2] = lowByte(goteo1.numberKeyPresses);


    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
           // Prepare upstream data transmission at the next possible time.
           LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        }
    Serial.println(F("Pqaquete procesado"));
    Serial.println(LMIC.freq);
}

void setup() {
    u8g2.begin();
    u8g2.setFont(u8x8_font_chroma48medium8_r);
    logo();
    #ifdef USE_IAS
    IAS.begin('P');
    IAS.setCallHome(true);                  // Set to true to enable calling home frequently (disabled by default)
    IAS.setCallHomeInterval(300);            // Call home interval in seconds, use 60s only for development. Please change it to at least 2 hours in production
    IAS.callHome(true);
    #else
    Serial.begin(115200);
    #endif
    pinMode(goteo1.PIN, INPUT_PULLUP);
    attachInterrupt(goteo1.PIN, isr, FALLING);
    Serial.println(F("Starting"));
    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    WiFi.setHostname(hostname);
    WiFi.begin(ssid, password);
    MDNS.begin(hostname);
    MDNS.enableWorkstation();
    MDNS.addService("snmp", "tcp", 161);
    snmp.setUDP(&udp);
    snmp.begin();
    snmp.addIntegerHandler(".1.3.6.1.4.1.5.0", &snmpGotas);

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(WiFi.localIP());
    }

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    LMIC_selectSubBand(1);

    //Disable FSB2-8, channels 16-72
    for (int i = 16; i < 73; i++) {
      if (i != 10)
        LMIC_disableChannel(i);
    }
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

    // Tipo sensor
    mydata[0] = 0x04;

    // Start job
    do_send(&sendjob);
}

void loop() {
  #ifdef USE_IAS
  IAS.loop();
  #endif
  os_runloop_once();
  snmpGotas = int(goteo1.numberKeyPresses);
  snmp.loop();
}
