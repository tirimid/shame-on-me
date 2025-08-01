// SPDX-License-Identifier: GPL-3.0-or-later

#define INCGENMODEL(name) \
	extern unsigned const name##_vert_cnt; \
	extern unsigned const name##_idx_cnt; \
	extern float const name##_verts[]; \
	extern unsigned const name##_idxs[];

#define INCXXD(name) \
	extern unsigned char const name[]; \
	extern unsigned const name##_len;

#define INCXXDMUT(name) \
	extern unsigned char name[]; \
	extern unsigned name##_len;

// data generated by gen-model.
INCGENMODEL(card_stack_obj)
INCGENMODEL(cube_obj)
INCGENMODEL(door_closed_obj)
INCGENMODEL(door_open_obj)
INCGENMODEL(lightbulb_obj)
INCGENMODEL(plane_obj)
INCGENMODEL(table_obj)
INCGENMODEL(window_obj)

// immutable data generated by xxd.
INCXXD(vcr_osd_mono_ttf)
INCXXD(black_png)
INCXXD(black50_png)
INCXXD(c_back_png)
INCXXD(c_clubs_6_png)
INCXXD(c_clubs_7_png)
INCXXD(c_clubs_8_png)
INCXXD(c_clubs_9_png)
INCXXD(c_clubs_10_png)
INCXXD(c_clubs_j_png)
INCXXD(c_clubs_q_png)
INCXXD(c_clubs_k_png)
INCXXD(c_clubs_a_png)
INCXXD(c_diamonds_6_png)
INCXXD(c_diamonds_7_png)
INCXXD(c_diamonds_8_png)
INCXXD(c_diamonds_9_png)
INCXXD(c_diamonds_10_png)
INCXXD(c_diamonds_j_png)
INCXXD(c_diamonds_q_png)
INCXXD(c_diamonds_k_png)
INCXXD(c_diamonds_a_png)
INCXXD(c_hearts_6_png)
INCXXD(c_hearts_7_png)
INCXXD(c_hearts_8_png)
INCXXD(c_hearts_9_png)
INCXXD(c_hearts_10_png)
INCXXD(c_hearts_j_png)
INCXXD(c_hearts_q_png)
INCXXD(c_hearts_k_png)
INCXXD(c_hearts_a_png)
INCXXD(c_spades_6_png)
INCXXD(c_spades_7_png)
INCXXD(c_spades_8_png)
INCXXD(c_spades_9_png)
INCXXD(c_spades_10_png)
INCXXD(c_spades_j_png)
INCXXD(c_spades_q_png)
INCXXD(c_spades_k_png)
INCXXD(c_spades_a_png)
INCXXD(ceiling_png)
INCXXD(door_png)
INCXXD(dummy_png)
INCXXD(dummy_face_png)
INCXXD(eyes_dummy_png)
INCXXD(eyes_dummy_face_png)
INCXXD(floor_png)
INCXXD(glasses_dummy_png)
INCXXD(glasses_dummy_face_png)
INCXXD(gray_png)
INCXXD(lightbulb_png)
INCXXD(something_png)
INCXXD(table_png)
INCXXD(wall_png)
INCXXD(window_png)
INCXXD(bg_ominous_ogg)
INCXXD(card0_ogg)
INCXXD(card1_ogg)
INCXXD(card2_ogg)
INCXXD(card3_ogg)
INCXXD(card4_ogg)
INCXXD(card5_ogg)
INCXXD(card6_ogg)
INCXXD(card7_ogg)
INCXXD(card8_ogg)
INCXXD(card_shuffle_ogg)
INCXXD(explode_ogg)
INCXXD(explode_muffled_ogg)
INCXXD(knock_ogg)
INCXXD(speak_arkady_ogg)
INCXXD(speak_gerasim_ogg)
INCXXD(speak_matthew_ogg)
INCXXD(speak_peter_ogg)

// mutable data generated by xxd.
INCXXDMUT(base_frag_glsl)
INCXXDMUT(base_vert_glsl)
INCXXDMUT(overlay_frag_glsl)
INCXXDMUT(overlay_vert_glsl)
INCXXDMUT(shadow_frag_glsl)
INCXXDMUT(shadow_geo_glsl)
INCXXDMUT(shadow_vert_glsl)
