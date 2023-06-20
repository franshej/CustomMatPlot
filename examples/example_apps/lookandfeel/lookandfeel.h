/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <cmp_lookandfeel.h>
#include <cmp_plot.h>
#include <example_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>

namespace examples {
class CustomLookAndFeel : public cmp::PlotLookAndFeel {
  juce::Font getGridLabelFont() const noexcept override {
    return juce::Font("Party LET", 35.f, juce::Font::bold);
  };

  juce::Font getXYTitleFont() const noexcept override {
    return juce::Font("Phosphate", 35.f, juce::Font::bold);
  };

  void drawFrame(juce::Graphics& g,
                 const juce::Rectangle<int> bound) override{};

  void drawGridLine(juce::Graphics& g, const cmp::GridLine& grid_line,
                    const bool grid_on) override{};
};

class lookandfeel : public juce::Component {
  // Declare plot object.
  cmp::Plot m_plot;

  // My custom lookandfeel class.
  CustomLookAndFeel m_lnf;

  // Make sure to null the lookandfeel before the m_lnf goes out of scope.
  ~lookandfeel() { m_plot.setLookAndFeel(nullptr); };

 public:
  lookandfeel() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    auto graph_attributes = cmp::GraphAttributeList(1);
    graph_attributes.front().graph_colour = juce::Colours::blueviolet;
    graph_attributes.front().path_stroke_type = juce::PathStrokeType(15);

    // Plot some values.
    m_plot.plot({cmp::generateSineWaveVector(100, -5.0f, 6.0f, 3.0f)}, {},
                graph_attributes);

    m_plot.setTitle("My cool Phosphate title!!!");
    m_plot.setXLabel("X label wow!");
    m_plot.setYLabel("YYYYYYY");

    m_lnf.setColour(cmp::Plot::grid_colour, juce::Colours::pink);
    m_lnf.setColour(cmp::Plot::x_grid_label_colour, juce::Colours::pink);
    m_lnf.setColour(cmp::Plot::y_grid_label_colour, juce::Colours::blueviolet);

    m_lnf.setColour(cmp::Plot::x_label_colour, juce::Colours::orange);
    m_lnf.setColour(cmp::Plot::y_label_colour, juce::Colours::whitesmoke);
    m_lnf.setColour(cmp::Plot::title_label_colour, juce::Colours::purple);

    m_lnf.setColour(cmp::Plot::background_colour, juce::Colour(30, 30, 30));

    // Set custom lookandfeel class.
    m_plot.setLookAndFeel(&m_lnf);
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
