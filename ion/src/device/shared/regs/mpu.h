#ifndef REGS_MPU_H
#define REGS_MPU_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

class MPU {
public:
  class TYPER : Register32 {
  public:
    REGS_FIELD_R(IREGION, uint8_t, 23, 16);
    REGS_FIELD_R(DREGION, uint8_t, 15, 8);
    REGS_BOOL_FIELD_R(SEPARATE, 0);
  };

  class CTRL : Register32 {
  public:
    REGS_BOOL_FIELD(PRIVDEFENA, 2);
    REGS_BOOL_FIELD(HFNMIENA, 1);
    REGS_BOOL_FIELD(ENABLE, 0);
  };

  class RNR : Register32 {
  public:
    REGS_FIELD(REGION, uint8_t, 7, 0);
  };

  class RBAR : Register32 {
  public:
    void setADDR(uint32_t address) volatile { /* assert((address & 0b11111) == 0);*/ setBitRange(31, 5, address >> 5); }
    REGS_BOOL_FIELD(VALID, 4);
    REGS_FIELD(REGION, uint8_t, 3, 0);
  };

  class RASR : Register32 {
  public:
    REGS_BOOL_FIELD(XN, 28);
    enum class AccessPermission : uint8_t {
      NoAccess = 0,
      PrivilegedRO = 5,
      PrivilegedRW = 1,
      PrivilegedRWUnprivilegedRO = 2,
      RO = 6,
      RW = 3
    };
    REGS_FIELD(AP, AccessPermission, 26, 24);
    REGS_FIELD(TEX, uint8_t, 21, 19);
    REGS_BOOL_FIELD(S, 18); // Shareable
    REGS_BOOL_FIELD(C, 17); // Cacheable
    REGS_BOOL_FIELD(B, 16); // Buffereable
    REGS_FIELD(SRD, uint8_t, 15, 8);
    enum class RegionSize : uint8_t {
      _32B = 0b00100,
      _64B = 0b00101,
      _128B = 0b00110,
      _256B = 0b00111,
      _512B = 0b01000,
      _1KB = 0b01001,
      _2KB = 0b01010,
      _4KB = 0b01011,
      _8KB = 0b01100,
      _16KB = 0b01101,
      _32KB = 0b01110,
      _64KB = 0b01111,
      _128KB = 0b10000,
      _256KB = 0b10001,
      _512KB = 0b10010,
      _1MB = 0b10011,
      _2MB = 0b10100,
      _4MB = 0b10101,
      _8MB = 0b10110,
      _16MB = 0b10111,
      _32MB = 0b11000,
      _64MB = 0b11001,
      _128MB = 0b11010,
      _256MB = 0b11011,
      _512MB = 0b11100,
      _1GB = 0b11101,
      _2GB = 0b11110,
      _4GB = 0b11111
    };

    REGS_FIELD(SIZE, RegionSize, 5, 1);
    REGS_BOOL_FIELD(ENABLE, 0);
  };

  constexpr MPU() {};
  REGS_REGISTER_AT(TYPER, 0x0);
  REGS_REGISTER_AT(CTRL, 0x04);
  REGS_REGISTER_AT(RNR, 0x08);
  REGS_REGISTER_AT(RBAR, 0x0C);
  REGS_REGISTER_AT(RASR, 0x10);
private:
  constexpr uint32_t Base() const {
    return 0xE000ED90;
  };
};

constexpr MPU MPU;

}
}
}

#endif
