#ifndef BOOTLOADER_SLOTS_EXAM_MODE_H
#define BOOTLOADER_SLOTS_EXAM_MODE_H

extern "C" {
    #include <stdint.h>
  }

namespace Bootloader {
namespace ExamMode {

class SlotsExamMode{
  public:
    static uint8_t FetchSlotKhiExamMode();
    static uint8_t FetchSlotExamMode(uint32_t start, uint32_t end, bool omega);
};

enum class ExamMode : int8_t {
    Unknown = -1,
    Off = 0,
    Standard = 1,
    NoSym = 2,
    NoSymNoText = 3,
    Dutch = 4,
  };

}
}

#endif
