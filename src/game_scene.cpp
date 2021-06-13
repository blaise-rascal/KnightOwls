
//////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                              //
//                                                                                              //
//                                                                                              //
//                                                                                              //
//                                                                                              //
//                                                                                              //
//                  Knight Owls game by Tom Horwath (Blaise Rascal)                             //
//                                                                                              //
//                This source code is terrible! View at your own risk!                          //
//                   Code is shared for education purposes ONLY.                                //
//                 You can use code snippets, but don't use the art,                            //
//                    music, or game design without my permission.                              //
//                Game engine used: https://github.com/GValiente/butano                         //
//                                                                                              //
//                                                                                              //
//                                                                                              //
//                                                                                              //
//                                                                                              //
//                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////



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

#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_items_knight_owls.h"
#include "bn_sprite_items_selection_cursor.h"
#include "bn_regular_bg_items_oceanbackground.h"
#include "bn_regular_bg_items_spellbook.h"

#include "game_scene.h"

#include "variable_8x8_sprite_font.h"


//TODO: clean up includes, remove from game those things built extraneously by $make clean $make

namespace{
        
        const int MERCS_FOR_SALE = 6;
        const int RUNES_PER_TURN = 4;
        const int MAX_BOAT_WEIGHT = 4;
        const int MAX_HULL = 4;
        const int STARTING_MERC_INDEX = 0;
        //const int 
        //const bn::array<3> MERC_POSITIONS
        //const bn::string<6> deploy_label_text("  DRAW");
}

//soon to do, better slection cursor, better energy bursts

game_scene::game_scene(bn::sprite_text_generator& text_generator):
    my_text_generator(text_generator),
    _ocean_bg(bn::regular_bg_items::oceanbackground.create_bg(0, 0)),
    _spellbook_bg(bn::regular_bg_items::spellbook.create_bg(0, 0)),
    //_chiyu_sprite(bn::sprite_items::chiyu.create_sprite(0, 0)),
    _selection_cursor_sprite(bn::sprite_items::selection_cursor.create_sprite(0, 30)),
    //weight_hud_text("WEIGHT: "),
    current_weight(0),
    current_power(0),
    current_runes(0),
    current_hull(0),
    runes_which_might_disappear(0),
    menu_position(0),
    state(0),
    last_tableau_x_pos(-110),
    
    last_merc_tableau_x_pos(-50),
    menu_position_max(1),
    current_wave(0),
    won_wave(false),
    total_merc_probs(0),
    random_num(0),
    player_stat_box_active(false),
    enemy_stat_box_active(false),
    exploded_once(false),
    previous_state(0)
{
    current_hull=MAX_HULL;

    // y = 12 * 1.2^2
    //attack, reward, penalty
    WaveInfoVector.push_back({14,1,1});
    WaveInfoVector.push_back({16,1,1});
    WaveInfoVector.push_back({19,1,1});
    WaveInfoVector.push_back({22,1,1});
    WaveInfoVector.push_back({25,1,2});
    WaveInfoVector.push_back({29,1,2});
    WaveInfoVector.push_back({34,1,2});
    WaveInfoVector.push_back({39,1,2});
    WaveInfoVector.push_back({46,-1, 9999});//-1 is victory, 9999 is death
    //WaveInfoVector.push_back({26});
    //WaveInfoVector.push_back({31});
    //WaveInfoVector.push_back({37});
    
    //                         name, cost, weight, power, gather, tileindex, availableforsale
    CardInfoVector.push_back({"MAGE",               3,0,0,      1,1,false});
    CardInfoVector.push_back({"ARCHER",             3,0,3,      0,2,false});
    CardInfoVector.push_back({"ENERGY SURGE",       0,1,5,      1,6,false});
    CardInfoVector.push_back({"MEGA ENERGY SURGE",  0,2,10,     2,7,false});
    CardInfoVector.push_back({"SPEAR-OWL",          5,0,6,      0,4,true});
    CardInfoVector.push_back({"MYSTIC",             5,0,0,      2,5,true});
    CardInfoVector.push_back({"THUG",               8,0,10,     0,0,true});
    CardInfoVector.push_back({"BRUISER",            12,0,16,    0,3,true});
    CardInfoVector.push_back({"SCIENTIST",          9,0,0,      4,8,true});
    CardInfoVector.push_back({"MERCHANT",           14,0,0,     7,9,true});

    //Generate the deck of mercs to draw from.
    for(int i =0; i< CardInfoVector.size(); i++)
    {
        if(CardInfoVector.at(i).availableforsale==true)
        {
            AllDrawableMercs.push_back(i);
        }
    }
    //CardInfoTempDeckWithoutReplacement

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
    _selection_cursor_sprite.set_z_order(-100);
    _selection_cursor_sprite.set_bg_priority(0); //lower z order means it shows up higher. wacky huh?

    _spellbook_bg.set_z_order(-90);
    _spellbook_bg.set_priority(0);

    _spellbook_bg.set_visible(false);
}

void game_scene::update()
{
    bn::random random_generator;
    //int random_num;

    //total merc probs started as 0, and is calculated here
    /*for(int i =0; i<CardInfoVector.size();i++)
    {
        total_merc_probs += CardInfoVector.at(i).probabilityweight;
    }*/
    while(true)
    {   
        //This is a deterministic random generator, so it must be spun every frame to not return the same numbers every boot.
        //Luckily this is not too slow to affect performance.
        //TODO: If I implement a title screen, this should start spinning then for maximum randomness
        random_num = random_generator.get(); // Do not alter this value! It continues spinning each frame

        switch(state)
        {
            case 0: // Beginning of game
            {   
                
                
                //_display_status(bn::to_string<50>(total_merc_probs));
                _display_status("A CHILL WIND BLOWS...","PRESS START TO BEGIN VOYAGE");
                for(int mercstoadd=0; mercstoadd<MERCS_FOR_SALE; mercstoadd++)
                {
                    //int card_to_add=0;
                    //MercenaryDeck.push_back(0);
                    bn::sprite_ptr NewTableauImg = bn::sprite_items::knight_owls.create_sprite(last_merc_tableau_x_pos, -38);
                    NewTableauImg.set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(0));
                    MercenaryTableau.push_back(NewTableauImg);
                    last_merc_tableau_x_pos+=20;
                    MercenaryTableau.at(mercstoadd).set_visible(false);
                }
                _update_hud_text();
                state = 101;
                break;
            }

//What SHOULD be done: summon 3 random owls at end of combat round (and when purchased show what's underneath)
//Maybe guarantee that an owl in the first round will be a mage? hmm
//summoned energy burst!
//+5 attack, +1 runes, +1 static
//summoned mega energy burst!
//+10 attack, +2 runes, +2 static
//Help Mode is ON.
//While Help Mode is enabled, tips will pop up, explaining how to play.
//You can disable Help Mode at any time in the PAUSE MENU by pressing START.
//HELP MODE TIP
//
//Only your flying units count for flying enemies!... nah... maybe]
//SPELLBOOK:  strange text at top
//All the owls in your spellbook are displayed here.
//When you summon an owl, it is chosen at random from the ones remaining in your spellbook.
//The ones you have already summoned are marked with an X.
//in owl language: RESTIAI FOLAR <-e umlaut
//The _____ is the elaborate summoning ritual in which talented summoners call on spirits from Owlhalla to aid them in battle.
//Static too high! You lose control.
//The summoning spell explodes!
//BRUISER and THUG
//ADD __ to your spellbook?
//SUMMON, SPELLBOOK, PASS
                    
            case 101: // Loop; wait for start press to add new enemy.
            {
                if(bn::keypad::start_pressed())
                {
                    state = 900;
                }
                bn::core::update();
                break; 
            }
            case 900:
            {
                bn::string<50> display_text_line_one("ENEMY APPEARS! kATTACK = ");
                display_text_line_one.append(bn::to_string<5>(WaveInfoVector.at(current_wave).attack));
                _display_status(display_text_line_one, "a:CONTINUE");

                
                enemy_stat_box_active=true;
                _update_enemy_stat_box();
                exploded_once=false;
                state = 1;
                break;
            }
            case 1: // Loop; wait for A press to start summon phase
            {
                if(bn::keypad::a_pressed())
                {
                    state = 10001;
                }
                bn::core::update();
                break;
            }
            case 10001:
            {
                //As you remove owls from your tree, you'll modify the player1deck. So save it here BEFORE modifications so we'll have access to it later
                player1deck_at_start_of_round = player1deck;
                player_stat_box_active=true;

                _generate_virt_menu(3, "SUMMON", "SPELLBOOK", "FIGHT!");
                if(exploded_once==false)
                {
                    _display_status("NEW ROUND START! SUMMONING PHASE","ud:MOVE, a:SELECT");
                }
                else
                {
                    _display_status("TRY, TRY AGAIN...","ud:MOVE, a:SELECT");
                }
                _update_hud_text();
                state = 3;
                break;
            }
            
            case 3: //Player1 Turn Loop
            {
                _navigate_through_virt_menu();//TODO: Make into separate state!
                
                if(bn::keypad::a_pressed())
                {
                    if(menu_position==0)
                    {
                        if(player1deck.size()>0){

                            //DRAW A CARD!!!
                            int index_to_remove = bn::abs(random_num) % player1deck.size();
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
                            bn::string<50> third_line_status("");
                            first_line_status.append(bn::to_string<18>(CardInfoVector.at(player1deck.at(index_to_remove)).name));
                            if(CardInfoVector.at(player1deck.at(index_to_remove)).weight==1)
                            {
                                first_line_status.append("!!!");
                            }
                            else if(CardInfoVector.at(player1deck.at(index_to_remove)).weight==2)
                            {
                                first_line_status.append("!!!!!!");
                            }
                            second_line_status.append(_generate_description_from_owl_index(player1deck.at(index_to_remove)));
                            if(current_weight>MAX_BOAT_WEIGHT){
                                _selection_cursor_sprite.set_visible(false);
                                
                                exploded_once=true;
                                third_line_status.append("iSTATIC TOO HIGH! a:CONTINUE");
                                state = 22;
                            }
                            else{
                                //remain in this state
                                third_line_status.append("ud:MOVE, a:SELECT");
                            }
                            //display status
                            _display_status(first_line_status,second_line_status,third_line_status);

                            //Display sprite
                            //newspriteposition= vector.at(size-1).position + 5
                            bn::sprite_ptr NewTableauImg = bn::sprite_items::knight_owls.create_sprite(last_tableau_x_pos, 20);
                            NewTableauImg.set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(CardInfoVector.at(player1deck.at(index_to_remove)).tileindex));//player1deck.at(index_to_remove).tileindex));
                            Player1Tableau.push_back(NewTableauImg);//bn::sprite_items::knight_owls.create_sprite(last_tableau_x_pos, 0));
                            //Player1Tableau.at().set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(20));
                            last_tableau_x_pos+=5;

                            //Delete the drawn card from the deck
                            player1deck.erase(player1deck.begin()+index_to_remove);
                        }
                        else{
                            _display_status("ERROR: NO MORE ITEMS","THIS SHOULDN'T HAPPEN");
                        }
                    }
                    else if(menu_position==1)
                    {//show spellbook!
                        previous_state=state;
                        state=23;
                    }
                    else if(menu_position==2)
                    {
                        state=5;
                    }
                }

                
                bn::core::update();
                break;
            }
            case 22: //Overloaded
            {
                if(bn::keypad::a_pressed())
                {
                    _display_status("YOU LOSE CONTROL.","THE SUMMONING SPELL EXPLODES!","a:CONTINUE");
                    state=4;
                }
                bn::core::update();
                break;
            }
            case 4: //Overloaded!
            {
                if(bn::keypad::a_pressed())
                {//TODO: JUST LOSE A LIFE AND GO BACK. ALSO, SAY WIN AND LOSE STUFF
                    
                    _return_owls_to_tree();
                    _display_status("ALL SUMMONED OWLS DISPERSE.","YOU LOSE 1me IN THE BLAST.","a:CONTINUE");
                    current_hull=current_hull-1;
                    _update_hud_text();
                    if(current_hull<=0)
                    {
                        state = 7;
                    }
                    else state = 1;
                }
                bn::core::update();
                break;
            }
            case 5: //Combat
            {
                //KILL SELECTION CURSOR, LABELS
                _clear_virt_menu();

                bn::string<50> first_line_status("SUMMONING OVER.");
                _display_status(first_line_status,"PRESS A TO RESOLVE COMBAT");
                _update_hud_text();
                state = 14;
                break;
            }
            case 14:
            {
                if(bn::keypad::a_pressed())
                {
                    if(current_power < WaveInfoVector.at(current_wave).attack)
                    {
                        
                        _display_status("ENEMY'S kATTACK IS HIGHER. DEFEAT!","PRESS A TO CONTINUE");
                        won_wave=false;
                        state=6;
                    }

                    if(current_power > WaveInfoVector.at(current_wave).attack)
                    {
                        _display_status("YOUR kATTACK IS HIGHER. VICTORY!","PRESS A TO CONTINUE");
                        won_wave=true;
                        state=13;
                    }
                    
                    if(current_power == WaveInfoVector.at(current_wave).attack)
                    {
                        _display_status("YOUR kATTACK = ENEMY'S kATTACK.","VICTORY!","PRESS A TO CONTINUE");
                        won_wave=true;
                        state=13;
                    }

                    /*if(current_power >= WaveInfoVector.at(current_wave).attack)
                    {
                        
                        state = 13;

                    }*/
                }
                bn::core::update();
                break;
            }
            case 13:
            {
                if(bn::keypad::a_pressed())
                {
                    if(current_wave==WaveInfoVector.size()-1)
                    {
                        state = 12; // YOU WIN!!!!!!!!!!!!!!!!!!!!
                    }
                    else
                    {
                        enemy_stat_box_active=false;
                        _update_enemy_stat_box();

                        
                        player_stat_box_active=false;

                        bn::string<50> first_line_status("REWARD: HEAL +");  
                        first_line_status.append(bn::to_string<4>(WaveInfoVector.at(current_wave).reward));
                        first_line_status.append(" mHP.");

                        bn::string<50> second_line_status("YOUR OWLS GATHERED +");  
                        second_line_status.append(bn::to_string<8>(runes_which_might_disappear));
                        second_line_status.append(" cDUST.");
                        
                        current_runes += runes_which_might_disappear;
                        runes_which_might_disappear = 0;
                        if(won_wave==true)
                        {
                            current_hull = current_hull + WaveInfoVector.at(current_wave).reward;
                            if(current_hull>MAX_HULL)
                            {
                                current_hull=MAX_HULL;
                            }
                            _display_status(first_line_status, second_line_status, "a:CONTINUE");//GATHER REWARD, GATHER
                        }
                        else
                        {
                            _display_status("COMBAT FINISHED.", second_line_status, "a:CONTINUE");
                        }
                        
                        _update_hud_text();
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
                    
                    enemy_stat_box_active=false;
                    bn::string<50> first_line_status("THE ENEMY STRIKES");
                    if(WaveInfoVector.at(current_wave).penalty>=1000)
                    {
                        first_line_status.append("!!!!!!!!!!");
                    }
                    else
                    {
                        first_line_status.append(", THEN RUNS AWAY!");
                    }
                    bn::string<50> second_line_status("YOU LOSE ");
                    second_line_status.append(bn::to_string<4>(WaveInfoVector.at(current_wave).penalty));  
                    second_line_status.append(" mHP.");  
                    current_hull=current_hull-WaveInfoVector.at(current_wave).penalty;
                    _update_hud_text();
                    _display_status(first_line_status, second_line_status, "a:CONTINUE");
                    if(current_hull<=0)
                    {
                        state = 7;
                    }
                    else{
                        state = 13;
                    }
                }
                bn::core::update();
                break;
            }
            case 7: // Game over
            {
                if(bn::keypad::a_pressed())
                {
                    enemy_stat_box_active=false;
                    _update_enemy_stat_box();
                    
                    _display_status("YOU DIED! GAME OVER.","RESETING NOT YET IMPLEMENTED");
                }
                bn::core::update();
                break;
            }
            case 8: // Either return owls or just move to buy phase
            {
                if(bn::keypad::a_pressed())
                {
                    // test this. it may not be correct
                    enemy_stat_box_active=false;
                    _update_enemy_stat_box();
                    
                    if(Player1Tableau.size()==0){
                        
                        state = 20;
                        /*if(won_wave==true)
                        {
                            state = 20;
                        }
                        else
                        {
                            state=10001;
                        }*/
                    }
                    else{
                        _return_owls_to_tree();
                        _display_status("ALL SUMMONED OWLS DISPERSE.","a:CONTINUE");
                        state = 9;
                        /*if(won_wave==true)
                        { 
                            _display_status("ALL SUMMONED OWLS DISPERSE.","a:CONTINUE");
                            state = 9;
                        }
                        else
                        {
                            state=10001;// ALWAYS go to 20 or 9
                        }*/
                    }
                }
                bn::core::update();
                break;
            }
            case 9: //Just returned owls, waiting on button press to enter buy phase
            {
                if(bn::keypad::a_pressed())
                {
                   state = 20;
                }
                bn::core::update();
                break;
            }
            case 20://intro to mercs appear
            {
                
                //Populate the deck to draw from
                TempMercDeckToDrawFrom = AllDrawableMercs;

                //Choose all the mercs
                //There will always be a mage for sale
                //There will always be an archer for sale
                //Maybe 3 random owls without replacement? (can be another mage or archer)
                //then 1 random owl that can be anything (including a repeat of any of the first ones)
                MercenaryDeck.clear();
                MercenaryDeck.push_back(0);//mage
                MercenaryDeck.push_back(1);//archer

                
                //Populate elements 2,3,4. Random owls with replacement from AllDrawableMercs
                for(int MercDeckIndex = 2; MercDeckIndex < 5; MercDeckIndex++)
                {
                    int card_to_draw= bn::abs(random_num) % TempMercDeckToDrawFrom.size();
                    MercenaryDeck.push_back(TempMercDeckToDrawFrom.at(card_to_draw));

                    //Delete the drawn card from the deck
                    TempMercDeckToDrawFrom.erase(TempMercDeckToDrawFrom.begin()+card_to_draw);
                    //spin the random number generator! (TODO: experiment with not spinning it and instead just using the same number but modulating it down a bunch; would be faster)
                    random_num = random_generator.get();
                }

                //Finally, draw 1 card that can be anything from AllDrawableMercs
                int card_to_draw = bn::abs(random_num) % AllDrawableMercs.size();
                MercenaryDeck.push_back(AllDrawableMercs.at(card_to_draw));
                


                for(int merctoillustrate=0; merctoillustrate<MERCS_FOR_SALE; merctoillustrate++)
                {
                    //int card_to_add=0;
                    //MercenaryDeck.push_back(0);
                    //bn::sprite_ptr NewTableauImg = bn::sprite_items::knight_owls.create_sprite(last_merc_tableau_x_pos, -38);
                    //NewTableauImg.set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(0);
                    //MercenaryTableau.push_back(NewTableauImg);
                    //last_merc_tableau_x_pos+=20;
                    MercenaryTableau.at(merctoillustrate).set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(CardInfoVector.at(MercenaryDeck.at(merctoillustrate)).tileindex));
                    MercenaryTableau.at(merctoillustrate).set_visible(true);
                }
                _display_status("A RIFT TO OWLHALLA APPEARS!","YOU MAY CONSCRIPT NEW OWLS WITH c.","a:CONTINUE");
                
                state = 21;
                break;
            }
            case 21:
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
                _display_status("BUY PHASE","ud:MOVE, a:SELECT");
                 
                _generate_virt_menu(3, "SHOP", "SPELLBOOK", "PASS");
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
                
                _navigate_through_virt_menu();
                if(bn::keypad::a_pressed())
                {
                    if(menu_position==0) // BUY
                    {
                        
                        if(_is_merc_deck_empty())
                        {
                            _display_status("NONE LEFT TO PURCHASE","a:CONTINUE");
                            state=21;
                        }
                        else{
                            state=19;
                        }
                    }
                    else if(menu_position==1) // SPELLBOOK
                    {//show spellbook!
                        previous_state=state;
                        state=23;
                    }
                    else if(menu_position==2) // PASS
                    {
                        current_wave+=1;
                        _clear_virt_menu();
                        state=900;

                    }
                }
                bn::core::update();
                break;
            }
            case 12:{
                _display_status("VOYAGE COMPLETE. YOU WIN!");
                bn::core::update();
                break;
            }
            case 19:{//to integrate or not...
                _start_hor_merc_menu();
                state=15;
                break;
            }
            case 15:{//BUY MENU!
                //check if all mercs purchased
                _navigate_through_hor_menu();

                if(bn::keypad::b_pressed())
                {
                    state = 10;
                }
                else if(bn::keypad::a_pressed())
                {
                    state = 16;
                }
                bn::core::update();
                break;
            }
            case 16:{
                bn::string<50> first_line_status("PURCHASE ");//TODO: Make this not update every frame
                first_line_status.append(CardInfoVector.at(MercenaryDeck.at(menu_position)).name);
                first_line_status.append(" FOR ");
                first_line_status.append(bn::to_string<8>(CardInfoVector.at(MercenaryDeck.at(menu_position)).cost));
                first_line_status.append("c?");
                bn::string<50> second_line_status("a:YES, b:NO");
                _display_status(first_line_status,second_line_status);
                state = 17;
                break;
            }
            case 17:{

                if(bn::keypad::b_pressed())//decided not to purchase, go back to hor menu
                {
                    //_selection_cursor_sprite.set_visible(true);
                    //_display_status("GOTTHISFAR");
                    _update_selection_cursor_from_hor_menu_position();
                    state=15;
                }
                else if(bn::keypad::a_pressed())
                {
                    _selection_cursor_sprite.set_visible(false);
                    if(current_runes >= CardInfoVector.at(MercenaryDeck.at(menu_position)).cost)
                    {
                        bn::string<50> first_line_status("");
                        first_line_status.append(CardInfoVector.at(MercenaryDeck.at(menu_position)).name);
                        first_line_status.append(" ADDED TO SPELLBOOK"); // clear that sprite!
                        MercenaryTableau.at(menu_position).set_visible(false);
                        player1deck.push_back(MercenaryDeck.at(menu_position));
                        current_runes -= CardInfoVector.at(MercenaryDeck.at(menu_position)).cost;
                        _update_hud_text();
                        _display_status(first_line_status,"a:CONTINUE");

                        
                        MercenaryDeck.at(menu_position) = -1; //-1 means that sprite is not present

                        if(_is_merc_deck_empty()==true)
                        {
                            state = 21;
                        }
                        else
                        {
                            state = 18;
                        }
                    }
                    else
                    {
                        _display_status("NOT ENOUGH cDUST!","a:CONTINUE");
                        state = 18;
                    }
                }
                bn::core::update();
                break;
            }
            case 18:{//waiting to return to purchase
                if(bn::keypad::a_pressed())
                {
                    state = 19;
                    //_selection_cursor_sprite.set_visible(true);
                }
                bn::core::update();
                break;
            }
            case 23:{//spellbook!
                
                _display_status("DISPLAYING SPELLBOOK","a:CONTINUE");
                //show the 
                state = 24;
                
                _spellbook_bg.set_visible(true);
                //_selection_cursor_sprite.set_visible(true);
                
                bn::core::update();
                break;//Overloaded
            }
            case 24:{//spellbook!
                if(bn::keypad::a_pressed())
                {
                    
                    _spellbook_bg.set_visible(false);
                    if(previous_state==3)
                        state=3;
                    else
                        state=10;
                    //_selection_cursor_sprite.set_visible(true);
                }
                bn::core::update();
                break;//Overloaded
            }
            /*case 19:{
                if(bn::keypad::a_pressed())
                {
                    //_replace_merc_with_random_owl(menu_position);
                    
                    MercenaryTableau.at(menu_position).set_visible(true);
                    
                    bn::string<50> first_line_status("NEW MERCENARY REVEALED: ");
                    //first_line_status.append(bn::to_string<11>(temp_rando));
                    //first_line_status.append(", ");
                    //first_line_status.append(bn::to_string<11>(total_merc_probs));
                    //first_line_status.append(", ");
                    //first_line_status.append(bn::to_string<11>(probweight_to_add));
                    //first_line_status.append(", ");
                    first_line_status.append(CardInfoVector.at(MercenaryDeck.at(menu_position)).name);
                    _display_status(first_line_status,"PRESS A TO CONTINUE");
                    state = 18;
                }
                bn::core::update();
                break;
            }*/
            default:
            {
                BN_ERROR("Invalid state");
                break;
            }
        }
        
    }
}


void game_scene::_update_enemy_stat_box()
{
    my_text_generator.set_center_alignment();
    first_enemy_stat_text_sprites.clear();
    second_enemy_stat_text_sprites.clear();
    third_enemy_stat_text_sprites.clear();
    if(enemy_stat_box_active)
    {
        bn::string<16> first_enemy_stat_text("k");
        bn::string<16> second_enemy_stat_text("WIN:");
        bn::string<16> third_enemy_stat_text("LOSE:");

        first_enemy_stat_text.append(bn::to_string<4>(WaveInfoVector.at(current_wave).attack));

        if(WaveInfoVector.at(current_wave).reward==-1)
        {   
            second_enemy_stat_text.append("VICTORY!");
        }
        else
        {
            second_enemy_stat_text.append("+");
            second_enemy_stat_text.append(bn::to_string<4>(WaveInfoVector.at(current_wave).reward));
            second_enemy_stat_text.append("m");
        }

        if(WaveInfoVector.at(current_wave).penalty==9999)
        {   
            third_enemy_stat_text.append("oDEATHo");
        }
        else
        {
            third_enemy_stat_text.append("-");
            third_enemy_stat_text.append(bn::to_string<4>(WaveInfoVector.at(current_wave).penalty));
            third_enemy_stat_text.append("m");
        }
        
        my_text_generator.generate(70, -38, first_enemy_stat_text, first_enemy_stat_text_sprites);
        my_text_generator.generate(70, -27, second_enemy_stat_text, second_enemy_stat_text_sprites);
        my_text_generator.generate(70, -16, third_enemy_stat_text, third_enemy_stat_text_sprites);
    }  
}


void game_scene::_update_hud_text()
{
    //TODO: make this just make & use 1 string rather than making a million. lol
    my_text_generator.set_left_alignment();
    //needed: enemy wave sprites, runes which might disappear sprite
    hull_text_sprites.clear();
    bn::string<20> hull_hud_text("");
    for(int hull_to_check = 0; hull_to_check < MAX_HULL; hull_to_check++)
    {
        if(hull_to_check<current_hull)
        {
            hull_hud_text.append("me");
        }
        else{
            hull_hud_text.append("ne");
        }
    }
    // The butano creator told me I should leave an 8 px border around the screen, but actual gba games would leave, like a 1 px border from the edge
    // Let's do 4px from the edge so that his feelings aren't hurt
    my_text_generator.generate(-116, -72, hull_hud_text, hull_text_sprites);

    total_runes_text_sprites.clear();
    bn::string<20> total_runes_hud_text("cx");
    total_runes_hud_text.append(bn::to_string<8>(current_runes));
    my_text_generator.generate(-116, -61, total_runes_hud_text, total_runes_text_sprites);



    

    my_text_generator.set_right_alignment();
    //needed: enemy wave sprites, runes which might disappear sprite
    wave_text_sprites.clear();
    bn::string<20> wave_hud_text("");
    for(int wave_to_check = 0; wave_to_check < WaveInfoVector.size(); wave_to_check++)
    {
        if(wave_to_check==WaveInfoVector.size()-1)
        {
            if(wave_to_check==current_wave)
                wave_hud_text.append("p");//red skull
            else
                wave_hud_text.append("o"); //white skull
        }
        else if(wave_to_check < current_wave)
        {
            wave_hud_text.append("y");//z is open but i like y better
        }
        else if(wave_to_check == current_wave)
        {
            wave_hud_text.append("w");//where you are
        }
        else{
            wave_hud_text.append("y");//closed
        }
    }
    my_text_generator.generate(118, -72, wave_hud_text, wave_text_sprites);


    power_text_sprites.clear();
    runes_that_might_disappear_text_sprites.clear();
    weight_text_sprites.clear();

    if(player_stat_box_active==true)
    {
        my_text_generator.set_center_alignment();

        bn::string<20> power_hud_text("k");
        power_hud_text.append(bn::to_string<8>(current_power));
        my_text_generator.generate(-70, -38, power_hud_text, power_text_sprites);

    

        bn::string<20> runes_that_might_disappear_hud_text("c+");
        runes_that_might_disappear_hud_text.append(bn::to_string<8>(runes_which_might_disappear));
        my_text_generator.generate(-70, -27, runes_that_might_disappear_hud_text, runes_that_might_disappear_text_sprites);

        //-39
        bn::string<20> weight_hud_text("i");
        weight_hud_text.append(bn::to_string<8>(current_weight));
        weight_hud_text.append("/");
        weight_hud_text.append(bn::to_string<2>(MAX_BOAT_WEIGHT));
        my_text_generator.generate(-70, -16, weight_hud_text, weight_text_sprites);
    }
    //-115 -39
    //-115 -50
    //-115 -61
    //-115 -72

    
}

void game_scene::_display_status(const bn::string<50>& statustextone, const bn::string<50>& statustexttwo, const bn::string<50>& statustextthree)
{
    my_text_generator.set_center_alignment();
    status_text_one_sprites.clear();
    status_text_two_sprites.clear();
    status_text_three_sprites.clear();
    if(statustexttwo.length()>0 && statustextthree.length()>0)//all three lines occupied
    {
        my_text_generator.generate(0, 50, statustextone, status_text_one_sprites);
        my_text_generator.generate(0, 61, statustexttwo, status_text_two_sprites);
        my_text_generator.generate(0, 72, statustextthree, status_text_three_sprites);
    }
    else if(statustexttwo.length()>0)//two lines occupied
    {
        my_text_generator.generate(0, 55, statustextone, status_text_one_sprites);
        my_text_generator.generate(0, 67, statustexttwo, status_text_two_sprites);
    }
    else//one line occupied
    {
        my_text_generator.generate(0, 61, statustextone, status_text_one_sprites);
    }
    //todo: error handling for this
}
/*
void game_scene::_replace_merc_with_random_owl(int which_merc_position)
{
    int card_to_add=10000;
    int temp_rando=bn::abs(random_num);
    int probweight_to_add=temp_rando%total_merc_probs;//Look at the last n digits, where n is total weight sum


    bool foundcard=false;
    

    int count_up_weights=0;
    int card_to_check=0;
    while(foundcard==false)
    {
        count_up_weights+=CardInfoVector.at(card_to_check).probabilityweight;
        if(probweight_to_add < count_up_weights)
        {
            foundcard = true;
            card_to_add = card_to_check;
        }
        card_to_check++;
    }
    
    //TODO: handle error if foundcard is false

    MercenaryTableau.at(which_merc_position).set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(CardInfoVector.at(card_to_add).tileindex));
    MercenaryDeck.at(which_merc_position)=card_to_add;
}

*/

void game_scene::_return_owls_to_tree()
{
    Player1Tableau.clear();
    current_power = 0;
    current_weight = 0;
    runes_which_might_disappear=0;
    last_tableau_x_pos = -110;
    _update_hud_text();
    player1deck = player1deck_at_start_of_round;
}

void game_scene::_point_cursor_at_letter(const bn::sprite_ptr& target_sprite)
{   
    _selection_cursor_sprite.set_y(target_sprite.y());
    _selection_cursor_sprite.set_x(target_sprite.x()-25);
    _selection_cursor_sprite.set_visible(true);
}

void game_scene::_point_cursor_at_owl(const bn::sprite_ptr& target_sprite)
{   
    _selection_cursor_sprite.set_y(target_sprite.y());
    _selection_cursor_sprite.set_x(target_sprite.x()-15);
    _selection_cursor_sprite.set_visible(true);
}


bn::string<50> game_scene::_generate_description_from_owl_index(int card_info_index)
{
    
    int weight_to_add = CardInfoVector.at(card_info_index).weight;
    int power_to_add = CardInfoVector.at(card_info_index).power;
    int runes_to_add = CardInfoVector.at(card_info_index).gather;
    bn::string<50> _description_string("");
    _description_string.append("k+");
    _description_string.append(bn::to_string<5>(power_to_add));
    _description_string.append(" c+");
    _description_string.append(bn::to_string<5>(runes_to_add));
    _description_string.append(" i+");
    _description_string.append(bn::to_string<5>(weight_to_add));
    return(_description_string);
}


void game_scene::_generate_virt_menu(int num_options, const bn::string<12>& menu_option_one, const bn::string<12>& menu_option_two, const bn::string<12>& menu_option_three)
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
    _update_selection_cursor_from_virt_menu_position();

}

void game_scene::_navigate_through_virt_menu()
{
    if(bn::keypad::up_pressed())
    {
        menu_position--;
        if(menu_position<0)
        {
            menu_position = 0;
        }
        _update_selection_cursor_from_virt_menu_position();
    }
    if(bn::keypad::down_pressed())
    {
        menu_position++;
        if(menu_position > menu_position_max)
        {
            menu_position = menu_position_max;
        }
        _update_selection_cursor_from_virt_menu_position();
    }
}

void game_scene::_navigate_through_hor_menu()
{
    
    if(bn::keypad::left_pressed())
    {
        /////
        bool found_first_available=false;
        for (int one_to_check=menu_position-1; one_to_check>=0; one_to_check--)
        {
            if(MercenaryDeck.at(one_to_check) != -1 && found_first_available==false)
            {
                found_first_available=true;
                menu_position=one_to_check;
            }
        }
        //if found_first_available is still false, then you stay in the same place
        //menu_position--;
        //if(menu_position<0)
        //{
        //    menu_position = 0;
        //}
        _update_selection_cursor_from_hor_menu_position();
    }
    if(bn::keypad::right_pressed())
    {
        bool found_first_available=false;
        for (int one_to_check=menu_position+1; one_to_check<MercenaryDeck.size(); one_to_check++)
        {
            if(MercenaryDeck.at(one_to_check) != -1 && found_first_available==false)
            {
                found_first_available=true;
                menu_position=one_to_check;
            }
        }
        _update_selection_cursor_from_hor_menu_position();
    }
}


//point_cursor_at_sprite, point_cursor_at_string
//MENU: vector of vector of sprites?
void game_scene::_update_selection_cursor_from_virt_menu_position()
{
    if(menu_position == 0)
    {
        _point_cursor_at_letter(first_menu_option_text_sprites.at(0));
    }
    else if(menu_position == 1){
        _point_cursor_at_letter(second_menu_option_text_sprites.at(0));
    }
    else{ //if you want to see error handling, there is none! bwahaha (really, this should be a switch statement. or the text sprites should be a vector so this whole decision chain would be unnecessary)
        _point_cursor_at_letter(third_menu_option_text_sprites.at(0));
    }
}


void game_scene::_update_selection_cursor_from_hor_menu_position()
{
    
    bn::string<50> first_line_status("");
    bn::string<50> second_line_status("");
    bn::string<50> third_line_status("lr:MOVE, a:SELECT, b:CANCEL");
    first_line_status.append(bn::to_string<18>(CardInfoVector.at(MercenaryDeck.at(menu_position)).name));
    first_line_status.append(" (COST: ");
    first_line_status.append(bn::to_string<5>(CardInfoVector.at(MercenaryDeck.at(menu_position)).cost));
    first_line_status.append("c)");
    second_line_status.append(_generate_description_from_owl_index(MercenaryDeck.at(menu_position)));
    
    _display_status(first_line_status,second_line_status,third_line_status);
    _point_cursor_at_owl(MercenaryTableau.at(menu_position));
}

void game_scene::_clear_virt_menu()
{
    _selection_cursor_sprite.set_visible(false);
    first_menu_option_text_sprites.clear();
    second_menu_option_text_sprites.clear();
    third_menu_option_text_sprites.clear();
    for(int i = 0; i<MercenaryDeck.size(); i++)
    {
        MercenaryTableau.at(i).set_visible(false);
    }
}

void game_scene::_start_hor_merc_menu()
{
    menu_position_max = MERCS_FOR_SALE - 1; //todo: delete this line

    /////
    bool found_first_available=false;
    int firstavailable = 1000;
    int one_to_check=0;
    while(!found_first_available)
    {
        if(MercenaryDeck.at(one_to_check) != -1)
        {
            found_first_available=true;
            firstavailable=one_to_check;
        }
        else{
            one_to_check++;
        }
    }
    menu_position = firstavailable;



    _selection_cursor_sprite.set_visible(true);
    //_display_status("GOTTHISFAR");
    _update_selection_cursor_from_hor_menu_position();
}

bool game_scene::_is_merc_deck_empty()
{
    bool allnegativeone = true;
    for(int i = 0; i<MercenaryDeck.size(); i++)
    {
        if(MercenaryDeck.at(i) != -1)
        {
            allnegativeone=false;
        }
    }
    return allnegativeone;
}