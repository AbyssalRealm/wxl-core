// IO game bindings: typed wrappers over the client's archive file-I/O primitives, curated by hand.
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
#include "offsets/engine/Io.hpp"

// Curated IO bindings. A module writes wxl::game::io::FileRead(...) instead of casting an address.
// The wrappers are inline typed calls (zero overhead); RegisterCatalog() adds the names to the
// enumerable catalog for tooling and the future scripting bridge.
namespace wxl::game::io
{
    namespace off = wxl::offsets::engine::io;

    // Open a file by name, fills outHandle; returns nonzero on success.
    inline int FileOpen(const char* name, uint32_t flags, void** outHandle)
    {
        return Native<off::Storage_FileOpenFn>(off::kFileOpen)(nullptr, name, flags, outHandle);
    }

    // Size of an open file; returns the low dword, fills sizeHigh.
    inline uint32_t FileSize(void* handle, uint32_t* sizeHigh)
    {
        return Native<off::Storage_FileSizeFn>(off::kFileSize)(handle, sizeHigh);
    }

    // Read len bytes into dst; fills read; returns nonzero on success.
    inline int FileRead(void* handle, void* dst, uint32_t len, uint32_t* read)
    {
        return Native<off::Storage_FileReadFn>(off::kFileRead)(handle, dst, len, read, nullptr, 0);
    }

    // Seek by method (0=begin, 1=current, 2=end); returns the new position low dword.
    inline uint32_t FileSeek(void* handle, int32_t distLow, uint32_t* distHigh, uint32_t method)
    {
        return Native<off::Storage_FileSeekFn>(off::kFileSeek)(handle, distLow, distHigh, method);
    }

    // Close an open file handle.
    inline int FileClose(void* handle)
    {
        return Native<off::Storage_FileCloseFn>(off::kFileClose)(handle);
    }

    // Add the IO bindings to the enumerable catalog (cold, at startup).
    inline void RegisterCatalog()
    {
        Register({ "IO::FileOpen",  off::kFileOpen,  "int(name, flags, &handle)" });
        Register({ "IO::FileRead",  off::kFileRead,  "int(handle, dst, len, &read)" });
        Register({ "IO::FileSize",  off::kFileSize,  "uint32(handle, &sizeHigh)" });
        Register({ "IO::FileSeek",  off::kFileSeek,  "uint32(handle, distLow, &distHigh, method)" });
        Register({ "IO::FileClose", off::kFileClose, "int(handle)" });
    }
}
