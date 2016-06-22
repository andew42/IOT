#include <user_config.h>
#include <SmingCore/SmingCore.h>

// This pin, GPIO1 (TX), flashes the blue LED on an ESP-01 module
#define LED_PIN 1

Timer procTimer;
bool state = true;

void blink()
{
	digitalWrite(LED_PIN, state);
	state = !state;
}

void init()
{
	pinMode(LED_PIN, OUTPUT);
	procTimer.initializeMs(1000, blink).start();
}
