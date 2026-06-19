// Game binding pattern: typed native calls + an enumerable catalog of curated client functions.
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
#include <span>

// A binding exposes one client function as a typed call. The call itself is a plain
// function-pointer cast (zero overhead, no vtable, no std::function), so bindings are safe in any
// path. The catalog is a parallel, ENUMERABLE registry of {name, address, signature}.
namespace wxl::game
{
    // One catalog entry. Hand-named: a curated name is the whole point (Texture::UploadMip, not
    // FUN_006b0075). signature is human-readable, for display and the future scripting bridge.
    struct BindingInfo
    {
        const char* name;
        uintptr_t   address;
        const char* signature;
    };

    // Add an entry to the enumerable catalog (cold, at startup). Does not affect calling.
    void Register(const BindingInfo& info);
    std::span<const BindingInfo> Catalog();
    const BindingInfo* Find(const char* name);

    // Typed access to a client address. Use at a call site: Native<Fn>(addr)(args...).
    template <class Fn>
    inline Fn Native(uintptr_t address) { return reinterpret_cast<Fn>(address); }
}
