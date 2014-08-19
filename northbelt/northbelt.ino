#include <Wire.h>
#include <LSM303.h>

LSM303 compass;
LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32768, -32768, -32768};

int CALIBRATE_LOOPS = 10000;
int VIBE_BASE = 4;
int VIBE_COUNT = 16;

float heading = 0.0;
float lastVibratedHeading = 180.0;

void calibrate() {
  
  for(int i = 0; i < CALIBRATE_LOOPS; i++) {
    compass.read();
    
    running_min.x = min(running_min.x, compass.m.x);
    running_min.y = min(running_min.y, compass.m.y);
    running_min.z = min(running_min.z, compass.m.z);
  
    running_max.x = max(running_max.x, compass.m.x);
    running_max.y = max(running_max.y, compass.m.y);
    running_max.z = max(running_max.z, compass.m.z);
    
    delay(100);
    }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  
  for (int i = 0; i < VIBE_COUNT; i++) {
    pinMode(VIBE_BASE + i, OUTPUT);
  }
  
  calibrate();

  compass.m_min = running_min;
  compass.m_max = running_max;
}

void maybeVibrateHeading() {
}

void loop() {
  compass.read();
  
  /*
  When given no arguments, the heading() function returns the angular
  difference in the horizontal plane between a default vector and
  north, in degrees.
  
  The default vector is chosen by the library to point along the
  surface of the PCB, in the direction of the top of the text on the
  silkscreen. This is the +X axis on the Pololu LSM303D carrier and
  the -Y axis on the Pololu LSM303DLHC, LSM303DLM, and LSM303DLH
  carriers.
  
  To use a different vector as a reference, use the version of heading()
  that takes a vector argument; for example, use
  
    compass.heading((LSM303::vector<int>){0, 0, 1});
  
  to use the +Z axis as a reference.
  */
  heading = compass.heading();
  maybeVibrateHeading();
  
  Serial.println(heading);
  delay(100);
}
