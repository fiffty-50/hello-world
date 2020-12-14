/*
 *openTail Log - A FOSS Tail Logbook Application
 *Copyright (C) 2020  Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "aaircraftentry.h"

namespace experimental {

AAircraftEntry::AAircraftEntry()
    : AEntry::AEntry(DEFAULT_AIRCRAFT_POSITION)
{}

AAircraftEntry::AAircraftEntry(int row_id)
    : AEntry::AEntry(DataPosition("aircraft", row_id))
{}

AAircraftEntry::AAircraftEntry(TableData table_data)
    : AEntry::AEntry(DEFAULT_AIRCRAFT_POSITION, table_data)
{}

} // namespace experimental
