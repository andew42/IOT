#include <user_config.h>
#include <stdio.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/OneWire/OneWire.h>
#include <Libraries/DS18S20/ds18s20.h>

DS18S20 ReadTemp;
Timer procTimer;
bool connected = false;
UdpConnection udp;

// Send UDP message
void send(char *msg)
{
	Serial.println(msg);
	if (connected)
		udp.sendString(msg);
}

// Read back temperature
void readData()
{
	uint8_t a;
	char msg[256];
	char id[32];

	if (!ReadTemp.MeasureStatus())  // the last measurement completed
	{
		if (ReadTemp.GetSensorsCount())   // is minimum 1 sensor detected ?
	  	{
	    	for(a=0;a<ReadTemp.GetSensorsCount();a++)   // prints for all sensors
	    	{
				// 64bit sensor id
	      		uint64_t info=ReadTemp.GetSensorID(a);
	      		if (ReadTemp.IsValidTemperature(a))   // temperature read correctly ?
	        	{
					//temperature,id=7215010c value=27.6875
					strcpy(msg, "temperature,id=");
					//strcat(msg, ltoa(info, id, 16));
					strcat(msg, ultoa_wp(info >> 32, id, 16, 8, '0'));
					strcat(msg, ultoa_wp(info & 0xffffffff, id, 16, 8, '0'));

					strcat(msg, " value=");
					strcat(msg, dtostrf(ReadTemp.GetCelsius(a), 0, 4, id));
					send(msg);
	        	}
	      		else
				{
					Serial.println("Invalid temperature NO ID");
				}
			}
			ReadTemp.StartMeasure();  // next measure, result after 1.2 seconds * number of sensors
		}
		else
			Serial.println("No valid Measure so far! wait please");
	}
}

// Connected to WiFi access point
void onConnected()
{
	connected = true;

	Serial.println("=== WiFi CONNECTED ===");
	Serial.print(WifiStation.getIP().toString());
	Serial.println("=============================");

	// Set up UDP connection
	// udp.connect(IPAddress(172, 20, 10, 2), 8089);
	udp.connect(IPAddress(192, 168, 0, 38), 8089);
	//udp.sendString("Hello!");
}

// Can't connect to WiFi access point
void connectFail()
{
	connected = false;
	// TODO FLASH LED, RETRY
	Serial.println("I'm NOT CONNECTED. Help!");
}

void init()
{
	// Debugging over serial @ 115200
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(true);

	// Kick of WiFi (wait 20s before giving up)
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);
	WifiStation.waitConnection(onConnected, 20, connectFail);

	// Start reading temperature on pin 2 (takes 1.2s per sensor)
    ReadTemp.Init(2);
	ReadTemp.StartMeasure();

	// Read new temperature every 15 seconds
	procTimer.initializeMs(15000, readData).start();
}
