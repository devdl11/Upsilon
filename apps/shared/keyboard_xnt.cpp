#include <assert.h>
#include "keyboard_xnt.h"

using namespace Shared;

constexpr char Shared::Keyboard_XNT::k_X[];
constexpr char Shared::Keyboard_XNT::k_N[];
constexpr char Shared::Keyboard_XNT::k_T[];
constexpr char Shared::Keyboard_XNT::k_THETA[];

Shared::Keyboard_XNT::XNT_KEY Shared::Keyboard_XNT::handleEvent(Ion::Events::Event event, Shared::Keyboard_XNT::AppsKeys app) {
  if (app != m_previous) {
    reset();
    m_previous = app;
  }
  if (m_toggle) {
    if (event != Ion::Events::XNT) {
      m_toggle = false;
      return OK_KEY();
    }
    m_index = m_index >= k_KEYS - 1 ? 0 : m_index + 1;
    return getKeyByIndex(m_index);
  } else {
    if (event != Ion::Events::XNT) {
      return OK_KEY();
    }
    m_toggle = !m_toggle;
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

void Shared::Keyboard_XNT::reset() {
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
