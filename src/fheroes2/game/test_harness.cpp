#include "test_harness.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

#include <SDL_events.h>
#include <SDL_mouse.h>

#include "game.h"
#include "game_interface.h"
#include "localevent.h"
#include "screen.h"
#include "image_tool.h"

namespace TestHarness
{
    bool isTestHarnessMode(int argc, char** argv)
    {
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--test-harness") {
                return true;
            }
        }
        return false;
    }

    void handleClick(int x, int y)
    {
        std::cout << "Click at: " << x << ", " << y << std::endl;
        SDL_Event event;
        event.type = SDL_MOUSEBUTTONDOWN;
        event.button.button = SDL_BUTTON_LEFT;
        event.button.x = x;
        event.button.y = y;
        SDL_PushEvent(&event);

        // Process any pending events
        LocalEvent::Get().HandleEvents(false);
    }

    void handleScreenshot(const std::string& filename)
    {
        // Get the current display
        const fheroes2::Display & display = fheroes2::Display::instance();

        // Check if we should use PNG format
        if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".png") {
            // Use SaveImage directly for PNG format
            fheroes2::SaveImage(display, filename);
        } else {
            // Use Save for other formats (defaults to BMP)
            fheroes2::Save(display, filename);
        }
        std::cout << "Screenshot saved to: " << filename << std::endl;
    }

    void handleWait(int milliseconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    void handleLoadSavegame(const std::string& path)
    {
        Game::LoadGame();
    }

    bool runTestHarnessCommands()
    {
        // Main test harness loop - just process commands
        while (true) {
            std::string line;
            std::getline(std::cin, line);
            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;

            if (cmd == "click") {
                int x, y;
                iss >> x >> y;
                handleClick(x, y);
            }
            else if (cmd == "screenshot") {
                std::string filename;
                iss >> filename;
                handleScreenshot(filename);
            }
            else if (cmd == "wait") {
                int ms;
                iss >> ms;
                handleWait(ms);
            }
            else if (cmd == "load") {
                std::string path;
                iss >> path;
                handleLoadSavegame(path);
            }
            else if (cmd == "stop") {
                break;
            }
            else {
                std::cerr << "Unknown command: " << cmd << std::endl;
            }
        }

        return true;
    }
}
