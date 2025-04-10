/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2025                                             *
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

#ifndef H2_SCREENSHOT_MANAGER_H
#define H2_SCREENSHOT_MANAGER_H

#include <string>

namespace fheroes2
{
    class Display;
}

class ScreenshotManager
{
public:
    ScreenshotManager() = default;
    ~ScreenshotManager() = default;

    ScreenshotManager( const ScreenshotManager & ) = delete;
    ScreenshotManager & operator=( const ScreenshotManager & ) = delete;

    ScreenshotManager( ScreenshotManager && ) = delete;
    ScreenshotManager & operator=( ScreenshotManager && ) = delete;

    // Take a screenshot of the current display and save it to the screenshots directory
    bool takeScreenshot( const fheroes2::Display & display );

private:
    // Generate a filename with timestamp for the screenshot
    std::string generateScreenshotFilename() const;
};

#endif
