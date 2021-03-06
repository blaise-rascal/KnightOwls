//See game_scene.cpp for info & license





#ifndef GAME_SCENE_H
#define GAME_SCENE_H

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
        int run_scene();

    private:
        //TODO: Maybe make const?  
        struct CardInfo { 
            bn::string<17> name;
            int cost;
            int weight;
            int powerone;
            int powertwo;
            int powertwopercentage;
            int gatherone;
            int gathertwo;
            int gathertwopercentage;
            int tileindex;
            int rarity;
            //0 = unobtainable (energy surges)
            //1 = common
            //2 = uncommon
        };

        struct NonOwlUpgradeInfo { 
            bn::string<20> name_minus_banner;
            bn::string<50> description_one;
            bn::string<50> description_two;
        };
        //TODO: Maybe make const?  
        struct WaveInfo { 
            int attack;
            int reward;
            int penalty;
            int enemy_index;
            //how many sprites
            //art for the enemies
            //eventually, probability of different attacks, add items to tree, etc.
            //ACTUALLY: attack, penalty, reward
        };
        //struct nonowlupgradeinfo
        //upgrade name, upgrade description
        struct EnemyInfo { 
                              //herman the german merman
            int tileindex;
            bn::string<27> name;//great pacific garbage patch
        };
        //Declare sprite pointers
        bn::sprite_text_generator& my_text_generator;
        
        //BACKGROUND POINTER TEST
        bn::regular_bg_ptr _ocean_bg;
        //bn::optional<bn::regular_bg_ptr> _background_image;


        //bn::regular_bg_ptr _swamp_bg;
        bn::regular_bg_ptr _spellbook_bg;
        bn::regular_bg_ptr _rift_bg;
        //bn::sprite_ptr _chiyu_sprite;

        //bn::sprite_ptr bomb_sprite;
        //bomb_sprite.set_bg_priority(2); 
        bn::sprite_ptr _selection_cursor_sprite;
        bn::sprite_ptr _right_book_arrow_sprite;
        bn::sprite_ptr _left_book_arrow_sprite;
        bn::sprite_ptr _enemy_sprite;


        //bf::status status;

        //string declarations
       /* bn::string<20> weight_hud_text;
        //bn::sprite_text_generator& _text_generator;
*/
        //ART LIST
        
        //boat
        //enemies
        int current_weight;
        int current_power;
        int current_runes;
        int current_hull;
        int max_hull;
        int max_boat_weight;
        int runes_which_might_disappear;
        //weight_hud_text.append(bn::to_string<8>(current_weight));

    

        int menu_position;
        int state;
        int last_tableau_x_pos;
        int last_tableau_y_pos;
        int last_merc_tableau_x_pos;
        int menu_position_max;
        int current_wave;
        bool won_wave;
        int total_merc_probs;
        int random_num;
        bool player_stat_box_active;
        bool enemy_stat_box_active;
        int state_before_spellbook;
        int state_before_summon_start;
        int number_sb_pages;
        int current_sb_page;
        int current_sb_owl;
        int upgrade_option_one;
        int upgrade_option_two;
        //int current_zone;
        int start_of_zone_two;
        int start_of_zone_three;
        int last_banner_tableau_x_pos;
       // int goblindicesides;
        //int runes_at_start_of_round;
        //int enemyindex;

        //declare text sprites
        bn::vector<bn::sprite_ptr, 10> weight_text_sprites; // probably only needs to be like 7
        bn::vector<bn::sprite_ptr, 4> hull_text_sprites; // probably only needs to be 4
        bn::vector<bn::sprite_ptr, 13> wave_text_sprites;
        bn::vector<bn::sprite_ptr, 7> zone_text_sprites;  
        //bn::vector<bn::sprite_ptr, 13> wave_text_sprites_two; 
        //bn::vector<bn::sprite_ptr, 13> wave_text_sprites_three; 
        bn::vector<bn::sprite_ptr, 10> total_runes_text_sprites;
        bn::vector<bn::sprite_ptr, 10> runes_that_might_disappear_text_sprites;
        bn::vector<bn::sprite_ptr, 10> power_text_sprites;
        bn::vector<bn::sprite_ptr, 50> status_text_one_sprites;
        bn::vector<bn::sprite_ptr, 50> status_text_two_sprites;
        bn::vector<bn::sprite_ptr, 50> status_text_three_sprites;
        
        bn::vector<bn::sprite_ptr, 10> first_menu_option_text_sprites;
        bn::vector<bn::sprite_ptr, 10> second_menu_option_text_sprites; //huh? why isn't this getting overflowed? the second menu option is "examine" which is more than 4 sprites... oh well, let's just set it to 10
        bn::vector<bn::sprite_ptr, 10> third_menu_option_text_sprites;

        bn::vector<bn::sprite_ptr, 13> first_enemy_stat_text_sprites; // probably needs to be like 4
        bn::vector<bn::sprite_ptr, 13> second_enemy_stat_text_sprites; // probably needs to be like 4
        bn::vector<bn::sprite_ptr, 13> third_enemy_stat_text_sprites; // probably needs to be like 4

        //bn::vector<bn::sprite_ptr, 7> button_sprites; // probably needs to be like 4

        //vectors containing card & state information
        bn::vector<int, 100> player1deck;
        bn::vector<int, 100> player1deck_at_start_of_round; //Uh, I guess I need both this AND cards removed? if I want to have a way to examine the tree, that is
        bn::vector<int, 100> player1deck_after_miasma; //Uh, I guess I need both this AND cards removed? if I want to have a way to examine the tree, that is

        bn::vector<CardInfo,11> CardInfoVector;
        bn::vector<CardInfo,10> UpgradedCardInfoVector;
        bn::vector<WaveInfo,36> WaveInfoVector;
        bn::vector<EnemyInfo,30> EnemyInfoVector;
        bn::vector<NonOwlUpgradeInfo,8> NonOwlUpgradeInfoVector;
        bn::vector<bn::sprite_ptr, 100> Player1Tableau;
        bn::vector<bn::sprite_ptr, 6> MercenaryTableau;
        bn::vector<bn::sprite_ptr, 6> SaleArrowsTableau;
        bn::vector<bn::sprite_ptr, 100> SpellbookTableau;
        bn::vector<bn::sprite_ptr, 5> BannerTableau;
        bn::vector<int, 6> MercenaryDeck;
        bn::vector<int, 3> AllUncommonAttackingMercs;
        bn::vector<int, 3> AllUncommonEconomyMercs;
       // bn::vector<int, 8> AllCommonAndUncommonMercs;
        bn::vector<int, 6> TempMercDeckToDrawFrom;
        bn::vector<int, 6> SaleMercDeckToDrawFrom;
        bn::array<int, 6>AmountMercOnSale;
        bn::vector<int, 8>DeckOfOwlUpgrades;
        bn::vector<int, 8>DeckOfNonOwlUpgrades;
        bn::array<bool,16>IsUpgradeResearched;//first 8 are owl upgrades, last 8 are:
        bn::array<int,3>BossAttacks;
        bn::array<bn::string<14>,3>ZoneNames;
        
/*
        OWL: +all (owl type) +x power... or whatever... depending on the owl
RIFT: +1 owl available for purchase at -1c
SURGE: first 3 owls guaranteed to not be energy surges
COUPON: one owl goes on sale every round (25% off)
BRAVERY: -1maxHP but +1 max static
STRENGTH: +2maxHP and heal to full
D20: All owls 5% chance for +10ATK
FIRST AID BANNER: After LOSS, 50% chance for +1HP
GOBLIN BANNER: Permanent goblin (1d10ATK) added to deck
VAMPIRE BANNER: Permanent vampire (2k, increases by 2k per victory) added to deck
BANK BANNER: After buy phase, you earn xc, where x is half your c rounded up.
UPGRADES MAGES TO: */
        //AT SHIPWRECK:
        //
        //todo: remove button

        //functions
        void _update_hud_text();
        void _update_wave_info_hud_text();
        void _update_enemy_stat_box();
        void _display_status(const bn::string<50>& statustextone, const bn::string<50>& statustexttwo = "", const bn::string<50>& statustextthree = "");
        void _update_selection_cursor_from_virt_menu_position();
        void _update_selection_cursor_from_hor_menu_position();
        void _update_spellbook_from_menu_position();
        void _return_owls_to_tree();
        void _point_cursor_at_letter(const bn::sprite_ptr& target_sprite);
        void _point_cursor_at_owl(const bn::sprite_ptr& target_sprite);
        bn::string<50> _generate_description_from_owl_index(int card_info_index, bool upgraded);
        void _generate_virt_menu(int num_options, const bn::string<20>& menu_option_one, const bn::string<20>& menu_option_two, const bn::string<20>& menu_option_three = "");
        void _start_hor_merc_menu();
        void _navigate_through_virt_menu();
        void _navigate_through_hor_menu();
        void _navigate_through_spellbook();
        int  _position_on_page(int absolute_owl);
        int  _price_on_sale(int merc_deck_index);
        void _clear_virt_menu();
        bool _is_merc_deck_empty();
        void _research_upgrade(int whichupgrade);
        bn::string<50> _generate_first_upgrade_description_from_upgrade_index(int isupgraded_index);
        bn::string<50> _generate_second_upgrade_description_from_upgrade_index(int isupgraded_index);
        bn::string<25> _generate_name_from_upgrade_index(int isupgraded_index);
        
        void _apply_pity();
        int _get_current_zone();
        //void _replace_merc_with_random_owl(int which_merc_position);
};
//}



#endif