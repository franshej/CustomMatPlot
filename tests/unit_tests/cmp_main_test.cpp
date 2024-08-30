#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

int main() {
  auto gui_scope = juce::ScopedJuceInitialiser_GUI();
  juce::UnitTestRunner runner;
  runner.runAllTests();
  const auto num_tests = runner.getNumResults();

  for (int i = 0; i < num_tests; ++i) {
    auto result = runner.getResult(i);
    if (result->failures > 0) {
      return 1;
    }
  }

  return 0;
}