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

#include "bn_sprite_items_hero_bomb_icon.h"


#include "variable_8x8_sprite_font.h"


int main()
{
    bn::core::init();
    bn::sprite_ptr chiyu_sprite = bn::sprite_items::chiyu.create_sprite(-120, -80);
    bn::sprite_ptr bomb_sprite = bn::sprite_items::hero_bomb_icon.create_sprite(10, 0);

    //bf::status status;
    bn::string<20> weight_hud_text("NUMBER PRESSES: ");
    int current_weight = 0;
    weight_hud_text.append(bn::to_string<8>(current_weight));

 
    bn::sprite_text_generator my_text_generator(variable_8x8_sprite_font);
    my_text_generator.set_left_alignment();
    //my_text_generator.set_bg_priority(1); 
    
    bn::vector<bn::sprite_ptr, 32> text_sprites;

    my_text_generator.generate(-100, 0, weight_hud_text, text_sprites);


    while(true)
    {
        if(bn::keypad::a_pressed())
            {
                text_sprites.clear();
                current_weight++;
                weight_hud_text="NUMBER PRESSES: ";
                weight_hud_text.append(bn::to_string<8>(current_weight));
                my_text_generator.generate(-100, 0, weight_hud_text, text_sprites);

            }
        bn::core::update();
    }
}
