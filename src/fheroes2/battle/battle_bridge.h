/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2019 - 2025                                             *
 *                                                                         *
 *   Free Heroes2 Engine: http://sourceforge.net/projects/fheroes2         *
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#pragma once

#include <cassert>
#include <cstdint>

namespace Battle
{
    class Unit;

    class Bridge final
    {
    public:
        Bridge() = default;
        Bridge( const Bridge & ) = delete;

        ~Bridge() = default;

        Bridge & operator=( const Bridge & ) = delete;

        void ActionUp();
        void ActionDown();

        void SetDestroyed();
        void SetPassability( const Unit & unit ) const;

        bool AllowUp() const
        {
            // Yes if not destroyed and lowered and there are no any troops (alive or dead) on or under the bridge
            return isValid() && isDown() && !isOccupied();
        }

        bool NeedDown( const Unit & unit, const int32_t dstIdx ) const;

        bool isPassable( const Unit & unit ) const;

        bool isValid() const
        {
            return !_isDestroyed;
        }

        bool isDestroyed() const
        {
            return _isDestroyed;
        }

        bool isDown() const
        {
            assert( !_isDestroyed || _isDown );

            return _isDown;
        }

    private:
        static bool isOccupied();

        bool _isDestroyed{ false };
        bool _isDown{ false };

        enum
        {
            CELL_ABOVE_BRIDGE = 39,
            CELL_BEFORE_MOAT = 48,
            CELL_MOAT = 49,
            CELL_GATES = 50,
            CELL_AFTER_GATES = 51,
            CELL_BELOW_BRIDGE = 61
        };
    };
}
