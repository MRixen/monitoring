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


byte i2c_data_in[2] = { 0x00, 0x00};

// Div
int const DELAY_WRITE_READ = 50;
int const MAX_DEVICES = 3;
bool debugMode;
bool lockAction = false;

double soll_pwm_in = 0;
double soll_pwm_out = 0;

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
	//Serial.println(soll_pwm_in);
	Serial.println(digitalRead(di_motorDirection1));
}

void loop()
{
	if ((digitalRead(di_s1) == HIGH) & (digitalRead(di_s2) == HIGH)) soll_pwm_out = soll_pwm_in; // No switch is pushed
	else if ((digitalRead(di_s1) == LOW) & (digitalRead(di_s2) == HIGH) & (digitalRead(di_motorDirection1) == HIGH))  soll_pwm_out = soll_pwm_in;// Switch in neg direction is pushed
	else if ((digitalRead(di_s1) == HIGH) & (digitalRead(di_s2) == LOW) & (digitalRead(di_motorDirection1) == LOW))  soll_pwm_out = soll_pwm_in;// Switch in pos direction is pushed
	else soll_pwm_out = 0; // Set to zero / stop motor

	analogWrite(do_pwm, (int)soll_pwm_out);

	//Serial.println(soll_pwm_out);


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
}
