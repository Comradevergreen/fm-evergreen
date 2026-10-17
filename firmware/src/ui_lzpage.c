/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* (0.8.5) the lens panels of ENV, LFO . MOD, SCL . ARP and GLO (ui_lens.c), as the approved mock-up's; included by
 * ui_graph.c after ui_lzedit.c (ed_cap, ed_chip). The canvas is the lens's panel (240 x 140, x 4..236).
 *   ENV  TRACKS: the six tracks' envelopes as tiles (the selected one tinted, its live level a dot; DEST: the three
 *        amounts under each); ENV keeps graph_adsr / the DEST columns; STAGE: the envelope with the stage picked
 *        shaded, the stage's own curve large under it, its time */
/* track t's envelope in the box (x, y, w, h): attack straight, decay and release as three bends, sustain flat */
static void env_mini(const track_t *t, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t c, int32_t th)
{
    int32_t a = 2 + t->p[P_ATK] * (w / 4) / 127, d = 3 + t->p[P_DEC] * (w / 4) / 127, r = 3 + t->p[P_REL] * (w / 4) / 127;
    int32_t s = t->p[P_SUS] * h / 127, b = y + h, x1 = x + a, x3 = x + w - r, k, px = x1, py = y;
    cv_line_t(x, b, x1, y, c, th);
    for (k = 1; k <= 4; k++) {                          /* (1/2, 3/4, 7/8, 15/16 of the way) */
        int32_t xx = x1 + d * k / 4, yy = b - s - (h - s) / (1 << k);
        cv_line_t(px, py, xx, yy, c, th);
        px = xx;
        py = yy;
    }
    cv_line_t(px, py, x3, b - s, c, th);
    px = x3;
    py = b - s;
    for (k = 1; k <= 4; k++) {
        int32_t xx = x3 + r * k / 4, yy = b - (k == 4 ? 0 : s / (1 << k));
        cv_line_t(px, py, xx, yy, c, th);
        px = xx;
        py = yy;
    }
}
static int32_t env_now(const track_t *t)                /* the loudest voice's level now, 0..1000 (-1: none) */
{
    int32_t lv = -1;
    uint32_t i;
    for (i = 0; i < NVOICE; i++)
        if (t->v[i].stage && t->v[i].stage < 4u)
            lv = clamp((int32_t)(((int64_t)t->v[i].env * 1000) >> 24), lv, 1000);
    return lv;
}
static void graph_env_tracks(int dest)
{
    uint32_t c, k;
    for (c = 0; c < NTRK; c++) {
        const track_t *t = &trk[c];
        int32_t x = 4 + (int32_t)(c % 3u) * 78, y = 2 + (int32_t)(c / 3u) * 68, sel = c == song.sel, lv;
        uint16_t bg = sel ? T_TINT : T_RAISE, ink = c >= trk_live ? T_DIM : sel ? T_THEME : T_MID;
        char n[3] = {'T', (char)('1' + c), 0};
        cv_rect(x, y, 76, 66, bg);
        if (sel) {
            cv_rect(x, y, 76, 1, T_THEME); cv_rect(x, y + 65, 76, 1, T_THEME);
            cv_rect(x, y, 1, 66, T_THEME); cv_rect(x + 75, y, 1, 66, T_THEME);
        }
        cv_text(x + 4, y + 4 - AF_X_CAP_Y, &AF_X, n, ink);
        cv_clip_text(x + 22, y + 4 - AF_X_CAP_Y, &AF_X, ENGINES[t->eng_req % NENGINES]->name, T_DIM, bg, 50);
        env_mini(t, x + 5, y + 16, 66, dest ? 34 : 44, c >= trk_live ? T_DIM : settings.palette == UI_JIANT_INDEX ?
                 heat_col(150) : T_THEME, sel ? 2 : 1);
        if (!dest && (lv = env_now(t)) >= 0)
            og_node((x + 70) * 16, (y + 60 - lv * 44 / 1000) * 16, 2, T_TEXT);
        for (k = 0; dest && k < 3u; k++) {              /* FILTER PITCH SHAPE: a bar each from the middle */
            int32_t v = t->p[P_ED_FLT + k], w = v * 34 / 128;
            cv_rect(x + 5, y + 54 + (int32_t)k * 4, 66, 1, T_LINE);
            cv_rect(w < 0 ? x + 38 + w : x + 38, y + 53 + (int32_t)k * 4, w < 0 ? -w : w ? w : 1, 3,
                    k == 0u ? T_THEME : k == 1u ? T_ACCENT : T_MID);
        }
    }
}
static void graph_stage(const track_t *t)
{
    static const char *const SN[4] = {"ATTACK", "DECAY", "SUSTAIN", "RELEASE"};
    uint32_t sg = ui.stage & 3u, id = P_ATK + sg;
    int32_t a = 2 + t->p[P_ATK] * 56 / 127, d = 3 + t->p[P_DEC] * 56 / 127, r = 3 + t->p[P_REL] * 56 / 127;
    int32_t x0 = 10, x1 = x0 + a, x2 = x1 + d, x3 = 226 - r, sx[5] = {x0, x1, x2, x3, 226}, x, py = 0;
    int32_t s = t->p[P_SUS] * 1000 / 127, v = t->p[id] * 1000 / 127;
    const char *u = "";
    char b[24];
    cv_rect(sx[sg], 4, sx[sg + 1u] - sx[sg], 40, T_TINT);   /* the stage picked, on the whole envelope */
    env_mini(t, 10, 6, 216, 36, T_MID, 1);
    for (x = 0; x <= 216; x++) {                        /* its own shape, large: up, down to SUS, held, down to 0 */
        int32_t q = x * 1000 / 216, e = 1000 - (int32_t)((int64_t)(1000 - q) * (1000 - q) * (1000 - q) / 1000000), y;
        y = sg == 0u ? q : sg == 1u ? 1000 - (1000 - s) * e / 1000 : sg == 2u ? s : s - s * e / 1000;
        if (sg == 0u)                                   /* (ATK: slow or fast as it is) */
            y = 1000 - (int32_t)((int64_t)(1000 - q) * (1000 - q) / 1000) * (1000 - v / 2) / 1000 - (1000 - q) * v / 2000;
        y = 122 - y * 66 / 1000;
        if (x)
            cv_line_t(10 + x - 1, py, 10 + x, y, settings.palette == UI_JIANT_INDEX ? heat_y(y, 56, 122, T_THEME) : T_THEME, 2);
        py = y;
    }
    cv_rect(10, 124, 217, 1, T_RAISE);
    ed_cap(10, 50, SN[sg], T_THEME);
    param_format(&TP[id], t->p[id], b, &u);
    str_cpy(b + str_len(b), u, 24 - str_len(b));
    cv_text_r(226, 50 - AF_X_CAP_Y, &AF_X, b, T_TEXT, T_SURF);
    cv_text(10, 128 - AF_X_CAP_Y, &AF_X, "0", T_DIM);
    cv_text_r(226, 128 - AF_X_CAP_Y, &AF_X, sg == 2u ? "HELD" : b, T_DIM, T_SURF);
}
/* the lens's panel for ENV's page pg (1), else 0 (its own) */
static int graph_env_lz(const track_t *t, const page_t *pg)
{
    if (pg->graph == GR_STAGE)
        graph_stage(t);
    else if (pd_level() == 0u)
        graph_env_tracks(pg->id[0] != P_ATK);
    else
        return 0;
    return 1;
}
