#include <user_config.h>
#include <stdio.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/OneWire/OneWire.h>
#include <Libraries/DS18S20/ds18s20.h>

// The influx database UDP address and port
#define SERVER_IP IPAddress(192, 168, 0, 14)
//#define SERVER_IP IPAddress(172, 20, 10, 2)
#define SERVER_PORT 8089

// Class handles 0..8 D18x20 sensors
DS18S20 ReadTemp;

// Trigger sample and send data
Timer procTimer;

// Wifi connection state
bool connected = false;

// Our UDP connection to influx
UdpConnection udp;

// Send UDP message
void send(char *msg) {
  Serial.println(msg);
  if (connected)
    udp.sendString(msg);
}

// Sends temperatures from last call and kick off next reading
void readData() {
  // If measurement is still in progress skip this cycle
  if (ReadTemp.MeasureStatus()) {
    Serial.println("Still measuring...");
    return;
  }

  // Report if no sensors connected
  if (ReadTemp.GetSensorsCount() == 0) {
    Serial.println("No sensors found");
    return;
  }

  // Send all tempratures as a single message, sending a message per reading
  // resulted in some packet loss, particularly for the latter messages
  char msg[512];
  msg[0] = 0;

  // foreach sensor
  for (uint8_t a = 0; a < ReadTemp.GetSensorsCount(); a++) {

    // This sensors 64bit sensor id
    uint64_t info = ReadTemp.GetSensorID(a);

    // Skip corrupt readings
    if (ReadTemp.IsValidTemperature(a)) {
      char id[32];

      // Add a new line if necessary
      if (msg[0])
        strcat(msg, "\n");

      // e.g. temperature,id=01234567890abcdef value=27.6875
      // ~50 characters
      strcat(msg, "temperature,id=");
      strcat(msg, ultoa_wp(info >> 32, id, 16, 8, '0'));
      strcat(msg, ultoa_wp(info & 0xffffffff, id, 16, 8, '0'));

      strcat(msg, " value=");
      strcat(msg, dtostrf(ReadTemp.GetCelsius(a), 0, 4, id));
    } else {
      // Should't be able to get here after first successful read
      Serial.println("Invalid temperature");
    }
  }

  // Send the message
  send(msg);

  // Next measure result after 1.2 seconds * number of sensors
  ReadTemp.StartMeasure();
}

// Connected to WiFi access point
void onConnected() {
  connected = true;

  Serial.println("=== WiFi CONNECTED ===");
  Serial.print(WifiStation.getIP().toString());
  Serial.println("=============================");

  // Set up UDP connection
  udp.connect(SERVER_IP, SERVER_PORT);
}

// Can't connect to WiFi access point
void connectFail() {
  connected = false;
  // TODO FLASH LED, RETRY?
  Serial.println("I'm NOT CONNECTED. Help!");
}

void init() {
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
