
//See game_scene.cpp for info & license



#include "bn_core.h"
#include "bn_display.h"

#include "bn_fixed_point.h"




/*#include "bn_sprite_ptr.h"
#include "bn_display.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprites.h"
#include "bn_vector.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_fixed_point.h"*/
#include "bn_sprite_text_generator.h"


#include "game_scene.h"

#include "main_menu_scene.h"
//SCENES
//0 is menu state
//1 is game state
#include "variable_8x8_sprite_font.h"

int main()
{
    //Must be called before butano functionality is used
    bn::core::init();
    int next_scene = 0;
    //text generator
    bn::sprite_text_generator my_text_generator(variable_8x8_sprite_font);
    my_text_generator.set_left_alignment();
    my_text_generator.set_bg_priority(1); 
    
    //bn::unique_ptr<game_scene> GameScenePointer(new main_menu_scene(my_text_generator));
    //next_scene = GameScenePointer->run_scene();
    while(true)
    {
        
        switch(next_scene)
        {
            case 0:
            {
                bn::unique_ptr<main_menu_scene> MainMenuScenePointer(new main_menu_scene(my_text_generator));
                next_scene = MainMenuScenePointer->run_scene();
                MainMenuScenePointer.reset();
                break;
            }
            case 1:
            {
                bn::unique_ptr<game_scene> GameScenePointer(new game_scene(my_text_generator));
                next_scene = GameScenePointer->run_scene();
                GameScenePointer.reset();
                break;
            }
            default:
            {
                BN_ERROR("Invalid next scene!!!");
                break;
            }
        }
        
    }
    //TODO: Add menuscene
    //put random number generation into main state
    //return next scene

}