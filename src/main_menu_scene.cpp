
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
#include "bn_sprite_items_enemies.h"
#include "bn_regular_bg_items_main_menu_background.h"
//#include "bn_regular_bg_items_spellbook.h"
//#include "bn_regular_bg_items_rift.h"

#include "main_menu_scene.h"

#include "bn_music_items.h"
#include "bn_music_actions.h"
#include "variable_8x8_sprite_font.h"


//TODO: clean up includes, remove from game those things built extraneously by $make clean $make

//soon to do, better slection cursor, better energy bursts

main_menu_scene::main_menu_scene(bn::sprite_text_generator& text_generator):
    my_text_generator(text_generator),
    _main_menu_bg(bn::regular_bg_items::main_menu_background.create_bg(0, 0)),
    _selection_cursor_sprite(bn::sprite_items::selection_cursor.create_sprite(0, 30)),
    _right_book_arrow_sprite(bn::sprite_items::right_book_arrow.create_sprite(110, -8)),
    _left_book_arrow_sprite(bn::sprite_items::left_book_arrow.create_sprite(-110, -8)),
    menu_position(0),
    menu_position_max(1),
    state(0),
    number_tutorial_pages(6),
    current_tutorial_page(0)
{
    bn::music_items::menumusic.play(0.3);
    _selection_cursor_sprite.set_visible(false);
    _selection_cursor_sprite.set_z_order(-100);
    _selection_cursor_sprite.set_bg_priority(0); //lower z order means it shows up higher. wacky huh?

    _right_book_arrow_sprite.set_visible(false);
    _right_book_arrow_sprite.set_z_order(-98);
    _right_book_arrow_sprite.set_bg_priority(0); //lower z order means it shows up higher. wacky huh?

    _left_book_arrow_sprite.set_visible(false);
    _left_book_arrow_sprite.set_z_order(-98);
    _left_book_arrow_sprite.set_bg_priority(0); //lower z order means it shows up higher. wacky huh?
}

int main_menu_scene::run_scene()
{
    //bn::random random_generator;
    
    while(true)
    {
        //random_num = random_generator.get();

        switch(state)//main menu
        {
            
            case 0:
            {
                _display_center_text("KNIGHT OWLS","VERSION: ALPHA 1.3");
                _display_status("ud:MOVE, a:SELECT");
                _generate_virt_menu("BEGIN GAME", "HOW TO PLAY", "CREDITS");
                
                state=1;
                break;
            }
            case 1:
            {
                _navigate_through_virt_menu();//TODO: Make into separate state!

                if(bn::keypad::a_pressed())
                {
                    if(menu_position==0)
                    {
                        bn::core::update();
                        return(1); //next_scene is game scene
                    }
                    else if(menu_position==1)
                    {
                        state = 10;
                        current_tutorial_page = 0;
                        _clear_virt_menu();
                        _display_status("lr:MOVE, b:RETURN");
                    }
                    else if(menu_position==2)
                    {
                        state = 4;
                        _clear_virt_menu();
                        _display_center_text(
                            
                            "GAME MADE BY BLAISE RASCAL.",
                            "IT IS AVAILABLE FOR FREE.",
                            "IT WAS MADE WITH DEVKITARM,",
                            "GBA.NINJA, AND BUTANO. SEE THE",
                            "CURRENT STATUS OF THE GAME AT",
                            "BLAISE-RASCAL.ITCH.IO/KNIGHT-OWLS.",
                            "SOURCE CODE IS FREELY AVAILABLE,",
                            "BUT ALL OTHER RIGHTS RESERVED."
                            //"-- RESTIA FULAR! --"
                        );
                        _display_status("b:RETURN");
                    }
                }
                bn::core::update();
                break;
            }
            case 10:
            {
                _display_center_text(
                    "(1/6) OVERVIEW",
                    "",
                    "YOU ARE A SUMMONER OF OWLS.",
                    "ARMED WITH YOUR SPELLBOOK",
                    "(WHICH YOU CAN VIEW AT ANY",
                    "TIME BY SELECTING \"SPELLBOOK\"),",
                    "YOU MUST FACE MANY ENEMIES.",
                    "KEEP YOUR mHP ABOVE ZERO, JOURNEY",
                    "THROUGH THREE DIFFERENT AREAS,",
                    "AND DEFEAT THE FINAL BOSS TO WIN!"
                    );
                _right_book_arrow_sprite.set_visible(true);
                _left_book_arrow_sprite.set_visible(false);
                state = 3;
                break;
            }
            case 11:
            {
                _display_center_text(
                    "(2/6) SUMMONING",
                    "",
                    "WHEN YOU SELECT \"SUMMON\", YOU",
                    "CAST A RANDOM SPELL FROM YOUR",
                    "SPELLBOOK. SPELLS CAN INCREASE",
                    "YOUR kATTACK, cDOWLLARS, OR",
                    "iSTATIC. IF YOUR iSTATIC",
                    "GOES ABOVE 4, YOU WILL LOSE",
                    "1 mHP AND HAVE TO RESTART THE",
                    "SUMMONING PHASE."
                    );
                _right_book_arrow_sprite.set_visible(true);
                _left_book_arrow_sprite.set_visible(true);
                state = 3;
                break;
            }
            case 12:
            {
                _display_center_text(
                    "(3/6) FIGHTING",
                    "",
                    "SELECTING \"FIGHT\" WILL",
                    "INITIATE COMBAT. IF THE ENEMY'S",
                    "kATTACK IS HIGHER, YOU LOSE",
                    "SOME mHP. IF YOUR kATTACK IS",
                    "HIGHER, YOU RECOVER 1mHP. (YOU",
                    "WIN TIES.) WIN OR LOSE, ALL",
                    "YOUR SPELLS WILL BE RETURNED",
                    "TO YOUR SPELLBOOK."
                    );
                _right_book_arrow_sprite.set_visible(true);
                _left_book_arrow_sprite.set_visible(true);
                state = 3;
                break;
            }
            case 13:
            {
                _display_center_text(
                    "(4/6) SHOPPING",
                    "",
                    "AFTER FIGHTING, YOU WILL HAVE",
                    "THE CHANCE TO BUY NEW OWLS TO",
                    "IMPROVE YOUR SPELLBOOK.",
                    "TIP: IF YOU DON'T WANT TO BUY",
                    "ANYTHING, YOU CAN SAVE YOUR c.",
                    "TIP: +c OWLS ARE BETTER EARLY",
                    "IN A NEW AREA, AND +k OWLS ARE",
                    "BETTER WHEN APPROACHING A BOSS."
                    );
                _right_book_arrow_sprite.set_visible(true);
                _left_book_arrow_sprite.set_visible(true);
                state = 3;
                break;
            }
            case 14:
            {
                _display_center_text(
                    "(5/6) MAP LEGEND",
                    "",
                    "vw = ENEMY. THE NUMBER IS HOW",
                    "MUCH HP YOU WILL LOSE IF YOU",
                    "LOSE COMBAT.",
                    "o = BOSS. IF A BOSS WINS",
                    "COMBAT, YOU LOSE THE GAME.",
                    "IF YOU DEFEAT A BOSS, YOU HEAL",
                    "mHP TO FULL."
                    );
                _right_book_arrow_sprite.set_visible(true);
                _left_book_arrow_sprite.set_visible(true);
                state = 3;
                break;
            }
            case 15:
            {
                _display_center_text(
                    "(6/6) MAP LEGEND",
                    "",
                    "q = MIASMA. A SINISTER FOG",
                    "THAT DRAINS ALL c, RESETS YOUR",
                    "SPELLBOOK TO ITS ORIGINAL FORM,",
                    "AND TAKES YOU TO A NEW AREA.",
                    "h = SHIPWRECK. GIVES YOU",
                    "BANNERS, WHICH ARE UPGRADES",
                    "THAT PERSIST EVEN AFTER MIASMA.",
                    "THERE ARE 16 DIFFERENT BANNERS."
                    );
                _right_book_arrow_sprite.set_visible(false);
                _left_book_arrow_sprite.set_visible(true);
                state = 3;
                break;
            }
            //MAP LEGEND PART ONE
            //- jklj = ENEMY. THE NUMBER IS HOW MUCH HP YOU WILL LOSE IF YOU LOSE COMBAT.
            // = BOSS. IF A BOSS WINS COMBAT, YOU LOSE THE GAME. IF YOU WIN COMBAT, YOU HEAL HP TO FULL.
            // = MIASMA. A SINISTER FOG THAT DRAINS ALL c, AND RESETS YOUR SPELLBOOK TO ITS ORIGINAL FORM.
            // = SHIPWRECK. GIVES YOU BANNERS, WHICH ARE PERMANENT UPGRADES THAT PERSIST EVEN AFTER MIASMA. THERE ARE 16 DIFFERENT BANNERS.

            //(optional: Immediately after every boss is a spellbook reset. When this happens, you lose all owls you've added to your spellbook, as well as any DUST you've gathered, but you keep any BANNERS you may have acquired along the way. From here on out, the game will be more-or-less a repeat of what happened before, but it will be harder. This means you should choose your BANNERS carefully to maximize your chances of success post-reset.)
            case 3:
            {
                if(bn::keypad::b_pressed())
                {
                    _display_center_text("");
                    state = 0;
                    _right_book_arrow_sprite.set_visible(false);
                    _left_book_arrow_sprite.set_visible(false);
                }
                else if(bn::keypad::left_pressed())
                {
                    if(current_tutorial_page>0)
                    {
                        current_tutorial_page--;
                        state = current_tutorial_page+10;
                    }
                }
                else if(bn::keypad::right_pressed())
                {
                    if(current_tutorial_page<number_tutorial_pages-1)
                    {
                        current_tutorial_page++;
                        state = current_tutorial_page+10;
                    }
                }
                bn::core::update();
                break;
            }
            case 4:
            {
                if(bn::keypad::b_pressed())
                {
                    _display_center_text("");
                    state = 0;
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
void main_menu_scene::_display_center_text(const bn::string<50>& centertextone, const bn::string<50>& centertexttwo, const bn::string<50>& centertextthree, const bn::string<50>& centertextfour, const bn::string<50>& centertextfive, const bn::string<50>& centertextsix, const bn::string<50>& centertextseven, const bn::string<50>& centertexteight, const bn::string<50>& centertextnine, const bn::string<50>& centertextten)
{
    my_text_generator.set_center_alignment();
    center_text_one_sprites.clear();
    center_text_two_sprites.clear();
    center_text_three_sprites.clear();
    center_text_four_sprites.clear();
    center_text_five_sprites.clear();
    center_text_six_sprites.clear();
    center_text_seven_sprites.clear();
    center_text_eight_sprites.clear();
    center_text_nine_sprites.clear();
    center_text_ten_sprites.clear();
    my_text_generator.generate(0, -60, centertextone, center_text_one_sprites);
    my_text_generator.generate(0, -49, centertexttwo, center_text_two_sprites);
    my_text_generator.generate(0, -38, centertextthree, center_text_three_sprites);
    my_text_generator.generate(0, -27, centertextfour, center_text_four_sprites);
    my_text_generator.generate(0, -16, centertextfive, center_text_five_sprites);
    my_text_generator.generate(0, -5, centertextsix, center_text_six_sprites);
    my_text_generator.generate(0, 6, centertextseven, center_text_three_sprites);
    my_text_generator.generate(0, 17, centertexteight, center_text_four_sprites);
    my_text_generator.generate(0, 28, centertextnine, center_text_five_sprites);
    my_text_generator.generate(0, 39, centertextten, center_text_ten_sprites);
}

//TODO: Put this in its own class maybe
void main_menu_scene::_display_status(const bn::string<50>& statustextone, const bn::string<50>& statustexttwo, const bn::string<50>& statustextthree)
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

void main_menu_scene::_generate_virt_menu(const bn::string<12>& menu_option_one, const bn::string<12>& menu_option_two, const bn::string<12>& menu_option_three)
{
    my_text_generator.set_center_alignment();
    first_menu_option_text_sprites.clear();
    second_menu_option_text_sprites.clear();
    third_menu_option_text_sprites.clear();

    my_text_generator.generate(0, -11, menu_option_one, first_menu_option_text_sprites);
    my_text_generator.generate(0, 0, menu_option_two, second_menu_option_text_sprites);
    my_text_generator.generate(0, 11, menu_option_three, third_menu_option_text_sprites);
    menu_position_max = 2;
    menu_position = 0;
    _selection_cursor_sprite.set_visible(true);
    _update_selection_cursor_from_virt_menu_position();

}

void main_menu_scene::_navigate_through_virt_menu()
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


//point_cursor_at_sprite, point_cursor_at_string
//MENU: vector of vector of sprites?
void main_menu_scene::_update_selection_cursor_from_virt_menu_position()
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

void main_menu_scene::_point_cursor_at_letter(const bn::sprite_ptr& target_sprite)
{   
    _selection_cursor_sprite.set_y(target_sprite.y());
    _selection_cursor_sprite.set_x(target_sprite.x()-25);
    _selection_cursor_sprite.set_visible(true);
}

void main_menu_scene::_clear_virt_menu() //TODO: Uh, maybe this should be called clear hor menu?
{
    _selection_cursor_sprite.set_visible(false);
    first_menu_option_text_sprites.clear();
    second_menu_option_text_sprites.clear();
    third_menu_option_text_sprites.clear();
    
    //_rift_bg.set_visible(false);
}