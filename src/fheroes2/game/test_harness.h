#pragma once

#include <string>

namespace TestHarness
{
    bool isTestHarnessMode(int argc, char** argv);
    void handleClick(int x, int y);
    void handleScreenshot(const std::string& path);
    void handleWait(int milliseconds);
    void handleLoadSavegame(const std::string& path);
    bool runTestHarnessCommands();
}
