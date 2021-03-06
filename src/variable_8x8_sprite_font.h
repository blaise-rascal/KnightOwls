/*
 * Copyright (c) 2020-2021 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef VARIABLE_8x8_SPRITE_FONT_H
#define VARIABLE_8x8_SPRITE_FONT_H

#include "bn_sprite_font.h"
#include "bn_sprite_items_variable_8x8_font.h"

constexpr const bn::string_view variable_8x8_sprite_font_utf8_characters[] = {
    "Á", "É", "Í", "Ó", "Ú", "Ü", "Ñ", "á", "é", "í", "ó", "ú", "ü", "ñ", "¡", "¿"
};

constexpr const int8_t variable_8x8_sprite_font_character_widths[] = {
    6,  // 32
    3,  // 33 !
    6,  // 34 "
    7,  // 35 #
    8,  // 36 $
    8,  // 37 %
    8,  // 38 &
    3,  // 39 '
    4,  // 40 (
    4,  // 41 )
    6,  // 42 *
    7,  // 43 +
    3,  // 44 ,
    7,  // 45 -
    3,  // 46 .
    8,  // 47 /
    7,  // 48 0
    7,  // 49 1
    7,  // 50 2
    7,  // 51 3
    7,  // 52 4
    7,  // 53 5
    7,  // 54 6
    7,  // 55 7
    7,  // 56 8
    7,  // 57 9
    5,  // 58 :
    5,  // 59 ;
    7,  // 60 <
    7,  // 61 =
    7,  // 62 >
    7,  // 63 ?
    8,  // 64 @
    7,  // 65 A
    7,  // 66 B
    7,  // 67 C
    7,  // 68 D
    7,  // 69 E
    7,  // 70 F
    7,  // 71 G
    7,  // 72 H
    7,  // 73 I
    7,  // 74 J
    7,  // 75 K
    7,  // 76 L
    8,  // 77 M
    7,  // 78 N
    7,  // 79 O
    7,  // 80 P
    8,  // 81 Q
    7,  // 82 R
    7,  // 83 S
    7,  // 84 T
    7,  // 85 U
    8,  // 86 V
    8,  // 87 W
    7,  // 88 X
    7,  // 89 Y
    7,  // 90 Z
    4,  // 91 [
    8,  // 92
    4,  // 93 ]
    7,  // 94 ^
    7,  // 95 _
    3,  // 96 `
    7,  // 97 a
    7,  // 98 b
    8,  // 99 c
    7,  // 100 d
    1,  // 101 e
    8,  // 102 f
    8,  // 103 g
    5,  // 104 h
    8,  // 105 i
    8,  // 106 j
    8,  // 107 k
    7,  // 108 l
    8,  // 109 m
    8,  // 110 n
    8,  // 111 o
    8,  // 112 p
    8,  // 113 q
    7,  // 114 r
    7,  // 115 s
    7,  // 116 t
    7,  // 117 u
    7,  // 118 v
    7,  // 119 w
    4,  // 120 x
    7,  // 121 y
    7,  // 122 z
    5,  // 123 {
    3,  // 124 |
    5,  // 125 }
    8,  // 126 ~
    7,  // Á
    6,  // É
    7,  // Í
    7,  // Ó
    7,  // Ú
    7,  // Ü
    7,  // Ñ
    5,  // á
    5,  // é
    5,  // í
    5,  // ó
    6,  // ú
    6,  // ü
    5,  // ñ
    3,  // ¡
    7,  // ¿
};

constexpr const bn::sprite_font variable_8x8_sprite_font(
        bn::sprite_items::variable_8x8_font, variable_8x8_sprite_font_utf8_characters,
        variable_8x8_sprite_font_character_widths);

#endif
