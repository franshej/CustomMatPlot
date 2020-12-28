#include "spl_grid.h"
#include <stdexcept>

template <class float_type>
static std::string convertFloatToString(const float_type value,
                                        int num_decimals, int string_len) {
  if (!(std::is_same<float, float_type>::value ||
        std::is_same<double, float_type>::value)) {
    throw std::invalid_argument("Type must be either float or double");
  }
  const auto &pow_of_ten =
      value == 0.f ? 0 : static_cast<int>(floor(log10(abs(value))));
  const auto &is_neg = static_cast<int>(value < 0);

  auto text_out = std::to_string(value);

  const auto &len_before_dec =
      pow_of_ten < 0 ? abs(pow_of_ten) : pow_of_ten + 1;
  const auto &req_len =
      len_before_dec + is_neg + num_decimals + 1 /* 1 = dot */;

  if (string_len <= len_before_dec + is_neg + 1) {
    if (pow_of_ten >= 0) {
      const auto &two_decimals = text_out.substr(is_neg + pow_of_ten + 1, 3);
      const auto &first_digit = text_out.substr(0, 1 + is_neg);
      text_out = first_digit + two_decimals + "e" + std::to_string(pow_of_ten);
    } else {
      auto three_decimals = text_out.substr(len_before_dec + is_neg + 1, 4);
      three_decimals.insert(1, ".");
      text_out = std::to_string(-1 * is_neg) + three_decimals + "e" +
                 std::to_string(pow_of_ten);
    }
  } else if (string_len < req_len) {
    text_out = text_out.substr(0, string_len);
  } else {
    text_out = text_out.substr(0, len_before_dec + is_neg + 1 + num_decimals);
  }

  return text_out;
}

void BaseGrid::paint(juce::Graphics &g) {
  g.setColour(m_grid_colour);

  for (const auto &path : m_grid_path) {
    g.strokePath(path, juce::PathStrokeType(1.0f));
  }

  g.setColour(m_text_colour);
  g.setFont(juce::Font("Arial Rounded MT", m_font_size, juce::Font::plain));
  for (const auto &y_axis_text : m_y_axis_texts) {
    g.drawText(y_axis_text.first, y_axis_text.second,
               juce::Justification::centredRight);
  }
  for (const auto &x_axis_text : m_x_axis_texts) {
    g.drawText(x_axis_text.first, x_axis_text.second,
               juce::Justification::centred);
  }

  g.setColour(m_frame_colour);

  // Draw frame
  g.drawRect(m_graph_area);

}

void BaseGrid::resized() {
  createGrid();
}

void BaseGrid::setGraphBounds(const juce::Rectangle<int> &graph_area) {
  m_graph_area = graph_area;
  createGrid();
}

void BaseGrid::yLim(const float &min, const float &max) { m_limY = {min, max}; }

void BaseGrid::xLim(const float &min, const float &max) { m_limX = {min, max}; }

void Grid::createGrid() {
  const auto &width = static_cast<float>(m_graph_area.getWidth());
  const auto &height = static_cast<float>(m_graph_area.getHeight());
  const auto &x = static_cast<float>(m_graph_area.getX());
  const auto &y = static_cast<float>(m_graph_area.getY());

  auto num_horizontal_lines = 3;
  if (height > 375.f) {
    num_horizontal_lines = 11;
  } else if (height <= 375.f && height > 135.f) {
    num_horizontal_lines = 5;
  }

  auto num_vertical_lines = 3;
  if (width > 435.f) {
    num_vertical_lines = 11;
  } else if (width <= 435.f && width > 175.f) {
    num_vertical_lines = 5;
  }

  m_grid_path.clear();

  for (int n_line = 0; n_line < num_horizontal_lines; ++n_line) {
    const auto &y_pos = height * (static_cast<float>(n_line) /
                                  static_cast<float>(num_horizontal_lines - 1));
    juce::Path path_grid;
    path_grid.startNewSubPath(x, y + y_pos);
    path_grid.lineTo(x + width, y + y_pos);
    m_grid_path.push_back(path_grid);
  }

  for (int n_line = 0; n_line < num_vertical_lines; ++n_line) {
    const auto &x_pos = width * (static_cast<float>(n_line) /
                                 static_cast<float>(num_vertical_lines - 1));
    juce::Path path_grid;
    path_grid.startNewSubPath(x + int(x_pos), y);
    path_grid.lineTo(x + x_pos, y + height);
    m_grid_path.push_back(path_grid);
  }

  if (m_is_grid_on) {
    juce::PathStrokeType p_type(1.0f);
    const std::vector<float> dashed_lines = {4, 8};

    for (auto &path : m_grid_path) {
      p_type.createDashedStroke(path, path, dashed_lines.data(), 2.f);
    }
  } else {
    juce::PathStrokeType p_type(1.f);

    if (width > 0 && height > 0) {
      const std::vector<float> dashed_x = {
		  height * 0.025f, width - (height * 0.05f), height * 0.025f};
      const std::vector<float> dashed_y = {
          height * 0.025f, height - (height * 0.05f), height * 0.025f};

      auto i = 0u;
      for (auto &path : m_grid_path) {
        if (i < num_horizontal_lines) {
          p_type.createDashedStroke(path, path, dashed_x.data(), 2);
        } else {
          p_type.createDashedStroke(path, path, dashed_y.data(), 2);
        }
        i++;
      }
    }
  }

  m_y_axis_texts.clear();
  m_x_axis_texts.clear();

  if (m_limX.first != m_limX.second && m_limY.first != m_limY.second) {
    const auto &y_diff =
        (m_limY.second - m_limY.first) / (num_horizontal_lines - 1);
    const auto &x_diff =
        (m_limX.second - m_limX.first) / (num_vertical_lines - 1);

    for (int n_line = 0; n_line < num_horizontal_lines; ++n_line) {
      const auto &y_pos =
          height * (static_cast<float>(n_line) /
                    static_cast<float>(num_horizontal_lines - 1));
      m_y_axis_texts.resize(m_y_axis_texts.size() + 1);
      m_y_axis_texts.back().first =
          convertFloatToString(m_limY.second - y_diff * (n_line), 2, 7);
      m_y_axis_texts.back().second = juce::Rectangle<int>(
          x - static_cast<int>(m_font_size * 9) - m_font_size / 2,
          y + y_pos - static_cast<int>(m_font_size / 2),
          static_cast<int>(m_font_size * 9), static_cast<int>(m_font_size));
    }

    for (int n_line = 0; n_line < num_vertical_lines; ++n_line) {
      const auto &x_pos = width * (static_cast<float>(n_line) /
                                   static_cast<float>(num_vertical_lines - 1));
      m_x_axis_texts.resize(m_x_axis_texts.size() + 1);
      m_x_axis_texts.back().first =
          convertFloatToString(m_limX.first + x_diff * (n_line), 2, 7);
      m_x_axis_texts.back().second = juce::Rectangle<int>(
          x + x_pos - static_cast<int>(m_font_size * 4.5),
          y + height + static_cast<int>(m_font_size / 2),
          static_cast<int>(m_font_size * 9), static_cast<int>(m_font_size));
    }
  }
}
