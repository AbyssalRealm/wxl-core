// Game-binding catalog storage: the enumerable registry of curated client functions.
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

#include "game/Binding.hpp"

#include <cstring>
#include <vector>

namespace wxl::game
{
    // Function-local static: the catalog builds at startup with no static-init-order dependency.
    namespace
    {
        std::vector<BindingInfo>& Store()
        {
            static std::vector<BindingInfo> store;
            return store;
        }
    }

    void Register(const BindingInfo& info) { Store().push_back(info); }

    std::span<const BindingInfo> Catalog() { return Store(); }

    const BindingInfo* Find(const char* name)
    {
        for (const BindingInfo& b : Store())
            if (std::strcmp(b.name, name) == 0) return &b;
        return nullptr;
    }
}
