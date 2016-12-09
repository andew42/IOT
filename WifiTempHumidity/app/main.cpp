#include <user_config.h>
#include <stdio.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/OneWire/OneWire.h>
#include <Libraries/DS18S20/ds18s20.h>
#include <Libraries/DHT/DHT.h>

// The influx database UDP address and port
#define SERVER_IP IPAddress(192, 168, 0, 14)
//#define SERVER_IP IPAddress(172, 20, 10, 2)
#define SERVER_PORT 8089 // Influx
//#define SERVER_PORT 8099 // Gateway

// Class handles 0..8 D18x20 sensors
DS18S20 ReadTemp;

// Humidity and Temprature Sensor (pin 5)
DHT dht(5);
bool dhtIintialised = false;

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
  // Handle the humitidty sensor first
  if (!dhtIintialised) {
    // First time around initialise
    dhtIintialised = true;
    dht.begin();
    Serial.printf("DHT Sensor Initialised:%d\n", dht.getLastError());
  }

  // Send all sensor measurements as a single message, sending a message per
  // reading
  // resulted in some packet loss, particularly for the latter messages
  char msg[512];
  msg[0] = 0;

  // Read temprature and humitidy
  TempAndHumidity th;
  if (dht.readTempAndHumidity(th)) {
    // e.g. temperature,id=01234567 value=27.6875
    // ~50 characters
    char cid[16], value[32];
    ultoa_wp(system_get_chip_id(), cid, 16, 8, '0');
    strcat(msg, "temperature,id=");
    strcat(msg, cid);
    strcat(msg, " value=");
    strcat(msg, dtostrf(th.temp, 0, 4, value));
    strcat(msg, "\n");

    // e.g. humidity,id=01234567 value=27.6875
    // ~50 characters
    strcat(msg, "humidity,id=");
    strcat(msg, cid);
    strcat(msg, " value=");
    strcat(msg, dtostrf(th.humid, 0, 4, value));
  }

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

      // e.g. temperature,id=0123456789abcdef value=27.6875
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

  // Print out some system stuff
  Serial.printf("Chip ID:%d\n", system_get_chip_id());
  Serial.printf("Vcc:%d\n", system_get_vdd33());
  Serial.printf("SDK Version:%s\n", system_get_sdk_version());
  Serial.printf("CPU Frequency:%d\n", system_get_cpu_freq());
  Serial.printf("Flash Size Map:%d\n", system_get_flash_size_map());
  Serial.printf("spi_flash_get_id:%d\n", spi_flash_get_id());

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
