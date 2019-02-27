/*
 * Sensor de consumo eléctrico
 * Para ESP32 TTGOv2
 * por Greencore Solutions
 * Usa serial2 y empaqueta hacia TTN
 * 
 * Debe definir APPEUI, DEVEUI, APPKEY
 */


#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include<U8g2lib.h>
#include<Arduino.h>


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
uint8_t mydata[] = "00000000000000000";

// Para lectura de Serial2
String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 14,
    .dio = {26, 33, 32}  // Pins for the Heltec ESP32 Lora board/ TTGO Lora32 with 3D metal antenna
};


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
            if(LMIC.dataLen) {
                // data received in rx slot after tx
                Serial.print(F("Data Received: "));
                Serial.write(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
                Serial.println();
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
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
    unsigned int sensorCount = 0;
    String sensorData[4] = "0000";
    // LOGO
    logo();
    delay(1000); // muestra el logo por 5 segundos  

    // TODO: Mover a funcion
    while (Serial2.available()) {
      // get the new byte:
      char inChar = (char)Serial2.read();
      // add it to the inputString:
      if (inChar == ':') {
        sensorData[sensorCount] = inputString;
        inputString="";
        sensorCount++;        
      } else if (inChar == '\n') {
        sensorData[3]= inputString;
        Serial.print("sensorData[0]: ");
        Serial.println(sensorData[0]);
        Serial.print("sensorData[1]: ");
        Serial.println(sensorData[1]);
        Serial.print("sensorData[2]: ");
        Serial.println(sensorData[2]);
        Serial.print("sensorData[3]: ");
        Serial.println(sensorData[3]); 
        inputString = "";
        stringComplete = true;
        sensorCount = 0;
      } else {
        inputString += inChar;
      }
    } 

    // Creando paquete para sensor0-3
    int tempInteger = sensorData[0].toInt();
    mydata[1] = highByte(tempInteger);
    mydata[2] = lowByte(tempInteger);
    float tempFloat = (sensorData[0].toFloat() - tempInteger) * 100;
    tempInteger = int(tempFloat);
    mydata[3] = highByte(tempInteger);
    mydata[4] = lowByte(tempInteger);

    tempInteger = sensorData[1].toInt();
    mydata[5] = highByte(tempInteger);
    mydata[6] = lowByte(tempInteger);
    tempFloat = (sensorData[1].toFloat() - tempInteger) * 100;
    tempInteger = int(tempFloat);
    mydata[7] = highByte(tempInteger);
    mydata[8] = lowByte(tempInteger);

    tempInteger = sensorData[2].toInt();
    mydata[9] = highByte(tempInteger);
    mydata[10] = lowByte(tempInteger);
    tempFloat = (sensorData[2].toFloat() - tempInteger) * 100;
    tempInteger = int(tempFloat);
    mydata[11] = highByte(tempInteger);
    mydata[12] = lowByte(tempInteger);

    tempInteger = sensorData[3].toInt();
    mydata[13] = highByte(tempInteger);
    mydata[14] = lowByte(tempInteger);
    tempFloat = (sensorData[3].toFloat() - tempInteger) * 100;
    tempInteger = int(tempFloat);
    mydata[15] = highByte(tempInteger);
    mydata[16] = lowByte(tempInteger);



    unsigned long currentMillis = millis();
    
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
           // Prepare upstream data transmission at the next possible time.
           LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        }
    Serial.println(F("Packet queued"));
    Serial.println(LMIC.freq);

 // INFORMACIÓN GENERAL
    char frecString[10]; 
    dtostrf(LMIC.freq/1000000.0,3,2,frecString);
    char paqString02[10]; 
    dtostrf(paqCont,2,0,paqString02);
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Consumo Eléctrico");
    u8g2.drawStr(0, 20, "Frecuencia: ");
    u8g2.drawStr(68,20,frecString);
    u8g2.drawStr(100,20," MHz");
    u8g2.drawStr(0,30,"Paquete: ");
    u8g2.drawStr(50,30,paqString02);

    u8g2.drawStr(0,40,"s1: ");
    u8g2.drawStr(60,40,"s2: ");
    u8g2.drawStr(0,50,"s3: ");
    u8g2.drawStr(60,50,"s4: ");

    char sensorData0[5];
    char sensorData1[5];
    char sensorData2[5];
    char sensorData3[5];
    sensorData[0].toCharArray(sensorData0, 5);
    sensorData[1].toCharArray(sensorData1, 5);
    sensorData[2].toCharArray(sensorData2, 5);
    sensorData[3].toCharArray(sensorData3, 5);
    u8g2.drawStr(30,40,sensorData0);
    u8g2.drawStr(90,40,sensorData1);
    u8g2.drawStr(30,50,sensorData2);
    u8g2.drawStr(90,50,sensorData3);

    u8g2.sendBuffer();
    paqCont++;
}

void setup() {
    Serial2.begin(115200); // Desde Arduino-Nano
    Serial.begin(115200);
    Serial.println(F("Starting"));
    u8g2.begin();
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
    //Disable FSB2-8, channels 16-72
    for (int i = 16; i < 73; i++) {
      if (i != 10)
        LMIC_disableChannel(i);
  
    }
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

    // tipo de sensor
    mydata[0] = 0x01;

    // Start job
    do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}
