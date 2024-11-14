SHELL      := /bin/bash
FQBN       := esp32:esp32:esp32
BUILD_PATH := ./build
SRC_CPP    := CompressorControl.cpp Hysteresis.cpp
SRC_INO    := IOTServer.ino
SRC_HEADER := CompressorControl.h Hysteresis.h
SRC_WEB    := IndexHtml.h
SRC_MISC   := Secrets.h Pins.h
ELF        := $(BUILD_PATH)/$(SRC_INO).elf
SERIAL_DEV := /dev/ttyUSB0
BAUD_RATE  := 115200
WEB_SRC    := ./web/index.html ./web/index.css ./web/index.js

all: $(ELF)

$(ELF): $(SRC_CPP) $(SRC_HEADER) $(SRC_INO) $(SRC_MISC) $(SRC_WEB)
	arduino-cli compile --verbose --build-path $(BUILD_PATH) -b $(FQBN) -p $(SERIAL_DEV) .

$(SRC_MISC):
	@./secrets
	@./pins

$(SRC_WEB): $(WEB_SRC)
	pushd web && ./integrate && popd

upload: $(ELF)
	arduino-cli upload --verbose -b $(FQBN) -p $(SERIAL_DEV) --input-dir $(BUILD_PATH)

clean:
	rm -vfr $(BUILD_PATH)

dist: clean
	rm -f $(SRC_MISC) $(SRC_WEB)

monitor:
	minicom --device=$(SERIAL_DEV) --baudrate=$(BAUD_RATE)
