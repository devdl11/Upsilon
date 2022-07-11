#ifndef BOOTLOADER_SLOTS_KHI_SLOT_H
#define BOOTLOADER_SLOTS_KHI_SLOT_H

#include <stdint.h>
#include <bootloader/slots/omega_slot.h>

namespace Bootloader {

  // NOTE : In fact this subclass is useless since Khi link the exam buffer the same way as Omega. But having it here is more clear.

class KhiSlot : public OmegaSlot {
  public:
    KhiSlot(const Bootloader::Slot * slot);

    virtual SlotType type() const override { return SlotType::Khi; }
};
}

#endif