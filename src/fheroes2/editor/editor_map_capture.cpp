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

#include "editor_interface.h"

#include "game.h"
#include "game_interface.h"
#include "interface_gamearea.h"
#include "translations.h"
#include "world.h"
#include "editor_map_capture.h"
#include "localevent.h"
#include "screen.h"

namespace Interface
{
    void MapCapture::startCapture()
    {
        _isActive = true;
        _currentScrollX = 0;
        _currentScrollY = 0;
    }

    void MapCapture::update()
    {
        if ( !_isActive )
            return;

        const LocalEvent & le = LocalEvent::Get();
        const fheroes2::Point & mousePos = le.getMouseCursorPos();

        // Check if mouse is near screen edges for scrolling
        const int32_t scrollThreshold = 20;
        const int32_t scrollSpeed = 8;

        if ( mousePos.x < scrollThreshold )
            _currentScrollX = -scrollSpeed;
        else if ( mousePos.x > fheroes2::Display::instance().width() - scrollThreshold )
            _currentScrollX = scrollSpeed;
        else
            _currentScrollX = 0;

        if ( mousePos.y < scrollThreshold )
            _currentScrollY = -scrollSpeed;
        else if ( mousePos.y > fheroes2::Display::instance().height() - scrollThreshold )
            _currentScrollY = scrollSpeed;
        else
            _currentScrollY = 0;
    }

    void MapCapture::showMessage()
    {
        // TODO: Implement message display functionality
    }
}
