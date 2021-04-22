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


#include "variable_8x8_sprite_font.h"


int main()
{
    //Must be called before bunato functionality is used
    bn::core::init();

    //text generator
    bn::sprite_text_generator my_text_generator(variable_8x8_sprite_font);
    my_text_generator.set_left_alignment();
    my_text_generator.set_bg_priority(1); 

    //bn::unique_ptr<game_scene> GameScenePointer(new game_scene(my_text_generator));
    //GameScenePointer->update();
}