#include <Wire.h>
#include <Adafruit_BME280.h>
#include <SPI.h>
#include <LowPower.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

//---------------------------------------------------INIT----------------------------------------------------------------
// Initialize LCD object for hardware SPI
// Adafruit_PCD8544(D/C,CE,RST);
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5);

//Initialize BME280
#define BME_280_I2C_ADDRESS 0x76
Adafruit_BME280 bme280;

//---------------------------------------------------VARIABLES----------------------------------------------------------------
int voltagePin = A0;
float temp;
float pressure;
float humidity;
byte wakeCounter = 4;
float voltage = 0.0;
int batteryPercent = 0;

// 'baroGraph', 8x12px
const unsigned char baroGraph [] PROGMEM = {
  0x07, 0x21, 0x21, 0x23, 0x21, 0x21, 0x23, 0xf9, 0x71, 0x23, 0x01, 0x07
};

// 'temp', 6x14px
const unsigned char tempGraph [] PROGMEM = {
  0x30, 0x48, 0x58, 0x48, 0x58, 0x48, 0x58, 0x48, 0x58, 0x48, 0x84, 0x84, 0x84, 0x78
};

// 'deg', 7x7px
const unsigned char deg [] PROGMEM = {
  0x38, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x38
};

// 'batteri', 6x13px
const unsigned char batteri [] PROGMEM = {
  0x78, 0xfc, 0x84, 0x84, 0x84, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc
};

// 'humidity', 6x11px
const unsigned char humidityGraph [] PROGMEM = {
  0x20, 0x50, 0x88, 0x88, 0x70, 0x00, 0x10, 0x28, 0x44, 0x44, 0x38
};

// 'hPa', 10x6px
const unsigned char hpa [] PROGMEM = {
  0x8e, 0x00, 0x8a, 0x00, 0x8e, 0x00, 0xe8, 0xc0, 0xa9, 0x40, 0xa8, 0xc0
};

//---------------------------------------------------FUNCTIONS----------------------------------------------------------------
//Display drawing: Temperature, pressure and voltage

void displayTemp()  {
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.drawBitmap(1, 0, tempGraph, 6, 14, BLACK);
  display.setCursor(14, 0);
  display.print(temp, 1); display.drawBitmap(64, 0, deg, 7, 7, BLACK);
  display.display();
}

void displayPressure()  {
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.drawBitmap(0, 16, baroGraph, 8, 12, BLACK);
  display.setCursor(14, 15);
  if (pressure < 1000) {
    display.print(pressure, 1); display.drawBitmap(74, 23, hpa, 10, 6, BLACK);  
  }
  else {
    display.print(pressure, 1);
  }
  
  display.display();
}

void displayVoltage() {
  display.setTextColor(BLACK);
  display.setTextSize(1);
  //display.drawBitmap(43, 33, batteri, 6, 13, BLACK);
  display.setCursor(53, 33);
  display.print(batteryPercent);display.print(" %");
  display.setCursor(53, 41);
  display.print(voltage); display.print("V");
  display.display();
}

void displayHumidity() {
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.drawBitmap(2, 34, humidityGraph, 6, 11, BLACK);
  display.setCursor(14, 33);
  display.print(int(humidity), 1); display.print("%");
  display.display();
}

void bme280Sleep() {
  Wire.beginTransmission(BME_280_I2C_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x48);
  Wire.endTransmission();
}

void bme280Wake() {
  Wire.beginTransmission(BME_280_I2C_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x4B);
  Wire.endTransmission();
}


//---------------------------------------------------SETUP----------------------------------------------------------------
void setup() {

  Wire.begin();

  //BME280 setup
  bme280.begin(BME_280_I2C_ADDRESS);

  pinMode(voltagePin, INPUT);

  //Display kontrast, rotasjon,
  display.begin();
  display.setContrast(57);
  display.setRotation(0);
  display.setTextWrap(false);
  display.clearDisplay();
  
}

//---------------------------------------------------LOOP----------------------------------------------------------------
void loop() {

wakeCounter++;

if (wakeCounter > 3) {
  
  wakeCounter = 0;
  
  bme280Wake();
    
  delay(50);

  analogRead(voltagePin);                                   //burn this reading
  voltage = ((analogRead(voltagePin)) / 1023.0) * 3.32;
  batteryPercent = int((voltage / 1.33 ) * 100.0);

  temp = bme280.readTemperature();
  pressure = (bme280.readPressure() / 100.0F);              //Convert pressure in floating point Pa to floating point hPa(100.0F = float 100.0)
  humidity = bme280.readHumidity();

  display.clearDisplay();
  displayTemp();
  displayPressure();
  displayVoltage();
  displayHumidity();
  display.drawFastVLine(50, 32, 16, BLACK);
  display.drawFastHLine(0, 30, 84, BLACK);
  display.display();

  bme280Sleep();
  }
  
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}