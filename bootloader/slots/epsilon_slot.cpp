#include <bootloader/slots/slot.h>
#include <bootloader/slots/epsilon_slot.h>
#include <bootloader/slots/slot_exam_mode.h>
#include <bootloader/utility.h>
#include <ion.h>
#include <math.h>

Bootloader::EpsilonSlot::EpsilonSlot(const Bootloader::Slot * slot, bool subclass) : Slot(slot->address()) {
  // We compute the epsilon version only once.
  const char * version = m_kernelHeader->version();
  char buffer[8] = {0};
  int bufferIndex = 0;
  for (int i = 0; i < 8; i++) {
    if (version[i] == '.') {
      continue;
    }
    buffer[bufferIndex++] = version[i];
  }
  buffer[bufferIndex] = '\0';
  m_epsilonVersion = Utility::atoi(buffer);

  if (subclass) {
    // EpsilonSlot is the parent class of another class, so we let the child set the exam address.
    return;
  }
  m_examAddress[0] = m_examAddress[1] = 0;

  // We compute the exam address only once.
  const uint32_t examAddress = m_userlandHeader->getExternalAppsFlashEnd();
  uint32_t endExamAddress = m_address + 0x10000;
  // FIXME: make the if work. 
  if ((uint8_t)log2(endExamAddress - examAddress) == 64) {
    m_examAddress[0] = examAddress;
    m_examAddress[1] = endExamAddress;
  } else {
    // We normally never reach this point. But if we do, we will have to analyse the slot in order to find the exam address.
    // TODO: implement this.

    m_examAddress[0] = examAddress;
    m_examAddress[1] = endExamAddress;
  }

}

uint32_t Bootloader::EpsilonSlot::getEpsilonVersion() {
  return m_epsilonVersion;
}

uint32_t * Bootloader::EpsilonSlot::getExamAddress() {
  return m_examAddress;
}

bool Bootloader::EpsilonSlot::isExamModeOn() {
  return (ExamMode::ExamMode)ExamMode::SlotsExamMode::FetchSlotExamMode(m_examAddress[0], m_examAddress[1], false) != ExamMode::ExamMode::Off && (ExamMode::ExamMode)ExamMode::SlotsExamMode::FetchSlotExamMode(m_examAddress[0], m_examAddress[1], false) != ExamMode::ExamMode::Unknown;
}