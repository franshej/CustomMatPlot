#include "spl_grid.h"

#include <stdexcept>

template <class float_type>
static std::string convertFloatToString(const float_type value,
                                        int num_decimals, int max_string_len) {
  if (!(std::is_same<float, float_type>::value ||
        std::is_same<double, float_type>::value)) {
    throw std::invalid_argument("Type must be either float or double");
  }
  const auto pow_of_ten =
      value == 0.f ? 0 : static_cast<int>(floor(log10(abs(value))));
  const auto is_neg = static_cast<int>(value < 0);

  auto text_out = std::to_string(value);

  const auto len_before_dec = pow_of_ten < 0 ? abs(pow_of_ten) : pow_of_ten + 1;
  const auto req_len = len_before_dec + is_neg + num_decimals + 1 /* 1 = dot */;

  if (max_string_len < req_len) {
    if (pow_of_ten >= 0) {
      const auto two_decimals =
          text_out.substr(static_cast<unsigned>(is_neg + pow_of_ten + 1), 3);
      const auto first_digit =
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

template <class num_type>
static std::tuple<num_type, num_type, num_type, num_type> getRectangleMeasures(
    juce::Rectangle<int> m_graph_area) {
  const auto x = static_cast<num_type>(m_graph_area.getX());
  const auto y = static_cast<num_type>(m_graph_area.getY());
  const auto width = static_cast<num_type>(m_graph_area.getWidth());
  const auto height = static_cast<num_type>(m_graph_area.getHeight());
  return std::make_tuple(x, y, width, height);
}

void BaseGrid::setLabels(const std::function<float(const float)> xToXPos,
                         const std::function<float(const float)> yToYPos) {
  const auto [x, y, width, height] = getRectangleMeasures<float>(m_graph_area);

  juce::Rectangle<int> *x_last_rect = nullptr;
  juce::Rectangle<int> *y_last_rect = nullptr;

  m_x_axis_labels.clear();
  m_y_axis_labels.clear();

  std::for_each(
      std::make_reverse_iterator(m_vertical_grid_lines.end()),
      std::make_reverse_iterator(m_vertical_grid_lines.begin()),
      [&](const auto &grid) {
        const auto x_val = grid->getXValues();

        const std::string x_label = convertFloatToString(x_val[0], 2, 6);

        auto x_label_area = juce::Rectangle<int>(
            x + xToXPos(x_val[0]) -
                static_cast<int>(((m_font_size / 2) * x_label.size()) / 2),
            y + height + static_cast<int>(m_font_size),
            static_cast<int>((m_font_size / 2) * x_label.size()),
            static_cast<int>(m_font_size));

        if (!x_last_rect) {
          m_x_axis_labels.push_back({x_label, x_label_area});
          x_last_rect = &m_x_axis_labels.back().second;
        } else {
          if (!x_last_rect->intersects(x_label_area)) {
            m_x_axis_labels.push_back({x_label, x_label_area});
            x_last_rect = &m_x_axis_labels.back().second;
          }
        }
      });

  std::for_each(std::make_reverse_iterator(m_horizontal_grid_lines.end()),
                std::make_reverse_iterator(m_horizontal_grid_lines.begin()),
                [&](const auto &grid) {
                  const auto y_val = grid->getYValues();

                  const std::string y_label =
                      convertFloatToString(y_val[0], 2, 6);

                  const auto width_y_label =
                      static_cast<int>((m_font_size / 2) * y_label.size());

                  auto y_label_area = juce::Rectangle<int>(
                      x - m_font_size - width_y_label,
                      y + yToYPos(y_val[0]) - static_cast<int>(m_font_size / 2),
                      width_y_label, static_cast<int>(m_font_size));

                  if (!y_last_rect) {
                    m_y_axis_labels.push_back({y_label, y_label_area});
                    y_last_rect = &m_y_axis_labels.back().second;
                  } else {
                    if (!y_last_rect->intersects(y_label_area)) {
                      m_y_axis_labels.push_back({y_label, y_label_area});
                      y_last_rect = &m_y_axis_labels.back().second;
                    }
                  }
                });
}

void BaseGrid::paint(juce::Graphics &g) {
  g.setColour(m_text_colour);
  g.setFont(juce::Font("Arial Rounded MT", m_font_size, juce::Font::plain));
  for (const auto &y_axis_text : m_y_axis_labels) {
    g.drawText(y_axis_text.first, y_axis_text.second,
               juce::Justification::centredRight);
    g.drawRoundedRectangle(
        {float(y_axis_text.second.getX()), float(y_axis_text.second.getY()),
         float(y_axis_text.second.getWidth()),
         float(y_axis_text.second.getHeight())},
        1.f, 1.f);
  }
  for (const auto &x_axis_text : m_x_axis_labels) {
    g.drawText(x_axis_text.first, x_axis_text.second,
               juce::Justification::centred);
    g.drawRoundedRectangle(
        {float(x_axis_text.second.getX()), float(x_axis_text.second.getY()),
         float(x_axis_text.second.getWidth()),
         float(x_axis_text.second.getHeight())},
        1.f, 1.f);
  }

  g.setColour(m_frame_colour);

  // Draw frame
  g.drawRect(m_graph_area);
}

void BaseGrid::resized() {
  calculateNumHorizontalVerticalLines();
  resizeDataHolders();
  createGrid();

  for (const auto &grid : m_horizontal_grid_lines) {
    if (grid) {
      grid->calculateXData();
      grid->calculateYData();
    }
  }

  for (const auto &grid : m_vertical_grid_lines) {
    if (grid) {
      grid->calculateXData();
      grid->calculateYData();
    }
  }
}

void BaseGrid::setGraphBounds(const juce::Rectangle<int> &graph_area) {
  m_graph_area = graph_area;
  resized();
}

void BaseGrid::yLim(const float &min, const float &max) { m_limY = {min, max}; }

void BaseGrid::xLim(const float &min, const float &max) { m_limX = {min, max}; }

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

void Grid::calculateNumHorizontalVerticalLines() {
  const unsigned width = m_graph_area.getWidth();
  const unsigned height = m_graph_area.getHeight();

  m_num_horizontal_lines = 3u;
  if (height > 375u) {
    m_num_horizontal_lines = 11u;
  } else if (height <= 375u && height > 135u) {
    m_num_horizontal_lines = 5u;
  }

  m_num_vertical_lines = 3u;
  if (width > 435u) {
    m_num_vertical_lines = 11u;
  } else if (width <= 435u && width > 175u) {
    m_num_vertical_lines = 5u;
  }
}

void SemiLogXGrid::calculateNumHorizontalVerticalLines() {
  const unsigned width = m_graph_area.getWidth();
  const unsigned height = m_graph_area.getHeight();

  m_num_horizontal_lines = 3u;
  if (height > 375u) {
    m_num_horizontal_lines = 11u;
  } else if (height <= 375u && height > 135u) {
    m_num_horizontal_lines = 5u;
  }

  m_num_vertical_lines = 10u;
}

void Grid::resizeDataHolders() {
  m_vertical_grid_lines.clear();
  m_vertical_grid_lines.reserve(m_num_vertical_lines);

  m_horizontal_grid_lines.clear();
  m_horizontal_grid_lines.reserve(m_num_horizontal_lines);
}

template <class graph_type>
static GraphLine *getAndAddGridLine(
    std::vector<std::unique_ptr<GraphLine>> &graph_lines,
    const juce::Colour grid_colour) {
  graph_lines.emplace_back(std::make_unique<graph_type>(grid_colour));
  return graph_lines.back().get();
}

template <class graph_type>
void BaseGrid::addGridLineVertical(const float x_val) {
  const auto [x, y, width, height] = getRectangleMeasures<float>(m_graph_area);

  auto grid_line =
      getAndAddGridLine<graph_type>(m_vertical_grid_lines, m_grid_colour);
  grid_line->setBounds(m_graph_area);

  grid_line->xLim(m_limX.first, m_limX.second);
  grid_line->yLim(0.f, height);

  grid_line->setXValues({x_val, x_val});
  grid_line->setYValues({0.f, height});

  if (!m_is_grid_on) {
    const std::vector<float> dashed_lines = {
        height * 0.025f, height - (height * 0.05f), height * 0.025f};
    grid_line->setDashedPath(dashed_lines);
  }

  addAndMakeVisible(grid_line, 0);
}

template <class graph_type>
void BaseGrid::addGridLineHorizontal(const float y_val) {
  const auto [x, y, width, height] = getRectangleMeasures<float>(m_graph_area);

  auto grid_line =
      getAndAddGridLine<graph_type>(m_horizontal_grid_lines, m_grid_colour);
  grid_line->setBounds(m_graph_area);

  grid_line->xLim(0.f, width);
  grid_line->yLim(m_limY.first, m_limY.second);

  grid_line->setXValues({0.f, width});
  grid_line->setYValues({y_val, y_val});

  if (!m_is_grid_on) {
    const std::vector<float> dashed_lines = {
        height * 0.025f, width - (height * 0.05f), height * 0.025f};
    grid_line->setDashedPath(dashed_lines);
  }

  addAndMakeVisible(grid_line, 0);
}

void Grid::createGrid() {
  const auto [x, y, width, height] = getRectangleMeasures<float>(m_graph_area);

  if (!(width > 0.f && height > 0.f)) return;

  // Create the vertical lines
  auto x_diff = (m_limX.second - m_limX.first) / float(m_num_vertical_lines);
  for (int i = 0; i <= m_num_vertical_lines; ++i) {
    const auto x_pos = m_limX.first + float(i) * x_diff;
    addGridLineVertical<LinearGraphLine>(x_pos);
  }

  // Then create the horizontal lines
  auto y_diff = (m_limY.second - m_limY.first) / float(m_num_horizontal_lines);
  for (int i = 0; i <= m_num_horizontal_lines; ++i) {
    const auto y_pos = m_limY.first + float(i) * y_diff;
    addGridLineHorizontal<LinearGraphLine>(y_pos);
  }

  auto xToXPos = [&](const float x) {
    return width * (1.f - (m_limX.second - x) / (m_limX.second - m_limX.first));
  };

  auto yToYPos = [&](const float y) {
    return height * (m_limY.second - y) / (m_limY.second - m_limY.first);
  };

  setLabels(xToXPos, yToYPos);
}

void SemiLogXGrid::resizeDataHolders() {
  m_min_exp = std::floor(log10(m_limX.first));
  m_max_exp = std::ceil(log10(m_limX.second));

  const auto top_out_of_sight_lines = (m_max_exp - log10(m_limX.second));
  const auto bottom_out_of_sight_lines = log10(m_limX.first) - m_min_exp;
  const auto num_out_of_sight_lines =
      top_out_of_sight_lines + bottom_out_of_sight_lines;

  m_exp_diff = static_cast<unsigned>(ceil(abs(m_max_exp) - abs(m_min_exp)));

  m_num_lines_exp = 10;
  m_num_tot_lines = (m_exp_diff * (m_num_lines_exp)) -
                    (num_out_of_sight_lines * m_num_lines_exp) +
                    m_num_horizontal_lines;

  m_num_vertical_lines = (m_exp_diff * (m_num_lines_exp)) -
                         (num_out_of_sight_lines * m_num_lines_exp);

  m_vertical_grid_lines.clear();
  m_vertical_grid_lines.reserve(m_num_vertical_lines);

  m_horizontal_grid_lines.clear();
  m_horizontal_grid_lines.reserve(m_num_horizontal_lines);
}

void SemiLogXGrid::createGrid() {
  const auto [x, y, width, height] = getRectangleMeasures<float>(m_graph_area);

  if (!(width > 0.f && height > 0.f)) {
    return;
  }

  // Frist create the vertical lines
  for (float curr_exp = m_min_exp; curr_exp < m_max_exp; ++curr_exp) {
    const auto curr_x_pos_base = pow(10.f, curr_exp);

    const auto x_diff =
        pow(10.f, curr_exp + 1.f) / static_cast<float>(m_num_lines_exp);
    for (float line = 0; line < m_num_lines_exp; ++line) {
      const auto x_val = curr_x_pos_base + line * x_diff;
      addGridLineVertical<LogXGraphLine>(x_val);
    }
  }

  // Then create the horizontal lines
  auto y_diff = (m_limY.second - m_limY.first) / float(m_num_horizontal_lines);
  for (int i = 0; i <= m_num_horizontal_lines; ++i) {
    const auto y_pos = m_limY.first + float(i) * y_diff;
    addGridLineHorizontal<LinearGraphLine>(y_pos);
  }

  auto xToXPos = [&](const float x) {
    return width * (log(x / m_limX.first) / log(m_limX.second / m_limX.first));
  };

  auto yToYPos = [&](const float y) {
    return height * (m_limY.second - y) / (m_limY.second - m_limY.first);
  };

  setLabels(xToXPos, yToYPos);
}
