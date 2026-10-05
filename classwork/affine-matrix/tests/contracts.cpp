#include <cmath>
#include <iostream>
#include <numbers>
#include <string_view>
#include <utility>

#include "practice/affine.hpp"

using namespace practice;

using V23 = decltype(std::declval<Matrix<double, 2, 3>&>().view());
using V32 = decltype(std::declval<Matrix<double, 3, 2>&>().view());
using F23 = decltype(std::declval<Matrix<float, 2, 3>&>().view());

static_assert(MatrixView<V23>);
static_assert(!MatrixView<int>);
static_assert(!MatrixView<std::mdspan<double, std::dextents<std::size_t, 2>>>);
static_assert(Multipliable<V23, V32>);
static_assert(!Multipliable<V23, V23>);
static_assert(!SameShape<V23, V32>);
static_assert(!SameShape<V23, F23>);

template <class A, class B>
concept CanAdd = requires(A a, B b) { add(a, b); };

template <class A, class B>
concept CanMultiply = requires(A a, B b) { multiply(a, b); };

static_assert(!CanAdd<V23, V32>);
static_assert(!CanMultiply<V23, V23>);
static_assert(std::same_as<decltype(std::declval<const Matrix<double, 2, 3>&>()
                                        .view()[0, 0]),
                           const double&>);

void check(bool ok) {
  if (!ok)
    throw std::runtime_error("contract mismatch");
}

template <Scalar T>
void near(T actual, T expected) {
  const T tolerance = std::same_as<T, float> ? T(1e-5) : T(1e-12);

  check(std::isfinite(actual) &&
        std::abs(actual - expected) <= tolerance * (T{1} + std::abs(expected)));
}

template <Scalar T>
void run(std::string_view name) {
  if (name == "storage") {
    Matrix<T, 2, 3> a;
    auto v = a.view();
    v[1, 2] = T{7};

    near(a.elements[5], T{7});

    const auto& read = a;

    near(read.view()[1, 2], T{7});
  } else if (name == "identity") {
    auto a = identity<T, 3>();
    for (std::size_t i = 0; i < 3; ++i)
      for (std::size_t j = 0; j < 3; ++j)
        near(a.view()[i, j], T(i == j));

    check(identity<T, 0>().elements.empty());
  } else if (name == "add_scale") {
    const Matrix<T, 2, 3> a{{1, 2, 3, 4, 5, 6}}, b{{6, 5, 4, 3, 2, 1}};
    auto c = add(a.view(), b.view());
    for (auto x : c.elements)
      near(x, T{7});
    auto d = scale(a.view(), T{-2});
    for (std::size_t i = 0; i < 6; ++i)
      near(d.elements[i], T{-2} * a.elements[i]);
  } else if (name == "transpose") {
    const Matrix<T, 2, 3> a{{1, 2, 3, 4, 5, 6}};
    auto t = transpose(a.view());
    const std::array<T, 6> expected{1, 4, 2, 5, 3, 6};

    check(t.elements == expected);

    auto restored = transpose(t.view());

    check(restored.elements == a.elements);
  } else if (name == "multiply") {
    const Matrix<T, 2, 3> a{{1, 2, 3, 4, 5, 6}};
    const Matrix<T, 3, 2> b{{7, 8, 9, 10, 11, 12}};
    auto c = multiply(a.view(), b.view());

    check(c.elements == std::array<T, 4>{58, 64, 139, 154});

    Matrix<T, 2, 0> empty_left;
    Matrix<T, 0, 3> empty_right;
    auto zero = multiply(empty_left.view(), empty_right.view());
    for (auto value : zero.elements)
      near(value, T{});
  } else if (name == "layout") {
    std::array<T, 6> values{1, 4, 2, 5, 3, 6};
    std::mdspan<const T, std::extents<std::size_t, 2, 3>, std::layout_left>
        column_major(values.data());
    const Matrix<T, 2, 3> row_major{{1, 2, 3, 4, 5, 6}};
    auto sum = add(column_major, row_major.view());
    for (std::size_t i = 0; i < 6; ++i)
      near(sum.elements[i], T{2} * row_major.elements[i]);
  } else if (name == "translation") {
    auto t = translation(T{3}, T{-2});
    auto p = transform_point(t, Point<T>{1, 4});

    near(p.x, T{4});
    near(p.y, T{2});

    auto d = transform_direction(t, Point<T>{1, 4});

    near(d.x, T{1});
    near(d.y, T{4});
  } else if (name == "scaling") {
    auto t = scaling(T{-2}, T{0});
    auto p = transform_point(t, Point<T>{3, 4});

    near(p.x, T{-6});
    near(p.y, T{});
  } else if (name == "rotation") {
    auto r = rotation(std::numbers::pi_v<T> / T{2});
    auto p = transform_point(r, Point<T>{1, 0});

    near(p.x, T{});
    near(p.y, T{1});

    auto inv = rotation(-std::numbers::pi_v<T> / T{2});
    p = transform_point(inv, p);

    near(p.x, T{1});
    near(p.y, T{});
  } else if (name == "composition") {
    auto t = translation(T{10}, T{0});
    auto r = rotation(std::numbers::pi_v<T> / T{2});
    auto s = scaling(T{2}, T{3});
    auto combined = compose(t, compose(r, s));
    auto p = transform_point(combined, Point<T>{1, 1});

    near(p.x, T{7});
    near(p.y, T{2});

    auto sequential = transform_point(
        t, transform_point(r, transform_point(s, Point<T>{1, 1})));

    near(p.x, sequential.x);
    near(p.y, sequential.y);

    auto reversed = transform_point(compose(r, t), Point<T>{1, 0});

    near(reversed.x, T{});
    near(reversed.y, T{11});
  } else
    throw std::invalid_argument("unknown test");
}

int main(int argc, char** argv) {
  try {
    if (argc != 2)
      throw std::invalid_argument("expected test name");
    run<float>(argv[1]);
    run<double>(argv[1]);
    std::cout << argv[1] << ": passed\n";
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
