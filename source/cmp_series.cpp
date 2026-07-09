/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "cmp_series.h"

#include <cstddef>
#include <mutex>
#include <numeric>
#include <stdexcept>

#include "cmp_datamodels.h"
#include "cmp_downsampler.h"
#include "cmp_plot.h"

namespace cmp {

SeriesDataView::SeriesDataView(
    const std::vector<float>& _x_data, const std::vector<float>& _y_data,
    const PixelPoints& _pixel_points,
    const std::vector<std::size_t>& _pixel_point_indices,
    const SeriesAttribute& _series_attribute)
    : x_data(_x_data),
      y_data(_y_data),
      pixel_points(_pixel_points),
      pixel_point_indices(_pixel_point_indices),
      series_attribute(_series_attribute) {}

SeriesDataView::SeriesDataView(const Series& series)
    : x_data(series.getXData()),
      y_data(series.getYData()),
      pixel_points(series.getPixelPoints()),
      pixel_point_indices(series.getPixelPointIndices()),
      series_attribute(series.getSeriesAttribute()) {}

const SeriesAttribute& Series::getSeriesAttribute() const noexcept {
  return m_series_attributes;
}

void Series::setIndicesToUpdate(const std::vector<std::size_t> indices) {
  m_indices_to_update = indices;
  updateXY();
  m_indices_to_update.clear();
}

void Series::setColour(const juce::Colour series_colour) {
  m_series_attributes.series_colour = series_colour;
}

std::tuple<juce::Point<float>, juce::Point<float>, size_t>
Series::findClosestPixelPointTo(const juce::Point<float>& this_pixel_point,
                                bool check_only_distance_from_x) const {
  // No pixel points.
  jassert(!m_pixel_points.empty());

  auto closest_pixel_point = juce::Point<float>();
  auto closest_data_point = juce::Point<float>();
  auto closest_i = 0u;

  auto closest_distance = std::numeric_limits<float>::max();
  std::size_t i = 0u;
  for (const auto& pixel_point : m_pixel_points) {
    const auto current_distance =
        check_only_distance_from_x
            ? std::abs(pixel_point.getX() - this_pixel_point.getX())
            : pixel_point.getDistanceSquaredFrom(this_pixel_point);
    if (current_distance < closest_distance) {
      closest_distance = current_distance;
      closest_pixel_point = pixel_point;
      closest_i = i;
      closest_data_point = juce::Point<float>(m_x_data[m_xy_indices[i]],
                                              m_y_data[m_xy_indices[i]]);
    }
    i++;
  }

  return {closest_pixel_point, closest_data_point, m_xy_indices[closest_i]};
}

std::pair<juce::Point<float>, size_t> Series::findClosestDataPointTo(
    const juce::Point<float>& this_data_point, bool check_only_distance_from_x,
    bool only_visible_data_points) const {
  // No y_data empty.
  jassert(!m_x_data.empty());

  // x_data & y_data must have the same size
  jassert(m_x_data.size() == m_y_data.size());

  const decltype(m_x_based_ds_indices)* indices = &m_x_based_ds_indices;
  decltype(m_x_based_ds_indices) all_indices;

  if (!only_visible_data_points) {
    all_indices.resize(m_x_data.size());

    std::iota(all_indices.begin(), all_indices.end(), 0u);

    indices = &all_indices;
  }

  auto nearest_x_dist = std::numeric_limits<float>::max();
  std::size_t nearest_i = 0u;

  for (const auto i : *indices) {
    const auto x = m_x_data[i];
    const auto current_x_dist = std::abs(x - this_data_point.getX());

    if (current_x_dist < nearest_x_dist) {
      nearest_x_dist = current_x_dist;
      nearest_i = i;
    }
  }

  const auto closest_data_point =
      juce::Point<float>(m_x_data[nearest_i], m_y_data[nearest_i]);

  return {closest_data_point, nearest_i};
}

juce::Colour Series::getColour() const noexcept {
  return m_series_attributes.series_colour.value();
}

juce::Point<float> Series::getDataPointFromPixelPointIndex(
    size_t pixel_point_index) const {
  return juce::Point<float>(m_x_data[m_xy_indices[pixel_point_index]],
                            m_y_data[m_xy_indices[pixel_point_index]]);
};

juce::Point<float> Series::getDataPointFromDataPointIndex(
    size_t data_point_index) const {
  return juce::Point<float>(m_x_data[data_point_index],
                            m_y_data[data_point_index]);
};

void Series::observableValueUpdated(ObserverId id, const bool& new_value) {
  updateXY();
}

void Series::resized(){};

void Series::paint(juce::Graphics& g) {
  const SeriesDataView series_data(*this);

  if (m_lookandfeel) {
    const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawSeries(g, series_data, getLocalBounds());
  }
}

void Series::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
    updateXIndicesAndPixelPointsIntern({});
    updateYIndicesAndPixelPointsIntern({});
  } else {
    m_lookandfeel = nullptr;
  }
}

void Series::setSeriesAttribute(const SeriesAttribute& series_attribute) {
  if (series_attribute.dashed_lengths)
    m_series_attributes.dashed_lengths = series_attribute.dashed_lengths;

  if (series_attribute.series_colour)
    m_series_attributes.series_colour = series_attribute.series_colour;

  if (series_attribute.series_opacity)
    m_series_attributes.series_opacity = series_attribute.series_opacity;

  if (series_attribute.on_pixel_point_paint)
    m_series_attributes.on_pixel_point_paint =
        series_attribute.on_pixel_point_paint;

  if (series_attribute.path_stroke_type)
    m_series_attributes.path_stroke_type = series_attribute.path_stroke_type;

  if (series_attribute.marker)
    m_series_attributes.marker = series_attribute.marker;

  if (series_attribute.gradient_colours)
    m_series_attributes.gradient_colours = series_attribute.gradient_colours;
}

void Series::setYValues(const std::vector<float>& y_data) {
  if (m_y_data.size() != y_data.size()) m_y_data.resize(y_data.size());
  std::copy(y_data.begin(), y_data.end(), m_y_data.begin());
}

void Series::setXValues(const std::vector<float>& x_data) {
  if (m_x_data.size() != x_data.size()) m_x_data.resize(x_data.size());
  std::copy(x_data.begin(), x_data.end(), m_x_data.begin());
}

bool Series::setXYValue(const juce::Point<float>& xy_value, size_t index) {
  if (index >= m_x_data.size()) return false;

  m_x_data[index] = xy_value.getX();
  m_y_data[index] = xy_value.getY();

  return true;
}

void Series::movePixelPoint(const juce::Point<float>& d_pixel_point,
                            size_t pixel_point_index) {
  if (pixel_point_index >= m_x_data.size()) return;

  m_x_data[pixel_point_index] += d_pixel_point.getX();
  m_y_data[pixel_point_index] += d_pixel_point.getY();
}

const std::vector<float>& Series::getYData() const noexcept { return m_y_data; }

const std::vector<float>& Series::getXData() const noexcept { return m_x_data; }

const PixelPoints& Series::getPixelPoints() const noexcept {
  return m_pixel_points;
}

const std::vector<size_t>& Series::getPixelPointIndices() const noexcept {
  return m_xy_indices;
}

void Series::updateX() {
  // x_lim must be set to calculate the xdata.
  if (!m_x_lim || m_x_data.empty()) return;

  updateXIndicesAndPixelPointsIntern(m_indices_to_update);
}

void Series::updateY() {
  if (!m_y_lim || m_y_data.empty()) return;

  updateYIndicesAndPixelPointsIntern(m_indices_to_update);
}

void Series::updateXIndicesAndPixelPointsIntern(
    const std::vector<size_t>& update_only_these_indices) {
  const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

  switch (m_downsampling_type) {
    case DownsamplingType::no_downsampling:
      m_x_based_ds_indices.resize(m_x_data.size());

      std::iota(m_x_based_ds_indices.begin(), m_x_based_ds_indices.end(), 0u);
      break;

    case DownsamplingType::x_downsampling:
      Downsampler<float>::calculateXIndices(m_x_scaling, m_x_lim, m_axes_bounds,
                                            m_x_data, m_x_based_ds_indices);

      break;

    case DownsamplingType::xy_downsampling:
      Downsampler<float>::calculateXIndices(m_x_scaling, m_x_lim, m_axes_bounds,
                                            m_x_data, m_x_based_ds_indices);
      return;
      break;

    default:
      break;
  }

  auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
  m_pixel_points.resize(m_x_based_ds_indices.size());
  lnf->updateXPixelPoints(update_only_these_indices, m_x_scaling, m_x_lim,
                          m_axes_bounds, m_x_data, m_x_based_ds_indices,
                          m_pixel_points);
}

void Series::updateYIndicesAndPixelPointsIntern(
    const std::vector<size_t>& update_only_these_indices) {
  auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
  const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

  m_xy_indices = m_x_based_ds_indices;

  switch (m_downsampling_type) {
    case DownsamplingType::no_downsampling:
      break;

    case DownsamplingType::x_downsampling:
      break;

    case DownsamplingType::xy_downsampling:
      Downsampler<float>::calculateXYBasedIdxs(m_x_based_ds_indices, m_y_data,
                                               m_xy_indices);

      m_pixel_points.resize(m_xy_indices.size());
      lnf->updateXPixelPoints(update_only_these_indices, m_x_scaling, m_x_lim,
                              m_axes_bounds, m_x_data, m_xy_indices,
                              m_pixel_points);
      break;

    default:
      break;
  }

  m_pixel_points.resize(m_xy_indices.size());
  lnf->updateYPixelPoints(update_only_these_indices, m_y_scaling, m_y_lim,
                          m_axes_bounds, m_y_data, m_xy_indices,
                          m_pixel_points);
}

void Series::updateXY() {
  updateX();
  updateY();
}

void Series::setType(const SeriesType series_type) {
  m_series_type = series_type;
}

SeriesType Series::getType() const noexcept { return m_series_type; }

void Series::observableValueUpdated(ObserverId id, const Scaling& new_value) {
  if (id == ObserverId::XScaling) {
    m_x_scaling = new_value;
    updateX();
  } else if (id == ObserverId::YScaling) {
    m_y_scaling = new_value;
    updateY();
  }
}

void Series::observableValueUpdated(ObserverId id,
                                    const Lim<float>& new_value) {
  if (id == ObserverId::XLim) {
    m_x_lim = new_value;
    if (m_series_type == SeriesType::horizontal) {
      m_x_data.resize(2);
      m_x_data.front() = m_x_lim.min;
      m_x_data.back() = m_x_lim.max;
    }
    updateXY();
  } else if (id == ObserverId::YLim) {
    m_y_lim = new_value;
    if (m_series_type == SeriesType::vertical) {
      m_y_data.resize(2);
      m_y_data.front() = m_y_lim.min;
      m_y_data.back() = m_y_lim.max;
    }
    updateY();
  }
}

void Series::observableValueUpdated(ObserverId id,
                                    const juce::Rectangle<int>& new_value) {
  if (id == ObserverId::AxesBounds) {
    m_axes_bounds = new_value;
    updateXY();
  }
}

void Series::observableValueUpdated(ObserverId id,
                                    const DownsamplingType& new_value) {
  if (id == ObserverId::DownsamplingType) {
    m_downsampling_type = new_value;
    updateXY();
  }
}

/************************************************************************************/
/*********************************SeriesList**************************************/
/************************************************************************************/

size_t size_from_series_type(const SeriesList& SeriesList,
                             const SeriesType series_type) {
  return std::count_if(SeriesList.begin(), SeriesList.end(),
                       [series_type](const auto& series) {
                         return series->getType() == series_type;
                       });
}

template <>
size_t SeriesList::size<SeriesType::any>() const noexcept {
  // Explicitly Call size of parent class.
  return std::vector<std::unique_ptr<Series>>::size();
}

template <>
size_t SeriesList::size<SeriesType::normal>() const noexcept {
  return size_from_series_type(*this, SeriesType::normal);
}

template <>
size_t SeriesList::size<SeriesType::vertical>() const noexcept {
  return size_from_series_type(*this, SeriesType::vertical);
}

template <>
size_t SeriesList::size<SeriesType::horizontal>() const noexcept {
  return size_from_series_type(*this, SeriesType::horizontal);
}

template <SeriesType t_series_type>
void SeriesList::resize(size_t new_size_of_type) {
  const auto current_size = size<t_series_type>();

  if (current_size == new_size_of_type) return;

  if (current_size > new_size_of_type) {
    // sort the series by type. nullptrs are sorted to the end.
    std::sort(begin(), end(), [](const auto& lhs, const auto& rhs) {
      if (!lhs) return false;
      if (!rhs) return true;
      return lhs->getType() < rhs->getType();
    });

    const auto num_to_erase = current_size - new_size_of_type;
    const auto erase_begin = std::find_if(
        begin(), end(),
        [](const auto& series) { return series->getType() == t_series_type; });

    erase(erase_begin, erase_begin + num_to_erase);
  } else {
    const auto new_size =
        size<SeriesType::any>() - current_size + new_size_of_type;
    std::vector<std::unique_ptr<Series>>::resize(new_size);
  }
}

template void SeriesList::resize<SeriesType::normal>(size_t new_size_of_type);
template void SeriesList::resize<SeriesType::vertical>(size_t new_size_of_type);
template void SeriesList::resize<SeriesType::horizontal>(
    size_t new_size_of_type);

template <SeriesType t_series_type>
std::vector<Series*> SeriesList::getSeriesOfType() {
  std::vector<Series*> result;
  for (auto& line : *this) {
    if (line->getType() == t_series_type) {
      result.push_back(line.get());
    }
  }
  return result;
}

/************************************************************************************/
/*********************************Spread****************************************/
/************************************************************************************/

void Spread::resized() {}

void Spread::paint(juce::Graphics& g) {
  if (m_lookandfeel) {
    const std::lock_guard<std::recursive_mutex> lock(plot_mutex);

    auto lnf = static_cast<Plot::LookAndFeelMethods*>(m_lookandfeel);
    lnf->drawSpread(g, m_lower_bound, m_upper_bound, m_spread_colour);
  }
}

void Spread::lookAndFeelChanged() {
  if (auto* lnf = dynamic_cast<Plot::LookAndFeelMethods*>(&getLookAndFeel())) {
    m_lookandfeel = lnf;
  } else {
    m_lookandfeel = nullptr;
  }
}

}  // namespace cmp
