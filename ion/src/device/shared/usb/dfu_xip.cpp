#include "calculator.h"

namespace Ion {
namespace USB {

void DFU(bool exitWithKeyboard, bool unlocked) {
  Ion::Device::USB::Calculator::PollAndReset(exitWithKeyboard, unlocked);
}

}
}
