#include <Wire.h>

// Pins for the switches
#define di_s1 2
#define di_s2 3
#define di_motorDirection1 6

// Pins for the output pwm values
#define do_motorDirection1 7
#define do_motorDirection2 8
#define do_pwm 9

// Div
int const DELAY_WRITE_READ = 50;
int const MAX_DEVICES = 3;
bool debugMode;
double soll_pwm_in = 0; // deg
double soll_pwm_out = 0; // deg

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
TwoWire wireReceive = TwoWire();


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

	wireReceive.begin(8);  
	wireReceive.onReceive(receiveEvent); // register event
}

void receiveEvent(int howMany) {
	// TODO: Expand to 2 bytes
	// Get motor angle as 1 byte
	soll_pwm_in = wireReceive.read(); // Max value is 254 because we send with 1 byte and pwm goes from 0 - 255

	// Make it a negative value when direction input value is set to zero
	//if (digitalRead(di_motorDirection1) == LOW) soll_motor_angle_temp = soll_motor_angle_temp*(-1);
	//Serial.println(soll_motor_angle_temp);

	// Get and write diretion
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
	// Set incoming pwm value as output when both switches are not pushed (incoming signal is high) and the temperature is under max value
	//if ((digitalRead(pinData_switch_in_1[i]) == HIGH) & (digitalRead(pinData_switch_in_2[i]) == HIGH) & (temperatureIsOk(i))) analogWrite(pinData_pwm_out[i], analogRead(pinData_pwm_in[i]));
	//else analogWrite(pinData_pwm_out[i], 0);

	if ((digitalRead(di_s1) == HIGH) & (digitalRead(di_s2) == HIGH)) soll_pwm_out = soll_pwm_in; // No switch is pushed
	else if ((digitalRead(di_s1) == LOW) & (digitalRead(di_s2) == HIGH) & (digitalRead(di_motorDirection1) == HIGH))  soll_pwm_out = soll_pwm_in;// Switch in neg direction is pushed
	else if ((digitalRead(di_s1) == HIGH) & (digitalRead(di_s2) == LOW) & (digitalRead(di_motorDirection1) == LOW))  soll_pwm_out = soll_pwm_in;// Switch in pos direction is pushed
	else soll_pwm_out = 0; // Set to zero / stop motor

	analogWrite(do_pwm, (int)soll_pwm_out);

//bool temperatureIsOk(int thermistor) {
	// Reset previous readings
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
//}