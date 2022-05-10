include ion/src/device/n0110/boot/Makefile

ion_device_src += $(addprefix ion/src/device/n0110/drivers/, \
  cache.cpp \
  external_flash.cpp \
  led.cpp \
  power.cpp \
  reset.cpp \
  usb.cpp \
  board.cpp \
  bldata.cpp \
)

ion_device_src += $(addprefix ion/src/device/n0110/, \
  platform_info.cpp \
)

LDSCRIPT ?= ion/src/device/n0110/flash.ld
