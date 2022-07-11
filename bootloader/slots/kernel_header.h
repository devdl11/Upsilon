#ifndef BOOTLOADER_SLOTS_KERNEL_HEADER_H
#define BOOTLOADER_SLOTS_KERNEL_HEADER_H

#include <stdint.h>
#include <bootloader/utility.h>

namespace Bootloader {

class KernelHeader {
public:
  const char * version() const;
  const char * patchLevel() const;
  const bool isValid() const;
  const bool isAboveVersion16() const;

  const uint32_t stackPointer() const {return m_stackPointer;}
  const uint32_t startPointer() const {return m_startPointer;}

private:
  KernelHeader();
  constexpr static uint32_t Magic = 0xDEC00DF0;
  const uint32_t m_unknown;
  const uint32_t m_signature;
  const uint32_t m_header;
  const char m_version[8];
  const char m_patchLevel[8];
  const uint32_t m_footer;
  const uint32_t m_stackPointer;
  const uint32_t m_startPointer;
};

}

#endif
