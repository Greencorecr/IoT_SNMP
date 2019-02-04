// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1, emon2, emon3, emon4;                   // Create an instance

void setup()
{  
  Serial.begin(115200);
  
  emon1.current(1, 111.1);             // Current: input pin, calibration.
  emon2.current(2, 111.1);             // Current: input pin, calibration.
  emon3.current(3, 111.1);             // Current: input pin, calibration.
  emon4.current(4, 111.1);             // Current: input pin, calibration.

}

void loop()
{
  double Irms1 = emon1.calcIrms(1480);  // Calculate Irms only
  double Irms2 = emon2.calcIrms(1480);  // Calculate Irms only
  double Irms3 = emon3.calcIrms(1480);  // Calculate Irms only
  double Irms4 = emon4.calcIrms(1480);  // Calculate Irms only
  
  Serial.print(Irms1);
  Serial.print(":");
  Serial.print(Irms2);
  Serial.print(":");
  Serial.print(Irms3);
  Serial.print(":");
  Serial.println(Irms4);
  delay(200000);
}
