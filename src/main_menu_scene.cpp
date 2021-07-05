
#include "bn_core.h"
#include "bn_display.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprites.h"
#include "bn_vector.h"
#include "bn_regular_bg_ptr.h"

#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_string.h"
#include "bn_array.h"
#include "bn_random.h"
#include "bn_sprite_text_generator.h"
#include "bn_fixed_point.h"

#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_items_knight_owls.h"
#include "bn_sprite_items_selection_cursor.h"
#include "bn_sprite_items_right_book_arrow.h"
#include "bn_sprite_items_left_book_arrow.h"
#include "bn_sprite_items_enemies.h"
#include "bn_regular_bg_items_oceanbackground.h"
#include "bn_regular_bg_items_spellbook.h"
#include "bn_regular_bg_items_rift.h"

#include "main_menu_scene.h"

#include "bn_music_items.h"
#include "bn_music_actions.h"
#include "variable_8x8_sprite_font.h"


//TODO: clean up includes, remove from game those things built extraneously by $make clean $make

//soon to do, better slection cursor, better energy bursts

main_menu_scene::main_menu_scene(bn::sprite_text_generator& text_generator):
    my_text_generator(text_generator),
    _selection_cursor_sprite(bn::sprite_items::selection_cursor.create_sprite(0, 30)),
    menu_position(0),
    menu_position_max(1),
    state(0)
{
    bn::music_items::voyagemusic.play(0.5);
    _selection_cursor_sprite.set_visible(false);
    _selection_cursor_sprite.set_z_order(-100);
    _selection_cursor_sprite.set_bg_priority(0); //lower z order means it shows up higher. wacky huh?
}

int main_menu_scene::run_scene()
{
    //bn::random random_generator;
    
    while(true)
    {
        //random_num = random_generator.get();

        switch(state)//main menu
        {
            
            case 0:
            {
                _display_text("ud:MOVE, a:SELECT");
                _generate_virt_menu("BEGIN GAME", "HOW TO PLAY", "CREDITS");
                
                state=1;
                break;
            }
            case 1:
            {
                _navigate_through_virt_menu();//TODO: Make into separate state!

                if(bn::keypad::a_pressed())
                {
                    if(menu_position==0)
                    {
                        bn::core::update();
                        return(1); //next_scene is game scene
                    }
                }
                bn::core::update();
                break;
            }
            default:
            {
                BN_ERROR("Invalid state");
                break;
            }
        }
        
    }
}

//TODO: Put this in its own class maybe
void main_menu_scene::_display_text(const bn::string<50>& statustextone, const bn::string<50>& statustexttwo, const bn::string<50>& statustextthree)
{
    my_text_generator.set_center_alignment();
    status_text_one_sprites.clear();
    status_text_two_sprites.clear();
    status_text_three_sprites.clear();
    if(statustexttwo.length()>0 && statustextthree.length()>0)//all three lines occupied
    {
        my_text_generator.generate(0, 50, statustextone, status_text_one_sprites);
        my_text_generator.generate(0, 61, statustexttwo, status_text_two_sprites);
        my_text_generator.generate(0, 72, statustextthree, status_text_three_sprites);
    }
    else if(statustexttwo.length()>0)//two lines occupied
    {
        my_text_generator.generate(0, 55, statustextone, status_text_one_sprites);
        my_text_generator.generate(0, 67, statustexttwo, status_text_two_sprites);
    }
    else//one line occupied
    {
        my_text_generator.generate(0, 61, statustextone, status_text_one_sprites);
    }
    //todo: error handling for this
}

void main_menu_scene::_generate_virt_menu(const bn::string<12>& menu_option_one, const bn::string<12>& menu_option_two, const bn::string<12>& menu_option_three)
{
    my_text_generator.set_center_alignment();
    first_menu_option_text_sprites.clear();
    second_menu_option_text_sprites.clear();
    third_menu_option_text_sprites.clear();

    my_text_generator.generate(0, 0, menu_option_one, first_menu_option_text_sprites);
    my_text_generator.generate(0, 11, menu_option_two, second_menu_option_text_sprites);
    my_text_generator.generate(0, 22, menu_option_three, third_menu_option_text_sprites);
    menu_position_max = 2;
    menu_position = 0;
    _selection_cursor_sprite.set_visible(true);
    _update_selection_cursor_from_virt_menu_position();

}

void main_menu_scene::_navigate_through_virt_menu()
{
    if(bn::keypad::up_pressed())
    {
        menu_position--;
        if(menu_position<0)
        {
            menu_position = 0;
        }
        _update_selection_cursor_from_virt_menu_position();
    }
    if(bn::keypad::down_pressed())
    {
        menu_position++;
        if(menu_position > menu_position_max)
        {
            menu_position = menu_position_max;
        }
        _update_selection_cursor_from_virt_menu_position();
    }
}


//point_cursor_at_sprite, point_cursor_at_string
//MENU: vector of vector of sprites?
void main_menu_scene::_update_selection_cursor_from_virt_menu_position()
{
    if(menu_position == 0)
    {
        _point_cursor_at_letter(first_menu_option_text_sprites.at(0));
    }
    else if(menu_position == 1){
        _point_cursor_at_letter(second_menu_option_text_sprites.at(0));
    }
    else{ //if you want to see error handling, there is none! bwahaha (really, this should be a switch statement. or the text sprites should be a vector so this whole decision chain would be unnecessary)
        _point_cursor_at_letter(third_menu_option_text_sprites.at(0));
    }
}

void main_menu_scene::_point_cursor_at_letter(const bn::sprite_ptr& target_sprite)
{   
    _selection_cursor_sprite.set_y(target_sprite.y());
    _selection_cursor_sprite.set_x(target_sprite.x()-25);
    _selection_cursor_sprite.set_visible(true);
}