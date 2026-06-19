// PeImage: the in-memory 32-bit PE a PatchScript edits through (the patcher toolbox).
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
#include <vector>

// The patcher's reusable toolbox: a loaded PE on disk + the byte/structural edits a PatchScript drives.
// The core provides these mechanics; a PatchScript (script) decides which to call and with what. Header
// pointers are recomputed on each call because AddImport can grow the buffer.
namespace wxl::patcher
{
    class PeImage
    {
    public:
        explicit PeImage(std::vector<uint8_t>& bytes) : bytes_(bytes) {}

        bool     valid() const;       // 32-bit PE with sane headers
        uint32_t imageBase() const;

        // Map a virtual address to a file offset, or 0 if it is not backed by a section.
        uint32_t VaToOffset(uint32_t va) const;

        // Overwrite `len` bytes at a VA. False if the VA is not mapped to file bytes.
        bool WriteVa(uint32_t va, const void* src, uint32_t len);

        // Is a section with this name present? (compares up to 8 chars)
        bool HasSection(const char* name) const;

        // Give the 32-bit image the full 4 GB address space (LARGE_ADDRESS_AWARE).
        bool SetLargeAddressAware();

        // Append an import of dll!func via a new section tagged `tagSection`. Idempotent: a no-op that
        // returns true if `tagSection` already exists.
        bool AddImport(const char* dll, const char* func, const char* tagSection);

        std::vector<uint8_t>& bytes() { return bytes_; }

    private:
        std::vector<uint8_t>& bytes_;
    };
}
