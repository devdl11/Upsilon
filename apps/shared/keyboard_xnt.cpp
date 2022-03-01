#include <assert.h>
#include "keyboard_xnt.h"
#include "../../escher/include/escher/container.h"
#include "../../escher/include/escher/app.h"

using namespace Shared;

constexpr char Shared::Keyboard_XNT::k_X[];
constexpr char Shared::Keyboard_XNT::k_N[];
constexpr char Shared::Keyboard_XNT::k_T[];
constexpr char Shared::Keyboard_XNT::k_THETA[];

Shared::Keyboard_XNT::XNT_KEY Shared::Keyboard_XNT::handleEvent(Ion::Events::Event event) {
  App * currentApp = Container::activeApp();
  AppsKeys app = currentApp->getAppKey();
  if (app != m_previous) {
    startCleaning();
    finalizeCleaning();
    m_previous = app;
  }
  if (m_toggle) {
    if (event != Ion::Events::XNT) {
      m_toggle = false;
      return OK_KEY();
    }
    m_index = m_index >= k_KEYS - 1 ? 0 : m_index + 1;
    m_handle = false;
    return getKeyByIndex(m_index);
  } else {
    if (event != Ion::Events::XNT) {
      return OK_KEY();
    }
    m_toggle = true;
    m_handle = true;
    m_index = getAppIndexByApp(app).getIndex();
    return getKeyByIndex(m_index);
  }
}

Shared::Keyboard_XNT::XNT_KEY Shared::Keyboard_XNT::getKeyByIndex(int i) {
  switch (i) {
    case 0:
      return X_KEY();
    case 1:
      return N_KEY();
    case 2:
      return T_KEY();
    case 3:
      return O_KEY();
    default:
      return OK_KEY();
  }
}

void Shared::Keyboard_XNT::startCleaning() {
  if (m_reset) {
    return;
  }
  m_reset = true;
}

void Shared::Keyboard_XNT::finalizeCleaning() {
  if (!m_reset) {
    return;
  }
  m_reset = false;
  m_handle = false;
  m_toggle = false;
  m_index = 0;
  m_previous = AppsKeys::NONE;
}

Keyboard_XNT::AppIndex Keyboard_XNT::getAppIndexByApp(Keyboard_XNT::AppsKeys app) {
  assert(app != AppsKeys::NONE);
  switch (app) {
    case AppsKeys::POINCARE:
      return PoincareSys();
    case AppsKeys::SEQUENCE:
      return SequenceApp();
    case AppsKeys::FUNCTION:
      return FunctionApp();
    case AppsKeys::CALC:
      return CalcApp();
    case AppsKeys::PYTHON:
      return PythonApp();
    default:
      return DefaultApp();
  }
}

bool Keyboard_XNT::isXNTKey(const char *buffer) {
  return buffer == k_X || buffer == k_N || buffer == k_T || buffer == k_THETA;
}

Poincare::LayoutCursor::Direction Keyboard_XNT::getSelectionDirection() {
  App * currentApp = Container::activeApp();
  AppsKeys app = currentApp->getAppKey();
  if (app == AppsKeys::STATISTICS) {
    return Poincare::LayoutCursor::Direction::Right;
  } else {
    return Poincare::LayoutCursor::Direction::Left;
  }

}
