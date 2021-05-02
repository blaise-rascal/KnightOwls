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

#include "bn_sprite_items_chiyu.h"
#include "bn_sprite_items_selection_cursor.h"
#include "bn_sprite_items_hero_bomb_icon.h"

#include "bn_regular_bg_items_oceanbackground.h"

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
//Maybe 
/*
pseudocode here!!!!

player1currentdeck vector <100> of ints (used for drawing examinetableau, and for drawing new cards)
player1allcardsbought vector <100> of ints (copy) -> (used so you come back to the same place next round)
player1fallencards vector <100> of ints, and indices in position array, and indices in sprite array (used for examinetableau, and also to see if you have enough for fusion, and also to decide where to place next sprite)
^^It's possible this vector should be combined with the vector of sprite pointers
array of <100> cardinfo (array of card struct objects) plus number currentlyintableau

- reference to sheet? (nah)
- reference to frame? nah just int of which frame
- name
- weight
- cost
- power
- (rarity?)
- amountcurrentlyintableau (yes!!)
- odds of getting bought
- description? could just be generated?
-IF FUSION STONE: what to fuse, how many needed (otherwise blank), frame to change to after fusion, description
//note that weapons are also possible; can be treated like fusion stones on fused sprites
const array of card positions (x, y) & their order (this array is the priority)

player1inplaytableau vector <100> of sprite pointers
player1examinetableau vector <100> of sprite pointers

AT BEGINNING OF ENTIRE GAME
    -coinflip (only if multiplayer)
    -round#=1 (or 0 or whatever)
    -summon 3 random mercenaries
    -initialize arrays
    -calculate total oddsweight
>beginning of round:
    -increment round number, enemy gets cp according to round#
    -restore currentdeck = allcardsbought, currentweight&gather = 0, attack = 
    -clear fallencards, make their position indices -1 (or 0 if cardpositions starts at 1)
    -clear mercenarytableau, generate 3 random mercenaries using oddsweight. store in a vector of mercenary indices and display a mercenary sprite ptr (plus text for each one suggesting the cost)

player 1: choose to SUMMON!, pass, examine
>summon
 -erase random from from player1currentdeck.
 -add that int to fallencards.
 -increment that index in cardinfo
 -to find position, iterate through fallencards positions, and make equal to the highest number+1
 -summon that sprite frame at that position in positionarray
 -add weight & gather values to max, update display
 -handle fusion stones:
    -for each fusion stone, if at least one exist:
        -determine how many4 can be fused (numberhave / numberneeded)
        -remove that many sprites from the tableau, by iterating through fallencards, and removing until all necessary removed - making sure to update the tableau, and fallensprites, and the card counts, and combat power, weight
        -finally add the correct number of fused sprites to the tableau
 -if currentweight>weightmax, put combatpower & gather to zero, clear sprites in tableau, go to combat state
 //each type of sprite can only be used in at most one recipe. otherwise there is ambiguity when you play a sprite that fulfils multiple stones, which one should be fulfiled.

>pass - go to combat state
>examine
    -go to examine state
    -for indeckvector, make that many sprites & just add them in a grid
    -for intableauvector, make that many sprites and just add them in a grid
    -maybe examine, but mostly just wait for button press to leave, and return to player 1 state (destroy the sprites)

>combat state
-compare damage. if equal or player is higher, COMBAT RESULTS: ROUND WON. NO DAMAGE
-otherwise ROUND LOST, BOAT TOOK 1 DAMAGE. you take 3 damage you lose!!!
-go to buy state. I mean, I COULD just let you buy anything...

//HOW NOT GROW OUT OF CONTROL ... add rocks somehow? Maybe enemies add rocks sometimes?


>BUY STATE
-remove owls from tableau, reset #s in amountintableau (ideally you could also examine at this point - but now examining is just 1 window for startingdeck)
-options include buy, inspect, pass. can move to select items, when you buy something:
    -it replaces itself with something random
    -add card to backup vector
    -remove that much from AE

-at end of buy state; replace all cards with new cards - clear text vectors, clear sprite vectors, clear array, pick 3 new random, generate text, sprite, etc. Or maybe they get moved down the line like star realms
//I should have a function to move the cursor to a sprite (you'd pass it the final non-empty sprite in a text array)
//also maybe a global spriteptr for currently hovered sprite (or an int or something)

new plan: just an endless attack, but you know what your enemy will hit you with; increases every round; but if you scratch out, you still gain 4 astral power? Also it persists between rounds? sure
*/
namespace{
        //initialize sprites
        
        const bn::string<6> deploy_label_text("  DRAW");
        const bn::string<4> pass_label_text("PASS");
        //const bn::array<int, 8> StartingDeck([0,0,1,1,2,2,3,3]);
        const int MENU_POSITION_MAX = 1;
}


/*
        bn::sprite_ptr bomb_sprite = bn::sprite_items::hero_bomb_icon.create_sprite(10, 0);
        //bomb_sprite.set_bg_priority(2); 
        bn::sprite_ptr selection_cursor_sprite = bn::sprite_items::selection_cursor.create_sprite(-30, 30);

        //bf::status status;
*/
        //string declarations
        //bn::string<20> weight_hud_text("WEIGHT: ");
        /*const bn::string<6> deploy_label_text("DEPLOY");
        const bn::string<4> pass_label_text("PASS");
        //bn::sprite_text_generator& _text_generator;


        int current_weight = 0;
        //weight_hud_text.append(bn::to_string<8>(current_weight));

    

        int menu_position = 0;
        const int MENU_POSITION_MAX = 1;

        //declare text sprites
        bn::vector<bn::sprite_ptr, 32> weight_text_sprites;
        bn::vector<bn::sprite_ptr, 6> deploy_text_sprites;
        bn::vector<bn::sprite_ptr, 4> pass_text_sprites;

        
}*/

game_scene::game_scene(bn::sprite_text_generator& text_generator):
    my_text_generator(text_generator),
    _ocean_bg(bn::regular_bg_items::oceanbackground.create_bg(0, 0)),
    _chiyu_sprite(bn::sprite_items::chiyu.create_sprite(0, 0)),
    _selection_cursor_sprite(bn::sprite_items::selection_cursor.create_sprite(0, 30)),
    //weight_hud_text("WEIGHT: "),
    current_weight(0),
    menu_position(0),
    state(0)
{
//pointer_to_text_generator(text_generator)
    //_chiyu_sprite(bn::sprite_items::chiyu.create_sprite(0, 0)),
//{
    //generate text



    my_text_generator.generate(-100, 30, deploy_label_text, deploy_label_text_sprites);
    my_text_generator.generate(0, 30, pass_label_text, pass_label_text_sprites);


    //make starting deck
    player1deck.push_back(0);
    player1deck.push_back(0);
    player1deck.push_back(0);
    player1deck.push_back(0);
    player1deck.push_back(1);
    player1deck.push_back(1);
    player1deck.push_back(2);
    player1deck.push_back(2);

    _display_status("POOL INCLUDES 0,0,0,0,1,1,2,2");
    //my_text_generator.set_center_alignment();
    //my_text_generator.set_left_alignment();


    _update_weight_text();
    _update_selection_cursor_from_menu_position();
    
    //initialize sprites
   // chiyu_sprite = bn::sprite_items::chiyu.create_sprite(-120, -80);
    /*bn::sprite_ptr bomb_sprite = bn::sprite_items::hero_bomb_icon.create_sprite(10, 0);
    //bomb_sprite.set_bg_priority(2); 
    bn::sprite_ptr selection_cursor_sprite = bn::sprite_items::selection_cursor.create_sprite(-30, 30);

    //bf::status status;

    //string declarations*/
    //bn::string<20> weight_hud_text("WEIGHT: ");/*
    //const bn::string<6> deploy_label_text("DEPLOY");
    //const bn::string<4> pass_label_text("PASS");
    //bn::sprite_text_generator& _text_generator;


    //int current_weight = 0;
    //weight_hud_text.append(bn::to_string<8>(current_weight));
    


    //int menu_position = 0;
/*
    //declare text sprites
    bn::vector<bn::sprite_ptr, 32> weight_text_sprites;
    bn::vector<bn::sprite_ptr, 6> deploy_text_sprites;
    bn::vector<bn::sprite_ptr, 4> pass_text_sprites;


    pointer_to_text_generator.generate(-100, 40, deploy_label_text, deploy_text_sprites);
    pointer_to_text_generator.generate(10, 40, pass_label_text, pass_text_sprites);
    _update_weight_text();*/
}

void game_scene::update()
{
    bn::random random_generator;
    int random_num;
    //_chiyu_sprite = bn::sprite_items::chiyu.create_sprite(-120, -80);
    while(true)
    {   
        //This is a deterministic random generator, so it must be spun every fram to not return the same numbers every boot.
        //Luckily this is not too slow to affect performance.
        random_num = random_generator.get();

        switch(state)
            {
            case 0: //Player1 Turn
                if(bn::keypad::a_pressed() && menu_position==0)
                {
                    if(player1deck.size()>0){
                        int index_to_remove = bn::abs(random_num%player1deck.size());
                        //_display_status(bn::to_string<20>(player1deck.at(index_to_remove)));
                        int weight_to_add = player1deck.at(index_to_remove);
                        current_weight=current_weight+weight_to_add;
                        player1deck.erase(player1deck.begin()+index_to_remove);
                        _update_weight_text();
                        _display_status(bn::string<40>("You drew a ").append(bn::to_string<2>(weight_to_add)));
                        if(current_weight>4){
                            //status_text_sprites.clear();
                            //my_text_generator.generate(0, -50, "BOAT SANK", status_text_sprites);
                            _display_status(bn::string<40>("You drew a ").append(bn::to_string<2>(weight_to_add)).append(". BOAT SANK"));
                        }
                        else{
                             _display_status(bn::string<40>("You drew a ").append(bn::to_string<2>(weight_to_add)));
                        }
                    }
                    else{
                        _display_status("NO MORE ITEMS TO DRAW");
                    }
                    
                    //_display_status(bn::to_string<15>(i).append(",").append(bn::to_string<4>(player1deck.size())));
                }
                if(bn::keypad::left_pressed())
                {
                    menu_position--;
                    if(menu_position<0)
                    {
                        menu_position = MENU_POSITION_MAX;
                    }
                    
                    _update_selection_cursor_from_menu_position();
                }
                if(bn::keypad::right_pressed())
                {
                    menu_position++;
                    if(menu_position > MENU_POSITION_MAX)
                    {
                        menu_position = 0;
                    }
                    _update_selection_cursor_from_menu_position();
                }
                break;

            default:
                BN_ERROR("Invalid state");
                break;
            }
        
        bn::core::update();
    }
}

void game_scene::_update_selection_cursor_from_menu_position()
{
    if(menu_position == 0)
    {
        _selection_cursor_sprite.set_x(-40);
    }
    else{
        _selection_cursor_sprite.set_x(40);
    }
}

void game_scene::_update_weight_text()
{
    weight_text_sprites.clear();
    my_text_generator.set_left_alignment();
    bn::string<20> weight_hud_text("WEIGHT: ");
    weight_hud_text.append(bn::to_string<8>(current_weight));
    weight_hud_text.append("/4");
    my_text_generator.generate(-100, 50, weight_hud_text, weight_text_sprites);
}

void game_scene::_display_status(const bn::string<40>& statustext)
{
    my_text_generator.set_center_alignment();
    status_text_sprites.clear();
    my_text_generator.generate(0, -50, statustext, status_text_sprites);
}

//void game_scene::_point_cursor_at(const bn::sprite_item)
//{

//}

//point_cursor_at_sprite, point_cursor_at_string