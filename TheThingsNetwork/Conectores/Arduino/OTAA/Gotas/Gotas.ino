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
   
static const u1_t PROGMEM DEVEUI[8]={ } ; // Device EUI, hex, lsb
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

static const u1_t PROGMEM APPEUI[8]={ }; // Application EUI, hex, lsb
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

static const u1_t PROGMEM APPKEY[16] = { }; // App Key, hex, msb
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); 

unsigned long previousMillis = 0;
const long interval = 300000;
unsigned int paqCont = 0;
uint8_t mydata[] = "000";

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

    // LOGO
    logo();
    delay(1000); // muestra el logo por 2 segundos 

    // INFORMACIÓN GENERAL
    char frecString[10]; 
    dtostrf(LMIC.freq/1000000.0,3,2,frecString);
    char paqString[10]; 
    dtostrf(paqCont,2,0,paqString);
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Sensor Gotas");
    u8g2.drawStr(0, 30, "Frecuencia: ");
    u8g2.drawStr(68,30,frecString);
    u8g2.drawStr(100,30," Mhz");
    u8g2.drawStr(0,50,"Paquete: ");
    u8g2.drawStr(50,50,paqString);
    u8g2.sendBuffer();
    delay(1000);

    // Contador de paquetes enviados a TTN
    paqCont++;
}

void setup() {
    pinMode(goteo1.PIN, INPUT_PULLUP);
    attachInterrupt(goteo1.PIN, isr, FALLING);
    Serial.begin(115200);
    Serial.println(F("Starting"));
    u8g2.begin();
    u8g2.setFont(u8x8_font_chroma48medium8_r);
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
    LMIC_selectSubBand(1);

 //Disable FSB1, channels 0-7
  for (int i = 0; i < 7; i++) {
    if (i != 10)
      LMIC_disableChannel(i);

  }
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
    os_runloop_once();
}
