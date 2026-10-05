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
  Matrix<T, N, N> ans{};
  auto mtrx = ans.view();

  for (std::size_t i = 0; i < N; ++i) {
    mtrx[i, i] = 1;
  }

  return ans;
}

template <MatrixView A, MatrixView B>
  requires SameShape<A, B>
auto add(A a, B b) -> Matrix<typename A::value_type, A::static_extent(0),
                         A::static_extent(1)> {
  using T = typename A::value_type;
  constexpr std::size_t rows = A::static_extent(0);
  constexpr std::size_t cols = A::static_extent(1);

  Matrix<T, rows, cols> ans{};
  auto mtrx = ans.view();

  for (std::size_t i = 0; i < rows; ++i) {
    for (std::size_t j = 0; j < cols; ++j) {
      mtrx[i, j] = a[i, j] + b[i, j];
    }
  }

  return ans;
}

template <MatrixView A>
auto scale(A a, typename A::value_type factor)
    -> Matrix<typename A::value_type, A::static_extent(0),
              A::static_extent(1)> {
  using T = typename A::value_type;
  constexpr std::size_t rows = A::static_extent(0);
  constexpr std::size_t cols = A::static_extent(1);

  Matrix<T, rows, cols> ans{};
  auto mtrx = ans.view();

  for (std::size_t i = 0; i < rows; ++i) {
    for (std::size_t j = 0; j < cols; ++j) {
      mtrx[i, j] = a[i, j] * factor;
    }
  }

  return ans;
}

template <MatrixView A>
auto transpose(A a) -> Matrix<typename A::value_type, A::static_extent(1),
                            A::static_extent(0)> {
  using T = typename A::value_type;
  constexpr std::size_t rows = A::static_extent(0);
  constexpr std::size_t cols = A::static_extent(1);

  Matrix<T, cols, rows> ans{};
  auto mtrx = ans.view();

  for (std::size_t i = 0; i < cols; ++i) {
    for (std::size_t j = 0; j < rows; ++j) {
      mtrx[i, j] = a[j, i];
    }
  }

  return ans;
}

template <MatrixView A, MatrixView B>
  requires Multipliable<A, B>
auto multiply(A a, B b) -> Matrix<typename A::value_type, A::static_extent(0),
                              B::static_extent(1)> {
  using T = typename A::value_type;
  constexpr std::size_t rows = A::static_extent(0);
  constexpr std::size_t cols = B::static_extent(1);
  constexpr std::size_t K = A::static_extent(1);

  Matrix<T, rows, cols> ans{};
  auto mtrx = ans.view();

  for (std::size_t i = 0; i < rows; ++i) {
    for (std::size_t j = 0; j < cols; ++j) {
      for (std::size_t k = 0; k < K; ++k) {
        mtrx[i, j] += a[i, k] * b[k, j];
      }
    }
  }

  return ans;
}

}
