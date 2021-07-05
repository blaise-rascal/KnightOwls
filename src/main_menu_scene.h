//See game_scene.cpp for info & license





#ifndef MAIN_MENU_SCENE_H
#define MAIN_MENU_SCENE_H

#include "bn_core.h"
#include "bn_display.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprites.h"
#include "bn_vector.h"
#include "bn_array.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
//#include "bn_sprite_items_chiyu.h"


#include "bn_fixed_point.h"


class main_menu_scene
{
    public:
        main_menu_scene(bn::sprite_text_generator& text_generator);//TODO: Add background??
        int run_scene();

    private:
        bn::sprite_text_generator& my_text_generator;
        bn::sprite_ptr _selection_cursor_sprite;
        int menu_position;
        int menu_position_max;
        int state;

        //fyi: I know this shouuld be an array
        bn::vector<bn::sprite_ptr, 6> first_menu_option_text_sprites;
        bn::vector<bn::sprite_ptr, 6> second_menu_option_text_sprites; //huh? why isn't this getting overflowed? the second menu option is "examine" which is more than 4 sprites...
        bn::vector<bn::sprite_ptr, 8> third_menu_option_text_sprites;
        bn::vector<bn::sprite_ptr, 8> fourth_menu_option_text_sprites;

        bn::vector<bn::sprite_ptr, 50> status_text_one_sprites;
        bn::vector<bn::sprite_ptr, 50> status_text_two_sprites;
        bn::vector<bn::sprite_ptr, 50> status_text_three_sprites;


        void _display_text(const bn::string<50>& statustextone, const bn::string<50>& statustexttwo = "", const bn::string<50>& statustextthree = "");
        void _generate_virt_menu(const bn::string<12>& menu_option_one, const bn::string<12>& menu_option_two, const bn::string<12>& menu_option_three = "");
        void _navigate_through_virt_menu();
        void _clear_virt_menu();
        void _point_cursor_at_letter(const bn::sprite_ptr& target_sprite);
        void _update_selection_cursor_from_virt_menu_position();

};
//}



#endif