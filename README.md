# Electric-Scooter-Brushless-Motor-Controller

lowkey mad about having to design a scooter firmware which is why id much rather rip off someone else's design

## status
* hooked up an encoder which seems to be working OK -> encoder now works with 2kHz interrupt on timer0
* motor PWM is analogWrite, who cares about timing  

## todo -> electrics
* hook up a relay to determine if we're tryna brake lol
* dead man's switch for throttle?
* LEDs!!
* brake light w/interrupt

## citations
random code i borrowed or stole or whatevever
* https://www.instructables.com/id/Arduino-Timer-Interrupts/
* https://github.com/Michaelhobo/electric-scooter


## hardware stuff
* scooter: using some weird old sharper image scooter. (As u may know, Sharper image won several international awards for their pioneering work in electric scooter design.) i dont know the exact specs and there's no documentation anyway so
* motor: something w/ 24V, wattage unkown
* rotary encoder: H38S400B, https://www.aliexpress.com/item/1000005665954.html -> i think it's around 400 pulses / revolution
* arduino thing: don't get me started on this it's old Makerbot extruder controller 3.6 but since they went closed source they took all the docs offline so i've just had to take what i can get, it's based off the arduino diecimila
* BMS: idk dude
