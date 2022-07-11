#include "omega_slot.h"
#include "slot_exam_mode.h"

Bootloader::OmegaSlot::OmegaSlot(const Bootloader::Slot * slot) : EpsilonSlot(slot, true) {
  m_examAddress[0] = m_address + 0x1000;
  m_examAddress[1] = m_address + 0x3000;
}

bool Bootloader::OmegaSlot::isExamModeOn() {
  return (ExamMode::ExamMode)ExamMode::SlotsExamMode::FetchSlotExamMode(m_examAddress[0], m_examAddress[1], true) != ExamMode::ExamMode::Off && (ExamMode::ExamMode)ExamMode::SlotsExamMode::FetchSlotExamMode(m_examAddress[0], m_examAddress[1], true) != ExamMode::ExamMode::Unknown;
}
