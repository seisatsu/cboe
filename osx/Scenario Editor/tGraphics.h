void	Set_up_win ();
void run_startup_g();
void 	load_main_screen();
void 	draw_main_screen();
void 	redraw_screen();
void draw_lb();
void draw_lb_slot (short which,short mode) ;
void draw_rb();
void draw_rb_slot (short which,short mode) ;
void set_up_terrain_buttons();
GWorldPtr load_pict(int picture_to_get);
void draw_terrain();
void draw_monsts();
Rect get_item_template_rect (short type_wanted);
void draw_items();
void force_tiny_redraw();
void redraw_selected_ter();
void draw_one_terrain_spot (short i,short j,unsigned char terrain_to_draw);
void draw_one_tiny_terrain_spot (short i,short j,unsigned char terrain_to_draw);
void Draw_Some_Item ( GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,location target,
char masked,short main_win);
void rect_draw_some_item (GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,Rect targ_rect,
char masked,short main_win);
Rect get_template_rect (unsigned char type_wanted);
void draw_frames();
void place_location();
void place_just_location();
void set_string(char *string,char *string2);
void undo_clip();
Boolean is_special(short i,short j);
void take_special(short i,short j);
void make_special(short i,short j);
void sort_specials();
Boolean is_field_type(short i,short j,short field_type);
void make_field_type(short i,short j,short field_type);
void take_field_type(short i,short j,short field_type);
Boolean is_web(short i,short j);
void make_web(short i,short j);
void take_web(short i,short j);
Boolean is_crate(short i,short j);
void make_crate(short i,short j);
void take_crate(short i,short j);
Boolean is_barrel(short i,short j);
void make_barrel(short i,short j);
void take_barrel(short i,short j);
Boolean is_fire_barrier(short i,short j);
void make_fire_barrier(short i,short j);
void take_fire_barrier(short i,short j);
Boolean is_force_barrier(short i,short j);
void make_force_barrier(short i,short j);
void take_force_barrier(short i,short j);
Boolean is_sfx(short i,short j,short type);
void make_sfx(short i,short j,short type);
void take_sfx(short i,short j,short type);
Boolean is_quickfire(short i,short j);
void make_quickfire(short i,short j);
void take_quickfire(short i,short j);
Boolean container_there(location l);
void char_win_draw_string(WindowPtr dest_window,Rect dest_rect,char *str,short mode,short line_height);
void char_port_draw_string(GrafPtr dest_window,Rect dest_rect,char *str,short mode,short line_height);
void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height);
void c2p(Str255 str) ;
void p2c(Str255 str);
void get_str(Str255 str,short i, short j);
short string_length(char *str);
Rect get_custom_rect (short which_rect);
