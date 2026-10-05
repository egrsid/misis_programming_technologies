# Affine Practice

C++23 starter: fixed-size `std::array` storage, standard `std::mdspan`, concepts and 2D affine transformations. No AVX2, explicit SIMD, custom allocator or third-party dependencies.

[Task 1: matrix operations](docs/Matrix.md) · [Task 2: affine transformations](docs/Affine.md)

Implement the `todo(...)` placeholders in `include/practice`. Storage, views, concepts and tests are supplied. Do not replace failures with hard-coded answers.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build -L scaffold --output-on-failure
ctest --test-dir build -L matrix --output-on-failure
ctest --test-dir build -L affine --output-on-failure
cmake --build build --target format-check
```

Requires CMake 3.25+, a C++23 standard library with `std::mdspan`, and clang-format for Google-style formatting targets. Tested compilation with GCC/libstdc++ 16.2.1.

**Initial state:** the storage test passes; nine implementation tests fail with `TODO`. This is intentional. After completing both tasks, all ten tests must pass. Tests remain active in Release and exercise both `float` and `double`.

Source code contains no comments; put explanations in your submission report. Returning an owning matrix is allowed; returning a view of a local array is not.
