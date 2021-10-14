#include "usb_connected_controller.h"
#include <apps/i18n.h>
#include "../global_preferences.h"

namespace USB
{

  static I18n::Message sUSBConnectedMessagesProtected[10] = {
      I18n::Message::USBConnected,
      I18n::Message::ConnectedMessage1,
      I18n::Message::ConnectedMessage2,
      I18n::Message::ConnectedMessage3,
      I18n::Message::BlankMessage,
      I18n::Message::ConnectedMessage4,
      I18n::Message::ConnectedMessage5,
      I18n::Message::ConnectedMessage6,
      I18n::Message::DfuStatus1,
      I18n::Message::DfuStatusProtected};

  static I18n::Message sUSBConnectedMessagesUnProtected[10] = {
      I18n::Message::USBConnected,
      I18n::Message::ConnectedMessage1,
      I18n::Message::ConnectedMessage2,
      I18n::Message::ConnectedMessage3,
      I18n::Message::BlankMessage,
      I18n::Message::ConnectedMessage4,
      I18n::Message::ConnectedMessage5,
      I18n::Message::ConnectedMessage6,
      I18n::Message::DfuStatus1,
      I18n::Message::DfuStatusUnProtected};

  static KDColor sUSBConnectedFGColors[] = {
      Palette::PrimaryText,
      Palette::PrimaryText,
      Palette::PrimaryText,
      Palette::AccentText,
      KDColorWhite,
      Palette::PrimaryText,
      Palette::PrimaryText,
      Palette::PrimaryText,
      Palette::PrimaryText,
      Palette::AccentText};

  static KDColor sUSBConnectedBGColors[] = {
      Palette::BackgroundHard,
      Palette::BackgroundHard,
      Palette::BackgroundHard,
      Palette::BackgroundHard,
      Palette::BackgroundHard,
      Palette::BackgroundHard,
      Palette::BackgroundHard,
      Palette::BackgroundHard,
      Palette::BackgroundHard,
      Palette::BackgroundHard};

  USBConnectedController::USBConnectedController() : ViewController(nullptr), step(0), already_init(false), m_messageView(sUSBConnectedMessagesProtected, sUSBConnectedFGColors, sUSBConnectedBGColors, 10)
  {
    if(already_init){
      return;
    }
    if (step == 0 && GlobalPreferences::sharedGlobalPreferences()->dfuStatus())
    {
      getMessageView()->update(sUSBConnectedMessagesUnProtected, sUSBConnectedFGColors, sUSBConnectedBGColors, 10);
    }
    // else if (step == 1 && GlobalPreferences::sharedGlobalPreferences()->dfuStatus())
    // {
    //   getMessageView()->update(sUSBConnectedMessages2UnProtected, sUSBConnectedFGColors, sUSBConnectedBGColors, 10);
    // }
    // else if (step == 1 && !GlobalPreferences::sharedGlobalPreferences()->dfuStatus())
    // {
    //   getMessageView()->update(sUSBConnectedMessages2Protected, sUSBConnectedFGColors, sUSBConnectedBGColors, 10);
    // }
    already_init = true;
  }

  // bool USBConnectedController::handleEvent(Ion::Events::Event event)
  // {
  //   if (event.isKeyboardEvent())
  //   {
  //     // Ne fonctionne pas pour l'instant fait bugger les events
  //     // if (step == 0)
  //     // {
  //     //   if (GlobalPreferences::sharedGlobalPreferences()->dfuStatus())
  //     //   {
  //     //     getMessageView()->update(sUSBConnectedMessagesUnProtected, sUSBConnectedFGColors, sUSBConnectedBGColors, 10);
  //     //   }
  //     //   else
  //     //   {
  //     //     getMessageView()->update(sUSBConnectedMessagesProtected, sUSBConnectedFGColors, sUSBConnectedBGColors, 10);
  //     //   }
  //     //   step = 1;
  //     // }
  //     // else
  //     // {
  //     //   if (GlobalPreferences::sharedGlobalPreferences()->dfuStatus())
  //     //   {
  //     //     getMessageView()->update(sUSBConnectedMessages2UnProtected, sUSBConnectedFGColors, sUSBConnectedBGColors, 10);
  //     //   }
  //     //   else
  //     //   {
  //     //     getMessageView()->update(sUSBConnectedMessages2Protected, sUSBConnectedFGColors, sUSBConnectedBGColors, 10);
  //     //   }
  //     //   step = 0;
  //     // }
  //     return false;
  //   }
  //   return false;
  // }

}
