// world loading bindings: tick / load-gate, async-I/O queue pumps, and load-state globals.
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
#include "offsets/game/World.hpp"

// Curated world-loading bindings. A module writes wxl::game::world::AsyncPending() instead of
// casting an address. The wrappers are inline typed calls (zero overhead); RegisterCatalog() adds
// the names to the enumerable catalog for tooling and the future scripting bridge.
namespace wxl::game::world
{
    namespace woff = wxl::offsets::game::world;

    // Run one world tick plus the loading-screen synchronous drain.
    inline void Tick(int param)
    {
        Native<woff::World_TickFn>(woff::kTick)(param);
    }

    // Block pumping the async queues until no async file read is pending.
    inline void AsyncWaitAll()
    {
        Native<woff::World_AsyncWaitAllFn>(woff::kAsyncWaitAll)();
    }

    // True while any async file request still has outstanding work.
    inline bool AsyncPending()
    {
        return Native<woff::World_AsyncPendingFn>(woff::kAsyncPending)() != 0;
    }

    // Service the async queues one pump.
    inline void AsyncServiceQueues()
    {
        Native<woff::World_AsyncServiceQueuesFn>(woff::kAsyncServiceQueues)(0, 0);
    }

    // True while the blocking load-gate drain runs.
    inline bool LoadActive()
    {
        return *reinterpret_cast<uint32_t*>(woff::kLoadActive) != 0;
    }

    // Read the focus world position (center of the load box / player position) into out[0..2].
    inline void FocusPos(float out[3])
    {
        out[0] = *reinterpret_cast<float*>(woff::kFocusPosX);
        out[1] = *reinterpret_cast<float*>(woff::kFocusPosY);
        out[2] = *reinterpret_cast<float*>(woff::kFocusPosZ);
    }

    // Add the world-loading bindings to the enumerable catalog (cold, at startup).
    inline void RegisterCatalog()
    {
        Register({ "World::Tick",               woff::kTick,               "void(int param)" });
        Register({ "World::AsyncWaitAll",        woff::kAsyncWaitAll,        "void()" });
        Register({ "World::AsyncPending",        woff::kAsyncPending,        "bool()" });
        Register({ "World::AsyncServiceQueues",  woff::kAsyncServiceQueues,  "void()" });
    }
}
