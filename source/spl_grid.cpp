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
    g.drawRoundedRectangle({float(x_axis_text.second.getX()),
                            float(x_axis_text.second.getY()),
                            float(x_axis_text.second.getWidth()),
                            float(x_axis_text.second.getHeight())},
                           1.f, 1.f);
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

std::pair<unsigned, unsigned> BaseGrid::getNumHorizontalVerticalLinesLog() {
  const unsigned width = m_graph_area.getWidth();
  const unsigned height = m_graph_area.getHeight();

  unsigned num_horizontal_lines = 6u;
  if (height > 375u) {
    num_horizontal_lines = 22u;
  } else if (height <= 375u && height > 135u) {
    num_horizontal_lines = 10u;
  }

  unsigned num_vertical_lines = 6u;
  if (width > 435u) {
    num_vertical_lines = 22u;
  } else if (width <= 435u && width > 175u) {
    num_vertical_lines = 10u;
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

      if (!m_is_grid_on) {
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

      if (!m_is_grid_on) {
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

      juce::Rectangle<int> *last_rect = nullptr;
      auto n_line = 0u;
      for (const auto &x_pos : m_x_coordinates) {
        const std::string x_label =
            convertFloatToString(m_limX.first + x_diff * n_line, 2, 6);
        n_line++;
        auto x_label_area = juce::Rectangle<int>(
            x + x_pos[0] -
                static_cast<int>(((m_font_size / 2) * x_label.size()) / 2),
            y + height + static_cast<int>(m_font_size / 2),
            static_cast<int>((m_font_size / 2) * x_label.size()),
            static_cast<int>(m_font_size));

        if (!last_rect) {
          m_x_axis_texts.push_back({x_label, x_label_area});
          last_rect = &m_x_axis_texts.back().second;
        } else {
          if (!last_rect->intersects(x_label_area)) {
            m_x_axis_texts.push_back({x_label, x_label_area});
            last_rect = &m_x_axis_texts.back().second;
          }
        }
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

    const auto [dummy1, num_tot_vertical_lines] =
        getNumHorizontalVerticalLinesLog();
    const auto [num_horizontal_lines, dummy2] = getNumHorizontalVerticalLines();

    const auto min_exp = std::floor(log10(m_limX.first));
    const auto max_exp = std::ceil(log10(m_limX.second));

    const auto exp_diff =
        static_cast<unsigned>(ceil(abs(max_exp) - abs(min_exp)));

    const auto num_lines_exp = std::ceil(num_tot_vertical_lines / exp_diff);
    const auto num_lines = exp_diff * num_lines_exp + num_horizontal_lines;

    if (m_grid_lines.size() != num_lines) {
      m_grid_lines.clear();

      m_grid_lines.resize(num_lines);
      m_y_coordinates.resize(num_lines);
      m_x_coordinates.resize(num_lines);
    }

    for (unsigned curr_exp = min_exp; curr_exp < max_exp; ++curr_exp) {
      const auto curr_x_pos_base = pow(10.f, static_cast<float>(curr_exp));

      const auto x_diff = pow(10.f, static_cast<float>(curr_exp + 1)) /
                          static_cast<float>(num_lines_exp);
      for (unsigned line = 0; line < num_lines_exp; ++line) {
        const auto x_pos = curr_x_pos_base + line * x_diff;

        const unsigned index = curr_exp * num_lines_exp + line;

        m_y_coordinates[index] = {0.f, height};
        m_x_coordinates[index] = {x_pos, x_pos};
      }
    }

    auto graph_line_index = 0;

    for (unsigned n_line = 1; n_line < exp_diff * num_lines_exp - 1; ++n_line) {

      if (m_grid_lines[graph_line_index] == nullptr) {
        m_grid_lines[graph_line_index] =
            std::make_unique<LogXGraphLine>(m_grid_colour);
      }

      m_grid_lines[graph_line_index]->setBounds(m_graph_area);

      m_grid_lines[graph_line_index]->xLim(m_limX.first, m_limX.second);
      m_grid_lines[graph_line_index]->yLim(0.f, height);

      m_grid_lines[graph_line_index]->updateYValues(&m_y_coordinates[n_line]);
      m_grid_lines[graph_line_index]->updateXValues(&m_x_coordinates[n_line]);

      if (!m_is_grid_on) {
        const std::vector<float> dashed_lengths = {
            height * 0.025f, height - (height * 0.05f), height * 0.025f};
        m_grid_lines[graph_line_index]->createDashedPath(dashed_lengths);
      }

      addAndMakeVisible(m_grid_lines[graph_line_index].get(), 0);

      graph_line_index++;
    }

    for (int n_line = exp_diff * num_lines_exp;
         n_line < num_horizontal_lines + exp_diff * num_lines_exp; ++n_line) {
      const auto y_pos =
          height * (static_cast<float>(n_line - exp_diff * num_lines_exp) /
                    static_cast<float>(num_horizontal_lines - 1));

      m_y_coordinates[n_line] = {y_pos, y_pos};
      m_x_coordinates[n_line] = {0, width};
    }

    for (int n_line = exp_diff * num_lines_exp + 1;
         n_line < num_horizontal_lines + exp_diff * num_lines_exp - 1;
         ++n_line) {

      if (m_grid_lines[graph_line_index] == nullptr) {
        m_grid_lines[graph_line_index] =
            std::make_unique<LinearGraphLine>(m_grid_colour);
      }

      m_grid_lines[graph_line_index]->setBounds(m_graph_area);

      m_grid_lines[graph_line_index]->xLim(0, width);
      m_grid_lines[graph_line_index]->yLim(0, height);

      m_grid_lines[graph_line_index]->updateYValues(&m_y_coordinates[n_line]);
      m_grid_lines[graph_line_index]->updateXValues(&m_x_coordinates[n_line]);

      if (!m_is_grid_on) {
        const std::vector<float> dashed_lines = {
            height * 0.025f, width - (height * 0.05f), height * 0.025f};
        m_grid_lines[graph_line_index]->createDashedPath(dashed_lines);
      }

      addAndMakeVisible(m_grid_lines[graph_line_index].get(), 0);

      graph_line_index++;
    }

    m_y_axis_texts.clear();
    m_x_axis_texts.clear();

    auto xToXPos = [&](const float x) {
      return width *
             (log(x / m_limX.first) / log(m_limX.second / m_limX.first));
    };

    juce::Rectangle<int> *last_rect = nullptr;

    std::for_each(std::make_reverse_iterator(m_x_coordinates.end())
        , std::make_reverse_iterator(m_x_coordinates.begin())
        , [&](const auto &x_pos) {
            const std::string x_label = convertFloatToString(x_pos[0], 2, 6);
            auto x_label_area = juce::Rectangle<int>(
                x + xToXPos(x_pos[0]) -
                static_cast<int>(((m_font_size / 2) * x_label.size()) / 2),
                y + height + static_cast<int>(m_font_size / 2),
                static_cast<int>((m_font_size / 2) * x_label.size()),
                static_cast<int>(m_font_size));

            if (!last_rect) {
                m_x_axis_texts.push_back({ x_label, x_label_area });
                last_rect = &m_x_axis_texts.back().second;
            }
            else {
                if (!last_rect->intersects(x_label_area)) {
                    m_x_axis_texts.push_back({ x_label, x_label_area });
                    last_rect = &m_x_axis_texts.back().second;
                }
            }
        });
  }
}
