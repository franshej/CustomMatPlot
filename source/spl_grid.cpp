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

  if (string_len < req_len) {
    if (pow_of_ten >= 0) {
      const auto &two_decimals =
          text_out.substr(static_cast<unsigned>(is_neg + pow_of_ten + 1), 3);
      const auto &first_digit =
          text_out.substr(0, static_cast<unsigned>(1 + is_neg));
      text_out = first_digit + two_decimals + "e" + std::to_string(pow_of_ten);
    } else {
      auto three_decimals = text_out.substr(len_before_dec + is_neg + 1, 4);
      three_decimals.insert(0, ".");
      text_out = std::to_string(-1 * is_neg) + three_decimals + "e" +
                 std::to_string(pow_of_ten);
    }
  } else {
    text_out = text_out.substr(0, len_before_dec + is_neg + 1 + num_decimals);
  }

  return text_out;
}

void BaseGrid::paint(juce::Graphics &g) {
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

void BaseGrid::resized() { createGrid(); }

void BaseGrid::setGraphBounds(const juce::Rectangle<int> &graph_area) {
  m_graph_area = graph_area;
  createGrid();
}

void BaseGrid::yLim(const float &min, const float &max) { m_limY = {min, max}; }

void BaseGrid::xLim(const float &min, const float &max) { m_limX = {min, max}; }

std::pair<unsigned, unsigned> BaseGrid::getNumHorizontalVerticalLines() {
  const unsigned width = m_graph_area.getWidth();
  const unsigned height = m_graph_area.getHeight();

  unsigned num_horizontal_lines = 3u;
  if (height > 375u) {
    num_horizontal_lines = 11u;
  } else if (height <= 375u && height > 135u) {
    num_horizontal_lines = 5u;
  }

  unsigned num_vertical_lines = 3u;
  if (width > 435u) {
    num_vertical_lines = 11u;
  } else if (width <= 435u && width > 175u) {
    num_vertical_lines = 5u;
  }

  return {num_horizontal_lines, num_vertical_lines};
}

void Grid::createGrid() {
  const auto width = static_cast<float>(m_graph_area.getWidth());
  const auto height = static_cast<float>(m_graph_area.getHeight());
  const auto x = static_cast<float>(m_graph_area.getX());
  const auto y = static_cast<float>(m_graph_area.getY());

  const auto [num_horizontal_lines, num_vertical_lines] =
      getNumHorizontalVerticalLines();

  if (width > 0.f && height > 0.f) {

    if (m_grid_lines.size() != (num_horizontal_lines + num_vertical_lines - 4u))
      m_x_coordinates.resize(
          static_cast<unsigned>(num_horizontal_lines + num_vertical_lines - 4u),
          std::vector<float>(2u));
    m_y_coordinates.resize(
        static_cast<unsigned>(num_horizontal_lines + num_vertical_lines - 4u),
        std::vector<float>(2u));
    m_grid_lines.resize(
        static_cast<unsigned>(num_horizontal_lines + num_vertical_lines - 4u));

    auto graph_line_index = 0u;
    for (int n_line = 1; n_line < num_horizontal_lines - 1; ++n_line) {
      const auto y_pos =
          height * (static_cast<float>(n_line) /
                    static_cast<float>(num_horizontal_lines - 1));
      m_y_coordinates[graph_line_index] = {y_pos, y_pos};
      m_x_coordinates[graph_line_index] = {0, width};

      if (m_grid_lines[graph_line_index] == nullptr) {
        m_grid_lines[graph_line_index] =
            std::make_unique<LinearGraphLine>(m_grid_colour);
      }

      m_grid_lines[graph_line_index]->setBounds(m_graph_area);

      m_grid_lines[graph_line_index]->xLim(0, width);
      m_grid_lines[graph_line_index]->yLim(0, height);

      m_grid_lines[graph_line_index]->updateYValues(
          &m_y_coordinates[graph_line_index]);
      m_grid_lines[graph_line_index]->updateXValues(
          &m_x_coordinates[graph_line_index]);

      if (m_is_grid_on) {
        const std::vector<float> dashed_lines = {4, 8};
        m_grid_lines[graph_line_index]->createDashedPath(dashed_lines);
      } else {
        const std::vector<float> dashed_lines = {
            height * 0.025f, width - (height * 0.05f), height * 0.025f};
        m_grid_lines[graph_line_index]->createDashedPath(dashed_lines);
      }

      addAndMakeVisible(m_grid_lines[graph_line_index].get(), 0);

      graph_line_index++;
    }

    for (int n_line = 1; n_line < num_vertical_lines - 1; ++n_line) {
      const auto x_pos = width * (static_cast<float>(n_line) /
                                  static_cast<float>(num_vertical_lines - 1));

      m_y_coordinates[graph_line_index] = {0, height};
      m_x_coordinates[graph_line_index] = {x_pos, x_pos};

      if (m_grid_lines[graph_line_index] == nullptr) {
        m_grid_lines[graph_line_index] =
            std::make_unique<LinearGraphLine>(m_grid_colour);
      }

      m_grid_lines[graph_line_index]->setBounds(m_graph_area);

      m_grid_lines[graph_line_index]->xLim(0, width);
      m_grid_lines[graph_line_index]->yLim(0, height);

      m_grid_lines[graph_line_index]->updateYValues(
          &m_y_coordinates[graph_line_index]);
      m_grid_lines[graph_line_index]->updateXValues(
          &m_x_coordinates[graph_line_index]);

      if (m_is_grid_on) {
        const std::vector<float> dashed_lengths = {4, 8};
        m_grid_lines[graph_line_index]->createDashedPath(dashed_lengths);
      } else {
        const std::vector<float> dashed_lengths = {
            height * 0.025f, height - (height * 0.05f), height * 0.025f};
        m_grid_lines[graph_line_index]->createDashedPath(dashed_lengths);
      }

      addAndMakeVisible(m_grid_lines[graph_line_index].get(), 0);

      graph_line_index++;
    }

    m_y_axis_texts.clear();
    m_x_axis_texts.clear();

    if (m_limX.first != m_limX.second && m_limY.first != m_limY.second) {
      const auto &y_diff =
          (m_limY.second - m_limY.first) / (num_horizontal_lines - 1);
      const auto &x_diff =
          (m_limX.second - m_limX.first) / (num_vertical_lines - 1);

      for (unsigned n_line = 0; n_line < num_horizontal_lines; ++n_line) {
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

      for (unsigned n_line = 0; n_line < num_vertical_lines; ++n_line) {
        const auto &x_pos =
            width * (static_cast<float>(n_line) /
                     static_cast<float>(num_vertical_lines - 1));
        m_x_axis_texts.resize(m_x_axis_texts.size() + 1);
        m_x_axis_texts.back().first =
            convertFloatToString(m_limX.first + x_diff * (n_line), 2, 6);
        m_x_axis_texts.back().second = juce::Rectangle<int>(
            x + x_pos - static_cast<int>(m_font_size * 4.5),
            y + height + static_cast<int>(m_font_size / 2),
            static_cast<int>(m_font_size * 9), static_cast<int>(m_font_size));
      }
    }
  }
}

void SemiLogXGrid::createGrid() {
  const auto width = static_cast<float>(m_graph_area.getWidth());
  const auto height = static_cast<float>(m_graph_area.getHeight());
  const auto x = static_cast<float>(m_graph_area.getX());
  const auto y = static_cast<float>(m_graph_area.getY());

  if (width > 0.f && height > 0.f) {
    const auto num_lines_exp = 10u;
    const auto [num_horizontal_lines, dummy] = getNumHorizontalVerticalLines();

    const auto min_exp = std::floor(log10(m_limX.first));
    const auto max_exp = std::ceil(log10(m_limX.second));

    const auto exp_diff =
        static_cast<unsigned>(ceil(abs(max_exp) - abs(min_exp)));

    const auto num_lines = exp_diff * num_lines_exp + num_horizontal_lines;

    if (m_grid_lines.size() != num_lines) {
      m_grid_lines.resize(num_lines);
      m_y_coordinates.resize(num_lines);
      m_x_coordinates.resize(num_lines);
    }

    const auto start_x_index = pow(10, min_exp);
    auto graph_line_index = 0u;

    for (unsigned curr_exp = min_exp; curr_exp < max_exp; ++curr_exp) {
      const auto curr_x_pos_base = pow(10.f, static_cast<float>(curr_exp));
      const auto x_diff = pow(10.f, static_cast<float>(curr_exp + 1)) /
                          static_cast<float>(num_lines_exp);
      for (unsigned line = 0; line < num_lines_exp; ++line) {
        if (m_grid_lines[graph_line_index] == nullptr) {
          m_grid_lines[graph_line_index] =
              std::make_unique<LogXGraphLine>(m_grid_colour);
        }

        const auto x_pos = curr_x_pos_base + line * x_diff;

        m_y_coordinates[graph_line_index] = {0.f, height};
        m_x_coordinates[graph_line_index] = {x_pos, x_pos};

        m_grid_lines[graph_line_index]->setBounds(m_graph_area);

        m_grid_lines[graph_line_index]->xLim(m_limX.first, m_limX.second);
        m_grid_lines[graph_line_index]->yLim(0.f, height);

        m_grid_lines[graph_line_index]->updateYValues(
            &m_y_coordinates[graph_line_index]);
        m_grid_lines[graph_line_index]->updateXValues(
            &m_x_coordinates[graph_line_index]);

        const std::vector<float> dashed_lines = {4, 8};
        m_grid_lines[graph_line_index]->createDashedPath(dashed_lines);

        addAndMakeVisible(m_grid_lines[graph_line_index].get(), 0);

        graph_line_index++;
      }
    }

    for (int n_line = 1; n_line < num_horizontal_lines - 1; ++n_line) {
      const auto y_pos =
          height * (static_cast<float>(n_line) /
                    static_cast<float>(num_horizontal_lines - 1));
      m_y_coordinates[graph_line_index] = {y_pos, y_pos};
      m_x_coordinates[graph_line_index] = {0, width};

      if (m_grid_lines[graph_line_index] == nullptr) {
        m_grid_lines[graph_line_index] =
            std::make_unique<LinearGraphLine>(m_grid_colour);
      }

      m_grid_lines[graph_line_index]->setBounds(m_graph_area);

      m_grid_lines[graph_line_index]->xLim(0, width);
      m_grid_lines[graph_line_index]->yLim(0, height);

      m_grid_lines[graph_line_index]->updateYValues(
          &m_y_coordinates[graph_line_index]);
      m_grid_lines[graph_line_index]->updateXValues(
          &m_x_coordinates[graph_line_index]);

      const std::vector<float> dashed_lines = {4, 8};
      m_grid_lines[graph_line_index]->createDashedPath(dashed_lines);

      addAndMakeVisible(m_grid_lines[graph_line_index].get(), 0);

      graph_line_index++;
    }
  }
}
