/**
 * Copyright (c) 2022 Frans Rosencrantz
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <vector>
#ifdef __cpp_constexpr
#if __cpp_constexpr >= 201907L
#define CONSTEXPR20 \
  constexpr  // This macro should be used in those cases where C++20 only
             // allows it. Example: virtual CONSTEXPR20 int foo() = 0;
#else
#define CONSTEXPR20
#endif
#endif

#ifdef __has_cpp_attribute
#if (__has_cpp_attribute(unlikely))
#define UNLIKELY [[unlikely]]
#else
#define UNLIKELY
#endif
#endif

#include <functional>
#include <optional>

#include "juce_gui_basics/juce_gui_basics.h"

namespace cmp {

/*============================================================================*/

class GraphLine;
class Grid;
class Frame;
class PlotLabel;
class Legend;
class Trace;
class GraphArea;
class PlotLookAndFeel;
template <typename T>
class Observable;
template <typename T>
class Observer;

struct LegendLabel;
struct GraphAttribute;
struct Marker;
struct GraphSpread;
struct GraphSpreadIndex;
struct GridLine;
struct GraphLineList;
struct AreLabelsSet;
struct CommonPlotParameterView;
struct GraphLineDataView;
template <class ValueType>
struct Lim;

/*============================================================================*/

typedef std::vector<std::unique_ptr<GraphLine>> GraphLines;
typedef std::vector<juce::Point<float>> PixelPoints;
typedef std::vector<GraphLineDataView> GraphLineDataViewList;
typedef std::pair<std::string, juce::Rectangle<int>> Label;
typedef std::vector<Label> LabelVector;
typedef std::vector<std::string> StringVector;
typedef std::vector<juce::Colour> ColourVector;
typedef std::vector<GraphAttribute> GraphAttributeList;
typedef std::vector<std::unique_ptr<GraphSpread>> GraphSpreadList;
typedef Lim<float> Lim_f;
// Callback function for when a graph line is changed. E.g. when it is changed
// when a pixel point is moved. void GraphLinesChangedCallback(const
// "GraphLineDataViewList &graph_line) { ... };""
typedef std::function<void(const GraphLineDataViewList& graph_line)>
    GraphLinesChangedCallback;

/*============================================================================*/

/** Enum to define the scaling of an axis. */
enum class Scaling : uint32_t {
  linear,     /** Linear scaling of the graph line. */
  logarithmic /** Logarithmic scaling of the graph line. */
};

/** Enum to define the type of downsampling. */
enum class DownsamplingType : uint32_t {
  no_downsampling, /** No downsampling. Slow when plotting alot of values. */
  x_downsampling,  /** Downsampling only based on the x-values, makes sure that
                    there is only one plotted value per x-pixel value. Fastest,
                    but will discard x-values that are located with the same
                    x-pixel value near each other. Recommended for real-time
                    plotting. */
  xy_downsampling, /** Skips x- & y-values that shares the same pixel on the
                      screen. It's quicker than 'no_downsampling' but slower
                      than 'x_downsampling'. */
};

enum class UserInput : uint64_t {
  // Mouse button pressed
  left = 1UL,
  right = 1UL << 1,
  middle = 1UL << 2,

  // Type of press
  end = 1UL << 16,
  start = 1UL << 17,
  drag = 1UL << 18,
  double_click = 1UL << 19,
  scroll_up = 1UL << 20,
  scroll_down = 1UL << 21,

  // Keyboard button
  shift = 1ULL << 32,
  ctrl = 1ULL << 33,
  alt = 1ULL << 34,

  // Event component
  graph_area = 1ULL << 46,
  legend = 1ULL << 47,
  tracepoint = 1ULL << 48,
  trace_label = 1ULL << 49,

};

constexpr enum UserInput operator|(const enum UserInput selfValue,
                                   const enum UserInput inValue) {
  return (enum UserInput)(uint64_t(selfValue) | uint64_t(inValue));
}

/** Enum to define a type of action that will occur for a input. */
enum class UserInputAction : uint32_t {
  /** Tracepoint related actions. */
  create_tracepoint,                /** Creates a tracepoint. */
  move_tracepoint_to_closest_point, /** Move a tracepoint to closest point to
                                       the mouse. */
  move_tracepoint_label,            /** Move a tracepoint label. */
  move_selected_trace_points,       /** Move a pixel point. */
  select_tracepoint,                /** Selecting a tracepoint. */
  select_tracepoints_within_selected_area, /** Selecting multiple tracepoints.
                                            */
  deselect_tracepoint,                     /** Deselecting a tracepoint. */

  /** Zoom related actions. */
  zoom_selected_area, /** Zoom in on selected area. */
  zoom_in,            /** Zoom in. */
  zoom_out,           /** Zoom out. */
  zoom_reset,         /** Reset the zoom. */

  /** Selection area related actions. */
  select_area_start, /** Set start positon for selected area. */
  select_area_draw,  /** Set end position of selected are and draw the area. */

  /** Pixel point related actions. */
  create_movable_pixel_point, /** Create a movable pixel point. */
  remove_movable_pixel_point, /** Remove a pixel point. */

  /** Legend related actions. */
  move_legend, /** Move a legend. */

  /** Panning */
  panning, /** Panning. */

  /** No action */
  none /** No action. */
};

/** Enum to define if the mouse has just start currently dragging or does not
 * drag. */
enum class MouseDragState : uint32_t {
  start, /** Start of a mouse drag. */
  drag,  /** Mouse is currently dragging. */
  none   /** No drag state. */
};

/** Enum to define when the tracepoint/Label should be visible or not. */
enum class TracePointVisibilityType : uint32_t {
  /** Tracepoint and label is not visible. */
  not_visible,
  /** Tracepoint is visible only when selected. Tracelabel is not visible. */
  point_visible_when_selected,
  /** Tracepoint and label are visible when selected. */
  point_label_visible_when_selected,
  /** Tracepoint is always visible. */
  visible,
};

/** Enum to define how a pixel point can be moved */
enum class PixelPointMoveType : uint32_t {
  /** Pixel point can't be moved. */
  none,
  /** Pixel point can be moved horizontally. */
  horizontal,
  /** Pixel point can be moved vertically. */
  vertical,
  /** Pixel point can be moved horizontally and vertically. */
  horizontal_vertical,
};

/** Enum to define what type of GraphLine. */
enum class GraphLineType : uint32_t {
  /** GraphLine is any type. */
  any,
  /** GraphLine is a normal graph line. */
  normal,
  /** GraphLine is a horizontal line. */
  horizontal,
  /** GraphLine is a vertical line. */
  vertical,
};

/** Enum to define if grid should be drawn or if the grid should be small */
enum class GridType : uint32_t {
  /** No grid is drawn. */
  none,
  /** Grid is drawn. */
  grid,
  /** Grid is drawn but with a smaller grid spacing. */
  tiny_grid,
  /** Grid drawn with translucent lines between. */
  grid_translucent,
  /** Tiny grid drawn with translucent lines between. */
  tiny_grid_translucent,
};

/** Enum to define which type of value to be observed. */
enum class ObserverId {
  GraphBounds,
  XLim,
  YLim,
  XScaling,
  YScaling,
};

/*============================================================================*/

/** @brief A template struct that defines min and max. */
template <class ValueType>
struct Lim {
  constexpr Lim(ValueType new_min, ValueType new_max)
      : min{new_min}, max{new_max} {}

  constexpr Lim(const juce::Point<ValueType> lim)
      : min{lim.getX()}, max{lim.getY()} {}

  ValueType min;
  ValueType max;

  constexpr Lim<ValueType>& operator/=(const ValueType val) {
    min /= val;
    max /= val;

    return *this;
  }

  constexpr Lim<ValueType>& operator=(const Lim<ValueType> rhs) {
    min = rhs.min;
    max = rhs.max;

    return *this;
  }

  bool operator==(const Lim<ValueType>& rhs) const noexcept {
    return min == rhs.min && max == rhs.max;
  }

  bool operator!=(const Lim<ValueType>& rhs) const noexcept {
    return min != rhs.min || max != rhs.max;
  }

  constexpr explicit operator bool() const noexcept {
    constexpr auto zero = static_cast<ValueType>(0);

    return max != zero || min != zero;
  }

  constexpr bool isMinOrMaxZero() const noexcept {
    constexpr auto zero = static_cast<ValueType>(0);

    return max == zero || min == zero;
  }

  constexpr Lim<ValueType> operator+(const ValueType val) {
    this->min += val;
    this->max += val;

    return *this;
  }

  constexpr Lim<ValueType> operator+=(const ValueType val) {
    this->min += val;
    this->max += val;

    return *this;
  }
};

template <class ValueType>
constexpr Lim<ValueType> operator/(const Lim<ValueType>& rhs,
                                   const ValueType val) {
  Lim<ValueType> new_lim;

  new_lim.min = rhs.min / val;
  new_lim.max = rhs.max / val;

  return new_lim;
};

template <class ValueType>
constexpr Lim<ValueType> operator+(const Lim<ValueType>& rhs,
                                   const ValueType val) {
  Lim<ValueType> new_lim;

  new_lim.min += val;
  new_lim.max += val;

  return new_lim;
};

/** @brief A struct that defines min and max using float. */
typedef Lim<float> Lim_f;

/** @brief A view of some common plot parameters. */
struct CommonPlotParameterView {
  CommonPlotParameterView(const juce::Rectangle<int>& gb, const Lim_f& xl,
                          const Lim_f& yl, const Scaling& xs, const Scaling& ys,
                          const DownsamplingType& ds)
      : graph_bounds{gb},
        x_lim{xl},
        y_lim{yl},
        x_scaling{xs},
        y_scaling{ys},
        downsampling_type{ds} {};
  CommonPlotParameterView(const juce::Rectangle<int>&&, const Lim_f&&,
                          const Lim_f&&, const Scaling&&, const Scaling&&,
                          const DownsamplingType&&) =
      delete;  // prevents rvalue binding
  const juce::Rectangle<int>& graph_bounds;
  const Lim_f &x_lim, &y_lim;
  const Scaling &x_scaling, &y_scaling;
  const DownsamplingType& downsampling_type;
};

struct Marker {
  /** Type of marker. */
  enum class Type {
    Circle,
    Pentagram,
    Square,
    UpTriangle,
    RightTriangle,
    DownTriangle,
    LeftTriangle
  } type;

  /** Contructor marker type only. */
  Marker(const Marker::Type t) : type{t} {};

  /** Marker outline color. */
  std::optional<juce::Colour> EdgeColour;

  /** Marker interior color. */
  std::optional<juce::Colour> FaceColour;

  /** PathStrokeType used when drawing the edge line of the marker. */
  juce::PathStrokeType edge_stroke_type{
      1.0f, juce::PathStrokeType::JointStyle::mitered,
      juce::PathStrokeType::EndCapStyle::rounded};

  static juce::Path getMarkerPathFrom(Marker marker, const float length) {
    juce::Path path;

    auto addUpTriangleTo = [length](auto& path) {
      path.addTriangle({0.0f, -length / 2.0f}, {-length / 2.0f, length / 2.0f},
                       {length / 2.0f, length / 2.0f});
    };

    switch (marker.type) {
      case Marker::Type::Circle:
        path.addEllipse({-length / 2.0f, -length / 2.0f, length, length});
        break;
      case Marker::Type::Pentagram:
        path.addStar({0, 0}, 5, length / 4.0f, length / 2.0f);
        break;
      case Marker::Type::Square:
        path.addRectangle(-length / 2.0f, -length / 2.0f, length, length);
        break;
      case Marker::Type::UpTriangle:
        addUpTriangleTo(path);
        break;
      case Marker::Type::RightTriangle:
        addUpTriangleTo(path);

        path.applyTransform(juce::AffineTransform::rotation(
            juce::MathConstants<float>::pi / 2.0f, 0.0f, 0.0f));
        break;
      case Marker::Type::DownTriangle:
        addUpTriangleTo(path);

        path.applyTransform(juce::AffineTransform::rotation(
            juce::MathConstants<float>::pi, 0.0f, 0.0f));
        break;
      case Marker::Type::LeftTriangle:
        addUpTriangleTo(path);

        path.applyTransform(juce::AffineTransform::rotation(
            juce::MathConstants<float>::pi * 3.0f / 2.0f, 0.0f, 0.0f));
        break;
      default:
        break;
    }

    return path;
  }
};

/** Attributes of a single graph. */
struct GraphAttribute {
  /** Colour of the graph_line. */
  std::optional<juce::Colour> graph_colour;

  /** Custom path stroke @see juce::PathStrokeType */
  std::optional<juce::PathStrokeType> path_stroke_type;

  /** Use dash_lengths to draw dashed graph_line. E.g. dashed_lengths = {2,
   * 2, 4, 6} will draw a line of 2 pixels, skip 2 pixels, draw 3 pixels, skip
   * 6 pixels, and then repeat. */
  std::optional<std::vector<float>> dashed_lengths;

  /** Set the opacity of the graph_line. Value must be between 0 (transparent)
   * and 1.0 (opaque). */
  std::optional<float> graph_line_opacity;

  /** The type of marker drawn on each pixel point. */
  std::optional<cmp::Marker> marker;

  /** Callback function which is triggerd for every plotted pixel_point. E.g.
   * Can be used to do custom plot markers for each pixel_point.*/
  std::function<void(juce::Graphics& g, juce::Point<float> data_point,
                     juce::Point<float> pixel_point)>
      on_pixel_point_paint{nullptr};

  /** Creates a vertical linear gradient between top and bottom of the graph
   * area. Only the gradient below the graph line is visible.  */
  std::optional<std::pair<juce::Colour, juce::Colour>> gradient_colours;
};

/** @brief A struct that defines between which two graph_lines the area is
 * filled. */
struct GraphSpreadIndex {
  std::size_t first_graph;
  std::size_t second_graph;
};

/** @brief A view of the data required to draw a graph_line */
struct GraphLineDataView {
  GraphLineDataView(const std::vector<float>& _x_data,
                    const std::vector<float>& _y_data,
                    const PixelPoints& _pixel_points,
                    const std::vector<std::size_t>& _pixel_point_indices,
                    const GraphAttribute& _graph_attribute);

  GraphLineDataView(const GraphLine& graph_line);
  GraphLineDataView(const std::vector<float>&&, const std::vector<float>&&,
                    const PixelPoints&&,
                    const std::vector<std::size_t>&&) =
      delete;  // prevents rvalue binding

  const std::vector<float>&x_data, &y_data;
  const PixelPoints& pixel_points;
  const std::vector<std::size_t>& pixel_point_indices;
  const GraphAttribute& graph_attribute;
};

/**
 * @brief A struct that defines a vector with fast push_back.
 *
 * This struct is used to speed up the downsampling process.
 * The push_back_fast method is about 20x - 30x faster than
 * std::vector::push_back.
 *
 * Example usage:
 * @code{.cpp}
 * std::vector<int> v;
 * fast_vector<int> fv(v, 50'000);
 *
 * for (int i = 0; i < 50'000; i++)
 *     fv.push_back(i);
 * @endcode
 *
 * @warning This will only work if the vector is resized with a large size
 * before resized to the correct size with resize_with_fast_push_back_size.
 *
 * @tparam T The type of elements stored in the vector.
 */
template <typename T>
struct fast_vector {
  /**
   * @brief Constructs a new fast vector object.
   *
   * This constructor initializes the vector with the given vector and resizes
   * it to the specified size.
   *
   * @param v The vector to initialize with.
   * @param size The size to resize the vector to.
   */
  constexpr fast_vector(std::vector<T>& v, std::size_t size) : vec(v) {
    vec.resize(size);
  }

  /**
   * @brief Destroys the fast vector object.
   *
   * Before the object is destroyed, the vector is resized using the
   * resize_with_fast_push_back_size function.
   */
  ~fast_vector() { resize_with_fast_push_back_size(); }

  /**
   * @brief Fast push_back for the vector.
   * @warning This will only work if the vector has a size already.
   * @param elem The element to be added to the vector.
   */
  constexpr void push_back(const T elem) noexcept { vec[index++] = elem; }

  /**
   * @brief push_back for the vector if the element is not in back of the
   * vector.
   * @warning This will only work if the vector has a size already.
   * @param elem The element to be added to the vector.
   */
  constexpr void push_back_if_not_in_back(const T elem) noexcept {
    if (vec.back() != elem) vec[index++] = elem;
  }

  fast_vector& operator=(const std::vector<T>& v) {
    vec = v;
    index = vec.size();
    return *this;
  }

  /**
   * @brief Resize the vector efficiently after using push_back.
   */
  constexpr void resize_with_fast_push_back_size() noexcept {
    vec.resize(index);
    index = 0u;
  }

  /**
   * @brief Get a const reference to the vector.
   * @return const std::vector<T>& A const reference to the vector.
   */
  constexpr const std::vector<T>& get() const noexcept { return vec; }

 private:
  std::size_t index = 0u;  ///< Index used for fast push_back.
  std::vector<T>& vec;     ///< Reference to the vector.
};

/*============================================================================*/
/*========================      Classes      =================================*/
/*============================================================================*/

template <typename T>
class Observer {
 public:
  virtual void valueUpdated(ObserverId id, const T& newValue) = 0;
};

template <typename T>
class Observable {
 public:
  Observable(ObserverId id, T value = T()) : id(id), value(value) {}

  Observable& operator=(const T& newValue) {
    value = newValue;
    notifyDependents();
    return *this;
  }

  Observable& operator=(const Observable& other) = delete;

  template <typename... Dependents>
  void addDependents(Dependents&... dependents) {
    (addDependent(dependents), ...);
  }
  operator const T&() const { return value; }
  ObserverId getId() const { return id; }
  const T* operator->() const { return &value; }

 private:
  ObserverId id;
  T value;
  std::vector<std::function<void(ObserverId, const T&)>> observers;

  template <typename ObserverType>
  void addDependent(ObserverType& dependent) {
    observers.push_back([&dependent](ObserverId id, const T& value) {
      dependent.valueUpdated(id, value);
    });
  }

  void notifyDependents() {
    for (auto& observer : observers) {
        observer(id, value);
    }
  }
};

/*============================================================================*/
/*========================     Functions     =================================*/
/*============================================================================*/

template <class ForwardIt, class ValueType>
CONSTEXPR20 void iota_delta(ForwardIt first, ForwardIt last, ValueType x0,
                            const ValueType dx) {
  while (first != last) {
    *first++ = x0;
    x0 += dx;
  }
}

template <class ValueType, class ForwardIt>
CONSTEXPR20 void iota_delta(ForwardIt first, ForwardIt last, ValueType x0,
                            const ValueType dx,
                            std::function<ValueType(ValueType)> f) {
  while (first != last) {
    *first++ = f(x0);
    x0 += dx;
  }
}

/*============================================================================*/

}  // namespace cmp
