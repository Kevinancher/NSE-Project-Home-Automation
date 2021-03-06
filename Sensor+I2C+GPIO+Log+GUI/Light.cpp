#include "Light.h"
#include <ctime>
#include <wiringPi.h>
#include <softPwm.h>
#include <thread>
#include <chrono>

/* Program runs on the principle of a dead-timer
   Once the dead timer is reached, the lights turn off
   Once motion is detected, the timers will update itself.   
*/ 

using namespace std;



void TurnOn(int pin){	// Turn on Light on pin
	int Value=0;
	while(Value<100){  // Fade in lights with a 100 x 20ms loop. Meaning over 2 seconds the light will fade in
		Value+=1;
		softPwmWrite(pin,Value); // PWM write to slowly increase brightness
		this_thread::sleep_for(chrono::milliseconds(20)); // time-delay (20ms)
	}
	return;
}

void TurnOff(int pin) { // Turn off Light on pin
	int Value=100;
	while(Value>0){ // Fade out lights with a 100 x 20ms loop. Meaning the light will fade out over the span of 2 seconds
		Value-=1;
		softPwmWrite(pin,Value); // PWM write to slowly decrease brightness
		this_thread::sleep_for(chrono::milliseconds(20)); // time-delay (20ms)
	}
	return;
}



Light::Light(int pin):Pin(pin){
	softPwmCreate(pin,0,100); // Create a PWM instance for pin with a value between 0 and 100
}

bool Light::Check(){
	if((time(0) > Timer)&&(Active)){ // if the current time is less than the Timer (for expiry)
		Active=false; //(meaning the dead-timer for the light has expired) and the Light is still active.
		thread turnoff(TurnOff, Pin); // Turn off the lights. 
		turnoff.detach();
		return 0; // Return 0 because the timer expired.
	}
	else if(Active) {
		return 1; // If light is still active (and on), return a 1
	}
	else
		return 0; // If the light is out, return 0
}

void Light::Set_Light(bool x) { 
	if((x) && (!Active)) { // If x is true and the lights are off. Turn on the lights
		Timer = time(0) + TimeOut; // current time + the time after which the lights will turn off sets the dead-time
		Active=true;
		thread turnon(TurnOn, Pin);
		turnon.detach();
	}
	else if((x) && (Active)) { // If x is true and the lights are on. Update the dead-timer (if lights are on, no need to turn them on)
		Timer = time(0) + TimeOut; // current time + the time after which the lights will turn off sets the dead-time
	}
	else if(!x)
		Timer=time(0)+3; // 3 seconds till lights turn off
}
