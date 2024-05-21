# Game Design Document

## Section 0: Summary
This section should tell the reader what to expect in the future sections.  In particular, it should contain at least:
- a working title for your game
- a list of your team members (and their roles if decided)
- a "concept statement" of your game ("your game in a tweet")

Title: Pong++

Team Members: Adarsh Kumarappan, Dhruv Sheth, Rayhan Zirvi

We do not have discrete roles for each person, but please see the sections below to see how we assigned the features to each person.

Concept: We will take the traditional game of 2 player pong and add new features such as an AI enemy and sound effects.

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

The game starts with the ball at the center and the paddles on the sides. Then, when the game starts, the ball moves randomly in some direction. It can bounce off the top and bottom walls and the paddles. The goal of the game is to have the players use the arrow keys to move their paddle up and down until the ball leaves the other side of the screen behind the opponent’s paddles. Then, that player wins. The ball accelerates even faster as time goes on. See more details below for features. In addition to a 2 player game, we can have different difficulties of the AI enemy as “levels” which the user can choose or as the user does better the AI becomes more difficult and can do things like move faster. Either side gets 5 points when they win. We can allow the player to have the game continue forever and later implement stopping the game when a player gets to some limit like 50 points. For additional features, we’ll also be implementing sound effects to the ball as it undergoes a collision and add a background music for all times. We will also be adding a scrolling environment later in the weeks and sprites for the tables and pong balls.

As mentioned, the right user use the up and down arrow keys to move the paddle up and down and the left user uses s and w keys.

The physics engine will be used in the collisions detection, time dependent ball motion, and drag force mentioned below.

Please see above for the game flow.

We will use sprites for the ball and paddle. Please see below.

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
- Keyboard handler for movement (assuming multiplayer game, we will implement the AI version later) - Dhruv
    - We will keep track of the up and down arrow keys for the right player and the s and w keys for the left player 
- Collision detection / elasticity - Rayhan
    - We can check if the ball collides with the paddle and if so make an elastic collision
    - Can test varying levels of elasticity
- Add background and images to each bodies - Adarsh
    - Assign a list of bodies and initialize images and attribute them to bodies such as pong racquets, pong ball and background.

Priority 2 Features
- Keep track of score as text - Rayhan
    - We can have a score at the top for each of the players
- Implement AI for an enemy- Dhruv
    - We have two options for this:
        - Implement a random position change for x depending on the difficulty level and make the pong for the computer move to the randomized offset position rather than the actual x position. Depending on the difficulty level, the offset could be decreased.
        - Another option is to always make the AI go to the correct x position of the ball, but add extra conveniences based on the difficulty, such as the ability to move left and right as well as being able to move to the ball faster
- Mouse handler for selection of difficulty level - Adarsh
    - Multiplayer or AI game
        - The user can choose in settings of it is a 2 player game or an AI game
    - Difficulty level selection:
        - Select a difficulty level at the start of the game

Priority 3 Features
- Time dependant ball motion - Dhruv
    - We can set a `free for all` gameplay mode where the speed of the ball keeps increasing depending on the time passed or the number of hits against walls or paddles
- Use drag force - Rayhan
    - We can simulate air resistance by using the drag force force creator that exists in force.c
- Add sound effects when hit ball - Adarsh
    - We can add a hitting sound when the ball makes contact with the paddle
    - The frequency of the sound can change based on how fast the ball hits the paddle
    - We can make a “whooshing” sound when the ball leaves the edge of the screen

Priority 4 Features
- Customizable pong board sprite - Dhruv
    - Color for the pong board
- Scrolling environment - Rayhan
    - We can have a cool image for a background that we move based on time of the game being run
- Use background music - Adarsh
    - We can have Mortal Kombat music playing in the background for good vibes 🙂


## Section 3: Timeline
This section should assign every feature in the previous section to a particular group member and a particular week they will implement it.

Week 1: 
Everyone will implement their Priority 1 Features

Week 2: 
Everyone will implement their Priority 2 Features

Week 3: 
Everyone will implement their Priority 3 and 4 Features

## Section 4: Disaster Recovery
This section should describe how each member plans to get back on track if they fall behind.  Please take this section seriously.

Since every group member has different amounts of time they can spend on this project and reasons they may fall behind, we will deal with this one by one.

If one of us falls behind, the first course of action will be to figure it out themselves. The second course is asking the rest of the team how to help. We will meet up and help this person catch up.

Adarsh is getting a head start on his SURF, so if he were to fall behind, it would be at the beginning and not the end of the week. So, he should be able to get back on track easily. Otherwise, Rayhan can take this since he is more free in the beginning of the week.

Rayhan is in CS 38 and so he spends more time on that class in the middle and end of the week. So, he can pass his work to Adarsh if he falls behind.

Dhruv is also in CS 38 but starts early and works on it at the end of the week as well sometimes, so Adarsh and Rayhan can switch if needed based on when Dhruv needs to work on this.

If any of us swap work or give work to other people, we will ensure that we make up for this in future days or the next week.

We also all have a group chat that we are very active on, and we often ask help from each other on there, so we will continue to use that to do our best that we do not fall behind.

