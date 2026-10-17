/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* (0.8.5) HOME's lens (ui_lens.c): the panels of ECO and BEING, drawn as the approved mock-up (lente_sistema.html,
 * HOME) in the lens's panel (240 x 140 at y 84). Included by ui_graph.c after ui_being.c. The beings are JIANT's own
 * (ui_being.c: the same genomes, isotherms, nuclei, the same swelling, exciting and swimming with each track's sound,
 * graph_ecosys's maths); the mock-up gave the chrome around them.
 *   ECO    (0.8.8: as 0.8.4's HOME again, graph_ecosys) the six tracks' beings swimming free over the whole panel, each
 *          by itself (wandering, bouncing off the walls, a hit kicking it on, swelling with its sound), its MUTATE dots
 *          round it and its number under it; the selected one larger, its number in the theme's colour (no ring, no
 *          grid, no names). The cytoplasm's granules drift behind them, the critters (below) among them. MACROS: the
 *          macros' routes counted at the top left, LATCH / HOLD LFO at the right
 *   BEING  the track's being large in the middle, its four HOME knobs as organs at the corners (a gauge each, its name
 *          and value, a vein to the body), the sound's name under it; ROUTES: the being small, M1..M4 down the left
 *          with their routes drawn to their destinations ("T1 CUT+40")
 * The critters (lz_cr_*): a fixed pool of tiny protozoa round the beings, ciliates (their cilia beating), spores (their
 * spines turning), drifting cells (a ring and a nucleus), flagellates (a tail waving), diatoms (a rod); they wander,
 * gather round the tracks that sound (each drawn to the loudest near it, orbiting it), scatter when a being is hit, swim
 * faster and pulse brighter with the output's level. CR_N of them, a few pixels each, stepped once a drawn frame from
 * what the UI already reads (the parts' peaks, the scope): nothing in the audio path. */
#define ECO_N NTRK
static const int16_t ECO_PX[ECO_N] = {44, 120, 196, 64, 146, 206}, ECO_PY[ECO_N] = {34, 28, 40, 84, 78, 86};   /* (where they
                                                         * start, as 0.8.4's HOME) */
static struct {
    int32_t en[ECO_N], pen[ECO_N];                      /* each track's level now (0..256, slewed), the frame before */
    int32_t sx[ECO_N], sy[ECO_N], vx[ECO_N], vy[ECO_N];   /* the swim: where it is in the panel and its speed, Q8 px */
    int32_t r[ECO_N];                                   /* its radius now (Q4 px: swollen, pulsing) */
    uint32_t tm[ECO_N];                                 /* each being's time (it runs with its sound) */
    uint32_t f0;
    uint8_t init;
} eco;
static int eco_off(uint32_t c) { return trk[c].p[P_MUTE] || c >= trk_live; }   /* (muted, or asleep: TRACKS 4) */
static const genome_t *eco_genome(uint32_t c)
{
    uint32_t e = trk[c].eng_req % NENGINES;
    return e == ENGI_DRUM ? &LANE_G_HOME : &GENOME[EGEN[e]];
}
/* track c's features: its genome, its HOME knobs, excited by its level k-fold (ui_being.c) */
static void eco_feat(uint32_t c, bfeat_t *f, int32_t k)
{
    uint8_t ids[4];
    uint32_t v, e = trk[c].eng_req % NENGINES;
    for (v = 0; v < 4u; v++) ids[v] = (uint8_t)ENGINES[e]->knob[v];
    bg_feat(&trk[c], eco_genome(c), ids, f);
    bg_excite(f, eco.en[c], k);
}
/* (JIANT 0.5, as 0.8.4's HOME: graph_ecosys's life) the beings once a drawn frame (dt frames since the last): each
 * track's level (the part's peak, slewed: a hit at once, a quick fall), its time, its size (it pulses, and swells up to
 * ~2.2 x; the selected one larger), its swim over the whole panel (wandering, bouncing off the walls, a hit kicking it
 * on up to ~7 x). Still with ANIM OFF */
/* (0.8.9) the beings' water: the panel (COLORS 232 x 140, BOTTOM 240 x 148), HUD's HOME the whole screen; its floor, the
 * numbers' line under it (HUD: the numbers only in the middle, the floor near the screen's foot) */
static int32_t eco_h(void) { return hud_home() ? 240 : (int32_t)graph_h(); }
static int32_t eco_bot(void) { return eco_h() - (hud_home() ? 6 : 18); }
static uint32_t eco_step(void)
{
    uint32_t c, k, dt = ui.frame - eco.f0;
    int32_t bot = eco_bot();                            /* (the panel's floor, the number's line under it: 0.8.4's 104 of
                                                         * 122, the lens's 140 its 122) */
    if (!eco.init) {
        eco.init = 1;
        for (c = 0; c < ECO_N; c++) {
            eco.sx[c] = ECO_PX[c] << 8; eco.sy[c] = ECO_PY[c] << 8;
            eco.vx[c] = (c & 1u) ? 90 : -110; eco.vy[c] = (c & 2u) ? 70 : -60;
        }
    }
    if (dt > 8u) dt = 8u;
    if (ui_prefs & PREF_ANIM_OFF) dt = 0;
    eco.f0 = ui.frame;
    for (c = 0; c < ECO_N; c++) {
        track_t *t = &trk[c];
        int32_t v = t->peak * 256 / 7000, r = (c == song.sel ? 17 : 12) * 16, rad, sp;
        if (v > 256) v = 256;
        eco.pen[c] = eco.en[c];
        if (dt) {                                       /* (once a frame: a redraw in the same frame draws the same) */
            t->peak = 0;
            eco.en[c] = v > eco.en[c] ? v : eco.en[c] + (v - eco.en[c]) / 4;
        }
        if (eco.en[c] > 2) eco.tm[c] += dt * (uint32_t)(200 + eco.en[c] * 5);
        else eco.tm[c] += dt * 60u;                     /* (silent: it still breathes, slowly) */
        r = r * (256 + eco.en[c] * 5 / 4 + (bg_sin(eco.tm[c] / 3u) >> 11)) / 256;
        eco.r[c] = r;
        rad = r >> 4;
        sp = 256 + eco.en[c] * 6;
        for (k = 0; k < dt; k++) {                      /* swimming in the panel, bouncing off its walls, wandering */
            eco.vx[c] += bg_sin(ui.frame * 211u + c * 16000u + k * 977u) >> 12;
            eco.vy[c] += bg_sin(ui.frame * 157u + c * 23000u + k * 613u) >> 12;
            eco.vx[c] = clamp(eco.vx[c], -140, 140);
            eco.vy[c] = clamp(eco.vy[c], -110, 110);
            eco.sx[c] += eco.vx[c] * sp >> 8;
            eco.sy[c] += eco.vy[c] * sp >> 8;
            if (eco.sx[c] < (rad + 8) << 8) { eco.sx[c] = (rad + 8) << 8; eco.vx[c] = -eco.vx[c]; }
            if (eco.sx[c] > (232 - rad) << 8) { eco.sx[c] = (232 - rad) << 8; eco.vx[c] = -eco.vx[c]; }
            if (eco.sy[c] < (rad + 4) << 8) { eco.sy[c] = (rad + 4) << 8; eco.vy[c] = -eco.vy[c]; }
            if (eco.sy[c] > (bot - rad) << 8) { eco.sy[c] = (bot - rad) << 8; eco.vy[c] = -eco.vy[c]; }
        }
    }
    return dt;
}
/* track c's being at (cx, cy) Q4, radius rq Q4, as HOME draws it (the most excited); asleep a dim outline */
static void eco_being(uint32_t c, int32_t cx, int32_t cy, int32_t rq)
{
    const genome_t *g = eco_genome(c);
    bfeat_t f;
    eco_feat(c, &f, 4);
    if (eco_off(c))
        bg_outline(g, &f, cx, cy, rq, 1, eco.tm[c], ux.mono || settings.palette == UI_BW_INDEX ? ux_gray(ux_luma(T_DIM) >> 3) : T_DIM);
    else
        being_lines(g, &f, cx, cy, rq, eco.en[c], eco.tm[c] + c * 20000u);
}

/* ------------------------------------------------------------------------------------------------ the critters --- */
#define CR_N 36u
enum { CR_CILIATE, CR_SPORE, CR_CELL, CR_FLAG, CR_DIATOM, CR_KINDS };
/* each kind's look, four phases of a 5 x 5 sprite (bit y * 5 + x): a ciliate's cilia beating, a spore's spines
 * turning, a cell's ring breathing round its nucleus, a flagellate's tail waving, a diatom's rod turning */
static const uint32_t CR_SPR[CR_KINDS][4] = {
    {0x008B9CAu, 0x00539D1u, 0x00539CAu, 0x008B9D1u},
    {0x0405404u, 0x1101011u, 0x0451144u, 0x1101011u},
    {0x0E8D62Eu, 0x00729C0u, 0x0E8D62Eu, 0x0EAC62Eu},
    {0x0045C60u, 0x0001D70u, 0x0083C60u, 0x0001F60u},
    {0x0007C00u, 0x0111110u, 0x0421084u, 0x1041041u}};
static struct {
    int16_t x[CR_N], y[CR_N], vx[CR_N], vy[CR_N];     /* Q4 px; Q4 px a frame */
    uint8_t ph[CR_N];
    uint8_t init;
} cr;
/* one drawn frame: dt frames on; the beings' centres bx / by (Q4), their levels eco.en; E the output (0..256). Each
 * critter wanders, is drawn to the loudest being near it and circles it, is thrown off by a hit */
static void cr_step(uint32_t dt, const int32_t *bx, const int32_t *by, uint32_t nb, int32_t E)
{
    uint32_t i, c, k;
    int32_t vmax = 10 + E / 10;                         /* (faster with the sound) */
    if (!cr.init) {
        uint32_t r = 0x2468ACE1u;
        for (i = 0; i < CR_N; i++) {
            r = r * 1664525u + 1013904223u;
            cr.x[i] = (int16_t)((8u + (r >> 8) % 224u) * 16u);
            cr.y[i] = (int16_t)((4u + (r >> 20) % (uint32_t)(eco_h() - 8)) * 16u);
            cr.ph[i] = (uint8_t)r;
        }
        cr.init = 1;
    }
    for (k = 0; k < dt; k++)
        for (i = 0; i < CR_N; i++) {
            int32_t ax = bg_sin((uint32_t)cr.ph[i] * 257u + i * 4099u + ui.frame * (40u + i)) >> 13;   /* (a wander) */
            int32_t ay = bg_sin((uint32_t)cr.ph[i] * 263u + i * 7919u + ui.frame * (33u + i)) >> 13;
            int32_t best = 0, tx = 0, ty = 0;
            for (c = 0; c < nb; c++) {
                int32_t dx = (bx[c] - cr.x[i]) >> 4, dy = (by[c] - cr.y[i]) >> 4, d2 = dx * dx + dy * dy + 200;
                int32_t w = eco_off(c) ? 0 : eco.en[c] * 4000 / d2;
                if (eco.en[c] - eco.pen[c] > 48 && d2 < 3800) {   /* (a hit: it throws them off) */
                    ax -= dx * 2;
                    ay -= dy * 2;
                }
                if (w > best) { best = w; tx = dx; ty = dy; }
            }
            if (best > 6) {                             /* toward it, and round it once near (an orbit, not into it) */
                int32_t s = (i & 1u) ? 1 : -1, near = tx * tx + ty * ty < 22 * 22;
                ax += near ? (-ty * s - tx / 2) / 8 : tx / 6;
                ay += near ? (tx * s - ty / 2) / 8 : ty / 6;
            }
            cr.vx[i] = (int16_t)clamp((cr.vx[i] * 15 >> 4) + ax, -vmax, vmax);
            cr.vy[i] = (int16_t)clamp((cr.vy[i] * 15 >> 4) + ay, -vmax, vmax);
            cr.x[i] = (int16_t)clamp(cr.x[i] + cr.vx[i], 6 * 16, 234 * 16);   /* (the panel's walls: they slide along) */
            cr.y[i] = (int16_t)clamp(cr.y[i] + cr.vy[i], 3 * 16, (eco_h() - 3) * 16);
            cr.ph[i] = (uint8_t)(cr.ph[i] + 9u + (i % CR_KINDS) * 3u + (uint32_t)E / 32u);
        }
}
/* the critters drawn: a sprite each (its phase), warm near a sounding being, the others the theme's tint, all brighter
 * with the output (E); the flagellates face their way */
static int32_t cr_x0 __attribute__((section(".pool")));   /* (0.8.9) HUD: the strip's left on the screen */
static void cr_draw(const int32_t *bx, const int32_t *by, uint32_t nb, int32_t E)
{
    uint32_t i, c, b;
    int grey = settings.palette == UI_GREY_INDEX || settings.palette == UI_BW_INDEX;
    for (i = 0; i < CR_N; i++) {
        int32_t x = (cr.x[i] >> 4) - 2, y = (cr.y[i] >> 4) - 2, near = 0, flip = i % CR_KINDS == CR_FLAG && cr.vx[i] < 0;
        if (y + 5 + cv_oy < 0 || y + cv_oy >= (int32_t)cv_h || x + 5 < cr_x0 || x >= cr_x0 + (int32_t)cv_w)
            continue;                                   /* (off the strip) */
        uint32_t spr = CR_SPR[i % CR_KINDS][(cr.ph[i] >> 6) & 3u];
        uint16_t col;
        for (c = 0; c < nb; c++) {
            int32_t dx = (bx[c] >> 4) - x, dy = (by[c] >> 4) - y;
            if (!eco_off(c) && dx * dx + dy * dy < 34 * 34 && eco.en[c] > near)
                near = eco.en[c];
        }
        col = near > 12 ? bg_col(70 + near * 150 / 256) : ux_mix(T_SURF, grey ? T_TEXT : T_THEME, 34 + E / 5 + (int32_t)(i % 3u) * 6);
        if (grey)                                       /* (GREY, MONO: its grey) */
            col = ux_gray(ux_luma(col) >> 3);
        for (b = 0; b < 25u; b++)
            if ((spr >> b) & 1u)
                cv_pset(x - cr_x0 + (flip ? 4 - (int32_t)(b % 5u) : (int32_t)(b % 5u)), y + (int32_t)(b / 5u), col);
    }
}

/* (0.8.5) a gauge, as the mock-up's: an arc of 270 degrees (from 7:30, clockwise) round (cx, cy) px, radius r, w px
 * thick, the value f (0..1000) in vc over the track tc; antialiased lines (og_line), 24 a turn */
static void lz_arc(int32_t cx, int32_t cy, int32_t r, int32_t w, int32_t f, uint16_t tc, uint16_t vc)
{
    int32_t k, i;
    for (k = 0; k < w; k++) {
        int32_t rr = (r - k) * 16, px = 0, py = 0;
        for (i = 0; i <= 18; i++) {                     /* (18 segments over 270 degrees) */
            uint32_t a = 0x6000u + (uint32_t)i * 0xC000u / 18u;   /* (from 135 degrees, y down) */
            int32_t x = cx * 16 + (og_cos(a) * rr >> 15), y = cy * 16 + (og_sin(a) * rr >> 15);
            if (i)
                og_line(px, py, x, y, i * 1000 <= f * 18 ? vc : tc);
            px = x;
            py = y;
        }
    }
}

/* ------------------------------------------------------------------------------------------------ the panels --- */
static uint32_t routes_count(void)                      /* the MOD slots whose SRC is a macro, every track */
{
    uint32_t i, k, n = 0;
    for (i = 0; i < NTRK; i++)
        for (k = 0; k < 4u; k++) {
            const int16_t *p = &trk[i].p[P_M1SRC + 3u * k];
            n += p[0] >= MS_M1 && p[0] < MS_M1 + 4 && p[1] && p[2];
        }
    return n;
}
/* ECO: 0.8.4's HOME (graph_ecosys) in the lens's panel: the cytoplasm's granules drifting, the critters, the six beings
 * swimming free over the whole panel, each with its MUTATE dots and its number under it (the selected one larger, its
 * number in the theme's colour). (0.8.9) Drawn into the canvas as the water's x0, y0 .. (eco_draw); SKIN HUD's HOME: the
 * water is the screen, every strip draws the part of it under it (eco_backdrop; a being off the strip skipped), the
 * numbers and the MACROS words in the middle strip only (words), clear of the floating ones */
static void eco_draw(int32_t x0, int32_t y0, int words)
{
    uint32_t c, k, dt = eco_step(), still = (ui_prefs & PREF_ANIM_OFF) != 0u;
    int32_t bx[ECO_N], by[ECO_N], E = bg_energy(), bot = eco_bot(), hud = hud_home();
    char b[16];
    cv_oy = -y0;
    for (k = 0; k < 22u; k++) {                         /* the cytoplasm: granules drifting */
        uint32_t h = k * 2654435761u;
        int32_t gx = 10 + (int32_t)((h >> 8) % 220u + ui.frame * (1u + (h >> 28)) / 6u) % 220;
        int32_t gy = 6 + (int32_t)(((h >> 16) % (uint32_t)(bot + 6)) + (still ? 0 : (uint32_t)(bg_sin((uint32_t)(ui.frame * 300u + h)) >> 13))) %
                         (uint32_t)(bot + 6);
        cv_rect(gx - x0, gy, 1 + (int32_t)(h >> 31), 1 + (int32_t)(h >> 31), ux_mix(T_SURF, T_THEME, 22 + (int32_t)(h >> 29) * 4));
    }
    for (c = 0; c < ECO_N; c++) {
        bx[c] = eco.sx[c] >> 4;
        by[c] = eco.sy[c] >> 4;
    }
    cr_step(dt, bx, by, ECO_N, E);
    cr_x0 = x0;
    cr_draw(bx, by, ECO_N, E);
    cr_x0 = 0;
    for (c = 0; c < ECO_N; c++) {
        int32_t rad = eco.r[c] >> 4, px = eco.sx[c] >> 8, py = eco.sy[c] >> 8, ny, reach = (rad >> 4) * 3 + 8;
        char num[2] = {(char)('1' + c), 0};
        if (py + reach < y0 || py - reach > y0 + (int32_t)cv_h || px + reach < x0 || px - reach > x0 + (int32_t)cv_w)
            continue;                                   /* (off the strip) */
        eco_being(c, bx[c] - x0 * 16, by[c], eco.r[c]);
        {   /* (0.6.43) MUTATE: a ring of 12 dots filling to its next change; a change: all lit */
            static const int8_t RX[12] = {0, 50, 87, 100, 87, 50, 0, -50, -87, -100, -87, -50};
            uint32_t pr = mut_progress(c), d, lit = pr == 0xFFFFu ? 0u : mut_flash[c] ? 12u : pr * 12u / 256u;
            int32_t rr2 = (rad >> 4) + 6;
            for (d = 0; pr != 0xFFFFu && d < 12u; d++) {
                int32_t dx = px + RX[d] * rr2 / 100, dy = py - RX[(d + 9u) % 12u] * rr2 / 100;
                cv_rect(dx - 1 - x0, dy - 1, 2, 2, d < lit ? (mut_flash[c] ? T_ACCENT : heat_col(160)) : T_RAISE);
            }
        }
        ny = clamp(py + rad / 2 + 4, 4, bot);
        if (words && (!hud || (ny >= (int32_t)graph_y() + (home_set() ? 12 : 2) && ny + 14 <= (int32_t)(graph_y() + graph_h()))))
            cv_text_in(px - 30 - x0, ny, 60, &AF_S, num, c == song.sel ? T_THEME : eco_off(c) ? T_DIM : hud ? T_TEXT : T_MID,
                       T_SURF);                         /* (0.9.0 HUD: TEXT, over the beings) */
    }
    if (words && home_set()) {                          /* MACROS: the routes, LATCH */
        int32_t y = hud ? (int32_t)graph_y() + 3 : 2;
        fmt_int(b, (int32_t)routes_count());
        str_cpy(b + str_len(b), " ROUTES", 8);
        cv_text(6, y, &AF_X, b, hud ? T_TEXT : T_DIM);
        cv_text_r(234, y, &AF_X, mac_latch ? "LATCH" : "HOLD LFO", mac_latch ? T_ACCENT : hud ? T_TEXT : T_DIM, T_SURF);
    }
    cv_oy = 0;
}
static void graph_eco(void) { eco_draw(0, 0, 1); }
static void eco_backdrop(int32_t x0, int32_t y0, int words) { eco_draw(x0, y0, words); }
/* BEING: the track's being large, its HOME knobs as organs at the corners */
static const int16_t ORG_X[4] = {30, 210, 30, 210}, ORG_Y[4] = {28, 28, 116, 116};
static void organ(uint32_t k, int32_t cx, int32_t cy)
{
    int16_t *vp;
    const param_desc_t *d = home_param(k, &vp);
    char v[12];
    const char *u;
    int32_t gx = ORG_X[k], gy = ORG_Y[k], hot = ui.hot_t && ui.hot_col == k;
    uint16_t c = hot ? T_ACCENT : T_THEME;
    og_quad_a(gx * 16, gy * 16, (gx + cx) * 8, gy * 16, (cx + (gx - cx) * 35 / 100) * 16, (cy + (gy - cy) * 35 / 100) * 16,
              ux_mix(T_SURF, c, 45), 12u, 256);
    og_node(gx * 16, gy * 16, 17, T_RAISE);
    lz_arc(gx, gy, 14, 3, clamp(RATIO(d, enum_rank(d, *vp)), 0, 1000), T_LINE, c);
    param_format(d, *vp, v, &u);
    if (u && u[0] && str_len(v) + str_len(u) <= 5u)     /* ("901Hz": its unit when it fits the organ) */
        str_cpy(v + str_len(v), u, sizeof v - str_len(v));
    GFX_HOOK_ALIGN(gx - 17, gy - 4, gx + 17, gy + 4, AL_HV, "BEING organ value centred");
    cv_clip_text(gx - HALF_UP(text_w(&AF_X, v) < 30 ? text_w(&AF_X, v) : 30), gy - 4 - AF_X_CAP_Y + 1, &AF_X, v, T_TEXT, T_RAISE, 30);
    GFX_HOOK_ALIGN(gx - 30, 0, gx + 30, 0, AL_H, "BEING organ name centred");
    cv_text_in(gx - 30, gy - 28, 60, &AF_X, d->label, hot ? T_ACCENT : T_MID, T_SURF);
}
static void graph_home_being(void)
{
    uint32_t dt = eco_step(), c = song.sel % NTRK, k;
    int32_t E = bg_energy(), cx = 120, cy = 68, bx = cx * 16, by = cy * 16;
    char b[16];
    if (home_set()) {                                   /* ROUTES: the being small, the macros and their routes */
        uint32_t m, i, j, row = 0;
        int32_t ry[12];
        cx = 128;
        cy = 66;
        bx = cx * 16;
        by = cy * 16;
        cr_step(dt, &bx, &by, 1, E);
        cr_draw(&bx, &by, 1, E);
        eco_being(c, bx, by, (13 * 16) * (256 + eco.en[c] * 5 / 4 + (bg_sin(eco.tm[c] / 3u) >> 11)) / 256);
        for (m = 0; m < 4u; m++) {
            int32_t y = 16 + (int32_t)m * 34, n = 0;
            char mn[3] = {'M', (char)('1' + m), 0};
            uint16_t col = ui.hot_t && ui.hot_col == m ? T_ACCENT : T_THEME;
            for (i = 0; i < NTRK; i++)
                for (j = 0; j < 4u && row < 12u; j++) {
                    const int16_t *p = &trk[i].p[P_M1SRC + 3u * j];
                    char e[20] = {'T', (char)('1' + i), ' ', 0};
                    int32_t ty;
                    if (p[0] != (int16_t)(MS_M1 + m) || !p[1] || !p[2] || n >= 3)
                        continue;
                    ty = 6 + (int32_t)row * 11;
                    ry[row++] = ty;
                    n++;
                    str_cpy(e + 3, mod_dst_name(&trk[i], p[1]), sizeof e - 3);
                    str_cpy(e + str_len(e), p[2] > 0 ? "+" : "", sizeof e - str_len(e));
                    fmt_int(e + str_len(e), p[2] * 100 / 64);
                    og_quad_a(34 * 16, y * 16, 120 * 16, y * 16, 168 * 16, (ty + 4) * 16, heat_col(70 + macro_v[m] * 150 / 127),
                              14u, 200);
                    cv_clip_text(171, ty, &AF_X, e, T_MID, T_SURF, 66);
                }
            og_node(22 * 16, y * 16, 12, T_RAISE);
            lz_arc(22, y, 10, 2, macro_v[m] * 1000 / 127, T_LINE, col);
            GFX_HOOK_ALIGN(12, y - 5, 32, y + 5, AL_HV, "ROUTES macro name in its knob");
            cv_text_in(12, y - 4, 20, &AF_X, mn, T_TEXT, T_RAISE);
            if (!n)
                cv_text(40, y - 4 - AF_X_CAP_Y + 1, &AF_X, "NO ROUTE", T_DIM);
        }
        (void)ry;
        return;
    }
    cr_step(dt, &bx, &by, 1, E);
    cr_draw(&bx, &by, 1, E);
    for (k = 0; k < 4u; k++)
        organ(k, cx, cy);
    {   /* the being: graph_being's size, its HOME knobs, the track's own level */
        const genome_t *g = eco_genome(c);
        bfeat_t f;
        eco_feat(c, &f, 2);
        if (eco_off(c))
            bg_outline(g, &f, bx, by, (30 * 16), 1, eco.tm[c], T_DIM);
        else
            being_lines(g, &f, bx, by, (30 * 16) * (256 + eco.en[c] / 3) / 256, eco.en[c], eco.tm[c] + c * 20000u);
    }
    sound_name(&trk[c], b);
    GFX_HOOK_ALIGN(60, 0, 180, 0, AL_H, "BEING sound name centred");
    cv_text_in(60, 129, 120, &AF_X, b[0] ? b : ENGINES[trk[c].eng_req % NENGINES]->name, T_MID, T_SURF);
}
static void graph_home_lz(void)
{
    if (home_level() == 1u)
        graph_home_being();
    else
        graph_eco();
}
