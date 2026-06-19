// FileDataID -> path resolution handed to the toolbox primitives.
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
#include <string>

namespace wxl::asset
{
    using PathResolver = bool (*)(void* user, uint32_t fileDataId, std::string& outPath);

    struct ResolveCtx
    {
        PathResolver resolve = nullptr; // may be null for formats that need no resolution
        void*        user    = nullptr; // opaque, passed back to resolve()
    };
}
