## Next version

### Fixed
- plotUpdateYOnly (realTimePlot).

### Added
- Renamed realTimePlot to plotUpdateYOnly.

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
