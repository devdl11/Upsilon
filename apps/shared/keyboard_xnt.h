#ifndef SHARED_KEYBOARD_XNT_H
#define SHARED_KEYBOARD_XNT_H

#include "../../ion/include/ion/events.h"
#include "../../ion/include/ion/unicode/code_point.h"
#include "../../poincare/include/poincare/layout_cursor.h"

namespace Shared {

class Keyboard_XNT {
public:
  static const unsigned long k_KEYS = 4;
  static constexpr char k_X[] = "x";
  static constexpr char k_N[] = "n";
  static constexpr char k_T[] = "t";
  static constexpr char k_THETA[] = "θ";

  class XNT_KEY {
  public:
    explicit XNT_KEY(const char * key) : m_key(key) {}

    const char * getKey() const { return m_key; }
  private:
    const char * m_key;
  };

  enum AppsKeys {
    PYTHON,
    CALC,
    FUNCTION,
    SEQUENCE,
    DEFAULT,
    POINCARE,
    STATISTICS,
    NONE
  };

  class AppIndex {
  public:
    explicit AppIndex(int index, AppsKeys app) : m_index(index), m_app(app) {}

    int getIndex() const { return m_index; }
    AppsKeys getApp() const { return m_app; }

  private:
    const int m_index;
    const AppsKeys m_app;
  };

  static XNT_KEY X_KEY() { return XNT_KEY(k_X); };
  static XNT_KEY N_KEY() { return XNT_KEY(k_N); };
  static XNT_KEY T_KEY() { return XNT_KEY(k_T); };
  static XNT_KEY O_KEY() { return XNT_KEY(k_THETA); };
  static XNT_KEY OK_KEY() { return XNT_KEY(nullptr); };

  static AppIndex PythonApp() { return AppIndex(0, AppsKeys::PYTHON); }
  static AppIndex CalcApp() { return AppIndex(0, AppsKeys::CALC); }
  static AppIndex FunctionApp() { return AppIndex(0, AppsKeys::FUNCTION); }
  static AppIndex SequenceApp() { return AppIndex(1, AppsKeys::SEQUENCE); }
  static AppIndex DefaultApp() { return AppIndex(0, AppsKeys::DEFAULT); }
  static AppIndex PoincareSys() { return AppIndex(0, AppsKeys::POINCARE); }
  static AppIndex StatisticsApp() { return AppIndex(0, AppsKeys::STATISTICS); }

  static AppIndex getAppIndexByApp(AppsKeys app);

  Keyboard_XNT() : m_toggle(false), m_index(0), m_previous(AppsKeys::NONE), m_reset(false), m_handle(false) {}

  XNT_KEY handleEvent(Ion::Events::Event event);

  int getIndex() const {return m_index; }
  bool isToggling() const { return m_toggle; }
  XNT_KEY getKeyByIndex(int i);
  void startCleaning();
  void finalizeCleaning();
  bool didJustReset() const { return m_reset; }
  bool didJustHandle() const { return m_handle; }
  Poincare::LayoutCursor::Direction getSelectionDirection();
  static bool isXNTKey(const char * buffer);

private:
  bool m_toggle;
  bool m_handle;
  int m_index;
  bool m_reset;
  AppsKeys m_previous;
};

}

#endif //LAURY UPSILON_KEYBOARD_XNT_H
