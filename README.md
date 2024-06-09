# Game Design Document

# Instructions
- Please don't use incognito. Otherwise, the achievements won't be saved.
- Please try on Chrome for best results.
- Please run without asan when testing achievements. Since Asan is so slow, the events may not be received and acted upon before the game ends.

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

The game starts with the player at the bottom left of the screen and the idea is that, instead of moving the player forward, we move the obstacles and background relative to the position and velocity of the player. The main menu of the game allows the player to see their recent stats, highest record so far in that instance of the game, custom sprite store, enter different levels and start playing in that instance of the game. We add several different levels to the game and add a probabilistic complexity to each level of the game. We associate a pseudorandomness to the generation of obstacles and power ups in the game and the velocity. The main controls of the game are the space bar that the user can use to shoot pellets that propel the jetpack character upwards. When they release the space bar, the jetpack character falls towards the floor. The user tries to keep going unlimited times until the user hits the zapper, rocket, or lasers. Then, the user loses and the game over screen shows up. Also, if the user tries the story mode the game will progress based on that like different levels. There are no win conditions but the user can try to achieve the achievements. For point we keep track of number of coins collected and distance covered.

We will use the space bar for the moving up and down of the jetpack.

The physics engine will be used in the collision detection and the coin collecting mechanics mentioned below.

Please see above for the game flow.

We will use sprites for the jetpack character, the zappers, lasers, and rockets. Please see below.


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
- Scrolling background and pseudo-randomly generating bodies for obstacles such as Zappers in the game. We will implement relative motion of the obstacles to the player and ensure that the pseudorandomness allows it to be passable. (Adarsh)
    - So, we will keep the player the same horizontally
    - The player only moves vertically
    - The obstacles and background moves to the left of the player
    - Also, we will make the obstacle randomly span vertically but within some random and acceptable time period so the game is not impossible to beat
    - We also need to handle the part where the objects enter and leave the screen and dynamically destroy all the objects that leave the screen. 
- Porting all the features from the physics engine implemented so far, namely the controls using keyboard and mouse handler, implementing the layout and position of the static bodies in the background and associating assets with bodies. (Dhruv)
    - We also need to port over the collision ability with the zapper and the forces and impulses with things like the zappers
- Home Screen and navigation, ability to navigate to multiple menus and associating a specific menu with objects in that menu. (Rayhan)
    - We will also include a settings UI that will use the features that we will implement in the future like the difficulty level potentially
    - This will also include the game over screen when the user hits the obstacle
    - This is extensible to the rockets and lasers too

Priority 2 Features
- Implement position of Lasers and rockets. Add forces here and associate a specific type of force associated when the main player collides with either of the obstacles. Ensure the compatibility of placement of lasers and rockets with the pseudorandomness of the zappers introduced earlier. Ensure that the speed of the rockets generates is commensurate with the speed of the movement of other obstacles and the background.
    - Also ensure that the rockets are moving to the left (Dhruv)
    - The laser ends should appear on both ends of the screen and then the laser activates a bit later
- Implementation of custom sound effects and background music. Associate a specific type of sound effect with each collision or force creator and have a specific randomly selected music chosen for each game chosen from a preexisting list of music tracks added to the game. Additionally, modify the sound effect frequency depending upon the collision with a specific type of object.  (Adarsh)
    - For example, based on how fast the collsioin with the zapper is, we can change the frequency of the resulting sound
    - Also, we can use custom music or the music from jetpack joyride if we are allowed to
- Add levels to the main menu to select from. The difficulty of the levels could be probabilistic and the generation of each obstacle could be randomly scaled depending on the difficulty of the level. Velocity of the background could be associated with each level and the frequency of generation of different obstacles could be attributed with each level. (Rayhan)
    - For example, the placement of the rockets is probabilistic but can track the user better for more difficult levels

Priority 3 Features
- Add coins to the game. We must generate multiple coins in a cluster depending upon the placement of the obstacles in the game (The main function determining the pseudorandomness of different obstacles added so far could be modified to include the generation of coins). Similar to the game, multiple coins can be generated with a specific shape at different positions. Add a magnet powerup to the game and associate a gravitational force with all the coin objects relative to the coin powerup collected. Attribute time period for the existence of the magnetic powerup in the game. Add different sprites for the coin to add a rotating effect for the coin as it remains on the screen. (Rayhan)
    - We can also update the shape of the coins to be different sizes of blocks probabilistically
    - This can change based on the level difficulty too
    - We can get the magnetic effect with gear icon upgrade that appears in the real game
- To store the previous records and achievements of the player playing the game, we can add a local memory in the game. The way this might work (we can modify this with better ideas later) is to use a text or storage file to store the current stats of the player and the records made so far and add it to the file. When the game is loaded, the text from the file can be rendered in a specific section of the menu. (Adarsh)
    - We can use the text file to keep track of progress toward some achievement
    - For example, we can have an achievement of total number of coins collected and read and write to the text file to update the progress towards this achievement
- Adding power ups to the game. The idea over here is to replicate the powerups in the jetpack joyride game such as boost, disable obstacles for a specific period of time and implement a random weight to the randomness of each powerup depending on the assistance it offers to the player playing the game.  (Dhruv)
    - This can also depend on the difficulty of game
    - So harder difficulties might mean the power up lasts for less time

Priority 4 Features
- Mechanics that are more similar to the real game: Here are some of these things (Adarsh)
    - We want to ensure that as the user presses the space bar, the jetpack moves up AND has bullets that spray downward
    - These bullets should spray in a bit of random direction instead of straight down to be more realistic
    - Also, we want the zappers to rotate at times like the real game
    - We also want multiple lasers to show up at a time and move down and up sometimes
- Gravity swap: Power-up that can switch gravity of Jetpack character (Rayhan)
    - We want to be able to walk on the ceiling and then switch to move down to the ground 
- Custom sprites: Depending upon the local memory feature that we implement, we can also add a menu in the home screen for custom sprites which use the coins collected and distance covered across the levels so far to buy different sprites. Each sprite can change the avatar of the player or the background or both depending on the amount the player spends on the custom sprites. (Dhruv)
    - This can aid customizability so we can easily add things here 



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

