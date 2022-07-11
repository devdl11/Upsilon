#ifndef BOOTLOADER_SLOTS_EPSILON_SLOT_H
#define BOOTLOADER_SLOTS_EPSILON_SLOT_H

#include <stdint.h>
#include <bootloader/slots/slot.h>

namespace Bootloader {

class EpsilonSlot : public Slot {
  public:
    EpsilonSlot(const Bootloader::Slot * slot) : EpsilonSlot(slot, false) {};
    EpsilonSlot(const Bootloader::Slot * slot, bool subclass);

    virtual SlotType type() const override { return SlotType::Epsilon; }

    uint32_t getUserlandVersion();
    uint32_t getEpsilonVersion();
    uint32_t * getExamAddress();
    
    virtual bool isExamModeOn();
  protected:
    uint32_t m_epsilonVersion;
    uint32_t m_examAddress[2];
    
  
};
}

#endif