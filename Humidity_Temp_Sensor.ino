#include "SparkFun_Si7021_Breakout_Library.h"

#define ARRAY_LENGTH(x)  (sizeof(x) / sizeof((x)[0])) 

IntervalTimer read_sensors_timer;
IntervalTimer show_digits_timer;

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
  { pinA, _NON, _NON, _NON, _NON, pinF, pinG }  // celsius
};

Weather sensor;

bool print_temp = false;
//---------------------------------------------------------------
void setup()
{
  Serial.begin(9600);   // open serial over USB at 9600 baud

  pinMode(LED, OUTPUT);   
  digitalWrite(LED, HIGH);   

  for(size_t i = 0; i < ARRAY_LENGTH(all_D_pins); i++) { 
    pinMode(all_D_pins[i], OUTPUT);   
  }
  for(size_t i = 0; i < ARRAY_LENGTH(all_number_pins); i++) { 
    pinMode(all_number_pins[i], OUTPUT);   
  }   
  
  pinMode(DP, OUTPUT);   
  
  reset_pins();

  sensor.begin();


  
  read_sensors_timer.begin(read_sensors, 4000000);
  read_sensors_timer.priority(254);

  show_digits_timer.begin(print_seven_segment, 10000);
  show_digits_timer.priority(200);

  humidity = sensor.getRH();
  tempf = sensor.readTempC();
}
//---------------------------------------------------------------
void loop()
{
  //Get readings from all sensors
  // print_seven_segment();
}

void read_sensors(){
  getWeather();
  // printInfo();
}

//---------------------------------------------------------------
void getWeather()
{
  if(print_temp) { tempf = sensor.getTempC(); }
  else { humidity = sensor.getRH(); }
  print_temp = !print_temp;
  // Measure Temperature from the HTU21D or Si7021
  // Temperature is measured every time RH is requested.
  // It is faster, therefore, to read it from previous RH
  // measurement with getTempC) instead with readTempC)
}
//---------------------------------------------------------------
void printInfo()
{
  //This function prints the weather data out to the default Serial Port
  Serial.print("Temp:");
  Serial.print(tempf);
  Serial.print("C, ");

  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.println("%");
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
  
    if(print_temp){
      show_digit(all_D_pins[0], 10, true, false);
    } else{
      show_digit(all_D_pins[0], temp_array[0], true, false);
    }
    delay(2);
    show_digit(all_D_pins[1], temp_array[1], true, false);
    delay(2);
    show_digit(all_D_pins[2], temp_array[2], true, true);
    delay(2);
    show_digit(all_D_pins[3], temp_array[3], true, false);
    delay(2);
}

/*
  Showing number 0-9 on a Common Anode 7-segment LED display
  Displays the numbers 0-9 on the display, with one second inbetween.
    A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   ---
    D
  This example code is in the public domain.
 */

void seven_segment(){
  
  for(size_t i = 0; i < ARRAY_LENGTH(all_D_pins); i++) { 
    show_digit(all_D_pins[i], 0, false, false);
  }
  delay(300);
  reset_pins(); 
  for(size_t i = 0; i < ARRAY_LENGTH(all_D_pins); i++) { 
    show_digit(all_D_pins[i], 1, false, false);
  }
  delay(300);
  reset_pins(); 
}

void reset_pins(){
  for(size_t i = 0; i < ARRAY_LENGTH(all_D_pins); i++) { 
    digitalWrite(all_D_pins[i], HIGH);   
  }
  for(size_t i = 0; i < ARRAY_LENGTH(all_number_pins); i++) { 
    digitalWrite(all_number_pins[i], LOW);   
  }  
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