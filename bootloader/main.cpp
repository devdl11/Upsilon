
#include <ion.h>
#include <assert.h>

#include <bootloader/boot.h>
#include <bootloader/interface/static/interface.h>
#include <bootloader/slots/slot.h>
#include <bootloader/slots/epsilon_slot.h>
#include <bootloader/slots/omega_slot.h>
#include <bootloader/slots/khi_slot.h>
#include <bootloader/slots/slots_manager.h>
#include <bootloader/slots/slot_exam_mode.h>
#include <bootloader/recovery.h>
#include <ion/src/device/shared/drivers/flash.h>

__attribute__ ((noreturn)) void ion_main(int argc, const char * const argv[]) {
  // Clear the screen
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColorBlack);
  // Initialize the backlight
  Ion::Backlight::init();

  // We check if there is a slot in exam_mode

  bool isSlotA = Bootloader::SlotsManager::A()->valid();

  if (isSlotA) {
    const Bootloader::Slot * slotA = Bootloader::SlotsManager::A();
    if(slotA->type() == Bootloader::SlotType::Epsilon && ((Bootloader::EpsilonSlot *)slotA)->isExamModeOn()) {
      slotA->boot();
    } else if (slotA->type() == Bootloader::SlotType::Omega && ((Bootloader::OmegaSlot *)slotA)->isExamModeOn()) {
      slotA->boot();
    }
  }

  bool isSlotB = Bootloader::SlotsManager::B()->valid();

  if (isSlotB) {
    const Bootloader::Slot * slotB = Bootloader::SlotsManager::B();
    if(slotB->type() == Bootloader::SlotType::Epsilon && ((Bootloader::EpsilonSlot *)slotB)->isExamModeOn()) {
      slotB->boot();
    } else if (slotB->type() == Bootloader::SlotType::Omega && ((Bootloader::OmegaSlot *)slotB)->isExamModeOn()) {
      slotB->boot();
    }
  }

  bool isSlotKhi = Bootloader::SlotsManager::Khi()->valid();

  if (isSlotKhi) {
    const Bootloader::Slot * slotKhi = Bootloader::SlotsManager::B();
    if(slotKhi->type() == Bootloader::SlotType::Khi && ((Bootloader::KhiSlot *)slotKhi)->isExamModeOn()) {
      slotKhi->boot();
    }
  }

  if (Bootloader::Recovery::hasCrashed()) {
    Bootloader::Recovery::recoverData();
  }

  Bootloader::Interface::drawLoading();

  // Boot the firmware
  Bootloader::Boot::boot();
}
