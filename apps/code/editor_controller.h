#ifndef CODE_EDITOR_CONTROLLER_H
#define CODE_EDITOR_CONTROLLER_H

#include <escher.h>
#include "script.h"

namespace Code {

class ScriptParameterController;

class EditorController : public ViewController, public TextAreaDelegate {
public:
  EditorController(ScriptParameterController * scriptParameterController);
  void setScript(Script script);

  /* ViewController */
  View * view() override { return &m_textArea; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::WantsMaximumSpace; }

  /* TextAreaDelegate */
  bool textAreaShouldFinishEditing(TextArea * textArea, Ion::Events::Event event) override;
  bool textAreaDidReceiveEvent(TextArea * textArea, Ion::Events::Event event) override;
  ::Toolbox * toolboxForTextArea(TextArea * textArea) override;

private:
  static constexpr int k_indentationSpacesNumber = 2;
  StackViewController * stackController();
  TextArea m_textArea;
  ScriptParameterController * m_scriptParameterController;
};

}

#endif

