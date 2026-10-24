/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
 * Portions Copyright (C) 2026 Juan Ignacio Antonucci (Juan Jiant) */
/* ADD (JIANT 0.9.9-snd5): additive, nine sine partials a voice whose ratios, levels and decays are the sound.
 * MODE picks the partials' ratios: HARM the first nine harmonics, ODD the odd ones (a clarinet's, a square's), WIDE
 * 1 2 3 4 6 8 11 13 16 (bright, airy), BELL a church bell's (hum, prime, tierce, quint, nominal ..), BAR a free bar's
 * (a vibraphone's, a xylophone's), DRUM a membrane's (Bessel's zeros). BRIGHT tilts the levels (1/k^2 .. flat), EVEN
 * the even partials against the odd, STRETCH spreads the ratios out or pulls them in (a stiff string, a bell bent),
 * PEAK a formant over the partials (FORMNT its height: a vowel, a resonance that LFO / ENV -> SHAPE sweep), DECAY each
 * partial's ring (127: held, an organ's, a pad's), DAMP the higher ones dying sooner (a pluck, a bell's tail). A
 * partial past ~19 kHz is silent (nothing aliases).
 *
 * Memory: none of its own. WHEEL's per voice state (eng_wheel.c drw_v: nine phases and gains a voice, by part) is a
 * part's either way (a part is WHEEL or ADD); the voice's age in blocks in its perc. Cost: WHEEL's (a table lookup and a
 * multiply-add a partial a sample); the levels and the decays once a block. A note from silence starts its partials in
 * phase (a bell's, a pluck's attack the same every time). */
static const char *const N_ADD_MODE[] = {"HARM", "ODD", "WIDE", "BELL", "BAR", "DRUM"};
enum { AD_HARM, AD_ODD, AD_WIDE, AD_BELL, AD_BAR, AD_DRUM, AD_N };
#define AD_R(x) ((uint16_t)((x) * 1024.0 + 0.5))         /* a ratio, Q10 */
static const uint16_t ADD_RATIO[AD_N][DRW_NP] = {
    {AD_R(1), AD_R(2), AD_R(3), AD_R(4), AD_R(5), AD_R(6), AD_R(7), AD_R(8), AD_R(9)},
    {AD_R(1), AD_R(3), AD_R(5), AD_R(7), AD_R(9), AD_R(11), AD_R(13), AD_R(15), AD_R(17)},
    {AD_R(1), AD_R(2), AD_R(3), AD_R(4), AD_R(6), AD_R(8), AD_R(11), AD_R(13), AD_R(16)},
    {AD_R(0.5), AD_R(1), AD_R(1.183), AD_R(1.506), AD_R(2), AD_R(2.514), AD_R(2.662), AD_R(3.011), AD_R(4.166)},
    {AD_R(1), AD_R(2.756), AD_R(5.404), AD_R(8.933), AD_R(13.344), AD_R(17.4), AD_R(9.85), AD_R(3.93), AD_R(15.1)},
    {AD_R(1), AD_R(1.594), AD_R(2.136), AD_R(2.296), AD_R(2.653), AD_R(2.918), AD_R(3.156), AD_R(3.501), AD_R(3.6)},
};
/* log2(k + 1), Q8: the tilt over the partials' ranks */
static const uint16_t ADD_LOG2[DRW_NP] = {0, 256, 406, 512, 594, 662, 719, 768, 811};
#define AD_TOP 1932735283u                               /* ~19.8 kHz as a phase increment: above it a partial is silent */

static void add_note_on(track_t *t, voice_t *v)
{
    uint32_t vi = (uint32_t)(v - t->v) % NVOICE, k;
    drw_vc_t *V = &drw_v[drw_part(t)][vi];
    if (!v->env) {                                       /* (from silence: in phase, its partials from nothing) */
        for (k = 0; k < DRW_NP; k++) {
            V->ph[k] = 0;
            V->gp[k] = 0;
        }
    }
    V->perc = 0;                                         /* (its age in blocks: the partials' decays from the note) */
    V->init = 1;
}

static void add_render(track_t *t, voice_t *v, int32_t *out, uint32_t n, const vmod_t *m)
{
    const int16_t *p = t->p;
    drw_vc_t *V = &drw_v[drw_part(t)][(uint32_t)(v - t->v) % NVOICE];
    const uint16_t *R = ADD_RATIO[(uint32_t)p[P_E0] % AD_N];
    int32_t acc[CTL], lv[DRW_NP], sum = 0, norm, slope = (127 - clamp(p[P_E1], 0, 127)) * 4, ev = clamp(p[P_E2], -64, 63);
    int32_t st = clamp(p[P_E3], -64, 63), form = clamp(p[P_E5], 0, 127), dec = clamp(p[P_E6], 0, 127), damp = clamp(p[P_E7], 0, 127);
    int32_t pos = clamp(p[P_E4] + ((m->shape - (64 << 8)) >> 8), 0, 127) * 2048 / 127;   /* (PEAK's rank, Q8: 0 .. 8) */
    uint32_t i, k, age = (uint32_t)V->perc, rate = dec >= 127 ? 0u : str_exp2n((uint32_t)dec * 2201u / 127u) * 19u / 65u;
    if (n > CTL)
        n = CTL;
    for (k = 0; k < DRW_NP; k++) {                       /* the levels: the tilt, EVEN, the formant */
        int32_t l = (int32_t)(str_exp2n((uint32_t)(slope * ADD_LOG2[k]) >> 8) >> 1), dx = (int32_t)k * 256 - pos;
        if (k && (k & 1u) && ev < 0) l = l * (64 + ev) >> 6;        /* (k odd: the 2nd, 4th .. partials) */
        if (k && !(k & 1u) && ev > 0) l = l * (64 - ev) >> 6;
        dx = dx < 0 ? -dx : dx;
        if (form && dx < 384)
            l += (l * form * (384 - dx)) >> 14;          /* (up to x4 at the peak, 1.5 ranks wide) */
        lv[k] = l;
        sum += l;
    }
    norm = (int32_t)((32768u * 32768u) / (uint32_t)(sum + 16384));   /* (the gains' sum: 0.67 one partial .. 0.95 nine: in
                                                         * phase at the note, their peaks add) */
    for (i = 0; i < n; i++)
        acc[i] = 0;
    for (k = 0; k < DRW_NP; k++) {
        uint32_t r = R[k], inc, ph = V->ph[k];
        int32_t g1, g0 = V->gp[k];
        if (st)                                          /* STRETCH: r (1 + st (r - 1) / 1024), at least a quarter */
            r = (uint32_t)clamp((int32_t)r + (int32_t)(((int32_t)r * ((st * ((int32_t)r - 1024)) >> 10)) >> 10), 256, 40000);
        if ((m->inc >> 16) * r >= (AD_TOP >> 16) << 10)  /* (above the top: silent) */
            g1 = 0;
        else {
            uint32_t rk = rate + rate * (uint32_t)damp * k / 128u, x;  /* (its decay: Q16 octaves a block; DAMP 127: the
                                                         * ninth 8 x faster) */
            x = !rk ? 0u : age >= 0xFFFFFFFFu / rk ? 0xFFFFu : (age * rk) >> 8;   /* (Q8 octaves down by now) */
            g1 = mulq15(mulq15(lv[k], norm), (int32_t)(str_exp2n(x) >> 1));
        }
        inc = (m->inc >> 10) * r;
        V->gp[k] = g1;
        if (!g0 && !g1) {
            V->ph[k] = ph + inc * n;
            continue;
        }
        if (g0 == g1) {
            for (i = 0; i < n; i++) {
                ph += inc;
                acc[i] += (SINE[ph >> 22] * g1) >> 15;
            }
        } else {                                         /* the gain ramped over the block */
            int32_t gq = g0 << CTL_LOG2, dg = g1 - g0;
            for (i = 0; i < n; i++) {
                ph += inc;
                gq += dg;
                acc[i] += (SINE[ph >> 22] * (gq >> CTL_LOG2)) >> 15;
            }
        }
        V->ph[k] = ph;
    }
    V->perc = (int32_t)(age < 0x7FFFFFu ? age + 1u : age);
    for (i = 0; i < n; i++)
        out[i] += voice_amp(acc[i] >> 1, m, i) << 1;
}

/* {MODE, BRIGHT, EVEN, STRETCH, PEAK, FORMNT, DECAY, DAMP}, {A D S R}, fenv, mono */
static const preset_t ADD_PRESETS[] = {
    {"ADD ORGAN", {AD_HARM, 70, 0, 0, 0, 0, 127, 0}, {0, 64, 127, 40}, 0, 0, FX(0, 15, 0, 30), PAT(5)},
    {"ADD SAW", {AD_HARM, 66, 0, 0, 0, 0, 127, 0}, {0, 64, 127, 35}, 0, 0, FX(0, 20, 0, 25), PAT(4)},
    {"HOLLOW", {AD_ODD, 58, 0, 0, 0, 0, 127, 0}, {4, 64, 127, 40}, 0, 0, FX(0, 10, 10, 35), PAT(5)},
    {"VOWEL AH", {AD_HARM, 84, 0, 0, 40, 110, 127, 0}, {10, 64, 127, 45}, 0, 0, FX(0, 20, 0, 40), PAT(5)},
    {"BELL", {AD_BELL, 100, 0, 0, 0, 0, 74, 60}, {0, 64, 127, 70}, 0, 0, FX(0, 0, 15, 55), PAT(6)},
    {"TUBULAR", {AD_BELL, 90, 0, 6, 70, 40, 88, 40}, {0, 64, 127, 80}, 0, 0, FX(0, 0, 10, 60), PAT(6)},
    {"VIBES", {AD_BAR, 72, 0, 0, 0, 0, 76, 74}, {0, 64, 127, 55}, 0, 0, FX(0, 25, 0, 40), PAT(6)},
    {"GLASS", {AD_WIDE, 112, 0, 20, 90, 60, 80, 34}, {0, 64, 127, 70}, 0, 0, FX(0, 20, 20, 60), PAT(5)},
    {"SKIN TOM", {AD_DRUM, 92, 0, 0, 0, 0, 46, 52}, {0, 64, 127, 30}, 0, 0, FX(0, 0, 0, 20), PAT(4)},
    {"AIR PAD", {AD_WIDE, 100, 20, -10, 100, 50, 127, 0}, {64, 64, 127, 80}, 0, 0, FX(0, 30, 15, 75), PAT(5)},
    {"PLUCK ADD", {AD_HARM, 112, 0, 4, 0, 0, 56, 92}, {0, 64, 127, 40}, 0, 0, FX(0, 0, 20, 30), PAT(3)},
    {"SUB ADD", {AD_ODD, 20, -40, 0, 0, 0, 127, 0}, {0, 64, 127, 25}, 0, 1, FX(0, 0, 0, 5), PAT(2)},
};

static const engine_t ENG_ADD = {
    .name = "ADD",
    .page_title = {"SPECTRUM", "SHAPE"},
    .edit = {
        {"MODE", F_ENUM, 0, AD_N - 1, AD_HARM, N_ADD_MODE, 0},
        {"BRIGHT", F_PCT, 0, 127, 70, 0, 0},
        {"EVEN", F_INT, -64, 63, 0, 0, 0},
        {"STRETCH", F_INT, -64, 63, 0, 0, 0},
        {"PEAK", F_PCT, 0, 127, 40, 0, 0},
        {"FORMNT", F_PCT, 0, 127, 0, 0, 0},
        {"DECAY", F_PCT, 0, 127, 127, 0, 0},
        {"DAMP", F_PCT, 0, 127, 0, 0, 0},
    },
    .presets = ADD_PRESETS,
    .npresets = NELEM(ADD_PRESETS),
    .note_on = add_note_on,
    .render = add_render,
    .knob = {P_E0, P_E1, P_E4, P_E6},
};
