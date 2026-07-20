## Next version

### Fixed
- plotUpdateYOnly (realTimePlot).

### Added
- `plot(SeriesDataList)` / `SeriesData`: plot one or more series bundled per
  series, where each carries its own x, y and attributes (order is now x, y).
  A single series can be plotted without the list: `plot({.y = samples})`.
- `Plot3D`: 3D line plotting with per-axis linear/logarithmic scaling.
- `clear()` (and `Plot3D::clear()`): remove all series. Plotting an empty
  list does the same.
- `plotUpdateYOnly(std::vector<float>)`: single-series overload for the common
  real-time case, matching the single-series `plot({.y = ...})`.
- Renamed realTimePlot to plotUpdateYOnly.
- Gradient below series using SeriesAttribute

### Breaking Changes
- **Breaking:** Dropped the `Graph` prefix from the data-series API.
  `GraphLine` → `Series`, `GraphAttribute` → `SeriesAttribute`,
  `GraphSpread` → `Spread`, `drawGraphLine` → `drawSeries` and
  `ColourIdsGraph` → `ColourIdsSeries`. Update your code accordingly.
- **Breaking:** Removed `plot(y_data, x_data, series_attributes)`. Use
  `plot(SeriesDataList)` / `plot(SeriesData)`; each series now bundles its own
  x, y and attributes (order is x, y).
- **Breaking:** `Plot3D::plot3` now takes `std::vector<Series3DData>` instead
  of separate x, y and z vectors.
- **Breaking:** Renamed the axis-limit setters to follow the `set*` convention:
  `xLim`/`yLim` → `setXLim`/`setYLim` (and `Plot3D::zLim` → `setZLim`).
- **Breaking:** Fixed a typo in a public colour id:
  `ColourIds::transluent_grid_colour` → `translucent_grid_colour`.

## 1.3.0 (2024-9-12)

### Fixed
- xy-downsamling issue missing points.
- Fixed grid line auto ticks.
- Fixed grid line labels formatting.

### Added
- Panning for logarithmic scaling.
- CI: build & run unit tests.
- Unit tests.
- Update JUCE 7.0.12
- Draw vertical line.
- Draw horizontal line.

## 1.2.3 (2023-10-8)

### Fixed
- ctrl, shift and alt key were not detected on Windows.
- Crash on updating plot with different data set.

## 1.2.2 (2023-10-5)

### Fixed
- x auto zoom disable when zoomed in.
- Fix NaN caused by non-std math functions.

### Added
- Colour override function for lookandfeel.

## 1.2.1 (2023-09-24)

### Fixed
- Zero lims.
- Add tracepoint.

## 1.2.0 (2023-09-16)

### Added
- Added extras, including custom look and feels.
- Added custom grid types.

### Improved
- Improved grid coordinates and cache.

### Breaking Changes
- **Breaking:** Modified `cmp::GridType` when enabling the grid. Please update your code accordingly.
