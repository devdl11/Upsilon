#include <bootloader/utility.h>
#include "kernel_header.h"

namespace Bootloader {

const char * KernelHeader::version() const {
  return m_version;
}

const char * KernelHeader::patchLevel() const {
  return m_patchLevel;
}

const bool KernelHeader::isValid() const {
  return m_header == Magic && m_footer == Magic;
}


const bool KernelHeader::isAboveVersion16 () const {
  int sum = Utility::versionSum(m_version, 2);
  char newVersion[] = "16";
  int min = Utility::versionSum(newVersion, 2);
  return sum >= min;
}


}
