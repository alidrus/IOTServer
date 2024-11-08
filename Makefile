SHELL      := /bin/bash
FQBN       := esp32:esp32:esp32
BUILD_PATH := ./build
SRCINO     := IOTServer.ino
ELF        := $(BUILD_PATH)/$(SRCINO).elf
SERIAL_DEV := /dev/ttyUSB0
BAUD_RATE  := 115200
WEB_SRC    := ./web/index.html ./web/index.css ./web/index.js

all: $(ELF)
.PHONY: all

IndexHtml.h: $(WEB_SRC)
	pushd web && ./integrate && popd

$(ELF): $(SRCINO) IndexHtml.h
	arduino-cli compile --verbose --build-path $(BUILD_PATH) -b $(FQBN) -p $(SERIAL_DEV) .

upload: $(ELF)
	arduino-cli upload --verbose -b $(FQBN) -p $(SERIAL_DEV) --input-dir $(BUILD_PATH)

clean:
	rm -vfr $(BUILD_PATH)/* IndexHtml.h

monitor:
	minicom --device=$(SERIAL_DEV) --baudrate=$(BAUD_RATE)
