
.raCer

this is a preview version of a low-gravity 3d racing game ("raCer") that I wrote in my spare time.

the game is written in "TkScript", my homegrown JIT script engine.
it uses SDL as a HAL and OpenGL for graphics rendering.



0. t a b l e   o f   c o n t e n t s
   ---------------------------------

             1. System Requirements
             2. How to play
             3. The controls
             4. How to make a new track
             5. Acknowledgments
             6. History
             7. License




1. System Requirements:
   -------------------

- Win2000/XP/Vista/Linux
- Decent CPU (>1Ghz)
       (probably much less will do -- on my 2.83Ghz P4 system the game takes ~1-5% CPU resources according to the window task manager !)
- Decent GPU.
       A "cheap" 30e passively cooled GeForce 8500 GT is enough to run the game with 4* multisampling, 1920x1200 @ 60hz.
       The graphics driver must support OpenGL 1.5 (cube mapping and the buffer object extension to be precise).
       The game has so far been tested successfully on a number of typical "office" PCs (equipped with ATI cards).

- **Update 2014**: Ported to OMAP3 based Open Pandora handheld (Cortex A8@1Ghz + SGX533 GPU@200Mhz)



2. How to play:
   -----------

First of all: This is an arcade game and meant to run at 60fps vsync.
              If the game happens to run too slow, try disabling some of the
              "expensive" render paths like shadows ('s') or cube mapping ('lshift-q', for the skybox).
              Resizing the window may help, too.

There are currently two kinds of maps: endless "lap-time record" maps
and "goal" maps.

Your objective is to reach the next checkpoint (the weird looking blue wireframe spheres!).

On a "lap-time record" map, the map resets to the first checkpoint after the last one
has been reached plus the lap time will reset to 0.

A "goal" map can be finished by reaching the last checkpoint (For now, take a look at 
the console output to see the finish time).





3. The controls:
   ------------

(lalt   = left alt key
 lshift = left shift key
 lctrl  = left control key
 )

   Game Keys:
        ESCAPE                   : Show/hide ingame menu
        return / JOY 'START'     : Reset player and map
    left+right / JOY AXIS 0      : Turn left/right
            up / JOY 'X'         : Accelerate
          down / JOY '[]'        : Brake
                 JOY AXIS 2      : Accelerate/Brake
             k / JOY 'R1'        : Boost (if allowed)
         SPACE / JOY 'L1'        : Shield/Walls
           i+o / JOY AXIS 1      : Nose up/down
           j+l / JOY 'L2' + 'R2' : Strafe left/right (both=brake hard)

   Other Keys:
             m              : Toggle OpenGL multi-sampling (default=off)
             q              : Toggle track rendering mode (none, dots, flat, default=textured)
       lctrl-q              : Toggle bilin. texture filter (default=nearest neighbour)
      lshift-q              : Toggle skybox (default=on)
     pgup/down              : Select skybox texture
             s              : Toggle stencil shadows (default=on)
             f              : Toggle fog (default=off)
             0              : (zero), Toggle triangle optimizer (default=on)
             7              : Toggle motion blur (default=on)

   Debug Keys:
            c         : Toggle debug cam (wasd+mouse)
            g         : Toggle draw grid (default=off)
            b         : Draw collision triangles
            x         : Draw force vectors
            z/y       : Draw debug collision spheres
            r         : Elevate player
            t         : Toggle collision detect debug mode (halt at next collision)
            h         : Step one frame (if coll.det. debug mode is active)
            3         : Toggle draw shadow volume (orange), silhouette (pink), normals (green) and light vectors (white)
            4         : Toggle brute force/silhouette shadow mode (default=silhouette)
            HOME      : Toggle direction of texture animation
            8         : Toggle allow boost (default=no boost)
            9         : Toggle joy axis 2 acceleration (default=off i.e. digital accel. via 'O' or 'R1')
            BACKSPACE : Toggle fullscreen (1920x1200)


   Editor Keys:        (** new 30-Nov-2008 **)
                  e  : (ingame), shortcut to invoke the editor
             ESCAPE  : Exit editor and restart race
                TAB  : (ingame), switch to editor and set cursor to last player position
                 F1  : Load "scratch" map from "test.rat"
                 F2  : Next map
                 F3  : Previous map
                 F5  : Start "author" hiscore recording
                 F6  : Start "gold" hiscore recording
                 F7  : Start "silver" hiscore recording
                 F8  : Start "bronze" hiscore recording
                       *** Note : Return to the editor to stop hiscore recording
                F12  : Save "scratch" map to "track to "test.rat"
            w,a,s,d  : Move camera (lshift for "sprint")
             cursor  : Move cursor
  lshift-cursor ^\/  : Move cursor y position (red marker)
              space  : Select nearest entity
        lctrl-space  : Select nearest entity and set camera 
          pgup/pgdn  : Select next/previous entity
                  t  : Move selected entity to cursor position
           lshift-t  : Copy selected (polyline-)entity to cursor position
             insert  : Insert new polyline entity at cursor
      lshift-insert  : Insert new checkpoint entity at cursor
             delete  : Delete selected entity
   lshift-cursor <>  : Inc/dec checkpoint id (if selected entity is a checkpoint)
                  l  : Toggle "goal flag" (if selected entity is a checkpoint)
                  p  : Set player start position/direction to current camera
                  e  : Toggle twist/height/width envelope edit mode
                TAB  : Select next envelope
                  f  : Toggle envelope/path edit mode (select a polyline entity first)
          BACKSPACE  : reset rotation of selected entity
                  i  : Scale selected entity by +5%
                  o  : Scale selected entity by -5%
        
   General editor keys:
               mouse : hold RMB to move cursor, RMB+LMB to move cursor y position
         lctrl-mouse : rotate selected entity, hold RMB to rotate alternate axis

   Envelope and path editor keys:
                 LMB : Select/Move points
              lshift : Hold down to select only control points; also used to zoom "fast"
                 RMB : Insert/delete control point (no comment about the insert code.. :P)
                lalt : Hold down to zoom x/y axis individually
           cursor-<> : Zoom horizontally 
          cursor-^\/ : Zoom vertically (+horizontally if lalt is not held down and path edit mode is active)
           BACKSPACE : Reset path/envelope zoom to (1; 1)


   Open Pandora keys: (**new 2014**)
            d-pad UP : Accelerate
          d-pad DOWN : Brake
          d-pad LEFT : Steer left
          d-pad LEFT : Steer right
            A button : Brake
            X button : Accelerate
            B button : Nose up




4. How to make a new track:
   -----------------------

This is a brief tutorial for the built in level editor.

Note: Please run the game in windowed mode when you edit tracks since some important info is printed
      in the window title !

Basic steps for making a new map:

1) Press "e" while playing a map to enter the editor.

2) Get familiar with the "quake" style camera.
   Keys "w,a,s,d" are used to move forward/left/backward/right, move the mouse to look around.

3) Delete all entities by repeatedly pressing "PAGE_DOWN" and "DELETE" (select next entity, delete selected entity)

4) Move the cursor (the green bar with the red marker that is surrounded by the yellow floor rectangle) to the start of your new map.
   To do that, either use the cursor keys OR hold the "RIGHT MOUSE BUTTON" while moving the mouse. 
   To change the "Y" position of the cursor, hold down the "LEFT MOUSE BUTTON".

4) Press "INSERT" to insert a new "polyline entity". 
   You should now see a new track path.
   You can move the path around by moving the cursor to a new location and then pressing 'T'.
   You can rotate the path by holding down the "LEFT CONTROL KEY" and moving the mouse. (hold the "RIGHT MOUSE BUTTON" to rotate alternate axis).
   You can reset the rotation by pressing "BACKSPACE".
   By pressing the "F" key you can toggle between normal and "path" edit mode. 
   In the path editor, you can change the shape of a track path, add/delete points and adjust the bezier control points.
   Please consider that your path will always be centered around the entity center position that can be changed as described above (move cursor, press 'T').
   Hold down lshift resp. lctrl while modifying a point to change the bezier control points.

5) Set the player start position. 
   To do that, move the camera to where you want it to be, then press "P".

6) It's time for the first play-test: 
   "ESCAPE" restarts the map at the player start position.

7) Repeat steps 4) and 6) and model/play your track. 
   You can also copy the currently selected entity (select entities with "PAGE UP" and "PAGE DOWN" or the mousewheel)
   to the current cursor position by pressing "LEFT SHIFT - T".

8) Each path has 4 modifier envelopes that can be used to modulate
       - The path's width
       - The path's height
       - The path's "twist"
       - The path's border height
   To edit an envelope, select a "polyline entity", then press "E" to toggle between normal and envelope edit mode.
   The "TAB" key is used to select an envelope (width/height/twist/borderHeight). 
   If an envelope is selected, you can use the mouse to drag the points around. 
   Hold down the "LEFT SHIFT" key to only select bezier control points -- this is useful if the points overlap.
   Hold down the "LEFT CONTROL" key to move control points 1 and 2 separately.
   The "RIGHT MOUSE BUTTON" is used to delete the highlighted point or insert a new point.
   The "CURSOR" keys are used to zoom in and out (left/right=zoom x, up/down=zoom y). Hold down "LEFT SHIFT" to zoom fast and
   use "LEFT ALT" to zoom x and y separately.
   You may have noticed another thinner green marker in the editor: This marker shows you the path position corresponding to the current
   horizontal mouse position in the envelope editor.

9) Now it's time for checkpoints:
   Move the cursor to where you want to place a checkpoint, then press "LEFT SHIFT - INSERT" to insert a "checkpoint entity".
   Checkpoints are assigned "id"s which are used to order the checkpoints before the race is started.
   Use "LEFT SHIFT - CURSOR LEFT/RIGHT" to change the "id" of a checkpoint.
   The "L" key is used to toggle a checkpoint's "goal" flag.

10) Do not forget to save your track (press F12 to save it to "maps/test.rat", F1 to re-load it)

11) Record hiscores by pressing F5..F8 in the editor (author, gold, silver, bronze). Return to the editor to stop recording.
    On "goal" maps, reach the goal and then select "Edit map".
    On "fastest lap" maps, set a lap time by racing the track, then press "ESCAPE" and select "Edit map".

12) If you are happy with your new track, make a copy of "maps/test.rat" (after saving, of course) and rename it to e.g. "my_cool_map_name.rat".
    Now edit "mapcycle.txt" and add the filename of your track so you can select it in the game just like the other "preset" maps (use F2/F3!).

13) Send the track file to me so I can playtest it and add it to the standard game distribution :)


Some tips and tricks:
   - I recommend to built a map from start to finish and test-pilot it whenever you made a change.
   - The best maps are often the ones that are easy to finish but hard to set records on
   - Enable the floor grid ('G' key) to improve orientation
   - Better use many short paths instead of few long ones. That makes envelope editing and further changes easier.
   - Be careful to avoid self-intersections in the extruded track paths : Do not make curves too narrow (depends on their width).
   - Use the "twist" envelope to smoothly tilt the track in the direction of a curve. This helps to keep the player "on track".
   - Use the "height" envelope to create smooth hills or even canyons..
   - Be careful of the control points of the start/end point of a path, use "LEFT SHIFT" to drag the control points instead of the center point !
     Degenerate control points can cause "twirls" at the beginning or end of a path.
   - While play-testing the game, use "TAB" to go to the editor and set the cursor to the last player position. This is useful to place
     platforms after long jumps, for example.
   - Use the "LEFT CONTROL + SPACE" key combo to warp the camera to the currently selected entity (especially useful on large maps)
   - To repeat myself: Play-test the map thorougly after making changes !



5. Acknowledgments:
   ---------------

Greetings to Nico (thanks for the nice skybox textures!), Rickmer (thanks for the maps!) and to Nils and Mirko (play-testing).

Last but not least, greetings to the libSDL team (Sam Lantinga et. al) and to Carsten Busse (original author of the tkmath plugin).

Well, and of course greetings to a whole lot of other people -- you know who you are ;).



6. History:
   -------

31-Oct-2008: First track-rendering code
01-Nov-2008: Added collision detection, a sphere can now be moved on the track
02-Nov-2008: Made that three spheres and added some constraints and springs to get a "ship".
             I also added keyboard/joystick (PS2 pad with "center" adapter) controls.
03-Nov-2008: Improved ship handling and collision detection
04-Nov-2008: First release
09-Nov-2008: New camera, fixed collision detection
16-Nov-2008: Again a new, more stable camera, physics and size finetuning. the ship handles
             much better now. also added a "wall forcefield/shield" (read: a trainer ;))
28-Nov-2008: <Started working on the editor>
30-Nov-2008: Added a track/polyline/envelope editor plus 5 demo tracks (bigone, bruchbude, himalaya, speedtrack, twistytrack)
             More physics improvements (e.g. ship slides down slopes now)
01-Dec-2008: Fixed a minor WTF in the sphere constraint physics which could cause a sphere to "bounce" on twisty track segments
02-Dec-2008: Added a cubemap skybox (thanks to Nico for contributing the cube textures !)
             Fixed PS2 pad support (I basically adjusted the left stick vertical axis to the new physics parameters (nose up/down))
12-Dec-2008: Rewrote camera, again. This fixes some glitches and the overall gameplay now feels more "direct" and yet smoother.
             Also improved the editor and added entity rotation and better mouse controls (cursor movement).
             Last but not least, I added a couple of new maps. The maps can be selected with F2 / F3 (next, prev).
14-Dec-2008: Added stencil volume shadow (z-pass) for the ship.
15-Dec-2008: Increased maximum track size (*3). Added some new maps (flatland, bumpy_race) and improved existing ones (speedtrack).
16-Dec-2008: Improved internal physics collision grid => decreases number of triangles tested per frame.
             Added a triangle optimizer that cuts down the triangle count by ~10-50%.
             Fixed some annoying bugs in the editor (cursor position after entity selection) and also added zoom keys to the envelope and path editor.
             Added "mapcycle.txt".
             Improved this "readme.txt". Added a small map-editing tutorial.             
17-Dec-2008: Fixed wall collision mesh (looked like a window shade in the last 2 or three releases :D). 
             Improved/finetuned triangle optimizer (less cracks). The optimizer can now be toggled with the '0' (zero) key, default=on.
             Added fogging which can be toggled with the 'f' key (default=off) (covers far plane clipping but makes a sunny map look like pea soup).
             Analog vs. digital joypad acceleration (axis 2) is now configurable and set to digital by default. Toggle it with the '9' key.
             Added motion blur (really kicks in at high speeds but also adds some kind of anti-aliasing at lower speeds, try the "speed track"). 
             Toggle the motion blur render path with the '7' key (default=enabled).
             Increased the track update speed in the editor (e.g. after moving/rotation an entity) by omitting collision preparation.
             Added '1' and '2' modifiers to the editor/entity rotation: Hold the keys down to prevent rotation about x respectively y axis.
             Improved ship handling: (Slightly) decreased turning ability at very high speeds.
             Added new map "aCCelr8r".
19-Dec-2008: Major change in the physics: Scaled map size by 0.01 (matches camera coordinates now). 
             Slight adjustment/fix to the camera (slightly higher and closer to the ship + I fixed a bug in the camera code that caused the camera to go
             a bit "rubber-band" in the corners of the map) and a very(!) subtle change in the gravity physics.
             Added new maps "crazy eight" and "through the ring" contributed by Rickmer (thanks!).
             Added new map "rings of fury".
             Slightly adjusted motion blur effect (it's slightly more blurry now).
             Fogging is now enabled by default.
20-Dec-2008: Slightly increased non-boost speed and decreased boost speed.
             First implementation of title, mapselect, ingame, mapfinish and mapfailed UI screens. Ingame key 'E' is now used to quickly invoke the editor.
             'ESCAPE' brings up the UI.
21-Dec-2008: Added replays (cannot be saved, yet).
             "author, gold, silver, bronze" hiscores are now saved/loaded with a track. Use F5-F8 in the editor to start hiscore recording.
14-Mar-2014: Optimized for mobile graphics (Open Pandora / SGX533)
29-Mar-2014: Added editor 'i' and 'o' keys; added tracks 'patchwork', 'twisty_track_big', 'grand_prix_big'
30-Mar-2014: Removed wall triangles; optimized track vertices (2 tris instead of 4)



7. License:
   -------

"raCer" is (c) 2008-2014 by Bastian Spiegel.

This package may be freely redistributed as long as it remains unchanged. Rental or sale is strictly prohibited.

Please notice that the "runtime" for this game, TkScript, is distributed under terms of the GNU General Public License. 

The game itself is a platform-agnostic archive file (data.tkx) which means that it will run on every
platform that the runtime is ported to.



Greetings and happy racing,
Bastian

Feel free to contact me at bs at tkscript de !
