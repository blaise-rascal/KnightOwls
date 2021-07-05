
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
    state(0)
{
    bn::music_items::voyagemusic.play(0.5);
}

int main_menu_scene::run_scene()
{
    //bn::random random_generator;

    while(true)
    {
        //random_num = random_generator.get();

        switch(state)
        {
            
            case 0:{

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
