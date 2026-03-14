This is the standalone C / C++ build of the `Minnie` vector graphics API (distributed under terms of the [MIT license](../LICENSE)).

[main.c](main.c) contains a set of 200 test cases for Minnie's ShaderVG render backend ([screenshots](../tkminnie/tests/screenshots/), [API docs](https://html-preview.github.io/?url=https://raw.githubusercontent.com/bsp2/tks/main/apidocs/ee/ShaderVG.html)).

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
- test_050_begin_textured_gouraud_triangle_fan_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_050_begin_textured_gouraud_triangle_fan_alpha.png)
- test_051_begin_textured_gouraud_triangle_strip
  
  ![screenshot](../tkminnie/tests/screenshots/test_051_begin_textured_gouraud_triangle_strip.png)
- test_052_begin_textured_gouraud_triangle_strip_alpha
  
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
- test_076_begin_polygon_custom_shader
  
  ![screenshot](../tkminnie/tests/screenshots/test_076_begin_polygon_custom_shader.png)
- test_077_begin_polygon_pattern
  
 ![screenshot](../tkminnie/tests/screenshots/test_077_begin_polygon_pattern.png)
- test_078_begin_polygon_pattern_aa
  
 ![screenshot](../tkminnie/tests/screenshots/test_078_begin_polygon_pattern_aa.png)
- test_079_begin_polygon_pattern_alpha
  
 ![screenshot](../tkminnie/tests/screenshots/test_079_begin_polygon_pattern_alpha.png)
- test_080_begin_polygon_pattern_alpha_aa
  
 ![screenshot](../tkminnie/tests/screenshots/test_080_begin_polygon_pattern_alpha_aa.png)
- test_081_begin_polygon_pattern_decal
  
 ![screenshot](../tkminnie/tests/screenshots/test_081_begin_polygon_pattern_decal.png)
- test_082_begin_polygon_pattern_decal_aa
  
 ![screenshot](../tkminnie/tests/screenshots/test_082_begin_polygon_pattern_decal_aa.png)
- test_083_begin_polygon_pattern_decal_alpha
  
 ![screenshot](../tkminnie/tests/screenshots/test_083_begin_polygon_pattern_decal_alpha.png)
- test_084_begin_polygon_pattern_decal_alpha_aa
  
 ![screenshot](../tkminnie/tests/screenshots/test_084_begin_polygon_pattern_decal_alpha_aa.png)
- test_085_ellipse_fill_aa_linear
  
  ![screenshot](../tkminnie/tests/screenshots/test_085_ellipse_fill_aa_linear.png)
- test_086_ellipse_fill_stroke_aa_linear
  
  ![screenshot](../tkminnie/tests/screenshots/test_086_ellipse_fill_stroke_aa_linear.png)
- test_087_ellipse_fill_aa_radial
  
  ![screenshot](../tkminnie/tests/screenshots/test_087_ellipse_fill_aa_radial.png)
- test_088_ellipse_fill_stroke_aa_radial
  
  ![screenshot](../tkminnie/tests/screenshots/test_088_ellipse_fill_stroke_aa_radial.png)
- test_089_ellipse_fill_aa_conic
  
  ![screenshot](../tkminnie/tests/screenshots/test_089_ellipse_fill_aa_conic.png)
- test_090_ellipse_fill_stroke_aa_conic
  
  ![screenshot](../tkminnie/tests/screenshots/test_090_ellipse_fill_stroke_aa_conic.png)
- test_091_ellipse_fill_aa_pattern
  
  ![screenshot](../tkminnie/tests/screenshots/test_091_ellipse_fill_aa_pattern.png)
- test_092_ellipse_fill_stroke_aa_pattern
  
  ![screenshot](../tkminnie/tests/screenshots/test_092_ellipse_fill_stroke_aa_pattern.png)
- test_093_ellipse_fill_aa_pattern_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_093_ellipse_fill_aa_pattern_alpha.png)
- test_094_ellipse_fill_stroke_aa_pattern_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_094_ellipse_fill_stroke_aa_pattern_alpha.png)
- test_095_ellipse_fill_aa_pattern_decal
  
  ![screenshot](../tkminnie/tests/screenshots/test_095_ellipse_fill_aa_pattern_decal.png)
- test_096_ellipse_fill_stroke_aa_pattern_decal
  
  ![screenshot](../tkminnie/tests/screenshots/test_096_ellipse_fill_stroke_aa_pattern_decal.png)
- test_097_ellipse_fill_aa_pattern_decal_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_097_ellipse_fill_aa_pattern_decal_alpha.png)
- test_098_ellipse_fill_stroke_aa_pattern_decal_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_098_ellipse_fill_stroke_aa_pattern_decal_alpha.png)
- test_099_roundrect_fill_aa_linear
  
  ![screenshot](../tkminnie/tests/screenshots/test_099_roundrect_fill_aa_linear.png)
- test_100_roundrect_fill_stroke_aa_linear
  
  ![screenshot](../tkminnie/tests/screenshots/test_100_roundrect_fill_stroke_aa_linear.png)
- test_101_roundrect_fill_aa_radial
  
  ![screenshot](../tkminnie/tests/screenshots/test_101_roundrect_fill_aa_radial.png)
- test_102_roundrect_fill_stroke_aa_radial
  
  ![screenshot](../tkminnie/tests/screenshots/test_102_roundrect_fill_stroke_aa_radial.png)
- test_103_roundrect_fill_aa_conic
  
  ![screenshot](../tkminnie/tests/screenshots/test_103_roundrect_fill_aa_conic.png)
- test_104_roundrect_fill_stroke_aa_conic
  
  ![screenshot](../tkminnie/tests/screenshots/test_104_roundrect_fill_stroke_aa_conic.png)
- test_105_roundrect_fill_aa_pattern
  
  ![screenshot](../tkminnie/tests/screenshots/test_105_roundrect_fill_aa_pattern.png)
- test_106_roundrect_fill_stroke_aa_pattern
  
  ![screenshot](../tkminnie/tests/screenshots/test_106_roundrect_fill_stroke_aa_pattern.png)
- test_107_roundrect_fill_aa_pattern_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_107_roundrect_fill_aa_pattern_alpha.png)
- test_108_roundrect_fill_stroke_aa_pattern_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_108_roundrect_fill_stroke_aa_pattern_alpha.png)
- test_109_roundrect_fill_aa_pattern_decal
  
  ![screenshot](../tkminnie/tests/screenshots/test_109_roundrect_fill_aa_pattern_decal.png)
- test_110_roundrect_fill_stroke_aa_pattern_decal
  
  ![screenshot](../tkminnie/tests/screenshots/test_110_roundrect_fill_stroke_aa_pattern_decal.png)
- test_111_roundrect_fill_aa_pattern_decal_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_111_roundrect_fill_aa_pattern_decal_alpha.png)
- test_112_roundrect_fill_stroke_aa_pattern_decal_alpha
  
  ![screenshot](../tkminnie/tests/screenshots/test_112_roundrect_fill_stroke_aa_pattern_decal_alpha.png)
- test_001_rect_fill_stroke_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_001_rect_fill_stroke_aa.png)
- test_113_rect_fill_aa_linear
  
  ![screenhots](../tkminnie/tests/screenshots/test_113_rect_fill_aa_linear.png)
- test_114_rect_fill_stroke_aa_linear
  
  ![screenhots](../tkminnie/tests/screenshots/test_114_rect_fill_stroke_aa_linear.png)
- test_115_rect_fill_aa_radial
  
  ![screenhots](../tkminnie/tests/screenshots/test_115_rect_fill_aa_radial.png)
- test_116_rect_fill_stroke_aa_radial
  
  ![screenhots](../tkminnie/tests/screenshots/test_116_rect_fill_stroke_aa_radial.png)
- test_117_rect_fill_aa_conic
  
  ![screenhots](../tkminnie/tests/screenshots/test_117_rect_fill_aa_conic.png)
- test_118_rect_fill_stroke_aa_conic
  
  ![screenhots](../tkminnie/tests/screenshots/test_118_rect_fill_stroke_aa_conic.png)
- test_119_rect_fill_aa_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_119_rect_fill_aa_pattern.png)
- test_120_rect_fill_stroke_aa_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_120_rect_fill_stroke_aa_pattern.png)
- test_121_rect_fill_aa_pattern_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_121_rect_fill_aa_pattern_alpha.png)
- test_122_rect_fill_stroke_aa_pattern_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_122_rect_fill_stroke_aa_pattern_alpha.png)
- test_123_rect_fill_aa_pattern_decal
  
  ![screenhots](../tkminnie/tests/screenshots/test_123_rect_fill_aa_pattern_decal.png)
- test_124_rect_fill_stroke_aa_pattern_decal
  
  ![screenhots](../tkminnie/tests/screenshots/test_124_rect_fill_stroke_aa_pattern_decal.png)
- test_125_rect_fill_aa_pattern_decal_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_125_rect_fill_aa_pattern_decal_alpha.png)
- test_126_rect_fill_stroke_aa_pattern_decal_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_126_rect_fill_stroke_aa_pattern_decal_alpha.png)
- test_127_rect_stroke_aa_linear
  
  ![screenhots](../tkminnie/tests/screenshots/test_127_rect_stroke_aa_linear.png)
- test_128_rect_stroke_aa_radial
  
  ![screenhots](../tkminnie/tests/screenshots/test_128_rect_stroke_aa_radial.png)
- test_129_rect_stroke_aa_conic
  
  ![screenhots](../tkminnie/tests/screenshots/test_129_rect_stroke_aa_conic.png)
- test_130_rect_stroke_aa_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_130_rect_stroke_aa_pattern.png)
- test_131_rect_stroke_aa_pattern_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_131_rect_stroke_aa_pattern_alpha.png)
- test_132_rect_stroke_aa_pattern_decal
  
  ![screenhots](../tkminnie/tests/screenshots/test_132_rect_stroke_aa_pattern_decal.png)
- test_133_rect_stroke_aa_pattern_decal_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_133_rect_stroke_aa_pattern_decal_alpha.png)
- test_134_ellipse_stroke_aa_linear
  
  ![screenhots](../tkminnie/tests/screenshots/test_134_ellipse_stroke_aa_linear.png)
- test_135_ellipse_stroke_aa_radial
  
  ![screenhots](../tkminnie/tests/screenshots/test_135_ellipse_stroke_aa_radial.png)
- test_136_ellipse_stroke_aa_conic
  
  ![screenhots](../tkminnie/tests/screenshots/test_136_ellipse_stroke_aa_conic.png)
- test_137_ellipse_stroke_aa_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_137_ellipse_stroke_aa_pattern.png)
- test_138_ellipse_stroke_aa_pattern_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_138_ellipse_stroke_aa_pattern_alpha.png)
- test_139_ellipse_stroke_aa_pattern_decal
  
  ![screenhots](../tkminnie/tests/screenshots/test_139_ellipse_stroke_aa_pattern_decal.png)
- test_140_ellipse_stroke_aa_pattern_decal_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_140_ellipse_stroke_aa_pattern_decal_alpha.png)
- test_141_roundrect_stroke_aa_linear
  
  ![screenhots](../tkminnie/tests/screenshots/test_141_roundrect_stroke_aa_linear.png)
- test_142_roundrect_stroke_aa_radial
  
  ![screenhots](../tkminnie/tests/screenshots/test_142_roundrect_stroke_aa_radial.png)
- test_143_roundrect_stroke_aa_conic
  
  ![screenhots](../tkminnie/tests/screenshots/test_143_roundrect_stroke_aa_conic.png)
- test_144_roundrect_stroke_aa_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_144_roundrect_stroke_aa_pattern.png)
- test_145_roundrect_stroke_aa_pattern_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_145_roundrect_stroke_aa_pattern_alpha.png)
- test_146_roundrect_stroke_aa_pattern_decal
  
  ![screenhots](../tkminnie/tests/screenshots/test_146_roundrect_stroke_aa_pattern_decal.png)
- test_147_roundrect_stroke_aa_pattern_decal_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_147_roundrect_stroke_aa_pattern_decal_alpha.png)
- test_148_begin_line_strip_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_148_begin_line_strip_pattern.png)
- test_149_begin_line_strip_pattern_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_149_begin_line_strip_pattern_aa.png)
- test_150_begin_line_strip_pattern_bevel
  
  ![screenhots](../tkminnie/tests/screenshots/test_150_begin_line_strip_pattern_bevel.png)
- test_151_begin_line_strip_pattern_bevel_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_151_begin_line_strip_pattern_bevel_aa.png)
- test_152_begin_line_strip_pattern_decal
  
  ![screenhots](../tkminnie/tests/screenshots/test_152_begin_line_strip_pattern_decal.png)
- test_153_begin_line_strip_pattern_decal_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_153_begin_line_strip_pattern_decal_aa.png)
- test_154_begin_line_strip_pattern_decal_bevel
  
  ![screenhots](../tkminnie/tests/screenshots/test_154_begin_line_strip_pattern_decal_bevel.png)
- test_155_begin_line_strip_pattern_decal_bevel_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_155_begin_line_strip_pattern_decal_bevel_aa.png)
- test_156_begin_line_strip_pattern_diagonal
  
  ![screenhots](../tkminnie/tests/screenshots/test_156_begin_line_strip_pattern_diagonal.png)
- test_157_begin_line_strip_pattern_diagonal_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_157_begin_line_strip_pattern_diagonal_aa.png)
- test_158_begin_line_strip_pattern_bevel_diagonal
  
  ![screenhots](../tkminnie/tests/screenshots/test_158_begin_line_strip_pattern_bevel_diagonal.png)
- test_159_begin_line_strip_pattern_bevel_diagonal_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_159_begin_line_strip_pattern_bevel_diagonal_aa.png)
- test_160_begin_line_strip_pattern_decal_diagonal
  
  ![screenhots](../tkminnie/tests/screenshots/test_160_begin_line_strip_pattern_decal_diagonal.png)
- test_161_begin_line_strip_pattern_decal_diagonal_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_161_begin_line_strip_pattern_decal_diagonal_aa.png)
- test_162_begin_line_strip_pattern_decal_bevel_diagonal
  
  ![screenhots](../tkminnie/tests/screenshots/test_162_begin_line_strip_pattern_decal_bevel_diagonal.png)
- test_163_begin_line_strip_pattern_decal_bevel_diagonal_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_163_begin_line_strip_pattern_decal_bevel_diagonal_aa.png)
- test_164_begin_lines_gouraud
  
  ![screenhots](../tkminnie/tests/screenshots/test_164_begin_lines_gouraud.png)
- test_165_begin_lines_gouraud_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_165_begin_lines_gouraud_aa.png)
- test_166_begin_points_square_gouraud
  
  ![screenhots](../tkminnie/tests/screenshots/test_166_begin_points_square_gouraud.png)
- test_167_begin_points_square_gouraud_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_167_begin_points_square_gouraud_aa.png)
- test_168_begin_points_round_gouraud
  
  ![screenhots](../tkminnie/tests/screenshots/test_168_begin_points_round_gouraud.png)
- test_169_begin_points_round_gouraud_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_169_begin_points_round_gouraud_aa.png)
- test_170_begin_lines_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_170_begin_lines_pattern.png)
- test_171_begin_lines_pattern_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_171_begin_lines_pattern_aa.png)
- test_172_begin_line_strip_miter
  
  ![screenhots](../tkminnie/tests/screenshots/test_172_begin_line_strip_miter.png)
- test_173_begin_line_strip_miter_aa
  
  ![screenhots](../tkminnie/tests/screenshots/test_173_begin_line_strip_miter_aa.png)
- test_174_lines_rand_aa_vbo
  
  ![screenhots](../tkminnie/tests/screenshots/test_174_lines_rand_aa_vbo.png)
- test_175_begin_line_strip_miter_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_175_begin_line_strip_miter_closed.png)
- test_176_begin_line_strip_miter_aa_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_176_begin_line_strip_miter_aa_closed.png)
- test_177_lines_rand2_aa_vbo
  
  ![screenhots](../tkminnie/tests/screenshots/test_177_lines_rand2_aa_vbo.png)
- test_178_line_strip_flat_bevel_32_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_178_line_strip_flat_bevel_32_pattern.png)
- test_179_line_strip_flat_bevel_aa_32_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_179_line_strip_flat_bevel_aa_32_pattern.png)
- test_180_line_strip_flat_bevel_14_2_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_180_line_strip_flat_bevel_14_2_pattern.png)
- test_181_line_strip_flat_bevel_aa_14_2_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_181_line_strip_flat_bevel_aa_14_2_pattern.png)
- test_182_begin_line_strip_bevel_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_182_begin_line_strip_bevel_pattern.png)
- test_183_begin_line_strip_bevel_aa_pattern
  
  ![screenhots](../tkminnie/tests/screenshots/test_183_begin_line_strip_bevel_aa_pattern.png)
- test_184_begin_line_strip_bevel_pattern_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_184_begin_line_strip_bevel_pattern_closed.png)
- test_185_begin_line_strip_bevel_aa_pattern_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_185_begin_line_strip_bevel_aa_pattern_closed.png)
- test_186_begin_line_strip_bevel_pattern_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_186_begin_line_strip_bevel_pattern_alpha.png)
- test_187_begin_line_strip_bevel_aa_pattern_alpha
  
  ![screenhots](../tkminnie/tests/screenshots/test_187_begin_line_strip_bevel_aa_pattern_alpha.png)
- test_188_begin_line_strip_bevel_pattern_alpha_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_188_begin_line_strip_bevel_pattern_alpha_closed.png)
- test_189_begin_line_strip_bevel_aa_pattern_alpha_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_189_begin_line_strip_bevel_aa_pattern_alpha_closed.png)
- test_190_begin_line_strip_bevel_pattern_decal
  
  ![screenhots](../tkminnie/tests/screenshots/test_190_begin_line_strip_bevel_pattern_decal.png)
- test_191_begin_line_strip_bevel_aa_pattern_decal
  
  ![screenhots](../tkminnie/tests/screenshots/test_191_begin_line_strip_bevel_aa_pattern_decal.png)
- test_192_begin_line_strip_bevel_pattern_decal_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_192_begin_line_strip_bevel_pattern_decal_closed.png)
- test_193_begin_line_strip_bevel_aa_pattern_decal_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_193_begin_line_strip_bevel_aa_pattern_decal_closed.png)
- test_194_begin_line_strip_bevel_linear
  
  ![screenhots](../tkminnie/tests/screenshots/test_194_begin_line_strip_bevel_linear.png)
- test_195_begin_line_strip_bevel_aa_linear
  
  ![screenhots](../tkminnie/tests/screenshots/test_195_begin_line_strip_bevel_aa_linear.png)
- test_196_begin_line_strip_bevel_radial
  
  ![screenhots](../tkminnie/tests/screenshots/test_196_begin_line_strip_bevel_radial.png)
- test_197_begin_line_strip_bevel_aa_radial
  
  ![screenhots](../tkminnie/tests/screenshots/test_197_begin_line_strip_bevel_aa_radial.png)
- test_198_begin_line_strip_bevel_conic_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_198_begin_line_strip_bevel_conic_closed.png)
- test_199_begin_line_strip_bevel_aa_conic_closed
  
  ![screenhots](../tkminnie/tests/screenshots/test_199_begin_line_strip_bevel_aa_conic_closed.png)
