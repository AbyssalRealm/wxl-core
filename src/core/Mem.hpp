// Copyright (C) 2026 WraithEngine
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

#include <cstddef>
#include <cstdint>

// In-process live byte patching of the client image. Used by patcher/ for
// the version-gate patch and by features that nop/replace inline code.
namespace wraith::core::mem
{
    // Copy `len` bytes from `src` into `dst`, toggling page protection around the write.
    bool Patch(void* dst, const void* src, size_t len);

    // Write `len` copies of `value` at `dst` (e.g. fill with 0x90 NOP).
    bool Fill(void* dst, uint8_t value, size_t len);
}
