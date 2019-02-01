/*
 * Sensor de puerta
 * Para ESP32 TTGOv2
 * por Greencore Solutions
 * Usa pin 34 en puerta1 en modo trigger
 * 
 * Debe definir NWKSKEY, APPSKEY, y DEVADDR
 */
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include<U8g2lib.h>


// Imagen de GreenCore 50x50 px
#define greenfoot_width 50
#define greenfoot_height 50
static const unsigned char greenfoot_bits[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x00, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x03, 0xff, 0xff, 0xff, 0x7f, 0xf0, 0xff, 0x03, 0xff, 0xc0, 0xff, 0x3f,
   0xe0, 0xff, 0x03, 0x7f, 0xc0, 0xff, 0x1f, 0xc0, 0xff, 0x03, 0x3f, 0x80,
   0xff, 0x0f, 0xc0, 0xff, 0x03, 0x3f, 0x80, 0xff, 0x0f, 0xc0, 0xff, 0x03,
   0x3f, 0x80, 0xff, 0x0f, 0xc0, 0xff, 0x03, 0x3f, 0x00, 0xff, 0x0f, 0xc0,
   0xff, 0x03, 0x3f, 0x80, 0xff, 0x0f, 0xc0, 0xff, 0x03, 0x7f, 0x80, 0xff,
   0x07, 0xc0, 0xff, 0x03, 0x7f, 0xc0, 0xff, 0x07, 0xe0, 0xff, 0x03, 0x7f,
   0xe0, 0xff, 0x03, 0xf8, 0xff, 0x03, 0xff, 0xe1, 0xff, 0x81, 0xff, 0xff,
   0x03, 0xff, 0xf1, 0xff, 0xc0, 0xff, 0xff, 0x03, 0xff, 0xf3, 0x7f, 0xe0,
   0xff, 0xff, 0x03, 0xff, 0xe3, 0x7f, 0xf0, 0xff, 0xff, 0x03, 0xff, 0xe3,
   0x3f, 0xf8, 0xff, 0xff, 0x03, 0xff, 0xe7, 0x1f, 0xfc, 0xff, 0xff, 0x03,
   0xff, 0xc7, 0x0f, 0xfe, 0xff, 0xff, 0x03, 0xff, 0xc7, 0x07, 0xff, 0xff,
   0xff, 0x03, 0xff, 0x87, 0x83, 0xff, 0xff, 0xff, 0x03, 0xff, 0x07, 0xc0,
   0xff, 0xff, 0xff, 0x03, 0xff, 0x07, 0xc0, 0xff, 0xff, 0xff, 0x03, 0xff,
   0x07, 0xe0, 0xff, 0x1f, 0xfc, 0x03, 0xff, 0x07, 0xe0, 0xff, 0x07, 0xf8,
   0x03, 0xff, 0x03, 0xf0, 0xff, 0x07, 0xf0, 0x03, 0xff, 0x03, 0xf0, 0xff,
   0x03, 0xf0, 0x03, 0xff, 0x01, 0xf0, 0xff, 0x01, 0xf0, 0x03, 0xff, 0x00,
   0xe0, 0xff, 0x00, 0xf0, 0x03, 0xff, 0x00, 0x80, 0x1f, 0x00, 0xf0, 0x03,
   0x7f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x03, 0x3f, 0x00, 0x00, 0x00, 0x00,
   0xf8, 0x03, 0x1f, 0x00, 0x00, 0xe0, 0x0f, 0xfc, 0x03, 0x0f, 0x00, 0x00,
   0xfe, 0x1f, 0xfe, 0x03, 0x07, 0x00, 0xe0, 0xff, 0xff, 0xff, 0x03, 0x03,
   0x00, 0xf8, 0xff, 0xff, 0xff, 0x03, 0x01, 0x00, 0xfe, 0xff, 0xff, 0xff,
   0x03, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0xc0, 0xff, 0xff,
   0xff, 0xff, 0x03, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0xf0,
   0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x01,
   0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0xfc, 0xff, 0xff, 0xff,
   0xff, 0x00 };

// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the prototype TTN
// network initially.
static const PROGMEM u1_t NWKSKEY[16] = { }; // Network Session Key, hex, MSB

// LoRaWAN AppSKey, application session key
// This is the default Semtech key, which is used by the prototype TTN
// network initially.
static const u1_t PROGMEM APPSKEY[16] = { }; // Application Sessoin Key, hex, MSB

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
static const u4_t DEVADDR = 0xfede2 ; // Device Address, 0xDecimal, MSB

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); 


unsigned long previousMillis = 0;
const long interval = 300000;
unsigned int paqCont = 0;
uint8_t mydata[] = "00000";

TaskHandle_t tempTaskHandle = NULL;


static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 30;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 14,
    .dio = {26, 33, 32}  // Pins for the Heltec ESP32 Lora board/ TTGO Lora32 with 3D metal antenna
};

struct Trigger {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};


Trigger puerta1 = {34, 0, false};

void IRAM_ATTR isr() {
    puerta1.numberKeyPresses += 1;
    puerta1.pressed = true;
    //Serial.println(puerta1.numberKeyPresses);
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
    int contador = 1;
    char actString[10]; 

    // Muestra cantidad de activaciones (triggers) al activarse el sensor de puerta
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(15, 10, "Activaciones ");
    
    // Activaciones - Triggers
    // Se envía paquete cada 30 segundos, por lo cual durante 20 segundos se reimprime el valor de puerta1.numberKeyPresses
    // para visualizar en pantalla cuando se abre
    while (contador != 20){
      Serial.println(contador);
      dtostrf(puerta1.numberKeyPresses,9,0,actString);

      // Dibuja cuadro del color del fondo para ocultar dato anterior 
      u8g2.setDrawColor(0);
      u8g2.drawBox(0,25,164,20);
      u8g2.sendBuffer(); 

      // Activa nuevamente color default (blanco)
      u8g2.setDrawColor(1);
      u8g2.drawStr(25, 40,actString);
      u8g2.sendBuffer(); 
      delay(1000);  
      contador++;
    }
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
            Serial.print(puerta1.numberKeyPresses);
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

    mydata[1] = ( puerta1.numberKeyPresses >> 24 ) & 0xFF;
    mydata[2] = ( puerta1.numberKeyPresses >> 16 ) & 0xFF;
    mydata[3] = ( puerta1.numberKeyPresses >> 8 ) & 0xFF;
    mydata[4] = puerta1.numberKeyPresses & 0xFF;
    
    unsigned long currentMillis = millis();


    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
           // Prepare upstream data transmission at the next possible time.
           LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        }
    Serial.println(F("Paquete procesado"));
    Serial.println(LMIC.freq);

    // LOGO
    logo();
    delay(2000); // muestra el logo por 2 segundos  
    
    // INFORMACIÓN GENERAL
    char frecString[10]; 
    dtostrf(LMIC.freq/1000000.0,3,2,frecString);
    char paqString[10]; 
    dtostrf(paqCont,2,0,paqString);
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Sensor Puerta");
    u8g2.drawStr(0, 30, "Frecuencia: ");
    u8g2.drawStr(68,30,frecString);
    u8g2.drawStr(100,30," Mhz");
    u8g2.drawStr(0,50,"Paquete: ");
    u8g2.drawStr(50,50,paqString);
    u8g2.sendBuffer();
    delay(2000);
    
    // Contador de paquetes enviados a TTN
    paqCont++;  
}

void setup() {
    pinMode(puerta1.PIN, INPUT_PULLUP);
    attachInterrupt(puerta1.PIN, isr, FALLING);
    Serial.begin(115200);
    Serial.println(F("Starting"));
    u8g2.begin();
//    u8g2.setFont(u8x8_font_chroma48medium8_r);
    #ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
    #endif

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Set static session parameters. Instead of dynamically establishing a session
    // by joining the network, precomputed session parameters are be provided.
    #ifdef PROGMEM
    // On AVR, these values are stored in flash and only copied to RAM
    // once. Copy them to a temporary buffer here, LMIC_setSession will
    // copy them into a buffer of its own again.
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
    #else
    // If not running an AVR with PROGMEM, just use the arrays directly 
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
    #endif


   for (int channel=0; channel<8; ++channel) {
     LMIC_disableChannel(channel);
   }
   for (int channel=9; channel<72; ++channel) {
      LMIC_disableChannel(channel);
   }

    // Disable link check validation
    LMIC_setLinkCheckMode(0);
    // Set data rate and transmit power (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(DR_SF7,14);

    // código sensor
    mydata[0] = 0x16;


    // Start job
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
