/*
ok so this is Jeffrey's arduino code for
his electric scooter which is GOING
to work, please thx

stuff to do: #FLAG

HARDWARE: so im using a fake arduino derivative
thing from an old Makerbot, this will be a learning
experience, wish i had an oescilloscope

CREDITS:
* thx to this dude https://github.com/PaulStoffregen/Encoder

I need to do this with PWM.

## PINZ:
* 2,3: encoder pins
* 12: motor, for now
* A6: read status. It would be cool to do some kind of weird
      analog multiplexing thing here to only use one pin for
      like 5 diff functions
* LED out: FAN pin = 14??

*/

#define ENCODER_USE_INTERRUPTS // what does this do
#include <Encoder.h>
#include "Arduino.h"
bool has_display = true; //is there a display or not
volatile bool show_speed;
// int speed_pin = 3; // throttle
int power_pin = 12; // powers motor via PWM
int led_pin = 17; // #FLAG probably change this to A7
int brake_pin = A6;
int brake_light_pin = 14; // ?

// ## PWM STUFF {
// TCCR1B = TCCR1B & 0b11111001; // something about a 32khz pwm?? idk

// } ## PWM STUFF


// ## ENCODER STUFF {
//      we'll be reading the encoder via interrupts
Encoder myEnc(2, 3);
volatile long position  = -999;
// } ## ENCODER STUFF


// don't know what this does  {
uint8_t off_level = 0;
uint8_t on_min_level = 96;
volatile unsigned long last_time;
uint16_t circumference = 200; //circumference of the wheel in mm
uint16_t damping_factor = 2; // filter parameter
volatile double speed = 0; // #FLAG no idea what the units for this are
volatile unsigned long prev_show_time;
volatile bool motor_state;
volatile double motor_speed;
volatile int pwm = 0;
volatile bool brake = true; // starting off with the brake ON
double max_speed = 10; // what is this in
volatile bool button_ok = true;
// } // dont knpw what this does

void display_speed() {
  if (has_display) {
    prev_show_time = millis();
    Serial.println(speed);
  }
}

void update_speed() { // looks like there's a 'tick' that happens every
// time the wheel goes round. lasers, ?
unsigned long new_time = millis();
unsigned long diff = new_time - last_time;
double new_speed = circumference/diff; //speed in mm per millisecond
speed += (new_speed - speed) / damping_factor; //LPF damping
last_time = new_time; // pretty cool, got a filter
Serial.println("update_speed");
if (show_speed) {
  display_speed();
}
}

void lightDance()
{ // this is where the fun LED updating happens
// if brake is on, turn brake light on
if(brake){ digitalWrite(brake_light_pin, HIGH);} // crank that
// we'll get turn signals and such sorted out a little later

}

void convertSpeed()
{
  // fuerte
  // read encoder val
  if(position<=0){ position = 0;}
  if(position>400){ position = 400;} // assuming 400 pulses / revolution

  pwm = map(position, 0, 400, 0,255);
  Serial.println(pwm);
}

void power_motor()
{
  convertSpeed();
  motor_speed = min(speed, max_speed);
  //turn relay switch to motor
  // digitalWrite(power_pin, HIGH); // maybe power a running light here?
  Serial.println("More power!!");
  // delay(100); // need to figure out timing
  analogWrite(power_pin, motor_speed);
}



void stop_motor() {
  digitalWrite(led_pin, LOW);
  motor_state = false;
  motor_speed = 0;
  //turn relay switch off.
  //I think this should be done immediately to start regen -> doesn't really
  // work for li-ion, route energy elsewhere plz (maybe to capacitor for lights?)
}


void setup()
{
  cli(); // stop interrupt
  pinMode(power_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  pinMode(brake_pin, INPUT);
  // Timer0 stuff {
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCNT0  = 0;//initialize counter value to 0
  TCCR0B = 0;// same for TCCR0B
  OCR0A = 7;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // 1024 prescaler:
  TCCR0B |= (1 << CS02) | (0 << CS01) | (1 << CS00); // see page 87 of atmega328p datasheet
  // http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
  // this mode sets the 1024 bit prescaler
  TIMSK0 |= (1 << OCIE0A); // page 88, timer/counter interrupt mask register
  //  "The corresponding interrupt is executed if a compare match in Timer/Counter0 occurs"
  // } timer stuff


  delay(1000);

  Serial.begin(115200);
  Serial.println("hi");
  last_time = 0;
  motor_state = false;
  show_speed = false;
  motor_speed = 0;
  display_speed();
  Serial.println("finished setup");
  sei();//allow interrupts
}

ISR(TIMER0_COMPA_vect){//timer0 interrupt 2kHz reads encoder + brake
  long newPos = myEnc.read();
  if (newPos != position) {
    position = newPos;
    // Serial.println(position);
  }
   // if brake pin is high, REPORT that
   brake = digitalRead(brake_pin); // slow
}


void loop()
{
  if (has_display) {
    unsigned long cur_time = millis();
    if (cur_time - prev_show_time >= 700) {
      show_speed = true;
    }
  }
  // brake = false; // #DEBUG
  // see if brake is on
  if(brake==false){
    power_motor();
  }
  else{
    stop_motor();
  }
  lightDance();
}
