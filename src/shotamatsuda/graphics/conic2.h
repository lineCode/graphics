//
//  shotamatsuda/graphics/conic2.h
//
//  The MIT License
//
//  Copyright (C) 2013-2017 Shota Matsuda
//
//  Includes algorithm based on Skia <skia.org>
//  Copyright (C) 2006 The Android Open Source Project
//  Distributed under a BSD-style license
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//

#pragma once
#ifndef SHOTA_GRAPHICS_CONIC2_H_
#define SHOTA_GRAPHICS_CONIC2_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

#include "shotamatsuda/math/promotion.h"
#include "shotamatsuda/math/vector.h"

namespace shotamatsuda {
namespace graphics {

// Based on Skia's SkGeometry

template <class T, int D>
class Conic;

template <class T>
using Conic2 = Conic<T, 2>;

template <class T>
class Conic<T, 2> final {
 public:
  using Type = T;
  using Point = Vec2<T>;
  static constexpr const int dimensions = 2;

 public:
  Conic();
  Conic(const Point& a,
        const Point& b,
        const Point& c,
        math::Promote<T> weight);

  // Copy semantics
  Conic(const Conic&) = default;
  Conic& operator=(const Conic&) = default;

  // Subdivision
  std::vector<Point> quadratics() const;
  std::vector<Point> quadratics(math::Promote<T> tolerance) const;

 private:
  std::vector<Point> subdivide(unsigned int level) const;
  std::pair<Conic, Conic> chop() const;

 public:
  union {
    std::array<Point, 3> points;
    struct {
      Point a;
      Point b;
      Point c;
    };
  };
  T weight;
};

// Comparison
template <class T, class U>
bool operator==(const Conic2<T>& lhs, const Conic2<U>& rhs);
template <class T, class U>
bool operator!=(const Conic2<T>& lhs, const Conic2<U>& rhs);

using Conic2i = Conic2<int>;
using Conic2f = Conic2<float>;
using Conic2d = Conic2<double>;

#pragma mark -

template <class T>
inline Conic<T, 2>::Conic() : a(), b(), c(), weight() {}

template <class T>
inline Conic<T, 2>::Conic(const Point& a,
                          const Point& b,
                          const Point& c,
                          math::Promote<T> weight)
    : points{{a, b, c}},
      weight(weight) {}

#pragma mark Comparison

template <class T, class U>
inline bool operator==(const Conic2<T>& lhs, const Conic2<U>& rhs) {
  return lhs.points == rhs.points && lhs.weight == rhs.weight;
}

template <class T, class U>
inline bool operator!=(const Conic2<T>& lhs, const Conic2<U>& rhs) {
  return !(lhs == rhs);
}

#pragma mark Subdivision

template <class T>
inline std::vector<Vec2<T>> Conic<T, 2>::quadratics() const {
  return subdivide(1);
}

template <class T>
inline std::vector<Vec2<T>> Conic<T, 2>::quadratics(
    math::Promote<T> tolerance) const {
  unsigned int subdivision{};
  if (tolerance >= 0) {
    static const unsigned int max_subdivision = 5;
    const auto k = (weight - 1) / (4 * (weight + 1));
    const auto x = k * (a.x - 2 * b.x + c.x);
    const auto y = k * (a.y - 2 * b.y + c.y);
    auto error = std::sqrt(x * x + y * y);
    for (; subdivision < max_subdivision && error > tolerance; ++subdivision) {
      error *= 0.25;
    }
  }
  return subdivide(subdivision);
}

template <class T>
inline std::vector<Vec2<T>> Conic<T, 2>::subdivide(unsigned int level) const {
  if (level) {
    const auto pair = chop();
    const auto first = pair.first.subdivide(--level);
    const auto second = pair.second.subdivide(level);
    std::vector<Vec2<T>> result;
    std::move(std::begin(first), std::end(first), std::back_inserter(result));
    std::move(std::begin(second), std::end(second), std::back_inserter(result));
    return std::move(result);
  } else {
    return std::vector<Vec2<T>>{b, c};
  }
}

template <class T>
inline std::pair<Conic2<T>, Conic2<T>> Conic<T, 2>::chop() const {
  const auto scale = 1 / (1 + this->weight);
  const auto weight = std::sqrt((1 + this->weight) / 2);
  const auto weighted = this->weight * b;
  const auto middle = (a + weighted + weighted + c) * scale / 2;
  return std::make_pair(Conic(a, (a + weighted) * scale, middle, weight),
                        Conic(middle, (weighted + c) * scale, c, weight));
}

}  // namespace graphics

namespace gfx = graphics;

using graphics::Conic;
using graphics::Conic2;
using graphics::Conic2i;
using graphics::Conic2f;
using graphics::Conic2d;

}  // namespace shotamatsuda

#endif  // SHOTA_GRAPHICS_CONIC2_H_
