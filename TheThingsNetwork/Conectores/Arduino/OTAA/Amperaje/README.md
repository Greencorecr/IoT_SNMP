# Sensor de Amperaje

El sensor de amperaje utiliza una shield diseñado para Arduino, al cual le notamos algunos problemas a la hora de tratar de leerlo directamente desde una ESP32, por lo cual se decide utiliza un Arduino Nano para realizar la lectura del sensor, y luego enviar este dato vía serial hacia el ESP32 para enviar el dato a TTN.

## Arduino

Se utiliza un Arduino Nano conectado a la placa [Energy Monitor](https://www.crcibernetica.com/energy-monitor-shield-v0-2/) de CRCibernética, conectando los cuatro puertos para sensor de amperaje, a los puerto A1-A4 del Nano.

Luego empaqueta los datos y los envía por medio de una comunicación serial con el ESP32, el cual recibe del dato luego de pasar por un convertidor de lógica.

## ESP32

Una vez que el ESP32 recibe el dato, lo almacena en una variable de ambiente, preparándose para enviar dicho dato hacia la red de TTN.

## Programando

Se recomienda subir el código a ambas tarjetas, utilizar una interface serial con capacidad de 3V-5V y compruebe el envío y recepción de ambas tarjetas de forma independiente, antes de conectar el convertidor de lógica.

## ¿Como lee el amperaje?

El sensor hace el uso de Transformadores de Corriente (o **CT** por sus siglas en inglés) individuales, de tipo split-core para para medir sin necesidad de interrumpir el cable de fase, hasta un [máximo de 100A](ftp://imall.iteadstudio.com/Sensor/IM120628007/Specs_IM120628007.pdf). Para comprender un poco la teoría de los CTs puede visitar este [tutorial](https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/introduction?redirected=true). Gracias a la Ley de Ohm, también podemos calcular los Watts sobre la línea, conociendo previamente el voltaje.

## Depuración

#### Los datos enviados a TTN son "0000" en lugar de un valor real del sensor.
Esto es normal para el paquete 0, sin embargo si recibe este error de manera esporádica, y el ESP32 no se está reiniciando o reiniciando su contador de TTN; dependiendo de la frecuencia en que desee enviar los paquetes de monitoreo, se recomienda que el valor del ``delay()`` en el Arduino-Nano, no sea mayor al valor de TX_INTERVAL.
