/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <cmp_plot.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "cmp_datamodels.h"
#include "cmp_lookandfeel.h"

using namespace cmp;

namespace examples {
class move_graph_points : public juce::Component {
  // Custom lookandfeel class to override the mouse input actions.
  class CustomLookAndFeel : public cmp::PlotLookAndFeel {
    // Override the mouse input actions to move the graph points.
    std::map<cmp::UserInput, cmp::UserInputAction> overrideUserInputMapAction(
        std::map<cmp::UserInput, cmp::UserInputAction>
            default_user_input_map_action) const noexcept override {
      auto new_user_input_map_action = default_user_input_map_action;

      new_user_input_map_action[UserInput::left_mouse_drag_end] =
          UserInputAction::select_tracepoints_within_selected_area;
      new_user_input_map_action[UserInput::left_mouse_drag_tracepoint] =
          UserInputAction::move_selected_trace_points;
      new_user_input_map_action[UserInput::left_mouse_double] = UserInputAction::none;

      return new_user_input_map_action;
    }
  } custom_lnf;

  // Declare plot object.
  cmp::Plot m_plot;

 public:
  move_graph_points() : m_plot{cmp::Plot()} {
    setSize(1200, 800);

    // Add the plot object as a child component.
    addAndMakeVisible(m_plot);

    // Set the graph point move type to vertical.
    m_plot.setMovePointsType(cmp::GraphPointMoveType::vertical);

    // Write the y data from the graph lines to the terminal.
    m_plot.setGraphLineDataChangedCallback(
        [](const cmp::GraphLineDataViewList graph_line_list) {
          // y_data from graph_lines to string.
          auto textToWrite = std::string();
          auto i = 0;
          for (const auto& graph_line : graph_line_list) {
            textToWrite += "Graphline " + std::to_string(++i) + " : ";
            for (const auto& y : graph_line.y_data) {
              textToWrite += std::to_string(y) + " ";
            }
            textToWrite += "\n";
          }

          DBG(textToWrite);
        });

    // Plot some values.
    m_plot.plot(
        {{1, 3, 7, 9, 13}, {9, 21, 4, 9, 32, 4, 5, 6, 7, 8, 9, 10, 11}});

    // Set the lookandfeel of the plot.
    m_plot.setLookAndFeel(&custom_lnf);
  };

  void resized() override {
    // Set the bounds of the plot to fill the whole window.
    m_plot.setBounds(getBounds());
  };
};
}  // namespace examples
