
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
#include "bn_regular_bg_ptr.h"
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
        //TODO: Maybe make const?   name, cost, weight, power, gather, probabilityweight
        struct CardInfo { 
            bn::string<15> name;
            int cost;
            int weight;
            int power;
            int gather;
            int probabilityweight;
        };
        //Declare sprite pointers
        bn::sprite_text_generator& my_text_generator;
        bn::regular_bg_ptr _ocean_bg;
        //bn::sprite_ptr _chiyu_sprite;

        //bn::sprite_ptr bomb_sprite;
        //bomb_sprite.set_bg_priority(2); 
        bn::sprite_ptr _selection_cursor_sprite;

        //bf::status status;

        //string declarations
       /* bn::string<20> weight_hud_text;
        //bn::sprite_text_generator& _text_generator;
*/

        int current_weight;
        int current_power;
        int current_runes;
        int total_runes;
        //weight_hud_text.append(bn::to_string<8>(current_weight));

    

        int menu_position;
        int state;
        int enemyattack;
        int last_tableau_x_pos;

        //declare text sprites
        bn::vector<bn::sprite_ptr, 20> weight_text_sprites;
        bn::vector<bn::sprite_ptr, 20> runes_text_sprites;
        bn::vector<bn::sprite_ptr, 20> power_text_sprites;
        bn::vector<bn::sprite_ptr, 6> deploy_label_text_sprites;
        bn::vector<bn::sprite_ptr, 50> status_text_one_sprites;
        bn::vector<bn::sprite_ptr, 50> status_text_two_sprites;
        bn::vector<bn::sprite_ptr, 4> pass_label_text_sprites;
        bn::vector<bn::sprite_ptr, 20> enemy_attack_text_sprites;

        //vectors containing card & state information
        bn::vector<int, 100> player1deck;
        bn::vector<CardInfo,10> CardInfoVector;
        bn::vector<bn::sprite_ptr, 100> Player1Tableau;

        //functions
        void _update_hud_text();
        void _display_status(const bn::string<50>& statustextone, const bn::string<50>& statustexttwo = "");
        void _update_selection_cursor_from_menu_position();
};
//}



#endif