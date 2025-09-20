% minnie
% bsp
% 17-Aug-2025
>>>
04Jul2025, 05Jul2025, 06Jul2025, 12Jul2025, 13Jul2025, 02Aug2025, 03Aug2025
04Aug2025, 16Aug2025, 17Aug2025
<<<
$(var:header)

# About

![]("../../../tkminnie/screenshots/minnie_017.png" "minnie" w=320)

`minnie` is a 2D / 2.5D vector graphics API with a simplified OpenVG / SVG like feature subset, distributed under terms of the [MIT license](https://opensource.org/license/mit).

The renderer processes a size optimized byte stream (`.mib`) which is either generated on-the-fly in RAM, read from flash / mass storage, or converted from a human-readable ASCII file format (`.min`), which in turn can be converted from `SVG` files.

The core target runtime consists of a single C++ header file (`minnie.h`) which can optionally be linked with the SGI tesselator to support complex concave paths with holes (even-odd fill rule).

Triangle rendering is performed with an antialiased software rasterizer which can be replaced by GPU acceleration as required.

# Table of Contents
$(buf!toc)


# Formats

## ASCII file format (.min)
- human readable text format (`.min`)
- compiled to size-optimized `.mib` binary format via offline preprocessor
- can be converted from `.svg` files via offline preprocessor
   - svg_loader `.min` export
- host-side pseudo ops / directives
   - build matrices via rotate / translate / scale / skew / frustum functions
   - path transformations (`geo_scale` / `gs`, `geo_transform` / `gxf`, `gxfs`)
   - tesselation resolution scaling (`seg_scale`)
   - select palette index by closest matching #rgb / #rrggbb color (`color` / `i`)
   - post-process palette colors (`pal_hsv` / `pal_brightness` / `pal_contrast`)
   - reverse paths and drawing order when targeting coverage-AA software rasterizer (`aa`)
>>>test016
~~~
geo 640 480
aa  on
pal #eee #6db #547 #7cb #fff #9dc #779 #498

; clear screen
f 0

; define path
p mypath
M -75 -75
r 150 150
z

; set 2D transform
n rotate(30) translate(320 240)

; draw filled path
i 1
k fill
d2 mypath

; draw stroked path
i 6
h miter
k 6
d2 mypath
~~~ lang=min
] test016_aa.min
<<<
>>>test046
~~~
geo 800 600
aa on
pal #e8e8e8 #b9d5e2 #000000 

; clear screen
f 0

; translate / scale geometry
gxf 16 3 6 6

; define svg path 'path6968'
ph path6968
svg M 16.928616,67.341255 \\
C 17.977291,66.142769 59.924304,6.6678972 59.924304,6.6678972 \\
L 87.189862,89.063815 50.186604,71.386145 58.426196,22.547838 \\
21.72256,78.577061 32.508935,84.26987 53.632252,48.465099 \\
16.628994,31.985915 10.786375,50.862071 \\
38.051933,64.045418 30.561395,75.580846 Z
z

; draw filled path
i 1
k fill
d path6968

; draw stroked path
i 2
k 0.25
h miter butt
d path6968
~~~ lang=min
] test046_evenodd.min
<<<
- example: $(buf!test016) $(buf!test046)
>>>test079
~~~
geo 640 480      ; set canvas size
pal #eee #247    ; define palette
ps 64            ; set default tesselation depth
p mypath         ; define path
m -200 -200      ; move to left / top corner
r 400 400 40 30  ; add rounded and centered rectangle
z                ; end of path
f 0              ; clear with color 0
i 1              ; select color 1
m 320 240        ; move cursor to screen center
d mypath         ; draw path
~~~ lang=min
] test079_minimal.min
<<<
>>>test079bin
~~~
01 e2 50 3c e3 01 ee 2e 74 84 8e 40 95 c7 c7 a6
c8 c8 14 0f ff 84 40 96 9f 77 d0 01 e0
~~~
] test079_minimal.mib
<<<
- example: $(buf!test079) => $(buf!test079bin) => ![]("../../../tkminnie/screenshots/gles_edgeaa_v01/minnie_079.png" "test079_minimal.min" w=320)


## Size-optimized binary stream format (.mib)
- generated on-the-fly in RAM
- or converted from `.min` file
- variable-sized ops / display-list instructions control VM-like state machine
   - see [Opcode Reference](#opcode-reference)
- implementations
   - `debug viewer`
      - can display the stream in a desktop window
      - can disassemble the stream in `.min` format
      - used for prototyping / debugging
   - `minnie.h` C++ header file / library
      - single file
      - resource-optimized implementation
      - supports custom memory allocators
      - no 3rd party dependencies
      - optional extension: SGI tesselator (even-odd paths with holes)
      - optional extension: Antialiased software rasterizer
      - e.g. for running on an embedded target

## SVG file size comparison

|:size   |:SVG name       |:size   |:minnie name             |:size factor
|   4189:| cake.svg       |    622 :| test018\_cake\_aa.mib  | 1:7
|  31681:| wild-boar.svg  |   5284 :| test039_wildboar.mib   | 1:6
|   6828:| valentines.svg |   1514 :| test033_valentines.mib | 1:5
|   1478:| bicycle.svg    |    358 :| test035_bicycle.mib    | 1:4
|  42352:| crab.svg       |   6425 :| test036_crab.mib       | 1:7
|  96719:| tiger.svg      |  17135 :| test040_tiger.mib      | 1:6
|  20743:| elefant.svg    |   1811 :| test043_elefant.mib    | 1:11
].table_fixed SVG file size comparison

# Features

## Fill / clear
- clear current draw-buffer with current color

## Paths
      - path types
         - convex
            - ![]("../../../tkminnie/screenshots/minnie_004_lores.png" "test004_fill.min" w=160) ![]("../../../tkminnie/screenshots/minnie_002_lores.png" "test002_fill.min" w=160)
         - concave
            - ![]("../../../tkminnie/screenshots/minnie_005_lores.png" "test005_concave.min" w=160) ![]("../../../tkminnie/screenshots/minnie_013_lores.png" "test013_reverse.min" w=160)
         - concave with holes
            - `HAVE_VGTESSELATE` build option (SGI tesselator that supports even-odd fill rule)
            - ![]("../../../tkminnie/screenshots/minnie_035_lores.png" "test035_bicycle.min" w=160) ![]("../screenshots/minnie_046_lores.png" "test046_evenodd.min" w=160)
      - sub paths
         - ![]("../../../tkminnie/screenshots/minnie_032_lores.png" "test032_subpaths.min" w=160) ![]("../screenshots/minnie_048_lores.png" "test048_nano.min" w=160)
      - per (sub-)path spline / arc / ellipse tesselation granularity
         - ![]("../../../tkminnie/screenshots/minnie_018_lores.png" "test018_cake_aa.min" w=160)
      - open and closed paths
         - ![]("../../../tkminnie/screenshots/minnie_007_lores.png" "test007_svg.min" w=160) ![]("../../../tkminnie/screenshots/minnie_033_lores.png" "test033_valentines.min" w=160)

### Path elements
      - lines
         - ![]("../../../tkminnie/screenshots/minnie_001_crop.png" "test001_lines.min" w=160)
         - horizontal lines (`h` / `H` in SVG)
         - vertical lines (`v` / `V` in SVG)
      - rectangles
         - ![]("../../../tkminnie/screenshots/minnie_000.png" "test000_rect.min" w=160)
      - rounded rectangles
         - ![]("../../../tkminnie/screenshots/gles_edgeaa_v01/minnie_074.png" "test074_roundrect.min" w=160)
      - ellipses
         - ![]("../../../tkminnie/screenshots/minnie_008_lores.png" "test008_ellipse.min" w=160)
      - cubic and quadratic splines / bezier curves
      - mirrored cubic and quadratic splines
      - elliptic arcs
         - ![]("../../../tkminnie/screenshots/minnie_039_lores.png" "test039_wildboard.min" w=160)

### SVG paths
      - full support for SVG path definition syntax (`.min` format)
         - ![]("../../../tkminnie/screenshots/minnie_036_lores.png" "test036_crab.min" w=160) ![]("../screenshots/minnie_040_hires.png" "test040_tiger.min" w=160) ![]("../../../tkminnie/screenshots/gles_edgeaa_v01/minnie_066.png" "test066_cat" w=160) ![]("../../../tkminnie/screenshots/gles_edgeaa_v01/minnie_069.png" "test069_bluejay" w=160)


## Path instancing

Once declared, paths may be rendered an arbitrary number of times.

Paths are always translated to the current cursor position, and may optionally be transformed by the current 2D or 3D matrix beforehand.

Each path instance can be assigned an individual stroke width and line join / cap style and stroke or fill color.

### No matrix transformation
      - simply translate to cursor position
      - ![]("../../../tkminnie/screenshots/minnie_011_lores.png" "test011_cursor.min" w=160)

### 2D matrix transformation
      - apply 2D matrix (2x3)
      - translate to cursor position
      - ![]("../../../tkminnie/screenshots/minnie_016_lores.png" "test016_aa.min" w=160)

### 3D matrix transformation
      - apply projective 3D matrix (4x4)
      - translate to cursor position
      - ![]("../../../tkminnie/screenshots/minnie_030_lores.png" "test030_minnie_3d.min" w=160)

## Clipping

### Clip to framebuffer
         - ![]("../../../tkminnie/screenshots/minnie_027_lores.png" "test027_clip2dfb.min" w=160) ![]("../../../tkminnie/screenshots/minnie_029_lores.png" "test029_3d.min" w=160)

### Clip to path
         - pre tesselation
            - ![]("../../../tkminnie/screenshots/minnie_023_lores.png" "test023_clip2d.min" w=160)
         - post tesselation
            - ![]("../../../tkminnie/screenshots/minnie_024_lores.png" "test024_clip2d.min" w=160)

## Filled and stroked rendering
      - color
      - stroke width
      - butt, round, square line caps
      - miter, round, bevel line joins
      - ![]("../../../tkminnie/screenshots/minnie_010_lores.png" "test010_d2d.min" w=160)
>>>
![]("../screenshots/minnie_044_lores.png" "test044_linejoin_miterlimit.min" w=160) ![]("../screenshots/minnie_045_lores.png" "test045_linejoin.min" w=160) ![]("../screenshots/minnie_047_lores.png" "test047_shapes" w=160)
<<<

## Color palettes
- 12 and 24 bits per pixel

## Stencil masking
- stencil color
- ![]("../../../tkminnie/screenshots/minnie_014_lores.png" "test014_mask.min" w=160)

## Framebuffers
- implicit main framebuffer
- up to 255 auxiliary buffers

## Blits
- target draw-buffer select op (`t \<fbIdx\>`)
- source read-buffer select op (`u \<fbIdx\>`)
- blit ops (`b \<dstX\> \<dstY\> \<w\> \<h\> \<srcX\> \<srcY\>`)

# Opcode reference

Binary stream format for `.mib` files and procedural buffers.

## Stream header byte
>>>aa_note
note: in AA mode, the main path drawops are reversed (e.g. for the coverage AA software rasterizer)
<<<
The first byte in a `.mib` stream is the header byte (def=0x01)
  - bits 0..5: version (LSB6, 1..63)
  - bit 6: 1=reverse drawing order (`aa` / `rev`)
     - $(buf!aa_note)
  - bit 7: endianness (1=big endian)

## 0x01
] RESVD x \<program\>
- *RESVD* execute VM program

## 0x02
] n \<mat2x3\> (u8)
- select 2D transformation matrix
- u8 mat_idx byte follows

## 0x03
] q \<mat4x4\> (u8)
- select 3D transformation matrix
- u8 mat_idx byte follows

## 0x04
] n \<mat2x3\> (u16)
- select 2D transformation matrix
- u16 mat_idx byte follows

## 0x05
] q \<mat4x4\> (u16)
- select 3D transformation matrix
- u16 mat_idx byte follows

## 0x0E
] h \<join\> \<cap\>
- select line join and cap types
- `h \<join\> \<cap\>`
   - join types: `none`, `miter`, `round`, `bevel`
   - cap types: `none`, `butt`, `round`, `square`
- join / cap byte follows (MSB4=join, LSB4=cap)

## 0x0F
] RESVD
- *RESVD*

## 0x10 .. 0x1F
] i \<palIdx\> (0..15)
- select color by palette index
- `i \<palIdx\>` (0..15)
- select palette index 0..15

## 0x20
] i \<palIdx\> (16..255)
- select color by palette index
- `i \<palIdx\>` (16..255)
- palette index byte follows

## 0x21
] m \<x\> \<y\> f32
- move pen relative to current position
- `m \<x\> \<y\>`
- dx / dy 32bit floats follow
- note: force 32 bit float moves with `m_f32 on`

## 0x22
] M \<x\> \<y\> f32
- move pen to absolute position
- `m \<x\> \<y\>`
- dx / dy 32bit floats follow
- note: force 32 bit float moves with `m_f32 on`

## 0x23
] c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\> (f32)
- add cubic spline segment to current (sub-)path
- `c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\>`
- 32bit floats follow
- (note) c1 is relative to last cursor pos
- (note) c2 is relative to dst (new cursor pos) (c2Abs = dstAbs - c2)
- (note) enable with `c_f32 on` or `f32 on`

## 0x24
] RESVD select logic op
- *RESVD* select logic op

## 0x25
] s \<c2x\> \<c2y\> \<x\> \<y\> (f32)
- add mirrored cubic spline segment to current (sub-)path
- `s \<c2x\> \<c2y\> \<x\> \<y\>`
- (implicit) c1 is mirrored version of previous c2 (`s` in SVG)
- 32bit floats follow
- (note) c2 is relative to dst (c2Abs = dstAbs - c2)
- (note) enable with `c_f32 on` or `f32 on`

## 0x26
] l \<x\> \<y\> (f32)
- add line segment to current (sub-)path
- `l \<x\> \<y\>`
- 32bit floats follow
- (note) enable with `l_f32 on` or `f32 on`

## 0x27
] ml \<miter_limit\> (u8)
- set miter limit (def=32.0)
- u8 byte follows

## 0x28
] m \<x\> \<y\> signed 6.2
- move pen relative to current position
- `m \<x\> \<y\>`
- dx / dy bytes follow

## 0x29
] m \<x\> \<y\> signed 14.2
- move pen relative to current position
- `m \<x\> \<y\>`
- dx / dy shorts follow

## 0x2A
] m \<x\> \<y\> signed 8.0
- move pen relative to current position
- `m \<x\> \<y\>`
- dx / dy bytes follow

## 0x2B
] m \<x\> \<y\> unsigned 8.0
- move pen relative to current position
- `m \<x\> \<y\>`
- dx / dy bytes follow

## 0x2C
] M \<x\> \<y\> unsigned 8.0
- move pen to absolute position
- `M \<x\> \<y\>`
- dx / dy bytes follow

## 0x2D
] M \<x\> \<y\> unsigned 8.0 even
- move pen to absolute position
- `M \<x\> \<y\>`
- dx / dy bytes follow (scaled by 2.0)

## 0x2E
] M \<x\> \<y\> unsigned 8.0 mul4
- move pen to absolute position
- `M \<x\> \<y\>`
- dx / dy bytes follow (scaled by 4.0)

## 0x2F
] M \<x\> \<y\> signed 14.2
- move pen to absolute position
- `M \<x\> \<y\>`
- dx / dy shorts follow

## 0x30 .. 0x36
] t \<fbIdx\> (0..6)
- select current draw-buffer
- `t \<fbIdx\>` (0..6)

## 0x37
] t \<fbIdx\> (7..255)
- select current draw-buffer
- `t \<fbIdx\>` (7..255)
- draw-buffer index byte follows

## 0x38 .. 0x3E
] u \<fbIdx\> (0..6)
- select current read-buffer
- `u \<fbIdx\>` (0..6)

## 0x3F
] u \<fbIdx\> (7..255)
- select current read-buffer
- `u \<fbIdx\>` (7..255)
- read-buffer index byte follows

## 0x40 .. 0x4D
] f \<palIdx\> (0..13)
- fill / clear current draw-buffer with palette index
- (no logic ops)
- `f \<palIdx\>` (0..13)

## 0x4E
] f \<palIdx\> (14..255)
- fill / clear current draw-buffer with palette index
- (no logic ops)
- `f \<palIdx\>` (14..255)
- palette index byte follows

## 0x4F
] f aa
- clear coverage alpha (AA rasterizer)
- `f aa`

## 0x50
] disable 2D / 3D clip paths
- disable 2D / 3D clip paths
- `w2off` / `w3off` / `woff`

## 0x51
] clip to framebuffer
- set 2D clip to framebuffer (clip tesselated vertices)

## 0x52
] w2p \<pathIdx\> (u8)
- set 2D clip to path (clip pre-tesselated vertices)
- `w2p \<pathIdx\>` (u8)
- pathIdx-1 unsigned byte follows

## 0x53
] w2p \<pathIdx\> (u16)
- set 2D clip to path (clip pre-tesselated vertices)
- `w2p \<pathIdx\>` (u16)
- pathIdx-1 unsigned short follows

## 0x54
] w2 \<pathIdx\> (u8)
- set 2D clip to path (clip tesselated vertices)
- `w2 \<pathIdx\>` (u8)
- pathIdx-1 unsigned byte follows

## 0x55
] w2 \<pathIdx\> (u16)
- set 2D clip to path (clip tesselated vertices)
- `w2 \<pathIdx\>` (u16)
- pathIdx-1 unsigned short follows

## 0x56
] w3fb
- set 3D clip viewport to framebuffer (znear=0.01)
- `w3fb`

## 0x57 .. 0x5D
] RESVD
- *RESVD*

## 0x5E
] RESVD free path <idx>
- *RESVD* free path \<idx\>
- unsigned short \<pathIdx\> follows

## 0x5F
] free last used path data
- free last used path data
- d* sets last used path

## 0x60
] RESVD b \<dstX\> \<dstY\>
- *RESVD* `b \<dstX\> \<dstY\>`
- blit rectangular region (entire srcBuf)
- signed 14.2 short (dst*)

## 0x61
] RESVD b \<dstX\> \<dstY\> \<w\> \<h\> \<srcX\> \<srcY\>
- *RESVD* `b \<dstX\> \<dstY\> \<w\> \<h\> \<srcX\> \<srcY\>`
- blit rectangular region
- signed 14.2 short (dst*)
- unsigned byte (src*/w/h)

## 0x62
] RESVD b \<dstX\> \<dstY\> \<w\> \<h\> \<srcX\> \<srcY\>
- *RESVD* `b \<dstX\> \<dstY\> \<w\> \<h\> \<srcX\> \<srcY\>`
- blit rectangular region
- signed 14.2 short (dst*) => 6*2 = 12 bytes
- unsigned short (src*/w/h)

## 0x63
] RESVD b \<dstX\> \<dstY\> \<w\> \<h\> \<srcX\> \<srcY\>
- *RESVD* `b \<dstX\> \<dstY\> \<w\> \<h\> \<srcX\> \<srcY\>`
- blit rectangular region
- signed 10.2 shorts (dst*)
- unsigned 12 bit shorts (src*/w/h)
- => 6*12=72bit = 9 bytes + 1 pad byte = 10 bytes

## 0x64
] RESVD b \<dstX\> \<dstY\> \<w\> \<h\> \<srcX\> \<srcY\>
- *RESVD* `b \<dstX\> \<dstY\> \<w\> \<h\> \<srcX\> \<srcY\>`
- blit rectangular region
- signed 14.2 short (dst*)
- unsigned byte (src*/w/h) * 4

## 0x65 .. 0x6F
] RESVD
- *RESVD*

## 0x70 .. 0x7F
] a \<palIdx\> (0..15)
- *RESVD* `a \<palIdx\>` (0..15)
- 0x70..0x7F: enable mask palIdx=0..15
- (note) uses stencil buffer on GPU

## 0x80
] a \<palIdx\> (16..255)
- `a \<palIdx\>` (16..255)
- palIdx byte follows

## 0x81
] a off
- disable mask (`a off`)

## 0x82
] k \<stroke\_w\>
- set stroke width
- `k \<stroke\_w\>`
- \<stroke_w\>  unsigned 6.2 byte (2 fractional bits). 0.25 (int 1) = draw (non-extruded) polyline (pseudo w "line")
- this is the *radius* (effective line width = 2\*stroke_w)

## 0x83
] k 0
- disable stroke / enable fill
- `k 0` / `k fill`

## 0x84
] p \<id\>
- begin convex or stroked path definition
- `p \<id\>`

## 0x85
] pt \<id\>
- begin concave path definition (no holes)
- will be tesselated using simple ear-clipping tesselator, not the SGI tesselator
- `pt \<id\>`

## 0x86
] psub
- begin sub-path definition

## 0x87
] ph \<id\>
- begin concave even-odd path definition (with holes)
- `ph \<id\>`
- will be tesselated using te SGI tesselator (even-odd fill rule)

## 0x88
] l \<x\> \<y\> (unsigned 6.2)
- add line segment to current (sub-)path
- `l \<x\> \<y\>`
- unsigned 6.2 bytes follow

## 0x89
] l \<x\> \<y\> (signed 14.2)
- add line segment to current (sub-)path
- `l \<x\> \<y\>`
- signed 14.2 shorts follow

## 0x8A
] l \<x\> \<y\> (unsigned 8.0)
- add line segment to current (sub-)path
- `l \<x\> \<y\>`
- unsigned 8.0 bytes follow

## 0x8B
] l \<x\> \<y\> (signed 6.2)
- add line segment to current (sub-)path
- `l \<x\> \<y\>`
- signed 6.2 bytes follow

## 0x8C
] l \<x\> \<y\> (signed 10.2 packed)
- add line segment to current (sub-)path
- `l \<x\> \<y\>`
- signed 10.2 packed 12bits follow
   - must sign extend during decoding

## 0x8D
] pi
- begin immediate path (rect, roundrect, ellipse, circle)
- `pi`
- will be rendered via specialized draw functions

## 0x8E
] j \<num\_seg\> (u8)
- set num segments for following spline / arc / ellipse ops (def=8)
- unsigned byte follows

## 0x8F
] j \<num\_seg\> (u16)
- set num segments for following spline / arc / ellipse ops (def=8)
- `j \<num\_seg\>` (must occur within path declaration)
- unsigned short follows

## 0x90
] c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\> (signed 6.2)
- add cubic spline segment to current (sub-)path
- `c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\>`
- delta < abs(32)  (signed 6.2 bytes)
- (note) c1 is relative to last cursor pos
- (note) c2 is relative to dst (new cursor pos) (c2Abs = dstAbs - c2)

## 0x91
] c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\> (signed 8.0)
- add cubic spline segment to current (sub-)path
- `c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\>`
- delta < abs(128)  (signed 8.0 bytes)
- (note) c1 is relative to last cursor pos
- (note) c2 is relative to dst (new cursor pos) (c2Abs = dstAbs - c2)

## 0x92
] c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\> (unsigned 8.0)
- add cubic spline segment to current (sub-)path
- `c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\>`
- 0 <= delta < 256  (unsigned 8.0 bytes)
- (note) c1 is relative to last cursor pos
- (note) c2 is relative to dst (new cursor pos) (c2Abs = dstAbs - c2)

## 0x93
] c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\> (signed 14.2)
- add cubic spline segment to current (sub-)path
- `c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\>`
- delta >= abs(32) (signed 14.2 words)
- (note) c1 is relative to last cursor pos
- (note) c2 is relative to dst (new cursor pos) (c2Abs = dstAbs - c2)

## 0x94
] c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\> (signed 10.2 packed)
- add cubic spline segment to current (sub-)path
- `c \<c1x\> \<c1y\> \<c2x\> \<c2y\> \<x\> \<y\>`
- delta >= abs(512) (signed 10.2 12bits) (must sign extend during decode)
- (note) c1 is relative to last cursor pos
- (note) c2 is relative to dst (new cursor pos) (c2Abs = dstAbs - c2)

## 0x95
] m \<x\> \<y\> -(unsigned 8.0)
- move pen relative to current position
- `m \<x\> \<y\>`
- dx-1 / dy-1 bytes follow

## 0x96
] m \<x\> \<y\> (unsigned 8.0*2)
- move pen relative to current position
- `m \<x\> \<y\>`
- dx-1 / dy-1 bytes follow (scaled by 2.0)

## 0x97
] RESVD
- *RESVD*

## 0x98
] s \<c2x\> \<c2y\> \<x\> \<y\> (signed 6.2)
- add mirrored cubic spline segment to current (sub-)path
- `s \<c2x\> \<c2y\> \<x\> \<y\>`
- (implicit) c1 is mirrored version of previous c2 (`s` in SVG)
- delta < abs(32)  (signed 6.2 bytes)
- (note) c2 is relative to dst (c2Abs = dstAbs - c2)

## 0x99
] s \<c2x\> \<c2y\> \<x\> \<y\> (signed 8.0)
- add mirrored cubic spline segment to current (sub-)path
- `s \<c2x\> \<c2y\> \<x\> \<y\>`
- (implicit) c1 is mirrored version of previous c2 (`s` in SVG)
- delta < abs(128)  (signed 8.0 bytes)
- note) c2 is relative to dst (c2Abs = dstAbs - c2)

## 0x9A
] s \<c2x\> \<c2y\> \<x\> \<y\> (unsigned 8.0)
- add mirrored cubic spline segment to current (sub-)path
- `s \<c2x\> \<c2y\> \<x\> \<y\>`
- (implicit) c1 is mirrored version of previous c2 (`s` in SVG)
- 0 <= delta < 256  (unsigned 8.0 bytes)
- (note) c2 is relative to dst (c2Abs = dstAbs - c2)

## 0x9B
] s \<c2x\> \<c2y\> \<x\> \<y\> (signed 14.2)
- add mirrored cubic spline segment to current (sub-)path
- `s \<c2x\> \<c2y\> \<x\> \<y\>`
- (implicit) c1 is mirrored version of previous c2 (`s` in SVG)
- delta >= abs(32)  (signed 14.2 words)
- (note) c2 is relative to dst (c2Abs = dstAbs - c2)

## 0x9C .. 0x9D
] RESVD
- *RESVD*

## 0x9E
] arc \<rx\> \<ry\> \<xrot\> \<flags\> \<dx\> \<dy\> (s14.2)
- add elliptic arc segment to current (sub-)path
- `arc \<rx\> \<ry\> \<xrot\> \<flags\> \<dx\> \<dy\>`
- s14.2 rx
- s14.2 ry
- u9.5:1:1 xrot
   - bits 15..7: 0..512 => 0..360 degrees
   - bits 6..2: angle fractional bits (0..32 => 0..1)
   - bit 1: arc_sweep flag
   - bit 0: large_arc flag
- s14.2 dx
- s14.2 dy

## 0x9F
] arc \<rx\> \<ry\> \<xrot\> \<flags\> \<dx\> \<dy\> (f32)
- add elliptic arc segment to current (sub-)path
- `arc \<rx\> \<ry\> \<xrot\> \<flags\> \<dx\> \<dy\>`
- f32 rx
- f32 ry
- u9.5:1:1 xrot
   - bits 15..7: 0..512 => 0..360 degrees
   - bits 6..2: angle fractional bits (0..32 => 0..1)
   - bit 1: arc_sweep flag
   - bit 0: large_arc flag
- f32 x
- f32 y
- (note) enable 32bit float arcs with `arc_f32 on` or `f32 on`

## 0xA0
] r \<w\> \<h\> (unsigned 6.2)
- add rectangle segment to current (sub-)path
- `r \<w\> \<h\>`
- unsigned 6.2 bytes

## 0xA1
] r \<w\> \<h\> (unsigned 14.2)
- add rectangle segment to current (sub-)path
- `r \<w\> \<h\>`
- unsigned 14.2 shorts

## 0xA2
] r \<w\> \<h\> (unsigned 8.0)
- add rectangle segment to current (sub-)path
- `r \<w\> \<h\>`
- unsigned 8.0 bytes

## 0xA3
] r \<w\> \<h\> (unsigned 8.0 even)
- add rectangle segment to current (sub-)path
- `r \<w\> \<h\>`
- unsigned 8.0 bytes (\*2, even coordinates)

## 0xA4
] r \<w\> \<h\> \<rx\> \<ry\> (f32)
- add rounded rectangle segment to current (sub-)path
- `r \<w\> \<h\>` \<rx\> \<ry\>
- 32 bit floats

## 0xA5
] r \<w\> \<h\> \<rx\> \<ry\> (u14.2)
- add rounded rectangle segment to current (sub-)path
- `r \<w\> \<h\>` \<rx\> \<ry\>
- unsigned 14.2 shorts

## 0xA6
] r \<w\> \<h\> \<rx\> \<ry\> (u8*2)
- add rounded rectangle segment to current (sub-)path
- `r \<w\> \<h\>` \<rx\> \<ry\>
- unsigned byte * 2.0

## 0xA7
] RESVD
- *RESVD*

## 0xA8
] RESVD procedural path ext
- *RESVD* procedural path ext

## 0xA9 .. 0xAF
] RESVD
- *RESVD*

## 0xB0
] e \<rx\> \<ry\> (unsigned 6.2)
- add ellipse segment to current (sub-)path
- `e \<rx\> \<ry\>`
- unsigned 6.2 bytes

## 0xB1
] e \<rx\> \<ry\> (unsigned 14.2)
- add ellipse segment to current (sub-)path
- `e \<rx\> \<ry\>`
- unsigned 14.2 shorts

## 0xB2
] e \<rx\> \<ry\> (unsigned 8.0)
- add ellipse segment to current (sub-)path
- `e \<rx\> \<ry\>`
- unsigned 8.0 bytes

## 0xB3
] e \<rx\> \<ry\> (unsigned 8.0 even)
- add ellipse segment to current (sub-)path
- `e \<rx\> \<ry\>`
- unsigned 8.0 bytes (\*2, even coordinates)

## 0xB4
] circle \<r\> (unsigned 14.2)
- add circle segment to current (sub-)path
- `circle \<r\>
- unsigned 14.2 shorts

## 0xB5 .. 0xBF
] RESVD
- *RESVD*

## 0xC0 .. 0xCF
] RESVD
- *RESVD*

## 0xD0
] d \<id\> (u8)
- draw path at cursor pos (no transformation)
- `d \<id\>`
- unsigned byte pathIdx follows (relative to end of path array)

## 0xD1
] d \<id\> (u16)
- draw path at cursor pos (no transformation)
- `d \<id\>`
- unsigned short pathIdx follows (relative to end of path array)

## 0xD2
] d2d \<id\> (u8)
- draw path at cursor pos and apply current transform\_2d matrix
- `d2d \<id\>`
- unsigned byte pathIdx follows (relative to end of path array)

## 0xD3
] d3d \<id\> (u8)
- draw path at cursor pos and apply current transform\_3d matrix
- `d3d \<id\>`
- unsigned byte pathIdx follows (relative to end of path array)

## 0xD4
] d2d \<id\> (u16)
- draw path at cursor pos and apply current transform\_2d matrix
- `d2d \<id\>`
- unsigned short pathIdx follows (relative to end of path array)

## 0xD5
] d3d \<id\> (u16)
- draw path at cursor pos and apply current transform\_3d matrix
- `d3d \<id\>`
- unsigned short pathIdx follows (relative to end of path array)

## 0xD6 .. 0xDF
] RESVD
- *RESVD*

## 0xE0
] end of stream (EOS)
- end of stream (EOS)

## 0xE1
] geo \<w\> \<h\> (u16)
- set canvas size
- u16 width / height
- `geo \<w\> \<h\>`

## 0xE2
] geo \<w\> \<h\> (u8)
- set canvas size (/8, e.g. 40x25 => 320x200, 240x150 => 1920x1200)
- `geo \<w\> \<h\>`

## 0xE3
] pal12 [idx:]#rgb
- set 12bit palette (and #colors)
- e.g. `pal #ecb #fff #000 #c72 #e73 #e84 #e96 #ea7 #fb8 #fb9 #fcb #fdc #fed #ffe #ccc #e68 #b35 #a24 #f77 #ffc #c34 #a12 #bbb #eeb #e85 #ea7 #fca #fed #9c3 #690 #e95 #fb9 #fdc #333 #666 #999 #920 #400 #e99 #b66`
- (num\_colors-1) byte followed by tightly packed 12bit color array (word-aligned at the end)

## 0xE4
] RESVD pal16 [idx:]#rrggbb
- *RESVD* set 16bit palette (and #colors)
- (num\_colors-1) byte followed by tightly packed 16bit color array

## 0xE5
] pal24 [idx:]#rrggb
- set 24bit palette (and #colors)
- e.g. `pal #e1cdb7 #ffffff #000000 #cc7226 #e87f3a #ea8c4d #ec9961 #eea575 #f1b288 #f3bf9c #f5ccb0 #f8d8c4 #fae5d7 #fcf2eb #cccccc #e5668c #b23259 #a5264c #ff727f #ffffcc #cc3f4c #a51926 #b2b2b2 #e5e5b2 #ea8e51 #efaa7c #f4c6a8 #f9e2d3 #99cc32 #659900 #eb955c #f2b892 #f8dcc8 #323232 #666666 #999999 #992600 #4c0000 #e59999 #b26565`
- (num\_colors-1) byte followed by (R,G,B) 8bit tuples

## 0xE7
] fb \<w\> \<h\>
- define additional draw buffers
- `fb \<w\> \<h\>`
- num\_draw\_bufs unsigned byte follows
- w h unsigned byte pairs follow (/8, 0=use canvas geo)

## 0xE8
] \<matrices\_2d\>
- define 2D matrices
- u16 \<num\_matrices\_2d\> - 1
- float16 2x3 matrices follow
   - row major (m11, m12, m21, m22)

## 0xE9
] \<matrices\_3d\>
- define 3D matrices
- u16 \<num\_matrices\_3d\> - 1
- float16 4x4 matrices follow
   - row major (m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44)

## 0xEA
] RESVD num\_consts \<n\>
- *RESVD* define VM constants
- num constants short follows
- constant data follows

## 0xEB
] RESVD num\_vars \<n\>
- *RESVD* allocate global VM variables
- num vars short follows
- (note) initialized to 0

## 0xEC
] RESVD stack \<size\>
- *RESVD* set VM data stack size (def=8 => 128 bytes)
- (stack size / 16) byte follows
   - 0..254. 255 is reserved for future extensions

## 0xED
] RESVD call\_stack \<size\>
- *RESVD* set VM call stack size (def=2 => 32 return addresses)
- (stack size / 16) byte follows
   - 0..254. 255 is reserved for future extensions

## 0xEE
] RESVD max\_ops \<n\>
- *RESVD* abort VM after \<n\>*1024 ops (def=64 => 64k ops)
- (max\_ops/1024) byte follows
   - 0..254. 255 is reserved for future extensions
- (note) used for infinite loop detection

## 0xEF
] RESVD programs
- *RESVD* define VM programs
- (numPrograms-1) byte follows
- program data follows
   - first word is program data size in bytes (excluding the size word)

## 0xF0 .. 0xFD
] RESVD
- *RESVD*

## 0xFE
] y (end open path)
- `y` end path definition (open)
- reset cursor to (0;0)

## 0xFF
] z (end closed path)
- `z` end path definition (closed)
- reset cursor to (0;0)


# Render backends
   - antialiased software rasterizer
      - `tri_aa` or `tri_aa_fx` (portable C code)
>>>
      - `amiga_tri_aa` 68k-assembly optimized rasterizer: ![]("../screenshots/tiger-320x240-68000-aa.png" "test040_tiger.min" w=160) (MC68000 Amiga 500)
<<<
   - portable to GPU-accelerated APIs
      - screenshots from OpenGL (ES) 2.0 edge-AA shader
         - ![]("../../../tkminnie/screenshots/gles_edgeaa_v01/minnie_040-800x600-edgeaa.png" "test040_tiger.min" w=160) ![]("../../../tkminnie/screenshots/gles_edgeaa_v01/minnie_036.png" "test036_crab.min" w=160) ![]("../../../tkminnie/screenshots/gles_edgeaa_v01/minnie_046.png" "test046_evenodd.min" w=160) ![]("../../../tkminnie/screenshots/gles_edgeaa_v01/minnie_033.png" "test033_valentines.min" w=160) ![]("../../../tkminnie/screenshots/gles_edgeaa_v01/minnie_017.png" "test017_minnie.min" w=160)
           - Ghostscript Tiger (@640x480) runs at..
              - 247 mio tris/sec (\~17240 fps) on a 1398MHz Apple M2pro GPU (19 cores * 16 execution units * 8 ALUs = 2432 ALUs)
              - 1195 mio tris/sec (\~83330 fps) on a 1582Mhz NVidia 1080 TI GPU (28 streaming multiprocessors * 128 cores = 3584 CUDA cores)
              - ..or, clock + ALU-complexity adjusted, approximately 0.29..0.84 mio tris/sec (\~20..58 fps) on a comparatively tiny 250MHz embedded GPU (2 shader units * 8 ALUs = 16 ALUs)

# License
~~~
Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
Copyright 2018-2025 by bsp

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
~~~
] MIT license


$(buf!footnote)
$(buf!w3validator)

Document created in $(var:gen_ms) on $(var:localdatetime)
].create
