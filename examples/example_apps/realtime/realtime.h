/**
 * Copyright (c) 2022 Frans Rosencrantz
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <cmp_plot.h>
#include <example_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

namespace examples {
class realtime : public juce::Component, private juce::Timer {
  // Declare plot object.
  cmp::Plot m_plot;

 public:
  realtime() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    startTimerHz(30);

    // Always plot atleast ones before calling realTimePlot
    m_plot.plot(
        {cmp::generateSineWaveVector<float>((1 << 10), -1.0f, 1.0f, 1, 1)});
  };

  void timerCallback() override {
    static auto periods = 0.0f;
    static auto phase = 0.0f;

    static std::function<float(void)> f_period = []() { return 1.0f; };
    static std::function<float(void)> f_phase = []() {
      return juce::MathConstants<float>::pi * 0.01f;
    };

    // Plot some values.
    m_plot.plotUpdateYOnly({cmp::generateSineWaveVector<float>(
        (1 << 10), -1.0f, 1.0f, periods, phase)});

    if (periods >= 75.f) {
      f_period = []() { return -1.0f; };
      f_phase = []() { return -juce::MathConstants<float>::pi * 0.01f; };
    } else if (periods <= 2.0f) {
      f_period = []() { return 1.0f; };
      f_phase = []() { return juce::MathConstants<float>::pi * 0.01f; };
    }

    periods += f_period();
    phase += f_phase();
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
