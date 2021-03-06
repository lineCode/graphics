//
//  shotamatsuda/graphics/shape2.h
//
//  The MIT License
//
//  Copyright (C) 2013-2017 Shota Matsuda
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
#ifndef SHOTA_GRAPHICS_SHAPE2_H_
#define SHOTA_GRAPHICS_SHAPE2_H_

#include <cstddef>
#include <list>
#include <iterator>

#include "shotamatsuda/algorithm/leaf_iterator_iterator.h"
#include "shotamatsuda/graphics/path.h"
#include "shotamatsuda/math/promotion.h"
#include "shotamatsuda/math/rectangle.h"
#include "shotamatsuda/math/vector.h"

namespace shotamatsuda {
namespace graphics {

template <class T, int D>
class Shape;

template <class T>
using Shape2 = Shape<T, 2>;

template <class T>
class Shape<T, 2> final {
 public:
  using Type = T;
  using Iterator = LeafIteratorIterator<
      typename std::list<Path2<T>>::iterator,
      typename Path2<T>::Iterator>;
  using ConstIterator = LeafIteratorIterator<
      typename std::list<Path2<T>>::const_iterator,
      typename Path2<T>::ConstIterator>;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
  static constexpr const int dimensions = 2;

 public:
  Shape() = default;
  explicit Shape(const Path2<T>& path);
  explicit Shape(const std::list<Path2<T>>& paths);

  // Copy semantics
  Shape(const Shape&) = default;
  Shape& operator=(const Shape&) = default;

  // Mutators
  void set(const std::list<Path2<T>>& paths);
  void reset();

  // Attributes
  bool empty() const { return paths_.empty(); }
  std::size_t size() const { return paths_.size(); }
  Rect2<math::Promote<T>> bounds(bool precise = false) const;

  // Adding commands
  void close();
  void moveTo(T x, T y);
  void moveTo(const Vec2<T>& point);
  void lineTo(T x, T y);
  void lineTo(const Vec2<T>& point);
  void quadraticTo(T cx, T cy, T x, T y);
  void quadraticTo(const Vec2<T>& control, const Vec2<T>& point);
  void conicTo(T cx, T cy, T x, T y, math::Promote<T> weight);
  void conicTo(const Vec2<T>& control,
               const Vec2<T>& point,
               math::Promote<T> weight);
  void cubicTo(T cx1, T cy1, T cx2, T cy2, T x, T y);
  void cubicTo(const Vec2<T>& control1,
               const Vec2<T>& control2,
               const Vec2<T>& point);

  // Paths
  const std::list<Path2<T>>& paths() const { return paths_; }
  std::list<Path2<T>>& paths() { return paths_; }

  // Conversion
  bool convertQuadraticsToCubics();
  bool convertConicsToQuadratics();
  bool convertConicsToQuadratics(math::Promote<T> tolerance);
  bool removeDuplicates(math::Promote<T> threshold);

  // Element access
  Path2<T>& operator[](int index) { return at(index); }
  const Path2<T>& operator[](int index) const { return at(index); }
  Path2<T>& at(int index);
  const Path2<T>& at(int index) const;
  Path2<T>& front() { return paths_.front(); }
  const Path2<T>& front() const { return paths_.front(); }
  Path2<T>& back() { return paths_.back(); }
  const Path2<T>& back() const { return paths_.back(); }

  // Iterator
  Iterator begin();
  ConstIterator begin() const;
  Iterator end();
  ConstIterator end() const;
  ReverseIterator rbegin() { return ReverseIterator(begin()); }
  ConstReverseIterator rbegin() const { return ConstReverseIterator(begin()); }
  ReverseIterator rend() { return ReverseIterator(end()); }
  ConstReverseIterator rend() const { return ConstReverseIterator(end()); }

 private:
  std::list<Path2<T>> paths_;
};

// Comparison
template <class T, class U>
bool operator==(const Shape2<T>& lhs, const Shape2<U>& rhs);
template <class T, class U>
bool operator!=(const Shape2<T>& lhs, const Shape2<U>& rhs);

using Shape2i = Shape2<int>;
using Shape2f = Shape2<float>;
using Shape2d = Shape2<double>;

#pragma mark -

template <class T>
inline Shape<T, 2>::Shape(const Path2<T>& path) : paths_{path} {}

template <class T>
inline Shape<T, 2>::Shape(const std::list<Path2<T>>& paths) : paths_(paths) {}

#pragma mark Mutators

template <class T>
inline void Shape<T, 2>::set(const std::list<Path2<T>>& paths) {
  paths_ = paths;
}

template <class T>
inline void Shape<T, 2>::reset() {
  paths_.clear();
}

#pragma mark Comparison

template <class T, class U>
inline bool operator==(const Shape2<T>& lhs, const Shape2<U>& rhs) {
  return lhs.paths() == rhs.paths();
}

template <class T, class U>
inline bool operator!=(const Shape2<T>& lhs, const Shape2<U>& rhs) {
  return !(lhs == rhs);
}

#pragma mark Attributes

template <class T>
inline Rect2<math::Promote<T>> Shape<T, 2>::bounds(bool precise) const {
  if (paths_.empty()) {
    return Rect2<math::Promote<T>>();
  }
  Rect2<math::Promote<T>> result;
  for (const auto& path : paths_) {
    const auto bounds = path.bounds(precise);
    if (bounds.empty()) {
      continue;
    }
    if (result.empty()) {
      result = std::move(bounds);
    } else {
      result.include(std::move(bounds));
    }
  }
  return std::move(result);
}

#pragma mark Adding commands

template <class T>
inline void Shape<T, 2>::close() {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().close();
}

template <class T>
inline void Shape<T, 2>::moveTo(T x, T y) {
  paths_.emplace_back();
  paths_.back().moveTo(x, y);
}

template <class T>
inline void Shape<T, 2>::moveTo(const Vec2<T>& point) {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().moveTo(point);
}

template <class T>
inline void Shape<T, 2>::lineTo(T x, T y) {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().lineTo(x, y);
}

template <class T>
inline void Shape<T, 2>::lineTo(const Vec2<T>& point) {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().lineTo(point);
}

template <class T>
inline void Shape<T, 2>::quadraticTo(T cx, T cy, T x, T y) {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().quadraticTo(cx, cy, x, y);
}

template <class T>
inline void Shape<T, 2>::quadraticTo(const Vec2<T>& control,
                                     const Vec2<T>& point) {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().quadraticTo(control, point);
}

template <class T>
inline void Shape<T, 2>::conicTo(T cx, T cy, T x, T y,
                                 math::Promote<T> weight) {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().conicTo(cx, cy, x, y, weight);
}

template <class T>
inline void Shape<T, 2>::conicTo(const Vec2<T>& control,
                                 const Vec2<T>& point,
                                 math::Promote<T> weight) {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().conicTo(control, point, weight);
}

template <class T>
inline void Shape<T, 2>::cubicTo(T cx1, T cy1, T cx2, T cy2, T x, T y) {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().cubicTo(cx1, cy1, cx2, cy2, x, y);
}

template <class T>
inline void Shape<T, 2>::cubicTo(const Vec2<T>& control1,
                                 const Vec2<T>& control2,
                                 const Vec2<T>& point) {
  if (paths_.empty()) {
    paths_.emplace_back();
  }
  paths_.back().cubicTo(control1, control2, point);
}

#pragma mark Conversion

template <class T>
inline bool Shape<T, 2>::convertQuadraticsToCubics() {
  bool changed{};
  for (auto& path : paths_) {
    if (path.convertQuadraticsToCubics()) {
      changed = true;
    }
  }
  return changed;
}

template <class T>
inline bool Shape<T, 2>::convertConicsToQuadratics() {
  bool changed{};
  for (auto& path : paths_) {
    if (path.convertConicsToQuadratics()) {
      changed = true;
    }
  }
  return changed;
}

template <class T>
inline bool Shape<T, 2>::convertConicsToQuadratics(math::Promote<T> tolerance) {
  bool changed{};
  for (auto& path : paths_) {
    if (path.convertConicsToQuadratics(tolerance)) {
      changed = true;
    }
  }
  return changed;
}

template <class T>
inline bool Shape<T, 2>::removeDuplicates(math::Promote<T> threshold) {
  bool changed{};
  for (auto& path : paths_) {
    if (path.removeDuplicates(threshold)) {
      changed = true;
    }
  }
  return changed;
}

#pragma mark Element access

template <class T>
inline Path2<T>& Shape<T, 2>::at(int index) {
  auto itr = std::begin(paths_);
  std::advance(itr, index);
  return *itr;
}

template <class T>
inline const Path2<T>& Shape<T, 2>::at(int index) const {
  auto itr = std::begin(paths_);
  std::advance(itr, index);
  return *itr;
}

#pragma mark Iterator

template <class T>
inline typename Shape<T, 2>::Iterator Shape<T, 2>::begin() {
  return Iterator(std::begin(paths_), std::end(paths_));
}

template <class T>
inline typename Shape<T, 2>::ConstIterator Shape<T, 2>::begin() const {
  return ConstIterator(std::begin(paths_), std::end(paths_));
}

template <class T>
inline typename Shape<T, 2>::Iterator Shape<T, 2>::end() {
  return Iterator(std::end(paths_), std::end(paths_));
}

template <class T>
inline typename Shape<T, 2>::ConstIterator Shape<T, 2>::end() const {
  return ConstIterator(std::end(paths_), std::end(paths_));
}

}  // namespace graphics

namespace gfx = graphics;

using graphics::Shape;
using graphics::Shape2;
using graphics::Shape2i;
using graphics::Shape2f;
using graphics::Shape2d;

}  // namespace shotamatsuda

#endif  // SHOTA_GRAPHICS_SHAPE2_H_
