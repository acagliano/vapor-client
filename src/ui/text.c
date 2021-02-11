#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <graphx.h>
#include <tice.h>

#define LINE_SPACING 10
uint8_t text_WrappedString(const char *str, uint24_t left_margin, uint8_t top_margin, uint24_t right_margin){
    char *word = str;
    char *current = str;
    uint8_t lines = 1;

    gfx_SetTextXY(left_margin, top_margin);

    while(true) {
        uint8_t width = 0;
        for(; *current != ' ' && *current != 0; current++) {
            width += gfx_GetCharWidth(*current);
        }

        if(gfx_GetTextX() + width > right_margin) {
            gfx_SetTextXY(left_margin, gfx_GetTextY() + LINE_SPACING);
            lines++;
        }

        for(; word <= current; word++) {
            gfx_PrintChar(*word);
        }

        if(*current == 0) break;
        current++;
    }
    return lines;
}

uint8_t num_len(uint24_t num){
    uint8_t len=0;
    do {
        len++;
        num/=10;
    } while(num>10);
    return num;
}

uint24_t text_CenterText(const char *string, const char c, uint24_t x, uint24_t w){
    uint24_t str_w = (string!=NULL) ? gfx_GetStringWidth(string) : (c!=0) ? gfx_GetCharWidth(c) : 0;
    if(!str_w) return str_w;
    w-=str_w;
    return x+(w/2);
}

void gfx_PrintStringCentered(const char* string, uint24_t x, uint8_t y, uint24_t w){
    gfx_PrintStringXY(string, x+((w-gfx_GetStringWidth(string))/2), y);
}
