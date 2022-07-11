#include <bootloader/slots/slot.h>
#include <ion/src/device/shared/drivers/board.h>
#include <ion/src/device/shared/drivers/flash.h>
#include <bootloader/boot.h>

namespace Bootloader {

const KernelHeader* Slot::kernelHeader() const {
  return m_kernelHeader;
}

const UserlandHeader* Slot::userlandHeader() const {
  return m_userlandHeader;
}

[[ noreturn ]] void Slot::boot() const {

  if (m_address == 0x90000000) {
    // If we are booting from slot A, we need to lock the slot B
    Ion::Device::Flash::LockSlotB();
  } else {
    // If we are booting from slot B, we need to lock the slot A (and Khi)
    Ion::Device::Flash::LockSlotA();
  }

  // Configure the MPU for the booted firmware
  Ion::Device::Board::bootloaderMPU();
  
  // Jump
  Ion::Device::Board::bootloaderJumpToSlot(kernelHeader()->stackPointer(), kernelHeader()->startPointer());
  for(;;);
}

}
