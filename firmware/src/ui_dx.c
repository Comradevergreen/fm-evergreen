/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
 * JIANT (FELUCCA TONIC): EDIT > SOUND and SOUND 2, a sound of the DRUM-X kit (docs/TONIC-DRUMX.md). */
/* Shown in the EDIT family on a DRUM track (ui.c page_visible). They edit the section's kit
 * (drumx_voice.c dx_kit, saved with the section: project.c FUNA):
 *   SOUND    LANE (the sound: BD .. CB; a key struck on the track picks its lane too), SIDE (patch A or B), PTCH, PMOD
 *   SOUND 2  DCAY, NOIS, COLR, MODE (the oscillator's wave, the noise filter, SNAP: 24 combinations)
 *   SOUND 3  (JIANT 0.4) LANE again, PMOD's mode (DECAY LONG NOISE SINE), DRIVE (OFF / ON), the wave alone
 * The values are the side's; the MORPH plays between the two (EDIT 2 MRPH). The panel shows the kit's specimen
 * with the sound's organ singled out (ui_plates.c graph_p_drum: the iris, its node lit). */
static uint8_t dx_ui_seen = 0xFFu;                   /* the last key note followed */
static const char *const DX_LANE_ABBR[8] = {"BD", "SD", "CP", "CH", "OH", "TM", "RS", "CB"};

/* a key struck on the track (seq.c key_on: last_note) picks the sound it plays */
static void dx_follow(void)
{
    if (last_note != dx_ui_seen) {
        if (dx_ui_seen != 0xFFu)                     /* (the first look only takes note of it) */
            dx_ui_lane = (uint8_t)drum_lane(last_note);
        dx_ui_seen = last_note;
    }
}

/* the mode as one value 0..23: wave x 6 + filter x 2 + SNAP, and back */
static uint32_t dx_mode_ix(uint32_t m) { return (m & 3u) * 6u + ((m >> 2) & 3u) * 2u + ((m >> 4) & 1u); }
static uint8_t dx_mode_of(uint32_t ix) { return (uint8_t)(ix / 6u | (ix % 6u / 2u) << 2 | (ix & 1u) << 4); }
static void dx_mode_name(char *b, uint32_t m)        /* "FM BP+" (b: 8 bytes) */
{
    static const char *const W[4] = {"SIN", "FM", "MTL", "BEL"}, *const F[3] = {"LP", "BP", "HP"};
    str_cpy(b, W[m & 3u], 8);
    str_cpy(b + str_len(b), " ", 8 - str_len(b));
    str_cpy(b + str_len(b), F[((m >> 2) & 3u) % 3u], 8 - str_len(b));
    if ((m >> 4) & 1u)
        str_cpy(b + str_len(b), "+", 8 - str_len(b));
}

/* KNOB k (0..3) turned s on the page */
static void dx_knob(const page_t *pg, uint32_t k, int32_t s)
{
    dx_lane_t *L = &dx_kit[dx_ui_lane & 7u];
    uint8_t *v = dx_ui_side ? L->b : L->a;
    if (dx_pagen(pg) == 7u) {                           /* (JIANT 0.6.49) LAYER: the sound's LAYER and FEEDBK */
        uint8_t *ly = &dx_lay[dx_ui_lane & 7u];
        int32_t la = (int32_t)DX_LAY_L(*ly), fb = (int32_t)DX_LAY_FB(*ly);
        if (k == 0u)
            dx_ui_lane = (uint8_t)clamp((int32_t)dx_ui_lane + (s > 0 ? 1 : -1), 0, 7);
        else if (k == 1u)
            la = clamp(la + (s > 0 ? 1 : -1), 0, 15);
        else if (k == 2u)
            fb = clamp(fb + (s > 0 ? 1 : -1), 0, 15);
        *ly = (uint8_t)(la | fb << 4);
        return;
    }
    if (dx_pagen(pg) == 6u) {                           /* (JIANT 0.6.48) SEND: the sound's own bus and how much */
        uint8_t *sn = &dx_snd[dx_ui_lane & 7u];
        uint32_t b = DXS_BUS(*sn);
        int32_t a = DXS_AMT(*sn);
        if (k == 0u)
            dx_ui_lane = (uint8_t)clamp((int32_t)dx_ui_lane + (s > 0 ? 1 : -1), 0, 7);
        else if (k == 1u)
            b = (uint32_t)clamp((int32_t)b + (s > 0 ? 1 : -1), 0, 3);
        else if (k == 2u)
            a = clamp(a + accel(EN_K1 + k, s, 63), 0, 63);
        if (k == 1u && b && !a)
            a = 32;                                     /* (a bus picked: something to hear) */
        *sn = (uint8_t)(b << 6 | (uint32_t)a);
        return;
    }
    if (dx_pagen(pg) == 5u) {                           /* (JIANT 0.6.36) TONE: the kit's BP and FEEDBK */
        uint32_t bp = DX_TONE_BP(dx_tone), fb = DX_TONE_FB(dx_tone);
        if (k == 0u) bp = (uint32_t)clamp((int32_t)bp + (s > 0 ? 1 : -1), 0, 15);
        else if (k == 1u) fb = (uint32_t)clamp((int32_t)fb + (s > 0 ? 1 : -1), 0, 15);
        dx_tone = (uint8_t)(bp | fb << 4);
        return;
    }
    if (dx_pagen(pg) == 4u) {                           /* (JIANT 0.5.1) X-MOD: the kit's, not a sound's */
        static const uint8_t MX[4] = {8, 127, 4, 127};
        int32_t v0 = dx_mot[k & 3u];
        dx_mot[k & 3u] = (uint8_t)clamp(v0 + (MX[k & 3u] > 8 ? accel(EN_K1 + k, s, 127) : (s > 0 ? 1 : -1)), 0, MX[k & 3u]);
        return;
    }
    if (dx_pagen(pg) == 3u) {                           /* (JIANT 0.4) SOUND 3 */
        if (k == 0u)
            dx_ui_lane = (uint8_t)clamp((int32_t)dx_ui_lane + (s > 0 ? 1 : -1), 0, 7);
        else if (k == 1u)
            L->mode = (uint8_t)((L->mode & ~0x60u) | (uint32_t)clamp((int32_t)((L->mode >> 5) & 3u) + (s > 0 ? 1 : -1), 0, 3) << 5);
        else if (k == 2u)
            L->mode = (uint8_t)(s > 0 ? L->mode | DX_DRIVE : L->mode & ~DX_DRIVE);
        else
            L->mode = (uint8_t)((L->mode & ~3u) | (uint32_t)clamp((int32_t)(L->mode & 3u) + (s > 0 ? 1 : -1), 0, 3));
        return;
    }
    if (!dx_page2(pg)) {
        if (k == 0u)
            dx_ui_lane = (uint8_t)clamp((int32_t)dx_ui_lane + (s > 0 ? 1 : -1), 0, 7);
        else if (k == 1u)
            dx_ui_side = s > 0;
        else
            v[k == 2u ? DXP_PITCH : DXP_PMOD] = (uint8_t)clamp(v[k == 2u ? DXP_PITCH : DXP_PMOD] + accel(EN_K1 + k, s, 127),
                                                              0, 127);
        return;
    }
    if (k < 3u) {
        uint32_t i = k == 0u ? DXP_DECAY : k == 1u ? DXP_NOISE : DXP_COLOR;
        v[i] = (uint8_t)clamp(v[i] + accel(EN_K1 + k, s, 127), 0, 127);
    } else {
        L->mode = dx_mode_of((uint32_t)clamp((int32_t)dx_mode_ix(L->mode) + (s > 0 ? 1 : -1), 0, 23));
    }
}

/* the four cards */
static void dx_cards(const page_t *pg)
{
    const dx_lane_t *L = &dx_kit[dx_ui_lane & 7u];
    const uint8_t *v = dx_ui_side ? L->b : L->a;
    char val[12];
    dx_follow();
    if (dx_pagen(pg) == 7u) {                           /* (JIANT 0.6.49) LAYER */
        uint32_t ly = dx_lay[dx_ui_lane & 7u], la = DX_LAY_L(ly), fb = DX_LAY_FB(ly);
        draw_column(0, "LANE", DX_LANE_ABBR[dx_ui_lane & 7u], "", VAL(0u), (int32_t)(dx_ui_lane & 7u) * 1000 / 7,
                    ICON_X_PATTERN);
        fmt_int(val, (int32_t)la);
        draw_column(1, "LAYER", la ? val : "OFF", "", la ? VAL(1u) : T_DIM, (int32_t)la * 1000 / 15, ICON_WAVE);
        fmt_int(val, (int32_t)fb);
        draw_column(2, "FEEDBK", fb ? val : "OFF", "", fb ? VAL(2u) : T_DIM, (int32_t)fb * 1000 / 15, ICON_DRIVE);
        draw_column(3, "", "", "", T_THEME, -1, ICON_NONE);
        return;
    }
    if (dx_pagen(pg) == 6u) {                           /* (JIANT 0.6.48) SEND */
        static const char *const BUS[4] = {"OFF", "CHORUS", "DELAY", "REVERB"};
        uint32_t sn = dx_snd[dx_ui_lane & 7u], b = DXS_BUS(sn);
        draw_column(0, "LANE", DX_LANE_ABBR[dx_ui_lane & 7u], "", VAL(0u), (int32_t)(dx_ui_lane & 7u) * 1000 / 7,
                    ICON_X_PATTERN);
        draw_column(1, "SEND", BUS[b], "", b ? VAL(1u) : T_DIM, (int32_t)b * 333, ICON_MIX);
        fmt_int(val, DXS_AMT(sn) * 100 / 63);
        draw_column(2, "AMOUNT", b ? val : "--", b ? "%" : "", b ? VAL(2u) : T_DIM, DXS_AMT(sn) * 1000 / 63, ICON_LEVEL);
        draw_column(3, "", "", "", T_THEME, -1, ICON_NONE);
        return;
    }
    if (dx_pagen(pg) == 5u) {                           /* (JIANT 0.6.36) TONE */
        uint32_t bp = DX_TONE_BP(dx_tone), fb = DX_TONE_FB(dx_tone);
        if (bp) {                                       /* (its centre, as the filter sets it) */
            uint32_t hz = CUTOFF_HZ[78u + bp * 2u];
            if (hz < 1000u) { fmt_int(val, (int32_t)hz); }
            else { fmt_fix(val, (int32_t)(hz / 100u), 1); }
        } else str_cpy(val, "OFF", sizeof val);
        draw_column(0, "BP", val, bp ? (CUTOFF_HZ[78u + bp * 2u] < 1000u ? "Hz" : "kHz") : "", bp ? VAL(0u) : T_DIM,
                    (int32_t)bp * 1000 / 15, ICON_TONE);
        fmt_int(val, (int32_t)fb);
        draw_column(1, "FEEDBK", fb ? val : "OFF", "", fb ? VAL(1u) : T_DIM, (int32_t)fb * 1000 / 15, ICON_DRIVE);
        draw_column(2, "", "", "", T_THEME, -1, ICON_NONE);
        draw_column(3, "", "", "", T_THEME, -1, ICON_NONE);
        return;
    }
    if (dx_pagen(pg) == 4u) {                           /* (JIANT 0.5.1) X-MOD */
        static const char *const RT[9] = {"OFF", "4BAR", "2BAR", "1BAR", "1/2", "1/4", "1/8", "1/16", "1/32"};
        static const char *const SH[5] = {"SINE", "TRI", "SAW", "RAMP", "S&H"};
        draw_column(0, "RATE", RT[dx_mot[XM_RATE] % 9u], "", dx_mot[XM_RATE] ? VAL(0u) : T_DIM, dx_mot[XM_RATE] * 125,
                    ICON_RATE);
        fmt_int(val, dx_mot[XM_DPTH]);
        draw_column(1, "DEPTH", val, "", dx_mot[XM_DPTH] ? VAL(1u) : T_DIM, dx_mot[XM_DPTH] * 1000 / 127, ICON_LEVEL);
        draw_column(2, "SHAPE", SH[dx_mot[XM_SHPE] % 5u], "", VAL(2u), dx_mot[XM_SHPE] * 250, ICON_WAVE);
        fmt_int(val, dx_mot[XM_RAND]);
        draw_column(3, "RAND", val, "", dx_mot[XM_RAND] ? VAL(3u) : T_DIM, dx_mot[XM_RAND] * 1000 / 127, ICON_NOISE);
        return;
    }
    if (dx_pagen(pg) == 3u) {                           /* (JIANT 0.4) SOUND 3 */
        static const char *const PM[4] = {"DECAY", "LONG", "NOISE", "SINE"}, *const W[4] = {"SIN", "FM", "MTL", "BEL"};
        draw_column(0, "LANE", DX_LANE_ABBR[dx_ui_lane & 7u], "", VAL(0u), (int32_t)(dx_ui_lane & 7u) * 1000 / 7,
                    ICON_X_PATTERN);
        draw_column(1, "PMOD", PM[(L->mode >> 5) & 3u], "", VAL(1u), (int32_t)((L->mode >> 5) & 3u) * 333, ICON_ENV);
        draw_column(2, "DRIVE", L->mode & DX_DRIVE ? "ON" : "OFF", "", L->mode & DX_DRIVE ? VAL(2u) : T_DIM,
                    L->mode & DX_DRIVE ? 1000 : 0, ICON_DRIVE);
        draw_column(3, "WAVE", W[L->mode & 3u], "", VAL(3u), (int32_t)(L->mode & 3u) * 333, ICON_WAVE);
        return;
    }
    if (!dx_page2(pg)) {
        draw_column(0, "LANE", DX_LANE_ABBR[dx_ui_lane & 7u], "", VAL(0u), (int32_t)(dx_ui_lane & 7u) * 1000 / 7,
                    ICON_X_PATTERN);
        draw_column(1, "SIDE", dx_ui_side ? "B" : "A", "", VAL(1u), dx_ui_side ? 1000 : 0, ICON_MIX);
        fmt_int(val, v[DXP_PITCH]);
        draw_column(2, "PITCH", val, "", VAL(2u), v[DXP_PITCH] * 1000 / 127, ICON_PITCH);
        fmt_int(val, v[DXP_PMOD]);
        draw_column(3, "PMOD", val, "", VAL(3u), v[DXP_PMOD] * 1000 / 127, ICON_ENV);
        return;
    }
    fmt_int(val, v[DXP_DECAY]);
    draw_column(0, "DECAY", val, "", VAL(0u), v[DXP_DECAY] * 1000 / 127, ICON_DECAY);
    fmt_int(val, v[DXP_NOISE]);
    draw_column(1, "NOISE", val, "", VAL(1u), v[DXP_NOISE] * 1000 / 127, ICON_NOISE);
    fmt_int(val, v[DXP_COLOR]);
    draw_column(2, "COLOR", val, "", VAL(2u), v[DXP_COLOR] * 1000 / 127, ICON_TONE);
    dx_mode_name(val, L->mode);
    draw_column(3, "MODE", val, "", VAL(3u), (int32_t)dx_mode_ix(L->mode) * 1000 / 23, ICON_WAVE);
}
/* what the cards and the panel show changed */
static uint32_t dx_sig(void)
{
    const dx_lane_t *L = &dx_kit[dx_ui_lane & 7u];
    uint32_t h = (uint32_t)dx_ui_lane * 31u + dx_ui_side * 977u + L->mode * 131u, i;
    h ^= (uint32_t)dx_mot[0] << 24 ^ (uint32_t)dx_mot[1] << 16 ^ (uint32_t)dx_mot[2] << 8 ^ dx_mot[3];   /* (X-MOD) */
    h = (h ^ dx_tone) * 2654435761u;                    /* (TONE) */
    for (i = 0; i < DV_NLANE; i++)                      /* (0.6.48) the SENDs, (0.6.49) the LAYERs */
        h = (h ^ dx_snd[i] ^ (uint32_t)dx_lay[i] << 8) * 16777619u;
    for (i = 0; i < DXP_N; i++)
        h = (h ^ (uint32_t)L->a[i] << 8 ^ L->b[i]) * 16777619u;
    return h;
}
