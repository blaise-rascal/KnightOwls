
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
    menu_position(0),
    state(0)
{
    bn::music_items::voyagemusic.play(0.5);
}

int main_menu_scene::run_scene()
{
    //bn::random random_generator;
    _display_text("PLACEHOLDER FOR MAIN MENU","PRESS START TO BEGIN GAME");
    while(true)
    {
        //random_num = random_generator.get();

        switch(state)//main menu
        {
            
            case 0:
            {
                if(bn::keypad::start_pressed())
                {
                    bn::core::update();
                    return(1); //next_scene is game scene
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