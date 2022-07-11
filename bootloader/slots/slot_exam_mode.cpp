#include <bootloader/slots/slot_exam_mode.h>
#include <assert.h>
#include <ion.h>
#include <ion/src/device/shared/drivers/flash.h>

namespace Bootloader {
namespace ExamMode {

/* The exam mode is written in flash so that it is resilient to resets.
 * We erase the dedicated flash sector (all bits written to 1) and, upon
 * deactivating or activating standard, nosym or Dutch exam mode we write one, two or tree
 * bits to 0. To determine in which exam mode we are, we count the number of
 * leading 0 bits. If it is equal to:
 * - 0[3]: the exam mode is off;
 * - 1[3]: the standard exam mode is activated;
 * - 2[3]: the NoSym exam mode is activated;
 * - 3[3]: the Dutch exam mode is activated;
 * - 4[3]: the NoSymNoText exam mode is activated. */

/* significantExamModeAddress returns the first uint32_t * in the exam mode
 * flash sector that does not point to 0. If this flash sector has only 0s or
 * if it has only one 1, it is erased (to 1) and significantExamModeAddress
 * returns the start of the sector. */

constexpr static size_t numberOfBitsInByte = 8;

// if i = 0b000011101, firstOneBitInByte(i) returns 5
size_t numberOfBitsAfterLeadingZeroes(int i) {
  int minShift = 0;
  int maxShift = numberOfBitsInByte;
  while (maxShift > minShift+1) {
    int shift = (minShift + maxShift)/2;
    int shifted = i >> shift;
    if (shifted == 0) {
      maxShift = shift;
    } else {
      minShift = shift;
    }
  }
  return maxShift;
}



uint8_t * SignificantSlotExamModeAddress(uint32_t s, uint32_t e, bool o) {
  uint32_t * persitence_start_32 = (uint32_t *)s;
  uint32_t * persitence_end_32 = (uint32_t *)e;
  if (o) {
    assert((persitence_end_32 - persitence_start_32) % 4 == 0);
    while (persitence_start_32 < persitence_end_32 && *persitence_start_32 == 0x0) {
      // Scan by groups of 32 bits to reach first non-zero bit
      persitence_start_32++;
    }
    uint8_t * persitence_start_8 = (uint8_t *)persitence_start_32;
    uint8_t * persitence_end_8 = (uint8_t *)persitence_end_32;
    while (persitence_start_8 < persitence_end_8 && *persitence_start_8 == 0x0) {
      // Scan by groups of 8 bits to reach first non-zero bit
      persitence_start_8++;
    }
    if (persitence_start_8 == persitence_end_8
    // we can't toggle from 0[3] to 2[3] when there is only one 1 bit in the whole sector
    || (persitence_start_8 + 1 == persitence_end_8 && *persitence_start_8 == 1)) {
      assert(Ion::Device::Flash::SectorAtAddress(s) >= 0);
      Ion::Device::Flash::EraseSector(s);
      return (uint8_t *)s;
    }
    return persitence_start_8;
  } else {
    persitence_end_32 = persitence_end_32 - 1;
    while (persitence_end_32 -  (uint32_t)(10 / 8) >= persitence_end_32 && *persitence_end_32 == 0xFFFFFFFF) {
      persitence_end_32 -= 1;
    }
    uint8_t * start = reinterpret_cast<uint8_t *>(persitence_start_32);
    uint8_t * end = reinterpret_cast<uint8_t *>(persitence_end_32 + 1) - 1;
    while (end >= start + 2 && *end == 0xFF) {
      end -= 1;
    }
    return end - 1;
  }
}

uint8_t SlotsExamMode::FetchSlotExamMode(uint32_t start, uint32_t end, bool omega) {
  uint8_t * readingAddress = SignificantSlotExamModeAddress(start, end, omega);
  if (omega) {
    // Count the number of 0[3] before reading address
    uint32_t nbOfZerosBefore = ((readingAddress - (uint8_t *)start) * numberOfBitsInByte) % 4;
    // Count the number of 0[3] at reading address
    size_t numberOfLeading0 = (numberOfBitsInByte - numberOfBitsAfterLeadingZeroes(*readingAddress)) % 4;
    return (nbOfZerosBefore + numberOfLeading0) % 4;
  } else {
    return *((uint8_t *)readingAddress);
  }
}

}
}
