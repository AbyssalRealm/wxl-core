// Game-UI root singleton and the render-enable flag that shows / hides the whole interface.
// Copyright (C) 2026 WarcraftXL
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>
#include <cstddef>

// INTERNAL to the core. The interface-layer root singleton and its render-enable flag. Clearing the
// flag skips the entire UI render+event pass while the 3D world keeps drawing. Modules use wxl::game.
namespace wxl::offsets::engine::ui
{
    // -> the interface root object (non-null after login).
    constexpr uintptr_t kUiRootPtr = 0x00B499A8;
    // int field on the root: 1 = UI shown, 0 = UI hidden. Initialized to 1.
    constexpr size_t    kUiEnabledFlag = 0x124C;
}
