/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <cmp_plot3d.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <cmath>
#include <vector>

namespace examples {
class plot3d : public juce::Component {
  // Declare plot object.
  cmp::Plot3D m_plot;

 public:
  plot3d() {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // A helix winding up through the data cube.
    constexpr std::size_t num_points = 400u;
    constexpr auto num_turns = 4.0f;

    std::vector<float> helix_x(num_points), helix_y(num_points),
        helix_z(num_points);

    for (std::size_t i = 0; i < num_points; ++i) {
      const auto t = float(i) / float(num_points - 1);
      const auto angle = t * num_turns * juce::MathConstants<float>::twoPi;

      helix_x[i] = std::sin(angle);
      helix_y[i] = std::cos(angle);
      helix_z[i] = t * 10.0f;
    }

    // A couple of straight lines through the cube.
    const std::vector<float> diagonal_x = {-1.0f, 1.0f};
    const std::vector<float> diagonal_y = {-1.0f, 1.0f};
    const std::vector<float> diagonal_z = {0.0f, 10.0f};

    const std::vector<float> edge_x = {-1.0f, -1.0f};
    const std::vector<float> edge_y = {1.0f, 1.0f};
    const std::vector<float> edge_z = {0.0f, 10.0f};

    m_plot.plot3({helix_x, diagonal_x, edge_x},
                 {helix_y, diagonal_y, edge_y},
                 {helix_z, diagonal_z, edge_z});

    m_plot.setTitle("plot3");
    m_plot.setXLabel("x");
    m_plot.setYLabel("y");
    m_plot.setZLabel("z");
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
