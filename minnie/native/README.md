This is the standalone C / C++ build of the `Minnie` vector graphics API (distributed under terms of the [MIT license](../LICENSE)).

[main.c](main.c) contains a set of ~70 test cases for Minnie's ShaderVG render backend ([screenshots](../tkminnie/tests/screenshots/), [API docs](https://html-preview.github.io/?url=https://raw.githubusercontent.com/bsp2/tks/main/apidocs/ee/ShaderVG.html)).

See [here](../tkminnie/README.md) for `TKS` language bindings and script version of the test cases.

# Prerequisites
- Clang compiler
- SDL2 (install via `% brew install sdl2`)

# Build instructions for macOS:
- `% . ./setenv_macos.sh`
- `% cd minnie/native/build/macos/`
- `m clean ; m bin`

# Running the test:
- `./minnie_test`

# Keyboard controls:
- `ESCAPE` exit
- `UP` select next test
- `DOWN` select previous test
- `SPACE` toggle animation
- `lctrl - SPACE` toggle slow-motion
- `RETURN` reset rotation
- `LEFT` decrease stroke width
- `RIGHT` increase stroke width
- `a` toggle anti-aliasing
- `d` toggle debug mode
- `l` toggle symmetry-lock (rounded rectangles, ellipses, ..)
- `r` decrease alpha
- `lctrl - r` decrease decal alpha
- `t` increase alpha
- `lctrl - t` increase decal alpha

# Screenshots
- test_000_rect_fill_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_000_rect_fill_aa.png)
- test_001_rect_fill_stroke_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_001_rect_fill_stroke_aa.png)
- test_002_rect_stroke_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_002_rect_stroke_aa.png)
- test_003_ellipse_fill_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_003_ellipse_fill_aa.png)
- test_004_ellipse_fill_stroke_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_004_ellipse_fill_stroke_aa.png)
- test_005_ellipse_stroke_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_005_ellipse_stroke_aa.png)
- test_006_roundrect_fill_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_006_roundrect_fill_aa.png)
- test_007_roundrect_fill_stroke_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_007_roundrect_fill_stroke_aa.png)
- test_008_roundrect_stroke_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_008_roundrect_stroke_aa.png)
- test_009_triangles_tex_uv_flat
  
  ![screenshot](../tkminnie/tests/screenshots/test_009_triangles_tex_uv_flat.png)
- test_010_triangles_tex_uv_gouraud
  
  ![screenshot](../tkminnie/tests/screenshots/test_010_triangles_tex_uv_gouraud.png)
- test_011_triangles_tex_uv_flat_decal
  
  ![screenshot](../tkminnie/tests/screenshots/test_011_triangles_tex_uv_flat_decal.png)
- test_012_triangles_tex_uv_gouraud_decal
  
  ![screenshot](../tkminnie/tests/screenshots/test_012_triangles_tex_uv_gouraud_decal.png)
- test_013_line_strip_flat_1
  
  ![screenshot](../tkminnie/tests/screenshots/test_013_line_strip_flat_1.png)
- test_014_line_strip_flat_2
  
  ![screenshot](../tkminnie/tests/screenshots/test_014_line_strip_flat_2.png)
- test_015_line_strip_flat_aa_1
  
  ![screenshot](../tkminnie/tests/screenshots/test_015_line_strip_flat_aa_1.png)
- test_016_line_strip_flat_aa_2
  
  ![screenshot](../tkminnie/tests/screenshots/test_016_line_strip_flat_aa_2.png)
- test_017_line_strip_flat_bevel
  
  ![screenshot](../tkminnie/tests/screenshots/test_017_line_strip_flat_bevel.png)
- test_018_line_strip_flat_bevel_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_018_line_strip_flat_bevel_aa.png)
- test_019_custom_shader_1
  
  ![screenshot](../tkminnie/tests/screenshots/test_019_custom_shader_1.png)
- test_020_custom_shader_2
  
  ![screenshot](../tkminnie/tests/screenshots/test_020_custom_shader_2.png)
- test_021_custom_shader_3(vbo)
  
  ![screenshot](../tkminnie/tests/screenshots/test_021_custom_shader_3(vbo).png)
- test_022_lines
  
  ![screenshot](../tkminnie/tests/screenshots/test_022_lines.png)
- test_023_lines_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_023_lines_aa.png)
- test_024_begin_line_strip
  
  ![screenshot](../tkminnie/tests/screenshots/test_024_begin_line_strip.png)
- test_025_begin_line_strip_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_025_begin_line_strip_aa.png)
- test_026_begin_line_strip_bevel
  
  ![screenshot](../tkminnie/tests/screenshots/test_026_begin_line_strip_bevel.png)
- test_027_begin_line_strip_bevel_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_027_begin_line_strip_bevel_aa.png)
- test_028_begin_lines
  
  ![screenshot](../tkminnie/tests/screenshots/test_028_begin_lines.png)
- test_029_begin_lines_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_029_begin_lines_aa.png)
- test_030_draw_filled_rectangle
  
  ![screenshot](../tkminnie/tests/screenshots/test_030_draw_filled_rectangle.png)
- test_031_begin_points_square
  
  ![screenshot](../tkminnie/tests/screenshots/test_031_begin_points_square.png)
- test_032_begin_points_square_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_032_begin_points_square_aa.png)
- test_033_begin_points_round
  
  ![screenshot](../tkminnie/tests/screenshots/test_033_begin_points_round.png)
- test_034_begin_points_round_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_034_begin_points_round_aa.png)
- test_035_begin_filled_triangles
  
  ![screenshot](../tkminnie/tests/screenshots/test_035_begin_filled_triangles.png)
- test_036_begin_filled_triangle_fan
  
  ![screenshot](../tkminnie/tests/screenshots/test_036_begin_filled_triangle_fan.png)
- test_037_begin_filled_triangle_strip
  
  ![screenshot](../tkminnie/tests/screenshots/test_037_begin_filled_triangle_strip.png)
- test_038_begin_filled_gouraud_triangles
  
  ![screenshot](../tkminnie/tests/screenshots/test_038_begin_filled_gouraud_triangles.png)
- test_039_begin_filled_gouraud_triangle_fan
  
  ![screenshot](../tkminnie/tests/screenshots/test_039_begin_filled_gouraud_triangle_fan.png)
- test_040_begin_filled_gouraud_triangle_strip
  
  ![screenshot](../tkminnie/tests/screenshots/test_040_begin_filled_gouraud_triangle_strip.png)
- test_041_begin_textured_triangles
  
  ![screenshot](../tkminnie/tests/screenshots/test_041_begin_textured_triangles.png)
- test_042_begin_textured_triangles_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_042_begin_textured_triangles_alpha.png)
- test_043_begin_textured_triangle_fan
  
  ![screenshot](../tkminnie/tests/screenshots/test_043_begin_textured_triangle_fan.png)
- test_044_begin_textured_triangle_fan_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_044_begin_textured_triangle_fan_alpha.png)
- test_045_begin_textured_triangle_strip
  
  ![screenshot](../tkminnie/tests/screenshots/test_045_begin_textured_triangle_strip.png)
- test_046_begin_textured_triangle_strip_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_046_begin_textured_triangle_strip_alpha.png)
- test_047_begin_textured_gouraud_triangles
  
  ![screenshot](../tkminnie/tests/screenshots/test_047_begin_textured_gouraud_triangles.png)
- test_048_begin_textured_gouraud_triangles_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_048_begin_textured_gouraud_triangles_alpha.png)
- test_049_begin_textured_gouraud_triangle_fan
  
  ![screenshot](../tkminnie/tests/screenshots/test_049_begin_textured_gouraud_triangle_fan.png)
- test_050_begin_textured_gouraud_triangle_fan_alpha.png
  
  ![screenshot](../tkminnie/tests/screenshots/test_050_begin_textured_gouraud_triangle_fan_alpha.png)
- test_051_begin_textured_gouraud_triangle_strip
  
  ![screenshot](../tkminnie/tests/screenshots/test_051_begin_textured_gouraud_triangle_strip.png)
- test_052_begin_textured_gouraud_triangle_strip_alpha.png
  
  ![screenshot](../tkminnie/tests/screenshots/test_052_begin_textured_gouraud_triangle_strip_alpha.png)
- test_053_text_1
  
  ![screenshot](../tkminnie/tests/screenshots/test_053_text_1.png)
- test_054_text_2_clip
  
  ![screenshot](../tkminnie/tests/screenshots/test_054_text_2_clip.png)
- test_055_text_3_swirl
  
  ![screenshot](../tkminnie/tests/screenshots/test_055_text_3_swirl.png)
- test_056_text_4_ul
  
  ![screenshot](../tkminnie/tests/screenshots/test_056_text_4_ul.png)
- test_057_text_5_ul_clip
  
  ![screenshot](../tkminnie/tests/screenshots/test_057_text_5_ul_clip.png)
- test_058_text_6_accel
  
  ![screenshot](../tkminnie/tests/screenshots/test_058_text_6_accel.png)
- test_059_text_7_accel_clip
  
  ![screenshot](../tkminnie/tests/screenshots/test_059_text_7_accel_clip.png)
- test_060_text_8_zoom
  
  ![screenshot](../tkminnie/tests/screenshots/test_060_text_8_zoom.png)
- test_061_text_9_zoom_sdf
  
  ![screenshot](../tkminnie/tests/screenshots/test_061_text_9_zoom_sdf.png)
- test_062_gradient_to_texture
  
  ![screenshot](../tkminnie/tests/screenshots/test_062_gradient_to_texture.png)
- test_063_triangles_gradient_linear
  
  ![screenshot](../tkminnie/tests/screenshots/test_063_triangles_gradient_linear.png)
- test_064_triangles_gradient_radial
  
  ![screenshot](../tkminnie/tests/screenshots/test_064_triangles_gradient_radial.png)
- test_065_triangles_gradient_conic
  
  ![screenshot](../tkminnie/tests/screenshots/test_065_triangles_gradient_conic.png)
- test_066_polygon_vbo
  
  ![screenshot](../tkminnie/tests/screenshots/test_066_polygon_vbo.png)
- test_067_polygon_aa_vbo
  
  ![screenshot](../tkminnie/tests/screenshots/test_067_polygon_aa_vbo.png)
- test_068_begin_polygon
  
  ![screenshot](../tkminnie/tests/screenshots/test_068_begin_polygon.png)
- test_069_begin_polygon_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_069_begin_polygon_aa.png)
- test_070_polygon_gradient_linear
  
  ![screenshot](../tkminnie/tests/screenshots/test_070_polygon_gradient_linear.png)
- test_071_polygon_gradient_linear_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_071_polygon_gradient_linear_aa.png)
- test_072_polygon_gradient_radial
  
  ![screenshot](../tkminnie/tests/screenshots/test_072_polygon_gradient_radial.png)
- test_073_polygon_gradient_radial_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_073_polygon_gradient_radial_aa.png)
- test_074_polygon_gradient_conic
  
  ![screenshot](../tkminnie/tests/screenshots/test_074_polygon_gradient_conic.png)
- test_075_polygon_gradient_conic_aa
  
  ![screenshot](../tkminnie/tests/screenshots/test_075_polygon_gradient_conic_aa.png)
