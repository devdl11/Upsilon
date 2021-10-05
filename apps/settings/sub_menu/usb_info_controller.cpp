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

  static I18n::Message text[] = {
      I18n::Message::UsbExplaination1,
      I18n::Message::UsbExplaination2,
      I18n::Message::UsbExplaination3};

  UsbInfoController::UsbInfoController(Responder *parentResponder) : GenericSubController(parentResponder),
                                                                     m_contentView(&m_selectableTableView),
                                                                     m_cell{},
                                                                     m_UsbCell(KDFont::LargeFont, KDFont::SmallFont)
  {
    for (int i = 0; i < k_maxNumberOfCells; i++)
    {
      m_cell[i].setMessage(text[i]);
      m_cell[i].setMessageFont(KDFont::LargeFont);
      m_cell[i].setAccessoryFont(KDFont::SmallFont);
      m_cell[i].setAccessoryTextColor(Palette::SecondaryText);
    }
  }

  bool UsbInfoController::handleEvent(Ion::Events::Event event)
  {
    I18n::Message childLabel = m_messageTreeModel->childAtIndex(selectedRow())->label();
    if ((Ion::Events::OK == event || Ion::Events::EXE == event) && childLabel == I18n::Message::UsbExplaination3)
    {
      if (!GlobalPreferences::sharedGlobalPreferences()->dfuStatus())
      {
        if (!GlobalPreferences::sharedGlobalPreferences()->isInExamMode())
        {
          Ion::LED::setColor(KDColorPurple);
          Ion::LED::setBlinking(500, 0.1f);
        }
        GlobalPreferences::sharedGlobalPreferences()->setDfuStatus(true);
        Container::activeApp()->displayWarning(I18n::Message::DfuWarning1, I18n::Message::DfuWarning2);
      }
      else
      {
        Container::activeApp()->displayWarning(I18n::Message::UsbSetting, I18n::Message::UsbAlreadyDeact);
      }
      return true;
    }

    return GenericSubController::handleEvent(event);
  }

  HighlightCell *UsbInfoController::reusableCell(int index, int type)
  {
    assert(type == 0);
    assert(index >= 0 && index < 3);
    return &m_cell[index];
  }

  int UsbInfoController::reusableCellCount(int type)
  {
    assert(type == 0);
    return 3;
  }

  void UsbInfoController::willDisplayCellForIndex(HighlightCell *celll, int index)
  {
    assert(index >= 0 && index <= k_maxNumberOfCells);
    GenericSubController::willDisplayCellForIndex(celll, index);

    MessageTableCellWithBuffer *myCell = (MessageTableCellWithBuffer *)celll;

    myCell->setMessage(text[index]);
  }

  void UsbInfoController::didEnterResponderChain(Responder *previousFirstResponder)
  {
    /* When a pop-up is dismissed, the exam mode status might have changed. We
    * reload the selection as the number of rows might have also changed. We
    * force to reload the entire data because they might have changed. */
    m_contentView.reload();

    // We add a message when the mode exam is on
    if (numberOfCautionLines() > 0)
    {
      I18n::Message cautionMessages[] = {I18n::Message::ExamModeWarning1, I18n::Message::ExamModeWarning2, I18n::Message::ExamModeWarning3};
      //m_contentView.setMessages(cautionMessages, numberOfCautionLines());
    }
    // --------------------------------------------------------------------------
  }

}
