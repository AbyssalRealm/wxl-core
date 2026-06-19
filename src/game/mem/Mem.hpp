// MEM game bindings: typed wrappers over the engine's heap allocator / free, curated by hand.
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

#include "game/Binding.hpp"
#include "offsets/engine/Mem.hpp"

// Curated MEM bindings. A module writes wxl::game::mem::Alloc(n) instead of casting an address.
// Use these to allocate buffers the engine will later free itself (and vice versa). The wrappers
// are inline typed calls (zero overhead); RegisterCatalog() adds the names to the enumerable
// catalog for tooling and the future scripting bridge.
namespace wxl::game::mem
{
    namespace off = wxl::offsets::engine::mem;

    // Allocate a block from the engine heap (size is rounded up internally).
    inline void* Alloc(uint32_t size, const char* file = "wxl", int line = 0, uint32_t flags = 0)
    {
        return Native<off::Mem_AllocFn>(off::kAlloc)(size, file, line, flags);
    }

    // Free a block obtained from the engine heap.
    inline void Free(void* ptr, const char* file = "wxl", int line = 0, uint32_t flags = 0)
    {
        Native<off::Mem_FreeFn>(off::kFree)(ptr, file, line, flags);
    }

    // Add the MEM bindings to the enumerable catalog (cold, at startup).
    inline void RegisterCatalog()
    {
        Register({ "Mem::Alloc", off::kAlloc, "void*(size, file, line, flags)" });
        Register({ "Mem::Free",  off::kFree,  "void(ptr, file, line, flags)" });
    }
}
