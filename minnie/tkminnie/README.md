![screenshot](tests/screenshots/test_073_polygon_gradient_radial_aa.png)

These are the `TKS` language bindings for the `Minnie` vector graphics API (distributed under terms of the [MIT license](../LICENSE)).

[](tests/test_shadervg.tks) contains a set of ~70 test cases for Minnie's ShaderVG render backend ([screenshots](tests/screenshots/), [API docs](../../apidocs/ee/ShaderVG.html)).

See [here](../native/README.md) for the native C / C++ version of the library and test cases.

# Build instructions for macOS:
- `$ . setenv_macos.sh`
- `$ cd minnie/tkminnie/`
- `$ m clean ; m bin && m install`

# Running the test:
- `$ cd tests`
- `$ tks test_shadervg`

# Keyboard controls:
`ESCAPE` exit
`SPACE` toggle animation
`lctrl - SPACE` toggle slow-motion
`RETURN` reset rotation
`UP` select next test
`DOWN` select previous test
`LEFT` decrease stroke width
`RIGHT` increase stroke width
`aa` toggle anti-aliasing
`d` toggle debug mode
`f` decrease radius-x scale
`lctrl - f` decrease both radius x and y scales
`g` increase radius-x scale
`lctrl - g` increase both radius x and y scales
`h` decrease radius-y scale
`lctrl - h` decrease both radius x and y scales
`j` increase radius-y scale
`lctrl - j` increase both radius x and y scales
`l` toggle symmetry-lock (rounded rectangles, ellipses, ..)
`m` toggle multisample-antialiasing (MSAA)
`n` increase AA range (debug)
`lctrl - n` decrease AA range (debug)
`o` draw border (debug)
`p` save screenshot
`q` decrease size scale x
`lctrl - q` decrease both size scale x and y
`r` decrease alpha
`lctrl - r` decrease decal alpha
`t` increase alpha
`lctrl - t` increase decal alpha
`v` toggle v-sync
`w` increase size scale x
`lctrl - w` increase both size scale x and y
`z` decrease size scale y
`lctrl - z` decrease both size scale x and y
`x` increase size scale y
`lctrl - x` increase both size scale x and y
