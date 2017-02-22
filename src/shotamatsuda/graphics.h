//
//  shotamatsuda/graphics.h
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
#ifndef SHOTA_GRAPHICS_H_
#define SHOTA_GRAPHICS_H_

namespace shotamatsuda {
namespace graphics {

extern const double version_number;
extern const unsigned char version_string[];

}  // namespace graphics
}  // namespace shotamatsuda

#include "shotamatsuda/graphics/channel.h"
#include "shotamatsuda/graphics/color.h"
#include "shotamatsuda/graphics/depth.h"
#include "shotamatsuda/graphics/conic.h"
#include "shotamatsuda/graphics/command.h"
#include "shotamatsuda/graphics/command_type.h"
#include "shotamatsuda/graphics/path.h"
#include "shotamatsuda/graphics/path_direction.h"
#include "shotamatsuda/graphics/shape.h"

#endif  // SHOTA_GRAPHICS_H_
