/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_projection_bench.cpp
 *
 * @brief Measures where 3D plot frame time actually goes.
 *
 * Compares the two per-point stages of a 3D repaint at several point counts:
 *
 *   project : Projector3D::updatePixelPoints, the (x, y, z) -> pixel maths.
 *   draw    : PlotLookAndFeel::drawSeries, building and stroking the path.
 *
 * The ratio between them says which stage is worth optimising. Both stages
 * run once per repaint on the same points, so their times are directly
 * comparable.
 */

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <vector>

#include "cmp_camera3d.h"
#include "cmp_datamodels.h"
#include "cmp_downsampler.h"
#include "cmp_lookandfeel.h"
#include "cmp_projector3d.h"

namespace {

using Clock = std::chrono::steady_clock;
using Seconds = std::chrono::duration<double>;

constexpr int bounds_width = 800;
constexpr int bounds_height = 600;

/** Run 'fn' repeatedly and return the fastest wall-clock time.
 *
 * The minimum is used rather than the mean: it is the run least disturbed by
 * scheduling and other noise, so it is the most stable estimate of the cost.
 */
template <class Fn>
double timeBestOf(const int repeats, Fn&& fn) {
  auto best = std::numeric_limits<double>::max();

  for (int i = 0; i < repeats; ++i) {
    const auto start = Clock::now();
    fn();
    const auto elapsed = Seconds(Clock::now() - start).count();

    best = std::min(best, elapsed);
  }

  return best;
}

/** A 3D helix, so the projected path sweeps across the whole axes area
 * instead of degenerating into a short line. */
void makeHelix(const std::size_t num_points, std::vector<float>& x_data,
               std::vector<float>& y_data, std::vector<float>& z_data) {
  x_data.resize(num_points);
  y_data.resize(num_points);
  z_data.resize(num_points);

  for (std::size_t i = 0; i < num_points; ++i) {
    const auto t = float(i) / float(num_points) * 12.0f * 3.14159265f;

    x_data[i] = 5.0f + 5.0f * std::cos(t);
    y_data[i] = 5.0f + 5.0f * std::sin(t);
    z_data[i] = 10.0f * float(i) / float(num_points);
  }
}

void runForPointCount(const std::size_t num_points,
                      cmp::PlotLookAndFeelBase& lnf, const bool save_images) {
  std::vector<float> x_data, y_data, z_data;
  makeHelix(num_points, x_data, y_data, z_data);

  std::vector<std::size_t> indices(num_points);
  std::iota(indices.begin(), indices.end(), 0u);

  const auto axes = cmp::Axes3{{{0.f, 10.f}, cmp::Scaling::linear},
                               {{0.f, 10.f}, cmp::Scaling::linear},
                               {{0.f, 10.f}, cmp::Scaling::linear}};
  const auto bounds = juce::Rectangle<int>(0, 0, bounds_width, bounds_height);
  const auto camera = cmp::Camera3D();

  cmp::PixelPoints pixel_points;

  // Enough repeats to stay above timer resolution at small point counts.
  const auto repeats = num_points <= 10'000 ? 200 : 20;

  const auto project_time = timeBestOf(repeats, [&] {
    const cmp::Projector3D projector(axes, camera, bounds);
    projector.updatePixelPoints(x_data, y_data, z_data, pixel_points);
  });

  // drawSeries dereferences series_colour unconditionally, so it must be set.
  cmp::SeriesAttribute attribute;
  attribute.series_colour = juce::Colours::aqua;

  juce::Image image(juce::Image::ARGB, bounds_width, bounds_height, true);

  const auto timeDraw = [&](const cmp::PixelPoints& points) {
    const cmp::SeriesDataView series_data(x_data, y_data, points, indices,
                                          attribute);

    return timeBestOf(repeats, [&] {
      juce::Graphics g(image);
      lnf.drawSeries(g, series_data, bounds);
    });
  };

  const auto draw_all_time = timeDraw(pixel_points);

  // The downsampled points, compacted the same way Series3D compacts them.
  std::vector<std::size_t> kept_indices;

  const auto downsample_time = timeBestOf(repeats, [&] {
    cmp::Downsampler<float>::calculatePixelBasedIdxs(pixel_points,
                                                     kept_indices);
  });

  cmp::PixelPoints downsampled(kept_indices.size());
  for (std::size_t i = 0; i < kept_indices.size(); ++i) {
    downsampled[i] = pixel_points[kept_indices[i]];
  }

  const auto draw_ds_time = timeDraw(downsampled);

  // Downsampling is only worth anything if it is invisible, so the two
  // renderings are compared pixel by pixel.
  const auto render = [&](const cmp::PixelPoints& points) {
    juce::Image out(juce::Image::ARGB, bounds_width, bounds_height, true);
    const cmp::SeriesDataView series_data(x_data, y_data, points, indices,
                                          attribute);
    juce::Graphics g(out);
    lnf.drawSeries(g, series_data, bounds);
    return out;
  };

  const auto image_all = render(pixel_points);
  const auto image_ds = render(downsampled);

  // A pixel is "covered" if the series drew anything on it at all. Comparing
  // coverage separates a genuinely missing line from a line that merely landed
  // a fraction of a pixel to the side.
  std::size_t differing_pixels = 0u;
  std::size_t covered_all = 0u;
  std::size_t covered_ds = 0u;
  std::size_t covered_only_in_one = 0u;
  std::size_t isolated_diffs = 0u;

  const auto isCovered = [](const juce::Colour c) {
    return c.getAlpha() > 0;
  };

  for (int y = 0; y < bounds_height; ++y) {
    for (int x = 0; x < bounds_width; ++x) {
      const auto a = image_all.getPixelAt(x, y);
      const auto b = image_ds.getPixelAt(x, y);

      if (isCovered(a)) ++covered_all;
      if (isCovered(b)) ++covered_ds;
      if (a == b) continue;

      ++differing_pixels;

      if (isCovered(a) != isCovered(b)) {
        ++covered_only_in_one;

        // If the other image covers a neighbouring pixel, the line just moved
        // slightly rather than disappearing.
        auto has_neighbour = false;
        const auto& other = isCovered(a) ? image_ds : image_all;

        for (int dy = -1; dy <= 1 && !has_neighbour; ++dy) {
          for (int dx = -1; dx <= 1 && !has_neighbour; ++dx) {
            const auto nx = x + dx;
            const auto ny = y + dy;

            if (nx < 0 || ny < 0 || nx >= bounds_width || ny >= bounds_height)
              continue;

            has_neighbour = isCovered(other.getPixelAt(nx, ny));
          }
        }

        if (!has_neighbour) ++isolated_diffs;
      }
    }
  }

  std::printf(
      "  [covered px: %zu -> %zu | differing: %zu | coverage-flips: %zu | "
      "isolated (real loss): %zu]\n",
      covered_all, covered_ds, differing_pixels, covered_only_in_one,
      isolated_diffs);

  // Write out one pair so the result can be judged by eye, not just by
  // counters. Opt-in, so a plain run leaves no files behind.
  if (save_images && num_points == 100'000u) {
    const auto save = [&](const juce::Image& src, const juce::String& name) {
      juce::Image opaque(juce::Image::RGB, bounds_width, bounds_height, false);
      {
        juce::Graphics g(opaque);
        g.fillAll(juce::Colour(0xff2C3E50));
        g.drawImageAt(src, 0, 0);
      }

      juce::File out =
          juce::File::getCurrentWorkingDirectory().getChildFile(name);
      out.deleteFile();

      juce::FileOutputStream stream(out);
      juce::PNGImageFormat png;
      png.writeImageToStream(opaque, stream);
    };

    save(image_all, "bench_all_points.png");
    save(image_ds, "bench_downsampled.png");
  }

  // A repaint is project + downsample + draw, so they are summed to compare
  // the whole frame before and after.
  const auto before = project_time + draw_all_time;
  const auto after = project_time + downsample_time + draw_ds_time;

  const auto to_us = [](const double s) { return s * 1e6; };

  std::printf("%10zu %9zu %11.1f %11.1f %11.1f %9.1fx\n", num_points,
              downsampled.size(), to_us(project_time), to_us(before),
              to_us(after), before / after);
}

}  // namespace

int main(int argc, char* argv[]) {
  auto gui_scope = juce::ScopedJuceInitialiser_GUI();

  // '--save-images' writes the 100k renderings to the working directory so the
  // downsampling can be checked by eye.
  auto save_images = false;
  for (int i = 1; i < argc; ++i) {
    if (juce::String(argv[i]) == "--save-images") save_images = true;
  }

  cmp::PlotLookAndFeel lnf;

  std::printf("3D plot per-repaint cost, best of N runs (%dx%d axes area)\n\n",
              bounds_width, bounds_height);
  std::printf("%10s %9s %11s %11s %11s %10s\n", "points", "kept",
              "project[us]", "before[us]", "after[us]", "speedup");
  std::printf("%10s %9s %11s %11s %11s %10s\n", "------", "----", "-----------",
              "----------", "---------", "-------");

  for (const std::size_t num_points :
       {100u, 1'000u, 10'000u, 100'000u, 1'000'000u}) {
    runForPointCount(num_points, lnf, save_images);
  }

  return 0;
}
