#include <FlashStorage.h> // Flash  storage library from https://github.com/cmaglie/FlashStorage
#include <Adafruit_DPS310.h>
#include <vector>

#define VBATTPIN A7 // Pin for battery voltage divider 
#define SMOOTHING 3 // Gives a refresh rate of ~ 1.6Hz
#define RETAIN 1<<11 // With this refresh rate, can store up to 21 minutes of data

// Data structure for storing stuff in flash memory 
typedef struct {
  long mils;
  double temp;
  double pressure;
  double alt;
  float batt;
  int csum;
} FlashVarsStruct;
FlashStorage(MEM, FlashVarsStruct);

Adafruit_DPS310 dps;
Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();

FlashVarsStruct data;
double pressure, temp, alt;
double max_alt;
int counter = 0;

double all_alts[RETAIN];
int delta[RETAIN];
long last_milli = 0;
int slot = 0;

void setup() {
  Serial.begin(9600);

  // Begin I2C on DPS310 sensor 
  Serial.println("DPS310");
  if (! dps.begin_I2C()) {             
    Serial.println("Failed to find DPS");
    while (1) yield();
  }
  Serial.println("DPS OK!");

  // Flash LED if sensor OK
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1500);
  digitalWrite(LED_BUILTIN, LOW);

  // Configures sensor to read at a rate of 64Hz, with an oversampling rate of 64x
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
}

void loop() {
  // If received data unload data 
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    
    if (command == "M") {         // Print max values
      data = MEM.read();
      Serial.println("MAX VALUES");
      Serial.print("alt (m): ");
      Serial.println(data.alt);
      Serial.print("pressure (Pa): ");
      Serial.println(data.pressure);
      Serial.print("tempurature (*C): ");
      Serial.println(data.temp);
      Serial.print("batt (V): ");
      Serial.println(data.batt);
      
    } else if (command == "A") {  // Print all data currently held 
      Serial.println("ALL DATA");
      for (int i = 0; i < RETAIN; i++) {
        Serial.print(all_alts[i]);
        Serial.print(",");
        Serial.println(delta[i]);
      }
      Serial.print("Loop index: ");  // Index for end of list 
      Serial.println(slot);
    }
  }

  
  sensors_event_t temp_event, pressure_event; // Unsure what this line does
  
  while (!dps.temperatureAvailable() || !dps.pressureAvailable()) {
    return; // wait until there's something to read
  }

  // Update rolling sums 
  dps.getEvents(&temp_event, &pressure_event);
  pressure += pressure_event.pressure;
  temp += temp_event.temperature;
  alt += dps.readAltitude(1014); // Calculates altitude using temperature and pressure, with input of sea level pressure
  counter ++;
  counter = counter % SMOOTHING;

  // If counter == 0, calculate averages and update variables
  if (counter == 0) {
    pressure /= SMOOTHING;
    temp /= SMOOTHING;
    alt /= SMOOTHING;
    if (alt > max_alt) {
      max_alt = alt;
      data.mils = millis();
      data.temp = temp;
      data.pressure = pressure;
      data.alt = alt;
      data.batt = getBattVoltage();
      data.csum = int(alt + pressure) % 0xff;
      MEM.write(data);
    }

    // Store smoothed data (altitudes and delta millis)
    all_alts[slot] = alt;
    delta[slot] = int(millis() - last_milli);
    last_milli = millis();
    slot++;
    slot %= RETAIN;
  }
}

// Read battery voltage 
float getBattVoltage() {
  float measuredvbat = analogRead(VBATTPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  return measuredvbat;
}
