#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

int main() {
  auto gui_scope = juce::ScopedJuceInitialiser_GUI();
  juce::UnitTestRunner runner;
  runner.runAllTests();
  return 0;
}