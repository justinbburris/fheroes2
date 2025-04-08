#!/usr/bin/env python3

from pathlib import Path
import sys

# Add the parent directory to the Python path so we can import the runner
sys.path.append(str(Path(__file__).parent.parent))
from runner import FHeroes2TestRunner

def test_main_menu():
    """Test basic main menu functionality."""
    runner = FHeroes2TestRunner()

    try:
        runner.start()

        # Wait for game to initialize
        runner.wait(2000)

        # Take a screenshot of the initial menu state
        runner.screenshot("main_menu_initial")

        # Click on "New Game" button (coordinates need to be determined)
        runner.click(400, 300)
        runner.wait(1000)
        runner.screenshot("new_game_screen")

        # Click back to main menu
        runner.click(100, 100)  # Assuming there's a back button
        runner.wait(1000)
        runner.screenshot("back_to_main_menu")

        # Click on "Load Game" button
        runner.click(400, 350)  # Adjust coordinates as needed
        runner.wait(1000)
        runner.screenshot("load_game_screen")

        # Click back to main menu again
        runner.click(100, 100)
        runner.wait(1000)
        runner.screenshot("main_menu_final")

    finally:
        runner.stop()

if __name__ == "__main__":
    test_main_menu()
