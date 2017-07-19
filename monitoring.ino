#include <Wire.h>

// Pins for the switches
#define di_s1 2
#define di_s2 3
#define di_motorDirection1 6

// Pins for the output pwm values
#define do_motorDirection1 7
#define do_motorDirection2 8
#define do_pwm 9

#define I2C_ID_CAN_BUS 7
#define I2C_ID_PID_CONTROLLER 8
#define I2C_ID_MONITOR 9

// Div
byte i2c_data_in[2] = { 0x00, 0x00 };
int const DELAY_WRITE_READ = 50;
int const MAX_DEVICES = 3;
bool debugMode;
bool lockAction = false;
double soll_pwm_in = 0;
double soll_pwm_out = 0;
TwoWire wireReceive = TwoWire();

// Data for the thermistors
float average[MAX_DEVICES] = { 0,0,0 };
float temp_steinhart[MAX_DEVICES] = { 0,0,0 };
int const RESISTOR_NOMINAL = 100000;  // 100kOhm   
int const TEMP_NOMINAL = 25;  // temp. for nominal resistance (25 C)
int const MAX_SAMPLES = 5; // Samples to get average temperature
int const BETA_COEFF = 3950; // Beta coefficient
int const RESISTOR_SERIES = 10000;
int const DELAY_TIME = 1000;
int const MAX_TEMPERATURE = 50; // Max. temp. for the motors in C
int samples[MAX_SAMPLES];

// Data for current sensor
const int AVERAGE_CYCLES = 100;
const int MAX_CURRENT_STALL_BIG_MOTOR = 3.9; // Ampere
const int MAX_CURRENT_STALL_SMALL_MOTOR = 1.5; // Ampere

void setup()
{
	// Configure serial interface
	Serial.begin(9600);

	// USER CONFIGURATION
	debugMode = true;

	// Define I/Os
	pinMode(di_s1, INPUT);
	pinMode(di_s2, INPUT);
	pinMode(di_motorDirection1, INPUT);

	pinMode(do_motorDirection1, OUTPUT);
	pinMode(do_motorDirection2, OUTPUT);

	// Set reference voltage to 3.3 V for analog input (minimize noise)
	//analogReference(EXTERNAL);

	Wire.begin(I2C_ID_MONITOR);
	Wire.onReceive(receiveEvent); // register event
}

void receiveEvent(int numBytes) {
	soll_pwm_in = wireReceive.read();

	// Get and write direction
	if (digitalRead(di_motorDirection1) == LOW) {
		digitalWrite(do_motorDirection1, LOW);
		digitalWrite(do_motorDirection2, HIGH);
	}
	else
	{
		digitalWrite(do_motorDirection1, HIGH);
		digitalWrite(do_motorDirection2, LOW);
	}
}

void loop()
{
	if (endstopOk() & currentOk() & temperatureOk()) soll_pwm_out = soll_pwm_in;
	else soll_pwm_out = 0; // Stop motor

	analogWrite(do_pwm, (int)soll_pwm_out);
}

bool endstopOk() {
	if ((digitalRead(di_s1) == HIGH) & (digitalRead(di_s2) == HIGH)) return true; // No switch is pushed
	else if ((digitalRead(di_s1) == LOW) & (digitalRead(di_s2) == HIGH) & (digitalRead(di_motorDirection1) == HIGH))  return true;// Switch in neg direction is pushed
	else if ((digitalRead(di_s1) == HIGH) & (digitalRead(di_s2) == LOW) & (digitalRead(di_motorDirection1) == LOW))  return true;// Switch in pos direction is pushed
	else return false; // Set to zero / stop motor
}

bool temperatureOk() {
	// Reset previous readings
	//int thermistor
	//average[thermistor] = 0;
	//temp_steinhart[thermistor] = 0;

	// Get average resistance
	//for (int i = 0; i < MAX_SAMPLES; i++) {
	//	average[thermistor] += analogRead(pinData_thermistor_in[thermistor]);
	//	delay(10);
	//}
	//average[thermistor] /= MAX_SAMPLES;

	//if (debugMode)
	//{
	//	Serial.print("Average analog value[");
	//	Serial.print(thermistor);
	//	Serial.print("]: ");
	//	Serial.println(average[thermistor]);
	//}

	// Convert the value to resistance
	//average[thermistor] = 1023 / average[thermistor] - 1;
	//average[thermistor] = RESISTOR_SERIES / average[thermistor];

	//if (debugMode)
	//{
	//	Serial.print("Thermistor resistance[");
	//	Serial.print(thermistor);
	//	Serial.print("]: ");
	//	Serial.println(average[thermistor]);
	//}


	//temp_steinhart[thermistor] = average[thermistor] / RESISTOR_NOMINAL;     // (R/Ro)
	//temp_steinhart[thermistor] = log(temp_steinhart[thermistor]);                  // ln(R/Ro)
	//temp_steinhart[thermistor] /= BETA_COEFF;                   // 1/B * ln(R/Ro)
	//temp_steinhart[thermistor] += 1.0 / (TEMP_NOMINAL + 273.15); // + (1/To)
	//temp_steinhart[thermistor] = 1.0 / temp_steinhart[thermistor];                 // Invert
	//temp_steinhart[thermistor] -= 273.15;                         // convert to C

	//if (debugMode)
	//{
	//	Serial.print("Temperature in C[");
	//	Serial.print(thermistor);
	//	Serial.print("]: ");
	//	Serial.print(temp_steinhart[thermistor]);
	//}

	//if (temp_steinhart[thermistor] < MAX_TEMPERATURE) return true;
	//else return false;

	return true;
}

bool currentOk() {
	// ACS 712 5 Ampere Strom Sensor

	// Die Verbindungen
	// 3,3V > A5 
	// 5V > ACS VCC
	// GND > ACS GND
	// A1 > ACS Out
	// Den ACS712 Stromsensor auslesen
	// Sens ist im Datenblatt auf Seite 2 mit 185 angegeben.
	// Für meinen Sensor habe ich 186 ermittelt bei 5.0V Vcc.
	// Sens nimmt mit ca. 38 pro Volt VCC ab.
	//
	// 3,3V muss zu Analog Eingang 5 gebrückt werden.
	// Der Sensoreingang ist Analog 1
	//
	// Parameter mitteln : die Anzahl der Mittlungen

	float sense = 186.0;           // mV/A Datenblatt Seite 2
	float sensdiff = 39.0;         // sense nimmt mit ca. 39/V Vcc ab. 
	float vcc, vsensor, amp, ampmittel = 0;
	int i;

	for (i = 0; i < AVERAGE_CYCLES; i++) {
		vcc = (float) 3.30 / analogRead(5) * 1023.0;    // Versorgungsspannung ermitteln
		vsensor = (float)analogRead(1) * vcc / 1023.0; // Messwert auslesen
		vsensor = (float)vsensor - (vcc / 2);            // Nulldurchgang (vcc/2) abziehen
		sense = (float) 186.0 - ((5.00 - vcc)*sensdiff);  // sense für Vcc korrigieren 
		amp = (float)vsensor / sense * 1000;            // Ampere berechnen
		ampmittel += amp;                               // Summieren
	}

	if ((ampmittel / AVERAGE_CYCLES) <= MAX_CURRENT_STALL_BIG_MOTOR) return true;
	else return false;
}
