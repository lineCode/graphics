//
//  shotamatsuda/graphics/path2.h
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
#ifndef SHOTA_GRAPHICS_PATH2_H_
#define SHOTA_GRAPHICS_PATH2_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <type_traits>
#include <utility>

#include "shotamatsuda/graphics/command.h"
#include "shotamatsuda/graphics/conic.h"
#include "shotamatsuda/graphics/path_direction.h"
#include "shotamatsuda/math/constants.h"
#include "shotamatsuda/math/promotion.h"
#include "shotamatsuda/math/rectangle.h"
#include "shotamatsuda/math/roots.h"
#include "shotamatsuda/math/vector.h"

namespace shotamatsuda {
namespace graphics {

extern void *enabler;

template <class T, int D>
class Path;

template <class T>
using Path2 = Path<T, 2>;

template <class T>
class Path<T, 2> final {
 public:
  using Type = T;
  using Command = Command2<T>;
  using Iterator = typename std::list<Command2<T>>::iterator;
  using ConstIterator = typename std::list<Command2<T>>::const_iterator;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
  static constexpr const int dimensions = 2;

 private:
  // Provided for reverse()
  struct Holder {
    explicit Holder(const Vec2<T>& vector) : vector(vector) {}
    explicit Holder(const math::Promote<T>& scalar) : scalar(scalar) {}
    operator const Vec2<T>&() const { return vector; }
    operator const math::Promote<T>&() const { return scalar; }
    union {
      Vec2<T> vector;
      math::Promote<T> scalar;
    };
  };

 public:
  Path() = default;
  explicit Path(const std::list<Command2<T>>& commands);

  // Copy semantics
  Path(const Path&) = default;
  Path& operator=(const Path&) = default;

  // Mutators
  void set(const std::list<Command2<T>>& commands);
  void reset();

  // Attributes
  bool empty() const { return commands_.empty(); }
  bool closed() const;
  std::size_t size() const { return commands_.size(); }
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

  // Commands
  const std::list<Command2<T>>& commands() const { return commands_; }
  std::list<Command2<T>>& commands() { return commands_; }

  // Direction
  PathDirection direction() const;
  Path& reverse();
  Path reversed() const;

  // Conversion
  bool convertQuadraticsToCubics();
  bool convertConicsToQuadratics();
  bool convertConicsToQuadratics(math::Promote<T> tolerance);
  bool removeDuplicates(math::Promote<T> threshold);

  // Element access
  Command2<T>& operator[](int index) { return at(index); }
  const Command2<T>& operator[](int index) const { return at(index); }
  Command2<T>& at(int index);
  const Command2<T>& at(int index) const;
  Command2<T>& front() { return commands_.front(); }
  const Command2<T>& front() const { return commands_.front(); }
  Command2<T>& back() { return commands_.back(); }
  const Command2<T>& back() const { return commands_.back(); }

  // Iterator
  Iterator begin() { return std::begin(commands_); }
  ConstIterator begin() const { return std::begin(commands_); }
  Iterator end() { return std::end(commands_); }
  ConstIterator end() const { return std::end(commands_); }
  ReverseIterator rbegin() { return ReverseIterator(begin()); }
  ConstReverseIterator rbegin() const { return ConstReverseIterator(begin()); }
  ReverseIterator rend() { return ReverseIterator(end()); }
  ConstReverseIterator rend() const { return ConstReverseIterator(end()); }

 private:
  // Bounding box
  template <class U = math::Promote<T>>
  Rect2<U> calculateApproximateBounds() const;
  template <class U = math::Promote<T>>
  Rect2<U> calculatePreciseBounds() const;
  template <class OutputIterator>
  unsigned int findQuadraticExtrema(const Vec2<T>& p0,
                                    const Vec2<T>& p1,
                                    const Vec2<T>& p2,
                                    OutputIterator result) const;
  template <class OutputIterator>
  unsigned int findCubicExtrema(const Vec2<T>& p0,
                                const Vec2<T>& p1,
                                const Vec2<T>& p2,
                                const Vec2<T>& p3,
                                OutputIterator result) const;
  template <class U = math::Promote<T>>
  Vec2<U> evaluateQuadraticAt(const Vec2<T>& p0,
                              const Vec2<T>& p1,
                              const Vec2<T>& p2,
                              U t) const;
  template <class U = math::Promote<T>>
  Vec2<U> evaluateCubicAt(const Vec2<T>& p0,
                          const Vec2<T>& p1,
                          const Vec2<T>& p2,
                          const Vec2<T>& p3,
                          U t) const;

  // Conversion
  template <
    class Method, class... Args,
    std::enable_if_t<std::is_member_pointer<Method>::value> *& = enabler
  >
  bool convertConicsToQuadratics(Method method, Args&&... args);

 private:
  std::list<Command2<T>> commands_;
};

// Comparison
template <class T, class U>
bool operator==(const Path2<T>& lhs, const Path2<U>& rhs);
template <class T, class U>
bool operator!=(const Path2<T>& lhs, const Path2<U>& rhs);

using Path2i = Path2<int>;
using Path2f = Path2<float>;
using Path2d = Path2<double>;

#pragma mark -

template <class T>
inline Path<T, 2>::Path(const std::list<Command2<T>>& commands)
    : commands_(commands) {}

#pragma mark Mutators

template <class T>
inline void Path<T, 2>::set(const std::list<Command2<T>>& commands) {
  commands_ = commands;
}

template <class T>
inline void Path<T, 2>::reset() {
  commands_.clear();
}

#pragma mark Comparison

template <class T, class U>
inline bool operator==(const Path2<T>& lhs, const Path2<U>& rhs) {
  return lhs.commands() == rhs.commands();
}

template <class T, class U>
inline bool operator!=(const Path2<T>& lhs, const Path2<U>& rhs) {
  return !(lhs == rhs);
}

#pragma mark Attributes

template <class T>
inline bool Path<T, 2>::closed() const {
  if (commands_.size() < 3) {
    return false;
  }
  if (commands_.back().type() == CommandType::CLOSE) {
    return true;
  }
  if (commands_.back().point() == commands_.front().point()) {
    return true;
  }
  return false;
}

template <class T>
inline Rect2<math::Promote<T>> Path<T, 2>::bounds(bool precise) const {
  if (precise) {
    return calculatePreciseBounds();
  }
  return calculateApproximateBounds();
}

template <class T>
template <class U>
inline Rect2<U> Path<T, 2>::calculateApproximateBounds() const {
  T min_x = std::numeric_limits<T>::max();
  T min_y = std::numeric_limits<T>::max();
  T max_x = std::numeric_limits<T>::lowest();
  T max_y = std::numeric_limits<T>::lowest();
  for (const auto& command : commands_) {
    switch (command.type()) {
      case CommandType::CUBIC:
        if (command.control2().x < min_x) min_x = command.control2().x;
        if (command.control2().y < min_y) min_y = command.control2().y;
        if (command.control2().x > max_x) max_x = command.control2().x;
        if (command.control2().y > max_y) max_y = command.control2().y;
        // Pass through
      case CommandType::CONIC:
      case CommandType::QUADRATIC:
        if (command.control().x < min_x) min_x = command.control().x;
        if (command.control().y < min_y) min_y = command.control().y;
        if (command.control().x > max_x) max_x = command.control().x;
        if (command.control().y > max_y) max_y = command.control().y;
        // Pass through
      case CommandType::LINE:
      case CommandType::MOVE:
        if (command.point().x < min_x) min_x = command.point().x;
        if (command.point().y < min_y) min_y = command.point().y;
        if (command.point().x > max_x) max_x = command.point().x;
        if (command.point().y > max_y) max_y = command.point().y;
        break;
      case CommandType::CLOSE:
        break;
      default:
        assert(false);
        break;
    }
  }
  if (min_x == std::numeric_limits<T>::max()) min_x = T();
  if (min_y == std::numeric_limits<T>::max()) min_y = T();
  if (max_x == std::numeric_limits<T>::lowest()) max_x = T();
  if (max_y == std::numeric_limits<T>::lowest()) max_y = T();
  return Rect2<U>(Vec2<U>(min_x, min_y), Vec2<U>(max_x, max_y));
}

template <class T>
template <class U>
inline Rect2<U> Path<T, 2>::calculatePreciseBounds() const {
  if (commands_.empty()) {
    return Rect2<U>();
  }
  Rect2<U> result(commands_.front().point());
  Vec2<U> extrema[4];
  for (auto current = std::begin(commands_), previous = current++;
       current != std::end(commands_); ++current, ++previous) {
    switch (current->type()) {
      case CommandType::CUBIC: {
        result.include(current->point());
        const auto count = findCubicExtrema(
            previous->point(),
            current->control1(),
            current->control2(),
            current->point(),
            extrema);
        for (int i{}; i < count; ++i) {
          result.include(extrema[i]);
        }
        break;
      }
      case CommandType::CONIC: {
        // TODO(shotamatsuda): Calculate conic extrema
        result.include(current->control());
        result.include(current->point());
        break;
      }
      case CommandType::QUADRATIC: {
        result.include(current->point());
        const auto count = findQuadraticExtrema(
            previous->point(),
            current->control(),
            current->point(),
            extrema);
        for (int i{}; i < count; ++i) {
          result.include(extrema[i]);
        }
        break;
      }
      case CommandType::LINE:
        result.include(current->point());
        break;
      case CommandType::MOVE:
      case CommandType::CLOSE:
        break;
      default:
        assert(false);
        break;
    }
  }
  return std::move(result);
}

template <class T>
template <class OutputIterator>
inline unsigned int Path<T, 2>::findQuadraticExtrema(
    const Vec2<T>& p0,
    const Vec2<T>& p1,
    const Vec2<T>& p2,
    OutputIterator result) const {
  const auto a = p0 - 2 * p1 + p2;
  const auto b = p1 - p0;
  math::Promote<T> x_root;
  math::Promote<T> y_root;
  const auto x_count = math::solveLinear(a.x, b.x, &x_root);
  const auto y_count = math::solveLinear(a.y, b.y, &y_root);
  unsigned int count{};
  if(x_count > 0 && 0 < x_root && x_root < 1) {
    *result++ = evaluateQuadraticAt(p0, p1, p2, x_root);
    ++count;
  }
  if(y_count > 0 && 0 < y_root && y_root < 1) {
    *result = evaluateQuadraticAt(p0, p1, p2, y_root);
    ++count;
  }
  return count;
}

template <class T>
template <class OutputIterator>
inline unsigned int Path<T, 2>::findCubicExtrema(
    const Vec2<T>& p0,
    const Vec2<T>& p1,
    const Vec2<T>& p2,
    const Vec2<T>& p3,
    OutputIterator result) const {
  const auto a = 3 * p3 - 9 * p2 + 9 * p1 - 3 * p0;
  const auto b = 6 * p0 - 12 * p1 + 6 * p2;
  const auto c = 3 * p1 - 3 * p0;
  math::Promote<T> x_roots[2];
  math::Promote<T> y_roots[2];
  const auto x_count = math::solveQuadratic(a.x, b.x, c.x, x_roots);
  const auto y_count = math::solveQuadratic(a.y, b.y, c.y, y_roots);
  unsigned int count{};
  if(x_count > 0 && 0 < x_roots[0] && x_roots[0] < 1) {
    *result++ = evaluateCubicAt(p0, p1, p2, p3, x_roots[0]);
    ++count;
  }
  if(y_count > 0 && 0 < y_roots[0] && y_roots[0] < 1) {
    *result++ = evaluateCubicAt(p0, p1, p2, p3, y_roots[0]);
    ++count;
  }
  if(x_count > 1 && 0 < x_roots[1] && x_roots[1] < 1) {
    *result++ = evaluateCubicAt(p0, p1, p2, p3, x_roots[1]);
    ++count;
  }
  if(y_count > 1 && 0 < y_roots[1] && y_roots[1] < 1) {
    *result = evaluateCubicAt(p0, p1, p2, p3, y_roots[1]);
    ++count;
  }
  return count;
}

template <class T>
template <class U>
inline Vec2<U> Path<T, 2>::evaluateQuadraticAt(const Vec2<T>& p0,
                                               const Vec2<T>& p1,
                                               const Vec2<T>& p2,
                                               U t) const {
  const auto a = (1 - t) * (1 - t);
  const auto b = 2 * (1 - t) * t;
  const auto c = t * t;
  return a * p0 + b * p1 + c * p2;
}

template <class T>
template <class U>
inline Vec2<U> Path<T, 2>::evaluateCubicAt(const Vec2<T>& p0,
                                           const Vec2<T>& p1,
                                           const Vec2<T>& p2,
                                           const Vec2<T>& p3,
                                           U t) const {
  const auto a = (1 - t) * (1 - t) * (1 - t);
  const auto b = 3 * (1 - t) * (1 - t) * t;
  const auto c = 3 * (1 - t) * t * t;
  const auto d = t * t * t;
  return a * p0 + b * p1 + c * p2 + d * p3;
}

#pragma mark Adding commands

template <class T>
inline void Path<T, 2>::close() {
  if (commands_.back().type() != CommandType::CLOSE) {
    commands_.emplace_back(CommandType::CLOSE);
  }
}

template <class T>
inline void Path<T, 2>::moveTo(T x, T y) {
  moveTo(Vec2<T>(x, y));
}

template <class T>
inline void Path<T, 2>::moveTo(const Vec2<T>& point) {
  commands_.clear();
  commands_.emplace_back(CommandType::MOVE, point);
}

template <class T>
inline void Path<T, 2>::lineTo(T x, T y) {
  lineTo(Vec2<T>(x, y));
}

template <class T>
inline void Path<T, 2>::lineTo(const Vec2<T>& point) {
  if (commands_.empty()) {
    moveTo(point);
  } else {
    if (commands_.back().type() == CommandType::CLOSE) {
      commands_.pop_back();
    }
    commands_.emplace_back(CommandType::LINE, point);
    if (point == commands_.front().point()) {
      close();
    }
  }
}

template <class T>
inline void Path<T, 2>::quadraticTo(T cx, T cy, T x, T y) {
  quadraticTo(Vec2<T>(cx, cy), Vec2<T>(x, y));
}

template <class T>
inline void Path<T, 2>::quadraticTo(const Vec2<T>& control,
                                    const Vec2<T>& point) {
  if (commands_.empty()) {
    moveTo(point);
  } else {
    if (commands_.back().type() == CommandType::CLOSE) {
      commands_.pop_back();
    }
    commands_.emplace_back(CommandType::QUADRATIC, control, point);
    if (point == commands_.front().point()) {
      close();
    }
  }
}

template <class T>
inline void Path<T, 2>::conicTo(T cx, T cy, T x, T y, math::Promote<T> weight) {
  conicTo(Vec2<T>(cx, cy), Vec2<T>(x, y), weight);
}

template <class T>
inline void Path<T, 2>::conicTo(const Vec2<T>& control,
                                const Vec2<T>& point,
                                math::Promote<T> weight) {
  if (commands_.empty()) {
    moveTo(point);
  } else {
    if (commands_.back().type() == CommandType::CLOSE) {
      commands_.pop_back();
    }
    commands_.emplace_back(CommandType::CONIC, control, point, weight);
    if (point == commands_.front().point()) {
      close();
    }
  }
}

template <class T>
inline void Path<T, 2>::cubicTo(T cx1, T cy1, T cx2, T cy2, T x, T y) {
  cubicTo(Vec2<T>(cx1, cy1), Vec2<T>(cx2, cy2), Vec2<T>(x, y));
}

template <class T>
inline void Path<T, 2>::cubicTo(const Vec2<T>& control1,
                                const Vec2<T>& control2,
                                const Vec2<T>& point) {
  if (commands_.empty()) {
    moveTo(point);
  } else {
    if (commands_.back().type() == CommandType::CLOSE) {
      commands_.pop_back();
    }
    commands_.emplace_back(CommandType::CUBIC, control1, control2, point);
    if (point == commands_.front().point()) {
      close();
    }
  }
}

#pragma mark Direction

template <class T>
inline PathDirection Path<T, 2>::direction() const {
  if (commands_.size() < 3 || !closed()) {
    return PathDirection::UNDEFINED;
  }
  T sum{};
  for (auto next = std::begin(commands_), current = next++;
       current != std::end(commands_); ++current, ++next) {
    if (next != std::end(commands_)) {
      sum += current->point().cross(next->point());
    } else {
      sum += current->point().cross(commands_.front().point());
    }
  }
  if (!sum) {
    return PathDirection::UNDEFINED;
  }
  return sum < 0 ? PathDirection::COUNTER_CLOCKWISE : PathDirection::CLOCKWISE;
}

template <class T>
inline Path2<T>& Path<T, 2>::reverse() {
  if (commands_.empty()) {
    return *this;
  }
  std::list<Holder> holders;
  for (auto& command : commands_) {
    switch (command.type()) {
      case CommandType::MOVE:
      case CommandType::LINE:
        holders.emplace_back(command.point());
        break;
      case CommandType::QUADRATIC:
        holders.emplace_back(command.control());
        holders.emplace_back(command.point());
        break;
      case CommandType::CONIC:
        holders.emplace_back(command.control());
        holders.emplace_back(command.weight());
        holders.emplace_back(command.point());
        break;
      case CommandType::CUBIC:
        holders.emplace_back(command.control1());
        holders.emplace_back(command.control2());
        holders.emplace_back(command.point());
        break;
      case CommandType::CLOSE:
        break;
      default:
        assert(false);
        break;
    }
  }
  if (commands_.back().type() == CommandType::CLOSE) {
    std::reverse(std::next(std::begin(commands_)),
                 std::prev(std::end(commands_)));
  } else {
    std::reverse(std::next(std::begin(commands_)),
                 std::end(commands_));
  }
  std::reverse(std::begin(holders), std::end(holders));
  auto itr = std::begin(holders);
  for (auto& command : commands_) {
    switch (command.type()) {
      case CommandType::MOVE:
      case CommandType::LINE:
        command.point() = *(itr++);
        break;
      case CommandType::QUADRATIC:
        command.control() = *(itr++);
        command.point() = *(itr++);
        break;
      case CommandType::CONIC:
        command.weight() = *(itr++);
        command.control() = *(itr++);
        command.point() = *(itr++);
        break;
      case CommandType::CUBIC:
        command.control1() = *(itr++);
        command.control2() = *(itr++);
        command.point() = *(itr++);
        break;
      case CommandType::CLOSE:
        break;
      default:
        assert(false);
        break;
    }
  }
  assert(itr == std::end(holders));
  return *this;
}

template <class T>
inline Path2<T> Path<T, 2>::reversed() const {
  return std::move(Path(*this).reverse());
}

#pragma mark Conversion

template <class T>
inline bool Path<T, 2>::convertQuadraticsToCubics() {
  bool changed{};
  for (auto current = std::begin(commands_), previous = current++;
       current != std::end(commands_);) {
    if (current->type() != CommandType::QUADRATIC) {
      previous = current++;
      continue;
    }
    const auto a = previous->point();
    const auto b = current->control();
    const auto c = current->point();
    current->type() = CommandType::CUBIC;
    current->control1() = a + (b - a) * 2 / 3;
    current->control2() = c + (b - c) * 2 / 3;
    changed = true;
  }
  return changed;
}

template <class T>
inline bool Path<T, 2>::convertConicsToQuadratics() {
  using F = std::vector<Vec2<T>> (Conic2<T>::*)(void) const;
  return convertConicsToQuadratics(static_cast<F>(&Conic2<T>::quadratics));
}

template <class T>
inline bool Path<T, 2>::convertConicsToQuadratics(math::Promote<T> tolerance) {
  using F = std::vector<Vec2<T>> (Conic2<T>::*)(math::Promote<T>) const;
  return convertConicsToQuadratics(static_cast<F>(&Conic2<T>::quadratics),
                                   tolerance);
}

template <class T>
template <
  class Method, class... Args,
  std::enable_if_t<std::is_member_pointer<Method>::value> *&
>
inline bool Path<T, 2>::convertConicsToQuadratics(Method method,
                                                  Args&&... args) {
  assert(method);
  bool changed{};
  for (auto current = std::begin(commands_), previous = current++;
       current != std::end(commands_);) {
    if (current->type() != CommandType::CONIC) {
      previous = current++;
      continue;
    }
    const Conic2<T> conic(previous->point(),
                          current->control(),
                          current->point(),
                          current->weight());
    const auto points = (conic.*method)(args...);
    current = commands_.erase(current);
    for (auto itr = std::begin(points); itr != std::end(points); ++itr) {
      current = commands_.emplace(current, CommandType::QUADRATIC,
                                  *itr, *(++itr));
      previous = current++;
    }
    changed = true;
  }
  return changed;
}

template <class T>
inline bool Path<T, 2>::removeDuplicates(math::Promote<T> threshold) {
  bool changed{};
  std::list<std::list<Iterator>> duplicates;
  bool duplicated{};
  for (auto current = std::begin(commands_), previous = current++;
       current != std::end(commands_); ++current, ++previous) {
    if (current->point().equals(previous->point(), threshold)) {
      if (!duplicated) {
        duplicates.emplace_back();
        duplicates.back().emplace_back(previous);
      }
      duplicates.back().emplace_back(current);
      duplicated = true;
      changed = true;
    } else {
      duplicated = false;
    }
  }
  for (auto& commands : duplicates) {
    assert(commands.size() > 1);
    for (auto itr = std::next(std::begin(commands));
         itr != std::end(commands); ++itr) {
      commands_.erase(*itr);
    }
    const auto& front = commands.front()->point();
    const auto& back = commands.back()->point();
    commands.front()->point() = (front + back) / 2;
  }
  return changed;
}

#pragma mark Element access

template <class T>
inline Command2<T>& Path<T, 2>::at(int index) {
  auto itr = std::begin(commands_);
  std::advance(itr, index);
  return *itr;
}

template <class T>
inline const Command2<T>& Path<T, 2>::at(int index) const {
  auto itr = std::begin(commands_);
  std::advance(itr, index);
  return *itr;
}

}  // namespace graphics

namespace gfx = graphics;

using graphics::Path;
using graphics::Path2;
using graphics::Path2i;
using graphics::Path2f;
using graphics::Path2d;

}  // namespace shotamatsuda

#endif  // SHOTA_GRAPHICS_PATH2_H_
