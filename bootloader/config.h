#ifndef BOOTLOADER_CONFIG_H
#define BOOTLOADER_CONFIG_H

#include <stdint.h>

namespace Bootloader {

  class InternalConfig {
    private:
      uint32_t m_header;
      uint32_t m_version;
      uint32_t m_signature;
      
      uint32_t m_footer;
  };

}

#endif