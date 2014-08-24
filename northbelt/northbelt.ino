#include <Wire.h>
#include <LSM303.h>
#include <EEPROM.h>

#define DEBUG 1

LSM303 compass;
LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32768, -32768, -32768};

int CALIBRATE_LOOPS = 500;
int VIBE_BASE = 4;
int VIBE_COUNT = 16;
int SMOOTHING_DEGREES = 5;
int LED_PIN = 13;

int EEPROM_CALIBRATED_ADDR = 0;
int EEPROM_CALIBRATION_VECTOR_ADDR = 1;

float CENTER_WEDGE = (360 / VIBE_COUNT) / 2;

int currentVibePin = VIBE_BASE;

int vibrateCycles = 0;
float heading = 0.0;
float lastVibratedHeading = 180.0;

void makeCalibrateStartVibePattern() {
  for (int i = VIBE_BASE; i < VIBE_BASE + VIBE_COUNT; i++) {
    digitalWrite(i, HIGH);
    delay(250);
    digitalWrite(i, LOW);
  }
}

void makeCalibrateDoneVibePattern() {
  for (int j = 0; j < 2; j++) {
    for (int i = VIBE_BASE; i < VIBE_BASE + VIBE_COUNT; i++) {
      digitalWrite(i, HIGH);
    }
    delay(250);
    for (int i = VIBE_BASE; i < VIBE_BASE + VIBE_COUNT; i++) {
      digitalWrite(i, LOW);
    }
  }
}

void dumpCalibrationData() {
    Serial.print("calibration data: ");
    Serial.print(running_min.x);
    Serial.print(" ");
    Serial.print(running_min.y);
    Serial.print(" ");
    Serial.print(running_min.z);
    Serial.print(", ");
    Serial.print(running_max.x);
    Serial.print(" ");
    Serial.print(running_max.y);
    Serial.print(" ");
    Serial.print(running_max.z);
    Serial.println();
}

void calibrate() {

  makeCalibrateStartVibePattern();
  
  int led = HIGH;
  
  for(int i = 0; i < CALIBRATE_LOOPS; i++) {
    compass.read();
    
    running_min.x = min(running_min.x, compass.m.x);
    running_min.y = min(running_min.y, compass.m.y);
    running_min.z = min(running_min.z, compass.m.z);
  
    running_max.x = max(running_max.x, compass.m.x);
    running_max.y = max(running_max.y, compass.m.y);
    running_max.z = max(running_max.z, compass.m.z);
    
    digitalWrite(LED_PIN, led);
    if (led == HIGH) {
      led = LOW;
    } else {
      led = HIGH;
    }
    
    delay(50);
  }

  if (DEBUG) {
    dumpCalibrationData();
  }
  
  makeCalibrateDoneVibePattern();
}

bool isCalibrated() {
  // Unwritten addresses have the value 255.
  // We write 128 when marking uncalibrated so we can
  // distinguish the two states.
  // We write 0 when calibration data is stored.
  int val = EEPROM.read(EEPROM_CALIBRATED_ADDR);
  if (DEBUG) {
    Serial.print("calibrated? ");
    Serial.println(val);
  }
  return val < 128;
}

int16_t readInt16FromEEPROM(int addr) {
  int16_t ret;
  byte* p = (byte*)(void*)&ret;
  for (int i = 0; i < sizeof(int16_t); i++) {
    *p++ = EEPROM.read(addr++);
  }
  return ret;
}

void writeInt16ToEEPROM(int addr, int16_t val) {
  byte* p = (byte*)(void*)&val;
  for (int i = 0; i < sizeof(int16_t); i++) {
    EEPROM.write(addr++, *p++);
  }
}

void readCalibrationData() {
  int addr = EEPROM_CALIBRATION_VECTOR_ADDR;
  running_min.x = readInt16FromEEPROM(addr);
  addr += sizeof(int16_t);
  running_min.y = readInt16FromEEPROM(addr);
  addr += sizeof(int16_t);
  running_min.z = readInt16FromEEPROM(addr);
  addr += sizeof(int16_t);
  
  running_max.x = readInt16FromEEPROM(addr);
  addr += sizeof(int16_t);
  running_max.y = readInt16FromEEPROM(addr);
  addr += sizeof(int16_t);
  running_max.z = readInt16FromEEPROM(addr);
  addr += sizeof(int16_t);
  
  if (DEBUG) {
    Serial.print("READ from EEPROM ");
    dumpCalibrationData();
  }
}

void writeCalibrationData() {
  int addr = EEPROM_CALIBRATION_VECTOR_ADDR;
  writeInt16ToEEPROM(addr, running_min.x);
  addr += sizeof(int16_t);
  writeInt16ToEEPROM(addr, running_min.y);
  addr += sizeof(int16_t);
  writeInt16ToEEPROM(addr, running_min.z);
  addr += sizeof(int16_t);
  writeInt16ToEEPROM(addr, running_max.x);
  addr += sizeof(int16_t);
  writeInt16ToEEPROM(addr, running_max.y);
  addr += sizeof(int16_t);
  writeInt16ToEEPROM(addr, running_max.z);
  addr += sizeof(int16_t);

  EEPROM.write(EEPROM_CALIBRATED_ADDR, 0);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  
  for (int i = 0; i < VIBE_COUNT; i++) {
    pinMode(VIBE_BASE + i, OUTPUT);
  }
  
  pinMode(LED_PIN, OUTPUT);

  if (DEBUG) {
    digitalWrite(LED_PIN, HIGH);
    delay(1500);
    Serial.println("DEBUG");
    digitalWrite(LED_PIN, LOW);
  }
  
  if (!isCalibrated()) {
    calibrate();
    writeCalibrationData();
  } else {
    readCalibrationData();
  }

  compass.m_min = running_min;
  compass.m_max = running_max;
}

void maybeVibrateHeading() {
  int motor = 0;
  int wedge = 0;
  if (abs(lastVibratedHeading - heading) > SMOOTHING_DEGREES) {
    lastVibratedHeading = heading;

    wedge = (((int)(heading + CENTER_WEDGE)) % 360);
    wedge = wedge / (360 / VIBE_COUNT);
    motor = wedge + VIBE_BASE;

    if (DEBUG) {
      Serial.print("vibe ");
      Serial.print(wedge);
      Serial.print(" ");
      Serial.print(motor);
      Serial.println();
    }
    digitalWrite(currentVibePin, LOW);
    digitalWrite(motor, HIGH);
    currentVibePin = motor;
    vibrateCycles = 8;
  }
}

void vibrateClock() {
  if (vibrateCycles > 0) {
    vibrateCycles--;
    if (vibrateCycles == 0) {
      digitalWrite(currentVibePin, LOW);
    }
  }
}

void loop() {
  compass.read();
  
  /*
   compass.heading((LSM303::vector<int>){0, 0, 1});
     => use the +Z axis as a reference (points directly through the PCB from
     silkscreened side to populated side).
  */
  heading = compass.heading((LSM303::vector<int>){0, 0, 1});
  maybeVibrateHeading();
  vibrateClock();
  
  if (DEBUG) {
    Serial.print("heading ");
    Serial.println(heading);
  }

  delay(100);
}
