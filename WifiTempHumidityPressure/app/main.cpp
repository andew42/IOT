#include <user_config.h>
#include <stdio.h>
#include <Wire.h>
#include <Libraries/BMP180/BMP180.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/OneWire/OneWire.h>
#include <Libraries/DS18S20/ds18s20.h>
#include <Libraries/DHT/DHT.h>

// Include D18x20 support (1 or 0)
#define D18X20 0

// The influx database UDP address and port
#define SERVER_IP IPAddress(192, 168, 0, 14)
//#define SERVER_IP IPAddress(172, 20, 10, 2)
#define SERVER_PORT 8089

// Class handles 0..8 D18x20 sensors (pin D6 GPIO12 4k7 pullup)
DS18S20 ReadTemp;

// Humidity and Temprature Sensor (pin D2 GPIO4 10K pullup)
DHT dht(4, DHT22);
bool dhtIintialised = false;

// BMP180 Pressure (CLK D3 GPIO0, DATA D4 GPIO2)
BMP180 barometer;
bool barometerConnected = false;

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
  // reading resulted in some packet loss, particularly for latter messages
  char msg[512], cid[16], value[32], id[32];
  msg[0] = 0;

  // Use the 8266 MAC ID for barometer and humidity
  ultoa_wp(system_get_chip_id(), cid, 16, 8, '0');

  // Read barometer temprature and pressure
  if (barometerConnected) {
    // e.g. temperature,id=f01234567 value=27.6875
    // Prepended f to id so it doesn't conflict with humidity id
    float bt = barometer.GetTemperature();
    strcat(msg, "temperature,id=f");
    strcat(msg, cid);
    strcat(msg, " value=");
    strcat(msg, dtostrf(bt, 0, 4, value));
    strcat(msg, "\n");

    // e.g. pressure,id=f01234567 value=101675
    long bp = barometer.GetPressure();
    strcat(msg, "pressure,id=f");
    strcat(msg, cid);
    strcat(msg, " value=");
    strcat(msg, ultoa_wp(bp, id, 10, 0, '0'));
  }

  // Read temprature and humitidy
  TempAndHumidity th;
  if (dht.readTempAndHumidity(th)) {

    // Add a new line if necessary
    if (msg[0])
      strcat(msg, "\n");

    // e.g. temperature,id=01234567 value=27.6875
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
  } else {
    Serial.printf("DHT Sensor Read Failed:%d\n", dht.getLastError());
  }

#if D18X20
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
#endif

  // Send the message
  send(msg);

#if D18X20
  // Next measure result after 1.2 seconds * number of sensors
  ReadTemp.StartMeasure();
#endif
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

// Barometer setup
void InitBarometer() {
  if (barometer.EnsureConnected()) {
    Serial.println("Connected to BMP180");

    // Reset the device to ensure a clean start
    barometer.SoftReset();

    // Initialize and pull the calibration data
    barometer.Initialize();

    barometerConnected = true;
  } else {
    Serial.println("Could not connect to BMP180");
    barometerConnected = false;
  }
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

  // Wire is I2C for BMP180
  Wire.begin();
  barometer = BMP180();
  InitBarometer();

#if D18X20
  // Start reading temperature on pin 12 (D6) (takes 1.2s per sensor)
  ReadTemp.Init(12);
  ReadTemp.StartMeasure();
#endif

  // Read new temperature every 15 seconds
  procTimer.initializeMs(15000, readData).start();
}
