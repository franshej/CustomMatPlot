#include "spl_grid.h"
#include <stdexcept>

void BaseGrid::paint(juce::Graphics &g) {
  g.setColour(m_grid_colour);

  for (const auto &path : m_grid_path) {
    g.strokePath(path, juce::PathStrokeType(1.0f));
  }
}

void BaseGrid::resized() { createGrid(); }

void BaseGrid::setGraphBounds(const juce::Rectangle<int> &graph_area) {
  m_graph_area = graph_area;
  createGrid();
}

void Grid::createGrid() {
  const auto &width = static_cast<float>(m_graph_area.getWidth());
  const auto &height = static_cast<float>(m_graph_area.getHeight());
  const auto &x = static_cast<float>(m_graph_area.getX());
  const auto &y = static_cast<float>(m_graph_area.getY());

  auto num_horizontal_lines = 3;
  if (width > 375.f) {
    num_horizontal_lines = 11;
  } else if (width <= 375.f && width > 135.f) {
    num_horizontal_lines = 5;
  }

  auto num_vertical_lines = 3;
  if (width > 435.f) {
    num_vertical_lines = 11;
  } else if (width <= 435.f && width > 175.f) {
    num_vertical_lines = 5;
  }

  // Height 375 pixlar ska den gå från 11 till 5 linjer, 135 till 3 linjer
  // Width 435 pixlar gå från 11 till 5 linjer, 175 till 3 linjer
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

  juce::PathStrokeType p_type(1.0f);
  const std::vector<float> dashed_lines = {4, 7};

  for (auto &path : m_grid_path) {
    p_type.createDashedStroke(path, path, dashed_lines.data(), 2);
  }
}
