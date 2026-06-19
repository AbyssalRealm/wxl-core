// AssetHandler base + the handler registry: where modules plug their per-format recipes in.
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
#include <string_view>
#include <vector>

#include "services/asset/Asset.hpp"
#include "services/asset/Resolver.hpp"

namespace wxl::asset
{
    // Base priorities are the registry's cross-format policy (e.g. ADT outranks WMO outranks M2 when a
    // name is ambiguous). A handler returns its own priority() so a module can slot above/below these.
    namespace priority
    {
        constexpr int kAdt = 300;
        constexpr int kWmo = 200;
        constexpr int kM2  = 100;
    }

    class AssetHandler
    {
    public:
        virtual ~AssetHandler() = default;

        // Which format this handler serves; the registry buckets by it.
        virtual Format format() const = 0;

        // Selection order within a format. Higher wins. Default to the format base priority.
        virtual int priority() const = 0;

        // Claim by data presence (chunk/field probe), never by an external version flag. The first
        // claiming handler in priority order wins. Cheap: peek headers, do not transform here.
        virtual bool Claims(std::span<const uint8_t> source) const = 0;

        virtual bool Translate(std::span<const uint8_t> in, const ResolveCtx& rc, std::vector<uint8_t>& out) const = 0;

    protected:
        AssetHandler(); // auto-registers into the registry
    };

    // The registry. Modules never call Add directly; the AssetHandler ctor does.
    namespace registry
    {
        void Add(AssetHandler* handler);
        std::span<AssetHandler* const> Handlers();

        bool Dispatch(std::string_view name, std::span<const uint8_t> in, const ResolveCtx& rc, std::vector<uint8_t>& out);
    }
}
