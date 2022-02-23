#ifndef SHARED_KEYBOARD_XNT_H
#define SHARED_KEYBOARD_XNT_H

#include "../../ion/include/ion/events.h"
#include "../../ion/include/ion/unicode/code_point.h"

namespace Shared {

class Keyboard_XNT {
public:
  static const unsigned long k_KEYS = 5;
  static constexpr char k_X = 'x';
  static constexpr char k_N = 'n';
  static constexpr char k_T = 't';

  class XNT_KEY {
  public:
    explicit XNT_KEY(const char key) : m_key(key) {}

    char getKey() const { return m_key; }
  private:
    const char m_key;
  };

  enum AppsKeys {
    PYTHON=0,
    CALC=0,
    FUNCTION=0,
    SEQUENCE=1,
    DEFAULT=0,
    POINCARE=0,
    NONE=-1
  };

  static const XNT_KEY X_KEY() { return XNT_KEY(k_X); };
  static const XNT_KEY N_KEY() { return XNT_KEY(k_N); };
  static const XNT_KEY T_KEY() { return XNT_KEY(k_T); };
  static const XNT_KEY O_KEY() { return XNT_KEY(UCodePointGreekSmallLetterTheta); };
  static const XNT_KEY OK_KEY() { return XNT_KEY(UCodePointNull); };

  Keyboard_XNT() : m_toggle(false), m_index(0), m_previous(AppsKeys::NONE) {}

  XNT_KEY handleEvent(Ion::Events::Event event, AppsKeys app);

  void setIndex(int i) { m_index = i; }
  int getIndex() const {return m_index; }
  bool isToggling() const { return m_toggle; }
  XNT_KEY getKeyByIndex(int i);
  void reset();

private:
  bool m_toggle;
  int m_index;
  AppsKeys m_previous;
};

}

#endif //LAURYUPSILON_KEYBOARD_XNT_H
