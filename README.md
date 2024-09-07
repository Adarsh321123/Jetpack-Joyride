# Jetpack Joyride

Team Members: Adarsh Kumarappan, Dhruv Sheth, Rayhan Zirvi

Welcome to our Jetpack Joyride clone written in C for Caltech's Software Design class!

# For Caltech Students:
Please do not look at this repo if you are currently taking or plan to take CS 3. The Caltech Honor Code states that "No member of the Caltech community shall take unfair advantage of any other member of the Caltech community."

# Usage
1. Install `sdl2`, `sdl2_gfx`, `sdl2_ttf`, and `sdl2_image`.
2. If necessary, export the necessary PATH variables to point to the libraries. For example, on an M1 Mac I must use:
```
export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig"
export C_INCLUDE_PATH="/opt/homebrew/include"
export LIBRARY_PATH="/opt/homebrew/lib"
export LD_LIBRARY_PATH="/opt/homebrew/lib"
source ~/.zshrc
```
3. [Download and install Emscripten](https://emscripten.org/docs/getting_started/downloads.html) using the `emsdk`
4. For M1 Mac users: run `EMSDK_PYTHON=/opt/homebrew/bin/python3`
5. Finally, run `make clean && make NO_ASAN=true game`.

Additionally:
- Please try running the game on Chrome for best results.
- There is an intentional 3 second delay for some powerups, such as the shield, to activate.

Please feel free to add an issue if you have any questions or concerns.

# Game Details

The player must run and dodge obstacles in order to survive at different difficulty levels. They can also collect coins and complete achievements. There are also 4 powerups available: shield, gravity swap, extra coins, and coin magnet.

The controls are very simple: press and hold the space bar to move up and release to move down. However, if the gravity swap powerup is on, tapping the space bar changes the direction of gravity.

## Roles

We made the physics engine together with some starter code from Prof. Blank!

Adarsh:
- Scrolling background, adding the player, and implementing the keyboard functionality for moving the player.
    - We will keep the player the same horizontally.
    - The player only moves vertically.
    - The background moves to the left of the player.
- Pseudo-randomly generating bodies for obstacles such as Zappers in the game + collision detection for ending the game.
    - We will make the obstacles randomly span vertically.
- Implement an event structure to be used by achievements in Priority 4.
    - There will be an observer and a subject.
    - The game play state has a subject which has observers watching it.
    - So, the subject can notify its observers that an event happened, and the observers can do as they wish.
- Add player achievements.
    - Implement file storage, so the file should be saved for the current game.
    - Use the event structure from Priority 3 to communicate that an event happened and act on it in the function pointer that the achievements use.
    - Show the achievements in the setting menu.

Dhruv:
- Porting all the features from the physics engine implemented before starting the game, namely the controls using keyboard and mouse handler, implementing the layout and position of the static bodies in the background and associating assets with bodies.
    - We also need to port over the collision ability for the zapper and the forces and impulses with things like the zappers
    - Implementation of encapsulation of different priority features in different files such as one for lasers, rockets and zappers. Additional encapsulation required for powerups and other features added in the game.
- Implement Lasers in this function. There are multiple subfeatures associated with the laser. First, ensure that the levels are passable which means zappers and lasers are not generated at the same time. Reuse the same images rendered for lasers the first time lasers are generated for the lasers generated at other timestamps to avoid the cost of rendering images and reduce the lag. Lasers are generated pseudorandomly. Add functionality of display for one lasers initially and then extend that to display multiple lasers at the same time.
- Adding rockets to the game ensures that the levels are still passable with rockets and zappers implemented and we must iterate upon the logic from passability. Add animation to rockets and lasers which allows changing from inactive to active state for lasers and from warning state to rocket being deployed for rockets. Ensure that the velocity of the rockets and the time for display for lasers depend on the difficulty of the level.
- Adding power ups to the game. We will be adding the basic powerup to disable all the obstacles for a specific amount of time. Implement the framework for the powerups to activate it using collision_handlers centrally. Additionally, add powerup to generate more coins for the given time frame of powerup. (Dhruv)
    - This can also depend on the difficulty of game
    - So harder difficulties might mean the power up lasts for less time.

Rayhan:
- Home screen and navigation, the ability to navigate to multiple menus, and associating a specific menu with objects in that menu.
    - We will also include a settings UI that will use the features that we implemented, like the difficulty level.
    - This will also include the game over screen when the user hits the obstacle.
- Add levels to the main menu to select from in the settings page. Display the current difficulty selected, and allow the user to toggle difficulty. The difficulty of the levels could be probabilistic and the frequency of generation of different obstacles could be attributed with each level.
- Add coins to the game. We must generate multiple coins in a cluster. We can also update the shape of the coins to be different sizes of blocks probabilistically. Add text to screen to track distance traveled and coins collected.
- Additional features such as:
    - Gravity swap: Power-up that can switch gravity of Jetpack character
    - Add a magnet powerup to the game and associate a gravitational force with all the coin objects relative to the coin powerup collected. Attribute time period for the existence of the magnetic powerup in the game.

## Citations

"Jetpack Joyride Wallpapers." Wallpaper Cave, wallpapercave.com/jetpack-joyride-wallpapers. Accessed 1 June 2024.

Pauli, Gustavo. "Jetpack-Goodride/assets." GitHub, github.com/Gustavo-Pauli/Jetpack-Goodride/tree/master/assets. Accessed 1 June 2024.

"Jetpack Joyride Wiki." Fandom, jetpackjoyride.fandom.com/wiki/Jetpack_Joyride_Wiki. Accessed 1 June 2024.
