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
}  // namespace scp