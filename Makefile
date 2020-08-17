
BOARD                  := nrf52dk_nrf52832

BOOTLOADER_BUILD_DIR   := mcuboot
BOOTLOADER_SRC_DIR     := bootloader/mcuboot/boot/zephyr
BOOTLOADER_KEY         := bootloader/mcuboot/root-rsa-2048.pem

APP_BUILD_DIR          := app
APP_SRC_DIR            := zephyr/samples/basic/blinky 

SIGNED_HEX             := $(abspath build_${APP_BUILD_DIR}/zephyr/zephyr.signed.hex)
SIGNED_BIN             := $(abspath build_${APP_BUILD_DIR}/zephyr/zephyr.signed.bin)

.PHONY: build
build: bootloader app
	@echo Done!
	@echo "hex: ${SIGNED_HEX}"
	@echo "bin: ${SIGNED_BIN}"

.PHONY: flash 
flash: app_flash
	@echo Done!
	@echo "hex: ${SIGNED_HEX}"
	@echo "bin: ${SIGNED_BIN}"

.PHONY: bootloader 
bootloader:
	west build -p auto -d build_${BOOTLOADER_BUILD_DIR} -b ${BOARD} ${BOOTLOADER_SRC_DIR} 

.PHONY: bootloader_flash 
bootloader_flash: bootloader
	west flash -d build_${BOOTLOADER_BUILD_DIR}

.PHONY: app 
app:
	west build -p auto -d build_${APP_BUILD_DIR} -b ${BOARD} ${APP_SRC_DIR} -- -DCONFIG_BOOTLOADER_MCUBOOT=y
	west sign -t imgtool -d build_${APP_BUILD_DIR} -- --key ${BOOTLOADER_KEY} 

.PHONY: app_flash 
app_flash: app bootloader_flash
	west flash --build-dir build_${APP_BUILD_DIR} --hex-file ${SIGNED_HEX} --bin-file ${SIGNED_BIN} 

.PHONY: clean
clean:
	rm -rf build*