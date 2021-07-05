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
        int state;
};
//}



#endif