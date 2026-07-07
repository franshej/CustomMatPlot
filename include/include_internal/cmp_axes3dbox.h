/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * @file cmp_axes3dbox.h
 *
 * @brief Componenet for drawing the 3D axes box (the 3D analog of the grid).
 *
 * @ingroup CustomMatPlotInternal
 */

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>

#include "cmp_camera3d.h"
#include "cmp_datamodels.h"
#include "cmp_math3d.h"

namespace cmp {

/** @brief The six faces of the data cube. */
enum class CubeFace : uint32_t { x_min, x_max, y_min, y_max, z_min, z_max };

/** @brief The data value of the fixed coordinate of a cube face. */
inline float getFaceValue(const CubeFace face, const Axes3& axes) noexcept {
  switch (face) {
    case CubeFace::x_min:
      return axes.x.lim.min;
    case CubeFace::x_max:
      return axes.x.lim.max;
    case CubeFace::y_min:
      return axes.y.lim.min;
    case CubeFace::y_max:
      return axes.y.lim.max;
    case CubeFace::z_min:
      return axes.z.lim.min;
    case CubeFace::z_max:
    default:
      return axes.z.lim.max;
  }
}

/** @brief The outward normal of a cube face. */
inline Vec3f getFaceNormal(const CubeFace face) noexcept {
  switch (face) {
    case CubeFace::x_min:
      return {-1.0f, 0.0f, 0.0f};
    case CubeFace::x_max:
      return {1.0f, 0.0f, 0.0f};
    case CubeFace::y_min:
      return {0.0f, -1.0f, 0.0f};
    case CubeFace::y_max:
      return {0.0f, 1.0f, 0.0f};
    case CubeFace::z_min:
      return {0.0f, 0.0f, -1.0f};
    case CubeFace::z_max:
    default:
      return {0.0f, 0.0f, 1.0f};
  }
}

/** @brief Whether a cube face faces away from the camera.
 *
 * A face is a back face when its outward normal points away from the
 * viewer. The grid is drawn on the back faces so it never covers the data.
 */
inline bool isBackFace(const CubeFace face, const Camera3D& camera) noexcept {
  // The depth component of the view-space normal is the dot product of the
  // normal and the direction towards the viewer. The tolerance keeps faces
  // seen exactly edge-on (e.g. the side faces in a top view) from counting
  // as back faces.
  constexpr auto edge_on_tolerance = 1e-6f;

  return camera.toViewSpace(getFaceNormal(face)).z < -edge_on_tolerance;
}

/** @brief The faces to draw the grid on for a given camera view. */
inline std::vector<CubeFace> selectBackFaces(const Camera3D& camera) {
  auto back_faces = std::vector<CubeFace>();

  for (const auto face :
       {CubeFace::x_min, CubeFace::x_max, CubeFace::y_min, CubeFace::y_max,
        CubeFace::z_min, CubeFace::z_max}) {
    if (isBackFace(face, camera)) back_faces.push_back(face);
  }

  return back_faces;
}

/** @brief The y-side face along which the x tick labels are drawn: the
 * bottom edge of the box on the side facing the viewer. */
inline CubeFace getXTickLabelYFace(const Camera3D& camera) noexcept {
  return isBackFace(CubeFace::y_min, camera) ? CubeFace::y_max
                                             : CubeFace::y_min;
}

/** @brief The x-side face along which the y tick labels are drawn: the
 * bottom edge of the box on the side facing the viewer. */
inline CubeFace getYTickLabelXFace(const Camera3D& camera) noexcept {
  return isBackFace(CubeFace::x_min, camera) ? CubeFace::x_max
                                             : CubeFace::x_min;
}

/** @brief The z-side face along which the x and y tick labels are drawn:
 * the back z face, i.e. the bottom of the box when viewed from above. */
inline CubeFace getTickLabelZFace(const Camera3D& camera) noexcept {
  return isBackFace(CubeFace::z_min, camera) ? CubeFace::z_min
                                             : CubeFace::z_max;
}

/** @brief The vertical edge along which the z tick labels are drawn: the
 * edge between the front x-side and the back y-side, like MATLAB places the
 * z-axis. Returns {x-side face, y-side face}. */
inline std::pair<CubeFace, CubeFace> getZTickLabelEdge(
    const Camera3D& camera) noexcept {
  const auto x_face = isBackFace(CubeFace::x_min, camera) ? CubeFace::x_max
                                                          : CubeFace::x_min;
  const auto y_face = isBackFace(CubeFace::y_max, camera) ? CubeFace::y_max
                                                          : CubeFace::y_min;

  return {x_face, y_face};
}

/**
 *  \class Axes3DBox
 *  \brief A class component to draw the axes box of a 3D plot. This is a
 *  subcomponenet to cmp::Plot3D.
 *
 *  Draws grid lines on the back faces of the data cube and tick labels
 *  along the bottom and vertical box edges.
 */
class Axes3DBox : public juce::Component {
 public:
  /** @brief Set the axes and camera and regenerate the ticks.
   *
   *  @param axes the three plot axes.
   *  @param camera the camera view.
   *  @return void.
   */
  void setParameters(const Axes3& axes, const Camera3D& camera);

  /** @brief Get the auto-generated x-ticks. */
  const std::vector<float>& getXTicks() const noexcept;

  /** @brief Get the auto-generated y-ticks. */
  const std::vector<float>& getYTicks() const noexcept;

  /** @brief Get the auto-generated z-ticks. */
  const std::vector<float>& getZTicks() const noexcept;

  /** @brief Get the x grid-line positions (log axes add minor lines). */
  const std::vector<float>& getXGridLines() const noexcept;

  /** @brief Get the y grid-line positions (log axes add minor lines). */
  const std::vector<float>& getYGridLines() const noexcept;

  /** @brief Get the z grid-line positions (log axes add minor lines). */
  const std::vector<float>& getZGridLines() const noexcept;

  //==============================================================================

  /** @internal */
  void resized() override;
  /** @internal */
  void paint(juce::Graphics& g) override;
  /** @internal */
  void lookAndFeelChanged() override;

 private:
  /** @internal */
  void updateTicks();

  Axes3 m_axes;
  Camera3D m_camera;
  // Ticks drive the labels; grid lines add log minor lines between decades.
  std::vector<float> m_x_ticks, m_y_ticks, m_z_ticks;
  std::vector<float> m_x_grid, m_y_grid, m_z_grid;
  juce::LookAndFeel* m_lookandfeel{nullptr};
};

}  // namespace cmp
