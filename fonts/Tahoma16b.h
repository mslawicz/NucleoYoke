

/*
 *
 * Tahoma16b
 *
 * created with FontCreator
 * written by F. Maximilian Thiele
 *
 * http://www.apetech.de/fontCreator
 * me@apetech.de
 *
 * File Name           : tahoma16b.h
 * Date                : 24.02.2019
 * Font size in bytes  : 13014
 * Font width          : 10
 * Font height         : 16
 * Font first char     : 32
 * Font last char      : 128
 * Font used chars     : 96
 *
 * The font data are defined as
 *
 * struct _FONT_ {
 *     uint16_t   font_Size_in_Bytes_over_all_included_Size_it_self;
 *     uint8_t    font_Width_in_Pixel_for_fixed_drawing;
 *     uint8_t    font_Height_in_Pixel_for_all_characters;
 *     unit8_t    font_First_Char;
 *     uint8_t    font_Char_Count;
 *
 *     uint8_t    font_Char_Widths[font_Last_Char - font_First_Char +1];
 *                  // for each character the separate width in pixels,
 *                  // characters < 128 have an implicit virtual right empty row
 *
 *     uint8_t    font_data[];
 *                  // bit field of all characters
 */

#ifndef TAHOMA16B_H
#define TAHOMA16B_H

const uint8_t FontTahoma16b[] = {
    0x32, 0xD6, // size
    0x08, // width
    0x10, // height
    0x20, // first char
    0x60, // char count
    
    // char widths
    0x00, 0x02, 0x06, 0x0C, 0x09, 0x12, 0x0B, 0x02, 
    0x05, 0x05, 0x08, 0x09, 0x04, 0x05, 0x02, 0x07, 
    0x09, 0x06, 0x09, 0x09, 0x0A, 0x09, 0x09, 0x09, 
    0x09, 0x09, 0x02, 0x04, 0x0A, 0x0A, 0x0A, 0x08, 
    0x0E, 0x0B, 0x0A, 0x0A, 0x0B, 0x09, 0x08, 0x0B, 
    0x0B, 0x06, 0x07, 0x0A, 0x08, 0x0D, 0x0B, 0x0B, 
    0x0A, 0x0B, 0x0B, 0x09, 0x0A, 0x0B, 0x0A, 0x0F, 
    0x0A, 0x0A, 0x09, 0x05, 0x07, 0x05, 0x0A, 0x0A, 
    0x03, 0x09, 0x09, 0x07, 0x09, 0x09, 0x06, 0x09, 
    0x09, 0x02, 0x05, 0x09, 0x02, 0x0E, 0x09, 0x09, 
    0x09, 0x09, 0x06, 0x07, 0x06, 0x09, 0x08, 0x0D, 
    0x09, 0x08, 0x08, 0x09, 0x02, 0x09, 0x0C, 0x0C, 
    
    
    // font data
    0xFE, 0xFE, 0x1B, 0x1B, // 33
    0x1F, 0x1F, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 34
    0x00, 0x30, 0xB0, 0x78, 0x36, 0x30, 0x30, 0x30, 0xB0, 0x78, 0x36, 0x30, 0x03, 0x1B, 0x07, 0x03, 0x03, 0x03, 0x03, 0x1B, 0x07, 0x03, 0x03, 0x00, // 35
    0x70, 0xF8, 0x8C, 0x84, 0xFF, 0x84, 0x84, 0x9C, 0x18, 0x0C, 0x1C, 0x10, 0x10, 0x7F, 0x10, 0x18, 0x0F, 0x07, // 36
    0xFC, 0xFE, 0x02, 0x02, 0x02, 0xFE, 0xFC, 0x00, 0x80, 0x60, 0x10, 0xCC, 0xE2, 0x20, 0x20, 0x20, 0xE0, 0xC0, 0x00, 0x01, 0x01, 0x01, 0x01, 0x11, 0x0C, 0x02, 0x01, 0x00, 0x00, 0x0F, 0x1F, 0x10, 0x10, 0x10, 0x1F, 0x0F, // 37
    0x00, 0x9C, 0xFE, 0x62, 0xC2, 0xE2, 0x3E, 0x1C, 0xC0, 0xC0, 0x00, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x11, 0x1B, 0x0E, 0x0F, 0x19, 0x10, // 38
    0x1F, 0x1F, 0x00, 0x00, // 39
    0xF0, 0xFC, 0x0E, 0x03, 0x01, 0x0F, 0x3F, 0x70, 0xC0, 0x80, // 40
    0x01, 0x03, 0x0E, 0xFC, 0xF0, 0x80, 0xC0, 0x70, 0x3F, 0x0F, // 41
    0x42, 0x24, 0x18, 0xFF, 0xFF, 0x18, 0x24, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 42
    0x80, 0x80, 0x80, 0x80, 0xF8, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, // 43
    0x00, 0x00, 0x00, 0x00, 0xC0, 0xFC, 0x3C, 0x04, // 44
    0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, // 45
    0x00, 0x00, 0x1C, 0x1C, // 46
    0x00, 0x00, 0x00, 0x80, 0x70, 0x0C, 0x03, 0xC0, 0x30, 0x0E, 0x01, 0x00, 0x00, 0x00, // 47
    0xF8, 0xFC, 0x06, 0x02, 0x02, 0x02, 0x06, 0xFC, 0xF8, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 48
    0x08, 0x08, 0xFE, 0xFE, 0x00, 0x00, 0x10, 0x10, 0x1F, 0x1F, 0x10, 0x10, // 49
    0x0C, 0x0E, 0x02, 0x02, 0x02, 0x82, 0xC6, 0x7C, 0x38, 0x10, 0x18, 0x1C, 0x16, 0x13, 0x11, 0x10, 0x10, 0x10, // 50
    0x0C, 0x0E, 0x02, 0x42, 0x42, 0x42, 0xE6, 0xBC, 0x18, 0x0C, 0x1C, 0x10, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 51
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0xFE, 0xFE, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x1F, 0x1F, 0x01, 0x01, // 52
    0x00, 0x3E, 0x3E, 0x22, 0x22, 0x22, 0x62, 0xC2, 0x82, 0x0C, 0x1C, 0x10, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 53
    0xF0, 0xF8, 0x2C, 0x26, 0x22, 0x22, 0x62, 0xC2, 0x80, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 54
    0x02, 0x02, 0x02, 0x02, 0x82, 0xE2, 0x7A, 0x1E, 0x06, 0x00, 0x00, 0x18, 0x1E, 0x07, 0x01, 0x00, 0x00, 0x00, // 55
    0x18, 0xBC, 0xE6, 0x42, 0x42, 0x42, 0xE6, 0xBC, 0x18, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 56
    0x78, 0xFC, 0x86, 0x02, 0x02, 0x02, 0x06, 0xFC, 0xF8, 0x00, 0x10, 0x11, 0x11, 0x11, 0x19, 0x0D, 0x07, 0x03, // 57
    0x70, 0x70, 0x1C, 0x1C, // 58
    0x00, 0x70, 0x70, 0x00, 0xC0, 0xFC, 0x3C, 0x04, // 59
    0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, // 60
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, // 61
    0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01, // 62
    0x0C, 0x0E, 0x02, 0x02, 0x82, 0xC6, 0x7C, 0x38, 0x00, 0x00, 0x00, 0x1B, 0x1B, 0x00, 0x00, 0x00, // 63
    0xE0, 0x10, 0x08, 0xC4, 0xE2, 0x32, 0x12, 0x12, 0xF2, 0xF2, 0x04, 0x08, 0x10, 0xE0, 0x07, 0x08, 0x10, 0x23, 0x47, 0x4C, 0x48, 0x48, 0x47, 0x4F, 0x08, 0x08, 0x04, 0x03, // 64
    0x00, 0x00, 0xE0, 0xF8, 0x1E, 0x06, 0x1E, 0xF8, 0xE0, 0x00, 0x00, 0x1C, 0x1F, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x1F, 0x1C, // 65
    0xFE, 0xFE, 0x42, 0x42, 0x42, 0x42, 0x66, 0xFC, 0x98, 0x00, 0x1F, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 66
    0xF0, 0xFC, 0x0C, 0x06, 0x02, 0x02, 0x02, 0x02, 0x0E, 0x0C, 0x03, 0x0F, 0x0C, 0x18, 0x10, 0x10, 0x10, 0x10, 0x1C, 0x0C, // 67
    0xFE, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x06, 0x04, 0x1C, 0xF8, 0xE0, 0x1F, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x18, 0x08, 0x0E, 0x07, 0x01, // 68
    0xFE, 0xFE, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x1F, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // 69
    0xFE, 0xFE, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 70
    0xF0, 0xFC, 0x0C, 0x06, 0x02, 0x02, 0x82, 0x82, 0x82, 0x8E, 0x8C, 0x03, 0x0F, 0x0C, 0x18, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x0F, // 71
    0xFE, 0xFE, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xFE, 0xFE, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, // 72
    0x02, 0x02, 0xFE, 0xFE, 0x02, 0x02, 0x10, 0x10, 0x1F, 0x1F, 0x10, 0x10, // 73
    0x00, 0x02, 0x02, 0x02, 0x02, 0xFE, 0xFE, 0x10, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 74
    0xFE, 0xFE, 0x40, 0xE0, 0xB0, 0x18, 0x0C, 0x06, 0x02, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x01, 0x03, 0x06, 0x0C, 0x18, 0x10, // 75
    0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // 76
    0xFE, 0x0E, 0x3C, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0xC0, 0x30, 0x0C, 0xFE, 0xFE, 0x1F, 0x00, 0x00, 0x00, 0x01, 0x07, 0x06, 0x01, 0x00, 0x00, 0x00, 0x1F, 0x1F, // 77
    0xFE, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0xFE, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0E, 0x1F, // 78
    0xF0, 0xFC, 0x0C, 0x06, 0x02, 0x02, 0x02, 0x06, 0x0C, 0xFC, 0xF0, 0x03, 0x0F, 0x0C, 0x18, 0x10, 0x10, 0x10, 0x18, 0x0C, 0x0F, 0x03, // 79
    0xFE, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x02, 0x86, 0xFC, 0x78, 0x1F, 0x1F, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, // 80
    0xF0, 0xFC, 0x0C, 0x06, 0x02, 0x02, 0x02, 0x06, 0x0C, 0xFC, 0xF0, 0x03, 0x0F, 0x0C, 0x18, 0x10, 0x30, 0x70, 0xD8, 0x8C, 0x8F, 0x83, // 81
    0xFE, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x86, 0xFC, 0x78, 0x00, 0x00, 0x1F, 0x1F, 0x01, 0x01, 0x01, 0x01, 0x03, 0x06, 0x0C, 0x18, 0x10, // 82
    0x38, 0x7C, 0xE6, 0xC2, 0xC2, 0xC2, 0xC2, 0x8E, 0x0C, 0x0C, 0x1C, 0x10, 0x10, 0x10, 0x10, 0x19, 0x0F, 0x07, // 83
    0x02, 0x02, 0x02, 0x02, 0xFE, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, // 84
    0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 85
    0x0E, 0x7E, 0xF0, 0x80, 0x00, 0x00, 0x80, 0xF0, 0x7E, 0x0E, 0x00, 0x00, 0x03, 0x1F, 0x1C, 0x1C, 0x1F, 0x03, 0x00, 0x00, // 86
    0x1E, 0xFE, 0xE0, 0x00, 0x00, 0xE0, 0xFC, 0x3E, 0xFC, 0xE0, 0x00, 0x00, 0xE0, 0xFE, 0x1E, 0x00, 0x03, 0x1F, 0x1C, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x07, 0x1F, 0x1C, 0x1F, 0x03, 0x00, // 87
    0x06, 0x0E, 0x18, 0x30, 0xE0, 0xE0, 0x30, 0x18, 0x0E, 0x06, 0x18, 0x1C, 0x06, 0x03, 0x01, 0x01, 0x03, 0x06, 0x1C, 0x18, // 88
    0x06, 0x1E, 0x38, 0xE0, 0xC0, 0xC0, 0xE0, 0x38, 0x1E, 0x06, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, // 89
    0x02, 0x02, 0x02, 0x82, 0xC2, 0x72, 0x3A, 0x0E, 0x06, 0x18, 0x1C, 0x17, 0x13, 0x10, 0x10, 0x10, 0x10, 0x10, // 90
    0xFF, 0xFF, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0x80, 0x80, 0x80, // 91
    0x03, 0x0C, 0x70, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0E, 0x30, 0xC0, // 92
    0x01, 0x01, 0x01, 0xFF, 0xFF, 0x80, 0x80, 0x80, 0xFF, 0xFF, // 93
    0x40, 0x20, 0x18, 0x04, 0x02, 0x02, 0x04, 0x18, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 94
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, // 95
    0x02, 0x06, 0x04, 0x00, 0x00, 0x00, // 96
    0x00, 0x20, 0x30, 0x90, 0x90, 0x90, 0xB0, 0xE0, 0xC0, 0x0E, 0x1F, 0x11, 0x10, 0x10, 0x10, 0x08, 0x1F, 0x1F, // 97
    0xFF, 0xFF, 0x20, 0x10, 0x10, 0x10, 0x30, 0xE0, 0xC0, 0x1F, 0x1F, 0x08, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 98
    0xC0, 0xE0, 0x30, 0x10, 0x10, 0x10, 0x20, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x10, 0x08, // 99
    0xC0, 0xE0, 0x30, 0x10, 0x10, 0x10, 0x20, 0xFF, 0xFF, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x10, 0x08, 0x1F, 0x1F, // 100
    0xC0, 0xE0, 0x30, 0x10, 0x10, 0x10, 0x30, 0xE0, 0xC0, 0x07, 0x0F, 0x19, 0x11, 0x11, 0x11, 0x11, 0x19, 0x09, // 101
    0x10, 0xFE, 0xFF, 0x11, 0x11, 0x01, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, // 102
    0xC0, 0xE0, 0x30, 0x10, 0x10, 0x10, 0x20, 0xF0, 0xF0, 0x07, 0x4F, 0xD8, 0x90, 0x90, 0x90, 0xC8, 0x7F, 0x3F, // 103
    0xFF, 0xFF, 0x20, 0x10, 0x10, 0x10, 0x30, 0xE0, 0xC0, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, // 104
    0xF6, 0xF6, 0x1F, 0x1F, // 105
    0x00, 0x10, 0x10, 0xF6, 0xF6, 0x80, 0x80, 0x80, 0xFF, 0x7F, // 106
    0xFF, 0xFF, 0x00, 0x80, 0xC0, 0x60, 0x30, 0x10, 0x00, 0x1F, 0x1F, 0x01, 0x01, 0x03, 0x06, 0x0C, 0x18, 0x10, // 107
    0xFF, 0xFF, 0x1F, 0x1F, // 108
    0xF0, 0xF0, 0x20, 0x10, 0x10, 0x10, 0xF0, 0xE0, 0x20, 0x10, 0x10, 0x10, 0xF0, 0xE0, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, // 109
    0xF0, 0xF0, 0x20, 0x10, 0x10, 0x10, 0x30, 0xE0, 0xC0, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, // 110
    0xC0, 0xE0, 0x30, 0x10, 0x10, 0x10, 0x30, 0xE0, 0xC0, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 111
    0xF0, 0xF0, 0x20, 0x10, 0x10, 0x10, 0x30, 0xE0, 0xC0, 0xFF, 0xFF, 0x08, 0x10, 0x10, 0x10, 0x18, 0x0F, 0x07, // 112
    0xC0, 0xE0, 0x30, 0x10, 0x10, 0x10, 0x20, 0xF0, 0xF0, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x10, 0x08, 0xFF, 0xFF, // 113
    0xF0, 0xF0, 0x20, 0x30, 0x30, 0x30, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, // 114
    0xE0, 0xF0, 0x90, 0x90, 0x10, 0x10, 0x20, 0x08, 0x11, 0x11, 0x13, 0x13, 0x1F, 0x0E, // 115
    0x10, 0xFE, 0xFE, 0x10, 0x10, 0x10, 0x00, 0x0F, 0x1F, 0x10, 0x10, 0x10, // 116
    0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x07, 0x0F, 0x18, 0x10, 0x10, 0x10, 0x08, 0x1F, 0x1F, // 117
    0x70, 0xF0, 0x80, 0x00, 0x00, 0x80, 0xF0, 0x70, 0x00, 0x03, 0x0F, 0x1C, 0x1C, 0x0F, 0x03, 0x00, // 118
    0x30, 0xF0, 0xC0, 0x00, 0x80, 0xE0, 0x70, 0xE0, 0x80, 0x00, 0xC0, 0xF0, 0x30, 0x00, 0x03, 0x1F, 0x1C, 0x0F, 0x03, 0x00, 0x03, 0x0F, 0x1C, 0x1F, 0x03, 0x00, // 119
    0x10, 0x30, 0x60, 0xC0, 0x80, 0xC0, 0x60, 0x30, 0x10, 0x10, 0x18, 0x0C, 0x07, 0x03, 0x07, 0x0C, 0x18, 0x10, // 120
    0x70, 0xF0, 0x80, 0x00, 0x00, 0x80, 0xF0, 0x70, 0x00, 0x03, 0x8F, 0xFC, 0x7C, 0x0F, 0x03, 0x00, // 121
    0x10, 0x10, 0x10, 0x10, 0x90, 0xD0, 0x70, 0x30, 0x18, 0x1C, 0x16, 0x13, 0x11, 0x10, 0x10, 0x10, // 122
    0x80, 0x80, 0xC0, 0x7C, 0x3E, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x3F, 0x7E, 0xC0, 0x80, 0x80, 0x80, // 123
    0xFF, 0xFF, 0xFF, 0xFF, // 124
    0x01, 0x01, 0x01, 0x03, 0x3E, 0x7C, 0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC0, 0x7E, 0x3F, 0x01, 0x00, 0x00, // 125
    0x80, 0xC0, 0x60, 0x60, 0x60, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x80, 0xE0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x01, 0x00, // 126
    0xFE, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0xFE, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F // 127
    
};

#endif
