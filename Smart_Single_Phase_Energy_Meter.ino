#include <LiquidCrystal.h>
#include <math.h>

// =====================================================
// LCD CONNECTIONS
// RS, E, D4, D5, D6, D7
// =====================================================
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


// =====================================================
// SENSOR PINS
// =====================================================
const int voltagePin = A0;   // ZMPT101B
const int currentPin = A1;   // ACS712


// =====================================================
// ADC SETTINGS
// =====================================================
const float ADC_REF = 5.0;
const int ADC_MAX = 1023;


// =====================================================
// CALIBRATION FACTORS
// =====================================================
// These MUST be determined experimentally.
//
// Example:
// If reference voltage = 230 V
// and Arduino initially reads 218 V:
//
// voltageCalibration = 230 / 218
//                    = 1.055
//
// Start with 1.0 and calibrate later.

float voltageCalibration = 1.0;
float currentCalibration = 1.0;


// =====================================================
// ACS712 SENSITIVITY
// =====================================================
// Select according to your ACS712:
//
// 5A  = 0.185 V/A
// 20A = 0.100 V/A
// 30A = 0.066 V/A
// =====================================================

float currentSensitivity = 0.100;   // ACS712-20A


// =====================================================
// NUMBER OF SAMPLES
// =====================================================

const int samples = 1000;


// =====================================================
// ENERGY
// =====================================================

float energy_kWh = 0.0;

unsigned long previousTime;


// =====================================================
// SETUP
// =====================================================

void setup()
{
  lcd.begin(16, 2);

  lcd.print("Energy Meter");
  delay(2000);

  lcd.clear();

  previousTime = millis();
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  float voltageRMS;
  float currentRMS;
  float activePower;
  float powerFactor;

  // Measure electrical parameters
  measureAC(
    voltageRMS,
    currentRMS,
    activePower,
    powerFactor
  );


  // ---------------------------------------------------
  // ENERGY CALCULATION
  // ---------------------------------------------------

  unsigned long currentTime = millis();

  float elapsedHours =
    (currentTime - previousTime) / 3600000.0;

  previousTime = currentTime;

  energy_kWh +=
    (activePower * elapsedHours) / 1000.0;


  // ---------------------------------------------------
  // DISPLAY VOLTAGE AND CURRENT
  // ---------------------------------------------------

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltageRMS, 1);
  lcd.print(" I:");
  lcd.print(currentRMS, 2);

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(activePower, 1);
  lcd.print("W");

  delay(2000);


  // ---------------------------------------------------
  // DISPLAY POWER FACTOR AND ENERGY
  // ---------------------------------------------------

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("PF:");
  lcd.print(powerFactor, 2);

  lcd.setCursor(0, 1);
  lcd.print("E:");
  lcd.print(energy_kWh, 3);
  lcd.print("kWh");

  delay(2000);
}


// =====================================================
// AC MEASUREMENT FUNCTION
// =====================================================

void measureAC(
  float &voltageRMS,
  float &currentRMS,
  float &activePower,
  float &powerFactor
)
{
  float voltageSum = 0.0;
  float currentSum = 0.0;

  float voltageSquareSum = 0.0;
  float currentSquareSum = 0.0;

  float powerSum = 0.0;


  // ---------------------------------------------------
  // FIND DC OFFSETS
  // ---------------------------------------------------

  for (int n = 0; n < samples; n++)
  {
    int voltageRaw = analogRead(voltagePin);
    int currentRaw = analogRead(currentPin);

    float voltage =
      (voltageRaw * ADC_REF) / ADC_MAX;

    float current =
      (currentRaw * ADC_REF) / ADC_MAX;

    voltageSum += voltage;
    currentSum += current;

    delayMicroseconds(200);
  }


  float voltageOffset =
    voltageSum / samples;

  float currentOffset =
    currentSum / samples;


  // ---------------------------------------------------
  // SAMPLE AC WAVEFORMS
  // ---------------------------------------------------

  voltageSquareSum = 0.0;
  currentSquareSum = 0.0;
  powerSum = 0.0;


  for (int n = 0; n < samples; n++)
  {
    int voltageRaw = analogRead(voltagePin);
    int currentRaw = analogRead(currentPin);


    // Convert ADC readings to sensor voltages
    float voltageSensor =
      (voltageRaw * ADC_REF) / ADC_MAX;

    float currentSensor =
      (currentRaw * ADC_REF) / ADC_MAX;


    // Remove sensor DC offsets
    float voltageAC =
      voltageSensor - voltageOffset;

    float currentAC =
      currentSensor - currentOffset;


    // -------------------------------------------------
    // RMS CALCULATION
    // -------------------------------------------------

    voltageSquareSum +=
      voltageAC * voltageAC;

    currentSquareSum +=
      currentAC * currentAC;


    // -------------------------------------------------
    // INSTANTANEOUS POWER
    // -------------------------------------------------

    float instantaneousPower =
      voltageAC * currentAC;

    powerSum += instantaneousPower;


    delayMicroseconds(200);
  }


  // ---------------------------------------------------
  // SENSOR RMS VALUES
  // ---------------------------------------------------

  float voltageSensorRMS =
    sqrt(voltageSquareSum / samples);

  float currentSensorRMS =
    sqrt(currentSquareSum / samples);


  // ---------------------------------------------------
  // CONVERT SENSOR VALUES TO REAL VALUES
  // ---------------------------------------------------

  voltageRMS =
    voltageSensorRMS *
    voltageCalibration;


  currentRMS =
    (currentSensorRMS /
     currentSensitivity) *
    currentCalibration;


  // ---------------------------------------------------
  // ACTIVE POWER
  // ---------------------------------------------------
  //
  // The power obtained directly from the sensor
  // waveforms is proportional to the average of
  // instantaneous voltage x current.
  //
  // The voltage and current calibration factors
  // are applied here.
  // ---------------------------------------------------

  activePower =
    (powerSum / samples)
    *
    voltageCalibration
    * currentCalibration
    / currentSensitivity;


  // ---------------------------------------------------
  // POWER FACTOR
  // ---------------------------------------------------

  float apparentPower =
    voltageRMS * currentRMS;


  if (apparentPower > 0.01)
  {
    powerFactor =
      activePower / apparentPower;
  }
  else
  {
    powerFactor = 0.0;
  }


  // Keep PF within practical limits
  if (powerFactor > 1.0)
    powerFactor = 1.0;

  if (powerFactor < -1.0)
    powerFactor = -1.0;
}