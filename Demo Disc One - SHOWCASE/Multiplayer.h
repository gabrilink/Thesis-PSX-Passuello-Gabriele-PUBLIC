#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>


#include "globals.h"
#include "display.h"



// 4 split screen in this example
#define NUM_VIEWPORTS 4
#define VIEWPORT_WIDTH  (320 / 2)
#define VIEWPORT_HEIGHT (240 / 2)

// In this sample the order table length is simple
//#define OT_LEN 8

typedef struct Display_buffer
{
    DRAWENV draw; // drawing environment
    DISPENV disp; // display environment
    u_long ot[OT_LEN]; // ordering table

} Display_buffer;

typedef struct {
    u_char cur_buf;
    Display_buffer disp_buffs[2][NUM_VIEWPORTS];
} System;

// Enumerazione per i bit per pixel
typedef enum {
    TEXTURE_4BIT,   // 0
    TEXTURE_8BIT,   // 1
    TEXTURE_16BIT   // 2
} TEXTURE_BPP;

typedef struct {
    u_int u0;
    u_int v0;
    u_int u1;
    u_int v1;
    u_int u2;
    u_int v2;
} TEXCOORD;

typedef struct {
    u_short tpage_id; // Texture page ID
    u_short clut_id;  // CLUT ID
    int width;
    int height;
    int was_loaded;   // Indica se la texture è stata caricata
    u_short u;        // Offset X della texture
    u_short v;        // Offset Y della texture
    int clutX;        // Coordinate X del CLUT
    int clutY;        // Coordinate Y del CLUT
} Texture;

typedef struct {
    POLY_F4 poly;
    SVECTOR pos;
    u_short width;
    u_short height;
} Sprite;

typedef struct {
    Sprite base; // ereditarietà simulata
    POLY_FT4 poly_tex;
    u_short u;
    u_short v;
    Texture texture;
} Sprite_textured;

// Costanti
static const u_int FOG_NEAR = 300;
//static const u_int var_SCREEN_Z = 512;

// Funzioni
void System_init(System* system);
void System_init_graphics(System* system);
void System_init_3d(System* system);
void System_set_dispenv(System* system);
void System_start_frame(System* system, int viewport_index);
void System_end_frame(System* system, int viewport_index);
void System_deinit(System* system);
void System_add_prim(System* system, void* prim, int depth, int viewport_index);

// Singleton instance
System* System_get_instance();

// Funzioni
void Texture_init(Texture* texture);
void Texture_load(Texture* texture, u_long* texdata, u_long* clutdata, TEXTURE_BPP bpp, int x, int y, int w, int h, int clutX, int clutY);
void Texture_copy_from(Texture* dest, const Texture* src);
void Texture_apply_to_primitive(Texture* texture, POLY_FT3* prim, TEXCOORD* tex_coord);

// Getters
u_short Texture_get_texture_page_id(const Texture* texture);
u_short Texture_get_clut_id(const Texture* texture);
int Texture_get_width(const Texture* texture);
int Texture_get_height(const Texture* texture);

// Funzioni per Sprite
void Sprite_init(Sprite* sprite);
void Sprite_set_size(Sprite* sprite, u_short width, u_short height);
void Sprite_set_pos(Sprite* sprite, u_short x, u_short y);
void Sprite_draw(Sprite* sprite);

// Funzioni per Sprite_textured
void Sprite_textured_init(Sprite_textured* sprite);
void Sprite_textured_set_size(Sprite_textured* sprite, u_short width, u_short height);
void Sprite_textured_set_pos(Sprite_textured* sprite, u_short x, u_short y);
void Sprite_textured_draw(Sprite_textured* sprite);
void Sprite_textured_draw_ordered(Sprite_textured* sprite, int depth);
void Sprite_textured_load_texture(Sprite_textured* sprite, u_long* texdata, u_long* clutdata, TEXTURE_BPP bpp, int x, int y, int w, int h, int clutX, int clutY);
void Sprite_textured_copy_texture(Sprite_textured* dest, const Sprite_textured* src);

#endif // SYSTEM_H_INCLUDED
