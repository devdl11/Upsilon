#ifndef BOOTLOADER_SLOTS_SLOTS_MANAGER_H
#define BOOTLOADER_SLOTS_SLOTS_MANAGER_H

#include <stdint.h>
#include <bootloader/slots/slot.h>

namespace Bootloader 
{
class SlotsManager {
  public:
  SlotsManager();

  static const Slot * A();
  static const Slot * B();
  static const Slot * Khi();

  private:
  static const uint32_t SlotA_Address = 0x90000000;
  static const uint32_t SlotB_Address = 0x90400000;
  static const uint32_t SlotKhi_Address = 0x90180000;

  static Slot * getSlotFrom(Slot * slot);

};
}

#endif
