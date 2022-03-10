#ifndef LAURYUPSILON_FUNCTION_ACTIVE_FUNCTION_TOOGLE_H
#define LAURYUPSILON_FUNCTION_ACTIVE_FUNCTION_TOOGLE_H


#include "../../ion/include/ion/storage.h"
#include "function_store.h"

namespace Shared {

class FunctionActiveFunctionToogle {
public:
  FunctionActiveFunctionToogle(FunctionStore * store, int * indexPtr) : m_functionStore(store), m_index(indexPtr) {}

  Ion::Storage::Record getRecord();
  void setCurrentIndex(int ni) { *m_index = ni; }
  void moveUp();
  void moveDown();

private:
  FunctionStore * m_functionStore;
  int * m_index;

};
}

#endif //LAURYUPSILON_FUNCTION_ACTIVE_FUNCTION_TOOGLE_H
