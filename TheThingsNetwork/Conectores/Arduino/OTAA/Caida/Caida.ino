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
//#include <U8x8lib.h>
#include<U8g2lib.h>


const int analogInPin = 36;  // Analog input pin that the potentiometer is attached to
int sensorValue = 0;        // value read from the pot

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

static const u1_t PROGMEM DEVEUI[8]={  } ; // Device EUI, hex, lsb
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

static const u1_t PROGMEM APPEUI[8]={  }; // Application EUI, hex, lsb
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

static const u1_t PROGMEM APPKEY[16] = {  }; // App Key, hex, msb
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16); 

unsigned long previousMillis = 0;
const long interval = 300000;
unsigned int paqCont = 0;
int estado = 0;
uint8_t mydata[] = "000";
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
    u8g2.clearBuffer();
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawXBMP(39,0,50,50,greenfoot_bits);
    u8g2.drawStr(5,64,"GreenCore Solutions");
    u8g2.sendBuffer();
    //delay(5000); // muestra el logo por 5 segundos
  
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
    //delay(3000);

    // Pantalla estado actual
    
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
     
    paqCont++;
}

void setup() {
    Serial.begin(115200);
    Serial.println(F("Starting"));
    u8g2.begin();
    #ifdef VCC_ENABLE
    // For Pinoccio Scout boardsc
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(3000);
    #endif

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

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

    // Tipo sensor
    mydata[0] = 0x02;

    // Start job
    do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}
