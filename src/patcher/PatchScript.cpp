// PatchScript registry.
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

#include "patcher/PatchScript.hpp"

#include <vector>

namespace
{
    // Function-local static: safe whatever the script ctor init order is.
    std::vector<wxl::patcher::PatchScript*>& List()
    {
        static std::vector<wxl::patcher::PatchScript*> v;
        return v;
    }
}

namespace wxl::patcher
{
    PatchScript::PatchScript() { registry::Add(this); }

    namespace registry
    {
        void Add(PatchScript* script) { List().push_back(script); }
        std::span<PatchScript* const> Scripts() { return List(); }
    }
}
