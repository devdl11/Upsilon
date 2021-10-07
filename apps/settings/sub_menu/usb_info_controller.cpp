#include "usb_info_controller.h"
#include "../../apps_container.h"
#include <apps/i18n.h>
#include <assert.h>
#include <cmath>
#include <poincare/preferences.h>
#include <apps/i18n.h>
#include "../../global_preferences.h"

using namespace Poincare;
using namespace Shared;

namespace Settings
{

  UsbInfoController::UsbInfoController(Responder *parentResponder) : GenericSubController(parentResponder),
                                                                     m_contentView(&m_selectableTableView)
  {
    for (int i = 0; i < k_maxSwitchCells; i++)
    {
      m_switchCells[i].setMessageFont(KDFont::LargeFont);
      //Ancien code au cas ou on souhaite ajouter d'autres éléments dans le menu
      // m_cell[i].setMessageFont(KDFont::LargeFont);
      // m_cell[i].setAccessoryFont(KDFont::SmallFont);
      // m_cell[i].setAccessoryTextColor(Palette::SecondaryText);
    }
  }

  bool UsbInfoController::handleEvent(Ion::Events::Event event)
  {
    if ((Ion::Events::OK == event || Ion::Events::EXE == event) && selectedRow() == 0)
    {
      if (!GlobalPreferences::sharedGlobalPreferences()->dfuStatus())
      {
        if (!GlobalPreferences::sharedGlobalPreferences()->isInExamMode())
        {
          Ion::LED::setColor(KDColorPurple);
          Ion::LED::setBlinking(500, 0.5f);
        }
        GlobalPreferences::sharedGlobalPreferences()->setDfuStatus(true);
        Container::activeApp()->displayWarning(I18n::Message::DfuWarning1, I18n::Message::DfuWarning2);
      }
      else
      {
        if (!GlobalPreferences::sharedGlobalPreferences()->isInExamMode())
        {
          Ion::LED::setColor(KDColorBlack);
        }
        GlobalPreferences::sharedGlobalPreferences()->setDfuStatus(false);
      }
      m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
      AppsContainer::sharedAppsContainer()->redrawWindow(true);
      return true;
    }

    if(GlobalPreferences::sharedGlobalPreferences()->dfuStatus()){
      Container::activeApp()->displayWarning(I18n::Message::UsbSetting, I18n::Message::USBSettingDeact);
      return true;
    }

    return GenericSubController::handleEvent(event);
  }

  HighlightCell *UsbInfoController::reusableCell(int index, int type)
  {
    assert(type == 2);
    if(type == 2){
      assert(index >= 0 && index < k_maxSwitchCells);
      return &m_switchCells[index];
    }
    return nullptr;
  }

  int UsbInfoController::reusableCellCount(int type)
  {
    assert(type == 2);
    if (type == 2){
      return k_maxSwitchCells;
    }
    return 0;
  }

  void UsbInfoController::willDisplayCellForIndex(HighlightCell *celll, int index)
  {
    GenericSubController::willDisplayCellForIndex(celll, index);

    if(index == 0){
      MessageTableCellWithSwitch *myCell = (MessageTableCellWithSwitch *)celll;
      SwitchView * mySwitch = (SwitchView *)myCell->accessoryView();
      mySwitch->setState(!GlobalPreferences::sharedGlobalPreferences()->dfuStatus());
    }

  }

int UsbInfoController::typeAtLocation(int i, int j) {
  switch (j) {
    case 0:
      return 2;
    default:
      return 1;
  }
}

  void UsbInfoController::didEnterResponderChain(Responder *previousFirstResponder)
  {
    m_contentView.reload();

    if (numberOfInfoLines() > 0)
    {
      I18n::Message infoMessages[] = {I18n::Message::USBE16_expl1, I18n::Message::USBE16_expl2, I18n::Message::USBE16_expl3};
      m_contentView.setMessages(infoMessages, numberOfInfoLines());
    }
  }

}
