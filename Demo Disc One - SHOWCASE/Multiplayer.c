#include "Multiplayer.h"

// Costanti
//const u_int FOG_NEAR = 300;
//const u_int SCREEN_Z = 512;

// Singleton instance
static System system_instance;

System* System_get_instance() {
    return &system_instance;
}

void System_init(System* system) {
    RECT rect;

    ResetGraph(0);
    InitGeom();
    DecDCTReset(0);
    CdInit();

    SetGraphDebug(0);

    setRECT(&rect, 0, 0, 320, 240);
    ClearImage(&rect, 0, 0, 0);
    SetDispMask(1);

    system->cur_buf = 0;
}

void System_init_graphics(System* system) {
    int i, viewport_x = 0, viewport_y = 0;
    u_char r, g, b;

    FntLoad(960, 256);
    SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));

    for (i = 0; i < NUM_VIEWPORTS; ++i) {
        switch (i) {
            case 0: viewport_x = 0;         viewport_y = 0;         r = 255; g = 0;   b = 0;   break;
            case 1: viewport_x = 320 / 2;  viewport_y = 0;         r = 0;   g = 255; b = 0;   break;
            case 2: viewport_x = 0;         viewport_y = 120;       r = 0;   g = 0;   b = 255; break;
            case 3: viewport_x = 320 / 2;  viewport_y = 120;       r = 255; g = 0;   b = 255; break;
        }

        SetDefDrawEnv(&system->disp_buffs[0][i].draw, viewport_x, viewport_y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
        SetDefDispEnv(&system->disp_buffs[0][i].disp, viewport_x, viewport_y + 240, 320, 240);

        SetDefDrawEnv(&system->disp_buffs[1][i].draw, viewport_x, viewport_y + 240, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
        SetDefDispEnv(&system->disp_buffs[1][i].disp, viewport_x, viewport_y, 320, 240);

        system->disp_buffs[0][i].draw.isbg = system->disp_buffs[1][i].draw.isbg = 1;
        setRGB0(&system->disp_buffs[0][i].draw, r, g, b);
        setRGB0(&system->disp_buffs[1][i].draw, r, g, b);
    }

    SetFarColor(0, 200, 0);
    SetDispMask(1);
}

void System_init_3d(System* system) {
    SetGeomOffset(160, 120);
    SetGeomScreen(SCREEN_Z);
    SetFogNear(FOG_NEAR, SCREEN_Z);
}

void System_set_dispenv(System* system) {
    system->cur_buf = !system->cur_buf;
    PutDispEnv(system->cur_buf ? &system->disp_buffs[1][0].disp :
                                 &system->disp_buffs[0][0].disp);
}

void System_start_frame(System* system, int viewport_index) {
    PutDrawEnv(system->cur_buf ? &system->disp_buffs[1][viewport_index].draw :
                                 &system->disp_buffs[0][viewport_index].draw);
    ClearOTagR(system->disp_buffs[system->cur_buf][viewport_index].ot, OT_LEN);
}

void System_end_frame(System* system, int viewport_index) {
    DrawOTag(&system->disp_buffs[system->cur_buf][viewport_index].ot[OT_LEN - 1]);
}

void System_deinit(System* system) {
    StopCallback();
}

void System_add_prim(System* system, void* prim, int depth, int viewport_index) {
    AddPrim(&system->disp_buffs[system->cur_buf][viewport_index].ot[depth], prim);
}

void Texture_init(Texture* texture) {
    texture->tpage_id = 0;
    texture->clut_id = 0;
    texture->width = 0;
    texture->height = 0;
    texture->was_loaded = 0;
}

void Texture_load(Texture* texture, u_long* texdata, u_long* clutdata, TEXTURE_BPP bpp, int x, int y, int w, int h, int clutX, int clutY) {
    if (clutdata != NULL) {
        texture->clut_id = LoadClut(clutdata, clutX, clutY);
    }

    texture->tpage_id = LoadTPage(
        texdata,
        bpp,
        0, // Semitransparency rate (default 0)
        x,
        y,
        w,
        h
    );

    texture->width = w;
    texture->height = h;
    texture->was_loaded = 1;
}

void Texture_copy_from(Texture* dest, const Texture* src) {
    dest->was_loaded = 1;
    dest->tpage_id = src->tpage_id;
    dest->clut_id = src->clut_id;
    dest->width = src->width;
    dest->height = src->height;
}

void Texture_apply_to_primitive(Texture* texture, POLY_FT3* prim, TEXCOORD* tex_coord) {
    setUV3(
        prim,
        tex_coord->u0 * texture->width,
        tex_coord->v0 * texture->height,
        tex_coord->u1 * texture->width,
        tex_coord->v1 * texture->height,
        tex_coord->u2 * texture->width,
        tex_coord->v2 * texture->height
    );

    prim->tpage = texture->tpage_id;
    prim->clut = texture->clut_id;
}

u_short Texture_get_texture_page_id(const Texture* texture) {
    return texture->tpage_id;
}

u_short Texture_get_clut_id(const Texture* texture) {
    return texture->clut_id;
}

int Texture_get_width(const Texture* texture) {
    return texture->width;
}

int Texture_get_height(const Texture* texture) {
    return texture->height;
}

void Sprite_init(Sprite* sprite) {
    SetPolyF4(&sprite->poly);
    SetShadeTex(&sprite->poly, 1); // Disable ShadeTex

    sprite->pos.vx = sprite->pos.vy = sprite->pos.vz = 0;

    setRGB0(&sprite->poly, 128, 128, 128);

    sprite->width = 0;
    sprite->height = 0;
}

void Sprite_set_size(Sprite* sprite, u_short width, u_short height) {
    sprite->width = width;
    sprite->height = height;
    setXYWH(&sprite->poly, sprite->pos.vx, sprite->pos.vy, width, height);
}

void Sprite_set_pos(Sprite* sprite, u_short x, u_short y) {
    sprite->pos.vx = x;
    sprite->pos.vy = y;
    setXYWH(&sprite->poly, sprite->pos.vx, sprite->pos.vy, sprite->width, sprite->height);
}

void Sprite_draw(Sprite* sprite) {
    DrawPrim(&sprite->poly);
}

