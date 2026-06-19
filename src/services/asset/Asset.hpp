// Asset base: the cold per-file-open object, plus the Tier-0 IFF byte plumbing.
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
#include <cstring>
#include <span>
#include <string_view>
#include <vector>

namespace wxl::asset
{
    // File classes the toolbox knows. Classify() maps a requested name to one of these (cold, once).
    enum class Format : uint32_t { Raw, Adt, Wdt, Wdl, Wmo, M2 };

    Format Classify(std::string_view name);

    // Pure IFF byte plumbing, no notion of any format's meaning.
    namespace iff
    {
        inline uint32_t Rd32(const uint8_t* p)       { uint32_t v; std::memcpy(&v, p, 4); return v; }
        inline void     Wr32(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }
        inline uint16_t Rd16(const uint8_t* p)       { uint16_t v; std::memcpy(&v, p, 2); return v; }
        inline void     Wr16(uint8_t* p, uint16_t v) { std::memcpy(p, &v, 2); }

        // Find a chunk by 4-byte magic in an IFF body. Returns the offset of the chunk DATA (past the
        // 8-byte chunk header), or npos. When found and outSize != null, writes the chunk data size.
        constexpr size_t npos = static_cast<size_t>(-1);
        size_t FindChunk(std::span<const uint8_t> body, uint32_t magic, uint32_t* outSize = nullptr);

        // Append a blob to the tail of buf and return the offset where it landed. Realloc-safe: callers
        // must re-fetch any interior pointer into buf after this returns.
        size_t AppendTail(std::vector<uint8_t>& buf, std::span<const uint8_t> blob);

        // Rewrite an M2Array {u32 count, u32 offset} pair living at a header field.
        void RewriteM2Array(uint8_t* field, uint32_t count, uint32_t offset);
    }

    class Asset
    {
    public:
        explicit Asset(std::span<const uint8_t> source)
            : source_(source), bytes_(source.begin(), source.end()) {}
        virtual ~Asset() = default; // cold boundary: vtable here is fine, hot paths use bytes() only

        Format                       format() const  { return format_; }
        std::span<const uint8_t>     source() const  { return source_; }
        std::vector<uint8_t>&        bytes()         { return bytes_; }
        const std::vector<uint8_t>&  bytes() const   { return bytes_; }
        bool                         changed() const { return changed_; }

    protected:
        void markChanged() { changed_ = true; }

        std::span<const uint8_t> source_;
        std::vector<uint8_t>     bytes_;
        Format                   format_  = Format::Raw;
        bool                     changed_ = false;
    };
}
