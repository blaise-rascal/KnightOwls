
#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "bn_core.h"
#include "bn_display.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprites.h"
#include "bn_vector.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_sprite_text_generator.h"
//#include "bn_sprite_items_chiyu.h"


#include "bn_fixed_point.h"

/*

REFERENCE ABOUT MEANING OF :: OPERATOR

namespace abc {
    int variable;
};

int main(){
    cin >> abc::variable;
}
*/

/*
namespace bn
{
    class sprite_text_generator;
}*/

//#include "variable_8x8_sprite_font.h"

class game_scene
{
    public:
        game_scene(bn::sprite_text_generator& text_generator);//TODO: Add background??
        void update();

    private:
        bn::sprite_text_generator& my_text_generator;
        bn::sprite_ptr _chiyu_sprite;

        //bn::sprite_ptr bomb_sprite;
        //bomb_sprite.set_bg_priority(2); 
        //bn::sprite_ptr selection_cursor_sprite;

        //bf::status status;

        //string declarations
       /* bn::string<20> weight_hud_text;
        const bn::string<6> deploy_label_text("DEPLOY");
        const bn::string<4> pass_label_text("PASS");
        //bn::sprite_text_generator& _text_generator;
*/

        int current_weight;
        //weight_hud_text.append(bn::to_string<8>(current_weight));

    /*

        int menu_position = 0;
        const int MENU_POSITION_MAX = 1;

        //declare text sprites*/
        bn::vector<bn::sprite_ptr, 32> weight_text_sprites;/*
        bn::vector<bn::sprite_ptr, 6> deploy_text_sprites;
        bn::vector<bn::sprite_ptr, 4> pass_text_sprites;
        bn::sprite_text_generator& pointer_to_text_generator;*/
        
        void _update_weight_text();
    
        //void _update_selection_cursor(int menu_position, bn::sprite_ptr selection_cursor_sprite);
};
//}

#endif