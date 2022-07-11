#include "slots_manager.h"
#include "slot.h"
#include "epsilon_slot.h"
#include "omega_slot.h"
#include "khi_slot.h"

const Bootloader::Slot * Bootloader::SlotsManager::A() {
  static Slot * RawA = new Slot(SlotA_Address);
  static Slot * A = getSlotFrom(RawA);
  return A;
}

const Bootloader::Slot * Bootloader::SlotsManager::B() {
  static Slot * RawB = new Slot(SlotB_Address);
  static Slot * B = getSlotFrom(RawB);
  return B;
}

const Bootloader::Slot * Bootloader::SlotsManager::Khi() {
  static Slot * RawKhi = new Slot(SlotKhi_Address);
  static Slot * Khi = getSlotFrom(RawKhi);
  return Khi;
}

Bootloader::Slot * Bootloader::SlotsManager::getSlotFrom(Slot * slot) {
  // NOTE : This is a dummy implementation. Maybe do something smarter later. 

  if (!slot->userlandHeader()->isOmega() && !slot->userlandHeader()->isUpsilon()) {
    return new EpsilonSlot(slot);
  } else if (slot->userlandHeader()->isUpsilon()) {
    return new OmegaSlot(slot);
  } else if (slot->userlandHeader()->isOmega() && slot->address() == SlotKhi_Address) {
    return new KhiSlot(slot);
  } else if (slot->userlandHeader()->isOmega()) {
    return new OmegaSlot(slot);
  } else {
    return slot;
  }
}