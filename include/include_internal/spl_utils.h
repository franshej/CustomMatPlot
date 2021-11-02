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
  constexpr explicit operator bool() const noexcept { return m_is_set; }
};

template <class T>
class ParamVal
    : public std::conditional_t<std::is_same_v<T, bool>, ParamBase<T>,
                                ExplicitBoolOperator<T>> {
 public:
  ParamVal(const T param) {
    m_is_set = false;
    m_param = param;
  };
  ParamVal(T& param) {
    m_is_set = false;
    m_param = param;
  };
  ParamVal() {
    m_is_set = false;
    m_param = T();
  };
  ~ParamVal() = default;

  T& operator=(const T& rhs) {
    if (&m_param == &rhs) return m_param;
    m_param = rhs;
    m_is_set = true;
    return m_param;
  }
  ParamVal<T> operator=(const ParamVal<T>& rhs) {
    if (this == &rhs) return *this;
    if (rhs) {
      m_param = rhs;
      m_is_set = true;
    }
    return *this;
  }
};
}  // namespace scp