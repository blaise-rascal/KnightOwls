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

//TODO: clean up includes, remove from game those things built extraneously by $make clean $make
#include "bn_fixed_point.h"


#include "bn_sprite_tiles_ptr.h"

#include "bn_sprite_items_knight_owls.h"
#include "bn_sprite_items_selection_cursor.h"
//#include "bn_sprite_items_hero_bomb_icon.h"

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
        
        //const bn::string<6> deploy_label_text("SUMMON");
        //const bn::string<4> pass_label_text("PASS");
        //const bn::array<int, 8> EventArray([0,0,1,1,2,2,3,3]); //Hmm, it'd be cool to have probabilities later on... Also maybe array of 
        //Things that an enemy can be:
        /*
            -Attack value 1 
                -probability
            -Attack value 2
                -probability

            -# of enemies
            -sprite for each enemy?
            -location of each enemy... ugh... all of a sudden i need a 4 position arrays*/
        
        const int MERCS_FOR_SALE = 3;
        const int RUNES_PER_TURN = 4;
        //const bn::string<6> deploy_label_text("  DRAW");
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
    //_chiyu_sprite(bn::sprite_items::chiyu.create_sprite(0, 0)),
    _selection_cursor_sprite(bn::sprite_items::selection_cursor.create_sprite(0, 30)),
    //weight_hud_text("WEIGHT: "),
    current_weight(0),
    current_power(0),
    current_runes(0),
    current_hull(3),
    runes_which_might_disappear(0),
    menu_position(0),
    state(0),
    enemy_attack(10),
    last_tableau_x_pos(-110),
    
    last_merc_tableau_x_pos(-20),
    menu_position_max(1)
    //enemyindex(0)
{
//pointer_to_text_generator(text_generator)
    //_chiyu_sprite(bn::sprite_items::chiyu.create_sprite(0, 0)),
//{
    //generate text


    
    //                         name, cost, weight, power, gather, tileindex, probabilityweight
    CardInfoVector.push_back({"MAGE",           7,0,0,1,1,1});
    CardInfoVector.push_back({"ARCHER",         7,0,3,0,2,1});
    CardInfoVector.push_back({"WARRIOR",        1,1,6,1,0,0});
    CardInfoVector.push_back({"HEAVY WARRIOR",  1,2,11,2,3,0});
    CardInfoVector.push_back({"SPEAR MASTER",   12,0,7,0,4,1});
    /*
    CardInfoVector.push_back({"MAGE",           7,0,0,1,0,10});
    CardInfoVector.push_back({"ARCHER",         7,0,3,0,5,10});
    CardInfoVector.push_back({"WARRIOR",        1,1,6,1,10,0});
    CardInfoVector.push_back({"HEAVY WARRIOR",  1,2,11,2,15,0});
    CardInfoVector.push_back({"SPEAR MASTER",         12,0,7,0,20,1});*/
    //make starting deck
    player1deck.push_back(0);
    player1deck.push_back(0);
    player1deck.push_back(1);
    player1deck.push_back(1);
    player1deck.push_back(2);
    player1deck.push_back(2);
    player1deck.push_back(3);
    player1deck.push_back(3);

    _selection_cursor_sprite.set_visible(false);
    _selection_cursor_sprite.set_z_order(-100); //lower z order means it shows up higher. wacky huh?
    //_display_status(bn::to_string<8>(CardInfoVector.at(1).name));
    //my_text_generator.set_center_alignment();
    //my_text_generator.set_left_alignment();

    
    
    
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
    int random_num; // Do not alter this value! It continues spinning each frame
    int total_merc_probs = 0;
    for(int i =0; i<CardInfoVector.size();i++)
    {

        total_merc_probs += CardInfoVector.at(i).probabilityweight;
    }
    //_chiyu_sprite = bn::sprite_items::chiyu.create_sprite(-120, -80);
    while(true)
    {   
        //This is a deterministic random generator, so it must be spun every frame to not return the same numbers every boot.
        //Luckily this is not too slow to affect performance.
        //TODO: If I implement a title screen, this should start spinning then for maximum randomness
        random_num = random_generator.get();

        switch(state)
        {
            case 0: // Beginning of game
            {   
                
                
                //_display_status(bn::to_string<50>(total_merc_probs));
                _display_status("A CHILL WIND BLOWS...","PRESS A TO START VOYAGE");
                state =100;
                break;
            }
            case 100: //Loop while waiting for A press - when pressed, summon mercs
            {
                if(bn::keypad::a_pressed())
                {
                    
                    //_display_status(bn::to_string<50>(total_merc_probs));
                    
                    int temp_rando = bn::abs(random_num);
                    for(int mercstoadd=0; mercstoadd<MERCS_FOR_SALE; mercstoadd++)
                    {
                        bool foundcard=false;
                        int card_to_add=1000;
                        int probweight_to_add=temp_rando%total_merc_probs;//Look at the last n digits, where n is total weight sum
                        temp_rando=temp_rando/total_merc_probs;//Remove the last n digits
                        int count_up_weights=0;
                        for(int card_to_check=0; card_to_check < CardInfoVector.size(); card_to_check++)//TODO maybe make this a while loop (more efficient). eh who cares
                        {
                            if(foundcard == false)
                            {
                                count_up_weights+=CardInfoVector.at(card_to_check).probabilityweight;
                                if(probweight_to_add < count_up_weights)
                                {
                                    foundcard = true;
                                    card_to_add = card_to_check;
                                }
                            }
                        }
                        //TODO: handle error if foundcard is false (though it'll be obvious, you can't add vector 1000)
                        //Add to mercenary deck
                        MercenaryDeck.push_back(card_to_add);
                        
                        bn::sprite_ptr NewTableauImg = bn::sprite_items::knight_owls.create_sprite(last_merc_tableau_x_pos, -38);
                        NewTableauImg.set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(CardInfoVector.at(card_to_add).tileindex));//player1deck.at(index_to_remove).tileindex));
                        MercenaryTableau.push_back(NewTableauImg);//bn::sprite_items::knight_owls.create_sprite(last_tableau_x_pos, 0));
                        //Player1Tableau.at().set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(20));
                        last_merc_tableau_x_pos+=20;
                    }
                    _display_status("THE STARS ALIGN", "PRESS A TO CONTINUE");
                    
                    state = 101;
                }
                bn::core::update();
                break;
            }
            case 101: // Loop; wait for A press to add new enemy. (ALSO set temp vector equal to current deck)
            {
                if(bn::keypad::a_pressed())
                {
                    bn::string<50> display_text_line_one("ENEMY APPEARS! ATTACK = ");
                    display_text_line_one.append(bn::to_string<4>(enemy_attack));
                    _display_status(display_text_line_one, "PRESS A TO CONTINUE");
                    bn::string<16> enemy_attack_text("ATK: ");
                    enemy_attack_text.append(bn::to_string<4>(enemy_attack));
                    my_text_generator.generate(70, 0, enemy_attack_text, enemy_attack_text_sprites);
                    state = 1;

                    //As you remove owls from your tree, you'll modify the player1deck. So save it here BEFORE modifications so we'll have access to it later
                    player1deck_at_start_of_round = player1deck;
                }
                bn::core::update();
                break; 
            }
            case 1: // Loop; wait for A press to add starting runes
            {
                if(bn::keypad::a_pressed())
                {
                    state = 10001;
                    //Add 4 units to mercenary vectors
                    //TODO: Maybe have player push to keep adding? We'll see if it feels random enough...
                    //Also TODO: Maybe run this random generator many times to be sure that it is working as expected; also keep in mind that max(random_num) must be greater than total_merc_probs*
                    
                    
                }
                bn::core::update();
                break;
            }
            case 10001:
            {
                _display_status("NEW ROUND START! 4 RUNES GENERATED", "PRESS A TO CONTINUE");
                current_runes+=RUNES_PER_TURN;
                _update_hud_text();
                state = 2;
                break;
            }
            case 2: // Loop; wait to switch to menu
            {
                if(bn::keypad::a_pressed())
                {
                //TODO: Reset values to zero of member variables
                    _generate_menu(2, "SUMMON", "PASS");
                    _display_status("SUMMONING PHASE","ARROWS TO MOVE, A TO SELECT");
                    _update_hud_text();
                    _update_selection_cursor_from_menu_position();
                    
                    
                    //eventarrayindex++;

                    //Make array


                    state = 3; //TODO: Maybe make this be "next state" so that we don't accidentally execute the code of 2 states in one frame
                }
                bn::core::update();

                break;
            }
            case 3: //Player1 Turn Loop
            {
                _navigate_through_menu();
                
                if(bn::keypad::a_pressed())
                {
                    if(menu_position==0)
                    {
                        if(player1deck.size()>0){

                            //DRAW A CARD!!!
                            int index_to_remove = bn::abs(random_num%player1deck.size());
                            int weight_to_add = CardInfoVector.at(player1deck.at(index_to_remove)).weight;
                            int power_to_add = CardInfoVector.at(player1deck.at(index_to_remove)).power;
                            int runes_to_add = CardInfoVector.at(player1deck.at(index_to_remove)).gather;
                            current_weight=current_weight+weight_to_add;
                            current_power=current_power+power_to_add;
                            runes_which_might_disappear=runes_which_might_disappear+runes_to_add;
                            _update_hud_text();
                            //_display_status(bn::string<40>("You drew a ").append(bn::to_string<2>(weight_to_add)));
                            //_display_status(bn::string<40>("You drew a ").append(CardInfoVector.at(player1deck.at(index_to_remove)).name);

                            bn::string<50> first_line_status("SUMMONED ");
                            bn::string<50> second_line_status("");
                            first_line_status.append(bn::to_string<18>(CardInfoVector.at(player1deck.at(index_to_remove)).name));
                            if(current_weight>4){
                                
                                second_line_status.append("BOAT OVERLOADED! PRESS A TO UNLOAD");
                                state = 4;

                            }
                            else{
                                /*second_line_status.append("WEIGHT+");
                                second_line_status.append(bn::to_string<4>(weight_to_add));
                                second_line_status.append(", ATTACK+");
                                second_line_status.append(bn::to_string<4>(power_to_add));
                                second_line_status.append(", RUNES+");
                                second_line_status.append(bn::to_string<4>(runes_to_add));*/
                                second_line_status.append(_generate_description_from_owl_index(player1deck.at(index_to_remove)));
                            }
                            //display status
                            _display_status(first_line_status,second_line_status);

                            //Display sprite
                            //newspriteposition= vector.at(size-1).position + 5
                            bn::sprite_ptr NewTableauImg = bn::sprite_items::knight_owls.create_sprite(last_tableau_x_pos, 0);
                            NewTableauImg.set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(CardInfoVector.at(player1deck.at(index_to_remove)).tileindex));//player1deck.at(index_to_remove).tileindex));
                            Player1Tableau.push_back(NewTableauImg);//bn::sprite_items::knight_owls.create_sprite(last_tableau_x_pos, 0));
                            //Player1Tableau.at().set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(20));
                            last_tableau_x_pos+=5;

                            //Delete the drawn card from the deck
                            player1deck.erase(player1deck.begin()+index_to_remove);
                        }
                        else{
                            _display_status("NO MORE ITEMS TO DRAW");
                        }
                    }
                    
                    else if(menu_position==1)
                    {
                        current_runes += runes_which_might_disappear;
                        runes_which_might_disappear = 0;
                        _update_hud_text();
                        state=5;
                    }
                    //_display_status(bn::to_string<15>(i).append(",").append(bn::to_string<4>(player1deck.size())));
                }


                
                bn::core::update();
                break;
            }
            case 4: //Overloaded!
            {
                if(bn::keypad::a_pressed())
                {
                    _return_owls_to_tree();
                    state=5;
                }
                bn::core::update();
                break;
            }
            case 5: //Combat
            {
                //KILL SELECTION CURSOR, LABELS
                
                _clear_menu();

                _display_status("SUMMONING COMPLETE","PRESS A TO RESOLVE COMBAT");
                if(bn::keypad::a_pressed())
                {
                    if(current_power>enemy_attack)
                    {
                        _display_status("YOUR ATK IS HIGHER. VICTORY!","PRESS A TO CONTINUE");
                        state = 8;
                    }
                    if(current_power<enemy_attack)
                    {
                        _display_status("ENEMY'S ATK IS HIGHER. DEFEAT!","PRESS A TO CONTINUE");
                        state=6;
                    }
                    if(current_power==enemy_attack)
                    {
                        _display_status("YOUR ATK = ENEMY ATK. VICTORY!","PRESS A TO CONTINUE");
                        state = 8;
                    }
                }
                bn::core::update();
                break;
            }
            case 6: // Defeat! Take 1 damage
            {
                if(bn::keypad::a_pressed())
                {
                    current_hull=current_hull-1;
                    if(current_hull==0)
                    {
                        _display_status("SHIP DESTROYED! GAME OVER");
                        state = 7;
                    }
                    else{
                        state = 8;
                        _display_status("SHIP TAKES ONE DAMAGE.","PRESS A TO CONTINUE");
                        _update_hud_text();
                    }
                }
                bn::core::update();
                break;
            }
            case 7: // Game over
            {if(bn::keypad::a_pressed())
                {
                    _display_status("GAME OVER!");

                }
                bn::core::update();
                break;
            }
            case 8: // Either return owls or just move to buy phase
            {
                if(bn::keypad::a_pressed())
                {
                    if(Player1Tableau.size()==0){
                        state = 10;
                    }
                    else{
                        _display_status("OWLS RETURNED TO CASTLE.","PRESS A TO CONTINUE");
                        _return_owls_to_tree();
                        state = 9;
                    }
                }
                bn::core::update();
                break;
            }
            case 9: //Just returned owls, waiting on button press to 
            {
                if(bn::keypad::a_pressed())
                {
                   state = 10;
                }
                bn::core::update();
                break;
            }
            case 10: //Intro to buy menu state
            {
                 _display_status("BUY PHASE","ARROWS TO MOVE, A TO SELECT");
                 
                _generate_menu(3, "SHOP", "EXAMINE TREE", "PASS");
                 //_point_cursor_at_sprite(MercenaryTableau.at(0));
                state = 11;
                break;
            }
            case 11: //Buy menu loop
            {
                //Shop Pass Examine
                //BUY PHASE, ARROWS TO MOVE, A TO SELECT
                //WARRIOR: ATK+2,WGT+0,RUNES+3
                //LRA: PURCHASE, B: CANCEL
                
                _navigate_through_menu();
                if(bn::keypad::a_pressed())
                {
                    if(menu_position==2)
                    {
                        _clear_menu();
                        state= 10001;
                    }

                
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


void game_scene::_update_hud_text()
{
    weight_text_sprites.clear();
    my_text_generator.set_left_alignment();
    bn::string<20> weight_hud_text("WEIGHT: ");
    weight_hud_text.append(bn::to_string<8>(current_weight));
    weight_hud_text.append("/4");
    my_text_generator.generate(-115, -72, weight_hud_text, weight_text_sprites);

    runes_text_sprites.clear();
    bn::string<20> runes_hud_text("RUNES: ");
    runes_hud_text.append(bn::to_string<8>(current_runes));
    if(runes_which_might_disappear>0)
    {
        runes_hud_text.append("+");
        runes_hud_text.append(bn::to_string<8>(runes_which_might_disappear));
    }
    my_text_generator.generate(-115, -61, runes_hud_text, runes_text_sprites);

    power_text_sprites.clear();
    bn::string<20> power_hud_text("ATTACK: ");
    power_hud_text.append(bn::to_string<8>(current_power));
    my_text_generator.generate(-115, -50, power_hud_text, power_text_sprites);

    hull_text_sprites.clear();
    bn::string<20> hull_hud_text("HULL: ");
    hull_hud_text.append(bn::to_string<8>(current_hull));
    my_text_generator.generate(-115, -39, hull_hud_text, hull_text_sprites);
}

void game_scene::_display_status(const bn::string<50>& statustextone, const bn::string<50>& statustexttwo)
{
    my_text_generator.set_center_alignment();
    status_text_one_sprites.clear();
    my_text_generator.generate(0, 61, statustextone, status_text_one_sprites);
    status_text_two_sprites.clear();
    my_text_generator.generate(0, 72, statustexttwo, status_text_two_sprites);
}

void game_scene::_return_owls_to_tree()
{
    Player1Tableau.clear();
    current_power = 0;
    current_weight = 0;
    runes_which_might_disappear=0;
    _update_hud_text();
}

void game_scene::_point_cursor_at_sprite(const bn::sprite_ptr& target_sprite)
{   
    _selection_cursor_sprite.set_y(target_sprite.y());
    _selection_cursor_sprite.set_x(target_sprite.x()-26);
    _selection_cursor_sprite.set_visible(true);
}

bn::string<50> game_scene::_generate_description_from_owl_index(int card_info_index)
{
    
    int weight_to_add = CardInfoVector.at(card_info_index).weight;
    int power_to_add = CardInfoVector.at(card_info_index).power;
    int runes_to_add = CardInfoVector.at(card_info_index).gather;
    bn::string<50> _description_string("");
    _description_string.append("WEIGHT+");
    _description_string.append(bn::to_string<5>(weight_to_add));
    _description_string.append(", ATTACK+");
    _description_string.append(bn::to_string<5>(power_to_add));
    _description_string.append(", RUNES+");
    _description_string.append(bn::to_string<5>(runes_to_add));
    return(_description_string);
}


void game_scene::_generate_menu(int num_options, const bn::string<12>& menu_option_one, const bn::string<12>& menu_option_two, const bn::string<12>& menu_option_three)
{
    my_text_generator.set_center_alignment();

    first_menu_option_text_sprites.clear();
    second_menu_option_text_sprites.clear();
    third_menu_option_text_sprites.clear();
    my_text_generator.generate(0, 0, menu_option_one, first_menu_option_text_sprites);
    my_text_generator.generate(0, 11, menu_option_two, second_menu_option_text_sprites);
    my_text_generator.generate(0, 22, menu_option_three, third_menu_option_text_sprites);
    menu_position_max = num_options -1;
    menu_position = 0;
    _selection_cursor_sprite.set_visible(true);
    _update_selection_cursor_from_menu_position();

}

void game_scene::_navigate_through_menu()
{
    if(bn::keypad::up_pressed())
    {
        menu_position--;
        if(menu_position<0)
        {
            menu_position = 0;
        }
        _update_selection_cursor_from_menu_position();
    }
    if(bn::keypad::down_pressed())
    {
        menu_position++;
        if(menu_position > menu_position_max)
        {
            menu_position = menu_position_max;
        }
        _update_selection_cursor_from_menu_position();
    }
}

//point_cursor_at_sprite, point_cursor_at_string
//MENU: vector of vector of sprites?
void game_scene::_update_selection_cursor_from_menu_position()
{
    if(menu_position == 0)
    {
        _point_cursor_at_sprite(first_menu_option_text_sprites.at(0));
    }
    else if(menu_position == 1){
        _point_cursor_at_sprite(second_menu_option_text_sprites.at(0));
    }
    else{ //if you want to see error handling, there is none! bwahaha
        _point_cursor_at_sprite(third_menu_option_text_sprites.at(0));
    }
}

void game_scene::_clear_menu()
{
    _selection_cursor_sprite.set_visible(false);
    first_menu_option_text_sprites.clear();
    second_menu_option_text_sprites.clear();
    third_menu_option_text_sprites.clear();
}