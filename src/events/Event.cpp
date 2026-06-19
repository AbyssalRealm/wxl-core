// Event bus: subscriber storage + dispatch.
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

#include "events/Event.hpp"

#include <vector>

namespace
{
    struct Sub { wxl::events::Handler fn; void* user; };

    // Function-local statics: built on first use, so a script's file-scope ctor can Subscribe at DLL
    // load regardless of translation-unit init order (no static-init-order fiasco).
    std::vector<Sub>& Bucket(wxl::events::Event e)
    {
        static std::vector<Sub> buckets[static_cast<size_t>(wxl::events::Event::Count)];
        return buckets[static_cast<size_t>(e)];
    }
}

namespace wxl::events
{
    void Subscribe(Event e, Handler handler, void* user)
    {
        if (e < Event::Count) Bucket(e).push_back({ handler, user });
    }

    void Emit(Event e, const void* args)
    {
        if (e >= Event::Count) return;
        for (const Sub& s : Bucket(e)) s.fn(s.user, args);
    }
}
