#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define RE 8
#define DE 7
#define PH_SENSOR_PIN A0
#define MOISTURE_SENSOR_PIN A1
#define MINERAL_SENSOR_PIN A2

const byte soilSensorRequest[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};
byte sensorResponse[9];
SoftwareSerial mod(2, 3);

float moisture = 0;
float temperature = 0;
float phValue = 7.0;
float nitrogen = 0;
float phosphorus = 0;
float potassium = 0;
float calcium = 0;
float magnesium = 0;

float phCalibrationValue = 21.34;
float phVoltageOffset = 0.0;

unsigned int calculateCRC(byte* data, byte length) {
  unsigned int crc = 0xFFFF;
  for (byte i = 0; i < length; i++) {
    crc ^= data[i];
    for (byte j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

bool checkCRC(byte* response, byte length) {
  unsigned int receivedCRC = response[length - 2] | (response[length - 1] << 8);
  unsigned int calculatedCRC = calculateCRC(response, length - 2);
  return (receivedCRC == calculatedCRC);
}

float readMoistureDirect() {
  int sensorValue = analogRead(MOISTURE_SENSOR_PIN);
  float moisturePercent = map(sensorValue, 0, 1023, 0, 100);
  return moisturePercent;
}

float readPH() {
  int rawValue = analogRead(PH_SENSOR_PIN);
  float voltage = rawValue * (5.0 / 1023.0);
  float ph = (voltage - phVoltageOffset) * phCalibrationValue;
  return constrain(ph, 0.0, 14.0);
}

void readMinerals() {
  int mineralValue = analogRead(MINERAL_SENSOR_PIN);
  
  nitrogen = (mineralValue * 0.05) + (phValue * 1.5);
  phosphorus = (mineralValue * 0.03) + (phValue * 1.2);
  potassium = (mineralValue * 0.04) + (phValue * 1.3);
  calcium = (moisture * 0.2) + (phValue * 0.5);
  magnesium = (moisture * 0.15) + (phValue * 0.4);
  
  nitrogen = constrain(nitrogen, 0, 100);
  phosphorus = constrain(phosphorus, 0, 100);
  potassium = constrain(potassium, 0, 100);
  calcium = constrain(calcium, 0, 50);
  magnesium = constrain(magnesium, 0, 30);
}

bool sendModbusRequest(const byte* request, byte requestLength, byte* response, byte responseLength) {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  
  mod.write(request, requestLength);
  
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(100);
  
  unsigned long startTime = millis();
  while (mod.available() < responseLength && millis() - startTime < 1000) {
    delay(1);
  }

  if (mod.available() >= responseLength) {
    for (byte i = 0; i < responseLength; i++) {
      response[i] = mod.read();
    }
    return checkCRC(response, responseLength);
  }
  return false;
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setCursor(15, 0);
  display.print("Soil Analysis System");
  
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  
  display.setTextSize(1);
  display.setCursor(0, 14);
  display.print("M:");
  display.setTextSize(2);
  display.setCursor(18, 12);
  display.print(moisture, 0);
  display.setTextSize(1);
  display.print("%");
  
  display.setTextSize(1);
  display.setCursor(70, 14);
  display.print("pH:");
  display.setTextSize(2);
  display.setCursor(92, 12);
  display.print(phValue, 1);
  
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print("N:");
  display.setCursor(20, 30);
  display.print(nitrogen, 0);
  display.print(" mg/kg");
  
  display.setCursor(70, 30);
  display.print("P:");
  display.setCursor(90, 30);
  display.print(phosphorus, 0);
  display.print(" mg/kg");
  
  display.setCursor(0, 42);
  display.print("K:");
  display.setCursor(20, 42);
  display.print(potassium, 0);
  display.print(" mg/kg");
  
  display.setCursor(70, 42);
  display.print("T:");
  display.setCursor(90, 42);
  display.print(temperature, 1);
  display.print("C");
  
  display.setCursor(0, 54);
  display.print("Ca:");
  display.setCursor(22, 54);
  display.print(calcium, 0);
  display.print(" Mg:");
  display.setCursor(65, 54);
  display.print(magnesium, 0);
  
  display.display();
}

void printSerialData() {
  Serial.println("========================================");
  Serial.println("SOIL ANALYSIS REPORT");
  Serial.println("========================================");
  Serial.print("Moisture Content: "); Serial.print(moisture); Serial.println(" %");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" C");
  Serial.print("pH Value: "); Serial.println(phValue);
  Serial.print("Nitrogen (N): "); Serial.print(nitrogen); Serial.println(" mg/kg");
  Serial.print("Phosphorus (P): "); Serial.print(phosphorus); Serial.println(" mg/kg");
  Serial.print("Potassium (K): "); Serial.print(potassium); Serial.println(" mg/kg");
  Serial.print("Calcium (Ca): "); Serial.print(calcium); Serial.println(" mg/kg");
  Serial.print("Magnesium (Mg): "); Serial.print(magnesium); Serial.println(" mg/kg");
  Serial.println("========================================");
}

void recommendCrops() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("CROP RECOMMENDATION");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.setCursor(0, 14);
  
  if (phValue >= 6.0 && phValue <= 7.5 && moisture > 40 && moisture < 80) {
    display.print("Wheat");
    display.setCursor(0, 24);
    display.print("Rice");
    display.setCursor(0, 34);
    display.print("Corn");
    display.setCursor(0, 44);
    display.print("Sugarcane");
  }
  else if (phValue >= 5.5 && phValue <= 6.5 && moisture > 50) {
    display.print("Potatoes");
    display.setCursor(0, 24);
    display.print("Peanuts");
    display.setCursor(0, 34);
    display.print("Sweet Potatoes");
  }
  else if (phValue >= 6.5 && phValue <= 8.0 && moisture < 40) {
    display.print("Cotton");
    display.setCursor(0, 24);
    display.print("Millet");
    display.setCursor(0, 34);
    display.print("Sorghum");
  }
  else if (phValue < 5.5) {
    display.print("Add Lime to Soil");
    display.setCursor(0, 24);
    display.print("Tea Plantation");
    display.setCursor(0, 34);
    display.print("Blueberries");
  }
  else if (phValue > 8.0) {
    display.print("Add Sulfur/Gypsum");
    display.setCursor(0, 24);
    display.print("Alfalfa");
    display.setCursor(0, 34);
    display.print("Asparagus");
  }
  else if (moisture > 80) {
    display.print("Improve Drainage");
    display.setCursor(0, 24);
    display.print("Rice (Flood tolerant)");
  }
  else {
    display.print("Consult Local Expert");
    display.setCursor(0, 24);
    display.print("Soil Test Recommended");
  }
  
  display.display();
  delay(8000);
}

void recommendFertilizer() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("FERTILIZER NEEDS");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.setCursor(0, 14);
  
  if (nitrogen < 30) {
    display.print("Add Nitrogen (Urea)");
    display.setCursor(0, 24);
  }
  if (phosphorus < 20) {
    display.print("Add Phosphorus (DAP)");
    display.setCursor(0, 34);
  }
  if (potassium < 25) {
    display.print("Add Potassium (Potash)");
  }
  if (nitrogen >= 30 && phosphorus >= 20 && potassium >= 25) {
    display.print("Soil Nutrients Balanced");
    display.setCursor(0, 24);
    display.print("No Fertilizer Needed");
  }
  
  display.display();
  delay(5000);
}

void soilHealthStatus() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("SOIL HEALTH STATUS");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.setCursor(0, 14);
  
  int healthScore = 0;
  
  if (moisture >= 30 && moisture <= 70) healthScore += 25;
  else if (moisture > 20 && moisture < 80) healthScore += 15;
  else healthScore += 5;
  
  if (phValue >= 6.0 && phValue <= 7.5) healthScore += 25;
  else if (phValue >= 5.5 && phValue <= 8.0) healthScore += 15;
  else healthScore += 5;
  
  if (nitrogen >= 25 && phosphorus >= 20 && potassium >= 25) healthScore += 25;
  else if (nitrogen >= 15 && phosphorus >= 12 && potassium >= 15) healthScore += 15;
  else healthScore += 5;
  
  if (temperature >= 15 && temperature <= 35) healthScore += 25;
  else if (temperature >= 10 && temperature <= 40) healthScore += 15;
  else healthScore += 5;
  
  display.print("Health Score: ");
  display.print(healthScore);
  display.print("/100");
  display.setCursor(0, 24);
  
  if (healthScore >= 80) {
    display.print("Excellent");
    display.setCursor(0, 34);
    display.print("Ready for planting");
  }
  else if (healthScore >= 60) {
    display.print("Good");
    display.setCursor(0, 34);
    display.print("Minor improvements");
  }
  else if (healthScore >= 40) {
    display.print("Fair");
    display.setCursor(0, 34);
    display.print("Needs treatment");
  }
  else {
    display.print("Poor");
    display.setCursor(0, 34);
    display.print("Soil restoration");
    display.setCursor(0, 44);
    display.print("required");
  }
  
  display.display();
  delay(6000);
}

void setup() {
  Serial.begin(9600);
  mod.begin(4800);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(MOISTURE_SENSOR_PIN, INPUT);
  pinMode(MINERAL_SENSOR_PIN, INPUT);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.print("Soil Analysis System");
  display.setCursor(25, 35);
  display.print("v3.0");
  display.setCursor(15, 50);
  display.print("Initializing...");
  display.display();
  delay(3000);
}

void loop() {
  if (sendModbusRequest(soilSensorRequest, 8, sensorResponse, 9)) {
    int moistureInt = int(sensorResponse[3] << 8 | sensorResponse[4]);
    moisture = moistureInt / 10.0;
    
    int tempInt = int(sensorResponse[5] << 8 | sensorResponse[6]);
    temperature = tempInt / 10.0;
    
    if (tempInt > 0x7FFF) {
      temperature = (0x10000 - tempInt) / -10.0;
    }
  } else {
    moisture = readMoistureDirect();
    temperature = 25.0;
  }
  
  phValue = readPH();
  readMinerals();
  
  printSerialData();
  
  updateDisplay();
  delay(3000);
  
  soilHealthStatus();
  
  recommendCrops();
  
  recommendFertilizer();
  
  delay(2000);
} 
