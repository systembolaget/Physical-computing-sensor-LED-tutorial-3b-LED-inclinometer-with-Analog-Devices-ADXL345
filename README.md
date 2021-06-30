# Physical computing sensor LED tutorial - LED inclinometer with Analog Devices ADXL345

Interactive spirit level or swell meter. How to calibrate, read and de-noise an accelerometer; driving an LED strip with Arduino and FastLED

### Setup

You can click the image to play the associated YouTube video

[![Alt text](Assets/3b%20result.jpg)](https://www.youtube.com/watch?v=v2gvWARR1lA)

### Schematic

![](Assets/3b%20schematic.png)

### Excel six-point-tumble xyz value averaging

![](Assets/3b%20six%20point%20tumble.png)

### De-noised output via exponential moving average

![](Assets/3b%20EMA.png)

### BOM

<pre>
€ 14,00 Adafruit Metro Mini 328 5V 16MHz microcontroller
€ 19,00 Adafruit ADXL345 - Triple-Axis Accelerometer (+-2g/4g/8g/16g)
€ 35,00 APA102C 144 RGB LED strip
€  4,00 Half-size transparent breadboard
€  1,00 Jumper cables
€  1,00 2,1mm DC barrel-jack
€  1,00 1000 µF 10V el. cap, 330Ω resistor
€ 15,00 MEANWELL GST60A05-P1J PSU
€ 90,00
</pre>  

### Useful links  

μc https://www.adafruit.com/product/2590  
Sensor https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf  
Sensor calibration https://www.analog.com/media/en/technical-documentation/application-notes/AN-1057.pdf  
EMA https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average  
LED strip https://learn.adafruit.com/adafruit-dotstar-leds  
LED library https://github.com/FastLED/FastLED (see also Wiki pages)  
 
