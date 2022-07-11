#ifndef _BOOTLOADER_INTERFACE_MENUS_HOME_H_
#define _BOOTLOADER_INTERFACE_MENUS_HOME_H_

#include <bootloader/interface/src/menu.h>
#include <bootloader/interface/menus/about.h>
#include <bootloader/interface/menus/installer.h>
#include <bootloader/slots/slot.h>

namespace Bootloader {
  class HomeMenu : public Menu {
    public:
      HomeMenu();

      void setup() override;
      void postOpen() override {};

      static AboutMenu * aboutMenu();
      static InstallerMenu * installerMenu();

    private:
      const char * getSlotOsText(const Slot * slot);
      const char * getSlotText(const Slot * slot);
      const char * getKernelText(const Slot * slot);
      const char * getVersionText(const Slot * slot);
  };
}

#endif
