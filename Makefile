all: ..esp8266.esp8266.nodemcuv2.bin ..esp8266.esp8266.nodemcuv2.elf

upload: ..esp8266.esp8266.nodemcuv2.bin ..esp8266.esp8266.nodemcuv2.elf
	arduino-cli upload --verbose -b esp8266:esp8266:nodemcuv2 -p /dev/tty.usbserial-1410 .

..esp8266.esp8266.nodemcuv2.bin: IOTServer.ino
	arduino-cli compile --verbose -b esp8266:esp8266:nodemcuv2 .

..esp8266.esp8266.nodemcuv2.elf: IOTServer.ino
	arduino-cli compile --verbose -b esp8266:esp8266:nodemcuv2 .

edit: IOTServer.ino
	vim IOTServer.ino

clean:
	rm -f ..arduino.avr.uno.elf ..arduino.avr.uno.hex
