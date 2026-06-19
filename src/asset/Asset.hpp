// Single include surface over the on-disk file-format contracts (one folder per format).
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

// The typed file-format contracts. Client.hpp = the target on-disk layout the native loader consumes;
// Source.hpp = the modern additions a transform normalizes. Contracts only: the actual read/merge
// recipes live in scripts (modules), per the core/module split.
#include "asset/adt/Client.hpp"
#include "asset/adt/Source.hpp"
#include "asset/m2/Client.hpp"
#include "asset/m2/Source.hpp"
#include "asset/wdl/Client.hpp"
#include "asset/wdl/Source.hpp"
#include "asset/wdt/Client.hpp"
#include "asset/wdt/Source.hpp"
#include "asset/wmo/Client.hpp"
#include "asset/wmo/Source.hpp"
