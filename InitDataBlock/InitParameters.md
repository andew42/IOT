The ESP-01 module I have have GD25Q80 flah chips 8Mbit (1MByte) roms
The Node MCU module I have has 32Mbit (4MByte) roms
http://www.esp8266.com/viewtopic.php?f=13&t=2506&start=10

The init data therefore needs to go at 0xfc000
https://nodemcu.readthedocs.io/en/dev/en/flash/

The init data is SDK version specific. It needs to be hacked to change ADC mode from External to VCC
http://ridiculousfish.com/hexfiend/

To flash battery check into a 8m device (ESP-01)
../opt/esp-open-sdk/utils/esptool.py -p /dev/cu.usbserial-A400f4um -b 115200 write_flash -ff 40m -fm qio -fs 8m 0xFC000 ./esp_init_data_default.vcc.adc.bin
To flash battery check into a 32m device (Node MCU)
../opt/esp-open-sdk/utils/esptool.py -p /dev/cu.SLAB_USBtoUART -b 115200 write_flash -ff 40m -fm qio -fs 32m 0x3FC000 ./esp_init_data_default.vcc.adc.bin

Terminal
python -m serial.tools.miniterm /dev/cu.usbserial-A400f4um 115200                 
