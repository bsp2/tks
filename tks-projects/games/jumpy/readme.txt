"jumpy" -- public alpha preview

** news

  --- 25-Jan-2011 ---

   - some code cleanup


  --- 06-Apr-2010 ---

   - added some secret exits and shortcuts
 
   - ninja rope is now unlocked at the end of world 'c' (or via a hidden exit in world 'a')

   - added room "hidden/funpark"


  --- 05-Apr-2010 ---

   - minor rope physics tuning

   - added new time challenges and alternative exits to some rooms

   - the game progress and best times are now persistant (file "hiscore.dat")


  --- 04-Apr-2010 ---

   - the rope can now be attached to items and (moving) platforms

   - rewrote rope physics to run in one iteration instead of 5, also
     fixed high-framerate bug

   - level timer now starts with first keypress

   - TODO: make some more nice rooms to rope-around in.. :)


  --- 03-Apr-2010 ---

   - added "hook" / ninja rope feature. 

     press 'd' to shoot the hook. when hooked, press UP to shorten the rope length 
      (gives you an upward boost when the rope is long).

     When the rope is short, press DOWN while falling to accelerate, then quickly
     press 'd' to release the rope. takes some practice ;)


  --- 02-Apr-2010 ---

   - more work on the rope physics

   - added room "debug/test_hook" 


  --- 01-Apr-2010 ---

   - first implementation of rope physics


  --- 31-Mar-2010 ---

   - some minor fixes, room "difficult/inverted ride" is now playable again

   - rewrote world selection screen

   - added new challenges for rooms "normal/transport", and "normal/timing"


  --- 30-Mar-2010 ---

   - added global:timescale attribute
   - added global:player_frictionh attribute
   - added global:player_flyfrictionh attribute
   - added global:player_crouchfrictionh attribute
   - added global:player_walkspeedx attribute
   - added global:player_flyspeedx attribute
   - added global:player_crouchjumpspeedx attribute

   - added new room "easy/configuration"


  --- 29-Mar-2010 ---

   - fixed: pressing RETURN on the "get ready" screen for the first room caused a warp 
            to the last room in the world.
 
   - the game speed can now be configured at start. 
     if you choose not to play at normal speed, it may become impossible to meet the 
     (bonus) challenge goals but the game may be easier to play!


  --- 28-Mar-2010 ---
 
   - added "elastic" platforms

   - added time display (for speedruns..)

   - added some (speed-run) challenge goals. Complete a room to find out if you
     have missed any challenge goals !

   - RETURN or SPACE can now be used to replay a level after completing it

   - PAGEUP can now be used to skip rooms that have already been completed

   - PAGEDOWN can now be used to replay the previous room

   - fine-tuned several old rooms

   - added new rooms "difficult/hardcore", and "easy/springtime"


  --- 27-Mar-2010 ---

   - fixed: player could warp through right or left hand side walls when jumping through a semipermeable wall

   - added wall:semipermeablex attribute

   - new feature: parametrizable macros

   - slightly modified rooms "difficult/baddies" and "easy/wall jump"

   - added new rooms "normal/clones", and "normal/upside down"


  --- 26-Mar-2010 ---

   - fixed: player can now properly stand on items when carrying a runner
 
   - fixed: standing on thrown runners did not always work correctly 

   - some short notes concerning yesterdays' new stomp-carry-and-throw-runners feature:

        - if you stomp on the same platform/wall a runner is walking on (gravity must be set to != 0),
          the runner will temporarily turn into a collectable block.
          The runner:revivetime attribute can be used to change the respawn timeout.

        - if you carry a runner, you can walk on collectables (UP items and thrown runners)
        
        - if you carry an UP item, you can jump through other UP items (same like in older versions)

        - thrown runners can be stacked

    - added spike:walkable attribute

    - added new entity shortcuts 'l', 'r', 't', and 'w'

    - added new rooms "normal/cakewalk", "easy/stacks", and "easy/throw"


  --- 25-Mar-2010 ---

   - new feature: player can now temporarily turn runners into collectable blocks and throw them.

   - added runner:collideother attribute

   - added runner:revivetime attribute

   - added new room "normal/stacks"


  --- 24-Mar-2010 ---

   - fixed: high framerate issues:
           + sticky walls
           + non-elastic camera movement
           + cannot move during crouch-jump (room "dont stop")

   - fixed: the "runner" in room "easy/movers" was not semi-permeable causing
            the player to glitch to the right when jumping off at high speeds

   - fixed: camera did not jump to start position after map reset (it was interpolated instead)

   - fixed: camera jumps during rotation could still occur when rotating more than 90 degrees

   - added new room "difficult/mind the gap"


  --- 23-Mar-2010 ---

   - speed-optimized room "easy/ride"
 
   - fixed camera jumps during room rotation.

   - added player:rotate attribute

   - added global:player_nojump attribute
   - added global:player_nowalljump attribute
   - added global:player_nofloat attribute

   - added player:rotate attribute

   - added documentation for §wave and §wheel entities
 
   - added new rooms "normal/rings of fire", and "difficult/points of view"


  --- 22-Mar-2010 ---

   - added "semipermeable" attribute to "runner" and "wall" entities
 
   - added cell detail translation vector (see "about entity placement")

   - rooms can now be rotated (counter-)clockwise

   - entities can now be grouped and animated (wave and wheel formations supported at the moment)

   - added new rooms "easy/stomp", "easy/movers", "easy/rotate", "easy/ride", and "easy/wave"


  --- 21-Mar-2010 ---

   - split maps into "easy", "normal", and "difficult" worlds. the original mapset was moved to "original".

   - added new room "easy/wall jump" 

   - TODO: show world complete message, then return to world selection screen


  --- 20-Mar-2010 ---

   - fixed: setting runner:diry to 1 or -1 via scripts did not work if the it was initially set to 0

   - fixed: mover speedx was added to player speedx even after jumping off.

   - fixed: player could get crushed whilst riding a platform and colliding with a left/right wall.
            platforms that move away from the player cannot crush the player anymore.

   - fixed: after walking off a plattform and falling down, the player was not allowed to float
            (unless floating was not used in the previous jump)

   - fixed: stomping through thin floors when gravity inverted and player is in a corner and stands up 
            (releases DOWN key) should now be fixed for good.

   - finetuned moving platform physics since they felt a bit glitchy in previous versions.

   - added special value "invert" that can be used to change the sign of a runners "dirx", "diry", and "gravity" attributes
   
   - added special value "toggle" that can be used to toggle the state of an entities "active", "visible", a runners "mover" and
     a doors "open" attributes.

   - added world selection and "maps/worlds.txt" world list

   - added 'test_invert_toggle', and 'test_falldown_float' debug maps

   - added new room 'inverted ride' contributed by Larz (and you thought my maps were difficult.. :-))
     (select world 'b' to play it!)


  --- 19-Mar-2010 ---
  
   - fixed: player cannot stomp through (thin) floors anymore when gravity is inverted

   - fixed: switch visible attribute did not work
 
   - fixed: invisible moving blocks were not drawn in edit mode

   - added switch.onlyrunners attribute (see below for details)

   - added 'invisible_switch_test', 'test_wall_stomp_fallthrough', 'test_onlyrunners' debug maps


  --- 18-Mar-2010 ---

   - fixed upper wall collision jitter when character gravity was inverted

   - fixed: entity property set command did not work with floating point numbers,
            the new entity/property separator char is now ':' instead of '.'

   - added "sequencer", "script", and "global" entities (see below)

   - lines starting with '#' in mapcycle.txt are now interpreted as comments

   - added "call", "toggle", "togglemover", "togglevisibility" script actions

   - added new rooms 'sequenced', 'invert', and 'avoid'


  --- 17-Mar-2010 ---

   - Note: if you want to learn about making your own maps for this game, unpack "map_sources.7z"
     and edit/add/delete maps as you like ! See below for a small mapping tutorial.

   - fixed a wall collision bug that prevented the player from being crushed when hitting the ceiling
     while riding on a platform.

   - added "entity.property newvalue" script command. this command is used to assign a new value to
     the property of a given entity.
     for a description of all available entities and their properties, please see the end of this README.

   - added switch.toggle_script property

   - added entity.translate property

   - added.entity.scale property

   - added general entity.visible property. if an entity is invisible, its movement still continues but
     it cannot collide and thus passes through walls (and the player)

   - added new rooms 'mission elevator', 'timing', and 'constellation'


  --- 16-Mar-2010 ---

   - fixed a walljumping issue that prevented you from jumping off a wall
     after sliding down for a longer period of time.

   - I noticed the keyboard rollover issue that makes walljumping harder than it is. 
     I did not find a real fix for that (it's a hardware issue that occurs with cheap keyboards)
     but the fix above makes it a bit more tolerable since it leaves the player more to switch
     between left/right key presses.

   - added joystick support. Any button to jump, left/right to move, down to crouch.


  --- 15-Mar-2010 ---

   - first public alpha preview version released

   - added simple script language that can be used to connect switches with doors and show/hide entities

   - added the rooms "the doors" and "time out".

   - added alternative crouch key 's' to ease crouched wall jumping and also to make it easier to finish
     the 'welcome' map in record time :)


  --- 14-Mar-2010 ---
  
   - first early alpha version released



** basic game controls:

    cursor up: jump
               * press UP after jumping in order to float (reduces falling speed)
                 this allows you to jump over wider gaps
               * when carrying a "jump item", press UP while jumpying to use it
                 this allows you to jump higher

            s:
  cursor down: crouch
               * if you have enough momentum, you can slide through low-ceilinged passages.
               * while falling down, press and hold DOWN to gain speed and stomp on the ground. 
                 - blue blocks can be crashed through at medium falling speeds
                 - red blocks can be crashed through at high falling speeds

  cursor left: move left
               * hold LEFT and UP to slide along a wall left side of the player

 cursor right: move right
               * hold RIGHT and UP to slide along a wall right side of the player

       RETURN: restart map

       INSERT: toggle editor mode (use pageUP/DOWN to select next/previous rooms)
                note: the game must have been started on the console with the following cmd
                       "./jumpy data.tkx -editor" to make this work


** wall jumps:

 you can jump off a wall when sliding down:
   1) first hold LEFT and UP respectively RIGHT and UP (depending on which side of the player the wall is)
      to slide

   2) release all keys, then quickly press UP and RIGHT respectively LEFT (the opposite direction)

 this allows you to climb up narrow passages by continuously jumping back and forth between the left and right walls.

 note: it takes some practice to not lose a jump item while carrying it during a walljump.

 note: if you play this on a bad/cheap keyboard, you may find that walljumping requires a different timing/is not as easy
       as on a proper keyboard like the Cherry RS6000, for example. Also see here <http://en.wikipedia.org/wiki/Rollover_%28key%29>.


** editor controls:

                 SPACE: toggle between editor and game mode
            move mouse: move cursor
 left alt + move mouse: pan / drag map offset (if the level is larger than the screen)
     left mouse button: place foreground ("fg") entity at current cursor position
    right mouse button: place background ("bg") entity at current cursor position	
           mouse wheel: cycle through foreground entity types
                     r: select default fg/bg entities
                  UP/w: select next fg entity
                DOWN/s: select previous fg entity
               RIGHT/d: select next bg entity
                LEFT/a: select previous bg entity
                    F1: save map to file "maps/test.map"
                     m: resize map. after pressing this key, you will be prompted
                        for the new map width / height in the console window (NOT the main window!)
                     n: rename map. after pressing this key, you will be prompted
                        for the new map name in the console window (NOT the main window!)
                DELETE: remove all items and entities (clear)

 the following entities can currently be placed in a map:

  ' '   space
  '#'   solid block
         size = 1x1
  'p'   player start
         size = 1.5x1
  'u'   jump item
         size = 1x1
  's'   small breakable block (stomp on this at medium falling speed to crash through)
         size = 2x1
  'm'   big breakable block (stomp on this at high falling speed to crash through)
         size = 2x1
  'f'   level finish
         size = 2x1
  'k'   killer block
         size = 1x1
  'w'   killer block, can be walked through if player carries an UP item
         size = 1x1
  'y'   semi-permeable platform (bottom)
         size = 1x1
  't'   semi-permeable platform (top)
         size = 1x1
  'l'   semi-permeable platform (left)
         size = 1x1
  'r'   semi-permeable platform (right)
         size = 1x1

 the torquoise 'X' indicates the current player position.

 Use UPPERCASE LETTERS to flag cell positions that can later be used to insert special entities 
 and assign properties / scripts. (for an example, see below).


** creating a new map

 1) create a directory "maps"

 2) create a text file "maps/worlds.txt" and add a line that reads "t:test:my test map info"

    The first char determines which key is used to select this map, 'test' is the name of
    the maps subdirectory, and the string after the last ':' char will be displayed as an
    info message in the world selector.
    The "any key" is used to select the first world in "worlds.txt".

 2) create a directory "maps/test"

 3) create a text file "maps/test/mapcycle.txt" and add a line that reads "test"

 3) create a text file "maps/test/test.map" that has the following content:

########
#      #
#    f #
#p     #
########
;

 4) start the game and play your first map :)


** example maps

 Unpack the archive "map_sources.7z" to this directory and restart the game.


** special map entities / metadata

 In addition to the char array that represents the basic level data, additional
 entities can be placed after the ';' character.

 For example, to make a room that requires the player to flip a switch that spawns
 a moving block and starts a timer that will open a door after 30 frames (0.5 seconds),
 you would do something like this:

################
#              #
#              #
#    A   B C f #
#p             #
################
;
§diz
place your map message here!
it may span multiple lines.
:)

§switch
cell=A
on_script=enable C, start timerB
off_script=stop timerB
toggle=1

§door
cell=B
vertical=1
open=0

§timer
id=timerB
timeout=30
expire_script=open B

§runner
cell=C
dirx=-1
diry=0
gravity=1.0
speed=0.5
mover=1
active=0


 Now place the lines above in a file called "maps/test/demo.map" and add the line "demo"
 to your "maps/test/mapcycle.txt" file so that it looks like this:

demo
test

 Start the game and you should be able to play the new map.

 You can use the builtin editor to edit the basic level layout, then use a text editor
 to add (scripted) entities.


** scriptable entity properties

  types:

    <string>: a single word, for example >>hello<<
     <float>: a floating point number, for example 1.23
       <int>: an integer number, for example 42
    <script>: a sequence of commands, separated by the ',' character.
              the command syntax is:

                 enable <id>
                   open <id>
               activate <id>
                  start <id>
                      1 <id>
           OR

                disable <id>
                  close <id>
             deactivate <id>
                      0 <id>
           OR

             <id>:<property> <value>

           OR

             togglemover <id>

           OR

            togglevisibility <id>
                    showhide <id>

           OR
             call <script_id>


           Examples:
             open mydoor
             close mydoor
             start mytimer
             stop mytimer
             enable mymover
             disable mymover
             myblock:scale 0;6
             mybaddie:gravity -1
             myplatform:speed 0.5
             finish:visible 1, start mytimer

  about entity placement:
   
   Entities can either be placed using the "pos" attribute, or by linking them to
   a named cell (upper case letter) using the "cell" attribute. This attribute
   allows to add a detail translation vector using the following syntax:

      cell=A
      cell=A+1.5;-0.5
      cell=A+3*0;6*2

   More than one entity can be placed at the same cell but be careful to give
   these entities unique identifiers ("id=myid") if you want to use them in scripts.
   The default id of an entity that has been placed using the "cell" attribute includes
   the translation vector.


--Entity: "wall"
    info: a simple, static block.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: pos
    type: <float>;<float>
    def.: <none>
    info: position in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: cell
    type: <uppercase char>
    def.: <none>
    info: used to set the position to the ASCII cell position that contains the same character.
          the cell position can be suffixed by a translation vector in detailed coordinate system, for example "A+1;-0.5".
  
    name: size
    type: <float>;<float>
    def.: 6;6
    info: size of this entity in the detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: translate
    type: <float>;<float>
    def.: <none>
    info: add given translation vector to current position, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: scale
    type: <float>;<float>
    def.: <none>
    info: add the given scaling vector to the current size, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: color
    type: <color>
     def: movers: #ff00ff00 , baddies: #ffff0000
    info: block color

    name: semipermeable
    type: <boolean>
     def: false
    info: true=no collision with bottom side, false=normal collision

    name: semipermeablex
    type: <boolean>
     def: false
    info: true=no collision with left and right sides, false=normal collision


--Entity: "elastic"
    info: A platform/wall attached to an invisible spring.
          The platform starts to move when the player or a runner jumps on it
          and it springs back to its reset position once the entity jumps off
          again.

          Please also see the "wall" attribute list.

     name: sinkspeed
     type: <float>
      def: -0.08
     info: how fast the platform moves when there is an entity on top of it

     name: elasticity
     type: <float>
      def: 0.15 (range 0..1)
     info: how fast the platform springs back to its reset position

     name: decay
     type: <float>
      def: 0.916 (range 0.6..0.99999)
     info: how fast the platform movement speed decays.

     name: range
     type: <float>
      def: 24
     info: how far the platform is allowed to move away from its reset position.


--Entity: "runner"
    info: a moving block that either moves the player or causes a map reset.
          "runner"s use the same physics as the player, except if they are moving vertically.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: pos
    type: <float>;<float>
    def.: <none>
    info: position in detailed coordinate system (one ASCII cell equals a 3x6 block)
          the cell position can be suffixed by a translation vector in detailed coordinate system, for example "A+1;-0.5".

    name: cell
    type: <uppercase char>
    def.: <none>
    info: used to set the position to the ASCII cell position that contains the same character
  
    name: size
    type: <float>;<float>
    def.: 6;6
    info: size of this entity in the detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: translate
    type: <float>;<float>
    def.: <none>
    info: add given translation vector to current position, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: scale
    type: <float>;<float>
    def.: <none>
    info: add the given scaling vector to the current size, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: dirx
    type: <int>
    def.: 0
    info: horizontal moving direction, -1=left, 1=right. 
          the special value "invert" can be used to invert the current direction.

    name: diry
    type: <int>
    def.: 0
    info: vertical moving direction, -1=down, 1=up
          the special value "invert" can be used to invert the current direction.

    name: gravity
    type: <float>
    def.: 1.0
    info: gravity modifier. can be used to make blocks float down. range is 0..1
          the special value "invert" can be used to invert the current direction.

    name: speed
    type: <float>
    def.: 1.0
    info: speed modifier. can be used to make blocks move slower. range is 0..1.
          please set gravity to 0 in order to change the speed of a vertically moving block.

    name: mover
    type: <boolean>
     def: true
    info: controls whether the moving block causes a map reset when the player collides with it (baddies) or
          whether it simply drags the player along its way (platforms, friendlies)
          the special value "toggle" can be used to toggle the current state.

    name: color
    type: <color>
     def: movers: #ff00ff00 , baddies: #ffff0000
    info: block color

    name: active
    type: <boolean>
     def: true
    info: initial state of moving block. true=moving/visible, false=inactive (no collisions, not visible)
          the special value "toggle" can be used to toggle the current state.

    name: turnaround
    type: <boolean>
     def: true
    info: true=turn around when hitting a wall, false=keep going.

    name: semipermeable
    type: <boolean>
     def: false
    info: true=no collision with left, right, and bottom sides, false=normal collision

    name: semipermeablex
    type: <boolean>
     def: false
    info: true=no collision with left and right sides, false=normal collision

    name: collideother
    type: <boolean>
     def: false
    info: true=can collide with other runners

    name: revivetime
    type: <float>
     def: 300
    info: number of frames (60=1 second) after which to re-enable this runner after it has been thrown.
          0=let block remain collectable/rethrowable



---Entity: "door"
    info: a block that can be enabled or disable by switching. the size of the door is automatically
          determined by finding the next non-space block left/right of respectivly above/below the player.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts
 
    name: vertical
    type: <boolean>
     def: true
    info: determines the orientation of the door.

    name: pos
    type: <float>;<float>
    def.: <none>
    info: position in detailed coordinate system (one ASCII cell equals a 3x6 block)
          the cell position can be suffixed by a translation vector in detailed coordinate system, for example "A+1;-0.5".

    name: cell
    type: <uppercase char>
    def.: <none>
    info: used to set the position to the ASCII cell position that contains the same character
  
    name: size
    type: <int>
    def.: 1
    info: width/height of the door in ASCII cells

    name: translate
    type: <float>;<float>
    def.: <none>
    info: add given translation vector to current position, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: scale
    type: <float>;<float>
    def.: <none>
    info: add the given scaling vector to the current size, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: open
    type: <boolean>
    def.: false
    info: initial state of the door.
          the special value "toggle" can be used to toggle the current state.

    name: color
    type: <color>
     def: #ffaaaa00
    info: block color

    
---Entity: "switch"
    info: a block that can triggers scripts when the player collides with it.
          trigger-like switches can be only activated once.
          toggles can be change state again after a short timeout.
          switches can be activated by the player and also by moving blocks.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: pos
    type: <float>;<float>
    def.: <none>
    info: position in detailed coordinate system (one ASCII cell equals a 3x6 block)
          the cell position can be suffixed by a translation vector in detailed coordinate system, for example "A+1;-0.5".

    name: cell
    type: <uppercase char>
    def.: <none>
    info: used to set the position to the ASCII cell position that contains the same character

    name: size
    type: <float>;<float>
    def.: 6;6
    info: size of this entity in the detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: translate
    type: <float>;<float>
    def.: <none>
    info: add given translation vector to current position, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: scale
    type: <float>;<float>
    def.: <none>
    info: add the given scaling vector to the current size, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: color
    type: <color>
     def: #ffffffff
    info: block color

    name: visible
    type: <boolean>
     def: true
    info: initial visibility state of the switch. true=visible, false=invisible.
          invisible switches can be used as triggers.
          the special value "toggle" can be used to toggle the current state.

    name: on
    type: <boolean>
     def: false
    info: initial state of the switch. true=activated, false=deactivated.

    name: toggle
    type: <boolean>
     def: true
    info: true=switch is a toggle. false=switch can only be activated once.

    name: onlyrunners
    type: <boolean>
     def: false
    info: true=switch can only be operated by runners. this can be used to create paths for moving blocks.

    name: on_script
    type: <script>
     def: <none>
    info: this script is run when the switch is activated.

    name: off_script
    type: <script>
     def: <none>
    info: this script is run when the switch is deactivated.

    name: toggle_script
    type: <script>
     def: <none>
    info: this script is run when the on/off state of the switch changes.


---Entity: "timer"
    info: a timer is an invisible entity that, after it has been started, triggers a script after
          a configurable timeout.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: timeout
    type: <int>
    def.: 120
    info: timeout in number of frames. 60=1 second

    name: expire_script
    type: <script>
     def: <none>
    info: this script is run when the timeout expires.

    name: active
    type: <boolean>
     def: true
    info: run state of timer. true=allowed to run/running, false=stopped/halted.
          timers need to be triggered to run.
          the special value "toggle" can be used to toggle the current state.


---Entity: "sequencer"
    info: a sequencer can trigger step scripts in configurable intervals.
          sequencers start automatically and can be halted using the "active"
          property respectively using start/stop commands.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: speed
    type: <float>
    def.: 24
    info: speed in number of frames. 60=1 second

    name: step0 .. step255
    type: <script>
     def: <none>
    info: script to run when step 'n' is reached. 

    name: active
    type: <boolean>
     def: true
    info: run state of sequencer. true=running, false=stopped.
          the special value "toggle" can be used to toggle the current state.


---Entity: "script"
    info: a script entity is used to group frequently used actions.
          the script will be run when it is called from another script.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: actions
    type: <script>
     def: <none>
    info: sequence of actions to perform when the script is run.


---Entity: "up"
    info: a jump item. after collecting it, the player gets one extra jump.
          the item respawns after a short timeout.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: pos
    type: <float>;<float>
    def.: <none>
    info: position in detailed coordinate system (one ASCII cell equals a 3x6 block)
          the cell position can be suffixed by a translation vector in detailed coordinate system, for example "A+1;-0.5".

    name: cell
    type: <uppercase char>
    def.: <none>
    info: used to set the position to the ASCII cell position that contains the same character


---Entity: "spike"
    info: a spike that the player must not collide with or the map will be reset.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: pos
    type: <float>;<float>
    def.: <none>
    info: position in detailed coordinate system (one ASCII cell equals a 3x6 block)
          the cell position can be suffixed by a translation vector in detailed coordinate system, for example "A+1;-0.5".

    name: cell
    type: <uppercase char>
    def.: <none>
    info: used to set the position to the ASCII cell position that contains the same character

    name: size
    type: <float>;<float>
    def.: 3;6
    info: size of this entity in the detailed coordinate system (one ASCII cell equals a 3x6 block).
          if a spike has been placed using a 'k' cell, it will be slightly smaller than 3x6.


---Entity: "finish"
    info: the room is completed when the player collides with this block.

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: pos
    type: <float>;<float>
    def.: <none>
    info: position in detailed coordinate system (one ASCII cell equals a 3x6 block)
          the cell position can be suffixed by a translation vector in detailed coordinate system, for example "A+1;-0.5".

    name: cell
    type: <uppercase char>
    def.: <none>
    info: used to set the position to the ASCII cell position that contains the same character

    name: translate
    type: <float>;<float>
    def.: <none>
    info: add given translation vector to current position, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: scale
    type: <float>;<float>
    def.: <none>
    info: add the given scaling vector to the current size, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: visible
    type: <boolean>
    def.: <true>
    info: initial state of this block. true=visible/reachable, false=invisble/unreachable.
          the special value "toggle" can be used to toggle the current state.

    name: walkable
    type: <boolean>
    def.: <false>
    info: true=player can walk through this spike when carrying an UP item


-- Entity: "wheel"
    info: a rotating formation of entities

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: pos
    type: <float>;<float>
    def.: <none>
    info: position in detailed coordinate system (one ASCII cell equals a 3x6 block)
          the cell position can be suffixed by a translation vector in detailed coordinate system, for example "A+1;-0.5".

    name: cell
    type: <uppercase char>
    def.: <none>
    info: used to set the position to the ASCII cell position that contains the same character

    name: size
    type: <float>;<float>
    def.: 3;6
    info: size of this entity in the detailed coordinate system (one ASCII cell equals a 3x6 block).

    name: translate
    type: <float>;<float>
    def.: <none>
    info: add given translation vector to current position, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: scale
    type: <float>;<float>
    def.: <none>
    info: add the given scaling vector to the current size, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: angle
    type: <float>
    def.: 0
    info: initial rotation angle. 0..360 degrees.

    name: speed
    type: <float>
    def.: 0
    info: rotation speed

    name: members
    type: <string>
    def.: 0
    info: comma separated list of entity ids

    name: active
    type: <boolean>
     def: true
    info: true=rotating, false=frozen
          the special value "toggle" can be used to toggle the current state.


-- Entity: "wave"
    info: a waving formation of entities

    name: id
    type: <string>
    def.: <none>
    info: used to address this entity in scripts

    name: pos
    type: <float>;<float>
    def.: <none>
    info: position in detailed coordinate system (one ASCII cell equals a 3x6 block)
          the cell position can be suffixed by a translation vector in detailed coordinate system, for example "A+1;-0.5".

    name: cell
    type: <uppercase char>
    def.: <none>
    info: used to set the position to the ASCII cell position that contains the same character

    name: size
    type: <float>;<float>
    def.: 3;6
    info: size of this entity in the detailed coordinate system (one ASCII cell equals a 3x6 block).

    name: translate
    type: <float>;<float>
    def.: <none>
    info: add given translation vector to current position, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: scale
    type: <float>;<float>
    def.: <none>
    info: add the given scaling vector to the current size, in detailed coordinate system (one ASCII cell equals a 3x6 block)

    name: angle
    type: <float>
    def.: 0
    info: initial rotation angle. 0..360 degrees.

    name: speed
    type: <float>
    def.: 0
    info: rotation speed

    name: innerspeed
    type: <float>
    def.: 0
    info: inner rotation speed

    name: members
    type: <string>
    def.: 0
    info: comma separated list of entity ids

    name: active
    type: <boolean>
     def: true
    info: true=rotating, false=frozen
          the special value "toggle" can be used to toggle the current state.

    name: type
    type: <string>
     def: "sin"
    info: "sin"=sinusoid animation, "tri"=trianglular, or "smoothstep"

    name: vertical
    type: <boolean>
     def: false
    info: true=vertical formation (animate x positions), false=horizontal formation (animate y positions)


---Entity: "global"
    info: used to change some global settings

    name: player_gravity
    type: <float> -1..1
     def: 1.0
    info: player gravity constant.

    name: player_gravitystompoffset
    type: <float> 0..400
     def: 80
    info: initial stomping time

    name: player_gravitystompscale
    type: <float> -1..1
     def: 1.0
    info: can be used to change boost power when gravity is inverted

    name: player_speed
    type: <float> -1..1
     def: 1.0
    info: player speed scale.

    name: player_alwayscrouch
    type: <boolean>
     def: false
    info: true=room must be played in crouched (jumping dot) mode

    name: player_nojump
    type: <boolean>
     def: false
    info: true=player is not allowed to jump

    name: player_nowalljump
    type: <boolean>
     def: false
    info: true=player is not allowed to jump off walls

    name: player_nofloat
    type: <boolean>
     def: false
    info: true=player is not allowed to doublejump/float


--Entity: "macro"
    info: Macros are used to define a parametrized entity structures.
          The "call" entity is used to insert a macro.
 
          - The first word of a macro is its id (usually on a single line).
          - Entities have to be tagged with '°' instead of '§'
          - a $parameter is replaced by its current value when a macro is called

          See map "normal/clones" or "normal/upside down" for an example.


--Entity: "call"
    info: Used to call a macro.
          The parameter "macro" is used to select a macro by its id.
          All parameters are "passed" to the macro, i.e. a string $test would be
          replaced by the value of parameter "test".


---Entity: "player"
    info: The entity "player" can be used in scripts to change player attributes. 
          Also see the "runner" attribute list.

    name: rotate
    type: <float>
     def: <none>
    info: Used to rotate the center of gravity.
          Accepted values are 
             "cw" (clockwise)
            "ccw" (counterclockwise)
             0..3 (absolute direction, down,right,up,left) 
           -3..+3 (+- prefixed number, rotate relative)

    name: timescale
    type: <float>
     def: 1.0
    info: can be used to slow down the overall game speed

    name: player_frictionh
    type: <float>
     def: 0.79
    info: horizontal friction while walking

    name: player_flyfrictionh
    type: <float>
     def: 0.81
    info: horizontal friction while flying/falling

    name: player_crouchfrictionh
    type: <float>
     def: 0.96
    info: horizontal friction while crouching

    name: player_walkspeedx
    type: <float>
     def: 0.2
    info: walking speed

    name: player_flyspeedx
    type: <float>
     def: 0.2
    info: steering speed while flying/falling

    name: player_crouchjumpspeedx
    type: <float>
     def: 0.05
    info: steering speed while crouch jumping


EOF
