% Synergy MIDI Tracker - Keyboard Shortcuts
% bsp
% 20-Jun-2025
>>>
17Nov2019, 15May2020, 30Oct2020, 19Apr2021, 13May2021, 14May2021, 02Jun2021, 13Feb2022, 24Feb2022, 07May2022, 08May2022, 24Jun2022, 19Nov2022, 04Feb2023, 18Mar2023, 24Mar2023, 23Jun2023, 24Jun2023, 17Aug2023, 31Oct2023, 03Nov2023, 10Nov2023, 14Nov2023, 16Nov2023, 17Nov2023, 19Nov2023, 02Dec2023, 16Dec2023, 02Feb2024, 03Feb2024, 01Oct2024, 05Oct2024, 06Oct2024, 02Nov2024, 07Jan2025, 16Jan2025, 25Jan2025, 26Jan2025, 28Feb2025, 07Mar2025, 09Mar2025, 28May2025, 01Jun2025, 12Jun2025, 13Jun2025, 20Jun2025
<<<
$(var:header)


# Table of Contents
$(buf!toc)

# General

`Synergy` supports keyboard shortcuts for most of its functions. It is highly recommended to memorize at least the most commonly used ones in order to speed up the workflow.

## US keyboard layout

All keyboard shortcuts assume a US keyboard layout.

The key names for german keyboards are stated in the comments.

caution: a real US keyboard has no `<` key (between lshift and z). Please set `UI.b\_key\_map\_slash\_to\_less = true;` in `synergy_config.tks` (default setting). `/` can then be used to e.g. clear events in the tracker editor. This also works on Windows with german or UK keyboards.

## Triadic keyboard shortcuts

`lctrl - x` initiates a triadic keyboard shortcut sequence.

For example, `lctrl - x s` means *press lctrl and x at the same time, release both keys and then press s*.

Holding `lctrl` for around half a second (without pressing a third key) will show a reminder dialog.

Some triadic shortcuts can be repeated after releasing `x` while still holding `lctrl`.

## Page commands

Some pages support additional (text-) commands which may not be accessible in the UI by other means.

Type `lctrl-\`` to set the input focus to the status bar command text field, then type `TAB` or `DOWN` to view a list of available commands.

By right-clicking the command text field, a list of default actions is shown. Holding any modifier key while selecting a default action will execute it immediately.

### Disable dead keys on Linux

As root, edit `/usr/share/X11/xkb/symbols/<your country code>` (e.g. `de`) and replace `dead_acute` by `acute`, `dead_grave` by `grave`, `circumflex` by `asciicircum`.

# Global

| Key Sequence               :|:Description
| lctrl - \`                  | Focuses status bar command text field (`lctrl - ^` on german kbd)
| F3                          | Select previous MIDI morph scene *A*
| F4                          | Select next MIDI morph scene *A*
| F5                          | Select previous MIDI morph scene *B*
| F6                          | Select next MIDI morph scene *B*
| F7                          | Select previous MIDI morph scene *C*
| F8                          | Select next MIDI morph scene *C*
| F9                          | Select previous MIDI morph scene *D*
| F10                         | Select next MIDI morph scene *D*
| lalt - F3                   | Select previous MIDI morph scene *E*
| lalt - F4                   | Select next MIDI morph scene *E*
| lalt - F5                   | Select previous MIDI morph scene *F*
| lalt - F6                   | Select next MIDI morph scene *F*
| lalt - F7                   | Select previous MIDI morph scene *G*
| lalt - F8                   | Select next MIDI morph scene *G*
| lalt - F9                   | Select previous MIDI morph scene *H*
| lalt - F10                  | Select next MIDI morph scene *H*
| lshift - F1                 | Show Audio Recorder dialog (Eureka audio tracks)
| lshift - F2                 | Show Audio LiveRec dialog (Eureka sample live-recording)
|                             |+ (note) `lctrl - a` switches to tracker audio dialog when tracker editor is visible, `lctrl - x c` returns focus to pattern editor
| lshift - F3                 | Show Delay Calculator dialog
| lshift - F10                | Show standard MIDI file (SMF) export dialog
| lshift - F11                | Show Tiny Synergy Replay (TSR) export dialog
| F12                         | Toggle metronome click
| F11                         |
|+lshift - F12                |+Toggle Map-SysEx-Out-Channel-1-events-to-Play-frame (prioritize Eureka sampler playback)
| lctrl - lshift - F12        | Toggle MIDI time code (MTC)
| lctrl - x (NUMPAD) +        | Show BPM dialog
| (NUMPAD) ENTER              | Start / Stop replay
| (NUMPAD) lshift - ENTER     | Start / Stop replay and send / don't send MMC record event(s)
| (NUMPAD) lctrl - x ENTER    | Send "undo" note-on message to DAW (must be mapped in DAW, see `config.midi_daw_remote_ctl_dev`)
| \=                          | Start / Stop replay (alternative key)
|                             |+(`´` on german kbds)
| lshift - \=                 | Start / Stop replay and send / don't send MMC record event(s) (alternative key)
|                             |+(`lshift - ´` on german kbds)
| lctrl - x \=                | Send "undo" note-on message to DAW (must be mapped in DAW, see `config.midi_daw_remote_ctl_dev`)
| (NUMPAD) 1                  | Transport: Go to previous bar
| (NUMPAD) 2                  | Transport: Go to next bar
| (NUMPAD) 3                  | Transport: Toggle cycle/loop
| lctrl - lshift - (NUMPAD) 3 | Transport: Show cycle length menu
| (NUMPAD) 4                  | Transport: Go to previous marker
| (NUMPAD) 6                  | Transport: Go to next marker
| (NUMPAD) 7                  | Transport: Go to previous beat
| (NUMPAD) 9                  | Transport: Go to next beat
| (NUMPAD) /                  | Transport: Decrease cycle length
| (NUMPAD) *                  | Transport: Increase cycle length
| (NUMPAD) .                  | Transport: Save scratch marker
| (NUMPAD) 0                  | Transport: Load scratch marker
| (NUMPAD) 5                  | Transport: Load marker (key 0..9 follows)
| (NUMPAD) 8                  | Transport: Save marker (key 0..9 follows)
| (NUMPAD) lctrl - lshift - 1 | Move to previous clip start (parent arranger)
| (NUMPAD) lctrl - lshift - 2 | Move to next clip start (parent arranger)
| lctrl - lshift - PAGEDOWN   | Move to previous clip start (parent arranger)
| lctrl - lshift - PAGEUP     | Move to next clip start (parent arranger)
| (NUMPAD) -                  | Decrease BPM (beats per minute)
| (NUMPAD) lshift - -         | Half BPM (beats per minute)
| (NUMPAD) +                  | Increase BPM (beats per minute)
| (NUMPAD) lshift - +         | Double BPM (beats per minute)
| (NUMPAD) lctrl - 1          | Show pipemap page
| (NUMPAD) lctrl - 2          | Show preferences page
| (NUMPAD) lctrl - 3          | Show current node editor page
| (NUMPAD) lctrl - 4          | Show MIDI devices setup page
| (NUMPAD) lctrl - 5          | Show project page
| (NUMPAD) lctrl - 6          | Show MIDI map page
| (NUMPAD) lctrl - 7          | Show piperoot page
| (NUMPAD) lctrl - 8          | Show current pipe page
| (NUMPAD) lctrl - 9          | Show audiotool page
| (NUMPAD) lctrl - 0          | Show SysEx page
| (NUMPAD) lctrl - ENTER      | Show MIDI input monitor page
| (NUMPAD) lctrl - +          | Show MIDI output monitor page
| (NUMPAD) lctrl - -          | Show SubSongs page
| (NUMPAD) lctrl - x 0        | Select multi-scene 1 OR subsong 0 (main) [see `config.b_lctrlx_kp_select_multimorph`]
| (NUMPAD) lctrl - x 1        | Select multi-scene 2 OR subsong 1 (main) [see config]
| (NUMPAD) lctrl - x 2        | Select multi-scene 3 OR subsong 2 (main) [see config]
| (NUMPAD) lctrl - x 3        | Select multi-scene 4 OR subsong 3 (main) [see config]
| (NUMPAD) lctrl - x 4        | Select multi-scene 5 OR subsong 4 (main) [see config]
| (NUMPAD) lctrl - x 5        | Select multi-scene 6 OR subsong 5 (main) [see config]
| (NUMPAD) lctrl - x 6        | Select multi-scene 7 OR subsong 6 (main) [see config]
| (NUMPAD) lctrl - x 7        | Select multi-scene 8 OR subsong 7 (main) [see config]
| (NUMPAD) lctrl - x 8        | Select multi-scene 9 OR subsong 8 (main) [see config]
| (NUMPAD) lctrl - x 9        | Select multi-scene 10 OR subsong 9 (main) [see config]
| b                           | Go to previous page (back)  (**deprecated**)
| n                           | Go to next page (forward)  (**deprecated**)
| q                           | Go to previous or parent page  (**deprecated**)
| g                           | Force global MIDI map context (as long as key is held down)
| lctrl - m                   | Edit current MIDI map context
| lctrl - s                   | Toggle pipe solo
| lctrl - d                   | Toggle node solo
| lctrl - lshift - d          | Toggle node mute
| right menu key              | Toggle keyboard focus selection mode
| \`: (once)                  | Edit controller aliases
|                             |+(`^` on german kbd)
| \`: (twice)                 | Edit current MIDI morph scene
|                             |+(`^` on german kbd)
| RIGHT MENU KEY              | Toggle keyboard focus selection mode
| ESC                         | Go to previous or parent page / cancel dialog
| lctrl - x ESC               | Open "stop" menu
| lctrl - x x                 | Bring *Eureka* window to front (if process is running)
| lshift - q                  | Quit application (dialog)
].kbd_table Global keyboard shortcuts


# Bezier Edit

| Key Sequence               :|:Description
| lshift modifier             | Scale ctlpoint length
| lctrl modifier              | ctr-point selected: Shrink/Expand surrounding points
| lctrl modifier              | ctl-point selected: Toggle symmetry lock
| lalt + mouse drag           |:
| MMB + mouse drag            |+Pan (when zoomed in)
| lctrl - a                   |:
| RMB hold                    |+
| lctrl + RMB                 |+
| right windows key           |+Show context menu
| LMB+RMB hold + mouse drag   | Zoom x/y (hold LMB for 0.25s, then also hold RMB, then drag mouse)
| lctrl - y                   | Undo (`lctrl - z` on german kbd)
| lctrl - lshift - y          | Redo (`lctrl - lshift - z` on german kbd)
| lctrl - SPACE               | Reset Zoom
| lctrl - lshift-SPACE        | Toggle Draw Envelope ("true 2d mode")
| mouse wheel                 | Select previous / next point
| lctrl - mouse wheel         | Select previous / next center point
| LEFT                        | Select towards-previous ctl-point
| RIGHT                       | Select towards-next ctl-point
| UP                          | Select next center point
| DOWN                        | Select previous center point
| lalt - wheel UP             | 
| lctrl - e                   | Toggle drag+rotate mode (bend curve segments by LMB-dragging them)
|+i                           |+Zoom In X
| lalt - wheel DOWN           | 
|+u                           |+Zoom Out X
| lalt - lctrl - wheel UP     | 
|+o                           |+Zoom In Y
| lalt - lctrl - wheel DOWN   | 
|+p                           |+Zoom Out Y
| DELETE                      | Delete selected point(s)
| f                           | Flip X
| lshift - f                  | Flip Y
| lctrl - f                   | Toggle draw-fill
| lctrl - lshift - f          | Make seamless (align last to first)
| g                           | Toggle Grid
| lshift - g                  | Toggle Grid Subdivision
| m                           | Toggle Smooth-Edit Mode
| lshift - m                  | Toggle Smooth-Edit XDist only (when enabled, ignore y-distance)
| lctrl - m                   | Toggle Smooth-Edit Ctl or Ctr Point Distance (when enabled, scale control points)
| n                           | Smooth All (10%)
| lshift - n                  | Smooth All (5%)
| lctrl - n                   | Smooth All (25%)
| lctrl - lshift - n          | Smooth All (50%)
| lctrl - q                   | Simplify (coarse)
| lctrl - lshift - q          | Simplify (fine)
| lctrl - r                   | Make all control points with angle < 45 degrees symmetrical
| lctrl - lshift - r          | Make all control points symmetrical
| lctrl - b                   | Set current envelope as backdrop
| lctrl - lshift - b          | Clear backdrop
| s                           | Toggle snap-to-grid
| lctrl - t                   | Trim curve to selection
].kbd_table Bezier Edit keyboard shortcuts



# Pages

## PipeMap

| Key Sequence       :|:Description
| UP                  | Select previous pipe
| DOWN                | Select next pipe
| LEFT                | Select previous pipe or node
| RIGHT               | Select next pipe or node
| lshift - HOME       | Select first node
| lshift - END        | Select last node
| lctrl - LEFT        | Select current pipe
| lctrl - RIGHT       | Select last node in current pipe
| PAGEUP              | Select first node
| PAGEDOWN            | Select last node
| lshift - PAGEUP     | Select next action
| lshift - PAGEDOWN   | Select previous action
| 1                   | Select *EXEC* action
| 2                   | Select *EDIT* action
| 3                   | Select *MUTE* action
| 4                   | Select *SOLO* action
| 5                   | Select *MOVE* action
| r                   | Show pipe root
| a                   | Show pipe or node context menu
| lctrl - a           | Show root context menu
| s                   | Toggle pipe or node solo
| lctrl - s           | Toggle pipe solo
| d                   | Toggle pipe or node mute
| lctrl - d           | Toggle pipe mute
| e                   | Edit current pipe
| INSERT              | Insert new pipe or node
| lshift - INSERT     | Clone pipe or node
| DELETE              | Delete pipe or node
| lshift - LEFT       | Move node to the left
| lshift - RIGHT      | Move node to the right
| lshift - UP         | Move pipe up
| lshift - DOWN       | Move pipe down
| w                   | Select next instrument-type node
| lshift - a          | Select next arranger-enabled node
| f                   | Select next arranger node
| lctrl - lshift - c  | Edit node comment
| HOME                | Select next used node pattern
| END                 | Select previous used node pattern
| (NUMPAD) lshift - 1 | Select used node pattern #1
| (NUMPAD) lshift - 2 | Select used node pattern #2
| (NUMPAD) lshift - 3 | Select used node pattern #3
| (NUMPAD) lshift - 4 | Select used node pattern #4
| (NUMPAD) lshift - 5 | Select used node pattern #5
| (NUMPAD) lshift - 6 | Select used node pattern #6
| (NUMPAD) lshift - 7 | Select used node pattern #7
| (NUMPAD) lshift - 8 | Select used node pattern #8
| (NUMPAD) lshift - 9 | Select used node pattern #9
| (NUMPAD) lshift - 0 | Select empty pattern (`\-\-\/`)
| RCTRL               | Arm node for recording (press again to disarm)
| RSHIFT              | (Re-)Start global recording (armed nodes). When recording is already active, undo + restart.
| lctrl - y           | Undo
|                     |+(`lctrl - z` on german kbd)
| lctrl - x RCTRL     | Toggle global recording pre-count
| lctrl - x t         | Show new-node dialog
| t                   | Show new-node dialog (preselect [current pipe])
| lctrl - x k         |
|+lctrl - x l         |+Toggle node keyjazz lock
| l                   | Toggle auto-lock keyjazz focus
| lctrl - TAB         | Show SysEx editor page
].kbd_table PipeMap Page keyboard shortcuts


## Pipe

| Key Sequence          :|:Description
| lctrl - lshift - UP    | Show previous pipe
| lctrl - lshift - DOWN  | Show next pipe
| a                      | Toggle show-in-arranger state of selected node
| d                      | (Un-)mute selected node
| s                      | (Un-)solo selected node
| r                      | Show pipe root
| e                      | Edit selected node
| q                      | Auto-assign automation port A from node class (show class selector)
| INSERT                 | Create new node
| lshift - INSERT        | Clone selected node
| DELETE                 | Delete selected node
| lctrl - lshift - c     | Toggle remark editor
| lshift - n             | Set keyboard focus to pipe name widget
].kbd_table Pipe Page keyboard shortcuts



# Nodes

## Common

| Key Sequence          :|:Description
| lctrl - lshift - LEFT  | Select previous node
| lctrl - lshift - RIGHT | Select next node
| lctrl - lshift - UP    | Select previous pipe
| lctrl - lshift - DOWN  | Select next pipe
| lctrl - g              | Enter pattern number
| HOME                   | Select next edit+play pattern (+1)
| lctrl - HOME           | Select next edit+play pattern group (+4)
| lctrl - lshift - HOME  | Select next used pattern
| END                    | Select previous edit+play pattern (-1)
| lctrl - END            | Select previous edit+play pattern group (-4)
| lctrl - lshift - END   | Select previous used pattern
| lctrl - x 1            | Set pipenode *A*
| lctrl - x 2            | Set pipenode *B*
| lctrl - x d            | Toggle pattern manager
| lctrl - x z            | Jump between pipenodes *A* / *B*
|                        |+(`lctrl - x y` on german kbd)
| lctrl - x END          | Global program change (master pipe, auto dev+ch A)
| lctrl - x HOME         | Global program change (master pipe, auto dev+ch B)
| lctrl - lshift - c     | Edit node comment
| lctrl - x u            | Focus node name widget
| lctrl - c              | Copy pattern to clipboard
| lctrl - w              | Cut pattern to clipboard
| lctrl - v              | Paste pattern clipboard
| lshift - n             | Edit pattern name (focus textfield widget)
| lctrl - r              | Clone pattern (4)
| lctrl - lshift - r     | Clone pattern (1)
| lctrl - x F3           | Delete pattern
| lctrl - s              | Solo pipe
| lctrl - d              | Solo node (within pipe)
| lctrl - lshift - s     | Mute pipe
| lctrl - lshift - d     | Mute node
| (NUMPAD) lshift - 1    | Select used node pattern #1
| (NUMPAD) lshift - 2    | Select used node pattern #2
| (NUMPAD) lshift - 3    | Select used node pattern #3
| (NUMPAD) lshift - 4    | Select used node pattern #4
| (NUMPAD) lshift - 5    | Select used node pattern #5
| (NUMPAD) lshift - 6    | Select used node pattern #6
| (NUMPAD) lshift - 7    | Select used node pattern #7
| (NUMPAD) lshift - 8    | Select used node pattern #8
| (NUMPAD) lshift - 9    | Select used node pattern #9
| (NUMPAD) lshift - 0    | Select empty pattern (--/)
| lctrl - x BACKSPACE    | Toggle ignore seek / prgchg / mute (per-node setting)
].kbd_table Common Node Editor keyboard shortcuts


## Arranger

### General

| Key Sequence          :|:Description
| 1                      | Select *Scene* tab
| 2                      | Select *Pattern* tab
| 3                      | Select *Timeline* tab
| 4                      | Select *Play* tab
].kbd_table Arranger Node Editor: General keyboard shortcuts

### Timeline Tool Selection
| Key Sequence          :|:Description
| q                      | Select *Select* / *Move* tool
| w                      | Select *Draw* tool (+ create new clip in grid cursor mode)
| lctrl - lshift - w     | Toggle 'draw new audio sample' mode
| e                      | Select *Erase* tool  OR  edit first select clip init ctl (if clip edit mode != default)
| r                      | Select *Razor* tool OR split clip at cursor position if tool is already selected
| t                      | Select *Mute* tool OR toggle clip mute at cursor position if tool is already selected
].kbd_table Arranger Node Editor: Timeline Tool Selection keyboard shortcuts

### Timeline (Common)

| Key Sequence                 :|:Description
| RETURN                        | Edit first selected clip (same as RMB-click) (depends on clip edit mode)
| lshift - RETURN               | Edit sample associated with audio clip (same as MMB-click) (Eureka sample page)
| 1 - 9 / 0 / - / .             | Edit initial clip controller value
| CURSOR keys                   | Select clips (+shift to add to selection, +lctrl to select on all tracks)
| lshift - CURSOR               | Expand selection
| lctrl - x e                   | Toggle edge-selection mode (move to start/end before selecting prev/next clip)
| x                             | Toggle grid-cursor / selection-cursor keyboard modes
| lctrl - LEFT                  | Grid cursor mode: move cursor left (modulo 4)
| lctrl - RIGHT                 | Grid cursor mode: move cursor right (modulo 4)
| lctrl - LEFT                  | Selection cursor mode: Go to marker clip start or previous marker
| lctrl - RIGHT                 | Selection cursor mode: Go to next marker
| lctrl - lalt - LEFT           | Go to marker clip start or previous marker
| lctrl - lalt - RIGHT          | Go to next marker
| lctrl - c                     | Copy selected clips to clipboard
| lctrl - v                     | Paste clips from clipboard (overwrite)
| lctrl - lshift - v            | Paste clips from clipboard (insert silence / move following clips)
| z                             | Switch between marker track and regular track
|                               |+(`y` on german kbd)
| lshift - z                    | Y-Expand selection (toggle)
|                               |+(`lshift - y` on german kbd)
| lalt - z                      | Select clips until end (press again to toggle between current track / all tracks)
|                               |+(`lalt - y` on german kbd)
| lctrl - UP                    | Y-Expand selection (upper becomes cursor track)
| lctrl - DOWN                  | Y-Expand selection (lower becomes cursor track)
| lctrl - LEFT / RIGHT          | Selection-cursor mode: select left / right clips on all tracks
| lctrl - lshift - LEFT / RIGHT | Grid-cursor-cursor mode: expand selection left / right and align (modulo 4)
| INSERT                        | Insert silence
| lshift - INSERT               | Insert silence (all tracks)
| DELETE                        | Delete selected clips or remove silence when there's no selection
| lshift - DELETE               | Delete selected clips and move OR delete on all tracks in grid-cursor mode
| lctrl - DELETE                | Delete selected clips (muted only)
| lctrl - lshift - DELETE       | Delete selected clips and move (muted only)
| lctrl - a                     | Select all track clips (when clip is selected), then all marker section clips or all clips
| lshift - a                    | Select all clips within marker section (if any)
| lctrl - lshift - a            | Select none
| lctrl - x a                   | Set selected clip lengths to pattern lengths
| c                             | Loop selected clip(s)
| lctrl - x l                   | Set left loop locator
| lctrl - x r                   | Set right loop locator
| lshift - c                    | Set cursor to loop start / end
| f                             | Center display to replay offset (follow)
| lctrl - lshift - f            | Cycle through auto-follow modes (off, paged, smooth)
| lctrl - f                     | Toggle zoom to selected clip(s)
| lshift - f                    | Toggle show-all
| lctrl - x f                   | Toggle Force (override multi-clip properties) mode
| g                             | Choose pattern for selected clip(s) (popup)
| lctrl - g                     | Clear audio clip sample + queue audio recording (liverec-enabled audio clip sample)
| lctrl - lshift -g             | Queue audio recording (liverec-enabled audio clip sample)
| d                             | Toggle track mute (last selected clip track)
| lshift - d                    | Toggle mute all tracks
| s                             | Toggle track solo (last selected clip track)
| l                             | Toggle track lock (last selected clip track)
| lshift - l                    | Invert track locks
| lctrl - l                     | Clear track locks
| lctrl - h                     | Align selected clips to grid
| m                             | Show / hide marker track
| lshift - m                    |
|+lshift - w                    |+Create marker clip for selection
| n                             | Edit clip name (focus textfield)
| p                             | Select next play mode (cycle)
| lshift - p                    | Select previous play mode (cycle)
| lshift - s                    | Toggle snap-to-grid
| v                             | Insert snapshot of currently playing pattern (current track)
| lshift - v                    | Insert snapshot of currently playing patterns (all tracks)
| lctrl - n                     | Insert unused pattern (group) at cursor position (or after last selected clip)
| lctrl - lshift - n            | Insert unused pattern (sub) at cursor position  (or after last selected clip)
| lctrl - r                     | Clone last selected clip + pattern
|                               |+note: same as lshift-r (clone marker section) when all clips in section are selected
| lctrl - lshift - r            | Clone last selected clip + pattern (sub)
| lshift - r                    | Clone current marker section and assign new marker pattern nr
| lshift - HOME                 | Go to start (offset 0)
| lshift - END                  | Go to loop start
| lctrl - HOME                  | Increment pattern nr of selected clips by +4
| HOME                          | Increment pattern nr of selected clips by +1
| lctrl - END                   | Decrement pattern nr of selected clips by -4
| END                           | Decrement pattern nr of selected clips by -1
| lctrl - lshift - HOME         | Select next used pattern nr (selected clips)
| lctrl - lshift - END          | Select previous used pattern nr (selected clips)
| SPACE                         | Start / stop replay
| lctrl - SPACE                 | Stop replay and don't rewind to start offset
| lshift - SPACE                | Queue play position (same as LMB-dblclick)
| RCTRL                         | Toggle clip mute+patternr state recording
| lshift - RCTRL                | Copy clip mute+patternnr state from arranger (at edit offset)
| a                             | Show tools and options menu
| lalt - a                      | Focus grid size widget
| lalt - s                      | Focus canvas size widget
| lshift - [                    | Zoom out (WHEEL up)
|                               |+(`lshift - ü` on german kbd)
| lshift - ]                    | Zoom in (WHEEL down)
|                               |+(`lshift - +` on german kbd)
| lctrl - ]                     | Increase track size
| lctrl - lshift - WHEELUP      |+(`lshift - +` on german kbd)
| lctrl - /                     | Decrease track size
| lctrl - lshift - WHEELDOWN    |+(`lshift - -` on german kbd)
| lctrl - x s                   | Toggle main / scratch timelines
| lshift - e                    |
|+ k                            |+Edit audio-clip sample in Eureka
| PAGEUP                        | Select next clip edit mode (initial controller)
| PAGEDOWN                      | Select previous clip edit mode (initial controller)
].kbd_table Arranger Node Editor: Timeline Common keyboard shortcuts



## Tracker

### Edit

| Key Sequence          :|:Description
| lalt - e               | Open Edit pulldown menu
| rctrl                  | Toggle edit mode
| backspace              | Toggle relative shift mode
| lshift - ]             | Zoom in
|                        |+(`lshift - ü` on german kbd)
| lshift - [             | Zoom out
|                        |+(`lshift - +` on german kbd)
| ' or f                 | Toggle zoom preset
|                        |+(`ä` on german kbd)
| lshift - a             | Enter step resolution / zoom (focus widget)
| lshift - d             | Enter default note duration  (focus widget)
| lshift - g             | Enter step number
| lctrl - 0              | Set step advance to 0 steps
| lctrl - 1              | Set step advance to 1 step
| lctrl - 2              | Set step advance to 2 steps
| lctrl - 3              | Set step advance to 3 steps
| lctrl - 4              | Set step advance to 4 steps
| lctrl - 5              | Set step advance to 5 steps
| lctrl - 6              | Set step advance to 6 steps
| lctrl - 7              | Set step advance to 7 steps
| lctrl - 8              | Set step advance to 8 steps
| lctrl - 9              | Advance to next event after editing a step
| lctrl - lshift - 0     | Set micro-shift to 96 ticks
| lctrl - lshift - 1     | Set micro-shift to 1 tick
| lctrl - lshift - 2     | Set micro-shift to 2 ticks
| lctrl - lshift - 3     | Set micro-shift to 3 ticks
| lctrl - lshift - 4     | Set micro-shift to 4 ticks
| lctrl - lshift - 5     | Set micro-shift to 6 ticks
| lctrl - lshift - 6     | Set micro-shift to 8 ticks
| lctrl - lshift - 7     | Set micro-shift to 12 ticks
| lctrl - lshift - 8     | Set micro-shift to 24 ticks
| lctrl - lshift - 9     | Set micro-shift to 48 ticks
| lctrl - x a            | Select instrument under cursor OR show audio dialog
|                        |+ (note) `lctrl - a` switches to AudioLiveRec dialog
| lctrl - x o            | Toggle chord clear-step mode
| lctrl - x t            | Start / stop tap mode
| lctrl - t              | Collapse pattern to scratch and start tap mode
| lctrl - u              | Copy controller port+type to ModMatrix Ctl.Clipboard A
|+lshift - 1             |+ (when pattern edit mode is off)
| lctrl - lshift - u     | Copy controller port+type to ModMatrix Ctl.Clipboard B
|+lshift - 2             |+ (when pattern edit mode is off)
| lctrl - f              | Detail-edit value
| lctrl - z              | Toggle note on / off or detail-edit value
|                        |+(`lctrl - y` on german kbd)
| lctrl - \\ (UK kbd)    | Toggle timing view
|                        |+(`lctrl - <` on german kbd)
|                        |+(`lctrl - -` on german kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
|                        |+(`lctrl - /` on US kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
| lctrl - lshift - \\ (UK kbd) | Toggle timing view move lock
|                        |+(`lctrl - lshift - <` on german kbd)
|                        |+(`lctrl - lshift - -` on german kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
|                        |+(`lctrl - lshift - /` on US PC kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
| lctrl - y              | Undo
|                        |+(`lctrl - z` on german kbd)
| lctrl - lshift - y     | Redo
|                        |+(`lctrl - lshift - z` on german kbd)
| lctrl - lshift - TAB   | Toggle envelope view
| lctrl - lshift - x     | Toggle focus envelope view edit mode widget
| lctrl - x TAB          | Toggle pattern / play / arp views
| lctrl - x BACKSPACE    | Toggle ignore seek / prgchg / mute mode
| lctrl - x INSERT       | Toggle follow-replay edit mode
| lctrl - x DELETE       | Swap lshift INSERT/DELETE behaviour
| lctrl - x \`           | Toggle piano widget
|                        |+(`lctrl - x - ^` on german kbd)
| lctrl - x 0            | Show Note Randomizer dialog
].kbd_table Tracker Node Editor: Edit keyboard shortcuts


### Cursor

| Key Sequence          :|:Description
| lalt - c               | Open Cursor pulldown menu
| lctrl - x q            | Toggle autoscroll mode
| lctrl - x r            | Toggle wrap-around mode
| lshift - TAB           | Move cursor to previous track
| TAB                    | Move cursor to next track
| lctrl - LEFT           | Move cursor to first cel or previous track
| lctrl - RIGHT          | Move cursor to next cel or next track
| UP                     | Move cursor to previous row
| PAGEUP                 | Move cursor *n* rows up and align
| lctrl - UP             | Move cursor *n* rows up
| DOWN                   | Move cursor to next row
| PAGEDOWN               | Move cursor *n* rows down and align
| lctrl - DOWN           | Move cursor *n* rows down
| lshift - HOME          | Move cursor to first row
| lshift - END           | Move cursor to last row
| LEFT                   | Move cursor to previous cel
| RIGHT                  | Move cursor to next cel
| lctrl - lalt - LEFT    | Move cursor to previous column group
| lctrl - lalt - RIGHT   | Move cursor to next column group
| lctrl - PAGEUP         | Move cursor to previous event
| lctrl - PAGEDOWN       | Move cursor to next event
| lctrl - x SPACE        | Show cel select / edit / add / delete dialog
].kbd_table Tracker Node Editor: Cursor keyboard shortcuts


### Replay

| Key Sequence          :|:Description
| lalt - r               | Open Replay pulldown menu
| lctrl - RETURN         | Set replay pattern (and update current replay loop)
| RETURN                 | Play step OR undo+restart recording
| lshift - RETURN        | Play note cel and cycle
| SPACE                  | Start / stop pattern replay OR start block loop (when block is selected)
| lctrl - SPACE          | Start / stop pattern replay from current row
| lctrl - x w            | Toggle stop-and-goto-bookmark mode
| lshift - SPACE         | Set (editor) pattern replay start offset OR queue block loop (during block replay)
| lctrl - lshift - SPACE | Toggle ignore-prgchg and follow-replay-pattern (*follow arranger*)
| RSHIFT                 | Start recording (press `RETURN` to undo+restart)
].kbd_table Tracker Node Editor: Replay keyboard shortcuts


### Track

| Key Sequence          :|:Description
| lalt - t               | Open Track pulldown menu
| lctrl - x s            | Edit track layout
| lctrl - x i            | Add cel
| lctrl - x n            | Add note cel
| lctrl - x p            | Toggle all VEL and DUR ctl styles (bar4<>hex, hi_nibble<>hex)
| lctrl - x [            | Toggle alternative cel styles
|                        |+(`lctrl - x ü` on german kbd)
| lshift - e             | Reverse track
| lshift - o             | Rotate track up
| lshift - p             | Rotate track down
| lctrl - x l            | Lock keyjazz to current track and node (or release it)
| lctrl - x k            | Auto-colorize cels (preserves custom cel colors)
| \\                     | Insert all-track-notes-off special note (`===`)
|                        |+(`#` on german and UK kbds)
| lshift - \\            | Insert all-MIDI-channel-notes-off special note (`\#\#\#`)
|                        |+(`lshift - #` on german and UK kbds)
| k                      | Edit track sound (SysEx, NodeAnalogRytm, or Eureka sample / plugin)
].kbd_table Tracker Node Editor: Track keyboard shortcuts


### Pattern

| Key Sequence          :|:Description
| lalt - p               | Open Pattern pulldown menu
| lctrl - n              | Create new pattern
| lctrl - lshift - n     | Create new pattern (sub)
| lctrl - r              | Clone pattern
| lctrl - lshift - r     | Clone pattern (sub)
| lctrl - x F3           | Free pattern
| lshift - BACKSPACE     | Clear pattern
| lctrl - x c            | Start output capture (aligned to bar start, stops when replay is stopped)
| lctrl - x v            | Paste output capture buffer to current pattern  (note: buffer is shared between all nodes)
| lctrl - x \\ (UK kbd)  | Export pattern to output capture buffer
|                        |+(`lctrl - x <` on german kbd)
|                        |+(`lctrl - x -` on german kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
|                        |+(`lctrl - x /` on US kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
| lshift - F7            | Import pattern from AnalogRytm node (pattern clipboard)
| lshift - F8            | Import pattern from TrigSeq node (current play pattern)
| lshift - F9            | Import pattern from Monitor node (pattern clipboard)
| lctrl - lshift - F9    | Import Standard MIDI File (.mid) (can also be imported via drag'n'drop)
| END                    | Edit previous pattern (-1)
| lctrl - END            | Edit previous pattern (-4)
| HOME                   | Edit next pattern (+1)
| lctrl - HOME           | Edit next pattern (+4)
| lctrl - l              | Edit pattern length
| lctrl - lshift - l     | Set pattern length to cursor position
| lshift - n             | Edit pattern name (focus widget)
| lctrl - p              | Double pattern length
| lctrl - lshift - p     | Double pattern length and duplicate events
| lctrl - x e            | Reverse pattern
| lctrl - x PAGEUP       | Rotate pattern up
| lctrl - x PAGEDOWN     | Rotate pattern down
| lshift - t             | Trim pattern start to cursor
].kbd_table Tracker Node Editor: Pattern keyboard shortcuts


### Events

| Key Sequence          :|:Description
| lalt - v               | Open Events pulldown menu
| lctrl - BACKSPACE      | Delete cel envelope
| DELETE                 | Delete step (current ctl) or clear block
| lctrl - DELETE         | Delete tick (current ctl) or clear block
| lshift - DELETE        | Delete step or block (all ctls)
| lctrl - lshift - DELETE| Delete tick (all ctls)
| \\ (UK kbd)            | Clear event or clear block (when there is a selection)
|                        |+(`<` on german PC kbd)
|                        |+(`/` on US PC kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
|                        |+(`-` on german PC kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
| lshift - \\ (UK kbd)   | Clear event(s) (all ctls)
|                        |+(`lshift - <` on german PC kbd)
|                        |+(`lshift - -` on german kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
|                        |+(`lshift - /` on US kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
| INSERT                 | Insert step (current ctl)
| lctrl - INSERT         | Insert tick (current ctl)
| lshift - INSERT        | Insert step (all ctls)
| lctrl - lshift - INSERT| Insert tick (all ctls)
| lctrl - x m            | Show Multiply delta times by constant dialog
| lctrl - x f            | Calc delay ticks (microshift to `CTL\_DELAY`)
| lctrl - x RCTRL        | Toggle recording pre-count
| lctrl - x RSHIFT       | Toggle note-off recording
| lctrl - x /            | Toggle `CTL\_DUR` recording
|                        |+(`lctrl - x - -` on german kbd)
| lctrl - x ,            | Upload capture buffer to Eureka procedural track sample
| lctrl - x .            | Toggle record-usrctl-at-note-on mode (queue)
| lctrl - x RETURN       | Toggle quantized recording
| lctrl - x h            | Toggle audio latency compensated recording (see prefs)
| lctrl - x UP           | Increase step or block controller value(s) (coarse)
| lctrl - x DOWN         | Decrease step or block controller value(s) (coarse)
| lctrl - x RIGHT        | Increase step or block controller value(s) (fine)
| lctrl - x LEFT         | Decrease step or block controller value(s) (fine)
| lctrl - o              | Copy MIDI controller output state to current track step
| lshift - s             | No Selection: Toggle slide (note overlap)  Selection: Sort Notes (+velocity / duration)
| lctrl - x 9            | Toggle echo-frame-rec-events-to-keyjazz-instrument-frame-play (e.g. unmapped synth param changes)
].kbd_table Tracker Node Editor: Events keyboard shortcuts


### Block

| Key Sequence          :|:Description
| lalt - b               | Open Block pulldown menu
| lctrl - x b            | Cycle through block modes (`CTL`, `TRACK`, `PATTERN`)
| lctrl - b              | Set block start
| lctrl - e              | Set block end
| lctrl - a              | Select entire pattern or none
| lshift - \<CURSOR\>    | Set selection
| lctrl - c              | Copy block or pattern (when there is no selection)
| lctrl - v              | Paste block (replace) or paste pattern (when there is no selection)
| lctrl - lshift - v     | Paste block (insert) or paste pattern (when there is no selection)
| lshift - F2            | Transpose block up
| lshift - F1            | Transpose block down
| lshift - F3            | Free envelope(s) (depends on block mode)
| lctrl - lshift - UP    | Move block up one step
| lctrl - lshift - LEFT  | Move block up one tick
| lctrl - lshift - DOWN  | Move block down one step
| lctrl - lshift - RIGHT | Move block down one tick
| lctrl - lshift - a     | Quantize block or pattern (when there is no selection)
|                        |+ (either hard quantize, or show advanced quantization dialog, depending on last choice)
| lctrl - i              | Interpolate controller values (current ctl)
].kbd_table Tracker Node Editor: Block keyboard shortcuts


### Instrument

| Key Sequence          :|:Description
| lalt - i               | Open Instrument pulldown menu
| lctrl - TAB            | Toggle pattern / instrument views
| F1                     | KeyJazz octave up
| F2                     | KeyJazz octave down
| lshift - w             | Enter default velocity (focus widget)
| lctrl - [              | Select previous instrument
|                        |+(`lctrl - ü` on german kbd)
| lctrl - ]              | Select next instrument
|                        |+(`lctrl - +` on german kbd)
| lshift - f             | Enter instrument nr (focus widget)
| lshift - k             | Edit currently selected instrument audio sample in Eureka
| lctrl - F12            | Toggle keyjazz mode
| lctrl - x a            | Select keyjazz instrument from current step and toggle edit mode
|                        |+OR show audio dialog (when there's no `CTL\_INSTR` event on current step)
| 1                      | In InstrumentView: focus instruments table
| 2                      | In InstrumentView: focus instrument name textfield
| 3                      | In InstrumentView: focus params table
| 4                      | In InstrumentView: focus param name textfield
| 5                      | In InstrumentView: focus param type combobox
| RIGHT                  | In InstrumentView: focus param table when instrument table has focus
| LEFT                   | In InstrumentView: focus instrument table when param table has focus
| k                      | Edit instrument sound (SysEx, NodeAnalogRytm, or Eureka sample / plugin)
].kbd_table Tracker Node Editor: Instrument keyboard shortcuts




## TrigSeq

### General

| Key Sequence          :|:Description
| lctrl - 1              | Switch to *Pattern* tab
| lctrl - 2              | Switch to *Grid* tab
| lctrl - 3              | Switch to *Play* tab
| a                      | Show context menu
| f                      | Fold / unfold tracks
| t                      | Show track setup dialog
| e                      | Show euclidean rhythm dialog
| LCTRL                  | (Hold) Force MIDI note step recording (e.g. in `CHROMATIC` output mode)
|                        |  hold to paint velocities
| LSHIFT                 | hold to paint mod1 (press both mouse buttons to clear)
| LALT                   | hold to paint mod2 (press both mouse buttons to clear)
| RCTRL                  | Toggle recording. Hold to momentarily enable step recording.
| lctrl - x RCTRL        | Toggle pre-count
| lctrl - x RSHIFT       | Cycle through recording modes (off, edit focus, always) (see "Play" tab 'Rec' checkbox)
| RSHIFT                 | Stop replay and recording OR create undo state and start replay+recording
| u                      | Undo / Redo
| RETURN                 | Beat-synchronized undo (while recording) (else: play step)
| lshift - F7            | Import pattern from AnalogRytm node (pattern clipboard)
| lshift - F9            | Import pattern from Monitor node (pattern clipboard)
| lctrl - TAB            | Cycle *Step Details* display mode
| lctrl - x l            | Toggle KeyJazz lock
].kbd_table TrigSeq Node Editor: General keyboard shortcuts


### Track

| Key Sequence             :|:Description
| lctrl - F3                | Clear track
| lshift - BACKSPACE        | Clear track
| BACKSPACE                 | Clear track trigs+repeats
| lalt - BACKSPACE          | Clear track microtime
| lctrl - BACKSPACE         | Clear track arg1+arg2
| lctrl - lalt - BACKSPACE  | Clear track mod1+mod2
| lshift - lalt - BACKSPACE | Clear track + mod init
| r                         | Reverse track trigs
| j                         | Randomize track trigs
| lctrl - j                 | Randomize repeats
| lshift - j                | Randomize arg1 (velocity)
| k                         | Edit track sound (SysEx, NodeAnalogRytm, or Eureka sample / plugin)
| lshift - k                | Load arg2 identity (0, 1, 2, ..)
| o                         | Rotate track left
| p                         | Rotate track right
| INSERT                    | Insert step
| DELETE                    | Delete step
| lshift - PAGEUP           | Increment length
| lshift - PAGEDOWN         | Decrement length
| s                         | Solo track
| d                         | Mute track
| lctrl - l                 | Focus track length widget
| PAGEUP                    | Increment track length
| PAGEDOWN                  | Decrement track length
| lctrl - a                 | Focus arg3 widget
| lctrl - l                 | Focus play\_length widget
| lctrl - h                 | Focus swing amount widget
| lctrl - t                 | Focus note length widget
| lshift - x                | Cut to track clipboard
| lshift - c                | Copy to track clipboard
| lshift - v                | Paste track clipboard
| lctrl - lshift - v        | Paste track clipboard to all patterns (show dialog)
| lalt - LEFT               | Decrease default note duration
| lalt - RIGHT              | Increase default note duration
].kbd_table TrigSeq Node Editor: Track keyboard shortcuts


### Pattern

| Key Sequence                  :|:Description
| lshift - F3                    | Clear pattern
| lctrl - lshift - BACKSPACE     | Clear pattern
| lshift - r                     | Reverse pattern
| lshift - o                     | Rotate pattern left
| lshift - p                     | Rotate pattern right
| lshift - INSERT                | Insert step on all tracks
| lshift - DELETE                | Delete step on all tracks
| lctrl - w                      | Cut pattern to clipboard
| lctrl - c                      | Copy pattern to clipboard
| lctrl - v                      | Paste pattern clipboard
| lctrl - r                      | Clone pattern
| lctrl - lshift - r             | Clone pattern (sub)
| lctrl - x g                    | Guess note-name profile id from output port(s)
| lctrl - x m                    | Remap tracks to current note-name profile
| lctrl - x t                    | Try revert to global output (when all tracks+patterns output to same port)
| lctrl - x c                    | Start output capture (aligned to bar start, stops when replay is stopped)
| lctrl - x v                    | Paste output capture buffer to current pattern  (note: buffer is shared between all nodes)
].kbd_table TrigSeq Node Editor: Pattern keyboard shortcuts


### Step

| Key Sequence          :|:Description
| LEFT                   | Move cursor left
| RIGHT                  | Move cursor right
| UP                     | Move cursor up
| DOWN                   | Move cursor down
| lshift - HOME          | Move cursor to first track
| lshift - END           | Move cursor to last track
| lctrl - LEFT           | Move step left
| lctrl - RIGHT          | Move step right
| lctrl - UP             | Move step up
| lctrl - DOWN           | Move step down
| SPACE                  | Toggle trig
| lctrl - SPACE          | Toggle repeat
| lctrl - lshift - SPACE | Toggle *follow arranger* mode
| RETURN                 | Play step (all tracks) (when not recording, else beat-synced undo)
| w                      | Play track trigger
| lshift - w             | Play step (current track, even if track is muted)
| x                      | Cut step to clipboard
| c                      | Copy step to clipboard
| v                      | Paste step clipboard
| 1                      | Set step arg1 (velocity index) to 0
| 2                      | Set step arg1 (velocity index) to 1
| 3                      | Set step arg1 (velocity index) to 2
| 4                      | Set step arg1 (velocity index) to 3
| 5                      | Select previous stepdetails view
| 6                      | Select next stepdetails view
| lctrl - x RIGHT        | Increment step arg2 (e.g. note duration) by 1
| lctrl - x UP           | Increment step arg2 (e.g. note duration) by 4
| lctrl - x LEFT         | Decrement step arg2 (e.g. note duration) by 1
| lctrl - x DOWN         | Decrement step arg2 (e.g. note duration) by 4
| lshift - LEFT          | Microtime-shift step to the left
| lshift - RIGHT         | Microtime-shift step to the right
| h                      | Toggle microtime limit (full step or 1/4 step)
].kbd_table TrigSeq Node Editor: Step keyboard shortcuts


### Track Setup Dialog

| Key Sequence          :|:Description
| ESCAPE                 | Close dialog
| 1                      | Focus-cycle mod1 widgets
| 2                      | Focus-cycle mod2 widgets
| a                      | Focus arg3
| c                      | Focus close button
| d                      | Focus tick duration
| f                      | Focus-cycle forced-output widgets
| j                      | Focus-cycle jitter widgets
| m                      | Focus-cycle forced-mod-output widgets
| r                      | Focus-cycle repeat widgets
| s                      | Focus microshift
| t                      | Focus-cycle transform widgets
| v                      | Focus-cycle velocity 0..3 widgets
].kbd_table TrigSeq Node Editor: Track Setup Dialog keyboard shortcuts




## CtlMorph

| Key Sequence          :|:Description
| lctrl - 1              | Switch to *Pattern* tab
| lctrl - 2              | Switch to *Controllers* tab
| lctrl - 3              | Switch to *Play* tab
| a                      | Show group tools menu
| c                      | Copy entry to clipboard
| lshift - c             | Copy group to clipboard
| v                      | Paste entry clipboard
| lshift - v             | Paste group clipboard
| INSERT                 | Add new entry
| lshift - INSERT        | Clone selected entry
| DELETE                 | Delete selected entry
| lctrl - o              | Update entries from current controller output state
| lctrl - lshift - o     | Update entries from current controller output state (all groups)
| lctrl - g              | Show cyclic morph dialog
| lshift - r             | Group reset (reset to center values)
| lctrl - e              | Query remote controller targets (Eureka mod matrix)
| lctrl - f              | Query remote controller values (Eureka mod matrix)
| lctrl - t              | Query remote controller names (Eureka mod matrix)
| lctrl - q              | Query all remote controller targets+names+values (Eureka mod matrix)
| lctrl - UP             | Move selected entry up
| lctrl - DOWN           | Move selected entry down
].kbd_table CtlMorph Node Editor: General keyboard shortcuts

NOTE: The *Query Remote* functions look at the first output entry to determine the (VST) device and MIDI channel




## Freeform

### General

| Key Sequence          :|:Description
| lctrl - 1              | Switch to *Ports* tab
| lctrl - 2              | Switch to *Pattern* tab
| lctrl - 3              | Switch to *Sequence* tab
| lctrl - 4              | Switch to *Play* tab
| lctrl - lshift - c     | Copy pattern to clipboard (differs from usual `lctrl - c` shortcut)
| a                      | Show context or selection menu
| f                      | Toggle track focus
| w                      | Arm / Disarm track
| q                      | (Un-)Focus track quantization amount widget
| lctrl - y              | Undo (`lctrl - z` on german kbd)
| lctrl - lshift - y     | Redo (`lctrl - lshift - z` on german kbd)
| TAB                    | Toggle Notes view
| lctrl - x RALT         | Toggle step-recording
| z OR k OR CAPSLOCK     | Toggle step-recording (`y` on german kbd)
| lctrl - x l            | Toggle KeyJazz lock
].kbd_table Freeform Node Editor: General keyboard shortcuts

### Sequence

| Key Sequence          :|:Description
| lshift - BACKSPACE         | Clear track
| lctrl - lshift - BACKSPACE | Clear pattern
| LEFT                   | Move cursor left
|                        |+StepRec Mode: Move recording cursor to previous 16th
| lshift - LEFT          | Move cursor left and expand selection
|                        |+StepRec Mode: Move recording cursor to previous 32th
| RIGHT                  | Move cursor right
|                        |+StepRec Mode: Move recording cursor to next 16th
| lshift - RIGHT         | Move cursor right and expand selection
|                        |+StepRec Mode: Move recording cursor to next 32th
| UP                     | Move cursor up
|                        |+StepRec Mode: Move recording cursor to previous quarter note
| lshift - UP            | Move cursor up and expand selection
| lctrl - UP             | Y-expand selection and move cursor to first track
| DOWN                   | Move cursor down
|                        |+StepRec Mode: Move recording cursor to next quarter note
| lshift - DOWN          | Move cursor down and expand selection
| lctrl - DOWN           | Y-expand selection and move cursor to last track
| lctrl - LEFT           | Move cursor home
| lctrl - lshift - LEFT  | Move cursor home and expand selection
| PAGEUP                 | Move cursor left 8 bars
| lshift - PAGEUP        | Move cursor left 8 bars and expand selection
| PAGEDOWN               | Move cursor right 8 bars
| lshift - PAGEDOWN      | Move cursor right 8 bars and expand selection
| 1                      | Loop 1 bar
| 2                      | Loop 2 bars
| 3                      | Loop 4 bars
| 4                      | Loop 8 bars
| a                      | Show context menu
| lctrl - a              | Select used bars on current track, on all tracks, or select nothing (cycle)
| lctrl - lshift - a     | Freeze quantization (selection or cursor bar)
| b                      | Merge performance controllers to bar starts of selected tracks (mw, bc, fc, ex, gp1..8, brightness(74), pitchbend, channel pressure, sustain (hold 1), portamento, sostenuto, soft pedal, legato, hold 2)
| lctrl - b              | Same as `b` but always start from bar 1 and merge until end of current loop
| c                      | Loop selection
| lctrl - c              | Copy selection to clipboard
| lctrl - x c            | Start output capture (aligned to bar start, stops when replay is stopped)
| lctrl - x v            | Paste output capture buffer to current pattern  (note: buffer is shared between all nodes)
| lctrl - x \\ (UK kbd)  | Export selection to output capture buffer
|                        |+(`lctrl - x <`  on german PC kbd)
|                        |+(`lctrl - x -` on german kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
|                        |+(`lctrl - x /` on US kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
| lshift - F9            | Paste NodeMonitor clipboard to current pattern
| lctrl - lshift - F9    | Import Standard MIDI File (.mid) (can also be imported via drag'n'drop)
| d                      | Mute / Unmute track
| e                      | Set Punch In / Out to selection
| \'                     | Detail-Edit in Tracker node
|+                       |+ `ä` on german kbd
|+                       |+ note: copies `<swing>` autodev b (when used)
| lctrl - e              | Export selection to Monitor clipboard
| lctrl - lshift - e     | Export selection to new Tracker node
| lctrl - x e            | Export pattern to new sub-song (show dialog)
| f                      | Toggle track focus
| lshift - f             | MIDI-Filter selection (show dialog)
| lctrl - f              | Set view to replay position
| lctrl - lshift -f      | Toggle autoscroll
| lctrl - x f            | Toggle auto-focus / auto-rec
| g                      | Set view to replay position
| i                      | Set Punch In to cursor
| lctrl - x l            | Set left loop locator to cursor
| lctrl - j              | Create velocity * modwheel modmatrix/modmatrices for selected track output port(s)
|                        |+ (modwheel input read from last selected track port)
| lctrl - lshift - j     | Create modwheel + modwheel2<delta> modmatrix/modmatrices for selected track output port(s)
|                        |+ (modwheel2 input read from last selected track port. centerpos = no change)
| k                      | Edit track sound (SysEx, NodeAnalogRytm, or Eureka sample / plugin)
| lctrl - l              | Create velocity curve modmatrix/modmatrices for selected track output port(s)
| lctrl - lshift - l     | Create velocity randomization modmatrix/modmatrices for selected track output port(s)
| lctrl - m              | Mix-Paste clipboard
| o                      | Set Punch Out to cursor
| lctrl - o              | Insert output states of selected tracks (MIDI cache)
| lctrl - lshift -o      | Clear output states of selected tracks (MIDI cache)
| p                      | Show bar program change dialog
| lshift - p             | Delete bar program change
| \\ (UK kbd)            | Insert output states of selected tracks (MIDI cache)
|                        |+(`<` on german kbd)
|                        |+(`-` on german kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
|                        |+(`/` on US kbd with `UI.b\_key\_map\_slash\_to_less==true`)
| lshift - \\ (UK kbd)   | Clear output states of selected tracks (MIDI cache)
|                        |+(`lshift - <` on german kbd)
|                        |+(`lshift - -` on german kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
|                        |+(`lshift - /` on US kbd with `UI.b\_key\_map\_slash\_to\_less==true`)
| [                      | Half grid note length (`ü` on german kbd)
| ]                      | Double grid note length (`+` on german kbd)
| w                      | Arm / Disarm track
| q                      | (Un-)Focus track quantization amount widget
| lctrl - q              | Freeze quantization (selection or cursor bar)
| r                      | Move loop to cursor
| lctrl - x r            | Set right loop locator to cursor
| s                      | Toggle solo track
| t                      | Set left loop locator to cursor
| lctrl - x h            | Clear undo / redo history
| lctrl - v              | Paste clipboard (replace)
| lshift - v             | Paste clipboard (insert)
| lshift - c             | Copy I/O Port to clipboard
| lshift - t             | Paste I/O Port from clipboard
| y                      | Set right loop locator to cursor (`z` on german kbd)
| INSERT                 | Insert bar(s) on selected track(s)
| lctrl - INSERT         | Insert bar(s) on selected track(s) including tempo
| DELETE                 | Clear or Delete selected frames, notes, or bars
| lshift - DELETE        | Delete selected bars
| lctrl - DELETE         | Delete selected bars including tempo (when selection contains no events)
| lctrl - lshift - DELETE | Delete selected bars including tempo
| RALT                   | Enable step-recording (momentary switch)
| lctrl - x RALT         | Toggle step-recording
| z                      | Toggle step-recording (y on german kbd)
| x                      | Toggle extended block selection mode (lctrl-lshift-\<cursor\> keys)
| RCTRL                  | Arm / Disarm node
| lshift - RCTRL         | Toggle replace / overdub recording mode
| lctrl - x RCTRL        | Toggle recording pre-count
| lctrl - x BACKSPACE    | Toggle ignore-seek-prgchg-mute
| RSHIFT                 | Start / Stop Recording (auto-arm node)
| RETURN                 | Synchronized record-restart at next bar (while recording is active)
|+                       |+OR: Detail-Edit in Tracker node (while recording is NOT active)
| lctrl - x RETURN       | Reset node last pattern start (editor) song offset to 0 (e.g. after editing node from Arranger, then removing it from Arranger)
| SPACE                  | Toggle Replay
| lctrl - SPACE          | Toggle Replay (leave cursor at replay position)
].kbd_table Freeform Node Editor: Sequence Tab keyboard shortcuts


### Sequence Frames/Tempo

| Key Sequence                     :|:Description
| b                                 | Edit bar BPM
| h                                 | Toggle snap-bar-handle-to-frame
| m                                 | Merge selected frames
| .                                 | Move selected frames left by one tick
| lshift - .                        | Move selected frames left by 64th note
| lctrl - .                         | Move selected frames left and snap to 32th note
| /                                 | Move selected frames right by one tick (`-` on german kbd)
| lshift - /                        | Move selected frames right by 64th note (`lshift - -` on german kbd)
| lctrl - /                         | Move selected frames right and snap to 32th note (`lctrl - -` on german kbd)
| RALT                              | Enable step-recording (momentary switch) + position recording cursor via mouse
| mouse MMB                         | same as RALT. hold lctrl or LMB to snap step-rec position to 16th note.
| lctrl - x RALT                    | Toggle step-recording
| z OR k or CAPSLOCK                | Toggle step-recording (`y` on german kbd)
| mouse WHEELUP                     | Select previous bar
|                                   |+Selection: lazy-reduce to first frame / audition previous frame
|                                   |+Mouse over selection: Move selected frames left by one tick
| mouse WHEELDOWN                   | Select next bar
|                                   |+Selection: lazy-reduce to first frame / audition next frame
|                                   |+Mouse over selection: Move selected frames right by one tick
| mouse lctrl - WHEELUP             | Snap-Move selected frames left
| mouse lctrl - WHEELDOWN           | Snap-Move selected frames right
| mouse lshift - WHEELUP            | Move selected frames left by 1/64
| mouse lshift - WHEELDOWN          | Move selected frames right by 1/64
| mouse RMB drag                    | Scroll horizontally
| mouse LMB click                   | (De-)Select frame near mouse
| mouse LMB drag                    | Move selected frames or create new selection
| mouse lctrl - LMB drag            | Snap-move selected frames
].kbd_table Freeform Node Editor: Sequence Tab Frames/Tempo view keyboard shortcuts


### Sequence Notes

| Key Sequence                     :|:Description
| DELETE                            | Delete selected notes
| RALT                              | Enable step-recording (momentary switch) + position recording cursor via mouse
| mouse MMB                         | same as RALT. hold lctrl or LMB to snap step-rec position to 16th note.
| lctrl - x RALT                    | Toggle step-recording
| z OR k or CAPSLOCK                | Toggle step-recording (`y` on german kbd)
| mouse WHEELUP                     | Select previous bar
|                                   |+Mouse over note: Move selected notes left by one tick
| mouse WHEELDOWN                   | Select next bar
|                                   |+Mouse over note: Move selected notes right by one tick
| mouse lctrl - WHEELUP             | No Selection: Increase note offset (scroll)
| mouse lctrl - WHEELDOWN           | No Selection: Decrease note offset (scroll)
| mouse lctrl - lshift - WHEELUP    | Increase note zoom
| mouse lctrl - lshift - WHEELDOWN  | Decrease note zoom
| mouse lalt - WHEELUP              | Selection: Move selected notes left by one tick
| mouse lalt - WHEELDOWN            | Selection: Move selected notes right by one tick
| mouse lctrl - WHEELUP             | Selection: Snap-Move selected notes left
| mouse lctrl - WHEELDOWN           | Selection: Snap-Move selected notes right
| mouse RMB drag                    | Scroll horizontally, vertically, or scale note velocities
| mouse lalt - RMB drag             | Offset note velocities
| mouse LMB click                   | (De-)Select note near mouse
| mouse LMB doubleclick             | Fit notes to view or select (chord-)notes near mouse position
| mouse LMB drag                    | Move selected notes, modify note duration(s), or create new selection
| mouse lshift - LMB drag           | Modify note duration(s)
| mouse lctrl - LMB drag            | Snap-move selected notes
].kbd_table Freeform Node Editor: Sequence Tab Notes view keyboard shortcuts


TIP: quick-undo during mouse drag: click the "other" mouse button (RMB when drag was started with LMB and vice versa)

TIP: hold LMB then press RMB to undo

TIP: hold RMB then press LMB to redo




## ModMatrix

| Key Sequence               :|:Description
| lctrl - 1                   | Select `Pattern` tab
| lctrl - 2                   | Select `Macro Ctl` tab
| lctrl - 3                   | Select `Entries` tab
| lctrl - 4                   | Select `Curves` tab
| lctrl - 5                   |
|+lctrl - x TAB               |+Select `Play` tab
| lctrl - e                   | Edit script for selected pattern
| lctrl - lshift - e          |
|+lctrl - p                   |+Init script from selected pattern and edit it
| lctrl - f                   | Edit script for selected entry
| INSERT                      | Create new entry
| lshift - INSERT             | Clone selected entry
| lctrl - INSERT              | Show `Paste` menu
| DELETE                      | Delete selected entry
| lctrl - UP                  | Move selected entry up
| lctrl - DOWN                | Move selected entry down
| lshift - c                  | Copy selected entry to clipboard
| lshift - v                  | Paste clipboard to selected entry
| lctrl - q                   | Enable / disable selected entry
| lshift - BACKSPACE          | Clear pattern (remove all entries)
].kbd_table ModMatrix Node Editor: keyboard shortcuts

### Entry script dialog

| Key Sequence               :|:Description
| ESCAPE                      |
|+lctrl - f                   |+Close dialog
| UP                          | Edit previous entry
| DOWN                        | Edit next entry
].kbd_table ModMatrix Node Editor: entry script dialog keyboard shortcuts

### Pattern script dialog

| Key Sequence               :|:Description
| ESCAPE                      |
|+lctrl - f                   |+Close dialog
|+lctrl - p                   |+Init script from current pattern
].kbd_table ModMatrix Node Editor: pattern script dialog keyboard shortcuts




## Monitor
| Key Sequence          :|:Description
| lctrl - 1              | Select Pattern tab
| lctrl - 2              | Select Monitor tab
| lctrl - 3              | Select Play tab
| INSERT                 | Delete frames before last marker and go to next pattern (clone)
| lctrl - INSERT         | Delete frames before last marker and go to next pattern (clone + sub)
| lctrl - UP             | (patch view) move pattern up
| lctrl - DOWN           | (patch view) move pattern down
| f                      | (patch view) set input filter from first event
| c                      | (patch view) set pattern name from detected chord
| q                      | (patch view) send SysEx request
| lctrl - q              | (patch view) send SysEx request (clear pattern + select FrameRec)
| DELETE                 | (patch view) delete pattern
| r                      | toggle replay (return to start)
| lshift - r             | toggle replay
| lctrl - lshift - SPACE | Toggle *follow arranger* mode
| d                      | Toggle lock current frame
| lctrl - f              | Toggle freeze
| h                      | Delete frames before last marker and go to next pattern
| lshift - h             | Delete frames before last marker and go to next pattern (sub)
| u                      | Clear and restart recording
| BACKSPACE              | (when locked) Clear current frame
| LEFT                   | (when locked) Go to previous frame
| lctrl - LEFT           | Go to previous marker
| RIGHT                  | (when locked) Go to next frame
| lctrl - RIGHT          | Go to next marker
| lshift - BACKSPACE     | Clear current pattern
| lctrl - DELETE         | Delete frames before current
| lctrl - TAB            | Toggle patch view
| PAGEUP                 | Select previous mode (Replay, Monitor(FrameRec), Monitor(FramePlay), Monitor(FrameRec+Play)
| PAGEDOWN               | Select next mode
| DELETE                 | (replay mode) Delete current frame
| BACKSPACE              | (replay mode) Clear current frame
| LEFT                   | (replay mode) Select previous frame
| RIGHT                  | (replay mode) Select next frame
| UP                     | (replay mode) Select next frame
| DOWN                   | (replay mode) Select previous frame
| a                      | (replay mode) Show Tools menu
| e                      | (replay mode) Merge frames (show dialog)
| j                      | (replay mode) Merge frames (show dialog) (marker only)
| f                      | (replay mode) Fix duration (show dialog)
| v                      | (replay mode) Fix velocity (show dialog)
| w                      | (replay mode) Repeat-Play current frame
| s                      | (replay mode) Toggle scrub-mode
| k                      | (replay mode) Trim to punch in/out
| l                      | (replay mode) Delete frames before last marker
| b                      | (replay mode) Fit BPM
| lshift - LEFT          | (replay mode) Move frame left
| lshift - RIGHT         | (replay mode) Move frame right
| lctrl - w              | (replay mode) Freeze (apply) replay filter
| DELETE                 | (record mode) Delete pattern events
| i                      | (record mode) Punch in
| o                      | (record mode) Punch out
| p                      | (record mode) Punch in or out
| e                      | (record mode) Switch to replay mode and Merge frames (show dialog)
| j                      | (record mode) Switch to replay mode and Merge frames (show dialog) (marker only)
| f                      | (record mode) Switch to replay mode and Fix duration (show dialog)
| v                      | (record mode) Switch to replay mode and Fix velocity (show dialog)
| k                      | (record mode) Switch to replay mode and Trim to punch in/out
| l                      | (record mode) Switch to replay mode and Delete frames before last marker
| t                      | Toggle or queue marker
| RCTRL                  | Toggle record mode
| lshift - RCTRL         | Select next mode
| lctrl - x RCTRL        | Toggle replay-sync mode (see note below)
| RSHIFT                 | Clear pattern and restart recording

tip: when "replay-sync" is enabled, the time offset relative to the replay(/record) start is recorded in the Monitor pattern.
      for example, when recording automation via a Monitor node that shall later be imported into e.g. a Tracker node, this
      should set to "enable" via `lctrl - x RCTRL`. This causes a marker frame to be inserted at the beginning of the recording.

tip: the (per-pattern) "Input Filter" setting can be used to restrict recording to certain devices / channels



## Modular

| Key Sequence          :|:Description
| h                      | Show / Hide connection lines
|+BACKSPACE              |+
| lctrl - x c            | Start output capture (aligned to bar start, stops when replay is stopped)
| lctrl - 1              | Select Pattern tab
| lctrl - 2              | Select I/O tab
| lctrl - 3              | Select Patch tab
| lctrl - 4              | Select Play tab
| lctrl - WHEELUP        | Select previous used pattern
| lctrl - WHEELDOWN      | Select next used pattern
].kbd_table Modular Node Editor: Keyboard shortcuts




$(buf!footnote)
$(buf!w3validator)

Document created in $(var:gen_ms) on $(var:localdatetime)
].create
