# Game Design Document

## Section 0: Summary
This section should tell the reader what to expect in the future sections.  In particular, it should contain at least:
- a working title for your game
- a list of your team members (and their roles if decided)
- a "concept statement" of your game ("your game in a tweet")

Title: Jetpack Joyride

Team Members: Adarsh Kumarappan, Dhruv Sheth, Rayhan Zirvi

We do not have discrete roles for each person, but please see the sections below to see how we assigned the features to each person.

Concept: We will be replicating the popular Jetpack Joyride game with some changes.

## Section 1: Gameplay
This section should address simple questions about how your game works:
- How does your game progress?
- What are the win and loss conditions?
- Are there levels?
- Are there points?

This section should also address:
- **controls** (keyboard? mouse? tongue?)
- **physics** (how does your game incorporate the physics engine?)
- **game flow** (what does the game look like from start to end for the player?)
- **graphics** (will you draw polygons? use sprites? make your own vector graphics?)

The game starts with the player at the bottom left of the screen and the idea is that, instead of moving the player forward, we move the obstacles and background relative to the position and velocity of the player. The settings of the game allows the player to see their stats on achievements. You can also enter difficulty levels and start playing in that instance of the game. We associate a pseudorandomness to the generation of obstacles and power ups in the game. The main controls of the game are the space bar that the user can use to move upwards. When they release the space bar, the character falls towards the floor. The user tries to keep going unlimited times until the user hits the zapper, rocket, or lasers. Then, the user loses and the game over screen shows up. here are no win conditions but the user can try to achieve the achievements.

We will use the space bar for the moving up and down of the character.

The physics engine will be used in the collision detection and the coin collecting mechanics mentioned below.

Please see above for the game flow.

We will use sprites for the character, the zappers, lasers, and rockets. Please see below.


## Section 2: Feature Set
This section should reduce your game to a set of individual features (remember iterative development?).  Your team should
assign these features to individual group members and assign each one a priority from one to four (1 = game cannot work without this, 4 = cool but not necessary).

We have gathered together the following list of some example features you might choose to implement:
- make your own graphics or sprites
- add sound effects
- implement a scrolling environment
- implement a networked/multiplayer game
- implement 2D parallax (https://en.wikipedia.org/wiki/Parallax)
- implement rendering of text
- implement a mouse handler
- implement an AI for an enemy
- implement speed-independent friction
- implement more accurate integration (current implementation uses a trapezoid sum)
- implement music

Priority 1 Features

- Scrolling background, adding the player, and implementing the keyboard functionality for moving the player. (Adarsh)
    - So, we will keep the player the same horizontally
    - The player only moves vertically
    - The background moves to the left of the player
- Porting all the features from the physics engine implemented so far, namely the controls using keyboard and mouse handler, implementing the layout and position of the static bodies in the background and associating assets with bodies. (Dhruv)
    - We also need to port over the collision ability for the zapper and the forces and impulses with things like the zappers
    - Implementation of encapsulation of different priority features in different files such as one for lasers, rockets and zappers. Additional encapsulation required for powerups and other features added in the game.
- Home Screen and navigation, ability to navigate to multiple menus and associating a specific menu with objects in that menu. (Rayhan)
    - We will also include a settings UI that will use the features that we will implement in the future like the difficulty level potentially
    - This will also include the game over screen when the user hits the obstacle


Priority 2 Features

- Implement Lasers in this function. There are multiple subfeatures associated with the laser. First, ensure that the levels are passable which means zappers and lasers are not generated at the same time. Reuse the same images rendered for lasers the first time lasers are generated for the lasers generated at other timestamps to avoid the cost of rendering images and reduce the lag. Lasers are generated pseudorandomly. Add functionality of display for one lasers initially and then extend that to display multiple lasers at the same time. (Dhruv)
- Pseudo-randomly generating bodies for obstacles such as Zappers in the game + collision detection for game over (Adarsh)
    - We will make the obstacle randomly span vertically
- Add levels to the main menu to select from in the settings page. Display the current difficulty selected, and allow the user to toggle difficulty. The difficulty of the levels could be probabilistic and the frequency of generation of different obstacles could be attributed with each level. (Rayhan)

Priority 3 Features

- Implement an event structure to be used by achievements in Priority 4 (Adarsh)
    - There will be an observer and a subject
    - The game play state has a subject which has observers watching it
    - So the subject can notify that an event happened, and the observers can do as they wish
- Add coins to the game. We must generate multiple coins in a cluster. We can also update the shape of the coins to be different sizes of blocks probabilistically. Add text to screen to track distance traveled and coins collected. (Rayhan)
- Adding rockets to the game ensures that the levels are still passable with rockets and zappers implemented and we must iterate upon the logic from passability. Add animation to rockets and lasers which allows changing from inactive to active state for lasers and from warning state to rocket being deployed for rockets. Ensure that the velocity of the rockets and the time for display for lasers depend on the difficulty of the level. (Dhruv)

Priority 4 Features
- Adding power ups to the game. We will be adding the basic powerup to disable all the obstacles for a specific amount of time. Implement the framework for the powerups to activate it using collision_handlers centrally. Additionally, add powerup to generate more coins for the given time frame of powerup. (Dhruv)
    - This can also depend on the difficulty of game
    - So harder difficulties might mean the power up lasts for less time.
- Add achievements of the player (Adarsh)
    - Implement file storage, so the file should be saved even for the current game session while the tab is still open and the server is running.
    - Use the event structure from Priority 3 to communicate that an event happened and act on it in the function pointer that the achievements use
    - Show the achievements in the setting menu
- Additional Powerups (Rayhan)
    - Gravity swap: Power-up that can switch gravity of Jetpack character
    - Add a magnet powerup to the game and associate a gravitational force with all the coin objects relative to the coin powerup collected. Attribute time period for the existence of the magnetic powerup in the game.

## Section 3: Timeline
This section should assign every feature in the previous section to a particular group member and a particular week they will implement it.

Week 1: 
Everyone will implement their Priority 1 Features

Week 2: 
Everyone will implement their Priority 2 Features

Week 3: 
Everyone will implement their Priority 3 and 4 Features

## Section 4: Disaster Recovery
Since every group member has different amounts of time they can spend on this project and reasons they may fall behind, we will deal with this one by one.

If one of us falls behind, the first course of action will be to try and figure it out ourselves. The second course is asking the rest of the team how to help. We will meet up and help this person catch up.

Adarsh is getting a head start on his SURF, so if he were to fall behind, it would be at the beginning and not the end of the week. So, he should be able to get back on track easily. Otherwise, Rayhan can take this since he is more free during the beginning of the week.

Rayhan is in CS 38 and so he spends more time on that class in the middle and end of the week. So, he can pass his work to Adarsh if he falls behind.

Dhruv is also in CS 38 but starts early and works on it at the end of the week as well sometimes, so Adarsh and Rayhan can switch if needed based on when Dhruv needs to work on this.

If any of us swap work or give work to other people, we will ensure that we make up for this in future days or the next week.

We also all have a group chat that we are very active on, and we often ask help from each other on there, so we will continue to use that to do our best so that we do not fall behind.

