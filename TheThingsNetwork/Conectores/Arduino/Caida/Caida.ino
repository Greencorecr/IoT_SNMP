/*
 * Sensor de caída eléctrica
 * Para ESP32 TTGOv2
 * por Greencore Solutions
 * Usa pin 36 con divisor de voltaje para monitorear +5V
 * Debe definir APPEUI, DEVEUI, APPKEY
 */


#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include<U8g2lib.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "config.h"

const int analogInPin = 36;  // Analog input pin that the potentiometer is attached to
int sensorValue = 0;        // value read from the pot

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); 

unsigned long previousMillis = 0;
const long interval = 300000;
unsigned int paqCont = 0;
int estado = 0;
uint8_t mydata[] = "000";
static osjob_t sendjob;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void logo(){
    // LOGO
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawXBMP(39,0,50,50,greenfoot_bits);
    u8g2.drawStr(5,64,"GreenCore Solutions");
    u8g2.sendBuffer();
}

void revisarEstado(){
    // Carga datos del sensor
    sensorValue = analogRead(analogInPin);
    
    // Adaptador conectado, recibe ~0.9v, no hay caída eléctrica. sensorValue == 0 cuando esta inicializando
    if (sensorValue == 0 || sensorValue > 800){
      estado = 0;
    } 
    // Adaptador desconectado, solo recibe energía de la batería, recibe ~0.5v, sí hay caída eléctrica
    else {
      estado = 1;
    }
}

void muestraDatos(){
    // Se llama funcion de revision
    revisarEstado();
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(40, 15, "Estado");

    char estadoString[2]; 
    dtostrf(estado,2,0,estadoString);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    if (estado == 0){
      u8g2.drawStr(62, 30,estadoString);
      u8g2.drawStr(15, 50,"Si hay electricidad");
    }
    else {
      u8g2.drawStr(62, 30,estadoString);
      u8g2.drawStr(15, 50,"No hay electricidad");
    }
    u8g2.sendBuffer(); 
}

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
            Serial.println("estado: ");
            Serial.println(estado);
            Serial.println("sensorValue: ");
            Serial.println(sensorValue);
            if(LMIC.dataLen) {
                // data received in rx slot after tx
                Serial.print(F("Data Received: "));
                Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            // Muestra datos de sensor
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

    // LOGO
    logo();
    delay(1000);
  
    // Se llama funcion de revision antes de alistar paquete
    revisarEstado();
    mydata[1] = estado;

    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } 
    else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
    }
    Serial.println(F("Packet queued"));
    Serial.println(LMIC.freq);

     // INFORMACIÓN GENERAL
    char frecString[10]; 
    dtostrf(LMIC.freq/1000000.0,3,2,frecString);
    char paqString[10]; 
    dtostrf(paqCont,2,0,paqString);
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Sensor Caida");
    u8g2.drawStr(0, 30, "Frecuencia: ");
    u8g2.drawStr(68,30,frecString);
    u8g2.drawStr(100,30," Mhz");
    u8g2.drawStr(0,50,"Paquete: ");
    u8g2.drawStr(50,50,paqString);
    u8g2.sendBuffer();
    delay(1000);
     
    paqCont++;
}

void setup() {
    Serial.begin(115200);
    Serial.println(F("Starting"));
    u8g2.begin();
    WiFi.setHostname(hostname);
    WiFi.begin(ssid, password);
    MDNS.begin(hostname);
    MDNS.enableWorkstation();
    MDNS.addService("snmp", "tcp", 161);
    logo();
    delay(5000);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(WiFi.localIP());
    }

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

    // Tipo sensor
    mydata[0] = 0x02;

    // Start job
    do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}
