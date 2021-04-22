#include "bn_core.h"
#include "bn_display.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprites.h"
#include "bn_vector.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_sprite_text_generator.h"


#include "bn_fixed_point.h"

#include "bn_sprite_items_chiyu.h"
#include "bn_sprite_items_selection_cursor.h"
#include "bn_sprite_items_hero_bomb_icon.h"

#include "game_scene.h"

#include "variable_8x8_sprite_font.h"
/*

REFERENCE ABOUT MEANING OF :: OPERATOR

namespace abc {
    int variable;
};

int main(){
    cin >> abc::variable;
}
*/



namespace{
        //initialize sprites
        bn::sprite_ptr chiyu_sprite = bn::sprite_items::chiyu.create_sprite(-120, -80);
        bn::sprite_ptr bomb_sprite = bn::sprite_items::hero_bomb_icon.create_sprite(10, 0);
        //bomb_sprite.set_bg_priority(2); 
        bn::sprite_ptr selection_cursor_sprite = bn::sprite_items::selection_cursor.create_sprite(-30, 30);

        //bf::status status;

        //string declarations
        bn::string<20> weight_hud_text("WEIGHT: ");
        const bn::string<6> deploy_label_text("DEPLOY");
        const bn::string<4> pass_label_text("PASS");
        bn::sprite_text_generator& _text_generator;


        int current_weight = 0;
        //weight_hud_text.append(bn::to_string<8>(current_weight));

    

        int menu_position = 0;
        const int MENU_POSITION_MAX = 1;

        //declare text sprites
        bn::vector<bn::sprite_ptr, 32> weight_text_sprites;
        bn::vector<bn::sprite_ptr, 6> deploy_text_sprites;
        bn::vector<bn::sprite_ptr, 4> pass_text_sprites;

        
}

void game_scene::game_scene(bn::sprite_text_generator& text_generator):

pointer_to_text_generator(text_generator)
{
    //generate text
    //my_text_generator.generate(-100, 0, weight_hud_text, weight_text_sprites);
    pointer_to_text_generator
    text_generator.generate(-100, 40, deploy_label_text, deploy_text_sprites);
    text_generator.generate(10, 40, pass_label_text, pass_text_sprites);
    _update_weight_text();
}

void game_scene::update()
{
    while(true)
    {
        if(bn::keypad::a_pressed())
        {
            current_weight++;
            weight_text_sprites.clear();
            _update_weight_text();

        }
        if(bn::keypad::left_pressed())
        {
            menu_position--;
            if(menu_position<0)
            {
                menu_position = MENU_POSITION_MAX;
            }
            _update_selection_cursor(menu_position, selection_cursor_sprite);
        }
        if(bn::keypad::right_pressed())
        {
            menu_position++;
            if(menu_position > MENU_POSITION_MAX)
            {
                menu_position = 0;
            }
            _update_selection_cursor(menu_position, selection_cursor_sprite);
        }

        bn::core::update();
    }
}

void _update_selection_cursor(int menu_position, bn::sprite_ptr selection_cursor_sprite)
{
    if(menu_position == 0)
    {
        selection_cursor_sprite.set_x(-30);
    }
    else{
        selection_cursor_sprite.set_x(30);
    }
}

void _update_weight_text()
{
    weight_hud_text="WEIGHT: ";
    weight_hud_text.append(bn::to_string<8>(current_weight));
    text_generator.generate(-100, 0, weight_hud_text, weight_text_sprites);
}
