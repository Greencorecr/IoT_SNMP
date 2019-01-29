/*
 * Sensor de gotas de agua
 * Para ESP32 TTGOv2
 * por Greencore Solutions
 * Usa pin 34 en goteo1 en modo trigger
 * 
 * Debe definir NWKSKEY, APPSKEY, y DEVADDR
 */


#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <U8x8lib.h>

// LoRaWAN NwkSKey, network session key
// This is the default Semtech key, which is used by the prototype TTN
// network initially.
static const PROGMEM u1_t NWKSKEY[16] = {  }; // Network Session Key, hex, MSB

// LoRaWAN AppSKey, application session key
// This is the default Semtech key, which is used by the prototype TTN
// network initially.
static const u1_t PROGMEM APPSKEY[16] = {  }; // Application Sessoin Key, hex, MSB

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
static const u4_t DEVADDR = 0xfede2 ; // Device Address, 0xDecimal, MSB

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

unsigned long previousMillis = 0;
const long interval = 300000;
unsigned int paqCont = 0;
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

struct Trigger {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};
Trigger goteo1 = {34, 0, false};

void IRAM_ATTR isr() {
    goteo1.numberKeyPresses += 1;
    goteo1.pressed = true;
    u8x8.setCursor(0,5);
    u8x8.print("Triggers: ");
    u8x8.print(goteo1.numberKeyPresses);
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
//            u8x8.drawString(0, 5, "Paquete enviado");
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
    mydata[1] = highByte(goteo1.numberKeyPresses);
    mydata[2] = lowByte(goteo1.numberKeyPresses);
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
    u8x8.setCursor(5,2);
    u8x8.print(LMIC.freq/1000000.0);
    u8x8.print(" Mhz");
    u8x8.setCursor(0,4);
    u8x8.print("Paquete ");
    u8x8.print(paqCont);
    paqCont++;
    delay(120000);
}

void setup() {
    pinMode(goteo1.PIN, INPUT_PULLUP);
    attachInterrupt(goteo1.PIN, isr, FALLING);
    Serial.begin(115200);
    Serial.println(F("Starting"));
    u8x8.begin();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
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

    mydata[0] = 0x04;
    mydata[1] = goteo1.numberKeyPresses;

    u8x8.drawString(0, 0, "Greencore");
    u8x8.drawString(0, 1, "Sensor gotas");
    u8x8.drawString(0, 2, "Freq ");

    // Start job
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
