/* SPDX-License-Identifier: GPL-3.0-only
 * JIANT (0.6.26): HOME + SELECT's DICE ALL, a song-idea generator. Included by ui_input.c (main loop only).
 *
 * Not a pattern per track at random but a piece: a STYLE first (its tempo, swing, scales, chord progressions, drum and
 * bass figures, how much space it leaves, which engines it likes for each role, its delay and reverb), after the
 * records the instrument loves: ROBOT POP (machines that sing: motorik 8ths, octave bass, clean arps), ELECTRO (broken
 * 808 kicks, 16th hats, a pumping one-note bass), BIG BEAT (breaks, acid bass with slides, stabs), ORGANIC (loops of
 * odd lengths turning against each other, a drone, a voice), ART ROCK (broken drums, 7th chords, a plucked figure with
 * holes), DUB PUNK (one drop, offbeat skanks, a walking bass), MINIMAL (space: a kick, a blip, one odd-length loop),
 * SYNTH POP (I V vi IV, a hook asked and answered, disco drums), IDM (euclidean and ratcheted drums, chance, chime arps
 * in odd lengths).
 * Then each track a ROLE (DRUMS, BASS, CHORDS, LEAD, ARP, TEXTURE; with TRACKS 4 the first four), an engine the style
 * likes for it (always another than it had, when it can), a factory preset whose name fits the role (BASS, PAD, LEAD,
 * PLUCK...) moved a little, its voice mode, gate, level and sends; and its part written from ONE harmony: the chords of
 * the progression (a bar each, or two), the bass on their roots, the chords voiced in the scale (7ths for some styles),
 * the arp over their tones, the lead a short motif repeated with a variation (A A' or a call and its answer), with whole
 * bars left silent. No automation and no modulation matrix beyond the macros' routes (parameters: no RAM of their own).
 * UNDO (GLO + G4) brings back what was there */

enum { RL_DRUM, RL_BASS, RL_PAD, RL_LEAD, RL_ARP, RL_TEX };
enum { DS_MOTORIK, DS_ELECTRO, DS_BREAK, DS_ORGANIC, DS_BROKEN, DS_DUB, DS_MINIMAL, DS_DISCO, DS_IDM };
enum { BS_OCT8, BS_PUMP, BS_ACID, BS_DRONE, BS_WALK, BS_BLIP, BS_DISCO, BS_IDM };
enum { PS_HOLD, PS_STAB, PS_PULSE, PS_SPARSE };
#define EG_A 0u                                         /* ANALOG */
#define EG_PH 2u                                        /* PHASE */
#define EG_LO 3u                                        /* LOFI */
#define EG_VO 5u                                        /* VOICE */
#define EG_WH 7u                                        /* WHEEL */
#define EG_NO 11u                                       /* NOISE */
#define EG_FM ENGI_FM6

typedef struct {
    const char *name;
    uint8_t bpm_lo, bpm_hi, swing;
    uint8_t scale[4];
    int8_t prog[3][4];                                  /* scale degrees, a chord each (127: the progression ends) */
    uint8_t drums, bass, pads;
    uint8_t space;                                      /* 0..100: how much is left silent */
    uint8_t seventh;                                    /* 1: the chords with their 7th */
    uint8_t odd;                                        /* 1: loops of odd lengths against each other */
    uint8_t e_bass[3], e_pad[3], e_lead[3], e_arp[3], e_tex[3];
    uint8_t rtype[2], dtime[2];                         /* reverb types, delay times (N_DTIME) */
    uint8_t bars;                                       /* 1: the chords change every bar, 2: every two */
} dstyle_t;

#define X 127
static const dstyle_t DSTYLE[] = {
    {"ROBOT POP", 116, 128, 0, {2, 3, 2, 9}, {{0, 0, 5, 3}, {0, 6, 5, 6}, {0, 3, X, X}}, DS_MOTORIK, BS_OCT8, PS_HOLD,
     30, 0, 0, {EG_A, EG_FM, EG_A}, {EG_A, EG_FM, EG_PH}, {EG_A, EG_VO, EG_LO}, {EG_A, EG_FM, EG_LO}, {EG_VO, EG_NO, EG_FM},
     {0, 1}, {5, 1}, 1},
    {"ELECTRO", 124, 134, 0, {2, 8, 7, 2}, {{0, 0, 0, 6}, {0, 1, 0, 6}, {0, X, X, X}}, DS_ELECTRO, BS_PUMP, PS_STAB,
     35, 0, 0, {EG_A, EG_FM, EG_PH}, {EG_A, EG_PH, EG_FM}, {EG_A, EG_LO, EG_PH}, {EG_A, EG_LO, EG_FM}, {EG_NO, EG_VO, EG_A},
     {1, 0}, {5, 4}, 1},
    {"BIG BEAT", 104, 124, 10, {12, 2, 4, 3}, {{0, 0, 3, 0}, {0, 6, 3, 4}, {0, 3, X, X}}, DS_BREAK, BS_ACID, PS_STAB,
     25, 0, 0, {EG_A, EG_PH, EG_A}, {EG_PH, EG_A, EG_WH}, {EG_A, EG_LO, EG_PH}, {EG_A, EG_FM, EG_LO}, {EG_NO, EG_VO, EG_LO},
     {1, 0}, {1, 4}, 1},
    {"ORGANIC", 88, 108, 18, {4, 5, 3, 9}, {{0, 4, X, X}, {0, X, X, X}, {0, 3, 0, 4}}, DS_ORGANIC, BS_DRONE, PS_SPARSE,
     45, 0, 1, {EG_FM, EG_VO, EG_PH}, {EG_WH, EG_VO, EG_FM}, {EG_VO, EG_FM, EG_WH}, {EG_FM, EG_PH, EG_LO}, {EG_VO, EG_NO, EG_WH},
     {4, 3}, {0, 5}, 2},
    {"ART ROCK", 78, 104, 6, {2, 7, 11, 3}, {{0, 5, 2, 6}, {0, 3, 5, 4}, {0, 2, 3, 6}}, DS_BROKEN, BS_WALK, PS_PULSE,
     35, 1, 0, {EG_A, EG_FM, EG_PH}, {EG_FM, EG_WH, EG_PH}, {EG_A, EG_VO, EG_PH}, {EG_FM, EG_PH, EG_A}, {EG_NO, EG_VO, EG_FM},
     {2, 3}, {4, 0}, 1},
    {"DUB PUNK", 72, 92, 8, {4, 1, 2, 12}, {{0, 3, 4, 3}, {0, 6, 3, 0}, {0, 4, X, X}}, DS_DUB, BS_WALK, PS_STAB,
     30, 0, 0, {EG_A, EG_PH, EG_FM}, {EG_WH, EG_A, EG_PH}, {EG_A, EG_LO, EG_WH}, {EG_FM, EG_A, EG_LO}, {EG_NO, EG_VO, EG_WH},
     {1, 0}, {0, 4}, 1},
    {"MINIMAL", 120, 127, 4, {2, 8, 3, 2}, {{0, X, X, X}, {0, 6, X, X}, {0, 1, X, X}}, DS_MINIMAL, BS_BLIP, PS_SPARSE,
     65, 0, 1, {EG_A, EG_FM, EG_A}, {EG_FM, EG_A, EG_PH}, {EG_FM, EG_A, EG_LO}, {EG_FM, EG_A, EG_PH}, {EG_NO, EG_FM, EG_VO},
     {0, 2}, {5, 2}, 2},
    {"SYNTH POP", 108, 122, 0, {1, 4, 9, 1}, {{0, 4, 5, 3}, {5, 3, 0, 4}, {0, 3, 4, 3}}, DS_DISCO, BS_DISCO, PS_PULSE,
     20, 1, 0, {EG_A, EG_FM, EG_A}, {EG_A, EG_FM, EG_WH}, {EG_A, EG_LO, EG_VO}, {EG_A, EG_FM, EG_LO}, {EG_VO, EG_WH, EG_FM},
     {2, 0}, {4, 1}, 1},
    {"IDM", 90, 150, 12, {9, 11, 7, 2}, {{0, 2, 5, 1}, {0, 5, 1, 4}, {0, 4, 2, X}}, DS_IDM, BS_IDM, PS_SPARSE,
     40, 1, 1, {EG_FM, EG_A, EG_PH}, {EG_FM, EG_PH, EG_LO}, {EG_FM, EG_LO, EG_A}, {EG_FM, EG_LO, EG_PH}, {EG_NO, EG_LO, EG_FM},
     {4, 2}, {3, 5}, 1},
};
#undef X
#define NDSTYLE (sizeof DSTYLE / sizeof DSTYLE[0])

static int32_t rr(int32_t lo, int32_t hi) { return lo + (int32_t)(rng() % (uint32_t)(hi - lo + 1)); }
static int chance(uint32_t pct) { return rng() % 100u < pct; }

/* the song's harmony: the progression and how long each chord lasts */
static struct { const int8_t *prog; uint32_t n, steps; int seventh; } dh;
static int32_t chord_at(uint32_t step) { return dh.prog[(step / dh.steps) % dh.n]; }

static void dstep_note(step_t *s, const int32_t *n, uint32_t cnt, uint32_t vel, uint32_t flags)
{
    uint32_t j;
    s->n = (uint8_t)cnt;
    for (j = 0; j < 4u; j++)
        s->note[j] = (uint8_t)(j < cnt ? clamp(n[j], 0, 127) : 0);
    s->time = ST_NOTE;
    s->flags = (uint8_t)flags;
    s->vel = (uint8_t)clamp((int32_t)vel, 40, 127);
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
    [DS_ELECTRO] = {"a......x..x.....", "....a.......a...", "....x.......x..x", "xaxaxaxaxaxaxaxa", "", "..............x.", "", "......x........."},
    [DS_BREAK]   = {"a.........x.....", "....a..g.g..a..g", "", "x.x.x.x.x.x.x.x.", "......x.......x.", "", "", ""},
    [DS_ORGANIC] = {"x.....x.........", "", "", "..g...g...g...g.", "", "......x....x....", "...x.....x..x...", "x..x..x...x..x.."},
    [DS_BROKEN]  = {"x..x....x.x.....", "....a..g.g..a...", "", "x.xgx.x.x.xgx.x.", ".......x........", "", "", ""},
    [DS_DUB]     = {"........a.......", "........x.......", "", "..x...x...x...x.", "......x.......x.", "", "...x.......x....", ""},
    [DS_MINIMAL] = {"x...x...x...x...", "", "....g.......g...", "..x...x...x...x.", "", "", "...x......x..x..", "......g........."},
    [DS_DISCO]   = {"x...x...x...x...", "....a.......a...", "....x.......x...", "x.x.x.x.x.x.x.x.", "..x...x...x...x.", "", "", ""},
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
            if (euclid(i, ks, 16, rot + 4u) && !(h & 1u)) h |= 1u << (chance(50) ? 1 : 2);
            if (euclid(i, kh + (b & 1u) * 2u - (b == 2u ? 3u : 0u), 16, b)) h |= 1u << (chance(20) ? 4 : 3);
            if (chance(10)) h |= 1u << (5u + rng() % 3u);
            if (!h) continue;
            s->time = ST_NOTE; s->hit = (uint8_t)h; s->acc = (uint8_t)(i % 4u == 0u ? h & 1u : 0u); s->vel = 100;
            if ((h & (1u << 3)) && chance(25)) step_set_ratchet(s, (uint32_t)rr(2, 4));
            if (!(h & 1u) && chance(30)) step_set_chance(s, (uint32_t)rr(45, 85));
        }
        return;
    }
    for (i = 0; i < len; i++) {
        step_t *s = &t->step[i];
        uint32_t h = 0, a = 0, ghost = 0, p = i % 16u, fill = len > 16u && i >= len - 4u;
        for (l = 0; l < NLANE; l++) {
            const char *b = DRUM_BAR[st->drums][l];
            char c = b && b[0] ? b[p] : '.';
            if (c == 'x' || c == 'a' || (c == 'g' && rng() % 4u)) h |= 1u << l;
            if (c == 'a') a |= 1u << l;
            if (c == 'g') ghost = 1;
        }
        if (fill) {                                     /* the fill: snare / toms over the last beat of the loop */
            if (chance(60)) h |= 1u << (chance(60) ? 1 : 5);
            h &= ~(1u << 4);
        }
        if (i >= 16u && !fill && chance(8)) h ^= 1u << (chance(50) ? 0 : 6);   /* bar 2: a hit moved */
        if (!h) continue;
        s->time = ST_NOTE; s->hit = (uint8_t)h; s->acc = (uint8_t)a; s->vel = (uint8_t)(ghost && !a ? 80 : 104);
        if (fill && chance(40)) step_set_ratchet(s, 2);
        if (st->drums == DS_ELECTRO && (h & (1u << 3)) && chance(12)) step_set_ratchet(s, 2);
    }
}

/* ---- the chords' notes: degree d of the scale from base, a triad (or with its 7th) */
static uint32_t dchord(const track_t *t, int32_t base, int32_t d, int seventh, int32_t *n)
{
    n[0] = scale_steps(t, base, d);
    n[1] = scale_steps(t, base, d + 2);
    n[2] = scale_steps(t, base, d + 4);
    if (seventh) { n[3] = scale_steps(t, base, d + 6); return 4; }
    return 3;
}

static void dice_bass(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = scale_snap(t, 36 + t->p[P_ROOT]), n[1];
    uint32_t i;
    static const uint8_t PUMP[16] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0};
    uint8_t acid[16];
    for (i = 0; i < 16u; i++)                           /* (BS_ACID: one bar's figure, kept: a riff, not noise) */
        acid[i] = (uint8_t)(i % 4u == 0u ? 1u : chance(45) ? 1u + rng() % 3u : 0u);
    for (i = 0; i < len; i++) {
        step_t *s = &t->step[i];
        int32_t d = chord_at(i), on = 0, oct = 0, step = 0, vel = 96;
        uint32_t fl = 0;
        switch (st->bass) {
        case BS_OCT8: on = i % 2u == 0u; oct = (i / 2u) & 1u; break;
        case BS_PUMP: on = PUMP[i % 16u]; oct = i % 16u == 11u; fl = i % 16u == 0u ? SF_ACCENT : 0; break;
        case BS_ACID: on = acid[i % 16u] != 0u; step = acid[i % 16u] == 2u ? 4 : acid[i % 16u] == 3u ? 6 : 0;
                      oct = acid[i % 16u] == 3u; fl = (i % 4u == 0u ? SF_ACCENT : 0) | (chance(20) ? SF_SLIDE : 0); break;
        case BS_DRONE: on = i % (dh.steps < 16u ? dh.steps : 16u) == 0u || (chance(8) && i % 2u == 0u); break;
        case BS_WALK: {                                 /* the root on 1, a fifth on 3, an approach into the next chord */
            uint32_t pp = i % dh.steps, last = dh.steps - 2u;
            on = pp == 0u || pp == 8u || pp == last || (chance(15) && i % 2u == 0u);
            step = pp == 8u ? 4 : pp == last ? chord_at(i + dh.steps) - d - 1 : 0;
            break;
        }
        case BS_BLIP: on = (i % 4u == 2u && chance(80)) || (i % 16u == 0u); vel = 90; break;
        case BS_DISCO: on = i % 2u == 1u || i % 4u == 0u; oct = i % 2u == 1u; break;
        default: on = chance(40); step = (int32_t)(rng() % 3u) * 2; oct = chance(15); break;   /* BS_IDM */
        }
        if (!on) continue;
        n[0] = scale_steps(t, base, d + step) + (oct ? 12 : 0);
        dstep_note(s, n, 1, (uint32_t)vel + (fl & SF_ACCENT ? 20u : 0u), fl);
        if (st->bass == BS_DRONE) dhold(t, i, 7, len);
        if (st->bass == BS_IDM && chance(12)) step_set_ratchet(s, 2);
    }
}

static void dice_pads(track_t *t, const dstyle_t *st, uint32_t len, int poly)
{
    int32_t base = scale_snap(t, 55 + t->p[P_ROOT]), n[4];
    uint32_t i;
    for (i = 0; i < len; i++) {
        step_t *s = &t->step[i];
        uint32_t pp = i % dh.steps, c;
        int on = 0;
        switch (st->pads) {
        case PS_HOLD: on = pp == 0u; break;
        case PS_STAB: on = (i % 4u == 2u && chance(70)) || (pp == 0u && chance(40)); break;   /* offbeats: skank, stab */
        case PS_PULSE: on = i % 2u == 0u && !(i % 8u == 6u && chance(50)); break;
        default: on = pp == 0u && (i == 0u || chance(65)); break;   /* PS_SPARSE: some chords (the first), the rest silence */
        }
        if (!on) continue;
        c = dchord(t, base, chord_at(i), dh.seventh, n);
        if (!poly) c = 1;
        dstep_note(s, n, c, 84, 0);
        if (st->pads == PS_HOLD || st->pads == PS_SPARSE) dhold(t, i, dh.steps - 1u, len);
        if (st->pads == PS_PULSE && chance(30)) dhold(t, i, 1, len);
    }
}

/* the lead: a motif of one bar (rhythm and degrees: chord tones on the strong steps), then the phrase A A' (the last
 * note moved) or A B (an answer), a bar left silent now and then; held notes */
static void dice_lead(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = scale_snap(t, 64 + t->p[P_ROOT]), n[1];
    int8_t deg[16];
    uint8_t on[16], hold[16];
    uint32_t i, bars = len / 16u ? len / 16u : 1u, answer = chance(50), space = st->space;
    int32_t d = rr(0, 4);
    for (i = 0; i < 16u; i++) {
        int strong = i % 4u == 0u;
        on[i] = (uint8_t)(chance(strong ? 70u - space / 3u : 30u - space / 5u));
        d = strong ? (int32_t)(rng() % 3u) * 2 : clamp(d + rr(-2, 2), -2, 9);   /* (strong: 0, 2, 4: the chord's) */
        deg[i] = (int8_t)d;
        hold[i] = (uint8_t)(on[i] && chance(35) ? rr(1, 3) : 0);
    }
    on[0] = 1;
    for (i = 0; i < len; i++) {
        uint32_t b = i / 16u, p = i % 16u;
        int32_t dd;
        if (!on[p]) continue;
        if (b && chance(space)) continue;               /* (a bar with holes) */
        if (bars >= 2u && b == bars - 1u && chance(space / 2u + 10u)) continue;   /* (the last bar: a breath) */
        dd = deg[p] + chord_at(i);
        if (b % 2u == 1u && answer && p >= 8u) dd += 2 - (int32_t)(rng() % 5u);   /* B: the answer moves */
        else if (b % 2u == 1u && p >= 12u) dd -= 1;                                 /* A': its end moved */
        n[0] = scale_steps(t, base, dd);
        dstep_note(&t->step[i], n, 1, p % 4u == 0u ? 104u : 90u, chance(12) ? SF_SLIDE : 0);
        if (hold[p]) dhold(t, i, hold[p], len);
    }
}

static void dice_arp(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = scale_snap(t, 60 + t->p[P_ROOT]), n[1], c[4];
    uint32_t i, k = (uint32_t)rr(5, 13), rate = st->drums == DS_IDM || chance(60) ? 1u : 2u, dir = rng() % 4u;
    uint32_t nc;
    for (i = 0; i < len; i++) {
        uint32_t idx = i / rate, j;
        if (i % rate) continue;
        if (!euclid(idx, k, 16, 0) && chance(70)) continue;   /* (holes in the figure) */
        nc = dchord(t, base, chord_at(i), dh.seventh, c);
        j = dir == 0u ? idx % nc : dir == 1u ? nc - 1u - idx % nc : dir == 2u ? (idx % (2u * nc - 2u) < nc ? idx % (2u * nc - 2u)
            : 2u * nc - 2u - idx % (2u * nc - 2u)) : rng() % nc;
        n[0] = c[j] + ((idx / nc) & 1u && chance(40) ? 12 : 0);
        dstep_note(&t->step[i], n, 1, idx % 4u == 0u ? 100u : 82u, 0);
        if (st->drums == DS_IDM && chance(10)) step_set_ratchet(&t->step[i], 2);
    }
}

static void dice_texture(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = scale_snap(t, 48 + t->p[P_ROOT]), n[1];
    uint32_t i;
    (void)st;
    for (i = 0; i < len; i += 16u) {                    /* a long note a bar, late or on time, the 5th or the root, or silence */
        uint32_t at = i + (uint32_t)rr(0, 2) * 4u;
        if (i && chance(40)) continue;
        n[0] = scale_steps(t, base, chance(50) ? 4 : 0);
        dstep_note(&t->step[at < len ? at : i], n, 1, 76, 0);
    }
    for (i = 0; i < len; i++)
        if (t->step[i].time == ST_NOTE) dhold(t, i, 11, len);
}

/* a factory preset of engine e whose name says the role (else any), loaded and moved a little */
static int dname_has(const char *s, const char *w)
{
    uint32_t i, j;
    for (i = 0; s[i]; i++) {
        for (j = 0; w[j] && s[i + j] == w[j]; j++) {}
        if (!w[j]) return 1;
    }
    return 0;
}
static uint32_t dice_fm6;                               /* FM6 tracks this dice: two at most (its CPU) */
static void dice_sound(track_t *t, uint32_t role, const uint8_t *el)
{
    static const char *const W[6][6] = {{0}, {"BASS", "SUB", "ACID", "GROWL", 0},
                                        {"PAD", "STR", "ORGAN", "CHOIR", "GOSPEL", "FLUTE"},
                                        {"LEAD", "LD", "SAW", "BRASS", "SQR", "WIRE"},
                                        {"PLUCK", "BELL", "KEY", "ARP", "MARIMBA", "TINE"},
                                        {"WIND", "RAIN", "WHISPER", "CHOIR", "METAL", "PAD"}};
    uint32_t e = el[rng() % 3u], tries, k, n, i, nm = 0;
    uint8_t m[24];
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
    for (i = 0; i < n && nm < sizeof m; i++)            /* the presets whose names fit the role */
        for (k = 0; k < 6u && W[role][k]; k++)
            if (dname_has(en->presets[i].name, W[role][k])) { m[nm++] = (uint8_t)i; break; }
    apply_preset_to(t, nm && chance(75) ? m[rng() % nm] : rng() % n);   /* (one in four: any of them, a surprise) */
    for (i = 0; i < 8u; i++) {                          /* (a sixth of each range: still that sound, a bit its own) */
        const param_desc_t *d = track_desc(t, P_E0 + i);
        int32_t span = (d->max - d->min) / 6;
        if (!d->label || d->label[0] == '-' || d->max <= d->min || d->fmt == F_ENUM) continue;
        t->p[P_E0 + i] = (int16_t)dice_by(t->p[P_E0 + i], d->min, d->max, span ? span : 1);
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
    dh.seventh = st->seventh;
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
        t->p[P_SGATE] = (int16_t)(r == RL_BASS ? (st->bass == BS_PUMP || st->bass == BS_BLIP ? 40 : 70) :
                                  r == RL_PAD ? 120 : r == RL_ARP ? 50 : r == RL_LEAD ? 90 : 120);
        t->p[P_SSWING] = 0;
        t->p[P_LEVEL] = (int16_t)(r == RL_DRUM ? 104 : r == RL_BASS ? 100 : r == RL_LEAD ? 96 : r == RL_TEX ? 74 : 86);
        t->p[P_PAN] = (int16_t)(r == RL_ARP ? rr(-30, -10) : r == RL_TEX ? rr(10, 30) : r == RL_PAD ? rr(-8, 8) : 0);
        t->p[P_REV] = (int16_t)(r == RL_PAD || r == RL_TEX ? rr(40, 75) : r == RL_LEAD ? rr(20, 45) : r == RL_DRUM ? rr(5, 20) : rr(0, 10));
        t->p[P_DLY] = (int16_t)(r == RL_LEAD || r == RL_ARP ? rr(15, 45) : r == RL_DRUM ? rr(0, 10) : 0);
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
    song.g[G_DFDBK] = (int16_t)rr(25, 70);
    song.g[G_DMIX] = (int16_t)rr(60, 105);
    song.g[G_DCOLOR] = (int16_t)rr(35, 85);
    song.g[G_DPIT] = st->drums == DS_IDM && chance(40) ? (chance(50) ? 12 : 7) : 0;
    song.g[G_DSPRY] = song.g[G_DPIT] ? (int16_t)rr(0, 40) : 0;
    song.g[G_RTYPE] = st->rtype[rng() & 1u];
    song.g[G_RSIZE] = (int16_t)rr(55, 105);
    song.g[G_RDAMP] = (int16_t)rr(25, 85);
    song.g[G_RMOD] = (int16_t)rr(0, 50);
    song.g[G_RPRE] = (int16_t)(song.g[G_RTYPE] == 3 ? rr(0, 7) : song.g[G_RTYPE] == 2 || song.g[G_RTYPE] == 4 ? rr(-12, 12) : rr(0, 30));
    song.g[G_CRATE] = (int16_t)rr(10, 60);
    song.g[G_CDEPTH] = (int16_t)rr(20, 80);
    song.g[G_STRN] = 0;
    song.g[G_SLPQ] = 1;
    macro_dice(3);
    ui_say("DICED: ", st->name);
    str_cpy(ui.msg2, "UNDO: GLO + G4", sizeof ui.msg2);
    ui.force = 1;
}
