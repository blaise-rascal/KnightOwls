
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                      //
//                                                                                                      //
//                                                                                                      //
//                                                                                                      //
//                                                                                                      //
//                                                                                                      //
//                      Knight Owls game by Tom Horwath (Blaise Rascal)                                 //
//                                                                                                      //
//                    This source code is terrible! View at your own risk!                              //
//                                                                                                      //
//                                          LICENSE:                                                    //
//                            Play the game all you want, for free.                                     //
//  Put it on your retro console, or your computer, or whatever! Recompile it from source! Go hog wild! //
// Either distribute it (for free) OR modify the source, but please don't distribute a modified version.//
//   (Exceptions can be made; please contact me to discuss releasing your proposed modified version.)   //
//                                                                                                      //
//                  Source code is shared primarily for education purposes.                             //
//      You can use code snippets in your own projects, but don't use the art, or the music,            //
//            or rip off huge portions of the game's design without my permission.                      //
//          (Feel free to ask permission! There's a good chance I'll give it to you.)                   //
//                  Additionally, all licenses in the licenses folder apply.                            //
//                                                                                                      //
//                                                                                                      //
//                                                                                                      //
//                                                                                                      //
//                                                                                                      //
//                                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////
//BRAINSTORMING//
/////////////////

//STORY MODE - A set of short story-driven challenges.
//Chapter 1 (Tutorial)
//ROGUELIKE MODE - A long, randomly generated, difficult journey. For the hardcore players!

//LEVEL 1 - A relatively quick and easy journey, with one boss at the end.
//LEVEL 2 - Introduces the banner mechanic, and features two bosses, with a spellbook reset in the middle. 
//LEVEL 3 - A long and arduous journey featuring three bosses and two spellbook resets. Are you up for the challenge?
//TUTORIAL & CREDITS - All info for this game is at ____ since I didn't have time to write it into the game.

//Game by Tom Horwath (Blaise Rascal).

//+4c (25% CHANCE TO DO NOTHING)
//+5k (25% CHANCE FOR DOUBLE)
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
#include "bn_sprite_items_right_book_arrow.h"
#include "bn_sprite_items_left_book_arrow.h"
#include "bn_sprite_items_banners.h"
#include "bn_sprite_items_enemies.h"
//#include "bn_sprite_items_button.h"
#include "bn_sprite_items_salearrows.h"
#include "bn_regular_bg_items_oceanbackground.h"
#include "bn_regular_bg_items_spellbook.h"
#include "bn_regular_bg_items_rift.h"

#include "game_scene.h"

#include "bn_music_items.h"
#include "bn_music_actions.h"
#include "variable_8x8_sprite_font.h"


//TODO: clean up includes, remove from game those things built extraneously by $make clean $make

namespace{
    
    const int MERCS_FOR_SALE = 6;
    const int RUNES_PER_TURN = 4;
    //const int MAX_BOAT_WEIGHT = 4;
    //const int MAX_HULL = 4;
    const int STARTING_MERC_INDEX = 0;
    const int NUMBER_SPELLBOOK_COLUMNS = 6;
    const int NUMBER_SPELLBOOK_ROWS = 3;
    const int ZONE_ONE_BG_Y = 256;
    const int ZONE_TWO_BG_Y = 0;
    const int ZONE_THREE_BG_Y = -256;
    //const int START_OF_ZONE_TWO = 12;
    //const int START_OF_ZONE_THREE = 24;
    //const int 
    //const bn::array<3> MERC_POSITIONS
    //const bn::string<6> deploy_label_text("  DRAW");
}

//soon to do, better slection cursor, better energy bursts

game_scene::game_scene(bn::sprite_text_generator& text_generator):
    my_text_generator(text_generator),
    _ocean_bg(bn::regular_bg_items::oceanbackground.create_bg(0,ZONE_ONE_BG_Y)),
    //_swamp_bg(bn::regular_bg_items::swampbackground.create_bg(0, 0)),
    _spellbook_bg(bn::regular_bg_items::spellbook.create_bg(0, 0)),
    _rift_bg(bn::regular_bg_items::rift.create_bg(0, 0)),
    _selection_cursor_sprite(bn::sprite_items::selection_cursor.create_sprite(0, 30)),
    _right_book_arrow_sprite(bn::sprite_items::right_book_arrow.create_sprite(110, -15)),
    _left_book_arrow_sprite(bn::sprite_items::left_book_arrow.create_sprite(-110, -15)),
    _enemy_sprite(bn::sprite_items::enemies.create_sprite(70, 10)), //TODO: Push up your stat box & enemy stat box, and enemy sprite

    //weight_hud_text("WEIGHT: "),
    current_weight(0),
    current_power(0),
    current_runes(0), // MODIFY THIS VALUE TO SET STARTING RUNES
    current_hull(0),
    max_hull(4),
    max_boat_weight(4),
    runes_which_might_disappear(0),
    menu_position(0),
    state(0),
    last_tableau_x_pos(-110),
    last_tableau_y_pos(10),
    
    last_merc_tableau_x_pos(-50),
    menu_position_max(1),
    current_wave(0),
    won_wave(false),
    total_merc_probs(0),
    random_num(0),
    player_stat_box_active(false),
    enemy_stat_box_active(false),
    state_before_spellbook(0),
    state_before_summon_start(0),
    number_sb_pages(0),
    current_sb_page(0),
    current_sb_owl(0),
    upgrade_option_one(0),
    upgrade_option_two(0),
    //current_zone(0),
    start_of_zone_two(0),
    start_of_zone_three(0),
    last_banner_tableau_x_pos(-110)
{
    current_hull=max_hull;

    bn::music_items::voyagemusic.play(0.5);
    
    //WaveInfoVector.push_back({-2,0,0,0});//-2 is miasma
    //WaveInfoVector.push_back({-2,0,0,0});//-2 is miasma
    //WaveInfoVector.push_back({14,1,1,0});
    //WaveInfoVector.push_back({14,1,1,0});
    //attack, reward, penalty, enemyinfoindex
    
    WaveInfoVector.push_back({14,1,1,0}); //LILYBAD
    WaveInfoVector.push_back({17,1,1,1}); //MARAUDING OWLSHIP 
    WaveInfoVector.push_back({20,1,1,2}); //FLOATING VILESTAR
    WaveInfoVector.push_back({23,1,1,3}); //LAMB ON THE LAM
    WaveInfoVector.push_back({-1,0,0,0});//-1 is shipwreck
    WaveInfoVector.push_back({29,1,2,4}); //DIRE LILYBAD
    WaveInfoVector.push_back({35,1,2,5}); //GOBLIN SURF-SHIP
    WaveInfoVector.push_back({42,1,2,6}); //CAT HYDRA
    WaveInfoVector.push_back({50,1,2,7}); //PRETZELCOATL
    WaveInfoVector.push_back({60, 9999, 9999,12});//-1 is victory, 9999 is death //LILYBANE
    WaveInfoVector.push_back({-1,0,0,0});//-1 is shipwreck
    WaveInfoVector.push_back({-2,0,0,0});//-2 is miasma
    WaveInfoVector.push_back({15,1,1,27}); //SURPRISED LILYBAD
    WaveInfoVector.push_back({18,1,1,17}); //HAUNTED OWLSHIP
    WaveInfoVector.push_back({21,1,1,13}); // LAMB WITH A LAMBIC
    WaveInfoVector.push_back({26,1,1,20}); // CORRUPTED SHADOWSTAR
    WaveInfoVector.push_back({-1,0,0,0});//-1 is shipwreck
    WaveInfoVector.push_back({33,1,2,21}); // SWIMMIN' WOMEN
    WaveInfoVector.push_back({41,1,2,19}); // VICIOUS FISHES
    WaveInfoVector.push_back({50,1,2,23}); //CAT HYDRA WITH SUNGLASSES
    WaveInfoVector.push_back({61,1,2,18}); // BORIS THE GRIMDARK
    WaveInfoVector.push_back({75, 9999, 9999,16});// GREAT PACIFIC GARBAGE PATCH
    WaveInfoVector.push_back({-1,0,0,0});//-1 is shipwreck
    WaveInfoVector.push_back({-2,0,0,0});//-2 is miasma
    WaveInfoVector.push_back({15,1,1,28}); //LILYDADJOKE
    WaveInfoVector.push_back({19,1,1,22}); //MIASMOID
    WaveInfoVector.push_back({23,1,1,26}); //BACKWARDS SURF-SHIP
    WaveInfoVector.push_back({29,1,1,14}); // CYCLOPEAN HOVERSTAR
    WaveInfoVector.push_back({-1,0,0,0});//-1 is shipwreck
    WaveInfoVector.push_back({37,1,2,11}); // INVISIBLE NEMESIS
    WaveInfoVector.push_back({46,1,2,24}); //MARY
    WaveInfoVector.push_back({58,1,2,15}); // CAT HYDRA WITH PARTY HATS
    WaveInfoVector.push_back({72,1,2,25}); // GUNGLASSES
    WaveInfoVector.push_back({90, 9999, 9999,8}); //HERMAN
    


    //TODO: NO SHOP AFTER BOSS!!!!!!!!!!!!!!!!!

    for(int i = 0; i < WaveInfoVector.size(); i++)
    {
        if (WaveInfoVector.at(i).attack==-2)
        {
            if(start_of_zone_two==0)
            {
                start_of_zone_two = i+1;
            }
            else
                start_of_zone_three = i+1;
        }
    }
    //
    EnemyInfoVector.push_back({0,"LILYBAD"});
    EnemyInfoVector.push_back({1,"MARAUDING OWLSHIP"});
    EnemyInfoVector.push_back({2,"VILE HOVERSTAR"});
    EnemyInfoVector.push_back({3,"LAMB ON THE LAM"});
    EnemyInfoVector.push_back({4,"DIRE LILYBAD"});
    EnemyInfoVector.push_back({5,"GOBLIN SURF-SHIP"});
    EnemyInfoVector.push_back({6,"CAT HYDRA"});
    EnemyInfoVector.push_back({7,"PRETZELCOATL"});
    EnemyInfoVector.push_back({8,"HERMAN THE MERMAN"});
    EnemyInfoVector.push_back({9,"SHIPWRECK"});
    EnemyInfoVector.push_back({10,"MIASMA"});
    EnemyInfoVector.push_back({11,"INVISIBLE NEMESIS"});
    EnemyInfoVector.push_back({12,"LILYBANE"});
    EnemyInfoVector.push_back({13,"LAMB WITH A LAMBIC"});
    EnemyInfoVector.push_back({14,"CYCLOPEAN HOVERSTAR"});
    EnemyInfoVector.push_back({15,"CAT HYDRA WITH PARTY HATS"});
    EnemyInfoVector.push_back({16,"GREAT PACIFIC TRASH HEAP"});
    EnemyInfoVector.push_back({17,"HAUNTED OWLSHIP"});
    EnemyInfoVector.push_back({18,"BORIS THE GRIMDARK"});
    EnemyInfoVector.push_back({19,"VICIOUS FISHES"});
    EnemyInfoVector.push_back({20,"CORRUPTED SHADOWSTAR"});
    EnemyInfoVector.push_back({21,"SWIMMIN' WOMEN"});
    EnemyInfoVector.push_back({22,"MIASMOID"});
    EnemyInfoVector.push_back({23,"CAT HYDRA WITH SUNGLASSES"});
    EnemyInfoVector.push_back({24,"MARY"});
    EnemyInfoVector.push_back({25,"GUNGLASSES"});
    EnemyInfoVector.push_back({26,"BACKWARDS SURF-SHIP"});
    EnemyInfoVector.push_back({27,"NAKED LILYBAD"});
    EnemyInfoVector.push_back({28,"LILYDADJOKE"});

    //8 d20 (5% to do +10k)
        //9 courage (+1 max static, -1 max HP)
        //10 strength (+2 max HP)
        //11 coupon (one owl goes on sale every round)
        //12 first aid (after fight, 50% chance for +1hp)
        //13 surge (first 3 owls summoned will not be surges)
        //14 risk (after loss +2c)
        //15 goblin (permanent goblin added to deck)
        //16 When you fight, if your +c this round was 8 or higher, gain +5k
    NonOwlUpgradeInfoVector.push_back({"D20","WHENEVER YOU SUMMON, ROLL A D20.","IF IT'S A 20, GET +10k."}); //0
    NonOwlUpgradeInfoVector.push_back({"COURAGE","+1 MAX iSTATIC, BUT -2 MAX mHP.",""}); //1
    NonOwlUpgradeInfoVector.push_back({"STRENGTH","+2 MAX AND CURRENT mHP.",""}); //2
    NonOwlUpgradeInfoVector.push_back({"STONKS","EVERY ROUND, 1 RANDOM OWL","COSTS -2c, AND ANOTHER +2c."}); //3
    NonOwlUpgradeInfoVector.push_back({"FIRST AID","AFTER FIGHT, 75% CHANCE TO","HEAL 1 mHP."}); //4
    NonOwlUpgradeInfoVector.push_back({"PITY","AFTER YOU LOSE A FIGHT,","GAIN +4c."}); //5
    NonOwlUpgradeInfoVector.push_back({"GOBLIN","ADD A PERMANENT GOBLIN TO","YOUR SPELLBOOK. (GOBLIN HAS +1D10k)"}); //6
    NonOwlUpgradeInfoVector.push_back({"MONEYBAGS","WHEN YOU FIGHT, IF YOUR +c THIS","ROUND WAS 10 OR HIGHER, GAIN +6k"}); //7
    //NonOwlUpgradeInfoVector.push_back({"COUNTDOWN","FIRST 2 SUMMONS WILL NOT BE","SURGES."});

//rare owls (probably listed elsewhere as well)
//bombardier

    AmountMercOnSale.fill(0);

    //each cardinfovector has an abilities vector, with 2 abilities (for now)
    //abilityindex & value
    //{0=atk 5=value} {2=static 1=value}
    //

    //mage = barn owl
    //archer = ?
    //spear-owl = ?
    //mystic = ?
    //thug = ?
    //energy knight = great horned owl
    //alchemist = ?
    //merchant = snowy owl


    //                         name,    cost, weight,     p1,p2,p2%    g1,g2,g2%   tileindex, rarity
   
    //CardInfoVector.push_back({"MAGE",               3,0,    0,0,0,      1,0,0,      1,1}); // stuff to add: int attackone int attackonepercentage int attacktwo int attacktwopercentage int 
    //CardInfoVector.push_back({"ARCHER",             4,0,    3,0,0,      0,0,0,      2,1});
    CardInfoVector.push_back({"MAGE",               3,0,    0,0,0,      1,0,0,      1,1}); // stuff to add: int attackone int attackonepercentage int attacktwo int attacktwopercentage int 
    CardInfoVector.push_back({"ARCHER",             4,0,    3,0,0,     0,0,0,      2,1});
    CardInfoVector.push_back({"ENERGY SURGE",       0,1,    5,0,0,      1,0,0,      6,0});
    CardInfoVector.push_back({"MEGA ENERGY SURGE",  0,2,    10,0,0,     2,0,0,      7,0});
    CardInfoVector.push_back({"SPEAR-OWL",          5,0,    4,12,25,    0,0,0,      4,2}); // WHEN SUMMONED: 50% chance double ATK
    CardInfoVector.push_back({"MYSTIC",             4,0,    -1,0,0,     2,0,0,      5,2}); // 50% chance for evil? or maybe: AFTER FIGHT: Random owl goes on sale?
    CardInfoVector.push_back({"THUG",               7,0,    12,0,0,    -2,0,0,      0,2}); // -1money
    CardInfoVector.push_back({"ENERGY KNIGHT",      12,1,   25,0,0,     0,0,0,      3,2}); // +1 energy
    CardInfoVector.push_back({"ALCHEMIST",          6,0,    0,0,0,       4,0,25,      8,2}); // +3money if your atk is even? or maybe AFTER FIGHT: 3 owls cost 1 less
    CardInfoVector.push_back({"MERCHANT",           10,0,   0,0,0,       5,0,0,      9,2}); // AFTER FIGHT: 3 random owls cost 1 less
    CardInfoVector.push_back({"GOBLIN",             0,0,   0,0,0,       0,0,0,      10,0}); // 10 uh the gobbo has a unique mechanic. maybe i should put it off til tomorrow.

    //todo: well obviously i don't need the repeat info
    UpgradedCardInfoVector.push_back({"MAGE",               3,0,    0,0,0,      1,2,50,      1,1}); // stuff to add: int attackone int attackonepercentage int attacktwo int attacktwopercentage int 
    UpgradedCardInfoVector.push_back({"ARCHER",             4,0,    4,0,0,     0,0,0,      2,1});
    UpgradedCardInfoVector.push_back({"ENERGY SURGE",       0,1,    5,0,0,      1,0,0,      6,0});
    UpgradedCardInfoVector.push_back({"MEGA ENERGY SURGE",  0,2,    10,0,0,     2,0,0,      7,0});
    UpgradedCardInfoVector.push_back({"SPEAR-OWL",          5,0,    4,12,50,    0,0,0,      4,2}); // WHEN SUMMONED: 50% chance double ATK
    UpgradedCardInfoVector.push_back({"MYSTIC",             4,0,    -2,0,0,     5,0,0,      5,2}); // 50% chance for evil? or maybe: AFTER FIGHT: Random owl goes on sale?
    UpgradedCardInfoVector.push_back({"THUG",               7,0,    18,0,0,    -3,0,0,      0,2}); // -1money
    UpgradedCardInfoVector.push_back({"ENERGY KNIGHT",      12,1,   35,0,0,     0,0,0,      3,2}); // +1 energy
    UpgradedCardInfoVector.push_back({"ALCHEMIST",          6,0,    0,0,0,       8,0,35,      8,2}); // +3money if your atk is even? or maybe AFTER FIGHT: 3 owls cost 1 less
    UpgradedCardInfoVector.push_back({"MERCHANT",           10,0,   0,0,0,       9,0,0,      9,2}); // AFTER FIGHT: 3 random owls cost 1 less
    //                         name,    cost, static, attack,   gather, tileindex, rarity

//builder owl
    // mage-powered golem If you have at least 4 mages in play, +20ATK
    // On purchase: -1HP

    //Generate the deck of mercs to draw from.
    for(int i =0; i< CardInfoVector.size(); i++)
    {
        if(CardInfoVector.at(i).rarity == 2)
        {
            AllUncommonMercs.push_back(i);
        }
        if(CardInfoVector.at(i).rarity==2 || CardInfoVector.at(i).rarity==1)
        {
            AllCommonAndUncommonMercs.push_back(i);
        }
    }

    for(int i = 0; i < 8; i++)
    {
        DeckOfOwlUpgrades.push_back(i);
        DeckOfNonOwlUpgrades.push_back(i);
    }
    //CardInfoTempDeckWithoutReplacement

    //make starting deck
    player1deck.push_back(2);
    player1deck.push_back(2);
    player1deck.push_back(3);
    player1deck.push_back(3);
    player1deck.push_back(0);
    player1deck.push_back(0);
    player1deck.push_back(1);
    player1deck.push_back(1);

    player1deck_after_miasma = player1deck;

    _selection_cursor_sprite.set_visible(false);
    _selection_cursor_sprite.set_z_order(-100);
    _selection_cursor_sprite.set_bg_priority(0); //lower z order means it shows up higher. wacky huh?

    
    _right_book_arrow_sprite.set_visible(false);
    _right_book_arrow_sprite.set_z_order(-98);
    _right_book_arrow_sprite.set_bg_priority(0); //lower z order means it shows up higher. wacky huh?

    _left_book_arrow_sprite.set_visible(false);
    _left_book_arrow_sprite.set_z_order(-98);
    _left_book_arrow_sprite.set_bg_priority(0); //lower z order means it shows up higher. wacky huh?

    _enemy_sprite.set_visible(false);

    _spellbook_bg.set_z_order(-90);
    _spellbook_bg.set_priority(0);
    _spellbook_bg.set_visible(false);

    
    _rift_bg.set_z_order(-80);
    _rift_bg.set_priority(1);
    _rift_bg.set_visible(false);

    //_ocean_bg.set_y(20);

    
   // _swamp_bg.set_visible(false);
}

int game_scene::run_scene()
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


                
                //_display_status(_generate_first_upgrade_description_from_upgrade_index(6),_generate_second_upgrade_description_from_upgrade_index(6));
                for(int mercstoadd=0; mercstoadd<MERCS_FOR_SALE; mercstoadd++)
                {
                    //int card_to_add=0;
                    //MercenaryDeck.push_back(0);
                    bn::sprite_ptr NewTableauImg = bn::sprite_items::knight_owls.create_sprite(last_merc_tableau_x_pos, -35);
                    NewTableauImg.set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(0));
                    MercenaryTableau.push_back(NewTableauImg);
                    MercenaryTableau.at(mercstoadd).set_visible(false);
                    MercenaryTableau.at(mercstoadd).set_z_order(-75);
                    MercenaryTableau.at(mercstoadd).set_bg_priority(1); //TODO: Plan out this better

                    bn::sprite_ptr NewerTableauImg = bn::sprite_items::salearrows.create_sprite(last_merc_tableau_x_pos-1, -27);
                    NewerTableauImg.set_tiles(bn::sprite_items::salearrows.tiles_item().create_tiles(0));
                    SaleArrowsTableau.push_back(NewerTableauImg);
                    SaleArrowsTableau.at(mercstoadd).set_visible(false);
                    SaleArrowsTableau.at(mercstoadd).set_z_order(-76);
                    SaleArrowsTableau.at(mercstoadd).set_bg_priority(1);
                    
                    last_merc_tableau_x_pos+=20;
                }
                int topleftx = -79;
                int toplefty = -50;
                //int currentx = topleftx;
                //int currenty = 
                //should probably be an array of vectors, or something. oh well
                for(int spellbookrow=0; spellbookrow<NUMBER_SPELLBOOK_ROWS; spellbookrow++)
                {
                    //int card_to_add=0;
                    //MercenaryDeck.push_back(0);
                    
                    for(int spellbookcolumn=0; spellbookcolumn<NUMBER_SPELLBOOK_COLUMNS; spellbookcolumn++)
                    {
                        bn::sprite_ptr NewTableauImg = bn::sprite_items::knight_owls.create_sprite(topleftx + spellbookcolumn*32, toplefty + spellbookrow*34);
                        NewTableauImg.set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(0)); //TODO: uh delete
                        NewTableauImg.set_z_order(-95);
                        NewTableauImg.set_bg_priority(0);
                        NewTableauImg.set_visible(false);
                        SpellbookTableau.push_back(NewTableauImg);
                        //last_merc_tableau_x_pos+=20;
                    }
                }
                IsUpgradeResearched.fill(false);
                //THIS IS WHERE YOU ADD NEW UPGRADES FOR TESTING!
                
                /*_research_upgrade(7);
                _research_upgrade(8);
                _research_upgrade(15);
                _research_upgrade(14);
                _research_upgrade(10);*/
                //_research_upgrade(7);
                _update_hud_text();
                _update_wave_info_hud_text();
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
//Only your flying units count for flying enemies!... nah... maybe
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


//TODO: Add "next" text and 

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
                
                if(WaveInfoVector.at(current_wave).attack == -1) //SHIPWRECK
                {
                    _enemy_sprite.set_tiles(bn::sprite_items::enemies.tiles_item().create_tiles(EnemyInfoVector.at(9).tileindex)); //shipwreck is 9, miasma is 10
                    
                    _enemy_sprite.set_visible(true);
                    _display_status("SHIPWRECK APPEARS.", "CHOOSE A BANNER TO SALVAGE.", "a:CONTINUE");
                    state = 26;
                }
                else if(WaveInfoVector.at(current_wave).attack == -2)
                {
                    _enemy_sprite.set_tiles(bn::sprite_items::enemies.tiles_item().create_tiles(EnemyInfoVector.at(10).tileindex)); //shipwreck is 9, miasma is 10

                    _enemy_sprite.set_visible(true);
                    _display_status("A CLOUD OF NOXIOUS MIASMA APPEARS!", "YOU ARE UNABLE TO ESCAPE.", "a:CONTINUE");
                    state = 33;
                }
                else //ENEMY & BOSS
                {
                    bn::string<50> display_text_line_one("");
                    bn::string<50> display_text_line_two("kATTACK = ");
                    display_text_line_one.append(bn::to_string<27>(EnemyInfoVector.at(WaveInfoVector.at(current_wave).enemy_index).name));
                    display_text_line_one.append(bn::to_string<27>(" APPEARS!"));
                    display_text_line_two.append(bn::to_string<5>(WaveInfoVector.at(current_wave).attack));
                    _display_status(display_text_line_one, display_text_line_two, "a:CONTINUE");

                    _enemy_sprite.set_tiles(bn::sprite_items::enemies.tiles_item().create_tiles(EnemyInfoVector.at(WaveInfoVector.at(current_wave).enemy_index).tileindex));
                    //_enemy_sprite.set_visible(true);
                    state_before_summon_start=900;
                    enemy_stat_box_active=true;
                    _update_enemy_stat_box();
                    state = 1;
                }
                _update_hud_text();
                _update_wave_info_hud_text();
                break;
            }
            case 33: // Loop; wait for A press to loop through miasma
            {
                if(bn::keypad::a_pressed())
                {
                    current_runes = 0;
                    player1deck = player1deck_after_miasma;
                    _display_status("YOUR SPELLBOOK IS DRAINED OF", "POWER, AND YOUR c IS DEPLETED.", "a:CONTINUE");

                    state = 34;
                    _update_hud_text();
                }
                bn::core::update();
                break;
            }
            case 34: // Loop; wait for A press to loop through miasma
            {
                if(bn::keypad::a_pressed())
                {
                    _display_status("YOU SHAKE OFF THE HEADACHE,", "AND NOTICE YOU ARE IN A NEW PLACE.", "a:CONTINUE");
                    current_wave++;
                    if(_get_current_zone()==1)
                    {
                        //_ocean_bg.delete();
                        _ocean_bg.set_y(ZONE_TWO_BG_Y);
                        //bn::core::update();
                        //_ocean_bg = bn::regular_bg_items::swampbackground.create_bg(0, 0);
                        //_ocean_bg.set_item(new bn::regular_bg_items::swampbackground.create_bg(0, 0));
                        //_swamp_bg.set_visible(true);
                    }
                    else
                    {
                        _ocean_bg.set_y(ZONE_THREE_BG_Y);
                        //_ocean_bg.set_visible(false);
                        //_swamp_bg.set_visible(false);
                    }
                    _enemy_sprite.set_visible(false);
                    _update_wave_info_hud_text();
                    state = 35;
                }
                bn::core::update();
                break;
            }
            case 35: // Loop; wait for A press to loop through miasma
            {
                if(bn::keypad::a_pressed())
                {
                   // _display_status("YOU FIND A MESSAGE IN A BOTTLE.", "DO YOU WANT TO READ IT?", "a:YES b:NO");
                    /*
                    if(_get_current_zone()==1)
                    {
                        _display_status("LIGHTS DANCE BETWEEN TREES.", "EYES GLINT IN THE DARK.", "a:CONTINUE");
                    }
                    else{ // zone = 2
                        _display_status("YOU ARE SURROUNDED BY STARS.", "COULD YOUR JOURNEY BE NEAR ITS END?", "a:CONTINUE");
                    }*/
                    state = 900;
                }
                bn::core::update();
                break;
            }
            case 36: // Loop; wait for A press to loop through miasma
            {
                if(bn::keypad::b_pressed())
                {
                    state = 900;
                }
                else if(bn::keypad::a_pressed())
                {
                    if(_get_current_zone()==1)
                    {
                        state = 37;
                    }
                    else{

                    }
                }
                bn::core::update();
                break;
            }
            case 26: // Loop; wait for A press to start shipwreck phase
            {
                if(bn::keypad::a_pressed())
                {
                    state = 27;
                }
                bn::core::update();
                break;
            }
            case 27: // intro to shipwreck phase
            {
                //_display_status("ud:MOVE, a:SELECT");
                //0-7 are owl upgrades, 8-15 are nonowl upgrades
                int upgrade_option_one_index = bn::abs(random_num) % DeckOfOwlUpgrades.size(); //5
                random_num = random_generator.get();
                
                upgrade_option_one = DeckOfOwlUpgrades.at(upgrade_option_one_index);
                DeckOfOwlUpgrades.erase(DeckOfOwlUpgrades.begin()+upgrade_option_one_index);


                int upgrade_option_two_index = bn::abs(random_num) % DeckOfNonOwlUpgrades.size();
                random_num = random_generator.get();
                
                upgrade_option_two = DeckOfNonOwlUpgrades.at(upgrade_option_two_index) + 8;
                DeckOfNonOwlUpgrades.erase(DeckOfNonOwlUpgrades.begin() + upgrade_option_two_index);

                //TODO: Check that whenever I interface with a vector or array of ints, I am interfacing with the contents of the array/vector, rather than the index


                _generate_virt_menu(2, _generate_name_from_upgrade_index(upgrade_option_one), _generate_name_from_upgrade_index(upgrade_option_two),"");
                //bn::core::update();
                state = 28;
                break;
            }
            //todo: check that randomness happens after every random number get
            case 28: // SHIPWRECK LOOP!
            {
                _navigate_through_virt_menu(); // todo : make this its own state
                if(menu_position==0)
                {
                    _display_status(_generate_first_upgrade_description_from_upgrade_index(upgrade_option_one), _generate_second_upgrade_description_from_upgrade_index(upgrade_option_one),"ud:MOVE, a:SELECT");
                }
                else if(menu_position==1)
                {
                    _display_status(_generate_first_upgrade_description_from_upgrade_index(upgrade_option_two), _generate_second_upgrade_description_from_upgrade_index(upgrade_option_two),"ud:MOVE, a:SELECT");
                }

                if(bn::keypad::a_pressed())
                {
                    
                    _enemy_sprite.set_visible(false);
                    bn::string<50> display_text_line_one("YOU PICKED ");
                    //_display_status(_generate_first_upgrade_description_from_upgrade_index(6),_generate_second_upgrade_description_from_upgrade_index(6));
                    if(menu_position==0)
                    {
                        _clear_virt_menu();
                        _research_upgrade(upgrade_option_one);
                        display_text_line_one.append(_generate_name_from_upgrade_index(upgrade_option_one));
                        display_text_line_one.append(".");
                        _display_status(display_text_line_one, "YOU SAIL ON.", "a:CONTINUE");
                        state = 29;
                    }
                    else if(menu_position==1)
                    {
                        _clear_virt_menu();
                        _research_upgrade(upgrade_option_two);
                        display_text_line_one.append(_generate_name_from_upgrade_index(upgrade_option_two));
                        display_text_line_one.append(".");
                        _display_status(display_text_line_one, "YOU SAIL ON.", "a:CONTINUE");
                        state = 29;
                    }
                    
                }
                bn::core::update();
                break;
            }
            case 29:
            {
                if(bn::keypad::a_pressed())
                {
                    current_wave+=1;
                    state = 900;
                }
                bn::core::update();
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
                AmountMercOnSale.fill(0);
                player_stat_box_active=true;
                state = 10002;
                break;
            }
            case 10002:
            {
                _generate_virt_menu(3, "SUMMON", "SPELLBOOK", "FIGHT!");
                if(state_before_summon_start==900) // came from game start or after shopping
                {
                    _display_status("NEW ROUND START! SUMMONING PHASE","ud:MOVE, a:SELECT");
                }
                else if(state_before_summon_start==22) // came from explosion
                {
                    _display_status("TRY, TRY AGAIN...","ud:MOVE, a:SELECT");
                }
                else if(state_before_summon_start==23) // came from spellbook
                {
                    _display_status("ud:MOVE, a:SELECT");
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
                            random_num = random_generator.get();

                            int weight_to_add = CardInfoVector.at(player1deck.at(index_to_remove)).weight;
                            int power_to_add = CardInfoVector.at(player1deck.at(index_to_remove)).powerone; //TOMCOMEBACK
                            int runes_to_add = CardInfoVector.at(player1deck.at(index_to_remove)).gatherone;

                            bn::string<50> first_line_status("SUMMONED ");
                            bn::string<50> second_line_status("");
                            bn::string<50> third_line_status("");
                            first_line_status.append(bn::to_string<18>(CardInfoVector.at(player1deck.at(index_to_remove)).name));
                            current_weight=current_weight+weight_to_add;
                            if(CardInfoVector.at(player1deck.at(index_to_remove)).weight==1)
                            {
                                first_line_status.append("!!!");
                            }
                            else if(CardInfoVector.at(player1deck.at(index_to_remove)).weight==2)
                            {
                                first_line_status.append("!!!!!!");
                            }
                            else
                            {
                                first_line_status.append(".");
                            }
                            
                            

                            if(player1deck.at(index_to_remove)==4)//4 is spear
                            {
                                int percentageroll = bn::abs(random_num) % 100; //0-99
                                random_num = random_generator.get();
                                if(percentageroll < CardInfoVector.at(player1deck.at(index_to_remove)).powertwopercentage)
                                {
                                    second_line_status.append("CRITICAL HIT! k+");
                                    second_line_status.append(bn::to_string<4>(CardInfoVector.at(player1deck.at(index_to_remove)).powertwo));
                                    current_power = current_power + CardInfoVector.at(player1deck.at(index_to_remove)).powertwo;
                                }
                                else{
                                    second_line_status.append("NO CRITICAL HIT THIS TIME. k+");
                                    second_line_status.append(bn::to_string<4>(CardInfoVector.at(player1deck.at(index_to_remove)).powerone));
                                    current_power = current_power + CardInfoVector.at(player1deck.at(index_to_remove)).powerone;
                                }
                            }
                            else if(player1deck.at(index_to_remove)==0 && IsUpgradeResearched[0]== true) // ugh this is awful. it's the mage, fyi
                            {
                                int percentageroll = bn::abs(random_num) % 100; //0-99
                                random_num = random_generator.get();
                                if(percentageroll < CardInfoVector.at(player1deck.at(index_to_remove)).gathertwopercentage)
                                {
                                    second_line_status.append("STROKE OF FORTUNE. c+");
                                    second_line_status.append(bn::to_string<4>(CardInfoVector.at(player1deck.at(index_to_remove)).gathertwo));
                                    runes_which_might_disappear = runes_which_might_disappear + CardInfoVector.at(player1deck.at(index_to_remove)).gathertwo;
                                }
                                else{
                                    second_line_status.append("STROKE OF MISFORTUNE. c+");
                                    second_line_status.append(bn::to_string<4>(CardInfoVector.at(player1deck.at(index_to_remove)).gatherone));
                                    runes_which_might_disappear = runes_which_might_disappear + CardInfoVector.at(player1deck.at(index_to_remove)).gatherone;
                                }
                            }
                            else if(player1deck.at(index_to_remove)==8) //8 is alchemist
                            {
                                int percentageroll = bn::abs(random_num) % 100; //0-99
                                random_num = random_generator.get();
                                if(percentageroll < CardInfoVector.at(player1deck.at(index_to_remove)).gathertwopercentage)
                                {
                                    second_line_status.append("HER EXPERIMENT FAILS! c+");
                                    second_line_status.append(bn::to_string<4>(CardInfoVector.at(player1deck.at(index_to_remove)).gathertwo));
                                    runes_which_might_disappear = runes_which_might_disappear + CardInfoVector.at(player1deck.at(index_to_remove)).gathertwo;
                                }
                                else{
                                    second_line_status.append("HER EXPERIMENT SUCCEEDS. c+");
                                    second_line_status.append(bn::to_string<4>(CardInfoVector.at(player1deck.at(index_to_remove)).gatherone));
                                    runes_which_might_disappear = runes_which_might_disappear + CardInfoVector.at(player1deck.at(index_to_remove)).gatherone;
                                }
                            }
                            else if(player1deck.at(index_to_remove)==9)//9 is merchant
                            {
                                //int percentageroll = bn::abs(random_num) % 100; //0-99
                                //random_num = random_generator.get();
                                runes_which_might_disappear = runes_which_might_disappear + CardInfoVector.at(player1deck.at(index_to_remove)).gatherone;
                                second_line_status.append("c+");
                                second_line_status.append(bn::to_string<4>(CardInfoVector.at(player1deck.at(index_to_remove)).gatherone));
                                second_line_status.append(". OWLS");
                                //second_line_status.append(bn::to_string<4>(AmountMercOnSale[0]));


                                SaleMercDeckToDrawFrom.clear();
                                for(int i =0; i< MERCS_FOR_SALE; i++)
                                {
                                    SaleMercDeckToDrawFrom.push_back(i);
                                }
                                for(int i =0; i<2; i++)
                                {
                                    int card_to_draw= bn::abs(random_num) % SaleMercDeckToDrawFrom.size();
                                    random_num = random_generator.get();
                                    second_line_status.append(" #");
                                    second_line_status.append(bn::to_string<4>(1+SaleMercDeckToDrawFrom.at(card_to_draw)));
                                    AmountMercOnSale[SaleMercDeckToDrawFrom.at(card_to_draw)] += 2;

                                    //Delete the drawn card from the deck
                                    SaleMercDeckToDrawFrom.erase(SaleMercDeckToDrawFrom.begin()+card_to_draw);
                                    //spin the random number generator! (TODO: experiment with not spinning it and instead just using the same number but modulating it down a bunch; would be faster)
                                    
                                }
                                second_line_status.append(" CHEAP THIS ROUND");
                                


                            }
                            else if(player1deck.at(index_to_remove)==10)//10 is goblin
                            {
                                int goblin_attack = 1 + (bn::abs(random_num) % 10);
                                random_num = random_generator.get();
                                current_power = current_power + goblin_attack;
                                //second_line_status.append("ROLLED ");
                                //second_line_status.append(bn::to_string<4>(goblin_attack));
                                //second_line_status.append(" ON THE D8. k+");
                                second_line_status.append("k+");
                                second_line_status.append(bn::to_string<4>(goblin_attack));
                            }
                            else
                            {
                                current_power=current_power+power_to_add;
                                runes_which_might_disappear=runes_which_might_disappear+runes_to_add;
                                
                                //_display_status(bn::string<40>("You drew a ").append(bn::to_string<2>(weight_to_add)));
                                //_display_status(bn::string<40>("You drew a ").append(CardInfoVector.at(player1deck.at(index_to_remove)).name);

                                second_line_status.append(_generate_description_from_owl_index(player1deck.at(index_to_remove),false));
                            }

                            if(IsUpgradeResearched[8])
                            {
                                bn::string<20> dtwenty_message(" D20=");
                                //dtwenty_message.append(" D20=");
                                int dtwentyroll = 1 + (bn::abs(random_num) % 20); //1-20
                                random_num = random_generator.get();
                                dtwenty_message.append(bn::to_string<4>(dtwentyroll));
                                if(dtwentyroll==20)
                                {
                                    current_power+=10;
                                    dtwenty_message.append("!!!! k+10");
                                }
                                if(player1deck.at(index_to_remove)==2 || player1deck.at(index_to_remove)==3 || player1deck.at(index_to_remove)==7) //If it's an energy surge, put it on the second line, since we don't have enough room otherwise.
                                {
                                    second_line_status.append(dtwenty_message);
                                }
                                else
                                {
                                    first_line_status.append(dtwenty_message);
                                }
                            }
                            _update_hud_text();

                            if(current_weight>max_boat_weight){
                                _selection_cursor_sprite.set_visible(false);
                                
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
                            bn::sprite_ptr NewTableauImg = bn::sprite_items::knight_owls.create_sprite(last_tableau_x_pos, last_tableau_y_pos);
                            NewTableauImg.set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(CardInfoVector.at(player1deck.at(index_to_remove)).tileindex));//player1deck.at(index_to_remove).tileindex));
                            Player1Tableau.push_back(NewTableauImg);//bn::sprite_items::knight_owls.create_sprite(last_tableau_x_pos, 0));
                            //Player1Tableau.at().set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(20));
                            last_tableau_x_pos+=5;
                            if(Player1Tableau.size()%15==0)
                            {
                                last_tableau_x_pos=-110;
                                last_tableau_y_pos+=4;
                            }

                            //Delete the drawn card from the deck
                            player1deck.erase(player1deck.begin()+index_to_remove);
                        }
                        else{
                            _display_status("ERROR: NO MORE ITEMS","THIS SHOULDN'T HAPPEN");
                        }
                    }
                    else if(menu_position==1)
                    {//show spellbook!
                        state_before_spellbook=3;
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
                    state_before_summon_start=22;
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
                    
                    AmountMercOnSale.fill(0);
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

                if(IsUpgradeResearched[15] && runes_which_might_disappear>=10)
                {
                    current_power+=6;
                    _display_status("SUMMONING OVER.","k+6 FOR EARNING AT LEAST 10c.","PRESS a TO RESOLVE COMBAT"); //moneybags
                }
                else{
                    _display_status("SUMMONING OVER.","PRESS a TO RESOLVE COMBAT");
                }
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
                        
                        _display_status("ENEMY'S kATTACK IS HIGHER. DEFEAT!","PRESS a TO CONTINUE");
                        won_wave=false;
                        state=6;
                    }

                    if(current_power > WaveInfoVector.at(current_wave).attack)
                    {
                        _display_status("YOUR kATTACK IS HIGHER. VICTORY!","PRESS a TO CONTINUE");
                        won_wave=true;
                        state=13;
                    }
                    
                    if(current_power == WaveInfoVector.at(current_wave).attack)
                    {
                        _display_status("YOUR kATTACK = ENEMY'S kATTACK.","VICTORY!","PRESS a TO CONTINUE");
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

                        if(runes_which_might_disappear<0)
                        {
                            runes_which_might_disappear=0;
                        }
                        
                        player_stat_box_active=false;


                        bn::string<50> second_line_status("YOUR OWLS GATHERED +");  
                        second_line_status.append(bn::to_string<8>(runes_which_might_disappear));
                        second_line_status.append(" cDOWLLARS.");
                        
                        current_runes += runes_which_might_disappear;
                        runes_which_might_disappear = 0;
                        if(won_wave==true)
                        {
                            
                            bn::string<50> first_line_status("REWARD: HEAL +");  
                            first_line_status.append(bn::to_string<4>(WaveInfoVector.at(current_wave).reward));
                            first_line_status.append(" mHP.");
                            current_hull = current_hull + WaveInfoVector.at(current_wave).reward;
                            if(current_hull>max_hull)
                            {
                                current_hull=max_hull;
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
                    
                    _display_status("YOU DIED! GAME OVER.","a:TRY AGAIN, b:RETURN TO MENU");
                    state = 25;
                }
                bn::core::update();
                break;
            }
            case 25: // Game over
            {
                if(bn::keypad::a_pressed())
                {
                    return(1); //next_scene is another game scene
                }
                else if(bn::keypad::b_pressed())
                {
                    return(0); //next_scene is menu
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
                    if(WaveInfoVector.at(current_wave).penalty == 9999)
                    {
                        current_wave++;
                        state = 900;
                    }
                    else
                        state = 20;
                }
                bn::core::update();
                break;
            }
            //case 900;
            case 20:
            {
                if(IsUpgradeResearched[12] == true)
                {
                    int first_aid_check= bn::abs(random_num) % 4;
                    random_num = random_generator.get();
                    if(first_aid_check != 0)
                    {
                        _display_status("YOU TRY TO APPLY FIRST AID...","...AND SUCCEED. +1mHP.","a:CONTINUE");
                        if(current_hull<max_hull)
                        {
                            current_hull++;
                            _update_hud_text();
                        }
                    }
                    else{
                        _display_status("YOU TRY TO APPLY FIRST AID...","...AND FAIL. DARN!","a:CONTINUE");
                    }
                    state = 31;
                    
                }
                
                else
                {
                    if(IsUpgradeResearched[13] == true && won_wave == false)
                    {
                        _apply_pity();
                        state = 32;
                    }
                    else
                        state = 30;
                }
                break;
            }
            case 31:
            {
                if(bn::keypad::a_pressed())
                    {
                        if(IsUpgradeResearched[13] == true && won_wave == false)
                        {
                            _apply_pity();
                            state = 32;
                        }
                        else
                            state = 30;
                    }
                bn::core::update();
                break;
            }
            case 32:
            {
                if(bn::keypad::a_pressed())
                {
                    state = 30;
                }
                bn::core::update();
                break;
            }
            case 30://intro to mercs appear
            {
                
                //Populate the deck to draw from
                TempMercDeckToDrawFrom = AllUncommonMercs;

                //Choose all the mercs
                //There will always be a mage for sale
                //There will always be an archer for sale
                //Maybe 3 random owls without replacement? (can be another mage or archer)
                //then 1 random owl that can be anything (including a repeat of any of the first ones)
                MercenaryDeck.clear();
                MercenaryDeck.push_back(0);//mage
                MercenaryDeck.push_back(1);//archer

                
                //Populate elements 2,3,4. Random owls with replacement from AllDrawableMercs
                for(int MercDeckIndex = 2; MercDeckIndex < 6; MercDeckIndex++)
                {
                    int card_to_draw= bn::abs(random_num) % TempMercDeckToDrawFrom.size();
                    random_num = random_generator.get();
                    MercenaryDeck.push_back(TempMercDeckToDrawFrom.at(card_to_draw));

                    //Delete the drawn card from the deck
                    TempMercDeckToDrawFrom.erase(TempMercDeckToDrawFrom.begin()+card_to_draw);
                    //spin the random number generator! (TODO: experiment with not spinning it and instead just using the same number but modulating it down a bunch; would be faster)
                    
                }
                /*
                //Finally, draw 1 card that can be anything from AllDrawableMercs
                int card_to_draw = bn::abs(random_num) % AllCommonAndUncommonMercs.size();
                MercenaryDeck.push_back(AllCommonAndUncommonMercs.at(card_to_draw));
                random_num = random_generator.get();
                //TODO: Check that the random num is randomized after every time a number is gotten
                //TODO: possible check to make sure that an attacking owl is generated for this last slot? Or nah; maybe just 4 uncommon...
                */

               
                _rift_bg.set_visible(true);
                if(IsUpgradeResearched[11])//stonks
                {
                    SaleMercDeckToDrawFrom.clear();
                    for(int i =0; i< MERCS_FOR_SALE; i++)
                    {
                        SaleMercDeckToDrawFrom.push_back(i);
                    }
                    int card_to_draw = 0;

                    bn::string<50> second_line_status("STONKS: OWL ");
/*
                    for (int i = 0; i < 3; i++)
                    {
                        card_to_draw = bn::abs(random_num) % SaleMercDeckToDrawFrom.size();
                        random_num = random_generator.get();
                        second_line_status.append("#");
                        second_line_status.append(bn::to_string<4>(1+SaleMercDeckToDrawFrom.at(card_to_draw)));
                        second_line_status.append(" ");
                        AmountMercOnSale[SaleMercDeckToDrawFrom.at(card_to_draw)] += 1;
                        SaleMercDeckToDrawFrom.erase(SaleMercDeckToDrawFrom.begin()+card_to_draw);
                    }

                    second_line_status.append("-1c, #");
                    card_to_draw= bn::abs(random_num) % SaleMercDeckToDrawFrom.size();
                    random_num = random_generator.get();
                    second_line_status.append(bn::to_string<4>(1+SaleMercDeckToDrawFrom.at(card_to_draw)));
                    AmountMercOnSale[SaleMercDeckToDrawFrom.at(card_to_draw)] -= 1;
                    second_line_status.append(" +1c");
*/
                    card_to_draw = bn::abs(random_num) % SaleMercDeckToDrawFrom.size();
                    random_num = random_generator.get();
                    second_line_status.append("#");
                    second_line_status.append(bn::to_string<4>(1+SaleMercDeckToDrawFrom.at(card_to_draw)));
                    second_line_status.append(" ");
                    AmountMercOnSale[SaleMercDeckToDrawFrom.at(card_to_draw)] += 2;
                    SaleMercDeckToDrawFrom.erase(SaleMercDeckToDrawFrom.begin()+card_to_draw);

                    second_line_status.append("-2c, #");
                    card_to_draw= bn::abs(random_num) % SaleMercDeckToDrawFrom.size();
                    random_num = random_generator.get();
                    second_line_status.append(bn::to_string<4>(1+SaleMercDeckToDrawFrom.at(card_to_draw)));
                    AmountMercOnSale[SaleMercDeckToDrawFrom.at(card_to_draw)] -= 2;
                    second_line_status.append(" +2c");

                    _display_status("A RIFT TO OWLHALLA APPEARS!", second_line_status, "a:CONTINUE");
                }
                else{
                    _display_status("A RIFT TO OWLHALLA APPEARS!", "YOU MAY CONSCRIPT NEW OWLS WITH c.", "a:CONTINUE");
                }
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
                    if(AmountMercOnSale[merctoillustrate] != 0)
                    {
                        SaleArrowsTableau.at(merctoillustrate).set_visible(true);
                        if(AmountMercOnSale[merctoillustrate]>0)
                        {
                            SaleArrowsTableau.at(merctoillustrate).set_tiles(bn::sprite_items::salearrows.tiles_item().create_tiles(0));
                        }
                        else
                        {
                            SaleArrowsTableau.at(merctoillustrate).set_tiles(bn::sprite_items::salearrows.tiles_item().create_tiles(1));
                        }
                    }
                }
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
                        
                        state_before_spellbook=11;
                        state=23;
                    }
                    else if(menu_position==2) // PASS
                    {
                        current_wave+=1;
                        _clear_virt_menu();
                        state=900;
                        //state_before_summon_start=
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
                //first_line_status.append(bn::to_string<8>(CardInfoVector.at(MercenaryDeck.at(menu_position)).cost));
                first_line_status.append(bn::to_string<8>(_price_on_sale(menu_position)));
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
                    //if(current_runes >= CardInfoVector.at(MercenaryDeck.at(menu_position)).cost)
                    if(current_runes >= _price_on_sale(menu_position))
                    {
                        bn::string<50> first_line_status("");
                        first_line_status.append(CardInfoVector.at(MercenaryDeck.at(menu_position)).name);
                        first_line_status.append(" ADDED TO SPELLBOOK"); // clear that sprite!
                        MercenaryTableau.at(menu_position).set_visible(false);
                        SaleArrowsTableau.at(menu_position).set_visible(false);
                        player1deck.push_back(MercenaryDeck.at(menu_position));
                        //current_runes -= CardInfoVector.at(MercenaryDeck.at(menu_position)).cost;
                        current_runes -= _price_on_sale(menu_position);
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
                        _display_status("NOT ENOUGH cDOWLLARS!","a:CONTINUE");
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
                //show the 
                state = 24;
                state_before_summon_start = 23;
                _spellbook_bg.set_visible(true);
                number_sb_pages=1+((player1deck.size()-1) / (NUMBER_SPELLBOOK_COLUMNS * NUMBER_SPELLBOOK_ROWS)); /// /18
                current_sb_page=0;
                current_sb_owl=0;
                //_selection_cursor_sprite.set_visible(true);
               
                _update_spellbook_from_menu_position();
                //bn::core::update();
                break;//Overloaded
            }
            case 24:{//spellbook!

                _navigate_through_spellbook();
                if(bn::keypad::b_pressed())
                {
                    for(int owltodisplay=0; owltodisplay < NUMBER_SPELLBOOK_COLUMNS * NUMBER_SPELLBOOK_ROWS; owltodisplay++)
                    {
                        SpellbookTableau.at(owltodisplay).set_visible(false);
                    }
                    _spellbook_bg.set_visible(false);
                    _right_book_arrow_sprite.set_visible(false);
                    _left_book_arrow_sprite.set_visible(false);
                    if(state_before_spellbook==3)
                        state=10002;
                    else if(state_before_spellbook==11)
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
        _enemy_sprite.set_visible(true);
        bn::string<16> first_enemy_stat_text("k");
        bn::string<16> second_enemy_stat_text("WIN:");
        bn::string<16> third_enemy_stat_text("LOSE:");

        first_enemy_stat_text.append(bn::to_string<4>(WaveInfoVector.at(current_wave).attack));

        if(WaveInfoVector.at(current_wave).reward==9999)
        {   
            second_enemy_stat_text.append("+MAXm");
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
        
        my_text_generator.generate(70, -47, first_enemy_stat_text, first_enemy_stat_text_sprites);
        my_text_generator.generate(70, -36, second_enemy_stat_text, second_enemy_stat_text_sprites);
        my_text_generator.generate(70, -25, third_enemy_stat_text, third_enemy_stat_text_sprites);
    }
    else{
        _enemy_sprite.set_visible(false);
    }
                
}


void game_scene::_update_hud_text()
{
    //TODO: make this just make & use 1 string rather than making a million. lol
    my_text_generator.set_left_alignment();
    //needed: enemy wave sprites, runes which might disappear sprite
    hull_text_sprites.clear();
    bn::string<20> hull_hud_text("");
    for(int hull_to_check = 0; hull_to_check < max_hull; hull_to_check++)
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
    

    power_text_sprites.clear();
    runes_that_might_disappear_text_sprites.clear();
    weight_text_sprites.clear();

    if(player_stat_box_active==true)
    {
        my_text_generator.set_center_alignment();

        bn::string<20> power_hud_text("k"); // If current power is negative, this will have a minus sign
        power_hud_text.append(bn::to_string<8>(current_power));
        my_text_generator.generate(-70, -47, power_hud_text, power_text_sprites);

        bn::string<20> runes_that_might_disappear_hud_text("c");
    
        if(runes_which_might_disappear>=0) // If runes which might disappear is negative, this will have a minus sign
        {
            runes_that_might_disappear_hud_text.append(bn::to_string<1>("+"));
        }
        runes_that_might_disappear_hud_text.append(bn::to_string<8>(runes_which_might_disappear));
        my_text_generator.generate(-70, -36, runes_that_might_disappear_hud_text, runes_that_might_disappear_text_sprites);

        //-39
        bn::string<20> weight_hud_text("i");
        weight_hud_text.append(bn::to_string<8>(current_weight));
        weight_hud_text.append("/");
        weight_hud_text.append(bn::to_string<2>(max_boat_weight));
        my_text_generator.generate(-70, -25, weight_hud_text, weight_text_sprites);
    }
    //-115 -39
    //-115 -50
    //-115 -61
    //-115 -72

}

void game_scene::_update_wave_info_hud_text()
{
//AFTER ACT 1 (SEA)
//YOU FIND A MESSAGE IN A BOTTLE, FLOATING IN THE WATER. DO YOU WISH TO READ IT?
// THUNDERING, THUNDERING... I USED TO FIND THE SOUND OF A STORM CALMING, BUT NO LONGER.
// RAIN PELTS OUR ROOF UNCEASINGLY, AND THE WIND IS LIKE A MIGHTY ROAR.
// WHAT HAS CAUSED THIS WRETCHED STORM? IS IT SOMEHOW PUNISHMENT FOR THE EVIL IN OUR HEARTS?
// I SUPPOSE IT DOESN'T MATTER. OF FAR GREATER IMPORTANCE, IS THE FACT THAT MY FAMILY HAS CONSTRUCTED
// A SAILING SHIP, AND SO WE MAY SURVIVE A LITTLE LONGER ONCE THE GROUND IS SWALLOWED UP BENEATH US BY THE RAIN.
// IF ANYONE READS THIS, KNOW THAT THERE IS NO STOPPING THE RAIN. IT WILL SPREAD, AND SPREAD, UNTIL ALL IS A WATERY GRAVEYARD.
// UNFORTUNATELY, IF YOU WISH TO KNOW WHERE TO GO NEXT, I HAVE NO ADVICE. WE ARE ALL REFUGEES NOW...

//AFTER ACT 2 (SWAMP)
//YOU FIND A MESSAGE IN A BOTTLE, FLOATING IN THE WATER. DO YOU WISH TO READ IT?
// WE HAVE BEEN WEEKS AT SEA IN OUR HASTILY-CONSTRUCTED VESSEL. THE BAD NEWS IS THAT WE ARE RUNNING LOW ON PROVISIONS.
// THE GOOD NEWS IS THAT MY YOUNGEST DAUGHTER HAS FOUND A SPELLBOOK. I DON'T QUITE UNDERSTAND
// HOW IT WORKS, BUT SOMEHOW, SHE IS ABLE TO USE IT TO SUMMON LITTLE OWLS WEARING MEDIEVAL WARRIOR OUTFITS.
// AN UNCONVENTIONAL FIGHTING TACTIC, TO BE SURE, BUT A WELCOME ONE.
// WE HAVE ENCOUNTERED SOME AWFULLY STRANGE MONSTERS ON OUR VOYAGE. A SORT OF PRIMORDIAL POWER FEELS LIKE IT'S BEEN LOOSED HERE, AND MAGIC CRACKLES AROUND US IN THE AIR.
// IN ANY CASE, WE HAVE SEARCHED THROUGHOUT THE WORLD, AND FOUND NO PLACE TO MAKE OUR NEW HOME.
// BUT WE HAVE ONE MORE HOPE. WE CAN SEARCH UPWARD...

//AFTER ACT 3 (SKY)
//AT LAST, WE ARE SAFE! MY RELIEF IS IMMEASURABLE.
//WHEN WE FIRST ARRIVED IN THE STARRY REALM ABOVE THE WORLD, IT APPEARED TO BE NO BETTER THAN THE WATERLOGGED PLANET WE HAD LEFT BEHIND.
//VICIOUS MONSTERS PROWL THE SKIES... I CAN ONLY BE THANKFUL THAT MY DAUGHTER'S SKILL WITH HER SPELLBOOK HAS GROWN, AND THAT HER OWLS ARE ON OUR SIDE.
//WE SEEMED TO BE GETTING NOWHERE, UNTIL WE ENCOUNTERED AN ECCENTRIC STRANGER NAMED HERMAN.
//HE INITIALLY DEMANDED WE FIGHT, SO THAT WE COULD "PROVE OURSELVES." AFTER WE DEFEATED HIM, HE TOLD US THE FOLLOWING:
//"HMMPH. ANOTHER "REFUGEE" FROM YOUR RAINY PLANET...
//...I'M AFRAID YOU CANNOT STAY HERE LONG. 
//THIS ISN'T YOUR HOME. YOU BELONG BACK ON EARTH.
//BUT I GUESS EARTH IS NO GOOD FOR YOU NOW, EH? AWFULLY WET DOWN THERE, ISN'T IT?
//FINE, YOU MAY STAY FOR NOW. BUT AS SOON AS THE WATER DRIES, YOU MUST GO BACK TO WHERE YOU CAME."
//AND THE WATER *WILL* DRY. I'M SURE OF IT. THESE THINGS ALWAYS COME IN CYCLES."
//I THANKED HIM PROFUSELY FOR HIS HOSPITALITY. IT APPEARS WE HAVE FOUND A SHELTER - FOR NOW.
//I AM NOT SO CERTAIN I AGREE WITH HIM THAT THERE IS HOPE THE EARTH WILL RECOVER... BUT WE SHALL SEE.
//PLEASE, IF YOU ARE READING THIS, SEEK US OUT IN THE SKIES. WE WILL HELP YOU.
    

    my_text_generator.set_right_alignment();
    //needed: enemy wave sprites, runes which might disappear sprite
    wave_text_sprites.clear();
    zone_text_sprites.clear();
    //wave_text_sprites_two.clear();
    //wave_text_sprites_three.clear();
    bn::string<20> wave_hud_text("");
    bn::string<10> zone_hud_text("");
    //int row_to_draw = 1;
    int first_wave_to_draw = 0;
    int last_wave_to_draw = 0;
    if(_get_current_zone() == 0)
    {
        zone_hud_text.append("SEA");
        first_wave_to_draw = 0;
        last_wave_to_draw = start_of_zone_two-1;
    }
    else if(_get_current_zone() == 1)
    {
        zone_hud_text.append("STORM");
        first_wave_to_draw = start_of_zone_two;
        last_wave_to_draw = start_of_zone_three-1;
    }
    else
    {
        zone_hud_text.append("SKY");
        first_wave_to_draw = start_of_zone_three;
        last_wave_to_draw = WaveInfoVector.size()-1;
    }

    for(int wave_to_check = first_wave_to_draw; wave_to_check <= last_wave_to_draw; wave_to_check++)//todo:separate into its own function
    {
        if(WaveInfoVector.at(wave_to_check).penalty == 9999) // BOSS
        {
            if(wave_to_check==current_wave)
                wave_hud_text.append("p");//red skull
            else
                wave_hud_text.append("o"); //white skull
        }
        else if(WaveInfoVector.at(wave_to_check).attack == -1) // SHIPWRECK
        {
            if(wave_to_check==current_wave)
                wave_hud_text.append("g");//red ship
            else
                wave_hud_text.append("f"); //white ship
        }
        /*else if(wave_to_check < current_wave)
        {
            wave_hud_text.append("y");//z is open but i like y better
        }*/
        else if(WaveInfoVector.at(wave_to_check).penalty == 1) // 1-POWER ENEMY
        {
            if(wave_to_check == current_wave)
            {
                wave_hud_text.append("t");//red 1
            }
            else
            {
                wave_hud_text.append("s");//white 1
            }
        }
        else if(WaveInfoVector.at(wave_to_check).penalty == 2)
        {                                                  // 2-POWER ENEMY
            if(wave_to_check == current_wave)
            {
                wave_hud_text.append("z");//red 2
            }
            else
            {
                wave_hud_text.append("y");//white 2
            }
        }
        else{                                                  // MIASMA (also go to next line)
            if(wave_to_check == current_wave)
            {
                wave_hud_text.append("j");//red miasma
            }
            else
            {
                wave_hud_text.append("q");//white miasma
            }   
        }
    }
    
    my_text_generator.generate(118, -72, wave_hud_text, wave_text_sprites);
    my_text_generator.generate(118, -63, zone_hud_text, zone_text_sprites);

    //my_text_generator.generate(118, -72, wave_hud_text, wave_text_sprites);

    
}

void game_scene::_display_status(const bn::string<50>& statustextone, const bn::string<50>& statustexttwo, const bn::string<50>& statustextthree)
{
//todo for this:
//clear button sprites
//after text is displayed, trawl through the strings and put a lil button under udlrba



    my_text_generator.set_center_alignment();
    status_text_one_sprites.clear();
    status_text_two_sprites.clear();
    status_text_three_sprites.clear();
    //button_sprites.clear();
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
    //put on the little circle things
    /*for(int i = 0; i <status_text_three_sprites.size(); i++)
    {
        if(statustextthree.at(i) == 'u' || statustextthree.at(i) == 'd' || statustextthree.at(i) == 'l' || statustextthree.at(i) == 'r' || statustextthree.at(i) == 'a' || statustextthree.at(i) == 'b')
        {
            bn::sprite_ptr NewTableauImg = bn::sprite_items::button.create_sprite(status_text_three_sprites.at(i).position());
            //bn::sprite_ptr NewTableauImg = bn::sprite_items::button.create_sprite(0,0);
            
            button_sprites.push_back(NewTableauImg);
        }
    }*/
    //Eh, I tried to get the button image working, but it doesn't work. Darn. Oh well. Darn.
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
    last_tableau_y_pos = 10;
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


bn::string<50> game_scene::_generate_description_from_owl_index(int card_info_index, bool upgraded)
{
    int weight_to_add = 0;
    int powerone_to_add = 0;
    int powertwo_to_add = 0;
    int powertwopercentage = 0;
    int runesone_to_add = 0;
    int runestwo_to_add = 0;
    int runestwopercentage = 0;
    if(upgraded == true)
    {
        weight_to_add = UpgradedCardInfoVector.at(card_info_index).weight;
        powerone_to_add = UpgradedCardInfoVector.at(card_info_index).powerone;
        powertwo_to_add = UpgradedCardInfoVector.at(card_info_index).powertwo;
        powertwopercentage = UpgradedCardInfoVector.at(card_info_index).powertwopercentage;
        runesone_to_add = UpgradedCardInfoVector.at(card_info_index).gatherone;
        runestwo_to_add = UpgradedCardInfoVector.at(card_info_index).gathertwo;
        runestwopercentage = UpgradedCardInfoVector.at(card_info_index).gathertwopercentage;
    }
    else{
        weight_to_add = CardInfoVector.at(card_info_index).weight;
        powerone_to_add = CardInfoVector.at(card_info_index).powerone;
        powertwo_to_add = CardInfoVector.at(card_info_index).powertwo;
        powertwopercentage = CardInfoVector.at(card_info_index).powertwopercentage;
        runesone_to_add = CardInfoVector.at(card_info_index).gatherone;
        runestwo_to_add = CardInfoVector.at(card_info_index).gathertwo;
        runestwopercentage = CardInfoVector.at(card_info_index).gathertwopercentage;

    }
    bn::string<50> _description_string("");
    if(card_info_index==9)//merchant
    {
        
        
        _description_string.append("c+");
        _description_string.append(bn::to_string<5>(runesone_to_add));
        _description_string.append(", AND PUT 2 OWLS ON SALE (-2c)");
    }
    else if(card_info_index==10)//goblin
    {
        _description_string.append("k+D10");
    }
    else{
        //TODO: Uh right now the nonzero value must be powerone. Maybe uh make it more flexible
        //ATTACK
        if(powertwopercentage!=0)
        {
            //todo: uh, make htis flexible enough that it can handle both negative values and percentages?
            
            _description_string.append("k+");
            _description_string.append(bn::to_string<5>(powerone_to_add));
            _description_string.append(" (");
            _description_string.append(bn::to_string<5>(100-powertwopercentage));
            _description_string.append("% CHANCE), k+");
            _description_string.append(bn::to_string<5>(powertwo_to_add));
            _description_string.append(" (");
            _description_string.append(bn::to_string<5>(powertwopercentage));
            _description_string.append("% CHANCE)");
            /*_description_string.append(bn::to_string<5>(100-powertwopercentage));
            _description_string.append("% CHANCE: k+");
            _description_string.append(bn::to_string<5>(powerone_to_add));
            _description_string.append(", ");
            _description_string.append(bn::to_string<5>(powertwopercentage));
            _description_string.append("% CHANCE: k+");
            _description_string.append(bn::to_string<5>(powertwo_to_add));*/
        }
        else
        {
            //ADD A PLUS SIGN IF VALUE > 0, KEEP THE MINUS SIGN IF VALUE < 0, DO NOT SHOW IF VALUE == 0
            if(powerone_to_add>0)
            {
                _description_string.append("k+");
                _description_string.append(bn::to_string<5>(powerone_to_add));
            }
            else if(powerone_to_add<0)
            {
                _description_string.append("k");
                _description_string.append(bn::to_string<5>(powerone_to_add));
            }

        }

        if(powerone_to_add!=0 && (runesone_to_add!=0 || weight_to_add>0))
        {
            _description_string.append(" ");
        }

        //GATHER
        if(runestwopercentage!=0)
        {
            //todo: uh, make htis flexible enough that it can handle both negative values and percentages?
            _description_string.append("c+");
            _description_string.append(bn::to_string<5>(runesone_to_add));
            _description_string.append(" (");
            _description_string.append(bn::to_string<5>(100-runestwopercentage));
            _description_string.append("% CHANCE), c+");
            _description_string.append(bn::to_string<5>(runestwo_to_add));
            _description_string.append(" (");
            _description_string.append(bn::to_string<5>(runestwopercentage));
            _description_string.append("% CHANCE)");
        }
        else
        {
            if(runesone_to_add>0)
            {
                _description_string.append("c+");
                _description_string.append(bn::to_string<5>(runesone_to_add));
            }
            else if(runesone_to_add<0)
            {
                _description_string.append("c");
                _description_string.append(bn::to_string<5>(runesone_to_add));
            }
        }

        if(runesone_to_add!=0 && weight_to_add>0)
        {
            _description_string.append(" ");
        }

        //STATIC IS A LITTLE SIMPLER BECAUSE NOTHING DECREASES STATIC & THERE ARE NO PERCENTAGE OPTIONS
        if(weight_to_add>0)
        {
            _description_string.append("i+");
            _description_string.append(bn::to_string<5>(weight_to_add));
        }
    }
    return(_description_string);
}


void game_scene::_generate_virt_menu(int num_options, const bn::string<20>& menu_option_one, const bn::string<20>& menu_option_two, const bn::string<20>& menu_option_three)
{
    my_text_generator.set_center_alignment();

    first_menu_option_text_sprites.clear();
    second_menu_option_text_sprites.clear();
    third_menu_option_text_sprites.clear();
    my_text_generator.generate(0, -5, menu_option_one, first_menu_option_text_sprites);
    my_text_generator.generate(0, 6, menu_option_two, second_menu_option_text_sprites);
    my_text_generator.generate(0, 16, menu_option_three, third_menu_option_text_sprites);
    menu_position_max = num_options -1;
    menu_position = 0;
    _selection_cursor_sprite.set_visible(true);
    _update_selection_cursor_from_virt_menu_position();

}


void game_scene::_update_spellbook_from_menu_position()
{
    //RECALCULATE sb_page & redraw owls
    current_sb_page = current_sb_owl / (NUMBER_SPELLBOOK_COLUMNS * NUMBER_SPELLBOOK_ROWS);
    for(int owltodisplay=current_sb_page*NUMBER_SPELLBOOK_COLUMNS*NUMBER_SPELLBOOK_ROWS ; owltodisplay < (current_sb_page+1)*NUMBER_SPELLBOOK_COLUMNS*NUMBER_SPELLBOOK_ROWS; owltodisplay++)
    {
        if(owltodisplay < player1deck.size())
        {
            SpellbookTableau.at(_position_on_page(owltodisplay)).set_tiles(bn::sprite_items::knight_owls.tiles_item().create_tiles(CardInfoVector.at(player1deck.at(owltodisplay)).tileindex));
            SpellbookTableau.at(_position_on_page(owltodisplay)).set_visible(true);
        }
        else{
            SpellbookTableau.at(_position_on_page(owltodisplay)).set_visible(false);
        }
    }
    if(current_sb_page !=0)
    {
        _left_book_arrow_sprite.set_visible(true);
    }
    else{
        _left_book_arrow_sprite.set_visible(false);
    }
    if(current_sb_page != number_sb_pages-1)
    {
        _right_book_arrow_sprite.set_visible(true);
    }
    else
    {
        _right_book_arrow_sprite.set_visible(false);
    }
    _point_cursor_at_owl(SpellbookTableau.at(_position_on_page(current_sb_owl)));

    bn::string<50> first_line_status("");
    bn::string<50> second_line_status("");
    //bn::string<50> third_line_status("ludr:MOVE, b:RETURN");
    first_line_status.append(bn::to_string<18>(CardInfoVector.at(player1deck.at(current_sb_owl)).name));
    //first_line_status.append(" (COST: ");
    //first_line_status.append(bn::to_string<5>(CardInfoVector.at(MercenaryDeck.at(menu_position)).cost));
    //first_line_status.append("c)");
    second_line_status.append(_generate_description_from_owl_index(player1deck.at(current_sb_owl),false));
    _display_status(first_line_status, second_line_status, "ludr:MOVE, b:RETURN");
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

int game_scene::_position_on_page(int absolute_owl)
{
    return absolute_owl-current_sb_page*NUMBER_SPELLBOOK_COLUMNS*NUMBER_SPELLBOOK_ROWS;
}

int game_scene::_price_on_sale(int merc_deck_index)
{
    int sale_price = CardInfoVector.at(MercenaryDeck.at(menu_position)).cost - AmountMercOnSale[merc_deck_index];
    if(sale_price<0)
    {
        return 0;
    }
    else
    {
        return sale_price;
    }
}

void game_scene::_navigate_through_spellbook()
{
    if(bn::keypad::up_pressed())
    {
        if(_position_on_page(current_sb_owl) - NUMBER_SPELLBOOK_COLUMNS >= 0)
        {
            current_sb_owl = current_sb_owl - NUMBER_SPELLBOOK_COLUMNS;
        }
        _update_spellbook_from_menu_position();
    }
    if(bn::keypad::down_pressed())
    {
        if((_position_on_page(current_sb_owl)+NUMBER_SPELLBOOK_COLUMNS < NUMBER_SPELLBOOK_COLUMNS*NUMBER_SPELLBOOK_ROWS) && (current_sb_owl+NUMBER_SPELLBOOK_COLUMNS < player1deck.size()))
        {
            current_sb_owl = current_sb_owl + NUMBER_SPELLBOOK_COLUMNS;
        }
        _update_spellbook_from_menu_position();
    }
    if(bn::keypad::left_pressed())
    {
        if(_position_on_page(current_sb_owl)%NUMBER_SPELLBOOK_COLUMNS==0) //if at far right
        {
            if(current_sb_page!=0)
            {
                current_sb_owl=current_sb_owl-13;//todo: make this a formula
            }
        }
        else
        {
            current_sb_owl=current_sb_owl-1;
        }
        _update_spellbook_from_menu_position();
    }
    if(bn::keypad::right_pressed())
    {
        if(_position_on_page(current_sb_owl)%NUMBER_SPELLBOOK_COLUMNS==NUMBER_SPELLBOOK_COLUMNS-1) //if at far right
        {
            if(current_sb_page!=number_sb_pages-1)
            {
                //Go to next page on the right - but to figure out what row, we gotta do something stupid and complicated
                if(current_sb_owl+13 < player1deck.size())
                {
                    current_sb_owl=current_sb_owl+13;
                }
                else if(current_sb_owl+7 < player1deck.size())
                {
                    current_sb_owl=current_sb_owl+7;
                }
                else{
                    current_sb_owl=current_sb_owl+1;
                }
            }
        }
        else if(current_sb_owl+1 < player1deck.size())
        {
            current_sb_owl=current_sb_owl+1;
        }
        _update_spellbook_from_menu_position();
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
    if(CardInfoVector.at(MercenaryDeck.at(menu_position)).rarity == 1)
    {
        first_line_status.append(" (COMMON, ");
    }
    else
    {
        first_line_status.append(" (UNCOMMON, ");
    }
    first_line_status.append("COSTS ");
    //first_line_status.append(bn::to_string<5>(CardInfoVector.at(MercenaryDeck.at(menu_position)).cost));
    first_line_status.append(bn::to_string<5>(_price_on_sale(menu_position)));
    first_line_status.append("c)");
    second_line_status.append(_generate_description_from_owl_index(MercenaryDeck.at(menu_position),false));
    
    _display_status(first_line_status,second_line_status,third_line_status);
    _point_cursor_at_owl(MercenaryTableau.at(menu_position));
}

void game_scene::_clear_virt_menu() //TODO: Uh, maybe this should be called clear hor menu?
{
    _selection_cursor_sprite.set_visible(false);
    first_menu_option_text_sprites.clear();
    second_menu_option_text_sprites.clear();
    third_menu_option_text_sprites.clear();
    for(int i = 0; i<MercenaryDeck.size(); i++)
    {
        MercenaryTableau.at(i).set_visible(false);
        SaleArrowsTableau.at(i).set_visible(false);
    }
    _rift_bg.set_visible(false);
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

void game_scene::_research_upgrade(int whichupgrade)
{
    IsUpgradeResearched[whichupgrade] = true;

    bn::sprite_ptr NewTableauImg = bn::sprite_items::banners.create_sprite(last_banner_tableau_x_pos, 37);
    NewTableauImg.set_tiles(bn::sprite_items::banners.tiles_item().create_tiles(whichupgrade));
    last_banner_tableau_x_pos+=12;
    BannerTableau.push_back(NewTableauImg);

    if(whichupgrade<8)//owl upgrade
    {
        //my god this is hacky lmao
        if(whichupgrade>1)//adding two if not mage or archer, because reasons
        {
            whichupgrade += 2;
        }
        CardInfoVector.at(whichupgrade) = UpgradedCardInfoVector.at(whichupgrade);
    }
    //upgrad 8 is d20, and is not handled by this function
    else if(whichupgrade == 9) //COURAGE
    {
        max_hull -= 2;
        max_boat_weight += 1;
        if(current_hull>max_hull)
        {
            current_hull=max_hull;
        }
    }
    else if(whichupgrade == 10) //strength
    {
        max_hull += 2;
        current_hull += 2;
    }
    else if(whichupgrade == 14) //GOBLIN
    {
        player1deck.push_back(10);
        player1deck_after_miasma.push_back(10);
        //No need to push back to player1deck_at_start_of_round because upgrades can't be researched during combat
    }
    
    _update_hud_text();
    /*
    NonOwlUpgradeInfoVector.push_back({"D20","ALL SPELLS HAVE 5% CHANCE FOR","+10k."}); //0   8
    NonOwlUpgradeInfoVector.push_back({"COURAGE","+1 MAX iSTATIC, BUT -1 MAX mHP.",""}); //1   9
    NonOwlUpgradeInfoVector.push_back({"STRENGTH","+2 MAX AND CURRENT mHP.",""}); //2   10
    NonOwlUpgradeInfoVector.push_back({"COUPON","ONE OWL GOES ON SALE EVERY","ROUND."}); //3   11
    NonOwlUpgradeInfoVector.push_back({"FIRST AID","AFTER FIGHT, 33.33% CHANCE TO","HEAL 1 mHP."}); //4   12
    NonOwlUpgradeInfoVector.push_back({"PITY","AFTER YOU LOSE A FIGHT, GAIN","+2c."}); //5   13
    NonOwlUpgradeInfoVector.push_back({"GOBLIN","ADD A PERMANENT GOBLIN TO","YOUR SPELLBOOK. (GOBLIN HAS +D8k)"}); //6   14
    NonOwlUpgradeInfoVector.push_back({"MONEYBAGS","WHEN YOU FIGHT, IF YOUR +c THIS","ROUND WAS 8 OR HIGHER, GAIN +6k"}); //7   15
    */
}

bn::string<50> game_scene::_generate_first_upgrade_description_from_upgrade_index(int isupgraded_index)
{
    bn::string<50> _description_string("");
    if(isupgraded_index <8)
    {
        //my god this is hacky lmao
        if(isupgraded_index>1)//adding two if not mage or archer, because reasons
        {
            isupgraded_index += 2;
        }
        _description_string.append("ALL ");
        _description_string.append(CardInfoVector.at(isupgraded_index).name);
        _description_string.append("S WILL BE:");
    }
    else
    {
        _description_string.append(NonOwlUpgradeInfoVector.at(isupgraded_index-8).description_one);
    }
    return(_description_string);
}

bn::string<50> game_scene::_generate_second_upgrade_description_from_upgrade_index(int isupgraded_index)
{
    bn::string<50> _description_string("");
    if(isupgraded_index <8)
    {
        //my god this is hacky lmao
        if(isupgraded_index>1)//adding two if not mage or archer, because reasons
        {
            isupgraded_index += 2;
        }
        _description_string.append(_generate_description_from_owl_index(isupgraded_index,true));
    }
    else
    {
        _description_string.append(NonOwlUpgradeInfoVector.at(isupgraded_index-8).description_two);
    }
    return(_description_string);
}

bn::string<25> game_scene::_generate_name_from_upgrade_index(int isupgraded_index)
{
    bn::string<25> _name_string("");
    if(isupgraded_index <8)
    {
        //my god this is hacky lmao
        if(isupgraded_index>1)//adding two if not mage or archer, because reasons
        {
            isupgraded_index += 2;
        }
        _name_string.append(CardInfoVector.at(isupgraded_index).name);
    }
    else
    {
        _name_string.append(NonOwlUpgradeInfoVector.at(isupgraded_index-8).name_minus_banner);
    }
    //_name_string.append(" +");
    _name_string.append(" BANNER");
    return(_name_string);
}

void game_scene::_apply_pity()
{
    _display_status("YOU LOST COMBAT, SO YOU GET","+4c THANKS TO PITY BANNER.","a:CONTINUE");
    current_runes+=4;
    _update_hud_text();
}

int game_scene::_get_current_zone()
{
    if(current_wave < start_of_zone_two)
    {
        return 0;
    }
    else if(current_wave < start_of_zone_three)
    {
        return 1;
    }
    else return 2;
}
//bn::string<50> _generate_description_from_owl_index(int card_info_index);

//post-jam todo list
// figure out how much ewrom, iwrom, etc. i'm using
// separate "card ability" into its own class, with a function _generate_description()
// figure out a better way to handle state transitions... have the state entrypoint be in the same case as the state loop
// replace all my dumb numbers (states, owl index, etc.) with enums
// whatever quality of life features people complain most that they aren't present
// replace many vectors with arrays
// rename text to reflect my new terminology (runes -> dust, weight -> static, etc.)
// figure out if the idea is good enough to keep working on
// fricking take a break, man

//ACTUAL post-jam todo list
// ability to view banners at any time
// ability to view spellbook when selecting banner (CHOOSE ENERGY KNIGHT BANNER - CHOOSE STRENGTH BANNER - BANNERS - SPELLBOOK)
// zone 2 and 3 backgrounds more interesting
// balance. (maybe replace some content like pity banner)
// pause menu - 
//      MUTE MUSIC - UNMUTE MUSIC
// SUMMONED ENERGY KNIGHT II - ENERGY KNIGHT III - LEVELS UP ALL ENERGY KNIGHTS UNTIL MIASMA.
//      INCREASE MUSIC VOLUME
//      DECREASE MUSIC VOLUME
//      SAVE AND RETURN TO MAIN MENU
//      RESUME GAME
// high score (which is saved)

// finish drawing enemies
// ability to save game!!
// for sale... one "rare" slot which can contain temporary upgrades, eggs, vamp owl, mage nest perhaps? and also one "procgen" slot
/*
small tasks, needed for the game to be complete

-button for movement descriptions
-main menu =

-lol autochess
*/