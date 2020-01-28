// Tutorial 3b. LED spirit level with Analog Devices ADXL345

// Main parts: Adafruit Metro Mini, APA102C strip 144 x RGB LED,
// Analog Devices ADXL345

// Libraries required to interface with the sensor via I2C and to
// drive RGB LEDs; use the latest versions
#include <Wire.h>
#include "FastLED.h"

// Variables that remain constant
#define DEVICE (0x53) // Sensor I2C address (is fixed!)
byte _buffer[6]; // Array to store the raw sensor output
char POWER_CTL = 0x2D; // Sensor power control register address
char DATA_FORMAT = 0x31; // Sensor data format register address
char DATAX0 = 0x32; // Sensor x-axis data register byte 0
char DATAX1 = 0x33; // Sensor x-axis data register byte 1
char DATAY0 = 0x34;
char DATAY1 = 0x35;
char DATAZ0 = 0x36;
char DATAZ1 = 0x37;
// Averaged output from 2-point calibration with 6-point-tumble
// method. The values are particular to each IC, meaning each
// sensor must undergo this process (no "default" values!)
const int xUp =  252; // Sensor/PCB pointing up
const int xDown = -262; // Sensor/PCB pointing down
const int yUp =  257;
const int yDown = -255;
const int zUp =  255;
const int zDown = -241;
// Sensor resolution in 2g mode is 3.9 mg/LSB, so output must
// be converted to g
const float rawToG = 0.0039;
// Exponential moving average "α" and "S" initialisation
float alphaEMA = 0.05;
float pitchEMA = 0.0;
const byte pinData = 4; // Digital output pin to LED strip
const byte pinClock = 3; // Digital output pin to LED strip
const byte numLeds = 144; // Number of LEDs
struct CRGB leds[numLeds]; // Array that stores each LED's data

// Variables that can change
int xRaw, yRaw, zRaw; // Store the raw sensor output
float xUp1g, xDown1g, yUp1g, yDown1g, zUp1g, zDown1g; // Decimal values for offset and gain calculations
float xOffset, xGain, yOffset, yGain, zOffset, zGain; // Decimal values for offset and gain calculations
int ledPosition, ledHue; // Store leading LED's position and hue

void setup()
{
  // Initialise the I2C library to read from the sensor
  Wire.begin();

  // Initialise serial output, only for initial calibration and testing
  // Serial.begin(115200);

  // Sets sensor to 2g mode, typical output -256 to +256 per axis
  registerWrite(DATA_FORMAT, 0x00);

  // Sets sensor to measuring mode
  registerWrite(POWER_CTL, 0x08);

  // Convert averaged calibration output from above to g
  xUp1g = xUp * rawToG; xDown1g = xDown * rawToG;
  yUp1g = yUp * rawToG; yDown1g = yDown * rawToG;
  zUp1g = zUp * rawToG; zDown1g = zDown * rawToG;

  // Calculate offset and gain values as per Analog Devices
  xOffset = 0.5 * (xUp1g + xDown1g); xGain = 0.5 * (xUp1g - xDown1g);
  yOffset = 0.5 * (yUp1g + yDown1g); yGain = 0.5 * (yUp1g - yDown1g);
  zOffset = 0.5 * (zUp1g + zDown1g); zGain = 0.5 * (zUp1g - zDown1g);

  // Initialise the FastLED library with the type of programmable RGB LED
  // used, the digital output pins the LED strip is wired to, the array that
  // holds each LED's data, and the number of LEDs in the strip. Note: BGR
  // (not RGB) is the required colour-order for APA102C LEDs
  FastLED.addLeds<APA102, pinData, pinClock, BGR>(leds, numLeds);

  // Sets the overall LED strip's brightness (values 0 - 255)
  FastLED.setBrightness(128);
}

void loop()
{
  // A call to this function reads from the ADXL345 sensor
  readSensor();

  // A call to this function applies offset and gain to the raw output
  // and calculates the roll (x) and pitch (y) angle
  calculateAngle();

  // Sets the leading LED's hue
  leds[ledPosition] = CHSV(ledHue, 255, 255);
  // Set LEDs as 0°, 45° and 90° markers; no array, I was too lazy ; )
  leds[0] = CRGB::Azure; leds[36] = CRGB::Azure; leds[71] = CRGB::Azure; leds[72] = CRGB::Azure; leds[108] = CRGB::Azure; leds[143] = CRGB::Azure;
  // Display all LED's data (= illuminate the LED strip)
  FastLED.show();
  // Generate a short tail behind the leading LED
  for (int i = 0; i < numLeds; i++)
  {
    leds[i].fadeToBlackBy(32);
  }
}

void readSensor()
{
  // The raw x, y and z data output is represented in six bytes
  byte bytesToRead = 6;

  // Read from the sensor's six data registers using the burst read
  // method of all six registers at once as per Analog Devices
  registerRead(DATAX0, bytesToRead, _buffer);

  // Convert output, 10 bit, 2 bytes, with the LSB first
  xRaw = (((int)_buffer[1]) << 8) | _buffer[0];
  yRaw = (((int)_buffer[3]) << 8) | _buffer[2];
  zRaw = (((int)_buffer[5]) << 8) | _buffer[4];
}

void calculateAngle()
{
  // Calculate adjusted x, y and z values based on offset and gain;
  // cast ints to floats at execution time as per Analog Devices
  float xCal = (((float)xRaw * rawToG - xOffset) / xGain) * 256;
  float yCal = (((float)yRaw * rawToG - yOffset) / yGain) * 256;
  float zCal = (((float)zRaw * rawToG - zOffset) / zGain) * 256;

  // Calculate roll and pitch angles where pitch = rotation around
  // y-axis and roll = rotation around x-axis
  float roll = atan2(yCal, zCal) * 180 / M_PI;
  float pitch = atan2(-xCal, sqrt(yCal * yCal + zCal * zCal)) * 180 / M_PI;

  // Calculate exponential moving average
  pitchEMA = (alphaEMA * pitch) + ((1 - alphaEMA) * pitchEMA);

  // Calculate the position of the leading LED depending on the
  // sensor's/PCB's y-axis (pitch)
  ledPosition = round(map(pitchEMA, 90, -90, 0, numLeds - 1));

  // Calculate the hue based on the sensor's/PCB's x-axis roll,
  // where port = red, starboard = green
  ledHue = map(roll, 30, -30, 96, 0);

  // Stop leading LED's jitter
  delay(10);
}

void registerWrite(byte address, byte value)
{
  // Write to a sensor register
  Wire.beginTransmission(DEVICE);
  Wire.write(address);
  Wire.write(value);
  Wire.endTransmission();
}

void registerRead(byte address, byte num, byte _buffer[])
{
  // Read from the sensor's registers into the _buffer array
  Wire.beginTransmission(DEVICE);
  Wire.write(address);
  Wire.endTransmission();
  Wire.beginTransmission(DEVICE);
  Wire.requestFrom(DEVICE, num);

  byte i = 0;

  while (Wire.available())
  {
    // Read one byte at a time
    _buffer[i] = Wire.read();
    i++;
  }

  Wire.endTransmission();
}
