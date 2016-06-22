# IOT

## Environment
This repository contains a snap shot (v1.5 June 2016) of [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk) and [Sming](https://github.com/SmingHub/Sming) for OSX.
Rather than a homebrew install, the setup sits in its own directory (opt) with all tools resolved via environment varables.
The following variables need setting via a source command. You will propbably need to install serial drivers for your Serail USB chip.

```bash
# use source ./setenv
export ESP_HOME=~/IOT/opt/esp-open-sdk
export SMING_HOME=~/IOT/opt/sming/Sming
export PATH=$PATH:~/IOT/opt/esp-open-sdk/utils
# export COM_PORT=/dev/cu.usbserial-A400f4um # Programmer
export COM_PORT=/dev/cu.SLAB_USBtoUART # NodeMCU
export ESPTOOL=$ESP_HOME/utils/esptool.py
export WIFI_SSID=yourssid
export WIFI_PWD=yourpassword
```

`make` and `make flash` should now work within a project directory. 

The projects use [influx](https://influxdata.com/) to log data. These are installed via brew on OSX.

## influxdb
Edit influxdb.conf to set up a UDP listner.
To have launchd start influxdb now and restart at login:
  `brew services start influxdb`
Or, if you don't want/need a background service you can just run:
  `influxd -config /usr/local/etc/influxdb.conf`

## chronograf
Edit chronograf.toml to allow internet access.
To run chronograf manually, you can specify the configuration file on the command line:
  `chronograf -config=/usr/local/etc/chronograf.toml`
To have launchd start homebrew/binary/chronograf now and restart at startup:
  `sudo brew services start homebrew/binary/chronograf`
Or, if you don't want/need a background service you can just run:
  `chronograf`

## kapacitor
To have launchd start kapacitor now and restart at login:
  `brew services start kapacitor`
Or, if you don't want/need a background service you can just run:
  `kapacitord -config /usr/local/etc/kapacitor.conf`
