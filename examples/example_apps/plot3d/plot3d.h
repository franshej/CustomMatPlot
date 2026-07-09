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
  // Declare plot objects.
  cmp::Plot3D m_plot_linear, m_plot_log;

 public:
  plot3d()
      : m_plot_log(cmp::Scaling::linear, cmp::Scaling::linear,
                   cmp::Scaling::logarithmic) {
    setSize(1600, 800);

    // Add the plot objects as child components.
    addAndMakeVisible(m_plot_linear);
    addAndMakeVisible(m_plot_log);

    // A helix winding up through the data cube. z is a *linear* ramp that
    // spans three decades (1 -> 1000). The SAME data is plotted on both a
    // linear and a logarithmic z-axis; only the axis scaling differs, so the
    // difference you see is purely the log transform redistributing the
    // winding (bunched near the top on the linear axis, evenly spread per
    // decade on the log axis).
    constexpr std::size_t num_points = 400u;
    constexpr auto num_turns = 4.0f;

    std::vector<float> helix_x(num_points), helix_y(num_points),
        helix_z(num_points);

    for (std::size_t i = 0; i < num_points; ++i) {
      const auto t = float(i) / float(num_points - 1);
      const auto angle = t * num_turns * juce::MathConstants<float>::twoPi;

      helix_x[i] = std::sin(angle);
      helix_y[i] = std::cos(angle);
      helix_z[i] = 1.0f + t * 999.0f;  // linear ramp 1 -> 1000
    }

    // A couple of straight lines through the cube.
    const std::vector<float> diagonal_x = {-1.0f, 1.0f};
    const std::vector<float> diagonal_y = {-1.0f, 1.0f};
    const std::vector<float> diagonal_z = {1.0f, 1000.0f};

    const std::vector<float> edge_x = {-1.0f, -1.0f};
    const std::vector<float> edge_y = {1.0f, 1.0f};
    const std::vector<float> edge_z = {1.0f, 1000.0f};

    // Linear plot (left side): same data on a linear z-axis.
    m_plot_linear.plot3({helix_x, diagonal_x, edge_x},
                        {helix_y, diagonal_y, edge_y},
                        {helix_z, diagonal_z, edge_z});
    m_plot_linear.setTitle("Linear Z-axis");
    m_plot_linear.setXLabel("x");
    m_plot_linear.setYLabel("y");
    m_plot_linear.setZLabel("z");

    // Logarithmic plot (right side): identical data on a log z-axis.
    m_plot_log.plot3({helix_x, diagonal_x, edge_x},
                     {helix_y, diagonal_y, edge_y},
                     {helix_z, diagonal_z, edge_z});
    m_plot_log.setTitle("Logarithmic Z-axis");
    m_plot_log.setXLabel("x");
    m_plot_log.setYLabel("y");
    m_plot_log.setZLabel("z (log)");
  };

  void resized() override {
    const auto bounds = getBounds();
    const auto half_width = bounds.getWidth() / 2;

    // Linear plot on the left.
    m_plot_linear.setBounds(0, 0, half_width, bounds.getHeight());

    // Log plot on the right.
    m_plot_log.setBounds(half_width, 0, half_width, bounds.getHeight());
  };
};
}  // namespace examples
