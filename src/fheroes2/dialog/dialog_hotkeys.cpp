/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2022 - 2024                                             *
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

#include "dialog_hotkeys.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "agg_image.h"
#include "cursor.h"
#include "dialog.h"
#include "game_delays.h"
#include "game_hotkeys.h"
#include "icn.h"
#include "image.h"
#include "interface_list.h"
#include "localevent.h"
#include "math_base.h"
#include "screen.h"
#include "settings.h"
#include "tools.h"
#include "translations.h"
#include "ui_button.h"
#include "ui_constants.h"
#include "ui_dialog.h"
#include "ui_scrollbar.h"
#include "ui_text.h"
#include "ui_window.h"

namespace
{
    const int32_t keyDescriptionLength = 300;
    const int32_t hotKeyLength = 120;
    const int32_t searchBoxHeight = 21;
    const int32_t searchBoxWidth = 268;

    class HotKeyElement : public fheroes2::DialogElement
    {
    public:
        HotKeyElement( const fheroes2::Key key, fheroes2::Image & output )
            : _restorer( output, 0, 0, 0, 0 )
            , _key( key )
        {
            // Text always occupies the whole width of the dialog.
            _area = { fheroes2::boxAreaWidthPx, fheroes2::getFontHeight( fheroes2::FontSize::NORMAL ) };
        }

        ~HotKeyElement() override = default;

        void draw( fheroes2::Image & output, const fheroes2::Point & offset ) const override
        {
            _restorer.restore();

            fheroes2::MultiFontText text;
            text.add( fheroes2::Text{ _( "Hotkey: " ), fheroes2::FontType::normalYellow() } );
            text.add( fheroes2::Text{ StringUpper( KeySymGetName( _key ) ), fheroes2::FontType::normalWhite() } );

            _restorer.update( offset.x, offset.y, fheroes2::boxAreaWidthPx, text.height() );

            text.draw( offset.x, offset.y, fheroes2::boxAreaWidthPx, output );
        }

        void processEvents( const fheroes2::Point & /*offset*/ ) const override
        {
            const LocalEvent & le = LocalEvent::Get();
            if ( le.isAnyKeyPressed() ) {
                _key = le.getPressedKeyValue();
                _keyChanged = true;
            }
        }

        // Never call this method as a custom image has nothing to popup.
        void showPopup( const int /*buttons*/ ) const override
        {
            assert( 0 );
        }

        bool update( fheroes2::Image & output, const fheroes2::Point & offset ) const override
        {
            if ( _keyChanged ) {
                _keyChanged = false;
                draw( output, offset );
                return true;
            }

            return false;
        }

        void reset()
        {
            _restorer.reset();
        }

        fheroes2::Key getKey() const
        {
            return _key;
        }

    private:
        mutable fheroes2::ImageRestorer _restorer;
        mutable fheroes2::Key _key;
        mutable bool _keyChanged{ false };
    };

    class HotKeyList : public Interface::ListBox<std::pair<Game::HotKeyEvent, Game::HotKeyCategory>>
    {
    public:
        using Interface::ListBox<std::pair<Game::HotKeyEvent, Game::HotKeyCategory>>::ListBox;

        using Interface::ListBox<std::pair<Game::HotKeyEvent, Game::HotKeyCategory>>::ActionListSingleClick;
        using Interface::ListBox<std::pair<Game::HotKeyEvent, Game::HotKeyCategory>>::ActionListPressRight;
        using Interface::ListBox<std::pair<Game::HotKeyEvent, Game::HotKeyCategory>>::ActionListDoubleClick;

        void RedrawItem( const std::pair<Game::HotKeyEvent, Game::HotKeyCategory> & hotKeyEvent, int32_t offsetX, int32_t offsetY, bool current ) override
        {
            fheroes2::Display & display = fheroes2::Display::instance();

            const fheroes2::FontType fontType = current ? fheroes2::FontType::normalYellow() : fheroes2::FontType::normalWhite();

            offsetY += 2;

            fheroes2::Text name( _( Game::getHotKeyEventNameByEventId( hotKeyEvent.first ) ), fontType );
            name.fitToOneRow( keyDescriptionLength );
            name.draw( offsetX + 4, offsetY, display );

            fheroes2::Text hotkey( Game::getHotKeyNameByEventId( hotKeyEvent.first ), fontType );
            hotkey.fitToOneRow( hotKeyLength );
            hotkey.draw( offsetX + keyDescriptionLength + 9, offsetY, hotKeyLength, display );
        }

        void RedrawBackground( const fheroes2::Point & /* unused */ ) override
        {
            if ( _listBackground ) {
                _listBackground->restore();
            }
        }

        void ActionCurrentUp() override
        {
            // Do nothing.
        }

        void ActionCurrentDn() override
        {
            // Do nothing.
        }

        void ActionListSingleClick( std::pair<Game::HotKeyEvent, Game::HotKeyCategory> & /*unused*/ ) override
        {
            // Do nothing.
        }

        void ActionListPressRight( std::pair<Game::HotKeyEvent, Game::HotKeyCategory> & hotKeyEvent ) override
        {
            fheroes2::MultiFontText title;

            title.add( fheroes2::Text{ _( "Category: " ), fheroes2::FontType::normalYellow() } );
            title.add( fheroes2::Text{ _( Game::getHotKeyCategoryName( hotKeyEvent.second ) ), fheroes2::FontType::normalWhite() } );
            title.add( fheroes2::Text{ "\n\n", fheroes2::FontType::normalWhite() } );
            title.add( fheroes2::Text{ _( "Event: " ), fheroes2::FontType::normalYellow() } );
            title.add( fheroes2::Text{ _( Game::getHotKeyEventNameByEventId( hotKeyEvent.first ) ), fheroes2::FontType::normalWhite() } );
            title.add( fheroes2::Text{ "\n\n", fheroes2::FontType::normalWhite() } );
            title.add( fheroes2::Text{ _( "Hotkey: " ), fheroes2::FontType::normalYellow() } );
            title.add( fheroes2::Text{ Game::getHotKeyNameByEventId( hotKeyEvent.first ), fheroes2::FontType::normalWhite() } );

            fheroes2::showMessage( fheroes2::Text{}, title, Dialog::ZERO );
        }

        void ActionListDoubleClick( std::pair<Game::HotKeyEvent, Game::HotKeyCategory> & hotKeyEvent ) override
        {
            HotKeyElement hotKeyUI( Game::getHotKeyForEvent( hotKeyEvent.first ), fheroes2::Display::instance() );

            // Okay and Cancel events are special cases as they are used in dialogs. By default we need to disable these events to allow to be set any key for an event.
            // Global events (that work on all screens) must be disabled as well.
            const fheroes2::Key okayEventKey = Game::getHotKeyForEvent( Game::HotKeyEvent::DEFAULT_OKAY );
            const fheroes2::Key cancelEventKey = Game::getHotKeyForEvent( Game::HotKeyEvent::DEFAULT_CANCEL );
            const fheroes2::Key fullscreenEventKey = Game::getHotKeyForEvent( Game::HotKeyEvent::GLOBAL_TOGGLE_FULLSCREEN );
            const fheroes2::Key textSupportModeEventKey = Game::getHotKeyForEvent( Game::HotKeyEvent::GLOBAL_TOGGLE_TEXT_SUPPORT_MODE );

            Game::setHotKeyForEvent( Game::HotKeyEvent::DEFAULT_OKAY, fheroes2::Key::NONE );
            Game::setHotKeyForEvent( Game::HotKeyEvent::DEFAULT_CANCEL, fheroes2::Key::NONE );
            Game::setHotKeyForEvent( Game::HotKeyEvent::GLOBAL_TOGGLE_FULLSCREEN, fheroes2::Key::NONE );
            Game::setHotKeyForEvent( Game::HotKeyEvent::GLOBAL_TOGGLE_TEXT_SUPPORT_MODE, fheroes2::Key::NONE );

            fheroes2::MultiFontText title;

            title.add( fheroes2::Text{ _( "Category: " ), fheroes2::FontType::normalYellow() } );
            title.add( fheroes2::Text{ _( Game::getHotKeyCategoryName( hotKeyEvent.second ) ), fheroes2::FontType::normalWhite() } );
            title.add( fheroes2::Text{ "\n\n", fheroes2::FontType::normalWhite() } );
            title.add( fheroes2::Text{ _( "Event: " ), fheroes2::FontType::normalYellow() } );
            title.add( fheroes2::Text{ _( Game::getHotKeyEventNameByEventId( hotKeyEvent.first ) ), fheroes2::FontType::normalWhite() } );

            const int result = fheroes2::showMessage( fheroes2::Text{}, title, Dialog::OK | Dialog::CANCEL, { &hotKeyUI } );

            Game::setHotKeyForEvent( Game::HotKeyEvent::DEFAULT_OKAY, okayEventKey );
            Game::setHotKeyForEvent( Game::HotKeyEvent::DEFAULT_CANCEL, cancelEventKey );
            Game::setHotKeyForEvent( Game::HotKeyEvent::GLOBAL_TOGGLE_FULLSCREEN, fullscreenEventKey );
            Game::setHotKeyForEvent( Game::HotKeyEvent::GLOBAL_TOGGLE_TEXT_SUPPORT_MODE, textSupportModeEventKey );

            if ( result == Dialog::OK ) {
                Game::setHotKeyForEvent( hotKeyEvent.first, hotKeyUI.getKey() );
            }
        }

        void initListBackgroundRestorer( fheroes2::Rect roi )
        {
            _listBackground = std::make_unique<fheroes2::ImageRestorer>( fheroes2::Display::instance(), roi.x, roi.y, roi.width, roi.height );
        }

        void updateScrollBarImage()
        {
            const int32_t scrollBarWidth = GetScrollbar().width();
            setScrollBarImage( fheroes2::generateScrollbarSlider( GetScrollbar(), false, GetScrollbar().getArea().height, VisibleItemCount(), _size(),
                                                                { 0, 0, scrollBarWidth, 8 }, { 0, 7, scrollBarWidth, 8 } ) );
        }

        void filterByText( const std::string & text, const std::vector<std::pair<Game::HotKeyEvent, Game::HotKeyCategory>> & allEvents )
        {
            std::vector<std::pair<Game::HotKeyEvent, Game::HotKeyCategory>> filteredEvents;

            if ( text.empty() ) {
                filteredEvents = allEvents;
            }
            else {
                const std::string lowerText = StringLower( text );
                for ( const auto & event : allEvents ) {
                    const std::string eventName = StringLower( _( Game::getHotKeyEventNameByEventId( event.first ) ) );
                    const std::string hotkeyName = StringLower( Game::getHotKeyNameByEventId( event.first ) );
                    if ( eventName.find( lowerText ) != std::string::npos || hotkeyName.find( lowerText ) != std::string::npos ) {
                        filteredEvents.push_back( event );
                    }
                }
            }

            SetListContent( filteredEvents );
            updateScrollBarImage();
            Redraw();
        }

        void Redraw() override
        {
            Interface::ListBox<std::pair<Game::HotKeyEvent, Game::HotKeyCategory>>::Redraw();
        }

    private:
        std::unique_ptr<fheroes2::ImageRestorer> _listBackground;
    };
}

namespace fheroes2
{
    void openHotkeysDialog()
    {
        // Setup cursor.
        const CursorRestorer cursorRestorer( true, ::Cursor::POINTER );

        fheroes2::Display & display = fheroes2::Display::instance();

        // Dialog height is capped with the current screen height minus 100 pixels.
        fheroes2::StandardWindow background( keyDescriptionLength + hotKeyLength + 8 + 75, std::min( display.height() - 100, 520 ), true, display );

        const fheroes2::Rect roi( background.activeArea() );
        const fheroes2::Rect listRoi( roi.x + 24, roi.y + 37 + searchBoxHeight + 10, keyDescriptionLength + hotKeyLength + 8, roi.height - 75 - searchBoxHeight - 10 );

        const fheroes2::Text title( _( "Hot Keys:" ), fheroes2::FontType::normalYellow() );
        title.draw( roi.x + ( roi.width - title.width() ) / 2, roi.y + 16, display );

        // Add search box
        const fheroes2::Point searchBoxPos( roi.x + ( roi.width - searchBoxWidth ) / 2, roi.y + 37 );
        const fheroes2::Sprite & searchBoxSprite = fheroes2::AGG::GetICN( ICN::BUYBUILD, 3 );
        fheroes2::Blit( searchBoxSprite, display, searchBoxPos.x, searchBoxPos.y );

        // We divide the list: action description and binded hot-key.
        background.applyTextBackgroundShading( { listRoi.x, listRoi.y, keyDescriptionLength + 8, listRoi.height } );
        background.applyTextBackgroundShading( { listRoi.x + keyDescriptionLength + 8, listRoi.y, hotKeyLength, listRoi.height } );

        const bool isEvilInterface = Settings::Get().isEvilInterfaceEnabled();

        // Prepare OKAY button and render its shadow.
        fheroes2::Button buttonOk;
        const int buttonOkIcn = isEvilInterface ? ICN::BUTTON_SMALL_OKAY_EVIL : ICN::BUTTON_SMALL_OKAY_GOOD;
        background.renderButton( buttonOk, buttonOkIcn, 0, 1, { 0, 7 }, StandardWindow::Padding::BOTTOM_CENTER );

        HotKeyList listbox( roi.getPosition() );
        listbox.initListBackgroundRestorer( listRoi );
        listbox.SetAreaItems( { listRoi.x, listRoi.y + 3, listRoi.width - 3, listRoi.height - 4 } );

        int32_t scrollbarOffsetX = roi.x + roi.width - 35;
        background.renderScrollbarBackground( { scrollbarOffsetX, listRoi.y, listRoi.width, listRoi.height }, isEvilInterface );

        const int32_t topPartHeight = 19;
        const int listIcnId = isEvilInterface ? ICN::SCROLLE : ICN::SCROLL;
        ++scrollbarOffsetX;

        listbox.SetScrollButtonUp( listIcnId, 0, 1, { scrollbarOffsetX, listRoi.y + 1 } );
        listbox.SetScrollButtonDn( listIcnId, 2, 3, { scrollbarOffsetX, listRoi.y + listRoi.height - 15 } );
        listbox.setScrollBarArea( { scrollbarOffsetX + 2, listRoi.y + topPartHeight, 10, listRoi.height - 2 * topPartHeight } );
        listbox.setScrollBarImage( fheroes2::AGG::GetICN( listIcnId, 4 ) );
        listbox.SetAreaMaxItems( ( listRoi.height - 7 ) / fheroes2::getFontHeight( fheroes2::FontSize::NORMAL ) );

        std::vector<std::pair<Game::HotKeyEvent, Game::HotKeyCategory>> hotKeyEvents = Game::getAllHotKeyEvents();

        listbox.SetListContent( hotKeyEvents );
        listbox.updateScrollBarImage();
        listbox.Redraw();

        // Search text input
        std::string searchText;
        size_t cursorPosition = 0;
        bool isCursorVisible = true;
        fheroes2::ImageRestorer searchBoxRestorer( display, searchBoxPos.x + 13, searchBoxPos.y + 1, searchBoxWidth - 26, searchBoxHeight - 3 );

        auto redrawSearchBox = [&display, &searchText, &cursorPosition, &isCursorVisible, &searchBoxRestorer]() {
            searchBoxRestorer.restore();
            fheroes2::Text text( insertCharToString( searchText, cursorPosition, isCursorVisible ? '_' : '\x7F' ), fheroes2::FontType::normalWhite() );
            text.draw( searchBoxRestorer.x(), searchBoxRestorer.y() + 2, display );
            display.render( searchBoxRestorer.rect() );
        };

        display.render( background.totalArea() );

        LocalEvent & le = LocalEvent::Get();
        while ( le.HandleEvents() ) {
            buttonOk.drawOnState( le.isMouseLeftButtonPressedInArea( buttonOk.area() ) );

            listbox.QueueEventProcessing();

            if ( le.MouseClickLeft( buttonOk.area() ) || Game::HotKeyCloseWindow() ) {
                return;
            }

            if ( le.isMouseRightButtonPressedInArea( buttonOk.area() ) ) {
                fheroes2::showStandardTextMessage( _( "Okay" ), _( "Exit this menu." ), Dialog::ZERO );
                continue;
            }

            // Handle search box input
            if ( le.isAnyKeyPressed() ) {
                const fheroes2::Key key = le.getPressedKeyValue();
                bool updateList = false;

                if ( key == fheroes2::Key::KEY_BACKSPACE || key == fheroes2::Key::KEY_DELETE ) {
                    if ( !searchText.empty() && cursorPosition > 0 ) {
                        searchText.erase( cursorPosition - 1, 1 );
                        --cursorPosition;
                        updateList = true;
                    }
                }
                else if ( key == fheroes2::Key::KEY_LEFT && cursorPosition > 0 ) {
                    --cursorPosition;
                }
                else if ( key == fheroes2::Key::KEY_RIGHT && cursorPosition < searchText.size() ) {
                    ++cursorPosition;
                }
                else if ( searchText.size() < 20 ) {
                    const size_t oldPos = cursorPosition;
                    cursorPosition = InsertKeySym( searchText, cursorPosition, key, LocalEvent::getCurrentKeyModifiers() );
                    if ( cursorPosition != oldPos ) {
                        updateList = true;
                    }
                }

                if ( updateList ) {
                    background.applyTextBackgroundShading( { listRoi.x, listRoi.y, keyDescriptionLength + 8, listRoi.height } );
                    background.applyTextBackgroundShading( { listRoi.x + keyDescriptionLength + 8, listRoi.y, hotKeyLength, listRoi.height } );
                    listbox.filterByText( searchText, hotKeyEvents );
                    display.render( listRoi );
                }
                redrawSearchBox();
            }

            // Handle cursor blinking
            if ( Game::validateAnimationDelay( Game::DelayType::CURSOR_BLINK_DELAY ) ) {
                isCursorVisible = !isCursorVisible;
                redrawSearchBox();
            }

            if ( !listbox.IsNeedRedraw() ) {
                continue;
            }

            listbox.Redraw();
            display.render( roi );
        }
    }
}
