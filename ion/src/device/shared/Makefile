ifneq ($(MODEL), n0110)
  include ion/src/device/shared/usb/Makefile
else
  include ion/src/device/bootloader/usb/Makefile
endif
include ion/src/device/shared/drivers/Makefile

ion_device_src += $(addprefix ion/src/device/shared/, \
  events.cpp \
  stack.cpp \
)
