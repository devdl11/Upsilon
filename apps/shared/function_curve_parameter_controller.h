#ifndef SHARED_FUNCTION_CURVE_PARAMETER_CONTROLLER_H
#define SHARED_FUNCTION_CURVE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "function_go_to_parameter_controller.h"
#include "function_active_function_toogle.h"

namespace Shared {

class FunctionCurveParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  FunctionCurveParameterController();
  View * view() override { return &m_selectableTableView; }
  TELEMETRY_ID("CurveParameter");
  void didBecomeFirstResponder() override;
  KDCoordinate cellHeight() override;
  void setRecordDelegate(FunctionActiveFunctionToogle * toogle) { m_recordDelegate = toogle; }
protected:
  bool handleGotoSelection();
  MessageTableCellWithChevron m_goToCell;
  SelectableTableView m_selectableTableView;
  FunctionActiveFunctionToogle * m_recordDelegate;
private:
  virtual FunctionGoToParameterController * goToParameterController() = 0;
};

}

#endif
