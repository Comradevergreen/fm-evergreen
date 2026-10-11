/* SPDX-License-Identifier: GPL-3.0-only
 * JIANT (0.6.26; 0.6.32: darker): HOME + SELECT's DICE ALL, a song-idea generator. Included by ui_input.c (main loop
 * only).
 *
 * Not a pattern per track at random but a piece: a STYLE first (its tempo, swing, scales, chord progressions, drum and
 * bass figures, how much space it leaves, how it voices its chords, how its lead moves, which engines it likes for each
 * role, its delay and reverb). (0.6.32) The styles are the night's: DARK MINIMAL (a kick, a rolling sub, one filtered
 * chord on the offbeat), DUB TECHNO (minor 9th stabs drowned in delay), IDM (euclidean drums against held, filtered
 * chords in odd loops), DARK ELECTRO (broken 808s, a pumping bass, phrygian stabs), ACID NIGHT (a squelching line over
 * a hypnotic beat), COLD WAVE (machines in minor: motorik, octave bass), BROKEN (loops of odd lengths, a drone, a
 * voice), HYPNOTIC (one chord, an ostinato that never quite repeats), DEEP BREAKS (a break, an acid bass, stabs).
 * Minor modes only (MIN, DOR, PHRY, HARM, MEL, MPEN): no major, no mixolydian, no blues.
 * Then each track a ROLE (DRUMS, BASS, CHORDS, LEAD, ARP, TEXTURE; with TRACKS 4 the first four), an engine the style
 * likes for it (always another than it had, when it can), a factory preset whose name fits the role and is not a
 * bright one (no bells, brass, marimbas, organs), moved a little, then FILTERED: its cutoff down by role (the chords
 * and arps darkest, a slow LFO breathing on the chords, an envelope plucking the arps), its voice mode, gate, level
 * and sends; and its part written from ONE harmony anchored on the tonic in every register (0.6.32: the chords and the
 * lead were built from the fifth and the third: a v chord over a i bass): the chords of the progression voiced as
 * the style likes (7ths, 9ths without the 5th, in-scale fourths, suspended) and led to the nearest voicing in a
 * middle register, the bass on their roots, the arp low over their tones, the lead a motif of small steps (or an
 * ostinato, or a few long notes), with whole bars left silent. No automation and no modulation matrix beyond the
 * macros' routes (parameters: no RAM of their own). UNDO (GLO + G4) brings back what was there */

enum { RL_DRUM, RL_BASS, RL_PAD, RL_LEAD, RL_ARP, RL_TEX };
enum { DS_MOTORIK, DS_ELECTRO, DS_BREAK, DS_ORGANIC, DS_BROKEN, DS_DUB, DS_MINIMAL, DS_DEEP, DS_IDM };
enum { BS_OCT8, BS_PUMP, BS_ACID, BS_DRONE, BS_ROLL, BS_BLIP, BS_SUB, BS_IDM };
enum { PS_HOLD, PS_STAB, PS_PULSE, PS_SPARSE, PS_DUB };
enum { VC_7, VC_9, VC_QUART, VC_SUS, VC_MIX };          /* the chords' voicings */
enum { LM_MOTIF, LM_OSTINATO, LM_SPARSE };              /* how the lead moves */
#define EG_A 0u                                         /* ANALOG */
#define EG_PH 2u                                        /* PHASE */
#define EG_LO 3u                                        /* LOFI */
#define EG_VO 5u                                        /* VOICE */
#define EG_NO 11u                                       /* NOISE */
#define EG_FM ENGI_FM6
#define SC_MIN 2u
#define SC_DOR 3u
#define SC_MPEN 6u
#define SC_HARM 7u
#define SC_PHRY 8u
#define SC_MEL 11u

typedef struct {
    const char *name;
    uint8_t bpm_lo, bpm_hi, swing;
    uint8_t scale[4];
    int8_t prog[3][4];                                  /* scale degrees, a chord each (127: the progression ends) */
    uint8_t drums, bass, pads;
    uint8_t space;                                      /* 0..100: how much is left silent */
    uint8_t voicing, lead;
    uint8_t odd;                                        /* 1: loops of odd lengths against each other */
    uint8_t e_bass[3], e_pad[3], e_lead[3], e_arp[3], e_tex[3];
    uint8_t rtype[2], dtime[2];                         /* reverb types (no SHIMMER: too bright), delay times (N_DTIME) */
    uint8_t bars;                                       /* 1: the chords change every bar, 2: every two */
} dstyle_t;

#define X 127
static const dstyle_t DSTYLE[] = {
    {"DARK MINIMAL", 122, 128, 4, {SC_MIN, SC_PHRY, SC_DOR, SC_MIN}, {{0, X, X, X}, {0, 5, X, X}, {0, 1, X, X}},
     DS_MINIMAL, BS_ROLL, PS_DUB, 60, VC_9, LM_OSTINATO, 1,
     {EG_A, EG_FM, EG_A}, {EG_PH, EG_A, EG_FM}, {EG_A, EG_PH, EG_FM}, {EG_FM, EG_A, EG_PH}, {EG_NO, EG_VO, EG_PH},
     {0, 4}, {1, 4}, 2},
    {"DUB TECHNO", 116, 124, 6, {SC_MIN, SC_DOR, SC_MIN, SC_MPEN}, {{0, X, X, X}, {0, 3, X, X}, {0, 5, 3, X}},
     DS_DEEP, BS_SUB, PS_DUB, 55, VC_9, LM_SPARSE, 0,
     {EG_A, EG_FM, EG_A}, {EG_A, EG_PH, EG_FM}, {EG_PH, EG_A, EG_VO}, {EG_FM, EG_A, EG_PH}, {EG_NO, EG_VO, EG_A},
     {4, 0}, {0, 4}, 2},
    {"IDM", 86, 140, 10, {SC_DOR, SC_MEL, SC_MIN, SC_HARM}, {{0, 5, 3, 1}, {0, 3, 6, 4}, {0, 2, 5, X}},
     DS_IDM, BS_IDM, PS_SPARSE, 45, VC_MIX, LM_MOTIF, 1,
     {EG_FM, EG_A, EG_PH}, {EG_FM, EG_PH, EG_LO}, {EG_PH, EG_LO, EG_FM}, {EG_FM, EG_LO, EG_PH}, {EG_NO, EG_LO, EG_PH},
     {4, 3}, {4, 5}, 1},
    {"DARK ELECTRO", 126, 134, 0, {SC_PHRY, SC_MIN, SC_HARM, SC_PHRY}, {{0, 0, 1, 0}, {0, 5, 1, X}, {0, X, X, X}},
     DS_ELECTRO, BS_PUMP, PS_STAB, 40, VC_SUS, LM_OSTINATO, 0,
     {EG_A, EG_FM, EG_PH}, {EG_A, EG_PH, EG_FM}, {EG_A, EG_PH, EG_VO}, {EG_A, EG_LO, EG_FM}, {EG_NO, EG_VO, EG_A},
     {1, 0}, {4, 1}, 1},
    {"ACID NIGHT", 124, 132, 4, {SC_PHRY, SC_HARM, SC_MIN, SC_DOR}, {{0, X, X, X}, {0, 1, X, X}, {0, 5, X, X}},
     DS_MINIMAL, BS_ACID, PS_SPARSE, 55, VC_QUART, LM_SPARSE, 0,
     {EG_A, EG_A, EG_PH}, {EG_PH, EG_A, EG_FM}, {EG_A, EG_PH, EG_VO}, {EG_FM, EG_A, EG_PH}, {EG_NO, EG_VO, EG_PH},
     {0, 4}, {4, 1}, 2},
    {"COLD WAVE", 108, 120, 0, {SC_MIN, SC_DOR, SC_MPEN, SC_MIN}, {{0, 5, 6, 4}, {0, 3, 5, 6}, {0, 6, 5, X}},
     DS_MOTORIK, BS_OCT8, PS_HOLD, 35, VC_7, LM_MOTIF, 0,
     {EG_A, EG_FM, EG_A}, {EG_A, EG_PH, EG_FM}, {EG_A, EG_VO, EG_PH}, {EG_A, EG_FM, EG_LO}, {EG_VO, EG_NO, EG_PH},
     {0, 1}, {1, 4}, 1},
    {"BROKEN", 80, 100, 14, {SC_DOR, SC_MIN, SC_MEL, SC_DOR}, {{0, 3, X, X}, {0, X, X, X}, {0, 6, 3, X}},
     DS_BROKEN, BS_DRONE, PS_SPARSE, 50, VC_QUART, LM_MOTIF, 1,
     {EG_FM, EG_VO, EG_PH}, {EG_PH, EG_VO, EG_FM}, {EG_VO, EG_FM, EG_PH}, {EG_FM, EG_PH, EG_LO}, {EG_VO, EG_NO, EG_PH},
     {4, 3}, {0, 4}, 2},
    {"HYPNOTIC", 124, 130, 2, {SC_PHRY, SC_MIN, SC_DOR, SC_HARM}, {{0, X, X, X}, {0, X, X, X}, {0, 1, X, X}},
     DS_DEEP, BS_BLIP, PS_DUB, 55, VC_SUS, LM_OSTINATO, 1,
     {EG_A, EG_FM, EG_PH}, {EG_PH, EG_A, EG_FM}, {EG_A, EG_PH, EG_LO}, {EG_A, EG_FM, EG_PH}, {EG_NO, EG_PH, EG_VO},
     {0, 4}, {5, 1}, 2},
    {"DEEP BREAKS", 108, 122, 8, {SC_MIN, SC_HARM, SC_DOR, SC_PHRY}, {{0, 5, X, X}, {0, 6, 5, X}, {0, 3, 1, X}},
     DS_BREAK, BS_ACID, PS_STAB, 40, VC_9, LM_SPARSE, 0,
     {EG_A, EG_PH, EG_A}, {EG_PH, EG_A, EG_FM}, {EG_A, EG_PH, EG_VO}, {EG_A, EG_FM, EG_PH}, {EG_NO, EG_VO, EG_PH},
     {1, 4}, {4, 1}, 1},
};
#undef X
#define NDSTYLE (sizeof DSTYLE / sizeof DSTYLE[0])

static int32_t rr(int32_t lo, int32_t hi) { return lo + (int32_t)(rng() % (uint32_t)(hi - lo + 1)); }
static int chance(uint32_t pct) { return rng() % 100u < pct; }

/* the song's harmony: the progression, how long each chord lasts, the voicing and the last chord voiced (the next one
 * is led to it) */
static struct { const int8_t *prog; uint32_t n, steps, voicing; int32_t last[4]; uint32_t nlast; } dh;
static int32_t chord_at(uint32_t step) { return dh.prog[(step / dh.steps) % dh.n]; }
/* the tonic at or above lo: every part's degree 0 (0.6.32: the chords and the lead had the fifth and the third) */
static int32_t dtonic(const track_t *t, int32_t lo) { return lo + ((t->p[P_ROOT] - lo) % 12 + 12) % 12; }

static void dstep_note(step_t *s, const int32_t *n, uint32_t cnt, uint32_t vel, uint32_t flags)
{
    uint32_t j;
    s->n = (uint8_t)cnt;
    for (j = 0; j < 4u; j++)
        s->note[j] = (uint8_t)(j < cnt ? clamp(n[j], 0, 127) : 0);
    s->time = ST_NOTE;
    s->flags = (uint8_t)flags;
    s->vel = (uint8_t)clamp((int32_t)vel + rr(-6, 6), 40, 127);   /* (a human's hand: never twice the same) */
}
static void dstep_tie(step_t *s) { s->n = 0; s->time = ST_TIE; s->flags = 0; s->vel = 0; }
/* holds the note of step i over the next k steps (as far as the loop goes and the next note) */
static void dhold(track_t *t, uint32_t i, uint32_t k, uint32_t len)
{
    uint32_t j;
    for (j = 1; j <= k && i + j < len; j++) {
        if (t->step[i + j].time == ST_NOTE)
            break;
        dstep_tie(&t->step[i + j]);
    }
}
/* an euclidean rhythm: k hits spread over n steps, turned by rot */
static int euclid(uint32_t i, uint32_t k, uint32_t n, uint32_t rot)
{
    if (!n || !k) return 0;
    i = (i + rot) % n;
    return (i * k) % n < k;
}

/* ---- DRUMS (lanes: 0 BD, 1 SD, 2 CP, 3 CH, 4 OH, 5 TM, 6 RS, 7 CB): a style's bar as strings ('x' a hit, 'a' accented,
 * 'g' a ghost: a hit three times in four), bar 2 the same with a fill over its last beat */
static const char *const DRUM_BAR[][8] = {
    [DS_MOTORIK] = {"x...x...x...x...", "....x.......x...", "", "x.x.x.x.x.x.x.x.", "", "", "..x.....x.x.....", ""},
    [DS_ELECTRO] = {"a......x..x.....", "....a.......a...", "....x.......x..x", "xgxgxgxgxgxgxgxg", "", "..............x.", "", "......x........."},
    [DS_BREAK]   = {"a.........x.....", "....a..g.g..a..g", "", "x.x.x.x.x.x.x.x.", "......x.......x.", "", "", ""},
    [DS_ORGANIC] = {"x.....x.........", "", "", "..g...g...g...g.", "", "......x....x....", "...x.....x..x...", "x..x..x...x..x.."},
    [DS_BROKEN]  = {"x..x....x.x.....", "....a..g.g..a...", "", "x.xgx.x.x.xgx.x.", ".......x........", "", "", ""},
    [DS_DUB]     = {"........a.......", "........x.......", "", "..x...x...x...x.", "......x.......x.", "", "...x.......x....", ""},
    [DS_MINIMAL] = {"a...x...a...x...", "", "....g.......g...", "ggxgggxgggxgggxg", "..x...x...x...x.", "", "...x..g...x..x..", "......g........."},
    [DS_DEEP]    = {"a...x...a...x...", "", "............g...", "..x...x...x...x.", "..g...x...g...x.", "", ".g.....x.g....x.", ""},
};
static void dice_drums(track_t *t, const dstyle_t *st, uint32_t len)
{
    uint32_t i, l;
    if (st->drums == DS_IDM) {                          /* euclids against each other, ratchets, chance */
        uint32_t kb = (uint32_t)rr(3, 6), ks = (uint32_t)rr(2, 4), kh = (uint32_t)rr(7, 13), rot = (uint32_t)rr(0, 7);
        for (i = 0; i < len; i++) {
            step_t *s = &t->step[i];
            uint32_t h = 0, b = i / 16u;                /* (each bar its own: the hats denser or sparser, the kick turned) */
            if (euclid(i, kb, 16, rot + (b == 3u ? 2u : 0u))) h |= 1u << 0;
            if (euclid(i, ks, 16, rot + 4u) && !(h & 1u)) h |= 1u << (chance(50) ? 1 : 6);
            if (euclid(i, kh + (b & 1u) * 2u - (b == 2u ? 3u : 0u), 16, b)) h |= 1u << (chance(20) ? 4 : 3);
            if (chance(8)) h |= 1u << (5u + rng() % 3u);
            if (!h) continue;
            s->time = ST_NOTE; s->hit = (uint8_t)h; s->acc = (uint8_t)(i % 4u == 0u ? h & 1u : 0u);
            s->vel = (uint8_t)(h & 1u ? 104 : rr(70, 100));
            if ((h & (1u << 3)) && chance(25)) step_set_ratchet(s, (uint32_t)rr(2, 4));
            if (!(h & 1u) && chance(30)) step_set_chance(s, (uint32_t)rr(45, 85));
        }
        return;
    }
    for (i = 0; i < len; i++) {
        step_t *s = &t->step[i];
        uint32_t h = 0, a = 0, ghost = 0, p = i % 16u, fill = len > 16u && i >= len - 4u && st->drums != DS_DEEP;
        for (l = 0; l < NLANE; l++) {
            const char *b = DRUM_BAR[st->drums][l];
            char c = b && b[0] ? b[p] : '.';
            if (c == 'x' || c == 'a' || (c == 'g' && rng() % 4u)) h |= 1u << l;
            if (c == 'a') a |= 1u << l;
            if (c == 'g') ghost = 1;
        }
        if (fill) {                                     /* the fill: snare / toms over the last beat of the loop */
            if (chance(50)) h |= 1u << (chance(50) ? 1 : 5);
            h &= ~(1u << 4);
        }
        if (i >= 16u && !fill && chance(8)) h ^= 1u << (chance(50) ? 6 : 7);   /* bar 2: a percussion moved */
        if (!h) continue;
        s->time = ST_NOTE; s->hit = (uint8_t)h; s->acc = (uint8_t)a;
        s->vel = (uint8_t)(ghost && !a ? rr(62, 82) : rr(96, 110));
        if (fill && chance(40)) step_set_ratchet(s, 2);
        if ((st->drums == DS_ELECTRO || st->drums == DS_MINIMAL) && (h & (1u << 3)) && chance(10)) step_set_ratchet(s, 2);
        if (!(h & 1u) && (st->drums == DS_MINIMAL || st->drums == DS_DEEP) && chance(15)) step_set_chance(s, (uint32_t)rr(50, 85));
    }
}

/* ---- the chords: degree d's notes from the tonic t0 in the style's voicing, then led (each note an octave up or down)
 * to the nearest of the last chord inside [lo, lo + 16]: no leaps, a middle register (dark under a low-passed filter) */
static uint32_t dchord(const track_t *t, int32_t t0, int32_t d, uint32_t vc, int32_t *n)
{
    static const int8_t V[4][4] = {{0, 2, 4, 6}, {0, 2, 6, 8}, {0, 3, 6, 9}, {0, 3, 4, 8}};   /* 7th, 9th (no 5th),
                                                                                                * fourths, sus4 add9 */
    uint32_t j, c = 4, i;
    if (vc == VC_MIX) vc = rng() % 4u;
    for (j = 0; j < c; j++) n[j] = scale_steps(t, t0, d + V[vc][j]);
    if (vc == VC_QUART && chance(40)) c = 3;
    for (j = 0; j < c; j++) {                           /* into the window [lo, lo + 16] */
        while (n[j] < t0 - 2) n[j] += 12;
        while (n[j] > t0 + 14) n[j] -= 12;
    }
    if (dh.nlast) {                                     /* led: each note the octave nearest the last chord's middle */
        int32_t mid = 0;
        for (i = 0; i < dh.nlast; i++) mid += dh.last[i];
        mid /= (int32_t)dh.nlast;
        for (j = 0; j < c; j++) {
            if (n[j] - mid > 7 && n[j] - 12 >= t0 - 5) n[j] -= 12;
            else if (mid - n[j] > 7 && n[j] + 12 <= t0 + 17) n[j] += 12;
        }
    }
    for (i = 1; i < c; i++)                             /* (low to high) */
        for (j = i; j > 0 && n[j] < n[j - 1]; j--) { int32_t x = n[j]; n[j] = n[j - 1]; n[j - 1] = x; }
    for (j = 1; j < c; j++)                             /* (no unisons: a clash an octave up) */
        if (n[j] == n[j - 1]) n[j] += 12;
    for (j = 0; j < c; j++) dh.last[j] = n[j];
    dh.nlast = c;
    return c;
}

static void dice_bass(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = dtonic(t, 31), n[1];
    uint32_t i;
    static const uint8_t PUMP[16] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0};
    uint8_t acid[16], roll[16];
    for (i = 0; i < 16u; i++) {                         /* (one bar's figure, kept: a riff, not noise) */
        acid[i] = (uint8_t)(i % 4u == 0u ? 1u : chance(40) ? 1u + rng() % 4u : 0u);
        roll[i] = (uint8_t)(i % 4u == 2u ? 1u : i % 4u == 3u ? chance(35) : i % 4u == 1u ? chance(12) : 0u);
    }
    for (i = 0; i < len; i++) {
        step_t *s = &t->step[i];
        int32_t d = chord_at(i), on = 0, oct = 0, step = 0, vel = 96;
        uint32_t fl = 0, p = i % 16u;
        switch (st->bass) {
        case BS_OCT8: on = i % 2u == 0u; oct = (i / 2u) & 1u; break;
        case BS_PUMP: on = PUMP[p]; oct = p == 11u; fl = p == 0u ? SF_ACCENT : 0; break;
        case BS_ACID: on = acid[p] != 0u; step = acid[p] == 2u ? 1 : acid[p] == 3u ? 4 : acid[p] == 4u ? -1 : 0;
                      oct = acid[p] == 3u && chance(50); fl = (p % 4u == 0u ? SF_ACCENT : 0) | (chance(25) ? SF_SLIDE : 0); break;
        case BS_DRONE: on = i % (dh.steps < 16u ? dh.steps : 16u) == 0u || (chance(8) && i % 2u == 0u); break;
        case BS_ROLL: on = roll[p]; step = p % 4u == 3u && chance(25) ? 6 : 0; vel = p % 4u == 2u ? 100 : 82; break;
        case BS_SUB: on = p == 0u || p == 10u || (p == 6u && chance(40)); if (p == 10u && chance(40)) step = 4; break;
        case BS_BLIP: on = (p % 4u == 2u && chance(80)) || p == 0u; vel = 90; step = p == 14u && chance(40) ? 1 : 0; break;
        default: on = chance(35); step = (int32_t)(rng() % 3u) * 2 - 2; oct = chance(10); break;   /* BS_IDM */
        }
        if (!on) continue;
        n[0] = scale_steps(t, base, d + step) + (oct ? 12 : 0);
        dstep_note(s, n, 1, (uint32_t)vel + (fl & SF_ACCENT ? 18u : 0u), fl);
        if (st->bass == BS_DRONE) dhold(t, i, 7, len);
        if (st->bass == BS_SUB) dhold(t, i, p == 0u ? 3 : 1, len);
        if (st->bass == BS_IDM && chance(12)) step_set_ratchet(s, 2);
    }
}

static void dice_pads(track_t *t, const dstyle_t *st, uint32_t len, int poly)
{
    int32_t t0 = dtonic(t, 50), n[4];
    uint32_t i, c, last_d = 0xFFFFu;
    uint8_t dub[16];
    {   /* (PS_DUB: one bar's stab figure, kept: the first offbeat, and two or three of the others, a 16th late now and
         * then) */
        static const uint8_t AT[6] = {6, 10, 14, 7, 11, 13};
        uint32_t m = (uint32_t)rr(2, 3), j;
        for (i = 0; i < 16u; i++) dub[i] = (uint8_t)(i == 2u);
        for (j = 0; j < m; j++) dub[AT[rng() % 6u]] = 1;
    }
    dh.nlast = 0;
    for (i = 0; i < len; i++) {
        step_t *s = &t->step[i];
        uint32_t pp = i % dh.steps, p = i % 16u;
        int on = 0;
        switch (st->pads) {
        case PS_HOLD: on = pp == 0u; break;
        case PS_STAB: on = (p % 4u == 2u && chance(55)) || (pp == 0u && chance(40)); break;
        case PS_PULSE: on = i % 4u == 0u && !(p == 12u && chance(50)); break;
        case PS_DUB: on = dub[p]; break;
        default: on = pp == 0u && (i == 0u || chance(65)); break;   /* PS_SPARSE: some chords (the first), the rest silence */
        }
        if (!on) continue;
        if ((uint32_t)chord_at(i) != last_d || st->pads != PS_DUB) {
            c = dchord(t, t0, chord_at(i), dh.voicing, n);
            last_d = (uint32_t)chord_at(i);
        } else {                                        /* (the same chord: the same voicing, stab after stab) */
            for (c = 0; c < dh.nlast; c++) n[c] = dh.last[c];
        }
        if (!poly) c = 1;
        dstep_note(s, n, c, st->pads == PS_DUB ? (p == 2u ? 92u : 80u) : 82u, 0);
        if (st->pads == PS_HOLD || st->pads == PS_SPARSE) dhold(t, i, dh.steps - 1u, len);
        if (st->pads == PS_PULSE && chance(40)) dhold(t, i, 2, len);
    }
}

/* the lead (from the tonic around C4): LM_MOTIF a one-bar motif of small steps (chord tones on the strong steps), then
 * A A' or a call and its answer; LM_OSTINATO a cell of 2 to 4 notes on an euclidean rhythm, turning (Marc Houle, The
 * Hacker: hypnotic, not a tune); LM_SPARSE a few long notes; bars left silent */
static void dice_lead(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = dtonic(t, 57), n[1];
    int8_t deg[16];
    uint8_t on[16], hold[16];
    uint32_t i, bars = len / 16u ? len / 16u : 1u, answer = chance(50), space = st->space;
    int32_t d = rr(0, 2) * 2;
    if (st->lead == LM_OSTINATO) {
        static const int8_t CELL[6][4] = {{0, 0, 1, 0}, {0, 2, 1, 0}, {4, 2, 0, 1}, {0, -1, 0, 2}, {0, 4, 3, 1}, {2, 1, 0, -2}};
        const int8_t *cl = CELL[rng() % 6u];
        uint32_t k = (uint32_t)rr(4, 7), rot = (uint32_t)rr(0, 3), nc = (uint32_t)rr(2, 4), j = 0;
        for (i = 0; i < len; i++) {
            uint32_t b = i / 16u;
            if (!euclid(i % 16u, k, 16, rot)) continue;
            if (b && b % 4u == 3u && chance(space)) continue;   /* (the fourth bar: a breath) */
            n[0] = scale_steps(t, base, cl[j % nc] + (b % 4u == 2u && j % nc == nc - 1u ? 1 : 0));
            dstep_note(&t->step[i], n, 1, j % nc == 0u ? 100u : 84u, chance(15) ? SF_SLIDE : 0);
            j++;
        }
        return;
    }
    for (i = 0; i < 16u; i++) {
        int strong = i % 4u == 0u;
        if (st->lead == LM_SPARSE) on[i] = (uint8_t)(i == 0u || (i % 4u == 2u && chance(18)) || (i == 8u && chance(40)));
        else on[i] = (uint8_t)(chance(strong ? 60u - space / 3u : 22u - space / 5u));
        d = strong ? (int32_t)(rng() % 3u) * 2 : clamp(d + rr(-1, 1), -2, 6);   /* (strong: 0, 2, 4; then small steps) */
        deg[i] = (int8_t)d;
        hold[i] = (uint8_t)(on[i] && (st->lead == LM_SPARSE ? 1 : chance(45)) ? (st->lead == LM_SPARSE ? rr(3, 7) : rr(1, 3)) : 0);
    }
    on[0] = 1;
    for (i = 0; i < len; i++) {
        uint32_t b = i / 16u, p = i % 16u;
        int32_t dd;
        if (!on[p]) continue;
        if (b && chance(st->lead == LM_SPARSE ? space / 3u : space)) continue;   /* (a bar with holes) */
        if (bars >= 2u && b == bars - 1u && chance(space / 2u + 10u)) continue;   /* (the last bar: a breath) */
        dd = deg[p] + chord_at(i);
        if (b % 2u == 1u && answer && p >= 8u) dd += 1 - (int32_t)(rng() % 3u);   /* B: the answer moves a step */
        else if (b % 2u == 1u && p >= 12u) dd -= 1;                                 /* A': its end moved */
        n[0] = scale_steps(t, base, dd);
        dstep_note(&t->step[i], n, 1, p % 4u == 0u ? 100u : 86u, chance(12) ? SF_SLIDE : 0);
        if (hold[p]) dhold(t, i, hold[p], len);
    }
}

static void dice_arp(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t t0 = dtonic(t, 50), n[1], c[4];
    uint32_t i, k = (uint32_t)rr(5, 11), rate = st->drums == DS_IDM || chance(55) ? 1u : 2u, dir = rng() % 4u;
    uint32_t nc;
    dh.nlast = 0;
    for (i = 0; i < len; i++) {
        uint32_t idx = i / rate, j;
        if (i % rate) continue;
        if (!euclid(idx, k, 16, 0) && chance(75)) continue;   /* (holes in the figure) */
        nc = dchord(t, t0, chord_at(i), dh.voicing, c);
        j = dir == 0u ? idx % nc : dir == 1u ? nc - 1u - idx % nc : dir == 2u ? (idx % (2u * nc - 2u) < nc ? idx % (2u * nc - 2u)
            : 2u * nc - 2u - idx % (2u * nc - 2u)) : rng() % nc;
        n[0] = c[j] + ((idx / nc) & 1u && chance(12) ? 12 : 0);
        dstep_note(&t->step[i], n, 1, idx % 4u == 0u ? 96u : 78u, 0);
        if (st->drums == DS_IDM && chance(10)) step_set_ratchet(&t->step[i], 2);
    }
}

static void dice_texture(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = dtonic(t, 43), n[1];
    uint32_t i;
    (void)st;
    for (i = 0; i < len; i += 16u) {                    /* a long note a bar, late or on time, the 5th or the root, or silence */
        uint32_t at = i + (uint32_t)rr(0, 2) * 4u;
        if (i && chance(45)) continue;
        n[0] = scale_steps(t, base, chance(30) ? 4 : chance(30) ? 1 : 0);
        dstep_note(&t->step[at < len ? at : i], n, 1, 72, 0);
    }
    for (i = 0; i < len; i++)
        if (t->step[i].time == ST_NOTE) dhold(t, i, 11, len);
}

/* a factory preset of engine e whose name says the role and is not a bright one (else any not bright), loaded, moved a
 * little, and filtered for the night */
static int dname_has(const char *s, const char *w)
{
    uint32_t i, j;
    for (i = 0; s[i]; i++) {
        for (j = 0; w[j] && s[i + j] == w[j]; j++) {}
        if (!w[j]) return 1;
    }
    return 0;
}
static int dbright(const char *s)                       /* (0.6.32) the cheerful ones: never picked */
{
    static const char *const B[] = {"BELL", "BRASS", "MARIMBA", "ORGAN", "GOSPEL", "FLUTE", "8BIT", "JAZZ", "ARCADE",
                                    "RAVE", "ROCK", "SQR BASS", "BYTEBEAT", "STEP LEAD"};
    uint32_t i;
    for (i = 0; i < NELEM(B); i++)
        if (dname_has(s, B[i])) return 1;
    return 0;
}
static uint32_t dice_fm6;                               /* FM6 tracks this dice: two at most (its CPU) */
static void dice_sound(track_t *t, uint32_t role, const uint8_t *el)
{
    static const char *const W[6][6] = {{0}, {"BASS", "ACID", "GROWL", 0},
                                        {"PAD", "PWM", "STR", "RESO", "CHOIR", 0},
                                        {"SAW LEAD", "SYNC", "WIRE", "VOX", "WAVE LEAD", "RESO"},
                                        {"PLUCK", "FLOAT ARP", "SINE KEY", "TINE", "RESO", "BYTE TONE"},
                                        {"WIND", "RAIN", "WHISPER", "METAL", "BYTE BENT", "FLOAT FM"}};
    uint32_t e = el[rng() % 3u], tries, k, n, i, nm = 0, na = 0;
    uint8_t m[24], any[24];
    const engine_t *en;
    for (tries = 0; tries < 6u && (e == t->eng_req || (e == ENGI_FM6 && dice_fm6 >= 2u)); tries++)
        e = el[rng() % 3u];                             /* (another engine than it had, when it can; FM6 twice at most) */
    if (e == ENGI_FM6 && dice_fm6 >= 2u)
        e = EG_A;
    dice_fm6 += e == ENGI_FM6;
    set_engine_of(t, e);
    en = ENGINES[t->eng_req % NENGINES];
    n = en->npresets;
    if (!n) return;
    for (i = 0; i < n; i++) {                           /* the presets whose names fit the role; the not bright ones */
        if (dbright(en->presets[i].name)) continue;
        if (na < sizeof any) any[na++] = (uint8_t)i;
        for (k = 0; k < 6u && W[role][k]; k++)
            if (dname_has(en->presets[i].name, W[role][k]) && nm < sizeof m) { m[nm++] = (uint8_t)i; break; }
    }
    apply_preset_to(t, nm && chance(85) ? m[rng() % nm] : na ? any[rng() % na] : rng() % n);
    for (i = 0; i < 8u; i++) {                          /* (a sixth of each range: still that sound, a bit its own) */
        const param_desc_t *d = track_desc(t, P_E0 + i);
        int32_t span = (d->max - d->min) / 6;
        if (!d->label || d->label[0] == '-' || d->max <= d->min || d->fmt == F_ENUM) continue;
        t->p[P_E0 + i] = (int16_t)dice_by(t->p[P_E0 + i], d->min, d->max, span ? span : 1);
    }
    {   /* (0.6.32) filtered: the cutoff down by role (ANALOG its own filter, the others the part's), some resonance; the
         * chords breathing (a slow LFO on the cutoff), the arps plucked (the envelope opens it a little) */
        static const uint8_t CUT[6][2] = {{0, 0}, {44, 76}, {52, 74}, {64, 86}, {54, 76}, {50, 80}};
        int32_t cut = rr(CUT[role][0], CUT[role][1]), res = rr(role == RL_BASS ? 10 : 18, role == RL_ARP ? 60 : 45);
        if (role == RL_DRUM) return;
        if (e == EG_A) {
            t->p[P_E4] = (int16_t)(t->p[P_E4] < cut ? t->p[P_E4] : cut);
            t->p[P_E5] = (int16_t)clamp(t->p[P_E5], res, 90);
        } else {
            t->p[P_FTYPE] = (int16_t)(role == RL_TEX && chance(35) ? 2 : 0);   /* (a texture: now and then band-passed) */
            t->p[P_FCUT] = (int16_t)(role == RL_BASS ? cut + 22 : cut);       /* (a bass: only its fizz taken) */
            t->p[P_FRES] = (int16_t)res;
        }
        if (role == RL_PAD || role == RL_TEX) {
            t->p[P_LRATE] = (int16_t)rr(4, 22);
            t->p[P_LD_FLT] = (int16_t)rr(8, 24);
        }
        if (role == RL_ARP || (role == RL_PAD && chance(50))) t->p[P_ED_FLT] = (int16_t)rr(10, 32);
        if (role == RL_PAD) t->p[P_ATK] = (int16_t)clamp(t->p[P_ATK], 0, 40);
    }
}

/* (0.6.27) never the same piece twice: the moment of the press (the timer's ticks, a human's timing) stirred into the
 * random numbers each throw; a fingerprint of the last 32 pieces (every track's steps, the tempo, the style) kept, and a
 * piece that matches one is thrown again; the tempo always moves (unless MENU > BPM LOCK) */
static uint32_t dice_seen[32], dice_seen_n;
static uint32_t dice_print(void)
{
    uint32_t k, h = 2166136261u ^ (uint32_t)song.g[G_BPM];
    for (k = 0; k < NTRK; k++)
        h = (h ^ steps_sig(&trk[k]) ^ trk[k].eng_req) * 16777619u;
    return h;
}
static void dice_one(void);
static void dice_all(void)
{
    uint32_t tries, k, h = 0;
    undo_mark();
    for (tries = 0; tries < 4u; tries++) {
        rng_state ^= fm1_ticks() * 2654435761u + tries * 40503u;
        if (!rng_state) rng_state = 0x1234567u;        /* (xorshift: never 0) */
        dice_one();
        h = dice_print();
        for (k = 0; k < 32u && k < dice_seen_n && dice_seen[k] != h; k++) {}
        if (k == 32u || k >= dice_seen_n) break;        /* (new: kept) */
    }
    dice_seen[dice_seen_n++ % 32u] = h;
}
static void dice_one(void)
{
    const dstyle_t *st = &DSTYLE[rng() % NDSTYLE];
    uint32_t k, dr = NTRK, role[NTRK], next = 0, nlive = trk_live < NTRK ? trk_live : NTRK, lens[NTRK];
    int16_t root = (int16_t)rr(0, 11), scale = (int16_t)st->scale[rng() % 4u];
    int pick_arp = st->pads == PS_SPARSE || chance(40);
    uint32_t pr = rng() % 3u;
    dice_fm6 = 0;
    if (st->prog[pr][0] == 127) pr = 0;
    dh.prog = st->prog[pr];
    for (dh.n = 0; dh.n < 4u && dh.prog[dh.n] != 127; dh.n++) {}
    dh.steps = 16u * st->bars;
    if (dh.steps * dh.n > NSTEP) dh.steps = NSTEP / dh.n;   /* (the whole progression inside the 64 steps) */
    dh.voicing = st->voicing;
    dh.nlast = 0;
    for (k = 0; k < NTRK && dr == NTRK; k++)            /* the drums: the DRUM track (none: the fourth, or the last live) */
        if (drum_track(&trk[k])) dr = k;
    if (dr == NTRK) { dr = nlive > 3u ? 3u : nlive - 1u; set_engine_of(&trk[dr], ENGI_DRUM); }
    {   /* the roles in track order: BASS, CHORDS (or ARP), LEAD, then ARP (or CHORDS), TEXTURE */
        static const uint8_t RA[2][5] = {{RL_BASS, RL_PAD, RL_LEAD, RL_ARP, RL_TEX}, {RL_BASS, RL_ARP, RL_LEAD, RL_PAD, RL_TEX}};
        for (k = 0; k < NTRK; k++)
            role[k] = k == dr ? (uint32_t)RL_DRUM : RA[pick_arp][next++ < 5u ? next - 1u : 4u];
    }
    for (k = 0; k < NTRK; k++) {                        /* (the scale first: the parts are written in it) */
        trk[k].p[P_ROOT] = root;
        trk[k].p[P_SCALE] = scale;
    }
    sl_root = root; sl_scale = scale;
    for (k = 0; k < NTRK; k++) {
        track_t *t = &trk[k];
        uint32_t r = role[k], len = dh.steps * dh.n;
        if (len < 16u) len = 16u;
        if (st->odd && (r == RL_ARP || r == RL_TEX || (r == RL_LEAD && chance(40))))
            len = (uint32_t)rr(5, 7) * 2u + (chance(50) ? 1u : 0u);   /* (an odd loop against the others: 10 .. 15) */
        if (r == RL_DRUM) { if (chance(70)) sound_dice_of(t, 1); }
        else dice_sound(t, r, r == RL_BASS ? st->e_bass : r == RL_PAD ? st->e_pad : r == RL_LEAD ? st->e_lead :
                              r == RL_ARP ? st->e_arp : st->e_tex);
        t->p[P_ROOT] = root; t->p[P_SCALE] = scale;
        t->p[P_VOICE] = (int16_t)(r == RL_PAD ? V_POLY : r == RL_ARP ? (chance(50) ? V_POLY : V_MONO) : V_LEGATO);
        t->p[P_SGATE] = (int16_t)(r == RL_BASS ? (st->bass == BS_PUMP || st->bass == BS_BLIP || st->bass == BS_ROLL ? 40 : 70) :
                                  r == RL_PAD ? (st->pads == PS_DUB || st->pads == PS_STAB ? rr(25, 45) : 120) :
                                  r == RL_ARP ? rr(30, 55) : r == RL_LEAD ? 85 : 120);
        t->p[P_SSWING] = 0;
        t->p[P_LEVEL] = (int16_t)(r == RL_DRUM ? 106 : r == RL_BASS ? 100 : r == RL_LEAD ? 88 : r == RL_TEX ? 70 : 84);
        t->p[P_PAN] = (int16_t)(r == RL_ARP ? rr(-30, -10) : r == RL_TEX ? rr(10, 30) : r == RL_PAD ? rr(-8, 8) : 0);
        t->p[P_REV] = (int16_t)(r == RL_PAD || r == RL_TEX ? rr(45, 85) : r == RL_LEAD ? rr(30, 60) : r == RL_DRUM ? rr(4, 16) : rr(0, 8));
        t->p[P_DLY] = (int16_t)(r == RL_LEAD || r == RL_ARP ? rr(25, 60) : r == RL_PAD && st->pads == PS_DUB ? rr(55, 90) :
                                r == RL_DRUM ? rr(0, 8) : 0);   /* (0.6.32: the dub chords into the delay) */
        load_begin(t, LOAD_PAT);
        track_defaults_steps(t);
        switch (r) {
        case RL_DRUM: dice_drums(t, st, len); break;
        case RL_BASS: dice_bass(t, st, len); break;
        case RL_PAD: dice_pads(t, st, len, 1); break;
        case RL_LEAD: dice_lead(t, st, len); break;
        case RL_ARP: dice_arp(t, st, len); break;
        default: dice_texture(t, st, len); break;
        }
        t->p[P_SLEN] = (int16_t)len;
        load_end(t);
        pat_sig[k] = steps_sig(t);
        lens[k] = len;
    }
    (void)lens;
    if (!(ui_prefs & PREF_BPM_LOCK)) {                  /* (the tempo always moves: at least 3 BPM) */
        int32_t old = song.g[G_BPM], v = rr(st->bpm_lo, st->bpm_hi), t;
        for (t = 0; t < 8 && (v - old < 3 && old - v < 3); t++) v = rr(st->bpm_lo, st->bpm_hi);
        if (v - old < 3 && old - v < 3) v = old + (old < 150 ? 5 : -5);
        song.g[G_BPM] = (int16_t)v;
    }
    song.g[G_SWING] = (int16_t)clamp((int32_t)st->swing + rr(-4, 4), 0, 100);
    song.g[G_DTIME] = st->dtime[rng() & 1u];
    song.g[G_DFDBK] = (int16_t)(st->pads == PS_DUB ? rr(60, 85) : rr(30, 70));
    song.g[G_DMIX] = (int16_t)rr(70, 110);
    song.g[G_DCOLOR] = (int16_t)rr(28, 58);              /* (0.6.32: the repeats darker each time) */
    song.g[G_DPIT] = st->drums == DS_IDM && chance(40) ? (chance(50) ? -12 : 7) : 0;
    song.g[G_DSPRY] = song.g[G_DPIT] ? (int16_t)rr(0, 40) : 0;
    song.g[G_RTYPE] = st->rtype[rng() & 1u];
    song.g[G_RSIZE] = (int16_t)rr(55, 105);
    song.g[G_RDAMP] = (int16_t)rr(55, 100);
    song.g[G_RFILT] = (int16_t)rr(-40, -8);               /* (0.6.32: dark rooms) */
    song.g[G_RMOD] = (int16_t)rr(0, 50);
    song.g[G_RPRE] = (int16_t)(song.g[G_RTYPE] == 3 ? rr(0, 4) : song.g[G_RTYPE] == 2 || song.g[G_RTYPE] == 4 ? rr(-12, 0) : rr(0, 30));
    song.g[G_CRATE] = (int16_t)rr(10, 60);
    song.g[G_CDEPTH] = (int16_t)rr(20, 80);
    song.g[G_STRN] = 0;
    song.g[G_SLPQ] = 1;
    macro_dice(3);
    ui_say("DICED: ", st->name);
    str_cpy(ui.msg2, "UNDO: GLO + G4", sizeof ui.msg2);
    ui.force = 1;
}
