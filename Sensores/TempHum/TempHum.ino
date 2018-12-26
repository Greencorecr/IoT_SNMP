/*
 * Sensor de temperatura y humedad
 * Para ESP32 TTGOv2
 * por Greencore Solutions
 * Usa pin 25 para leer al DHT11
 * 
 * Debe definir NWKSKEY, APPSKEY, y DEVADDR
 */


#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include "DHTesp.h"
#include "Ticker.h"
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
uint8_t mydata[] = "00000";


DHTesp dht;

void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;
/** Ticker for temperature reading */
Ticker tempTicker;
/** Comfort profile */
ComfortState cf;
/** Flag if task should run */
bool tasksEnabled = false;
/** Pin number for DHT11 data pin */
int dhtPin = 25;

bool initTemp() {
  byte resultValue = 0;
  // Initialize temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT initiated");

  // Start task to get temperature
  xTaskCreatePinnedToCore(
      tempTask,                       /* Function to implement the task */
      "tempTask ",                    /* Name of the task */
      4000,                           /* Stack size in words */
      NULL,                           /* Task input parameter */
      5,                              /* Priority of the task */
      &tempTaskHandle,                /* Task handle. */
      1);                             /* Core where the task should run */

  if (tempTaskHandle == NULL) {
    Serial.println("Failed to start task for temperature update");
    return false;
  } else {
    // Start update of environment data every 20 seconds
    tempTicker.attach(20, triggerGetTemp);
  }
  return true;
}

void triggerGetTemp() {
  if (tempTaskHandle != NULL) {
     xTaskResumeFromISR(tempTaskHandle);
  }
}

void tempTask(void *pvParameters) {
  Serial.println("tempTask loop started");
  while (1) // tempTask loop
  {
    if (tasksEnabled) {
      // Get temperature values
      getTemperature();
    }
    // Got sleep again
    vTaskSuspend(NULL);
  }
}

bool getTemperature() {
  // Reading temperature for humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  TempAndHumidity newValues = dht.getTempAndHumidity();
  // Check if any reads failed and exit early (to try again).
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return false;
  }

  Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity));
  u8x8.setCursor(0,5);
  u8x8.print("Temp: ");
  u8x8.print((int) newValues.temperature);
  u8x8.setCursor(0,6);
  u8x8.print("Hum: ");
  u8x8.print((int) newValues.humidity);

  mydata[1] = highByte((int) newValues.temperature);
  mydata[2] = lowByte((int) newValues.temperature);
  mydata[3] = highByte((int) newValues.humidity);
  mydata[4] = lowByte((int) newValues.humidity);

  return true;
}

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
  initTemp();
  // Signal end of setup() to tasks
  tasksEnabled = true;
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

    mydata[0] = 0x08;

    u8x8.drawString(0, 0, "Greencore");
    u8x8.drawString(0, 1, "Sensor T+H");
    u8x8.drawString(0, 2, "Freq ");

    // Start job
    do_send(&sendjob);
}

void loop() {
  if (!tasksEnabled) {
    // Wait 2 seconds to let system settle down
    delay(2000);
    // Enable task that will read values from the DHT sensor
    tasksEnabled = true;
    if (tempTaskHandle != NULL) {
      vTaskResume(tempTaskHandle);
    }
  }
  //yield();
  os_runloop_once();
}
