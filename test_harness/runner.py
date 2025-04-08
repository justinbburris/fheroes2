#!/usr/bin/env python3

import os
import subprocess
import time
from pathlib import Path

class FHeroes2TestRunner:
    def __init__(self, game_path=None):
        self.game_path = game_path or "./fheroes2"
        self.process = None
        self.screenshot_dir = Path("screenshots")
        self.screenshot_dir.mkdir(exist_ok=True)

    def start(self):
        """Start the game in test harness mode."""
        env = os.environ.copy()
        if os.getenv("CI"):
            env["SDL_VIDEODRIVER"] = "dummy"

        self.process = subprocess.Popen(
            [self.game_path, "--test-harness"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            env=env
        )

    def send_command(self, cmd):
        """Send a command to the game."""
        if not self.process:
            raise RuntimeError("Game process not started")
        self.process.stdin.write(cmd + "\n")
        self.process.stdin.flush()

    def click(self, x, y):
        """Simulate a mouse click at the given coordinates."""
        self.send_command(f"click {x} {y}")

    def wait(self, milliseconds):
        """Wait for the specified number of milliseconds."""
        self.send_command(f"wait {milliseconds}")

    def screenshot(self, name):
        """Take a screenshot and save it with the given name."""
        path = self.screenshot_dir / f"{name}.png"
        self.send_command(f"screenshot {path}")

    def load_savegame(self, path):
        """Load a saved game from the given path."""
        self.send_command(f"load_savegame {path}")

    def stop(self):
        """Stop the game process."""
        if self.process:
            self.send_command("exit")
            self.process.wait()
            self.process = None

def main():
    # Basic test scenario
    runner = FHeroes2TestRunner()

    try:
        print("Starting fheroes2 in test harness mode...")
        runner.start()

        # Wait for game to initialize (5 seconds)
        print("Waiting for game to initialize...")
        runner.wait(5000)

        # Take a screenshot of the initial state
        print("Taking initial screenshot...")
        runner.screenshot("initial_state")
        runner.wait(1000)

        # Click in the center of the screen
        print("Clicking center of screen...")
        runner.click(320, 240)
        runner.wait(1000)
        runner.screenshot("after_click")

    finally:
        print("Stopping test harness...")
        runner.stop()

if __name__ == "__main__":
    main()
