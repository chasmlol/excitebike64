/* A prototype for every function of the game. */
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

/* src/game/render.c */
void render_load_rider(ResFile *r, s32 n, s32 **ids, s32 arg3, s32 arg4, s32 arg5);
void render_update_screen_layout(void);
void render_init(void);
void render_free(void);
Gfx *render_set_viewport(Gfx *dl, Mtx *m, s32 n, s32 flag);
Gfx *render_set_viewport_2(Gfx *dl, s32 p2, s32 p3);
u32 *render_set_viewport_3();
u32 *render_set_viewport_4(u32 *dl, s32 p2, s32 p3);
s32 render_get_rider_lod(s32 x, s32 y, s32 z, s32 k, s32 c, s32 q, s32 n);
Gfx *render_begin_menu_frame(Gfx *gdl, s32 fb, u8 *mtx);
Gfx *render_begin_menu_frame_2();
Gfx *render_draw_sound_menu(Gfx *gdl, s32 fb, Dyn *arg2);
Gfx *render_draw_controls_menu(Gfx *gdl, s32 fb, u8 *mtx);
Gfx *render_clear_viewports(Gfx *gdl);
void render_draw_race(Dyn *dyn, s32 fb, Gfx **pgdl);
Gfx *render_draw_riders(Gfx *gdl, s32 vp, s32 p3, s32 xlu);
void render_update_rider_palette(s32 pl, s32 k);
Gfx *render_restore_state(Gfx *gdl, s32 a1, s32 a2);
Gfx *render_draw_place_markers(Gfx *gdl, s32 arg1);
Gfx *render_draw_temp_gauge(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *render_draw_image(s32 x, s32 y, Img *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw);
void render_update_viewports(void);
Gfx *render_draw_credits(Gfx *gdl);
void render_make_world_mtx(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
Gfx *render_load_ucode(Gfx *gdl, s32 a1, s32 a2, s32 a3, s32 a4);
void render_reset_viewports(void);
void render_apply_split_layout(void);
void render_blit_place_glyph(u8 *src, s32 x, s32 y, s32 w, s32 h);
void render_lerp_rgb(u8 *param_1, u8 *param_2, u8 *param_3, s32 param_4, s32 param_5);
void render_lerp_palette(u16 *p1, u16 *p2, u16 *p3, s32 a, s32 n);

/* src/game/podium.c */
void podium_update_camera(void);
void podium_init(void);
Gfx *podium_draw(Gfx *gdl, s32 x, s32 arg2);
void podium_update(void);
void podium_exit(void);
void podium_rotate_xz(s32 a0, s32 a1, s32 a2, s32 *p4, s32 *p5);

/* src/game/unnamed/s0_8000CD00.c */
void cont_finish_read(void);
s32 cont_finish_read_2(void);
void cont_read_pads(void);
int cont_init(void);
void start_controller_read(void);
void rumble_start(u32 a0, s32 a1);
void rumble_stop_all(void);
void eeprom_write_verify(s32 p1, u8 *p2, s32 p3);

/* src/game/main.c */
void game_main(void);
void main_convert_pad_input(OSContPad *pad, u32 *out);
void main_init_memory(void);
void main_sched_event_thread(void);
void func_8000E8B0(s32 param_1);
void main_init_dev_pi_handle(void);
void main_init_sched(void);
void main_hang(void);
void func_8000E9C0(void);
void func_8000EA24(void);
void func_8000EA2C(void);
void func_8000EA54(void);
s32 func_8000EA5C(void);

/* src/game/unnamed/s0_8000EB00.c */
void debug_printf(s32 a, ...);
u16 isqrt_u32(u32 r);
s32 isqrt_u32_2(u32 r);
u32 isqrt_u32_3(u32 r);
s32 isqrt_u32_4(s32 r);
void fatal_halt(s32 arg0) __attribute__((noreturn));
void *fatal_halt_2(s32);
void fatal_halt_mem_report(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void fatal_halt_mem_report_2();
s32 char_to_upper(s32 a0);
s32 char_to_lower(s32 a0);
void mem_fill(u8 *a0, u8 a1, s32 a2);
void mem_fill_2(void *a0, s32 a1, s32 a2);
void mem_fill_3();
void mem_fill_4(u8 *a0, s32 a1, s32 a2);
void str_cat(char *dst, char *src);
void str_copy_n(u8 *d, u8 *s, s32 n);
s32 str_icmp(u8 *param_1, u8 *param_2);
void critical_enter(void);
void critical_leave(void);
void int_to_str(s32 v, char *out);
void u32_to_hex_str(u32 param_1, char *param_2);
void str_upper(u8 *s);
void str_copy(char *a0, char *a1);
void debug_log();
void debug_log_2(char *buf);
void debug_log_3(char *fmt, ...);
void debug_log_close(void);
u32 random_byte(void);
s32 random_byte_2(void);

/* src/game/unnamed/s0_8000F060.c */
void snd_init(void);
void snd_load_engine_samples(void);
void snd_race_init(Slot_2 *param_1);
void snd_race_exit(void);
void snd_update_riders(void);
void snd_update_rider(s32 arg0);
void snd_play_rider_sfx(s32 k, s32 n, s32 vol);
void snd_drain(void);
void snd_shutdown(void);
void snd_update(void);
void snd_play_sfx(s32 a0);
void snd_set_master_volume(u32 a);
s32 snd_get_master_volume(void);
void snd_trigger_impact(s32 a0, s32 a1, s32 a2, s32 a3);
void snd_trigger_impact_2(s32 a0, s32 a1);
void snd_set_volume(s32 mode, s32 v);
s32 snd_get_volume(s32 param_1);
void snd_stop_all_riders(void);
void snd_load_menu_sounds(void);
void snd_play_menu_sound(s32 param_1);
void func_8001106C(void);
void snd_stop_handle(s32 *p);
void snd_service(void);
void snd_engine_callback(void);
void snd_free_engine_samples(void);
void snd_stop_rider(s32 a0);
void snd_play_rider_sfx_pitch(s32 i, s32 j, s32 vol, s32 pan, s32 arg4);

/* src/game/unnamed/s0_80011490.c */
void mtx_from_angles_pos_scale(MtxS *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
void mtx_from_rot_y_scale(MtxS_2 *m, f32 c, f32 sn, s32 s);
void mtx3i_from_angles(s32 *param_1, s32 param_2, s32 param_3, s32 param_4);
void mtx3i_mul(s32 *a, s32 *b, s32 *c);
void mtx3f_mul_transposed(f32 *r, f32 *a, f32 *b);
void mtx_mul(s16 *a, s16 *b, s16 *o);
void quat_to_angles(f32 *p, s32 *o1, s32 *o2, s32 *o3);
void math_to_polar(s32 x, s32 y, s16 *pAng, s32 *pRatio);
void mtx3f_from_angles(f32 *m, s16 a, s16 b, s16 c);
void r250_init(void);
void mtx3i_test_angles(void);
void mtx_transpose_rot(u8 *p);
void mtx3i_transpose(s32 *p);
void vec3i_mul_mtx3i(s32 *param_1, s32 *param_2, s32 *param_3);
void mtx3i_to_angles(s32 *param_1, u16 *param_2, u16 *param_3, u16 *param_4);
s32 math_acosi(s32 a);
s32 math_asini(s32 x);
f32 math_atan2f(f32 p1, f32 p2);
f32 math_asinf(f32 p);
void mtx3f_transpose(f32 m[3][3]);
void vec3_mul_mtx3f(f32 *v, f32 *m, f32 *o);
void mtx3f_mul_vec3(f32 *a, f32 *b, f32 *o);
void vec3_cross(Vec3f *a, Vec3f *b, Vec3f *r);
f32 vec3_dot(f32 *a, f32 *b);
void quat_from_euler(f32 *out, f32 x, f32 y, f32 z);
void quat_to_mtx3f(f32 *m, f32 *q);
void quat_to_mtx3i(s32 *m, f32 *q);
f32 math_exp(f32 x);
u8 r250_next(void);
u32 math_sin_rand(f32 *p);
void mtx_from_rot3i_pos(s32 *src, u16 *aux, s16 *dst);
void mtx_from_bones(Obj_5 *src, MtxS_2 *dst, s32 n);
void quat_mul(f32 *o, f32 *p, f32 *q);
f32 func_80013D3C(f32 x);

/* src/game/unnamed/s0_80013D50.c */
s32 cic_is_6103(void);

/* src/game/prim.c */
Gfx *prim_build_dl(s32 flags, Model *mdl, Seg_3 **segs, Gfx *gdl);
s32 prim_build_dl_2(s32, void *, s32, s32);
void prim_animate_textures(u32 *param_1, s32 param_2);
void prim_flip_images(ImgSet *s);
void prim_set_tex_mode_callback(s32 a0);
void prim_set_tex_addr_callback(s32 a0);
void prim_init(void);
void prim_exit(void);
void func_80014AD8(s32 x);
void prim_alloc_seg_dl(void **param_1, u32 *param_2, s32 param_3, u32 param_4);
void prim_alloc_dl(u32 *p1, u32 a1, u32 a2, u32 a3);
void prim_flip_image_rows(u8 *p, s32 stride, s32 rows);
void prim_pkg_unrelocate(Hdr **pp);
void prim_pkg_relocate(Hdr **pp);
void prim_n64_relocate(u32 *p);
s32 prim_find_nearest_color(u16 *p, s32 r, s32 g, s32 b, s32 flag, s32 n);

/* src/game/min.c */
void race_init_replay(void);
void race_finish_replay(s32 param_1);
void race_update_replay(void);
void race_init(s32 arg);
void race_exit(void);
void race_begin(void);
void race_update_intro(void);
void race_intro_load(void);
void race_intro_unload(void);
void race_pause_update_options(void);
void race_pause_update_controller(void);
void race_update(u32 *arg);
void race_load_surface_table(Res_3 *r, s32 idx);
void race_load_frontend_code(s32 x);
void race_init_ghost_replay(s32 p);
void race_teardown(void);
void race_apply_volumes(void);
void func_80019EC0(s32 param_1);
void race_set_video_mode(void);
void race_alloc_text_objs(void);
void race_free_text_objs(void);
void race_mute_volumes(void);

/* src/game/bikenew.c */
void bikes_init(void);
void bike_apply_rider_stats(u8 *a0, s32 a1);
f32 bike_wheel_ground_bisect(Obj_7 *o, f32 *p, f32 p3);
void bike_wheel_find_ground(Bike_4 *b, Wheel *w, f32 dt, u8 flag);
void bike_update_wheel_contacts(Bike_5 *b);
void bike_wheel_spring_force(Rec_9 *p, f32 x);
void bike_update_suspension(u8 *s);
void bike_rear_tire_force(Bike_6 *a, Cam *b);
f32 bike_raycast_ground(V3 *a, V3 *b, f32 t, f32 d, f32 *e);
void bike_place_on_ground(Bike_7 *b);
void bike_wheel_surface_bumps(Obj_8 *a0, f32 *a1);
void bike_update_brakes(u8 *p);
void bike_update_drive_force(Bike_8 *p);
void bike_process_input(Obj_9 *o, s32 a1);
void bike_update_tire_forces(Bike_42 *b);
void bike_solve_tire_constraints(Bike_9 *b);
f32 bike_calc_lean_torque(u8 *p);
void bike_compute_forces(Bike_10 *b);
void bike_integrate_velocity(u8 *param_1);
void bike_integrate_position(Bike_11 *b);
void bike_update_render_pose(Bike_42 *b);
void bike_handle_landing(void *a0);
f32 bike_predict_landing(Body *p, f32 g, f32 t, f32 *out);
void bike_align_to_ground(Bike_12 *b);
void bike_update_wheelie(Ent_12 *e);
void bike_update_jump(Bike_13 *b);
void bike_update_alpha(u8 *a2);
void bike_update(Bike_14 *b, s32 n);
u32 bike_remap_buttons(u32 a0, s32 a1);
void bikes_update(s32 n);
float bike_get_ground_height(float *p);
void func_80022908(Obj_10 *p);
void bikes_free(void);
void bike_apply_surface_bumps(u8 *p);
void bike_update_finish_celebration(u8 *p);
void bike_update_dirt_spray(Obj_11 *o);

/* src/game/arngfx.c */
void arn_init(void);
Gfx *arn_draw(Gfx *gdl, s32 a1, s32 a2, s32 view);
Gfx *arn_draw_water(Gfx *gdl, s32 p2, s32 p3, u32 sh);
Gfx *arn_draw_far(Gfx *gdl, s32 a1, s32 a2, s32 mode);
void arn_cull_models(u32 *list, s32 p, s32 strict, s16 *slots, s32 nslots);
void arn_apply_noise_shading(Model_2 *m);
void arn_free(void);
void arn_update_view(s32 param_1);
void arn_relocate(u32 *param);

/* src/game/tubegfx.c */
void tube_init(void);
void tube_free(void);
void tube_update_view(s32 n);
Gfx *tube_draw_layer(Gfx *gdl, s32 mtx, s32 a2, s32 view, s32 fog);
Gfx *tube_draw_far(Gfx *gdl, s32 mtx, s32 a2, s32 view);
Gfx *tube_draw_water(Gfx *gdl, s32 mtx, s32 a2, s32 view);
void tube_cull_sections(List_4 *l, s32 n);
void tube_assign_objects(Lst *a, Lst *b);
s32 tube_get_water_level(s32 x, s32 y, s32 *out);
s32 **tube_build_dlists(Ent_19 *h, s32 p2, u32 p3, s32 p4);
void tube_remap_tex_addr(u32 *param_1);
void tube_init_tex_groups(void);
void tube_load_pvs(void);
void tube_calc_fog_range(s32 a, s32 *b, s32 *c);
void tube_draw(Gfx *gdl, s32 a, s32 b, s32 c);
Gfx *tube_draw_2(Gfx *, Dyn *, s32, s32);
short *tube_alloc_mtx(s32 p1, s32 p2, s32 p3, s32 p4);
Gfx *tube_set_anim_tex_segs(Gfx *gdl, s32 idx);
void tube_free_tex_segs(void);
void tube_reset_objects(void);
void tube_rotate_vertex_colors(Ent_22 *p);
void tube_animate_textures(void);
void tube_spawn_splash(s32 param_1, s32 param_2, s32 param_3, s32 param_4);
void tube_update_splashes(void);
Gfx *tube_tex_mode_callback(Gfx *gdl, s32 flag);

/* src/game/camera.c */
void cam_init(s32 arg0);
void cam_build_segment_order(void);
void cam_update(void);
f32 cam_calc_ground_pitch(Obj_13 *self);
f32 cam_calc_air_pitch(u8 *p);
void cam_update_rider_select(Obj_14 *p, s32 *out, s32 arg2);
int cam_calc_bike_yaw(u8 *s);
s32 cam_mode_soccer(Bike_16 *bike, Rec6 *out);
f32 cam_calc_hill_climb_pitch(u8 *param_1);
void cam_mode_hill_climb(Obj_15 *p, s32 *out);
void cam_update_chase(Bike_17 *p, s32 *out, s32 z);
s32 cam_update_chase_2(Bike_16 *p, Rec6 *out, s32 z);
void cam_mode_fall_view(Ent_23 *e, Rec_4 *r);
void cam_start_fall_view(Ent_24 *p);
void cam_mode_stunt_pip(Bike_18 *p, Rec6 *out);
void cam_smooth_to(Ent_2 *p);
void cam_snap_to(Ent_2 *src);
s32 cam_snap_to_2(Rec6 *src);
void cam_snap_to_moving(S *p, f32 *v);
s32 cam_find_zone(void);
f32 cam_zone_get_progress(Obj_17 *p, s32 a, s32 b);
void cam_zone_update_follow(Obj_18 *param_1, s32 *param_2);
void cam_zone_update_dolly(Actor *act, Rec6 *out);
void cam_get_track_heading(s16 p1, f32 p2, u16 *p3);
void cam_get_track_pos(s16 param_1, f32 param_2, f32 *param_3, f32 *param_4, f32 *param_5);
void cam_collide_walls(s32 a, s32 b, s32 c);
void cam_shift_xz(s32 p1, s32 p2, s32 p3, s32 p4);
void cam_clamp_to_track(s32 m, s32 n);
void cam_clamp_to_track_2(s32 a);
void cam_keep_above_ground(s32 param_1, s32 param_2);
void cam_shift_z(s32 p, s32 q);
void cam_mode_chase_7m(int param_1, int *param_2);
void cam_mode_chase_far_alt(s32 a, s32 *b);
void cam_mode_side_view(Obj_20 *param_1, s32 *param_2);
void func_8002D528(Obj_21 *o, Out_2 *out);
void cam_mode_chase_near(u8 *p, s32 *p2);
void cam_mode_chase_mid(s32 param_1, s32 *param_2);
void cam_mode_chase_far(s32 a, s32 *b);
void cam_mode_celebration(Obj_22 *o, s32 *out);
void cam_mode_first_person(s32 *p1, s32 *p2);
void func_8002D888(Player_10 *arg0, s32 *arg1);
void func_8002D890(void);
void func_8002D898(void);
void cam_free(void);
void cam_get_track_turn(s16 p1, f32 f, u16 *p3);
void cam_get_track_slope(s32 a0, f32 f, s16 *out);
void cam_set_frozen(s32 i, s32 v);

/* src/game/height.c */
void height_init(Area_2 *area, s32 b, s32 c, u32 *polys);
void height_free(void);
s32 height_test_area(s32 id, s32 x, s32 y, s32 z);
s32 height_get_ground(s32 x, s32 y, s32 z);
s32 height_get_desert(s32 x, s32 z);
float height_get_desert_f(float x, float z);
void height_get_ground_color(s32 x, s32 z, s32 *out);
s32 height_get_tri_verts(s32 p1, s32 p2, Vec3i *v);
void height_get_ground_normal(f32 *param_1);
void height_collect_ground_tris(s32 x0, s32 z0, s32 x1, s32 z1, s32 hy);
void height_push_off_wall(f32 *pos);
s32 height_raycast_desert_objs(f32 *p1, f32 *p2, f32 *p3, f32 *p4);
void height_push_out_of_desert_objs(f32 *p);
u8 height_raycast_walls(f32 *start, f32 *end, f32 *out, f32 *tmax, s32 wantFlag);
void height_gather_walls(s32 x0, s32 z0, s32 x1, s32 z1);
void height_init_walls(Hdr_5 **param_1, s32 param_2);
void height_init_walls_reloc(int *param_1);
void height_free_walls(void);
s32 point_in_tri_xz(s32 *p1, s32 *p2, s32 x, s32 y);
int height_get_grid_cell(int x, int y);
s32 point_in_poly_xz(Poly_2 *poly, s32 px, s32 pz);
s32 func_80031FA0(s32 p1, s32 p2, s32 p3);
s32 func_8003207C(s32 p1, s32 p2, s32 p3);

/* src/game/sun.c */
void sun_init(void);
void sun_update_screen_pos(s32 param_1, s32 param_2, s32 arg2);
void sun_check_occlusion(s32 a0);
Gfx *sun_draw_flare(Gfx *gdl, u8 *mtx, s32 idx);
Gfx *sun_draw_stars(Gfx *gdl, s32 arg);
s32 project_to_screen(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9);
void sun_free(void);
s32 zbuffer_decode(s32 x);
s32 sun_alloc_vtx(s32 a, s32 idx);

/* src/game/unnamed/s0_80033E30.c */
void sky_scan_zbuffer(s32 arg0);
void sky_update_scroll(void);
Gfx *sky_draw_strip(s32 a, s32 b, s32 *tex, Gfx *gdl);
Gfx *sky_draw(Gfx *gdl, s32 unused, u8 *s);
s32 sky_get_sun_screen_pos(s32 *outx, s32 *outy, s32 p3, s32 p4);
void func_80035630(void);
void sky_reset_clip(void);
Gfx *sky_texrect(Gfx *gdl, s32 x, s32 y, s32 w, s32 h, s32 u, s32 v, s32 du, s32 dv, s32 x0, s32 y0, s32 x1, s32 y1, u32 *flag);

/* src/game/unnamed/s0_800358C0.c */
void func_800358C0(void);
void engine_update_heat(Obj_27 *o);
void engine_update(s32 idx, s32 dt);
f32 engine_torque_at_rpm(f32 x);
void engine_reset(Vec3f *p);

/* src/game/mud2.c */
void particles_update(void);
void particle_spawn(s32 type, u8 b, V3 *pos, V3 *vel, u8 *col);
void particle_spawn_2(s32 type, s32 b, V3 *pos, V3 *vel, u8 *col);
void particle_spawn_3(s32 a, s32 b, f32 *c, f32 *d, u8 *e);
void particle_spawn_4(s32 a, s32 b, f32 *c, s32 *d, s32 *e);
void particles_emit_sparks(Obj_29 *param_1);
void particles_load_textures(void *param_1);
void particles_emit_bike_dirt(void);
void particles_emit_water_splash(u8 *p);
void particles_emit_dust_cloud(void);
Gfx *particles_draw(Gfx *gdl, s32 view, s32 arg2);
Gfx *rain_draw(Gfx *gdl, s32 mtx, s32 buf);
void particles_init(s32 p);
void particles_free(void);
void particles_start_dust_cloud(Vec3f *src, s32 val);
void rain_init(void);
void rain_free(void);
void particles_start_sparks(u8 *p);
void particles_project_point(f32 *v, s32 *a1, s32 *a2, s32 *a3, s32 *a4, s32 a5, s32 idx);
void particles_start_exhaust_puff(s32 param_1);

/* src/game/shadow.c */
Blk_2 *shadow_clear_texture(Blk_2 *gdl, s32 unused, s32 n);
Gfx *shadow_render_caster(s32 idx, Gfx *gdl, Mtx *m);
Gfx *shadow_draw(s32 id, s32 unused, Gfx *gdl, s32 arg3);
Gfx *shadow_draw_debug_texture(s32 param_1, s32 param_2, Gfx *gdl);
void shadow_build_mesh(s32 n);
void shadow_clip_triangle(Facet_2 *a, s32 lim);
void shadow_init(void);
void shadow_free(void);

/* src/game/skel.c */
void *skel_load(u32 *param_1, s32 param_2);
void skel_update_world_transforms(Node_6 *n, s32 count);
s32 func_8003C588(Rec_20 *recs, s32 n, s32 *out, s32 *mat, s32 *pos, s32 idx);
void skel_make_verts_bone_local(Blk_10 *tbl, List_6 *list);
void skel_reset_pose(s32 *param_1, s32 param_2);
void skel_assign_parents(Ent_35 *base, s32 val, s32 n);

/* src/game/unnamed/s0_8003C8E0.c */
void rider_ik_arm(Joint *j, s32 idx, Vec3i *tgt, Obj_30 *o);
void rider_ik_leg(Joint_2 *j, s32 idx, Vec3i *tgt, s32 *arg3);
void rider_apply_ik(s32 *param_1);
void rider_update_pose(Model_4 *m, s32 i);
void rider_anims_load(u8 *param_1);
void rider_anim_apply(Bone_2 *bones, s32 param_2, s32 param_3);
void bike_update_trick_offset(u8 *param_1);
void bike_skel_update(s32 *p1, s32 p2);
void rider_update_head_look(void);
void rider_update_input_history(Obj_31 *p, s32 n);
void func_8003F128(void);
void rider_update_fist_pump(void);
void rider_anim_overlay_limb(Row_4 *p1, s32 param_2, s32 p3, s32 p4, s32 arg4);
s32 rider_anim_get_length(s32 a0, s32 *a1);
void rider_pose_snapshot(Ent_39 *p, s32 n);
s32 rider_ik_split_len(s32 a, s32 b, s32 c);
void rider_anims_unload(void);

/* src/game/desgfx.c */
void des_init(void);
void des_update_view(s32 arg0);
Gfx *des_draw_near_terrain(Gfx *gdl, Mtx *m, s32 arg2, s32 arg3);
Gfx *des_draw_terrain(Gfx *gdl, s32 a1, s32 a2, s32 a3);
Gfx *des_draw_waypoint_marker(Gfx *gdl, s32 p2, s32 idx, s32 arg3);
Blk_2 *des_draw_arrow(Blk_2 *gdl, s32 p2, s32 arg2, s32 arg3);
void des_calc_lod_grid(void);
void des_update_patches(void);
void des_build_patch(s32 n, s32 x, s32 y, s32 mode);
s32 des_build_patch_dl(s32 mask);
void des_free(void);
void des_set_arrow_angle(s32 a0);
void divmod_floor(s32 a, s32 b, s32 *q, s32 *r);
void des_bbox_add(s32 param_1, s32 param_2);
void des_bbox_reset(void);
Gfx *des_tex_mode_callback(Gfx *gdl, s32 x);
void des_set_waypoint(s32 x);

/* src/game/fractal.c */
void fractal_update_tiles(s32 px, s32 pz);
void fractal_generate(Field *f, s32 n);
s32 fractal_corner_height(s32 a0, s32 a1);
void fractal_subdivide_edge(u8 *buf);
void fractal_tile_init(Gen *p, s32 x, s32 y);
void fractal_place_objects(volatile Field_2 *f);
s32 fractal_get_visible_objects(s32 x, s32 z, s32 ang, s32 range, Elem_2 *out, s32 max);
void fractal_emit_fire_smoke(void);
void fractal_init(s32 param_1);
void fractal_reset(void);
void fractal_generate_step(s32 n);
u8 fractal_rand_byte(void);
s32 fractal_tile_seed(s32 param_1, s32 param_2);

/* src/game/unnamed/s0_800445B0.c */
void bike_crash_begin(Bike_23 *b, s32 n);
void bike_crash_update(Bike_24 *b);
s32 phys_body_step(Body_3 *b, Pt_4 *t, s32 n, u8 m);
void phys_contact_impulse(Body_4 *b, Vec3f *n, Vec3f *p, f32 maxr, Vec3f *c, f32 fr);
f32 phys_check_resting(f32 *a, f32 *b, f32 *h, Vec3f *v, s32 n, f32 thr);
void bike_respawn(Bike_26 *b);
void bike_set_contact_friction(f32 x);
void bike_queue_crash(u8 *p, u8 v);
void bike_queue_crash_2(Bike_5 *b, s32 arg1);
void bike_queue_crash_3(Obj_57 *p, s32 v);
f32 body_time_to_ground(f32 *p);
void rider_change_anim(Obj_35 *o, s32 b);

/* src/game/unnamed/desert.c */
void desert_init(void);
void desert_export_checkpoints(void);
void desert_update_race(void);
void desert_ai_steer(u8 *s);

/* src/game/unnamed/s0_80048230.c */
void font_load_race_set(void *arc);
void font_load_menu_set(u32 *param_1);
void font_unload_set(void);
Gfx *font_gfx_setup(Gfx *g);

/* src/game/path.c */
f32 path_get_distance(s32 a, s32 b);
void path_calc_normals(void);
void path_calc_spline_coeffs(s32 idx, s32 stop);
void path_calc_slope_and_bank(s32 i, s32 end);
void path_calc_curvature(s32 start, s32 stop);
u8 path_find_crest_ahead(f32 x, s32 idx);
void path_calc_curvatures(void);
void path_generate_straight(void);
void path_load(u32 *p);
void path_calc_corner_speed(s32 idx);
void path_apply_braking_limits(s32 end, s32 i);
void path_limit_jump_speeds(void);
void path_calc_widths_and_progress(void);
void path_init(u32 *param_1);
s32 path_find_nearest_node(Vec3f *pos, s32 start, s32 count, u8 mode);
f32 path_get_lateral_offset(Vec3f *pos, s32 idx);
void path_get_segment_and_t(Vec3f *pos, s32 *area, f32 *t, s32 p4);
void path_update_bike(Bike_28 *b);
void path_check_lap(Ent_47 *p, s32 a1);
s32 path_find_segment(Vec3f *pos, s32 start, s32 count, u8 mode);
void path_calc_corner_speeds(s32 param_1, s32 param_2);
void path_free(void);

/* src/game/tunes.c */
void tunes_init(s32 param_1);
s32 tunes_mix_streams(void);
void tunes_update_song(void);
void tunes_play_song(s32 param_1);
void tunes_stop_song(void);
void tunes_update_crowd_queue(void);
void tunes_set_ambient(s32 param_1, s32 param_2, s32 arg2);
void tunes_update_speech(void);
void announce_lead_collision(u32 a, u32 b);
void announce_lap(s32 param_1);
void announce_leader_lap(s32 param_1, s32 param_2);
void announce_place(u32 a, u32 b);
void announce_winner(u32 param_1);
void announce_crash(u32 param_1);
void announce_crash_2(s32 a0);
void announce_crash_landing(u32 param_1);
void announce_takeout(u32 a, u32 b);
void announce_trick(u32 a, s32 b);
void func_8004E3EC(u32 a, u32 b, s32 c);
void announce_place_change(u32 param_1, s32 param_2, u32 param_3);
void announce_finish(u32 p);
void announce_early_leader(u32 param_1);
void announce_best_lap(u32 param_1);
void announce_record(u32 param_1);
void announce_goal(void);
void func_8004F064(u32 param_1);
s32 tunes_is_song_playing(void);
void tunes_shutdown(void);
int tunes_play_speech(s32 param_1, s32 param_2, s32 param_3);
s32 tunes_get_speech_queue_time(void);
u32 tunes_find_sample(u32 key);
void tunes_queue_crowd(s32 p);
void announce_reset(void);
void tunes_queue_speech(s32 a0);
void tunes_stop_speech(void);
void func_8004F5C8(s32 on);
void tunes_mix_callback(s32 *p, s32 n);
void tunes_init_channel(s32 *p);
void tunes_free_channel(u8 *param_1);
void tunes_update(void);
void tunes_apply_echo(s16 *p, s32 n);
void tunes_update_volumes(void);
s32 announce_check_rider(u32 *p);

/* src/game/event.c */
void tutorial_init_lesson(s32 a0);
void tutorial_check_lesson_skip(void);
void tutorial_update(void);
void event_init(s32 a0, s32 a1);
s32 event_check_record_time(s32 *p);
u32 event_rider_finished(s32 p0);
void event_rider_lap_completed(s32 p);
void event_update_countdown(void);
void event_update_hillclimb(void);
void event_update_stunt(void);
void event_update_soccer(void);
void event_update_race(void);
void event_update(s32 arg0);
void event_hud_init(void);
void event_format_time(s32 param_1, char *param_2);
void event_format_time_short(s32 param_1, char *param_2);
void event_set_place_text(u8 *p, s32 n, s32 arg2);
Gfx *event_draw_hud_testride(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *event_draw_hud_hillclimb(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *event_draw_hud_stunt(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *event_draw_hud_gates(Gfx *gdl, s32 a1, s32 a2, s32 idx);
Gfx *event_draw_countdown(Gfx *gdl, s32 a);
void *event_draw_position(void *param_1, s32 param_2, void *param_3);
Gfx *event_draw_hud_race(Gfx *gdl, s32 a1, s32 a2, s32 pl);
Gfx *event_draw_hud_soccer(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *event_draw_glyph_string(Gfx *gdl, s32 arg1, u8 *str, s32 x, s32 y);
Gfx *tutorial_draw_hud(Gfx *gdl, s32 arg1, s32 arg2, s32 arg3);
s32 tutorial_next_lesson_unlocked(void);
Gfx *event_draw_hud(Gfx *gdl, s32 p1, s32 p2, s32 p3);
Gfx *event_draw_temp_gauge(Gfx *gdl, s32 a1, s32 a2, s32 pl);
s32 tutorial_has_prev_lesson(void);
void event_exit(void);
u32 track_res_get(s32 key);
s32 track_res_load_map(s32 *p);
u32 tutorial_lesson_passed(void);
u8 tutorial_lesson_passed_2(void);
s32 event_request_exit(void);
void event_gates_init(void);
void event_stop_sounds_rumble(void);
void event_update_gates(void);
void event_hillclimb_init(void);
void event_stunt_init(void);
void event_soccer_init(void);
void event_reset_places(void);
void event_clear_season_race_points(void);
void func_8005B2E8(void);
void event_hud_free(void);
Gfx *func_8005B404(s32 p1, s32 p2, s32 p3, Gfx *dl, s32 p5);
s32 tutorial_lesson_has_demo(void);
s32 track_res_find(s32 key);

/* src/game/unnamed/s0_8005B5E0.c */
void func_8005B5E0(void);

/* src/game/unnamed/s0_8005B5F0.c */
void ai_update_throttle(u8 *p);
void ai_set_steering(Bike_33 *b, f32 maxv);
void ai_update_steering(Bike_34 *b);
void bike_update_steering(u8 *p, s32 m);
void func_8005C8F4(void);
void func_8005C8FC(void);

/* src/game/decal.c */
void debug_update_value_display(void);
void debug_dump_spline(void);
void debug_record_spline_node(Obj_40 *o);
void debug_ftoa(char *p, f32 x, s32 n);
f32 func_8005CEFC(f32 x);
f32 debug_round_sig3(f32 x);
void debug_print_float(char *param_1, f32 param_2);
void debug_print_float2(char *param_1, f32 param_2, f32 param_3);
void debug_print_vec3(s32 arg0, f32 *v);
void debug_print_int(char *param_1, s32 param_2);
void debug_print_hex(char *param_1, u32 param_2);
void decal_init(void);
void decal_update_tire_tracks(void);
Rec_33 *decal_add_quad(s32 p1, s32 *p2, s32 *p3, s32 *p4, s32 *p5, s32 p6);
Gfx *decal_draw(Gfx *gdl, s32 arg1, Mtx *arg2, s32 arg3);
void decal_free(void);
void decal_end_track(s32 a0);
void decal_clear_all(void);

/* src/game/unnamed/replay.c */
void replay_record_start(u8 *param_1, s32 param_2);
void replay_write_bits(s32 v, s32 n);
void replay_record_frame(u32 *p);
s32 replay_record_finish(void);
void replay_decode_block(void);
void replay_write_header(void);
void replay_read_header(void);
void replay_skip_header(void);
void replay_play_start(s32 param_1);
s32 replay_play_start_2(s32 x);
void replay_play_start_keep_setup(s32 param_1);
s32 replay_play_frame(s32 *dst);
s32 replay_get_info(u8 *s, u32 *a1, u32 *a2, u32 *a3);
s32 replay_validate(u8 *p, s32 n);
u32 calc_checksum24(u8 *p, s32 n);
s32 replay_read_bit(void);
s32 replay_read_bits(s32 n);

/* src/game/collide.c */
Node_16 *collide_build_sphere_tree(Param *p);
s32 collide_spheres_overlap(Bv *a, Bv *b, Xf *x1, Xf *x2, Vec3f *o1, Vec3f *o2);
s32 collide_find_deepest_overlap(Box_3 *p1, Box_3 *p2, Xf_2 *m1, Xf_2 *m2, Vec3f *v1, Vec3f *v2);
s32 collide_resolve_rider_hit(void *p1, void *p2);
s32 collide_apply_impulse(Obj_42 *a, Obj_42 *b);
s32 collide_bodies(u8 *p1, u8 *p2);
void collide_tree_clear_cache(Node_17 *p);
void collide_free_node_pool(void);
void collide_init_node_pool(void);
void collide_reset_body(u8 *p);
void collide_update_bike_speeds(u8 *p);
void collide_reset_tree(Node_18 *p);

/* src/game/unnamed/s0_80061F80.c */
s32 ai_route_lap_ge(s8 *p, s16 v);
s32 ai_route_lap_le(u8 *a0, s16 a1);
u8 ai_route_toggle(void);
s32 ai_route_lap_gt0(u8 *a0);
u8 ai_route_toggle_from_lap(s8 *p, s16 v);
s32 ai_route_random(void);
s32 ai_route_lap_lt_or_wide(Obj_43 *p, s16 s);
s32 ai_route_never(void);

/* src/game/hacks.c */
void hacks_init(void);
void hack_gravel_pit_init(void);
void hack_gravel_pit_update(void);
Gfx *hack_gravel_pit_draw_pass2(Gfx *gdl, s32 unused, s32 p3, s32 p4);
void hack_mill_update(void);
void hacks_update(void);
void hacks_exit(void);
s32 hacks_draw_pass2(s32 a0, Dyn *arg1, s32 arg2, s32 arg3);
s32 hacks_draw_pass1(s32 x, Dyn *arg1, s32 arg2, s32 arg3);
void hack_mill_init(void);
void hack_mill_exit(void);
s32 hack_mill_draw_pass1(s32 x);
void *hack_mill_draw_pass2(void *p);
void hack_gravel_pit_exit(void);
s32 hack_gravel_pit_draw_pass1(s32 x);

/* src/game/unnamed/s0_80062F50.c */
void race_spawn_extra_rider(Spawn *sp);
s32 race_try_despawn_extra_rider(Player_21 *p, Player_21 *q);
void race_update_extra_riders(Player_22 *p);
void race_announce_position_changes(void);
void race_sort_positions(void);
void race_queue_rider_behind(Ent_65 *p, Ent_65 *p2, s32 p3);
void race_queue_rider_ahead(u8 *a, u8 *b, s32 n);
void race_reinsert_queued_rider(u8 *p1, s16 *p2, f32 p3, s32 *p4);
void race_update_riders(void);
void ai_update_lanes(void);
void race_init_ai_skills(s32 arg0, s32 count);
void race_jitter_ai_order(s32 p1, s32 n);
void race_load_extra_rider_spawns(s32 arg0);
void race_free_extra_rider_spawns(void);
void race_set_grid_order(s32 a, s32 b);
void race_reset_grid_order(void);
void race_randomize_ai_order(s32 arg0, s32 count);
void race_init_standings(s32 arg0);
void race_update_rider_queues(u8 *p);
s32 ai_lane_is_blocked(Obj_47 **p, f32 x);
void race_restore_grid_order(void);
void race_apply_grid_order(void);
void race_grid_from_standings(void);

/* src/game/unnamed/sched.c */
void sched_init(u8 *param_1, int arg1);
void sched_thread(void);
void sched_init_vi_mode(s32 p);
void sched_add_client(s32 a0, s32 a1);
void sched_submit_task(s32 a0);
s32 sched_get_current_task(void);
void sched_notify_clients(void *arg);
void sched_run_task(u8 *param_1);
void sched_set_vi_mode(s32 p);
void fault_draw_hex(s32 x, s32 y, u32 bits);
void func_80065A5C(void);
void *sched_get_os_vi_mode(s32 a0);

/* src/game/unnamed/s0_80065AF0.c */
s32 func_80065AF0(void);

/* src/game/instance.c */
s32 instances_load(u32 *a, s32 b, s32 c, Box_4 *d, s32 e);
Gfx *instances_draw(Gfx *gdl, Mtx *m, s32 unused, s32 view, s32 vis);
void instances_free(void);

/* src/game/shatter.c */
s32 shatter_tri_longest_edge(Tri3_2 *t, s32 *outI, s32 *outN, s32 *ox, s32 *oy, s32 *oz);
void shatter_init(Mdl_2 *p);
void shatter_update_pieces(void);
void shatter_check_riders(void);
void shatter_break_object(s32 param_1);
void shatter_reset(void);
void shatter_free(void);
void shatter_update(void);

/* src/game/map.c */
void map_init(void);
Gfx *map_draw_progress_bar(Gfx *gdl);
Gfx *map_draw_rider_dot(Gfx *gdl, s32 player, s32 big);
Gfx *map_draw_progress_marker(Gfx *gdl, s32 idx, s32 half);
Gfx *map_draw_minimap(Gfx *gdl, Dyn *arg1, s32 arg2);
void map_get_rider_pos(s32 player, s32 *px, s32 *py);
void map_build_image(void);
void map_get_node_pos(f32 *a, f32 *b, f32 *c);
Gfx *map_draw_dot(Gfx *gdl, s32 idx, s32 flag, s32 x, s32 y);
void map_stamp_brush(u16 *dst, u16 *src, s32 x, s32 y);
void map_free(void);

/* src/game/palmod.c */
void palmod_register(Obj_51 *a, s32 b);
void palmod_update(void);
void palmod_init(s32 p);
void palmod_free(void);
void palmod_sepia(s32 a0, s32 *a1, s32 *a2, s32 *a3);
void palmod_desaturate_rgb(u16 *dst, u8 *src);
void palmod_desaturate_pal(u16 *dst, u16 *src);

/* src/game/flash.c */
void flash_init(s32 arg0);
void flash_init_2(void);
void flash_update(void);
void flash_free(void);
void flash_start(s32 x);

/* src/game/unnamed/s0_8006B1A0.c */
void excite3d_init(void);
void excite3d_cpu_steer(u8 *p);
void excite3d_update(void);
void excite3d_player_steer(Obj_52 *p);
s32 excite3d_lane_is_clear(Obj_53 *o, s32 n);
u8 excite3d_lane_is_clear_2(Obj_52 *p, s32 a1);
void excite3d_update_progress(Obj_54 *o);
void excite3d_find_segment(Obj_55 *o);

/* src/game/unnamed/s0_8006BFC0.c */
void soccer_ball_init_contacts(void);
void soccer_ball_reset(s32 a0);
void soccer_ball_update(void);

/* src/game/unnamed/s0_8006C510.c */
Gfx *soccer_ball_draw(Gfx *param_1, s32 param_2, s32 param_3);
Gfx *finishline_draw_user_track(Gfx *gdl, Mtx *proj, s32 i, s32 j);
Gfx *finishline_draw_tutorial(Gfx *gdl, Mtx *proj, s32 n);
Gfx *hud_draw_track_intro(Gfx *gdl, s32 arg1, s32 arg2, s32 arg3);
void hud_draw_congratulations(u32 *param_1, Dyn *arg1, s32 arg2);
Gfx *hud_draw_congratulations_2(Gfx *, Dyn *, s32);
void soccer_ball_load_model(void);
void finishline_load_models(void);
void soccer_ball_free_model(void);
void finishline_free_models(void);
void save_init_defaults(void);
s32 save_update_unlocks(void);
void save_load(s32 arg0);
void save_write(s32 x, s32 arg1);

/* src/game/unnamed/s0_8006F620.c */
void stunt_init_input_tables(void);
void stunt_read_input(Ent_78 *e);
void stunt_update(Obj_56 *o);
void stunt_start(Ent_79 *o, s32 n);
s32 stunt_animate(Obj_57 *p, s32 k);
void stunt_add_points(Obj_58 *o);
s32 stunt_seq_less(u8 *a, u8 *b);
void func_80070560(s32 arg0);
s32 stunt_record_done(Obj_59 *p);

/* src/game/ponytail.c */
void ponytail_simulate(s32 idx, Obj_60 *o);
void ponytail_init(s32 n);
void ponytail_free(s32 a0);
void ponytail_update(s32 a0, s32 a1, s32 a2, void *a3);

/* src/game/unnamed/s0_800716C0.c */
void ambient_update(void);
void ambient_load(void);
void ambient_clear(void);

/* src/game/snow.c */
void snow_init(void);
Gfx *snow_draw(Gfx *gdl, s32 mtx, s32 arg2);
void snow_update(void);
void snow_respawn_flake(s32 *param_1);
void snow_free(void);

/* src/game/remove.c */
void remove_walls_init(Mdl2 *m, Obj2 *o);
void remove_walls_trigger(s32 id);
void remove_walls_free(void);
void remove_walls_reset(void);

/* src/game/unnamed/s0_80072F70.c */
Gfx *screen_draw_wipe(Gfx *gdl, Dyn *arg1, s32 arg2);

/* src/game/unnamed/s1_80073200.c */
void game_boot(void);
void idle_thread(s32 param_1);

/* src/game/shell.c */
s32 shell_format_pak_file_label(u8 *src, char *dst, s32 flag);
void shell_blend_palette_to_gray(s32 alpha, u16 *src);
void shell_init(void);
void shell_cb_soccer(s32 param_1);
void shell_cb_season(s32 arg0);
void shell_splash_input(Item_8 *a, Pal *b, u32 *arg2, Pad16 *arg3);
void shell_controller_settings_input(s32 param_1, s32 param_2, u32 *param_3, s8 *param_4);
void shell_records_input(s32 param_1, s32 param_2, u32 *param_3, s32 param_4);
void shell_text_entry_input(Obj_61 *o, s32 unused, s32 *btn, Pad16 *arg3);
void shell_race_results_input(Obj_62 *p1, s32 p2, u32 *p3, Pad16 *arg3);
void shell_season_standings_input(Info *arg0, s32 arg1, u32 *arg2, Pad16 *arg3);
void shell_options_input(Info *arg0, s32 arg1, s32 *arg2, Pad16 *arg3);
void shell_rider_select_input(Obj_63 *p1, s32 p2, u32 *p3, Pad16 *arg3);
void shell_season_select_input(Arg0 *a0, Arg1 *a1, s32 *pad, Pad16 *arg3);
s32 shell_is_round_unlocked(s32 n);
void shell_list_menu_input(Item_9 *item, Ctl_19 *ctl, s32 *btn, Pad16 *arg3);
void shell_filter_stick_dirs(u32 *p);
void shell_update(Pad16 *pad);
void shell_exit(void);
void shell_unload_screen(Scr *s);
void shell_build_player_palettes(void);
void shell_update_season_progress(void);
void shell_load_screen(Item_11 *p);
void shell_goto_screen(Item_12 *p1, s32 p2, s32 p3, s32 arg3);
void shell_goto_screen_2(s32 a, s32 b, s32 c);
void shell_goto_screen_3(void *menu, s32 a1, s32 a2);
void shell_draw_rider_stats(s32 x, s32 y, s32 p3, s32 p4);
Gfx *shell_draw_results_menu(Gfx *gdl, s32 p2, s32 sel, char **strs);
Gfx *shell_draw_players_difficulty_popup(Gfx *gdl, s32 p2, s32 p3);
Gfx *shell_draw_yes_no_dialog(Gfx *gdl, s32 p2, char *str);
void shell_draw_rider_bio(s32 p, s32 q);
Gfx *shell_draw_screen(Mtx *m, s32 fb, Gfx *gdl, Ctl_21 *pad, s32 e);
void shell_draw(Mtx *m, s32 fb, Gfx **pgdl);
void shell_load_gui_res(void);
void shell_free_gui_res(void);
void shell_cb_desert(s32 p);
void shell_cb_select_track(s32 a);
void shell_cb_time_trials(s32 a0);
void shell_cb_tutorial(s32 arg);
void shell_cb_exhibition(s32 x);
void shell_cb_excite3d(s32 p);
void shell_cb_hill_climb(s32 p);
void shell_cb_custom_tracks(s32 x);
void func_800880DC(s32 a0);
void shell_cb_original_excitebike(void);
void shell_cb_stunt_course(int param_1);
s32 shell_pak_track_invalid(u32 *p, s32 n);
void shell_backup_race_results(void);
void shell_restore_race_results(void);
s32 shell_is_round_screen_unlocked(Obj_67 *o);
void shell_main_menu_or_back_input(u8 *p, s32 x, u32 *f, void *s);
s32 shell_is_round_unlocked_tt(s32 p);
s32 func_800884E0(void);

/* src/game/pak.c */
void pak_build_note_list(s32 p, s32 z);
Gfx *pak_draw_button_text(Gfx *gdl, u8 *s, s32 p3, s32 p4, s32 x, s32 y);
void pak_screen_init(void);
void pak_screen_draw(s32 arg0, s32 arg1, Gfx **pgdl);
void pak_refresh(s32 p);
void pak_screen_update(Pad16 *arg0);
void pak_screen_exit(void);
s32 pak_read_note_states(u32 p);
void pak_probe_port(s32 chan);
void pak_probe_all(void);
void pak_query_controllers(void);
void pak_update_free_pages(void);
int pak_get_free_bytes(u32 param_1);
s32 pak_create_note(u32 param_1, char *param_2, char *param_3, s32 param_4, s32 param_5, s32 param_6);
s32 pak_delete_note(s32 channel, s32 file_no);
s32 pak_read_note(u32 param_1, s32 param_2, u32 param_3, u32 param_4, u8 *param_5);
s32 pak_write_note(u32 p1, s32 p2, u32 p3, u32 p4, u8 *p5);
void pak_eeprom_read(s32 c, u8 *buf, s32 n);
s32 pak_repair(u32 param_1);
void pak_ascii_to_pfs(u8 *p, s32 n);
void pak_pfs_to_ascii(u8 *buf, s32 n);
void pak_alloc_note_tables(void);
void pak_free_note_tables(void);
void pak_remove_new_note_entry(s32 idx);
void pak_draw_text(s32 *p1, char *p2, s32 p3, s32 p4, s32 p5, s32 p6);
void pak_set_text(char *param_1, s32 param_2, s32 param_3);
void pak_draw_text_centered(void *param_1, void *param_2, s32 param_3, s32 param_4, s32 param_5, s32 param_6);
void pak_draw_text_right(void *param_1, void *param_2, s32 param_3, s32 param_4, s32 param_5, s32 param_6);
void pak_set_state(s32 a0, s32 a1);
void pak_get_plug_mask(u8 *param_1);
s32 pak_get_plug_mask_2(u8 *param_1);

/* src/game/arnedgfx.c */
void arned_add_wall_quad(s32 x, s32 z, s32 w, s32 h);
Gfx *arned_draw_walls(Gfx *gdl);
void arned_init_grid(s32 w, s32 h);
void arned_set_cell(s32 x, s32 y, s32 w0, s32 b0a, s32 w1, s32 b0b, s32 b0c, s32 h0, s32 h1);
void arned_set_camera(s32 idx, s32 x, s32 y, s32 z, s16 rx, s16 ry);
void arned_set_camera_2(s32 idx, s32 x, s32 y, s32 z, s32 rx, s32 ry);
Gfx *arned_draw(s32 idx, Gfx *gdl, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9);
void arned_load_piece_textures(s32 param_1, s32 param_2, s32 param_3);
void arned_set_object_view(s32 n, s32 id, s32 x, s32 y, s32 w, s32 h, s32 p7, s32 p8, s32 p9, s32 p10, s32 p11, s32 p12);
void arned_add_object(s32 param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
s16 *arned_find_vertex(s32 param_1, s32 param_2);
void arned_free_grid(void);
void arned_transform_piece_verts(Vtx_t *in, s32 n, s32 rot, Vtx_t *out, s32 dx, s32 dz, s32 flag);
void arned_set_viewport(s32 a0, s32 a1, s32 a2, s32 a3);
void func_8008EFFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
void arned_set_scissor(s32 a0, s32 a1, s32 a2, s32 a3);

/* src/game/gui.c */
void gui_init(void);
s32 *gui_draw_message(Gfx *gdl, s32 a1, s32 a2, s32 a3);
Gfx *gui_draw_prompt(Gfx *gdl, s32 arg1, s32 arg2, char *s0, char *s1, char *s2, s32 f1, s32 f2);
s32 gui_menu_input(Ctl_22 *c, s32 arg1);
Gfx *gui_draw_menu(s32 arg0, Gfx *gdl, Menu_5 *m, s32 x, s32 y);
s32 gui_menu_fix_cursor(Ctl_2 *c);
void gui_free(void);

/* src/game/edit.c */
s32 edit_cam_step(void);
void edit_cam_update_target(void);
void edit_init(void);
void edit_exit(void);
void edit_update_name_entry(Game_36 *arg0, s32 arg1);
void edit_update_main_menu(void);
void edit_update_track_menu(void);
void edit_update_place_piece(void);
void edit_remove_preview_pieces(s32 arg0);
void edit_remove_preview_pieces_2(void);
void edit_try_close_loop(Node_28 *n);
void edit_update(void);
Gfx *edit_draw_name_entry(Gfx *gdl, s32 arg1);
void edit_draw(s32 arg0, s32 arg1, Gfx **pgdl);
void edit_update_piece_ports(Rec_49 *r);
void edit_add_overlay_line(s32 idx, s32 x0, s32 y0, s16 z0, s32 x1, s32 y1, s32 z1, u8 r, u8 g, u8 b);
void edit_add_overlay_line_2(s32 idx, s32 x0, s32 y0, s32 z0, s32 x1, s32 y1, s32 z1, s32 r, s32 g, s32 b);
Gfx *edit_draw_overlay_lines(u8 *a0, s32 arg1, Gfx *gdl);
s32 edit_project_to_screen(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, s32 *sz, s32 *sw, s32 arg7, s32 arg8);
Gfx *edit_draw_track_view(s32 arg0, s32 arg1, Gfx *gdl);
Node_29 *edit_insert_piece(Node_29 *a, Node_29 *b, s16 *pt);
void edit_delete_piece(u8 *p);
s32 edit_get_port_x(Blk_22 *b, s32 s);
s32 edit_get_port_y(Spr_4 *p, s32 k);
void edit_place_piece_after(Node_30 *p, Node_30 *c);
void edit_place_piece_before(Spr_5 *a, Spr_5 *b);
void edit_set_start_piece_choices(void);
s32 edit_update_fitting_pieces(Obj_72 *o);
void edit_init_user_pieces(void);
void edit_user_piece_remove_first_segment(void);
void edit_user_piece_remove_last_segment(void);
void edit_user_piece_prepend_segment(void);
void edit_user_piece_append_segment(void);
int func_8009D224(int param_1, u16 *param_2);
s32 func_8009D3D4(s32 p1, s16 *p2);
int func_8009D5D8(int n, u16 *p);
int func_8009D788(int n, s16 *p);
s32 func_8009D98C(s32 n);
s32 func_8009DC6C(s32 param_1);
void edit_user_piece_adjust_height(void);
void edit_update_user_piece(void);
s32 func_8009EB00(s32 a, s32 b);
s32 func_8009EB08(Ctx_4 *c);
void edit_find_start_piece(void);
void edit_user_piece_reset_cursor(s32 arg0);
void edit_calc_track_cam_elevation(void);
void edit_calc_piece_cam_elevation(void);
s32 edit_ease_delta(s32 mode, s32 v, s32 d, s32 f);
void func_8009EF20(s32 a0);
void edit_cam_snap(void);
void edit_clear_track(s32 a0);
void edit_save_track_to_game_pak(void);
s32 edit_can_delete_piece(void);
void edit_reset_overlay_lines(s32 param_1);
void edit_update_user_piece_usage(void);
void edit_free_user_piece_meshes(s32 n);

/* src/game/nesn64.c */
void nes_init(void);
void nes_update_input(volatile s32 *p);
Gfx *nes_draw_setup(s32 a0, s32 a1, Gfx *gdl);
void nes_draw_frame(s32 a0, s32 a1, Gfx **pgdl);
void nes_draw_bg_row(s32 row);
void nes_draw_sprites(s32 arg0);
void nes_draw_back_sprites(s32 param);
void nes_audio_init(void);
void nes_apu_render(s16 *p);
void nes_audio_update(void);
void nes_exit(void);
void nes_audio_free(void);
s32 nes_should_show_exit_msg(void);
void nes_apu_reset_channels(void);
void nes_init_saved_track(u8 *p);
void nes_apu_update_square(s32 idx, s32 mode);
void nes_apu_update_triangle(s32 n, s32 m);
void nes_apu_update_noise(s32 n, s32 m);
void nes_write_io(s32 addr, s32 value);
u32 nes_read_io(s32 code, s32 unused, u32 val);
u32 nes_read_io_2();
u32 nes_read_io_3(u32 a);
s32 nes_read_io_4();
void nes_op_asl(void);
void nes_op_asr(void);
void nes_op_rol(void);
void nes_op_ror(void);
void nes_op_lsr(void);
void nes_op_brk(void);
void nes_op_php(void);
void nes_op_rti(void);
void nes_cpu_nmi(void);
void nes_run_frame(void);
s32 nes_cpu_reset(Nes *machine, void (*rowCallback)(s32));
void nes_op_adc(void);
void nes_op_and(void);
void nes_op_bcc(void);
void nes_op_bcs(void);
void nes_op_beq(void);
void nes_op_bit(void);
void nes_op_bmi(void);
void nes_op_bne(void);
void nes_op_bpl(void);
void nes_op_bvc(void);
void nes_op_bvs(void);
void nes_op_clc(void);
void nes_op_cld(void);
void nes_op_cli(void);
void nes_op_clv(void);
void nes_op_cmp(void);
void nes_op_cpx(void);
void nes_op_cpy(void);
void nes_op_dec(void);
void nes_op_dex(void);
void nes_op_dey(void);
void nes_op_eor(void);
void nes_op_inc(void);
void nes_op_inx(void);
void nes_op_iny(void);
void nes_op_jmp(void);
void nes_op_jsr(void);
void nes_op_lda(void);
void nes_op_ldx(void);
void nes_op_ldy(void);
void nes_op_nop(void);
void nes_op_ora(void);
void nes_op_pha(void);
void nes_op_pla(void);
void nes_op_plp(void);
void nes_op_rts(void);
void nes_op_sbc(void);
void nes_op_sec(void);
void nes_op_sed(void);
void nes_op_sei(void);
void nes_op_sta(void);
void nes_op_stx(void);
void nes_op_sty(void);
void nes_op_tax(void);
void nes_op_tay(void);
void nes_op_tsx(void);
void nes_op_txa(void);
void nes_op_txs(void);
void nes_op_tya(void);
void nes_am_implied_stub(void);
void nes_am_implied(void);
void nes_am_accumulator_stub(void);
void nes_am_accumulator(void);
void nes_am_absolute_stub(void);
void nes_am_absolute(void);
void nes_am_zeropage_stub(void);
void nes_am_zeropage(void);
void nes_am_immediate_stub(void);
void nes_am_immediate(void);
void nes_am_absolute_x_stub(void);
void nes_am_absolute_x(void);
void nes_am_absolute_y_stub(void);
void nes_am_absolute_y(void);
void nes_am_indexed_indirect_stub(void);
void nes_am_indexed_indirect(void);
void nes_am_indirect_indexed_stub(void);
void nes_am_indirect_indexed(void);
void nes_am_zeropage_x_stub(void);
void nes_am_zeropage_x(void);
void nes_am_zeropage_y_stub(void);
void nes_am_zeropage_y(void);
void nes_am_relative_stub(void);
void nes_am_relative(void);
void nes_am_jump_absolute_stub(void);
void nes_am_jump_absolute(void);
void nes_am_jump_indirect_stub(void);
void nes_am_jump_indirect(void);
void nes_ea_absolute(void);
void nes_ea_zeropage(void);
void nes_ea_absolute_x(void);
void nes_ea_absolute_y(void);
void nes_ea_indexed_indirect(void);
void nes_ea_indirect_indexed(void);
void nes_ea_zeropage_x(void);
void nes_ea_zeropage_y(void);
void nes_apu_write(s32 p, u8 v);
s32 nes_cpu_read(s32 addr);
void nes_cpu_write(s32 addr, s32 val);

/* src/game/unnamed/s1_800A42A0.c */
s32 func_800A42A0(void);

/* src/game/UsrTrk.c */
s32 usrtrk_pack_track(u8 *buf);
s16 usrtrk_pack_track_2(u8 *p);
void usrtrk_load_layout(u8 *p);
void usrtrk_load_into_editor(u8 *p);
void usrtrk_layout_from_editor(void);
void usrtrk_load_pieces(void);
void usrtrk_apply_heights(Mesh_8 **dst, Mesh_8 **src, s16 *h, s32 n);
void usrtrk_clone_piece_meshes(Set *out1, Set *out2, s32 count);
void usrtrk_unload(void);
void usrtrk_free_user_piece_meshes(s32 param_1);
void usrtrk_build_track(void);
s16 usrtrk_get_packed_size(void);

/* src/game/sfx.c */
s32 sfx_load_sample_name(u8 *name);
s32 sfx_load_sample_id(s32 arg0);
void sfx_free_sample(s32 param_1);
s32 sfx_play(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 arg5);
void sfx_update_voices(void);
void sfx_set_priority(s32 param_1, s32 param_2);
void sfx_stream_free(s32 *param_1);
void sfx_stream_start(Chan_6 *p, s32 a1, s32 a2, s32 a3, s32 a4);
void sfx_stream_update(Ring *r);
void sfx_get_driver_caps(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 sfx_init(s32 p1, s32 p2, s32 p3, s32 p4, s32 *p5, s32 *p6);
void sfx_shutdown(void);
s32 sfx_create_sample(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5);
void sfx_free_all_samples(void);
void sfx_get_sample_info(s32 arg0, s32 *arg1);
void sfx_set_sample_info(void);
void sfx_write_sample(s32 arg0, u8 *arg1, u32 arg2);
void sfx_stop(s32 id);
void sfx_stop_all(void);
void sfx_pause_all(void);
void sfx_resume_all(void);
void func_800B9138(void);
void func_800B9160(void);
void sfx_set_volume(s32 key, s32 val);
void sfx_set_pan(s32 a, s32 b);
void sfx_set_rate(s32 key, s32 v);
void sfx_set_position(s32 p1, u32 p2);
void sfx_set_end_callback(s32 param_1, s32 param_2, s32 param_3);
void sfx_set_fade(s32 id, s32 v);
s32 sfx_get_volume(s32 param_1);
s32 sfx_get_pan(s32 p1);
s32 sfx_get_rate(s32 param_1);
s32 sfx_get_priority(s32 param_1);
s32 sfx_get_frame_count(void);
void sfx_reap_sounds(void);
void sfx_set_frame_callback(s32 a0);
s32 sfx_get_frame_callback(void);
void sfx_echo_init(s32 arg0);
void sfx_echo_set_level(s32 arg0);
void sfx_echo_free(void);
void func_800B9A68(void);
s32 *sfx_stream_create(s32 p1);
void sfx_stream_write(Ring_2 *r, u32 n, u8 *src);
void sfx_stream_set_rate(Obj_80 *o, s32 a1);
void sfx_stream_set_volume(Handle *h, s32 v);
void sfx_stream_set_pan(s32 *o, s32 v);
void sfx_stream_set_priority(s32 *o, s32 v);
void sfx_mute(void);
void sfx_unmute(void);
void sfx_driver_callback(void);
void sfx_copy_bytes(u8 *d, u8 *s, s32 n);
s32 sfx_str_icmp(u8 *a, u8 *b);
void sfx_get_voice_handles(s32 *out);
Ent_99 *sfx_alloc_sound(void);
void sfx_free_sound(s32 *p);
void sfx_insert_sound(Node_45 *n);
void sfx_unlink_sound(Node_46 *n);
s32 sfx_find_sound(s32 param_1);
u32 sfx_get_position(s32 p);
void sfx_set_sample(s32 p1, s32 p2);
void sfx_echo_enable(s32 arg0);
u32 sfx_stream_read(Ring_3 *r, u32 n, u8 *dst);
void sfx_lock_sample(s32 i);
void func_800BA5F0(s32 a0);
s32 sfx_get_voice(s32 param_1);

/* src/game/softmix.c */
s32 softmix_init(s32 param_1, u32 param_2, s32 param_3, s32 arg3);
s32 softmix_load_sample_file(s32 param_1);
s32 softmix_load_sample_name(s32 param_1, s32 arg1, s32 arg2, s32 arg3);
s32 softmix_load_sample_id(s32 param);
s32 softmix_create_sample(u8 *p1, s32 p2, s32 p3, s32 p4, s32 p5);
void softmix_voice_set_sample(s32 param_1, s32 param_2);
void softmix_voice_set_sample_2(s32 a);
void softmix_voice_set_volume(s32 i, s32 val);
void softmix_voice_set_volume_2(s32 a);
void softmix_voice_set_pan(s32 i, s32 val);
void softmix_voice_set_pan_2(s32 a);
void softmix_mix(s32 *param_1, s32 param_2);
void softmix_sam_decompress(SamHeader **param_1);
s32 softmix_get_caps(s32 *a, s32 *b, s32 *c);
void softmix_shutdown(void);
void softmix_free_sample(u32 p);
void softmix_get_sample_info(s32 idx, s32 *out);
void softmix_set_sample_info(s32 i, u32 *src);
void softmix_set_sample_info_2(s32 a);
void softmix_voice_set_rate(s32 param_1, s32 param_2);
void softmix_voice_set_rate_2(s32 a);
void softmix_voice_set_position(s32 i, s32 v);
void softmix_voice_set_position_2(s32 a);
s32 softmix_voice_get_position(s32 a0);
void softmix_voice_set_flags(s32 n, u32 flags);
void softmix_voice_set_flags_2(s32 a);
s32 softmix_voice_get_flags(s32 i);
s32 softmix_get_sample_count(s32 arg0);
s32 softmix_get_sample_count_2(void);
void softmix_convert_output(s32 a, s32 b, s32 c);
s32 softmix_echo_init(s32 size);
void softmix_echo_enable(s32 p);
void softmix_echo_set_level(s32 k);
void softmix_echo_free(s32 arg0);
void softmix_write_sample(u32 idx, u8 *src, s32 n);
void softmix_write_sample_2(s32 a);
void softmix_set_decompressor(SamHeader *(*fn)(SamHeader *));
void softmix_set_decompress_buf(s32 a, s32 b);
void softmix_voice_update_levels(s32 i);
s32 softmix_get_sample_data(s32 a0);
void softmix_set_master_volume(s32 x);

/* src/game/n64snd.c */
s32 n64snd_init(s32 param_1, s32 param_2, s32 param_3, s32 param_4, u32 *param_5, s32 *param_6);
void n64snd_update(void);
s32 n64snd_get_caps(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3);
void n64snd_shutdown(void);
void n64snd_load_sample_named(s32 a, s32 b, s32 c, s32 d);
void n64snd_load_sample_index(s32 a);
void n64snd_create_sample(s32 a, s32 b, s32 c, s32 d, s32 e);
void n64snd_free_sample(u32 a);
void n64snd_get_sample_info(s32 param_1, s32 *param_2);
void n64snd_set_sample_info(s32 a);
void n64snd_write_sample_data(s32 a);
void n64snd_voice_set_sample(s32 a);
void n64snd_voice_set_rate(s32 a);
void n64snd_voice_set_volume(s32 a);
void n64snd_voice_set_pan(s32 a);
void n64snd_voice_set_position(s32 a);
void n64snd_voice_get_position(s32 a);
void n64snd_voice_set_flags(s32 a);
void n64snd_voice_get_flags(s32 a);
void n64snd_get_sample_clock(s32 a);
u32 n64snd_get_buffer_clock(void);
void n64snd_set_callback(s32 v);
void n64snd_echo_init(s32 a);
void n64snd_echo_enable(s32 a);
void n64snd_echo_set_level(s32 a);
void n64snd_echo_free(s32 a);
void n64snd_stub1(void);
void n64snd_stub2(void);
void n64snd_stub3(void);
void n64snd_set_mute(s32 a0);
void n64snd_clear_output(void);

/* src/game/unnamed/lib_800BC3D0.c */
void mem_report(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void mem_report_2(void);
void mem_report_totals(void);
void *mem_alloc_dbg(s32 size, u32 flags, s32 owner, s32 tag);
s32 *mem_alloc_dbg_2(s32 a, s32 b, void *c, s32 d);
void mem_free(void *ptr);
s32 mem_free_2(void *ptr);
void *mem_alloc_at_dbg(void *ptr, s32 size, s32 tag, s32 id);
s32 mem_dump_blocks(s32 a, u32 b, s32 c);
void mem_report_clear_mark(void);
void mem_report_set_mark(void);
s32 mem_init(s32 p0, s32 p1, s32 a, s32 b);
s32 *mem_calloc_dbg(s32 a0, s32 a1, s32 a2, s32 a3);
s32 *mem_realloc_dbg(s32 *p, u32 size, s32 a, s32 b, s32 c);
s32 mem_check(void);
s32 mem_get_size(s32 *p);
void mem_alloc(s32 a, s32 b);
void mem_calloc(u32 a, u32 b);
void mem_realloc(s32 *a, s32 b, s32 c);
void mem_alloc_at(s32 a, s32 b);
Block *mem_format_heap(Block **start, Block **end);
s32 mem_validate(Block *start, Block *end, Block *list);
void str_cat_hex(char *a0, u32 a1);

/* src/game/unnamed/lib_800BDCB0.c */
s32 swd_decode_token(s32 *p1, u32 *p2);
s32 swd_decompress(void);
void swd_reset_bits(void);
u32 swd_read_bits(s32 param_1);

/* src/game/file.c */
s32 file_init(u8 *src);
Ent_106 *file_load_page(Obj_81 *so, s32 si, s8 sf);
s32 file_shutdown(void);
Slot_17 *file_open(u8 *param_1, char *arg1);
void file_close(Q_2 *q);
void file_seek(s32 *o, s32 v, s32 mode);
s32 file_tell(s32 *p);
s32 file_read(u8 *buf, s32 size, s32 count, Stream_2 *f);
s32 file_eof(s32 *a0);
u8 *file_find(u8 *param_1);
Node_51 *file_get_page(s32 *k, s32 id, s8 flag);
void file_read_rom(void *param_1, void *param_2, s32 param_3);

/* src/game/unnamed/lib_800BEC20.c */
s32 dma_start(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
void dma_update(void);
s32 dma_init(void);
s32 dma_get_status(s32 a0);
void dma_release(s32 a0);
void dma_wait(s32 x);

/* src/game/unnamed/lib_800BF040.c */
s32 dma_epi_start(s32 a0, u16 *a1, s32 a2);

/* src/game/unnamed/lib_800BF0E0.c */
s32 dma_pi_get_cmd_queue(void);
u32 *dma_cart_rom_init(void);

/* src/game/unnamed/lib_800BF280.c */
s32 dma_jam_mesg(Q_3 *q, s32 v, s32 mode);

/* src/game/unnamed/lib_800BF3C0.c */
Blk_29 *sram_init(void);
int sram_read(u32 param_1);
s32 sram_write(s32 p);

/* src/game/resman.c */
s32 res_open_set(char *path, Obj_82 **out);
u32 res_load(u32 *pa0, s32 pa1);
void res_register_type(u8 *name, s32 val);
void res_set_clear(s32 *p);
void res_close_set(s32 *param_1);
void res_unload(u32 *p);
s32 res_unload_2(u32 *p);
void res_unload_all(ListHdr *h);
void res_load_by_name(u32 *param_1, u8 *param_2, u32 *param_3);
void *res_load_by_name_2(void *a0, void *a1, void *a2);
s32 res_find_gid(u8 *a0, u8 *a1);
s32 res_get_size(ResFile_2 *r, s32 idx);
void res_set_load_buffer(s32 a0);
void func_800C0264(s32 *param_1, u32 param_2);
u32 res_hash_name(u8 *p);

/* src/game/decoder.c */
SamHeader *decoder_decode_adpcm(SamAdpcm *s);
SamHeader *decoder_decode_sample(SamHeader *sam);
SamHeader *func_800C055C(SamAdpcm *p);

/* src/game/fontcode.c */
s32 font_load(ResFile *arc, u32 **ids, char *name);
u8 *font_draw_text(s32 *p, s32 x, s32 y);
s32 font_get_char_pos(Obj_83 *p, s32 x, s32 unused, s32 n);
u8 *font_measure_text(u8 *s, Font_6 *f, u32 *outMax, s32 *outW);
Gfx *font_render(Gfx *gdl);
Gfx *font_render_pages_scaled(Gfx *gdl, Spr **list);
Gfx *font_render_pages(Gfx *gdl, Spr **lists);
s32 font_pack_glyphs(u8 *hdr);
s32 font_alloc_glyph_space(u32 *map);
void font_set_scale(s32 a, s32 b, s32 c, s32 d);
u32 font_unload(void);
s32 *font_text_create(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
u8 *font_text_clear(u8 *p);
char *font_text_add(Text *p, char *s);
void func_800C204C(s32 arg0);
void str_make_initials(char *dst, char *src);
void func_800C20F0(u8 *s);
void font_name_upper(u8 *s);
void func_800C21BC(u8 *s);
void func_800C21F0(u8 *s);
s32 font_reset_queue(void);

/* libultra functions its headers don't declare, and other views of libultra functions */
s32 __osDisableInt(void);
void __osRestoreInt(s32);
void __osPiGetAccess(void);
void __osPiRelAccess(void);
s32 osAiGetStatus_2(void);
s32 strlen(char *s);
u32 strlen_2(char *s);
void *memcpy(void *, const void *, u32);
void memcpy_2(void *dst, void *src, s32 n);
void __osEnqueueAndYield(s32 *p);
s32 *__osPopThread(Q_3 *q);
s32 __osSpGetStatus(void);
void __osMotorAccess(void *a, s32 c);
void __osInitialize_autodetect(void);

/* other code */
void tunes_decode_adpcm_block(s32 src, s16 *dst, s32 count);
void softmix_mix_voices_mono(void);
void softmix_mix_voices_stereo(void);
void softmix_output_8bit(void);
void softmix_output_16bit(void);
void decoder_decode_ima_block(u8 *src, u16 *dst, s32 count);
void decoder_decode_msadpcm_block(u8 *a, u16 *b, s32 c, u8 *d);
int abs(int);
char *strcpy(char *, const char *);

#endif
