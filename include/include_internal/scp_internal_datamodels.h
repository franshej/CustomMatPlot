#pragma once

namespace scp {
/*============================================================================*/

class LookAndFeelMethodsBase;

/*============================================================================*/

template <class T>
class ParamBase {
 public:
  operator T() const { return m_param; }

 protected:
  bool m_is_set{false};
  T m_param;
};

template <class T>
class ExplicitBoolOperator : public ParamBase<T> {
 public:
  constexpr explicit operator bool() const noexcept { return this->m_is_set; }
};

template <class T>
class ParamVal
    : public std::conditional_t<std::is_same_v<T, bool>, ParamBase<T>,
                                ExplicitBoolOperator<T>> {
 public:
  ParamVal(const T param) {
    this->m_is_set = false;
    this->m_param = param;
  };
  ParamVal(T& param) {
    this->m_is_set = false;
    this->m_param = param;
  };
  ParamVal() {
    this->m_is_set = false;
    this->m_param = T();
  };
  ~ParamVal() = default;

  T& operator=(const T& rhs) {
    if (&this->m_param == &rhs) return this->m_param;
    this->m_param = rhs;
    this->m_is_set = true;
    return this->m_param;
  }
  ParamVal<T> operator=(const ParamVal<T>& rhs) {
    if (this == &rhs) return *this;
    if (rhs) {
      this->m_param = rhs;
      this->m_is_set = true;
    }
    return *this;
  }
};

/*============================================================================*/

class BaseGraphLine : public juce::Component {
 public:
  virtual ~BaseGraphLine() = default;
  virtual void setColour(const juce::Colour graph_colour) = 0;
  virtual juce::Colour getColour() const noexcept = 0;
  virtual void setXLim(const float min, const float max) = 0;
  virtual void setYLim(const float min, const float max) = 0;
  virtual void setXValues(const std::vector<float>& x_values) noexcept = 0;
  virtual void setYValues(const std::vector<float>& y_values) noexcept = 0;
  virtual const std::vector<float>& getYValues() noexcept = 0;
  virtual const std::vector<float>& getXValues() noexcept = 0;
  virtual const std::vector<juce::Point<float>>& getGraphPoints() noexcept = 0;
  virtual void setDashedPath(
      const std::vector<float>& dashed_lengths) noexcept = 0;
  virtual void updateYGraphPoints() = 0;
  virtual void updateXGraphPoints() = 0;
};

/*============================================================================*/

}  // namespace scp