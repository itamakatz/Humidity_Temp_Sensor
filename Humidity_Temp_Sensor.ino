/*
	Common Anode 7-segment LED display
  
    A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   ---
    D
 */

#include "SparkFunHTU21D.h"

#define ARRAY_LENGTH(x)	(sizeof(x) / sizeof((x)[0])) 
#define UPDATE_7SEGMENT_MS 10
#define DISPLAY_TEMP_INTEVAL_MS 7000
#define DISPLAY_HUMIDITY_INTEVAL_MS 3000
#define READ_DATA_INTERVAL_MS 500 // 4000
// #define SERIAL_ENABLE

IntervalTimer read_sensors_timer;
IntervalTimer show_digits_timer;
IntervalTimer toggle_display_timer;

float humidity = 0;
float tempf = 0;

int LED = 13;

int pinA = 8;
int pinB = 12; // pin 7 on 7 segment
int pinC = 29;
int pinD = 31;
int pinE = 32; // 1 on 7 segment
int pinF = 9;
int pinG = 28;

int DP = 30;

int D1 = 27;
int D2 = 11;
int D3 = 10;
int D4 = 7;

int all_D_pins[] = { D1, D2, D3, D4 };
int all_number_pins[] = { pinA, pinB, pinC, pinD, pinE, pinF, pinG };

int _NON = -1;

int multidim_num_pins[][7] = {
	{ pinA, pinB, pinC, pinD, pinE, pinF, _NON }, // 0
	{ _NON, pinB, pinC, _NON, _NON, _NON, _NON }, // 1
	{ pinA, pinB, _NON, pinD, pinE, _NON, pinG }, // 2
	{ pinA, pinB, pinC, pinD, _NON, _NON, pinG }, // 3
	{ _NON, pinB, pinC, _NON, _NON, pinF, pinG }, // 4
	{ pinA, _NON, pinC, pinD, _NON, pinF, pinG }, // 5
	{ pinA, _NON, pinC, pinD, pinE, pinF, pinG }, // 6
	{ pinA, pinB, pinC, _NON, _NON, _NON, _NON }, // 7
	{ pinA, pinB, pinC, pinD, pinE, pinF, pinG }, // 8
	{ pinA, pinB, pinC, _NON, _NON, pinF, pinG }, // 9
	{ pinA, pinB, _NON, _NON, _NON, pinF, pinG }  // celsius
};

HTU21D sensor_HTU21D;

bool print_temp = true;
//---------------------------------------------------------------
void setup()
{
#ifdef SERIAL_ENABLE
	Serial.begin(9600); // open serial over USB at 9600 baud
#endif
	pinMode(LED, OUTPUT);	 
	digitalWrite(LED, HIGH);	 

	for(size_t i = 0; i < ARRAY_LENGTH(all_D_pins); i++) { pinMode(all_D_pins[i], OUTPUT); }
	for(size_t i = 0; i < ARRAY_LENGTH(all_number_pins); i++) { pinMode(all_number_pins[i], OUTPUT); }	 
	
	pinMode(DP, OUTPUT);	 
	
	reset_pins();

	sensor_HTU21D.begin();

	read_sensors_timer.begin(read_sensors, READ_DATA_INTERVAL_MS * 1000);
	read_sensors_timer.priority(254);

	toggle_display_timer.begin(toggleDisplay, DISPLAY_HUMIDITY_INTEVAL_MS * 1000);
	toggle_display_timer.priority(250);

	show_digits_timer.begin(print_seven_segment, UPDATE_7SEGMENT_MS * 1000);
	show_digits_timer.priority(200);

	read_sensors();
}

void loop(){ }

void read_sensors(){
	tempf = sensor_HTU21D.readTemperature();
	humidity = sensor_HTU21D.readHumidity();
	// if(print_temp) { tempf = sensor_HTU21D.readTemperature(); }
	// else { humidity = sensor_HTU21D.readHumidity(); }

#ifdef SERIAL_ENABLE
	Serial.println("Temp:" + String(tempf) + "C, " + "Humidity:" + String(humidity) + "%");
#endif
}

void toggleDisplay(){ 
	print_temp = !print_temp; 
	if(print_temp) { toggle_display_timer.update(DISPLAY_HUMIDITY_INTEVAL_MS * 1000); }
	else { toggle_display_timer.update(DISPLAY_TEMP_INTEVAL_MS * 1000); }
}

void print_seven_segment(){
	
	int temp_array [4];

	int print_sensor;
	if(print_temp) { print_sensor = (int) (tempf * 100); }
	else { print_sensor = (int) (humidity * 100); }

	for(size_t i = 0; i < ARRAY_LENGTH(temp_array); i++)
	{
		temp_array[i] = print_sensor % 10;
		print_sensor /= 10;
	}
	
	if(print_temp) { show_digit(all_D_pins[0], 10, true, false); } 
	else { show_digit(all_D_pins[0], temp_array[0], true, false); }

	delay(2);
	show_digit(all_D_pins[1], temp_array[1], true, false);
	delay(2);
	show_digit(all_D_pins[2], temp_array[2], true, true);
	delay(2);
	show_digit(all_D_pins[3], temp_array[3], true, false);
	delay(2);
}

void reset_pins(){
	for(size_t i = 0; i < ARRAY_LENGTH(all_D_pins); i++) { digitalWrite(all_D_pins[i], HIGH); }
	for(size_t i = 0; i < ARRAY_LENGTH(all_number_pins); i++) { digitalWrite(all_number_pins[i], LOW); }	
	digitalWrite(DP, LOW);
}

void show_digit(int D_pin, int number, bool reset, bool enable_dp){

	if(reset) { reset_pins(); }
	
	digitalWrite(D_pin, LOW); 
	if(enable_dp) { digitalWrite(DP, HIGH); } 

	for(size_t i = 0; i < ARRAY_LENGTH(multidim_num_pins[number]); i++) { 
		if(multidim_num_pins[number][i] == _NON) { continue; }
		digitalWrite(multidim_num_pins[number][i], HIGH);
	}
}