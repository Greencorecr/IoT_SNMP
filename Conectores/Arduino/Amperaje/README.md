# Sensor de Amperaje

El sensor de amperaje utiliza una shield diseñado para Arduino, al cual le notamos algunos problemas a la hora de tratar de leerlo directamente desde una ESP32, por lo cual se decide utiliza un Arduino Nano para realizar la lectura del sensor, y luego enviar este dato vía serial hacia el ESP32 para enviar el dato a TTN.

## Arduino

Se utiliza un Arduino Nano conectado a la placa [Energy Monitor](https://www.crcibernetica.com/energy-monitor-shield-v0-2/) de CRCibernética, conectando los cuatro puertos para sensor de amperaje, a los puerto A1-A4 del Nano.

Luego empaqueta los datos y los envía por medio de una comunicación serial con el ESP32, el cual recibe del dato luego de pasar por un convertidor de lógica.

## ESP32

Una vez que el ESP32 recibe el dato, lo almacena en una variable de ambiente, preparándose para enviar dicho dato hacia la red de TTN.

## Programando

Se recomienda subir el código a ambas tarjetas, utilizar una interface serial con capacidad de 3V-5V y compruebe el envío y recepción de ambas tarjetas de forma independiente, antes de conectar el convertidor de lógica.
