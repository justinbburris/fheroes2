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

#include "screenshot_manager.h"

#include <SDL.h>
#include <ctime>

#include "image.h"
#include "image_tool.h"
#include "logging.h"
#include "screen.h"
#include "system.h"

bool ScreenshotManager::takeScreenshot( const fheroes2::Display & display )
{
    const std::string screenshotPath = System::concatPath( System::GetDataDirectory( "fheroes2" ), "screenshots" );

    // Create screenshots directory if it doesn't exist
    if ( !System::IsDirectory( screenshotPath ) ) {
        System::MakeDirectory( screenshotPath );
    }

    const std::string filename = System::concatPath( screenshotPath, generateScreenshotFilename() );

    // Get the SDL renderer from the window
    SDL_Window * window = SDL_GetWindowFromID( 1 );
    if ( !window ) {
        ERROR_LOG( "Failed to get SDL window for screenshot. Error: " << SDL_GetError() )
        return false;
    }

    SDL_Renderer * renderer = SDL_GetRenderer( window );
    if ( !renderer ) {
        ERROR_LOG( "Failed to get SDL renderer for screenshot. Error: " << SDL_GetError() )
        return false;
    }

    // Get the current render target size
    int width, height;
    SDL_GetRendererOutputSize( renderer, &width, &height );

    // Create a surface to hold the screenshot data
    SDL_Surface * surface = SDL_CreateRGBSurface( 0, width, height, 32, 0, 0, 0, 0 );
    if ( !surface ) {
        ERROR_LOG( "Failed to create SDL surface for screenshot. Error: " << SDL_GetError() )
        return false;
    }

    // Read pixels from the renderer
    if ( SDL_RenderReadPixels( renderer, NULL, surface->format->format, surface->pixels, surface->pitch ) != 0 ) {
        ERROR_LOG( "Failed to read pixels from renderer. Error: " << SDL_GetError() )
        SDL_FreeSurface( surface );
        return false;
    }

    // Convert SDL surface to fheroes2::Image
    fheroes2::Image screenshot( width, height );
    const uint32_t * inY = static_cast<uint32_t *>( surface->pixels );
    uint8_t * outY = screenshot.image();

    for ( int y = 0; y < height; ++y ) {
        const uint32_t * inX = inY;
        uint8_t * outX = outY;

        for ( int x = 0; x < width; ++x, ++inX, ++outX ) {
            const uint8_t r = ( *inX >> 16 ) & 0xFF;
            const uint8_t g = ( *inX >> 8 ) & 0xFF;
            const uint8_t b = *inX & 0xFF;
            *outX = fheroes2::GetColorId( r, g, b );
        }

        inY += surface->pitch / 4;
        outY += width;
    }

    SDL_FreeSurface( surface );

    // Save using the existing image tool functionality
    if ( !fheroes2::Save( screenshot, filename ) ) {
        ERROR_LOG( "Failed to save screenshot to " << filename )
        return false;
    }

    DEBUG_LOG( DBG_GAME, DBG_INFO, "Screenshot saved to " << filename )
    return true;
}

std::string ScreenshotManager::generateScreenshotFilename() const
{
    // Create a unique timestamp-based filename in the format: fheroes2_YYYYMMDD_HHMMSS.png
    const time_t now = time( nullptr );
    const struct tm * timeinfo = localtime( &now );
    char timestamp[32];
    strftime( timestamp, sizeof( timestamp ), "%Y%m%d_%H%M%S", timeinfo );
    return "fheroes2_" + std::string( timestamp ) + ".png";
}
