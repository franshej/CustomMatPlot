#include <set>
class CustomLookAndFeel : public cmp::PlotLookAndFeel {
  // cmp::PlotLookAndFeel::setColour
  // void setColour(ColourIds::background_colour, Colour(200, 0, 0));

  juce::Font getGridLabelFont() const noexcept override {
    return juce::Font("Arial", 12.f, juce::Font::plain);
  };

  juce::Font getXYTitleFont() const noexcept override {
    return juce::Font("Arial", 12.f, juce::Font::plain);
  };

  void drawFrame(juce::Graphics& g,
                 const juce::Rectangle<int> bound) override{};

  // void drawGridLine(juce::Graphics& g, const cmp::GridLine& grid_line,
  //  const bool grid_on) override {};

  std::size_t getMarkerLength() const noexcept override { return 7u; };
};

PlotComponent::PlotComponent() {
  auto bounds = getWindowComponentBounds();

  // setBounds(bounds);

  m_plot.setBounds(bounds);

  // Add the plot object as a child component.
  addAndMakeVisible(m_plot);

  // Plot some values.
  // m_plot.plot({ {1, 3, 7, 9, 13, 120, 43, 76,21,2,5,7,43,54,12,6,2} });

  m_plot.gridON(true, true);

  // Create some data to visulize.
  std::vector<std::vector<float>> y_data;

  y_data.push_back(cmp::generateUniformRandomVector(80, -1.0f, +3.0f));
  y_data.push_back(cmp::generateUniformRandomVector(80, -3.0f, +3.0f));

  // Setting new colours on graph one and two.
  auto graph_attributes = cmp::GraphAttributeList(y_data.size());
  graph_attributes[0].graph_colour = Colour(255, 203, 4);
  graph_attributes[1].graph_colour = Colour(255, 51, 154);
  ;

  graph_attributes[0].marker = cmp::Marker::Type::Circle;

  cmp::PlotLookAndFeel* laf = new CustomLookAndFeel();

  laf.setColour(Plot::grid_colour, juce::Colour(200, 200, 200));
  laf.setColour(Plot::x_grid_label_colour, juce::Colour(200, 200, 200));
  laf.setColour(Plot::y_grid_label_colour, juce::Colour(200, 200, 200));

  laf.setColour(Plot::x_label_colour, juce::Colour(200, 200, 200));
  laf.setColour(Plot::y_label_colour, juce::Colour(200, 200, 200));
  laf.setColour(Plot::title_label_colour, juce::Colour(200, 200, 200));

  laf.setColour(Plot::background_colour, juce::Colour(30, 30, 30));

  graph_attributes[0].path_stroke_type = juce::PathStrokeType(1.0f);

  m_plot.setLookAndFeel(laf);

  // Set x and y limits // crashes... ?
  // m_plot.xLim(-1.0f, 5.0f);
  // m_plot.yLim(-10.0f, +10.0f);

  m_plot.setDownsamplingType(cmp::DownsamplingType::no_downsampling);

  // std::vector<std::string> vec = { "1", "2", "3", "4", "5", "6", "7", "8",
  // "9", "10", "11" };
  m_plot.setYTicks(
      {-5.f, -4.f, -3.f, -2.f, -1.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f});
  m_plot.setYTickLabels(
      {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"});

  // Plot some values.
  m_plot.plot(y_data, {}, graph_attributes);

  // TODO !!!
  // Check for all pointers and resident classes and destroy them here
  // and if required in the destructor to avoid memory leaks
}