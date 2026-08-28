ARDUINO CODE
============

PROJECT
-------
Smart Single-Phase Energy Meter with Digital Calibration and Load Monitoring


DESCRIPTION
-----------

This Arduino program interfaces a ZMPT101B AC voltage sensor and an
ACS712 AC current sensor with an Arduino Uno.

The program samples the AC voltage and current waveforms and calculates:

1. RMS Voltage (Vrms)
2. RMS Current (Irms)
3. Active Power (P)
4. Power Factor (PF)
5. Total Energy Consumption (kWh)

The calculated values are displayed on a 16x2 LCD.



IMPORTANT VARIABLES
-------------------

Number of samples:

const int samples = 1000;

Voltage calibration:

float voltageCalibration = 1.0;

Current calibration:

float currentCalibration = 1.0;

ACS712 sensitivity for 20A version:

float currentSensitivity = 0.100;


ACS712 SENSITIVITY
------------------

5A  -> 0.185 V/A
20A -> 0.100 V/A
30A -> 0.066 V/A


PROGRAM OPERATION
-----------------

1. The Arduino reads the ZMPT101B and ACS712 analog outputs.

2. The program determines the DC offset of both sensors.

3. The DC offset is removed from the sampled signals.

4. The RMS voltage is calculated.

5. The RMS current is calculated.

6. Instantaneous power is calculated from voltage and current samples.

7. Average active power is calculated.

8. Power factor is calculated.

9. Energy consumption is accumulated using active power and elapsed time.

10. The results are displayed on the 16x2 LCD.


RMS VOLTAGE
-----------

Vrms = sqrt[(1/N) * Sum(vn^2)]


RMS CURRENT
-----------

Irms = sqrt[(1/N) * Sum(in^2)]


ACTIVE POWER
------------

Instantaneous power:

pn = vn * in

Average active power:

P = (1/N) * Sum(vn * in)


POWER FACTOR
------------

PF = P / (Vrms * Irms)


ENERGY
-------

Energy is calculated using:

E = Power * Time

The result is converted to kWh:

E(kWh) = P(W) * Time(hours) / 1000


DIGITAL CALIBRATION
-------------------

The calibration factors initially start at:

voltageCalibration = 1.0
currentCalibration = 1.0

These values must be adjusted using reference measurements.


VOLTAGE CALIBRATION EXAMPLE
---------------------------

Reference voltage = 230 V
Arduino reading = 218 V

Calibration factor:

230 / 218 = 1.055

Therefore:

float voltageCalibration = 1.055;


CURRENT CALIBRATION EXAMPLE
---------------------------

Reference current = 1.00 A
Arduino reading = 0.94 A

Calibration factor:

1.00 / 0.94 = 1.064

Therefore:

float currentCalibration = 1.064;


LCD DISPLAY
-----------

The LCD alternates between:

V: xxx.x I: x.xx
P: xxx.x W

and:

PF: x.xx
E: x.xxx kWh



CALIBRATION FORMULA
-------------------

Calibration Factor =
Reference Reading / Arduino Reading

Calibrated Reading =
Arduino Reading * Calibration Factor


IMPORTANT
---------

The voltage calibration factor is NOT 1

The correct value depends on the actual ZMPT101B module and its
adjustment.

The ACS712 sensitivity must match the actual ACS712 version.

The code should be calibrated before using the readings for
experimental results.


