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

#include "screen.h"
#include "image_palette.h"
#include "logging.h"
#include "system.h"

bool ScreenshotManager::takeScreenshot( const fheroes2::Display & display )
{
    const std::string screenshotPath = System::concatPath( System::GetDataDirectory( "fheroes2" ), "screenshots" );

    // Create screenshots directory if it doesn't exist
    if ( !System::IsDirectory( screenshotPath ) ) {
        System::MakeDirectory( screenshotPath );
    }

    const std::string filename = System::concatPath( screenshotPath, generateScreenshotFilename() );

    // Create SDL surface from display
    // The game uses an 8-bit indexed color mode (one byte per pixel)
    SDL_Surface * surface = SDL_CreateRGBSurface( 0, display.width(), display.height(), 8, 0, 0, 0, 0 );
    if ( !surface ) {
        ERROR_LOG( "Failed to create SDL surface for screenshot. Error: " << SDL_GetError() )
        return false;
    }

    // Set up palette
    // The game internally uses a 6-bit palette (0-63 range for R,G,B components)
    // We need to convert it to an 8-bit palette (0-255 range) for the output image
    std::vector<SDL_Color> paletteSDL;
    paletteSDL.resize( 256 );
    const uint8_t * gamePalette = fheroes2::getGamePalette();

    for ( int i = 0; i < 256; ++i ) {
        // Each color in the palette uses 3 bytes (R,G,B)
        const uint8_t * value = gamePalette + i * 3;
        SDL_Color & col = paletteSDL[i];

        // Convert from 6-bit (0-63) to 8-bit (0-255) color space by left-shifting 2 bits (multiply by 4)
        // This maps the range 0-63 to 0-252
        col.r = *value << 2;
        col.g = *( value + 1 ) << 2;
        col.b = *( value + 2 ) << 2;
        col.a = 255;  // Set full opacity as the game doesn't use alpha channel
    }

    SDL_SetPaletteColors( surface->format->palette, paletteSDL.data(), 0, 256 );

    // Copy display data to surface
    // Each pixel in display is an 8-bit index (0-255) into the color palette
    const uint8_t * imageData = display.image();
    memcpy( surface->pixels, imageData, static_cast<size_t>( display.width() * display.height() ) );

    // Save surface to file
    // Use PNG format if available, otherwise fall back to BMP
#if defined( WITH_IMAGE )
    const int result = IMG_SavePNG( surface, System::encLocalToUTF8( filename ).c_str() );
#else
    const int result = SDL_SaveBMP( surface, System::encLocalToUTF8( filename ).c_str() );
#endif

    SDL_FreeSurface( surface );

    if ( result != 0 ) {
        ERROR_LOG( "Failed to save screenshot to " << filename << ". Error: " << SDL_GetError() )
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
