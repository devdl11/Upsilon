#ifndef BOOTLOADER_SLOTS_OMEGA_SLOT_H
#define BOOTLOADER_SLOTS_OMEGA_SLOT_H

#include <stdint.h>
#include <bootloader/slots/epsilon_slot.h>

namespace Bootloader {

class OmegaSlot : public EpsilonSlot {
  public:
    // ? Do the same as in EpsilonSlot ?
    OmegaSlot(const Bootloader::Slot * slot);

    virtual SlotType type() const override { return SlotType::Omega; }

    virtual bool isExamModeOn() override;
  
};
}

#endif