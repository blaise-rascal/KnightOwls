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
        bn::regular_bg_ptr _main_menu_bg;
        bn::sprite_ptr _selection_cursor_sprite;
        bn::sprite_ptr _right_book_arrow_sprite;
        bn::sprite_ptr _left_book_arrow_sprite;
        int menu_position;
        int menu_position_max;
        int state;
        int number_tutorial_pages;
        int current_tutorial_page;

        //yes, I know this shouuld be an array. fight me!!!!
        bn::vector<bn::sprite_ptr, 6> first_menu_option_text_sprites;
        bn::vector<bn::sprite_ptr, 6> second_menu_option_text_sprites; //huh? why isn't this getting overflowed?
        bn::vector<bn::sprite_ptr, 8> third_menu_option_text_sprites;

        bn::vector<bn::sprite_ptr, 50> status_text_one_sprites;
        bn::vector<bn::sprite_ptr, 50> status_text_two_sprites;
        bn::vector<bn::sprite_ptr, 50> status_text_three_sprites;
        
        bn::vector<bn::sprite_ptr, 50> center_text_one_sprites;
        bn::vector<bn::sprite_ptr, 50> center_text_two_sprites;
        bn::vector<bn::sprite_ptr, 50> center_text_three_sprites;
        bn::vector<bn::sprite_ptr, 50> center_text_four_sprites;
        bn::vector<bn::sprite_ptr, 50> center_text_five_sprites;
        bn::vector<bn::sprite_ptr, 50> center_text_six_sprites;
        bn::vector<bn::sprite_ptr, 50> center_text_seven_sprites;
        bn::vector<bn::sprite_ptr, 50> center_text_eight_sprites;
        bn::vector<bn::sprite_ptr, 50> center_text_nine_sprites;
        bn::vector<bn::sprite_ptr, 50> center_text_ten_sprites;

        void _display_status(const bn::string<50>& statustextone, const bn::string<50>& statustexttwo = "", const bn::string<50>& statustextthree = "");
        void _display_center_text(const bn::string<50>& centertextone, const bn::string<50>& centertexttwo = "", const bn::string<50>& centertextthree = "", const bn::string<50>& centertextfour = "", const bn::string<50>& centertextfive = "", const bn::string<50>& centertextsix = "", const bn::string<50>& centertextseven = "", const bn::string<50>& centertexteight = "", const bn::string<50>& centertextnine = "", const bn::string<50>& centertextten = "");
        void _generate_virt_menu(const bn::string<12>& menu_option_one, const bn::string<12>& menu_option_two, const bn::string<12>& menu_option_three = "");
        void _navigate_through_virt_menu();
        void _clear_virt_menu();
        void _point_cursor_at_letter(const bn::sprite_ptr& target_sprite);
        void _update_selection_cursor_from_virt_menu_position();

};
//}



#endif