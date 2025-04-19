/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2023 - 2025                                             *
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
 *   along with this program; if not, write to the                        *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#pragma once

#include <cstdint>

#include "editor_interface.h"
#include "interface_base.h"
#include "interface_gamearea.h"
#include "math_base.h"
#include "image.h"

namespace Interface
{
    class EditorMapArea : public GameArea
    {
    public:
        explicit EditorMapArea( EditorInterface & interface_ )
            : GameArea( static_cast<BaseInterface &>( interface_ ) )
            , _interface( interface_ )
        {}

        void renderTileAreaSelect( fheroes2::Image & display, const int32_t startTileId, const int32_t endTileId, const bool isActionObject );

    private:
        EditorInterface & _interface;
    };
}
