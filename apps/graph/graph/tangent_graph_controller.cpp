#include "tangent_graph_controller.h"
#include "../app.h"
#include "../../apps_container.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/preferences.h>

using namespace Shared;
using namespace Poincare;

namespace Graph {

TangentGraphController::TangentGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  m_graphView(graphView),
  m_bannerView(bannerView),
  m_graphRange(curveViewRange),
  m_recordDelegate(nullptr)
{
}

const char * TangentGraphController::title() {
  return I18n::translate(I18n::Message::Tangent);
}

void TangentGraphController::viewWillAppear() {
  Shared::SimpleInteractiveCurveViewController::viewWillAppear();
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
  m_graphView->drawTangent(true);
  m_graphView->setOkView(nullptr);
  m_graphView->selectMainView(true);
  m_bannerView->setNumberOfSubviews(BannerView::k_numberOfSubviews);
  reloadBannerView();
  m_graphView->reload();
}

void TangentGraphController::didBecomeFirstResponder() {
  if (curveView()->isMainViewSelected()) {
    m_bannerView->abscissaValue()->setParentResponder(this);
    m_bannerView->abscissaValue()->setDelegates(textFieldDelegateApp(), this);
    Container::activeApp()->setFirstResponder(m_bannerView->abscissaValue());
  }
}

bool TangentGraphController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  Shared::TextFieldDelegateApp * myApp = textFieldDelegateApp();
  double floatBody;
  if (myApp->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(m_recordDelegate->getRecord());
  assert(function->plotType() == Shared::ContinuousFunction::PlotType::Cartesian);
  double y = function->evaluate2DAtParameter(floatBody, myApp->localContext()).x2();
  m_cursor->moveTo(floatBody, floatBody, y);
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
  reloadBannerView();
  curveView()->reload();
  return true;
}

void TangentGraphController::setRecordDelegate(Shared::FunctionActiveFunctionToogle * record) {
  m_graphView->selectRecord(record->getRecord());
  m_recordDelegate = record;
}

void TangentGraphController::reloadBannerView() {
  if (m_recordDelegate->getRecord().isNull()) {
    return;
  }
  FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(m_cursor, m_recordDelegate->getRecord(), Shared::FunctionApp::app()->functionStore(), AppsContainer::sharedAppsContainer()->globalContext());
  GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, m_recordDelegate->getRecord());
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters + PrintFloat::charSizeForFloatsWithPrecision(Preferences::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  Poincare::Context * context = textFieldDelegateApp()->localContext();

  constexpr int precision = Preferences::MediumNumberOfSignificantDigits;
  const char * legend = "a=";
  int legendLength = strlcpy(buffer, legend, bufferSize);
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(m_recordDelegate->getRecord());
  double y = function->approximateDerivative(m_cursor->x(), context);
  PoincareHelpers::ConvertFloatToText<double>(y, buffer + legendLength, bufferSize - legendLength, precision);
  m_bannerView->aView()->setText(buffer);

  legend = "b=";
  legendLength = strlcpy(buffer, legend, bufferSize);
  Shared::TextFieldDelegateApp * myApp = textFieldDelegateApp();
  assert(function->plotType() == Shared::ContinuousFunction::PlotType::Cartesian);
  y = -y*m_cursor->x()+function->evaluate2DAtParameter(m_cursor->x(), myApp->localContext()).x2();
  PoincareHelpers::ConvertFloatToText<double>(y, buffer + legendLength, bufferSize - legendLength, precision);
  m_bannerView->bView()->setText(buffer);
  m_bannerView->reload();
}

bool TangentGraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, m_recordDelegate->getRecord());
}

bool TangentGraphController::handleEnter() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

bool TangentGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_recordDelegate->moveUp();
    m_graphView->selectRecord(m_recordDelegate->getRecord());
    m_graphView->reload();
    return true;
  } else if (event == Ion::Events::Down) {
    m_recordDelegate->moveDown();
    m_graphView->selectRecord(m_recordDelegate->getRecord());
    m_graphView->reload();
    return true;
  }
  return SimpleInteractiveCurveViewController::handleEvent(event);
}

}
