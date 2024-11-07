SHELL      := /bin/bash
FQBN       := esp32:esp32:esp32
BUILD_PATH := ./build
SRCINO     := IOTServer.ino
ELF        := $(BUILD_PATH)/$(SRCINO).elf
SERIAL_DEV := /dev/ttyUSB0
BAUD_RATE  := 115200
WEB_OUT    := ./web/web.out
WEB_SRC    := ./web/page.js ./web/page.html

all: $(ELF)
.PHONY: all

$(ELF): $(SRCINO) IndexHtml.h
	arduino-cli compile --verbose --build-path $(BUILD_PATH) -b $(FQBN) -p $(SERIAL_DEV) .

upload: $(ELF)
	arduino-cli upload --verbose -b $(FQBN) -p $(SERIAL_DEV) --input-dir $(BUILD_PATH)

$(WEB_OUT): $(WEB_SRC)
	pushd web/ && \
	./printify > web.out && \
	./file2clipboard web.out && \
	popd

web: $(WEB_OUT)
.PHONY: web

clean:
	rm -vfr $(BUILD_PATH)/* $(WEB_OUT)

monitor:
	minicom --device=$(SERIAL_DEV) --baudrate=$(BAUD_RATE)
