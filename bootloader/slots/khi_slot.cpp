#include "khi_slot.h"
#include "slot_exam_mode.h"

Bootloader::KhiSlot::KhiSlot(const Bootloader::Slot * slot) : OmegaSlot(slot) {
  m_examAddress[0] = m_address + 0x1000;
  m_examAddress[1] = m_address + 0x3000;
}
