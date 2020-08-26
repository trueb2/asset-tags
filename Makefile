
BOARD                  := nrf52dk_nrf52832

BOOTLOADER_BUILD_DIR   := mcuboot
BOOTLOADER_SRC_DIR     := bootloader/mcuboot/boot/zephyr
BOOTLOADER_KEY         := bootloader/mcuboot/root-rsa-2048.pem

APP_BUILD_DIR          := app
APP_SRC_DIR            := apps/asset-tag 

SIGNED_HEX             := $(abspath build_${APP_BUILD_DIR}/zephyr/zephyr.signed.hex)
SIGNED_BIN             := $(abspath build_${APP_BUILD_DIR}/zephyr/zephyr.signed.bin)

SMP_NAME               := QSIB_ASSET

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
	west flash --runner pyocd --build-dir build_${BOOTLOADER_BUILD_DIR}

.PHONY: app 
app:
	west build -p auto -d build_${APP_BUILD_DIR} -b ${BOARD} ${APP_SRC_DIR}
	west sign -t imgtool -d build_${APP_BUILD_DIR} -- --key ${BOOTLOADER_KEY} 

.PHONY: app_flash 
app_flash: app bootloader_flash
	west flash --runner pyocd --build-dir build_${APP_BUILD_DIR} --hex-file ${SIGNED_HEX} --bin-file ${SIGNED_BIN} 

pyocd-gdbserver:
	pyocd gdbserver

pyocd-gdbclient:
	${HOME}/opt/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-gdb -x .pyocd-gdbinit build_${APP_BUILD_DIR}/zephyr/zephyr.elf

jlink-gdbserver:
	JLinkGDBServer -Device NRF52 -If SWD -Speed 4000 -RTTChannel 0

jlink-gdbclient:
	${HOME}/opt/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-gdb -x .jlink-gdbinit build_${APP_BUILD_DIR}/zephyr/zephyr.elf

logs:
	JLinkRTTClient

smp-list:
	newtmgr --conntype ble --connstring ctlr_name=hci0,peer_name='${SMP_NAME}' image list

smp-dfu:
	newtmgr --conntype ble --connstring ctlr_name=hci0,peer_name='${SMP_NAME}' image list
	newtmgr --conntype ble --connstring ctlr_name=hci0,peer_name='${SMP_NAME}' image upload build_${APP_BUILD_DIR}/zephyr/zephyr.signed.bin
	newtmgr --conntype ble --connstring ctlr_name=hci0,peer_name='${SMP_NAME}' image list

.PHONY: clean
clean:
	rm -rf build*