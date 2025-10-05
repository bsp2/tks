![screenshot](../tkminnie/tests/screenshots/test_073_polygon_gradient_radial_aa.png)

This is the standalone C / C++ build of the `Minnie` vector graphics API (distributed under terms of the [MIT license](../LICENSE)).

[](main.c) contains a set of ~70 test cases for Minnie's ShaderVG render backend ([screenshots](../tkminnie/tests/screenshots/), [API docs](../../apidocs/ee/ShaderVG.html)).

See [here](../tkminnie/README.md) for `TKS` language bindings and script version of the test cases.

# Build instructions for macOS:
- `$ . setenv_macos.sh`
- `$ cd minnie/native/build/macos/`
- `m clean ; m bin`

# Running the test:
- `./minnie_test`

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
`l` toggle symmetry-lock (rounded rectangles, ellipses, ..)
`r` decrease alpha
`lctrl - r` decrease decal alpha
`t` increase alpha
`lctrl - t` increase decal alpha
