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
*


*/

#define ENCODER_USE_INTERRUPTS // what does this do
#include <Encoder.h>
#include "Arduino.h"
bool has_display = true; //is there a display or not
volatile bool show_speed;
// int speed_pin = 3; // throttle
int power_pin = 7;
int motor_relay_pin = 18; // #FLAG
// int esc_pin = 10; // ESC = electronic speed control
int led_pin = 17; // #FLAG probably change this to A7


// ## PWM STUFF {
 TCCR1B = TCCR1B & 0b11111001; // something about a 32khz pwm?? idk

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
volatile double speed; // #FLAG no idea what the units for this are
volatile unsigned long prev_show_time;
volatile bool motor_state;
volatile double motor_speed;
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

int get_esc_out() { // this likely converts to a speed controller
  uint16_t outval = map((int) motor_speed, 0, (int) max_speed, 127, 130); // why 127 -> 130
  return outval;
}

void power_motor() {
  digitalWrite(led_pin, HIGH);
  motor_state = true;
  motor_speed = min(speed, max_speed);
  //turn relay switch to motor
  digitalWrite(motor_relay_pin, HIGH);
  Serial.println("More power!!");
  delay(100);
  /* analogWrite(esc_pin, 130);  // honey we don't have an ESC
  */
}



void stop_motor() {
  digitalWrite(led_pin, LOW);
  motor_state = false;
  motor_speed = 0;
  //turn relay switch off.
  //I think this should be done immediately to start regen -> doesn't really
  // work for li-ion, route energy elsewhere plz (maybe to capacitor for lights?)
  digitalWrite(motor_relay_pin, LOW);
  analogWrite(esc_pin, 100);
}


void test_fall() { // doubt imma need this
  if (button_ok) {
    button_ok = false;
    Serial.print("test_fall triggered...");
    analogWrite(esc_pin, 90);
    Serial.print("writing analog esc value...");
    Serial.println(millis());
  }
}

void test_rise() { // ...or this
  Serial.println("test rise");
  if (!button_ok) {
    Serial.print("test_rise triggered...");
    button_ok = true;
    Serial.println(millis());
  }
}


void test_change() {
  Serial.println("test change");
  if (digitalRead(power_pin)) {
    if (!button_ok) {
      Serial.print("test_rise triggered...");
      analogWrite(esc_pin, 90);
      button_ok = true;
      Serial.println(millis());
    }
  } else {
    if (button_ok) {
      button_ok = false;
      Serial.print("test_fall triggered...");
      analogWrite(esc_pin, 96);
      Serial.print("writing analog esc value...");
      Serial.println(millis());
    }
  }
}

void setup()
{
  // pinMode(speed_pin, INPUT); // getting rid of this bc of encoder
  pinMode(power_pin, INPUT); // guessing this tells us if there's power??
  pinMode(motor_relay_pin, OUTPUT); // #FLAG: is this PWM
  // pinMode(esc_pin, OUTPUT); // still dont know what this does
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  digitalWrite(power_pin, HIGH);
  digitalWrite(motor_relay_pin, HIGH);
  analogWrite(esc_pin, 0);
  delay(1000);
  // analogWrite(esc_pin, 90);
  // If digitalPinToInterrupt breaks, here is the mapping:
  // p->i: 3->0

  // attachInterrupt(digitalPinToInterrupt(speed_pin), update_speed, FALLING);
  //attachInterrupt(digitalPinToInterrupt(power_pin), power_motor, RISING);

  //attachInterrupt(digitalPinToInterrupt(power_pin), stop_motor, FALLING);
  //attachInterrupt(4, test_rise, RISING);
  //attachInterrupt(4, test_fall, FALLING);
  attachInterrupt(digitalPinToInterrupt(4), test_change, CHANGE);

  Serial.begin(115200);
  Serial.println("hi");
  last_time = 0;
  speed = 0;
  motor_state = false;
  show_speed = false;
  motor_speed = 0;
  display_speed();
  Serial.println("finished setup");
}

long position  = -999;

void loop()
{

    long newPos = myEnc.read();
    if (newPos != position) {
      position = newPos;
    //  Serial.println(position); // need to map this to motor speed
    }

  if (has_display) {
    unsigned long cur_time = millis();
    if (cur_time - prev_show_time >= 700) {
      show_speed = true;
    }
  }
}
