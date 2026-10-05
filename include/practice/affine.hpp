#pragma once

#include "practice/matrix.hpp"

namespace practice {

template <Scalar T>
struct Point {
  T x{}, y{};
};

template <Scalar T>
using Transform = Matrix<T, 3, 3>;

template <Scalar T>
Transform<T> translation(T, T) {
  todo("translation");
}

template <Scalar T>
Transform<T> scaling(T, T) {
  todo("scaling");
}

template <Scalar T>
Transform<T> rotation(T) {
  todo("rotation");
}

template <Scalar T>
Transform<T> compose(const Transform<T>&, const Transform<T>&) {
  todo("compose");
}

template <Scalar T>
Point<T> transform_point(const Transform<T>&, Point<T>) {
  todo("transform_point");
}

template <Scalar T>
Point<T> transform_direction(const Transform<T>&, Point<T>) {
  todo("transform_direction");
}

}
