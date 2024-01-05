# a4-4300-cpp
try to clone the assignment 4 game of COMP4300 on Youtube teached by Dave Churchill

**dependencies:**
- SFML 2.6.1
- DearImGui 1.90
- ImGui-SFML 2.6

DEMO

**to-do**

------------------------------------------------------------------------------ 
Program Specification
------------------------------------------------------------------------------ 
This game must have the following features:

**Assets:**
- Entities in the game will be rendered using varius Textures and Animations
  which we will be calling Assets (along with Fonts and Sounds)
- Assets are loaded once at the beginning of the program and stored in the 
  Assets class, which is stored by the GameEngine class
- All Assets are defined in assets.txt, with the syntax defined below 

**Player:**
- The player Entity in the game is represented by Notlink, which has several
  different Animations: RunDown, RunUp, RunRight, StandDown, StandUp, 
  StandRight, AtkUp, AtkDown, AtkRight. You must determine which direction
  and state the player is currently in and assign the correct Animation.
- The Player is assigned the following animations in the direction facing.
  Please note that Left will be accomplished by mirroring Right (x scale -1)
  StandDir  - When no input (or both opposite) is being given to the player
  RunDir    - When move input is given to the player in a direction
  AttackDir - When the player's sword is currently visible from an attack
- The player moves with the following controls:
  Left: A, Right: D, Up: W, Down: S, Attack: Space
- The player can move up, left, down, right, at any time during the game.
- The player can only move eithger horizontally or vertically at a given time
- If opposite directions are held, the player will be given a Stand animation
- The player can only attack once its current attact animation has finished.
- The player collides with 'Tile' entities in the level (see level syntax)
  whose bounding box blocks movements and cannot move through them
- The player will be given a CBoundBox of a size specified in the level file
- The player will be given a CHeath component with a max health that is
  specified in the level file
- When the player's current health is <= 0, the player respawns, and the
  LinkDie sound should play
- If the player has a CInvicibility component, it takes no damage from
  enemies and its sprite is made transparent. The duration of the component
  is reduced each frame, and when the duration reaches 0, the component is
  removed, making it vulnerable to attack again.
- When the player collides with an enemy while not invincible:
  - player's current health is reduced by the enemy damage component
  - the LinkHit sound should player
  - player is given a CInvincibility component which lasts for 30 frames

**Other Entities:**
- Each tile and npc in the game specifies two boolean values of whether it
  blocks movements and whether it blocks vision. If a tile blocks movement,
  the player collides with it and cannot pass through. If an entity blocks
  vision then it affects enemy line of sight. See spec below.
- CBoundBox has two new booleans to reflect thease changes
- When the player steps onto a tile with the "Black" animation, they
  are teleported to an random "Black" tile on the map

**Attacking:**
- When the player attacks, a sword appears for 10 frames and then disappears
  approximately 1 tile away from the player in the direction they are facing.
- The player's sword should be given a bounding box equal to the anim size
- When the sword collides with an enemy, it deals its damage component worth
  of damage to the health of the enemy that it collides with. The sword should
  only do damage for one frame, and then its damage component should be
  removed so that it does not deal damage for every frame it overlaps.
- When the sword is swung, the Slash sound should play
- When the sword collides with an enemy, the EnemyHit sound should play
- When an enemy's health is reduced to 0, it is destroyed
- When an enemy is destroyed, the EnemyDie sound should be play

**NPC Entities:**
- NPC entities will each be given an 'ai' behavior in the level file, either
  'Follow' or 'Patrol'.
- 'Follow' means it will follow the player when the player is direct line of
  sight, or head back to its original position when not in line of sight.
  When an entity is at his home position, it should not oscillate.
- 'Patrol' means it will move toward a series of patrol positions, looping
  back around to the start when the final one is reached. An NPC has reached
  its patrol position if it is within a distance of 5 from it.

**Tiles:**
- Tiles are Entities that define the level geometry and interact with players
- Tiles can be given any Animation that is defined in the Assets file
- Tiles will be given a CBoundingBox equal to the size of the animation
  tile->getComponent<CAnimation>().animation.getSize()
- The current animation displayed for a tile can be retreieved with:
  tile->getComponent<CAnimation>().animation.getName()
- If a tile is given the Heart animation, any NPC that collides with it
  should have its health filed to maxmium, an the tile is destroyed.

**Drawing:**
- The game window is given a fixed size of 1280x768 pixels (20x12 tiles)
- Rendering of entities is provided for you, as well as a 'debug' rendering
  mode which can be toggled with the 'F' key, and the 'R' key toggling
  texture rendering.
- You are required to change the camera position (window.view()) of the game
  window to alternate between two modes based on the m_follow variable:
  true:  Follow Mode - the camera is always centered on NotLink
  false: Room Mode   - the camera is centered on the middle of the room

**Rooms:**
- Each level is split up into individual 'room' given an (rx,ry) coordinate
- Room (rx,ry) is defined by the rectangle (rx*w, ry*h, w, h) where w and h are
  the window width and height respectively.
- Each tile is given a (tx,ty) grid position within a room. By default, the
  grid cells and all tiles have size 64x64 pixels
- When NotLink moves to a new room, the camera should center to that room

**GUI:**
- You must implement the "Animations" and "EntityManager" tabs in ImGui
  so that they have the same functionality as the solution.

**Misc:**
- The 'P' key should pause the game
- The 'C' key toggles debugging view
- The 'T' key toggles textures rendering
- The 'G' key toggles drawing the grid
- The 'Y' key should toggle between follw camera or 'room' camera
- The 'ESC' key should go 'back' to the Main Menu, or quit if on the Main Menu

-------------------------------------------------------------------------------
Config Files
-------------------------------------------------------------------------------
There will be two configuration files in this assignment. The Assets config
file, and the level configuration file.

------------------------------------------------------------------------------ 
Assets File Specification
------------------------------------------------------------------------------ 
There will be three different line types in the Assets file, each of which
correspond to a different type of Asset. They are as follows:

Texture Asset Specification:
Texture N P
  Texture Name     N std::string (it will have no spaces)
  Texture FilePath P std::string (it will have no spaces)

Animation Asset Specification:
Animation N T F S
  Animation Name  N std::string (it will have no spaces)
  Texture Name    T std::string (refer to an existing texture)
  Frame Count     F int (number of frames in the Animation)
  Anim Speed      S int (number of game frames between anim frames)
  
Font Asset Specification:
Font N P
  Font Name     N std::string (it will have no spaces)
  Font FilePath P std::string (it will have no spaces)

Sound Asset Specification:
Sound N P
  Sound Name     N std::string (it will have no spaces)
  Sound FilePath P std::string (it will have no spaces)

------------------------------------------------------------------------------ 
Level Specification File
------------------------------------------------------------------------------ 
Player Specification:
Player X Y BX BY S H
  Spawn Position X Y int, int
  Bounding Box Size BX BY int, int
  Speed S float
  Max Health H int

Tile Specification:
Tile Name RX RY TX TY BM BV
  Animation Name Name std::string
  Room Coordinate RX RY int, int
  Tile Positon TX TY int, int
  Block Movement BM int (1 = true, 0 = false)
  Block Vision BV int (1 = true, 0 = false)

NPC Specification:
  NPC Name RX RY TX TY BM BV H D AI ...
  Animation Name Name std::string
  Room Coordinate RX RY int, int
  Tile Positon TX TY int, int
  Block Movement BM int (1 = true, 0 = false)
  Block Vision BV int (1 = true, 0 = false)
  Max Health H int int > 0
  Damage D int > 0
  AI Behavior Name AI std::string
  AI Parameters ... (see below)

AI = Follow
  ... = S
  Follow Speed S float (speed to follow player)

AI = Patrol
  ... = S N X1 Y1 X2 Y2 ... XN YN
  Patrol Speed S float
  Patrol Position N int (number of patrol positions)
  Position 1-N Xi Yi int, int (Tile Position of Patrol Postion i)

For Example:
NPC Tektite 0 0 15 10 0 0 2 1 Patrol 2 4 15 10 15 7 17 7 17 10
  - Spawn an NPC with animation name Tektite in room (0,0) with tile pos (15,10)
  - This NPC has max health of 2 and damge of 1
  - This NPC does not block movement or vision
  - This NPC has a Patro AI with speed 2 and 4 positions, each in room (0,0)
    Positions: (15,10) (15,7) (17,7) (17,10)
