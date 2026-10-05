#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <limits>
#include <mdspan>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace practice {

template <class T>
concept Scalar = std::same_as<T, float> || std::same_as<T, double>;

template <Scalar T, std::size_t Rows, std::size_t Cols>
struct Matrix {
  static_assert(Cols == 0 ||
                Rows <= std::numeric_limits<std::size_t>::max() / Cols);

  using value_type = T;

  std::array<T, Rows * Cols> elements{};

  auto view() & {
    return std::mdspan<T, std::extents<std::size_t, Rows, Cols>>(
        elements.data());
  }

  auto view() const& {
    return std::mdspan<const T, std::extents<std::size_t, Rows, Cols>>(
        elements.data());
  }

  auto view() && = delete;
  auto view() const&& = delete;
};

template <class M>
concept MatrixView = requires(const M& m, std::size_t i) {
  typename M::element_type;
  typename M::value_type;
  requires Scalar<typename M::value_type>;
  requires M::rank() == 2;
  requires M::static_extent(0) != std::dynamic_extent;
  requires M::static_extent(1) != std::dynamic_extent;
  { m[i, i] } -> std::convertible_to<typename M::value_type>;
};

template <class A, class B>
concept SameShape =
    MatrixView<A> && MatrixView<B> &&
    std::same_as<typename A::value_type, typename B::value_type> &&
    A::static_extent(0) == B::static_extent(0) &&
    A::static_extent(1) == B::static_extent(1);

template <class A, class B>
concept Multipliable =
    MatrixView<A> && MatrixView<B> &&
    std::same_as<typename A::value_type, typename B::value_type> &&
    A::static_extent(1) == B::static_extent(0);

[[noreturn]] inline void todo(const char* operation) {
  throw std::logic_error(std::string("TODO: ") + operation);
}

template <Scalar T, std::size_t N>
Matrix<T, N, N> identity() {
  todo("identity");
}

template <MatrixView A, MatrixView B>
  requires SameShape<A, B>
auto add(A, B) -> Matrix<typename A::value_type, A::static_extent(0),
                         A::static_extent(1)> {
  todo("add");
}

template <MatrixView A>
auto scale(A, typename A::value_type)
    -> Matrix<typename A::value_type, A::static_extent(0),
              A::static_extent(1)> {
  todo("scale");
}

template <MatrixView A>
auto transpose(A) -> Matrix<typename A::value_type, A::static_extent(1),
                            A::static_extent(0)> {
  todo("transpose");
}

template <MatrixView A, MatrixView B>
  requires Multipliable<A, B>
auto multiply(A, B) -> Matrix<typename A::value_type, A::static_extent(0),
                              B::static_extent(1)> {
  todo("multiply");
}

}
