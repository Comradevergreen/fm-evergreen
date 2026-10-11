/* SPDX-License-Identifier: GPL-3.0-only
 * JIANT (0.6.26; 0.6.32 darker; 0.6.34 a generator of pieces, not of conditions): HOME + SELECT's DICE ALL. Included
 * by ui_input.c (main loop only).
 *
 * Three choices make a piece, then guard rails keep it musical inside the chaos:
 *   GENRE   (dstyle_t) its tempo, scales, progressions, drum family (or none) and kits, bass figure and register, how
 *           its chords move, how its lead and its arp speak, which engines it likes for each role, its space (reverb,
 *           delay, how wet). The night's (DARK MINIMAL, DUB TECHNO, IDM, DARK IDM, DARK ELECTRO, ACID NIGHT, COLD WAVE,
 *           BROKEN, HYPNOTIC, DEEP BREAKS) and the others the engines can do: SOUNDTRACK (enveloping pads, no drums or a
 *           heartbeat), HARD TECHNO, INDIE POP, ELECTRO PUNK, NEO CLASSICAL (Reich's pulses and phases, Glass's
 *           additive arps, Part's tintinnabuli).
 *   ENERGY  0 SPACE (two or three things sounding, long tails: minimalism, so the timbres are cared for), 1 MID,
 *           2 FRANTIC (everything, denser, ratchets); inside the genre's range.
 *   ORCHESTRATION  which roles play: the genre's order of importance cut by the energy; the drums may not play at all;
 *           a role left out keeps its sound and an empty sequence (play it, record it).
 * The guard rails: one harmony anchored on the tonic in every register; the chords always four notes, voiced in a
 * middle register and led to the nearest voicing; the bass deep (from E1); the arps in the middle octaves (C3..C5); the
 * lead under G5 and coherent: a rhythmic motif of a few notes, its contour in steps (a leap filled back by a step), its
 * strong beats on chord tones, developed over four bars (A, A moved to the next chord, A, a cadence held on the
 * tonic); high notes softer, a part that sits high quieter; space between notes. A throw in five gets a TWIST, one
 * experiment (two loops phasing, a comb, the grain delay an octave down, a polymetric lead, a drone): surprises with
 * refinement. Sounds: a preset whose name fits the role and is not a bright one, its envelope shaped for the role and
 * the energy, filtered by role with an envelope on the filter, a TYPE that changes, an LFO of its own shape; the kit
 * one of seven families (808, HARD, CLICK, INDUS, SOFT, GLITCH, ELECTRO) built on the DRUM-X kit. No automation and no
 * modulation matrix beyond the macros' routes. UNDO (GLO + G4) brings back what was there */

enum { RL_DRUM, RL_BASS, RL_PAD, RL_LEAD, RL_ARP, RL_TEX, RL_N };
enum { DS_MOTORIK, DS_ELECTRO, DS_BREAK, DS_BROKEN, DS_MINIMAL, DS_DEEP, DS_HARD, DS_PUNK, DS_INDIE, DS_SPARSE, DS_IDM,
       DS_NONE };
enum { KT_808, KT_HARD, KT_CLICK, KT_INDUS, KT_SOFT, KT_GLITCH, KT_ELECTRO };
enum { BS_OCT8, BS_PUMP, BS_ACID, BS_DRONE, BS_ROLL, BS_BLIP, BS_SUB, BS_PUNK, BS_INDIE, BS_RUMBLE, BS_IDM };
enum { PS_HOLD, PS_STAB, PS_PULSE, PS_SPARSE, PS_DUB, PS_SWELL };
enum { VC_7, VC_9, VC_QUART, VC_SUS, VC_ADD9, VC_MIX };   /* the chords' voicings: always four notes */
enum { LM_MOTIF, LM_OSTINATO, LM_SPARSE, LM_TINTI };     /* how the lead speaks */
enum { AR_UPDOWN, AR_ALBERTI, AR_GLASS, AR_PULSE, AR_WALK };   /* how the arp speaks */
#define EG_A 0u                                         /* ANALOG */
#define EG_PH 2u                                        /* PHASE */
#define EG_LO 3u                                        /* LOFI */
#define EG_VO 5u                                        /* VOICE */
#define EG_NO 11u                                       /* NOISE */
#define EG_FM ENGI_FM6
#define SC_MAJ 1u
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
    uint8_t drums, drum_pct, kit[2];                    /* the family (DS_NONE: never), how often it plays, two kits */
    uint8_t bass, pads, voicing, lead, arp;
    uint8_t e_lo, e_hi;                                 /* its energies */
    uint8_t prio[5];                                    /* the melodic roles, the most important first */
    uint8_t odd;                                        /* percent: loops of odd lengths against each other */
    uint8_t e_bass[3], e_pad[3], e_lead[3], e_arp[3], e_tex[3];
    uint8_t rtype[2], dtime[2];                         /* reverb types (no SHIMMER), delay times (N_DTIME) */
    uint8_t bars, wet;                                  /* chords every 1 or 2 bars; 0..100: how wet the space */
} dstyle_t;

#define X 127
#define P_DANCE {RL_BASS, RL_PAD, RL_LEAD, RL_ARP, RL_TEX}
#define P_ARPS {RL_BASS, RL_ARP, RL_PAD, RL_LEAD, RL_TEX}
#define P_PADS {RL_PAD, RL_TEX, RL_LEAD, RL_BASS, RL_ARP}
#define P_SONG {RL_BASS, RL_LEAD, RL_PAD, RL_ARP, RL_TEX}
#define P_CLASS {RL_ARP, RL_PAD, RL_LEAD, RL_TEX, RL_BASS}
static const dstyle_t DSTYLE[] = {
    {"IDM", 86, 140, 10, {SC_PHRY, SC_HARM, SC_MIN, SC_DOR}, {{0, 5, 1, X}, {0, 3, 1, 5}, {0, 1, 5, 4}},
     DS_IDM, 85, {KT_GLITCH, KT_CLICK}, BS_IDM, PS_SPARSE, VC_MIX, LM_MOTIF, AR_WALK, 0, 2, P_ARPS, 60,
     {EG_FM, EG_A, EG_PH}, {EG_FM, EG_PH, EG_LO}, {EG_PH, EG_LO, EG_FM}, {EG_FM, EG_LO, EG_PH}, {EG_NO, EG_LO, EG_PH},
     {4, 3}, {4, 5}, 1, 60},
    {"DARK IDM", 78, 112, 14, {SC_PHRY, SC_HARM, SC_PHRY, SC_MIN}, {{0, X, X, X}, {0, 1, X, X}, {0, 5, 1, X}},
     DS_IDM, 65, {KT_GLITCH, KT_INDUS}, BS_DRONE, PS_SPARSE, VC_QUART, LM_SPARSE, AR_WALK, 0, 1, P_PADS, 60,
     {EG_FM, EG_PH, EG_A}, {EG_PH, EG_FM, EG_VO}, {EG_PH, EG_VO, EG_FM}, {EG_FM, EG_PH, EG_LO}, {EG_NO, EG_VO, EG_LO},
     {4, 3}, {5, 4}, 2, 75},
    {"DARK MINIMAL", 122, 128, 4, {SC_MIN, SC_PHRY, SC_DOR, SC_MIN}, {{0, X, X, X}, {0, 5, X, X}, {0, 1, X, X}},
     DS_MINIMAL, 90, {KT_CLICK, KT_808}, BS_ROLL, PS_DUB, VC_9, LM_OSTINATO, AR_PULSE, 0, 1, P_DANCE, 40,
     {EG_A, EG_FM, EG_A}, {EG_PH, EG_A, EG_FM}, {EG_A, EG_PH, EG_FM}, {EG_FM, EG_A, EG_PH}, {EG_NO, EG_VO, EG_PH},
     {0, 4}, {1, 4}, 2, 60},
    {"DUB TECHNO", 116, 124, 6, {SC_MIN, SC_DOR, SC_MIN, SC_MPEN}, {{0, X, X, X}, {0, 3, X, X}, {0, 5, 3, X}},
     DS_DEEP, 90, {KT_SOFT, KT_808}, BS_SUB, PS_DUB, VC_9, LM_SPARSE, AR_PULSE, 0, 1, P_PADS, 0,
     {EG_A, EG_FM, EG_A}, {EG_A, EG_PH, EG_FM}, {EG_PH, EG_A, EG_VO}, {EG_FM, EG_A, EG_PH}, {EG_NO, EG_VO, EG_A},
     {4, 0}, {0, 4}, 2, 85},
    {"DARK ELECTRO", 126, 134, 0, {SC_PHRY, SC_MIN, SC_HARM, SC_PHRY}, {{0, 0, 1, 0}, {0, 5, 1, X}, {0, X, X, X}},
     DS_ELECTRO, 100, {KT_ELECTRO, KT_808}, BS_PUMP, PS_STAB, VC_SUS, LM_OSTINATO, AR_UPDOWN, 1, 2, P_DANCE, 0,
     {EG_A, EG_FM, EG_PH}, {EG_A, EG_PH, EG_FM}, {EG_A, EG_PH, EG_VO}, {EG_A, EG_LO, EG_FM}, {EG_NO, EG_VO, EG_A},
     {1, 0}, {4, 1}, 1, 35},
    {"ACID NIGHT", 124, 132, 4, {SC_PHRY, SC_HARM, SC_MIN, SC_DOR}, {{0, X, X, X}, {0, 1, X, X}, {0, 5, X, X}},
     DS_MINIMAL, 95, {KT_808, KT_HARD}, BS_ACID, PS_SPARSE, VC_QUART, LM_SPARSE, AR_PULSE, 1, 2, P_DANCE, 0,
     {EG_A, EG_A, EG_PH}, {EG_PH, EG_A, EG_FM}, {EG_A, EG_PH, EG_VO}, {EG_FM, EG_A, EG_PH}, {EG_NO, EG_VO, EG_PH},
     {0, 4}, {4, 1}, 2, 45},
    {"COLD WAVE", 108, 120, 0, {SC_MIN, SC_DOR, SC_MPEN, SC_MIN}, {{0, 5, 6, 4}, {0, 3, 5, 6}, {0, 6, 5, X}},
     DS_MOTORIK, 85, {KT_ELECTRO, KT_SOFT}, BS_OCT8, PS_HOLD, VC_7, LM_MOTIF, AR_UPDOWN, 1, 2, P_SONG, 0,
     {EG_A, EG_FM, EG_A}, {EG_A, EG_PH, EG_FM}, {EG_A, EG_VO, EG_PH}, {EG_A, EG_FM, EG_LO}, {EG_VO, EG_NO, EG_PH},
     {0, 1}, {1, 4}, 1, 45},
    {"BROKEN", 80, 100, 14, {SC_DOR, SC_MIN, SC_MEL, SC_DOR}, {{0, 3, X, X}, {0, X, X, X}, {0, 6, 3, X}},
     DS_BROKEN, 70, {KT_SOFT, KT_GLITCH}, BS_DRONE, PS_SPARSE, VC_QUART, LM_MOTIF, AR_WALK, 0, 1, P_SONG, 60,
     {EG_FM, EG_VO, EG_PH}, {EG_PH, EG_VO, EG_FM}, {EG_VO, EG_FM, EG_PH}, {EG_FM, EG_PH, EG_LO}, {EG_VO, EG_NO, EG_PH},
     {4, 3}, {0, 4}, 2, 65},
    {"HYPNOTIC", 124, 130, 2, {SC_PHRY, SC_MIN, SC_DOR, SC_HARM}, {{0, X, X, X}, {0, X, X, X}, {0, 1, X, X}},
     DS_DEEP, 95, {KT_CLICK, KT_808}, BS_BLIP, PS_DUB, VC_SUS, LM_OSTINATO, AR_PULSE, 1, 2, P_ARPS, 50,
     {EG_A, EG_FM, EG_PH}, {EG_PH, EG_A, EG_FM}, {EG_A, EG_PH, EG_LO}, {EG_A, EG_FM, EG_PH}, {EG_NO, EG_PH, EG_VO},
     {0, 4}, {5, 1}, 2, 55},
    {"DEEP BREAKS", 108, 122, 8, {SC_MIN, SC_HARM, SC_DOR, SC_PHRY}, {{0, 5, X, X}, {0, 6, 5, X}, {0, 3, 1, X}},
     DS_BREAK, 100, {KT_ELECTRO, KT_SOFT}, BS_ACID, PS_STAB, VC_9, LM_MOTIF, AR_UPDOWN, 1, 2, P_DANCE, 0,
     {EG_A, EG_PH, EG_A}, {EG_PH, EG_A, EG_FM}, {EG_A, EG_PH, EG_VO}, {EG_A, EG_FM, EG_PH}, {EG_NO, EG_VO, EG_PH},
     {1, 4}, {4, 1}, 1, 40},
    {"SOUNDTRACK", 64, 92, 0, {SC_MIN, SC_DOR, SC_MEL, SC_MPEN}, {{0, 5, 3, 6}, {0, 3, X, X}, {0, 5, 2, 6}},
     DS_SPARSE, 35, {KT_SOFT, KT_808}, BS_SUB, PS_SWELL, VC_ADD9, LM_SPARSE, AR_ALBERTI, 0, 1, P_PADS, 30,
     {EG_A, EG_FM, EG_PH}, {EG_A, EG_PH, EG_VO}, {EG_FM, EG_PH, EG_VO}, {EG_FM, EG_A, EG_PH}, {EG_NO, EG_VO, EG_PH},
     {4, 0}, {0, 4}, 2, 90},
    {"HARD TECHNO", 138, 150, 0, {SC_PHRY, SC_MIN, SC_HARM, SC_PHRY}, {{0, X, X, X}, {0, 1, X, X}, {0, X, X, X}},
     DS_HARD, 100, {KT_HARD, KT_INDUS}, BS_RUMBLE, PS_STAB, VC_SUS, LM_OSTINATO, AR_PULSE, 1, 2, P_DANCE, 0,
     {EG_A, EG_PH, EG_A}, {EG_PH, EG_A, EG_FM}, {EG_A, EG_PH, EG_LO}, {EG_A, EG_PH, EG_FM}, {EG_NO, EG_NO, EG_PH},
     {1, 0}, {4, 2}, 2, 30},
    {"INDIE POP", 100, 124, 6, {SC_MAJ, SC_DOR, SC_MAJ, SC_MIN}, {{0, 2, 5, 3}, {5, 3, 0, 4}, {0, 3, 1, 4}},
     DS_INDIE, 90, {KT_SOFT, KT_ELECTRO}, BS_INDIE, PS_PULSE, VC_ADD9, LM_MOTIF, AR_ALBERTI, 1, 2, P_SONG, 0,
     {EG_A, EG_FM, EG_A}, {EG_A, EG_PH, EG_FM}, {EG_A, EG_VO, EG_FM}, {EG_FM, EG_A, EG_PH}, {EG_VO, EG_NO, EG_PH},
     {0, 1}, {1, 4}, 1, 40},
    {"ELECTRO PUNK", 126, 138, 0, {SC_MIN, SC_PHRY, SC_MIN, SC_MPEN}, {{0, 0, 5, 6}, {0, 6, X, X}, {0, 1, 0, 6}},
     DS_PUNK, 100, {KT_HARD, KT_ELECTRO}, BS_PUNK, PS_STAB, VC_SUS, LM_MOTIF, AR_UPDOWN, 2, 2, P_SONG, 0,
     {EG_A, EG_PH, EG_A}, {EG_A, EG_PH, EG_LO}, {EG_A, EG_VO, EG_LO}, {EG_A, EG_LO, EG_PH}, {EG_NO, EG_VO, EG_LO},
     {1, 1}, {2, 1}, 1, 25},
    {"NEO CLASSICAL", 70, 112, 0, {SC_MIN, SC_DOR, SC_MAJ, SC_MEL}, {{0, 5, 3, 4}, {0, 3, 4, 0}, {0, 2, 5, 4}},
     DS_NONE, 0, {KT_SOFT, KT_SOFT}, BS_SUB, PS_HOLD, VC_ADD9, LM_TINTI, AR_GLASS, 0, 1, P_CLASS, 50,
     {EG_FM, EG_PH, EG_A}, {EG_PH, EG_A, EG_VO}, {EG_FM, EG_PH, EG_VO}, {EG_FM, EG_PH, EG_A}, {EG_VO, EG_PH, EG_NO},
     {0, 4}, {0, 1}, 2, 70},
};
#undef X
#define NDSTYLE (sizeof DSTYLE / sizeof DSTYLE[0])
#define DSTYLE_IDM 2u                                   /* (the first two: IDM, DARK IDM; picked more often) */

static int32_t rr(int32_t lo, int32_t hi) { return lo + (int32_t)(rng() % (uint32_t)(hi - lo + 1)); }
static int chance(uint32_t pct) { return rng() % 100u < pct; }

/* the piece: the harmony (the progression, how long each chord lasts, the voicing, the last chord voiced: the next is
 * led to it), the energy, the twist */
enum { TW_NONE, TW_PHASE, TW_COMB, TW_GRAIN, TW_POLY, TW_DRONE };
static struct {
    const int8_t *prog;
    uint32_t n, steps, voicing, nlast, energy, twist, kit;
    int32_t last[4];
} dh;
static int32_t chord_at(uint32_t step) { return dh.prog[(step / dh.steps) % dh.n]; }
/* the tonic at or above lo: every part's degree 0 (0.6.32: the chords and the lead had the fifth and the third) */
static int32_t dtonic(const track_t *t, int32_t lo) { return lo + ((t->p[P_ROOT] - lo) % 12 + 12) % 12; }

static void dstep_note(step_t *s, const int32_t *n, uint32_t cnt, uint32_t vel, uint32_t flags)
{
    uint32_t j;
    int32_t top = 0;
    s->n = (uint8_t)cnt;
    for (j = 0; j < 4u; j++) {
        s->note[j] = (uint8_t)(j < cnt ? clamp(n[j], 0, 127) : 0);
        if (j < cnt && n[j] > top) top = n[j];
    }
    s->time = ST_NOTE;
    s->flags = (uint8_t)flags;
    if (top > 74) vel -= (uint32_t)((top - 74) * 2 < 30 ? (top - 74) * 2 : 30);   /* (high notes measured: softer) */
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

/* ---- DRUMS (lanes: 0 BD, 1 SD, 2 CP, 3 CH, 4 OH, 5 TM, 6 RS, 7 CB): a family's bar as strings ('x' a hit, 'a'
 * accented, 'g' a ghost: three times in four), a fill over the loop's last beat; the energy thins (SPACE: the hats
 * mostly gone) or fills (FRANTIC: ghost hats, ratchets) it */
static const char *const DRUM_BAR[][8] = {
    [DS_MOTORIK] = {"x...x...x...x...", "....x.......x...", "", "x.x.x.x.x.x.x.x.", "", "", "..x.....x.x.....", ""},
    [DS_ELECTRO] = {"a......x..x.....", "....a.......a...", "....x.......x..x", "xgxgxgxgxgxgxgxg", "", "..............x.", "", "......x........."},
    [DS_BREAK]   = {"a.........x.....", "....a..g.g..a..g", "", "x.x.x.x.x.x.x.x.", "......x.......x.", "", "", ""},
    [DS_BROKEN]  = {"x..x....x.x.....", "....a..g.g..a...", "", "x.xgx.x.x.xgx.x.", ".......x........", "", "", ""},
    [DS_MINIMAL] = {"a...x...a...x...", "", "....g.......g...", "g.x...xgg.x...x.", "..x...x...x...x.", "", "...x..g...x..x..", "......g........."},
    [DS_DEEP]    = {"a...x...a...x...", "", "............g...", "..x...x...x...x.", "..g...x...g...x.", "", ".g.....x.g....x.", ""},
    [DS_HARD]    = {"a...a...a...a...", "", "....x.......x...", "xgxgxgxgxgxgxgxg", "..x...x...x...x.", "", "..g..g....g..g..", "......g.......g."},
    [DS_PUNK]    = {"a.....x.a.x.....", "....a.......a...", "", "x.x.x.x.x.x.x.x.", "", "", "", ""},
    [DS_INDIE]   = {"a......xa.x.....", "....a.......a..g", "", "x.xgx.xgx.xgx.xg", "..............x.", "", "", ""},
    [DS_SPARSE]  = {"x...............", "", "", "", "", "", "......g.....x...", "..........g....."},
};
static void dice_drums(track_t *t, const dstyle_t *st, uint32_t len)
{
    uint32_t i, l, e = dh.energy;
    if (st->drums == DS_IDM) {                          /* euclids against each other, ratchets, chance */
        uint32_t kb = (uint32_t)rr(3, 6), ks = (uint32_t)rr(2, 4), kh = (uint32_t)rr(5, 9) + e * 2u, rot = (uint32_t)rr(0, 7);
        for (i = 0; i < len; i++) {
            step_t *s = &t->step[i];
            uint32_t h = 0, b = i / 16u;
            if (euclid(i, kb, 16, rot + (b == 3u ? 2u : 0u))) h |= 1u << 0;
            if (euclid(i, ks, 16, rot + 4u) && !(h & 1u)) h |= 1u << (chance(50) ? 1 : 6);
            if (euclid(i, kh + (b & 1u) * 2u, 16, b) && (e || chance(40))) h |= 1u << (chance(20) ? 4 : 3);
            if (chance(8)) h |= 1u << (5u + rng() % 3u);
            if (!h) continue;
            s->time = ST_NOTE; s->hit = (uint8_t)h; s->acc = (uint8_t)(i % 4u == 0u ? h & 1u : 0u);
            s->vel = (uint8_t)(h & 1u ? 104 : rr(70, 100));
            if ((h & (1u << 3)) && chance(e == 2u ? 40 : 15)) step_set_ratchet(s, (uint32_t)rr(2, 4));
            if (!(h & 1u) && chance(30)) step_set_chance(s, (uint32_t)rr(45, 85));
        }
        return;
    }
    for (i = 0; i < len; i++) {
        step_t *s = &t->step[i];
        uint32_t h = 0, a = 0, ghost = 0, p = i % 16u;
        uint32_t fill = len > 16u && i >= len - 4u && st->drums != DS_DEEP && st->drums != DS_SPARSE && e;
        for (l = 0; l < NLANE; l++) {
            const char *b = DRUM_BAR[st->drums][l];
            char c = b && b[0] ? b[p] : '.';
            if (c == 'x' || c == 'a' || (c == 'g' && rng() % 4u)) h |= 1u << l;
            if (c == 'a') a |= 1u << l;
            if (c == 'g') ghost = 1;
        }
        if (e == 0u && (h & 0x18u) && chance(65)) h &= ~0x18u;   /* (SPACE: the hats mostly gone) */
        if (e == 2u && !(h & 0x18u) && p % 2u == 1u && chance(35)) { h |= 1u << 3; ghost = 1; }   /* (FRANTIC: more) */
        if (fill) {
            if (chance(50)) h |= 1u << (chance(50) ? 1 : 5);
            h &= ~(1u << 4);
        }
        if (i >= 16u && !fill && chance(8)) h ^= 1u << (chance(50) ? 6 : 7);   /* bar 2: a percussion moved */
        if (!h) continue;
        s->time = ST_NOTE; s->hit = (uint8_t)h; s->acc = (uint8_t)a;
        s->vel = (uint8_t)(ghost && !a ? rr(62, 82) : rr(96, 110));
        if (fill && chance(40)) step_set_ratchet(s, 2);
        if ((h & (1u << 3)) && chance(e == 2u ? 14 : 4)) step_set_ratchet(s, 2);
        if (!(h & 1u) && (st->drums == DS_MINIMAL || st->drums == DS_DEEP || st->drums == DS_SPARSE) && chance(15))
            step_set_chance(s, (uint32_t)rr(50, 85));
    }
}
/* the kit: a family built on the DRUM-X kit (each sound's wave, pitch modulation, drive, decay, pitch, noise and
 * colour moved, both patches), then the kit's knobs (TONE DECAY NOISE FM FOLD DRIVE) */
static uint8_t kmode(uint8_t m, int32_t w, int32_t pm, int32_t drv)   /* -1: keep that field */
{
    if (w >= 0) m = (uint8_t)((m & ~3u) | (uint32_t)w);
    if (pm >= 0) m = (uint8_t)((m & ~(3u << 5)) | (uint32_t)pm << 5);
    if (drv >= 0) m = (uint8_t)(drv ? m | DX_DRIVE : m & ~DX_DRIVE);
    return m;
}
static void kmove(dx_lane_t *L, int32_t pitch, int32_t decay, int32_t noise, int32_t color)
{
    uint8_t *ab[2] = {L->a, L->b};
    uint32_t k;
    for (k = 0; k < 2u; k++) {
        ab[k][DXP_PITCH] = (uint8_t)clamp(ab[k][DXP_PITCH] + pitch + rr(-3, 3), 0, 127);
        ab[k][DXP_DECAY] = (uint8_t)clamp(ab[k][DXP_DECAY] + decay + rr(-4, 4), 0, 127);
        ab[k][DXP_NOISE] = (uint8_t)clamp(ab[k][DXP_NOISE] + noise, 0, 127);
        ab[k][DXP_COLOR] = (uint8_t)clamp(ab[k][DXP_COLOR] + color + rr(-4, 4), 0, 127);
    }
}
static void dice_kit(track_t *t, uint32_t kt)
{
    uint32_t l;
    int16_t *p = t->p;
    for (l = 0; l < 8u; l++) dx_kit[l] = DX_KIT_DEF[l];
    dh.kit = kt;
    switch (kt) {
    case KT_808:                                        /* the boom: a long pitch fall, deep toms, soft hats */
        dx_kit[DV_KICK].mode = kmode(dx_kit[DV_KICK].mode, DXW_SINE, DXM_LONG, 0);
        kmove(&dx_kit[DV_KICK], -3, 26, -4, -10); kmove(&dx_kit[DV_TOM], -6, 14, 0, -10);
        kmove(&dx_kit[DV_HATC], -6, 0, 0, -18); kmove(&dx_kit[DV_HATO], -6, 0, 0, -18);
        p[P_E2] = (int16_t)rr(48, 62); p[P_E3] = (int16_t)rr(64, 84); p[P_E7] = 0; p[P_E6] = 0; break;
    case KT_HARD:                                       /* driven kick, big clap, bright metal hats, folded */
        dx_kit[DV_KICK].mode = kmode(dx_kit[DV_KICK].mode, DXW_SINE, DXM_DECAY, 1);
        kmove(&dx_kit[DV_KICK], 2, 6, 6, 20); kmove(&dx_kit[DV_CLAP], 0, 14, 0, 6);
        dx_kit[DV_TOM].mode = kmode(dx_kit[DV_TOM].mode, -1, -1, 1);
        p[P_E2] = (int16_t)rr(62, 80); p[P_E3] = (int16_t)rr(48, 62); p[P_E7] = (int16_t)rr(60, 105);
        p[P_E6] = (int16_t)rr(15, 45); break;
    case KT_CLICK:                                      /* minimal: short, dry, clicks and blips */
        for (l = 0; l < 8u; l++) kmove(&dx_kit[l], 0, -20, -6, 0);
        kmove(&dx_kit[DV_KICK], 6, 6, 0, 0);
        dx_kit[DV_RIM].mode = kmode(dx_kit[DV_RIM].mode, DXW_BELL, -1, -1);
        dx_kit[DV_BELL].mode = kmode(dx_kit[DV_BELL].mode, DXW_FM, DXM_SINE, -1);
        p[P_E2] = (int16_t)rr(55, 70); p[P_E3] = (int16_t)rr(30, 46); p[P_E7] = 0; p[P_E6] = 0; break;
    case KT_INDUS:                                      /* metal, noise, the pitch scrambled */
        dx_kit[DV_SNARE].mode = kmode(dx_kit[DV_SNARE].mode, DXW_METAL, DXM_NOISE, -1);
        dx_kit[DV_CLAP].mode = kmode(dx_kit[DV_CLAP].mode, DXW_METAL, -1, -1);
        dx_kit[DV_TOM].mode = kmode(dx_kit[DV_TOM].mode, DXW_FM, DXM_NOISE, 1);
        kmove(&dx_kit[DV_SNARE], -4, 10, 14, 0); kmove(&dx_kit[DV_TOM], 0, 0, 30, 10);
        p[P_E2] = (int16_t)rr(55, 72); p[P_E4] = (int16_t)rr(64, 90); p[P_E5] = (int16_t)rr(35, 80);
        p[P_E6] = (int16_t)rr(25, 65); p[P_E7] = (int16_t)rr(20, 60); break;
    case KT_SOFT:                                       /* round, brushed, warm */
        kmove(&dx_kit[DV_KICK], 0, 4, 0, -14); kmove(&dx_kit[DV_SNARE], 0, 4, 12, -24);
        dx_kit[DV_SNARE].mode = (uint8_t)((dx_kit[DV_SNARE].mode & ~(3u << 2)) | (uint32_t)DXF_LP << 2);
        kmove(&dx_kit[DV_HATC], -4, 2, 0, -24); kmove(&dx_kit[DV_HATO], -4, -8, 0, -24);
        p[P_E2] = (int16_t)rr(38, 54); p[P_E3] = (int16_t)rr(54, 70); p[P_E7] = 0; p[P_E6] = 0; break;
    case KT_GLITCH:                                     /* bells, lasers, grit, short */
        dx_kit[DV_RIM].mode = kmode(dx_kit[DV_RIM].mode, DXW_BELL, DXM_NOISE, -1);
        dx_kit[DV_BELL].mode = kmode(dx_kit[DV_BELL].mode, DXW_BELL, DXM_SINE, -1);
        dx_kit[DV_TOM].mode = kmode(dx_kit[DV_TOM].mode, DXW_FM, DXM_SINE, -1);
        dx_kit[DV_CLAP].mode = kmode(dx_kit[DV_CLAP].mode, -1, DXM_NOISE, -1);
        for (l = 2; l < 8u; l++) kmove(&dx_kit[l], rr(-8, 8), -14, 0, 0);
        p[P_E2] = (int16_t)rr(50, 70); p[P_E3] = (int16_t)rr(36, 56); p[P_E5] = (int16_t)rr(15, 55); p[P_E7] = 0; break;
    default:                                            /* KT_ELECTRO: 808 and 909 together, a snappy snare */
        kmove(&dx_kit[DV_KICK], -2, 12, 0, 0); kmove(&dx_kit[DV_SNARE], 2, -6, 6, 8);
        p[P_E2] = (int16_t)rr(56, 72); p[P_E3] = (int16_t)rr(52, 70); p[P_E7] = (int16_t)rr(0, 30); p[P_E6] = 0; break;
    }
    p[P_E0] = (int16_t)rr(20, 100);                     /* MORPH: somewhere between A and B */
    p[P_E1] = (int16_t)rr(52, 70);                      /* TUNE */
    t->p[P_FTYPE] = 0;                                  /* the kit through a soft low-pass: the hats not glassy */
    t->p[P_FCUT] = (int16_t)(kt == KT_HARD ? rr(108, 120) : rr(100, 116));
    t->p[P_FRES] = (int16_t)rr(0, 18);
}

/* ---- the chords: degree d's four notes from the tonic t0 in the voicing, then led (each note an octave up or down)
 * to the nearest of the last chord inside [t0 - 2, t0 + 17] (C3..F4 or so): no leaps, a middle register */
static uint32_t dchord(const track_t *t, int32_t t0, int32_t d, uint32_t vc, int32_t *n)
{
    static const int8_t V[5][4] = {{0, 2, 4, 6}, {0, 2, 6, 8}, {0, 3, 6, 9}, {0, 3, 4, 8}, {0, 2, 4, 8}};   /* 7th,
                                                                 * 9th (no 5th), fourths, sus4 add9, add9 */
    uint32_t j, i;
    if (vc == VC_MIX) vc = rng() % 5u;
    for (j = 0; j < 4u; j++) n[j] = scale_steps(t, t0, d + V[vc][j]);
    for (j = 0; j < 4u; j++) {
        while (n[j] < t0 - 2) n[j] += 12;
        while (n[j] > t0 + 15) n[j] -= 12;
    }
    if (dh.nlast) {                                     /* led: each note the octave nearest the last chord's middle */
        int32_t mid = 0;
        for (i = 0; i < dh.nlast; i++) mid += dh.last[i];
        mid /= (int32_t)dh.nlast;
        for (j = 0; j < 4u; j++) {
            if (n[j] - mid > 7 && n[j] - 12 >= t0 - 5) n[j] -= 12;
            else if (mid - n[j] > 7 && n[j] + 12 <= t0 + 17) n[j] += 12;
        }
    }
    for (i = 1; i < 4u; i++)                            /* (low to high) */
        for (j = i; j > 0 && n[j] < n[j - 1]; j--) { int32_t x = n[j]; n[j] = n[j - 1]; n[j - 1] = x; }
    for (j = 1; j < 4u; j++)                            /* (no unisons: a clash an octave up) */
        if (n[j] == n[j - 1]) n[j] += 12;
    for (j = 0; j < 4u; j++) dh.last[j] = n[j];
    dh.nlast = 4;
    return 4;
}

/* ---- the bass: deep (from E1: its register starts low), the figure the genre's, denser with the energy */
static void dice_bass(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = dtonic(t, 28), n[1];
    uint32_t i, e = dh.energy;
    static const uint8_t PUMP[16] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0};
    uint8_t acid[16], roll[16], ind[16];
    for (i = 0; i < 16u; i++) {                         /* (one bar's figure, kept: a riff, not noise) */
        acid[i] = (uint8_t)(i % 4u == 0u ? 1u : chance(30 + e * 10u) ? 1u + rng() % 4u : 0u);
        roll[i] = (uint8_t)(i % 4u == 2u ? 1u : i % 4u == 3u ? chance(20 + e * 15u) : i % 4u == 1u ? chance(e * 8u) : 0u);
        ind[i] = (uint8_t)(i == 0u || i == 10u || (i == 6u && chance(50)) || (i == 14u && chance(40)));
    }
    if (st->bass != BS_DRONE && st->bass != BS_SUB && base < 33) base += 12;   /* (a figure: from A1; a drone: E1) */
    for (i = 0; i < len; i++) {
        step_t *s = &t->step[i];
        int32_t d = chord_at(i), on = 0, oct = 0, step = 0, vel = 96;
        uint32_t fl = 0, p = i % 16u, hold = 0;
        switch (st->bass) {
        case BS_OCT8: on = i % 2u == 0u && (e || i % 4u == 0u); oct = (i / 2u) & 1u; break;
        case BS_PUMP: on = PUMP[p]; oct = p == 11u; fl = p == 0u ? SF_ACCENT : 0; break;
        case BS_ACID: on = acid[p] != 0u; step = acid[p] == 2u ? 1 : acid[p] == 3u ? 4 : acid[p] == 4u ? -1 : 0;
                      oct = acid[p] == 3u && chance(50); fl = (p % 4u == 0u ? SF_ACCENT : 0) | (chance(25) ? SF_SLIDE : 0); break;
        case BS_DRONE: on = i % (dh.steps < 16u ? dh.steps : 16u) == 0u; hold = 15; break;
        case BS_ROLL: on = roll[p]; vel = p % 4u == 2u ? 100 : 80; break;
        case BS_SUB:
            on = p == 0u || (e && p == 10u) || (e == 2u && p == 6u);
            hold = p == 0u ? (e ? 5 : 13) : 2;
            if (p == 10u && chance(40)) step = 4;
            break;
        case BS_BLIP: on = (p % 4u == 2u && chance(70)) || p == 0u; vel = 90; break;
        case BS_PUNK: on = i % 2u == 0u; step = p >= 12u && chance(40) ? 4 : 0; fl = p % 8u == 0u ? SF_ACCENT : 0; break;
        case BS_INDIE: on = ind[p]; step = p == 14u ? (chord_at(i + 2u) - d) - 1 : p == 10u && chance(40) ? 4 : 0;
                       hold = p == 0u ? 3 : 1; break;
        case BS_RUMBLE: on = p % 4u != 0u && (p % 4u == 2u || chance(30 + e * 20u)); vel = p % 4u == 2u ? 96 : 76; break;
        default: on = chance(25 + e * 10u); step = (int32_t)(rng() % 3u) * 2 - 2; oct = chance(10); break;   /* BS_IDM */
        }
        if (!on) continue;
        n[0] = scale_steps(t, base, d + step) + (oct ? 12 : 0);
        dstep_note(s, n, 1, (uint32_t)vel + (fl & SF_ACCENT ? 16u : 0u), fl);
        if (hold) dhold(t, i, hold, len);
        if (st->bass == BS_IDM && chance(12)) step_set_ratchet(s, 2);
    }
}

/* ---- the chords: always four notes; held, swelling, stabbed, pulsed, dubbed or sparse */
static void dice_pads(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t t0 = dtonic(t, 50), n[4];
    uint32_t i, c, last_d = 0xFFFFu, e = dh.energy;
    uint8_t dub[16];
    {   /* (PS_DUB: one bar's stab figure, kept: the first offbeat and one to three of the others) */
        static const uint8_t AT[6] = {6, 10, 14, 7, 11, 13};
        uint32_t m = (uint32_t)rr(1, 2) + (e == 2u), j;
        for (i = 0; i < 16u; i++) dub[i] = (uint8_t)(i == 2u);
        for (j = 0; j < m; j++) dub[AT[rng() % 6u]] = 1;
    }
    dh.nlast = 0;
    for (i = 0; i < len; i++) {
        step_t *s = &t->step[i];
        uint32_t pp = i % dh.steps, p = i % 16u;
        int on = 0;
        switch (st->pads) {
        case PS_HOLD: case PS_SWELL: on = pp == 0u; break;
        case PS_STAB: on = (p % 4u == 2u && chance(35 + e * 15u)) || (pp == 0u && chance(40)); break;
        case PS_PULSE: on = i % (e == 2u ? 2u : 4u) == 0u && !(p == 12u && chance(50)); break;
        case PS_DUB: on = dub[p]; break;
        default: on = pp == 0u && (i == 0u || chance(65)); break;   /* PS_SPARSE: some chords (the first), the rest silence */
        }
        if (!on) continue;
        if ((uint32_t)chord_at(i) != last_d || st->pads == PS_HOLD || st->pads == PS_SWELL || st->pads == PS_SPARSE) {
            c = dchord(t, t0, chord_at(i), dh.voicing, n);
            last_d = (uint32_t)chord_at(i);
        } else {                                        /* (the same chord: the same voicing, stab after stab) */
            for (c = 0; c < dh.nlast; c++) n[c] = dh.last[c];
        }
        dstep_note(s, n, c, st->pads == PS_DUB ? (p == 2u ? 90u : 78u) : st->pads == PS_SWELL ? 74u : 80u, 0);
        if (st->pads == PS_HOLD || st->pads == PS_SWELL || st->pads == PS_SPARSE) dhold(t, i, dh.steps - 1u, len);
        if (st->pads == PS_PULSE && chance(40)) dhold(t, i, 1, len);
    }
}

/* ---- the lead (around the tonic at G3..: never past G5). LM_MOTIF: a rhythmic motif of 2..5 notes (more with the
 * energy), its contour in steps (a leap filled back by a step the other way), its strong steps on chord tones;
 * the phrase A, A moved to the next chord (a sequence), A (its end varied), a cadence (its first half, then the tonic
 * held). LM_OSTINATO: a cell of 2..4 notes on an euclidean rhythm, turning. LM_SPARSE: a few long chord tones
 * stepping. LM_TINTI: Part's M voice, a scale line in long notes, rising or falling around the tonic */
static int32_t dnear_ct(int32_t deg, int32_t chord)    /* the chord tone (chord + 0, 2, 4) nearest a degree */
{
    int32_t best = chord, k;
    for (k = -7; k <= 11; k += 2) {
        int32_t c = chord + k;
        if (((c - chord) % 7 + 7) % 7 > 4) continue;    /* (0, 2, 4 of the chord: its triad) */
        if ((deg - c < 0 ? c - deg : deg - c) < (deg - best < 0 ? best - deg : deg - best)) best = c;
    }
    return best;
}
static void dice_lead(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = dtonic(t, 55), n[1];
    uint32_t i, e = dh.energy;
    if (st->lead == LM_OSTINATO) {
        static const int8_t CELL[6][4] = {{0, 0, 1, 0}, {0, 2, 1, 0}, {4, 2, 0, 1}, {0, -1, 0, 2}, {0, 4, 3, 1}, {2, 1, 0, -2}};
        const int8_t *cl = CELL[rng() % 6u];
        uint32_t k = (uint32_t)rr(3, 5) + e, rot = (uint32_t)rr(0, 3), nc = (uint32_t)rr(2, 4), j = 0;
        for (i = 0; i < len; i++) {
            uint32_t b = i / 16u;
            if (!euclid(i % 16u, k, 16, rot)) continue;
            if (b % 4u == 3u && chance(50)) continue;  /* (the fourth bar: a breath) */
            n[0] = scale_steps(t, base, cl[j % nc] + (b % 4u == 2u && j % nc == nc - 1u ? 1 : 0));
            dstep_note(&t->step[i], n, 1, j % nc == 0u ? 98u : 82u, chance(15) ? SF_SLIDE : 0);
            j++;
        }
        return;
    }
    if (st->lead == LM_SPARSE || st->lead == LM_TINTI) {   /* long notes: 1 or 2 a bar (TINTI: a scale line) */
        int32_t d = st->lead == LM_TINTI ? rr(2, 5) : 2, dir = chance(50) ? -1 : 1;
        for (i = 0; i < len; i += 8u) {
            uint32_t b = i / 16u;
            if (st->lead == LM_SPARSE && ((i % 16u) || (b % 2u == 1u && chance(40)))) continue;
            if (st->lead == LM_SPARSE) d = dnear_ct(d + rr(-1, 1), chord_at(i));
            else { d += dir; if (d < -2 || d > 7) { dir = -dir; d += 2 * dir; } }
            n[0] = scale_steps(t, base, d);
            dstep_note(&t->step[i], n, 1, 84, 0);
            dhold(t, i, st->lead == LM_TINTI ? 6 : 12, len);
        }
        return;
    }
    {   /* LM_MOTIF */
        uint8_t at[6];
        int8_t deg[6];
        uint32_t k = (uint32_t)rr(2, 3) + e, j, rot = (uint32_t)rr(0, 5), bars = len / 16u ? len / 16u : 1u;
        int32_t d = (int32_t)(rng() % 3u) * 2, prev = 0;
        if (k > 5u) k = 5u;
        for (i = 0, j = 0; i < 16u && j < k; i++)       /* the motif's rhythm: k onsets, euclidean, turned */
            if (euclid(i, k, 16, rot)) at[j++] = (uint8_t)i;
        k = j;
        for (j = 0; j < k; j++) {                       /* its contour: steps; a leap, then a step back */
            int32_t mv = chance(70) ? (chance(50) ? 1 : -1) : (chance(50) ? rr(2, 3) : -rr(2, 3));
            if (j && (prev >= 2 || prev <= -2)) mv = prev > 0 ? -1 : 1;   /* (the gap filled) */
            if (j) d = clamp(d + mv, -3, 7);
            deg[j] = (int8_t)d;
            prev = j ? mv : 0;
        }
        for (i = 0; i < bars; i++) {
            uint32_t ph = i % 4u, b0 = i * 16u;
            int32_t ch = chord_at(b0), shift = ph == 1u ? chord_at(b0) - chord_at(b0 >= 16u ? b0 - 16u : b0) : 0;
            if (bars >= 2u && i == bars - 1u) {         /* the cadence: half the motif, then a chord tone near the
                                                         * tonic held to the end */
                uint32_t h = (k + 1u) / 2u, land;
                for (j = 0; j < h; j++) {
                    n[0] = scale_steps(t, base, at[j] % 4u == 0u ? dnear_ct(deg[j] + shift, ch) : deg[j] + shift);
                    dstep_note(&t->step[b0 + at[j]], n, 1, at[j] % 4u == 0u ? 96u : 84u, 0);
                }
                land = b0 + (h < k && at[h] > 8u ? at[h] : 8u);
                if (land > b0 + at[h - 1u] && land < len) {
                    n[0] = scale_steps(t, base, dnear_ct(0, ch));
                    dstep_note(&t->step[land], n, 1, 90, 0);
                    dhold(t, land, len - land - 1u, len);
                }
                continue;
            }
            if (ph == 2u && chance(30)) continue;       /* (now and then a bar of rest: space) */
            for (j = 0; j < k; j++) {
                int32_t dd = deg[j] + shift;
                uint32_t pos = b0 + at[j];
                if (ph == 2u && j == k - 1u) dd += chance(50) ? 1 : -1;   /* (A's end varied) */
                if (at[j] % 4u == 0u) dd = dnear_ct(dd, ch);
                if (pos >= len) break;
                n[0] = scale_steps(t, base, dd);
                if (n[0] > 79) n[0] -= 12;              /* (under G5) */
                dstep_note(&t->step[pos], n, 1, at[j] % 4u == 0u ? 96u : 82u, chance(10) ? SF_SLIDE : 0);
                {   /* held to the next onset, a step of air before it (more air with less energy) */
                    uint32_t nxt = j + 1u < k ? at[j + 1u] : 16u + at[0], gap = nxt - at[j];
                    if (gap > 2u + (2u - e)) dhold(t, pos, gap - 2u - (2u - e), len);
                }
            }
        }
    }
}

/* ---- the arp, in the middle octaves (C3..C5) over the chord's four notes. AR_UPDOWN: up and down with holes;
 * AR_ALBERTI: low, high, middle, high; AR_GLASS: Glass's additive groups (3, then 3+2, ...) in a steady pulse;
 * AR_PULSE: Reich's repeated tone with accents; AR_WALK: a random walk over the chord (the IDM's) */
static void dice_arp(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t t0 = dtonic(t, 50), n[1], c[4];
    uint32_t i, e = dh.energy, rate = e == 2u || st->arp == AR_GLASS ? 1u : 2u, k = (uint32_t)rr(5, 9) + e * 2u, w = 0;
    static const uint8_t ALB[4] = {0, 3, 1, 3};
    if (st->arp == AR_GLASS || st->arp == AR_PULSE) k = 16;   /* (a steady pulse: no holes) */
    dh.nlast = 0;
    for (i = 0; i < len; i++) {
        uint32_t idx = i / rate, j;
        if (i % rate) continue;
        if (!euclid(idx % 16u, k, 16, 0) && chance(80)) continue;   /* (holes in the figure: space) */
        if (i % dh.steps == 0u || !dh.nlast) dchord(t, t0 + 2, chord_at(i), dh.voicing, c);
        else for (j = 0; j < 4u; j++) c[j] = dh.last[j];
        switch (st->arp) {
        case AR_ALBERTI: j = ALB[idx % 4u]; break;
        case AR_GLASS: { uint32_t g = (idx / 8u) % 2u ? 5u : 3u; j = (idx % g) % 4u; break; }
        case AR_PULSE: j = idx % 8u == 7u ? 2u : 0u; break;
        case AR_WALK: w = (w + (chance(50) ? 1u : 3u)) % 4u; j = w; break;
        default: j = (idx % 6u) < 4u ? idx % 6u : 6u - idx % 6u; break;   /* up 0 1 2 3, down 2 1 */
        }
        n[0] = c[j];
        while (n[0] > 72) n[0] -= 12;                   /* (the middle octaves) */
        while (n[0] < 48) n[0] += 12;
        dstep_note(&t->step[i], n, 1, idx % 4u == 0u ? 92u : st->arp == AR_PULSE ? 70u : 78u, 0);
        if (st->drums == DS_IDM && chance(10)) step_set_ratchet(&t->step[i], 2);
    }
}

/* ---- the texture: a long low note a bar or two (the root, the fifth, the second), or a drone over everything */
static void dice_texture(track_t *t, const dstyle_t *st, uint32_t len)
{
    int32_t base = dtonic(t, 43), n[1];
    uint32_t i;
    (void)st;
    for (i = 0; i < len; i += 16u) {
        uint32_t at = i + (uint32_t)rr(0, 2) * 4u;
        if (i && chance(45)) continue;
        n[0] = scale_steps(t, base, chance(30) ? 4 : chance(30) ? 1 : 0);
        dstep_note(&t->step[at < len ? at : i], n, 1, 72, 0);
    }
    for (i = 0; i < len; i++)
        if (t->step[i].time == ST_NOTE) dhold(t, i, dh.twist == TW_DRONE ? 63 : 11, len);
}

/* ---- the sound: a factory preset of engine e whose name says the role and is not a bright one, moved a little;
 * its envelope shaped for the role and the energy; filtered for the role */
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
                                        {"PAD", "PWM", "STR", "CHOIR", 0},
                                        {"SAW LEAD", "SYNC", "WIRE", "VOX", "WAVE LEAD", "TINE"},
                                        {"PLUCK", "FLOAT ARP", "SINE KEY", "TINE", "RESO", "BYTE TONE"},
                                        {"WIND", "RAIN", "WHISPER", "METAL", "BYTE BENT", "FLOAT FM"}};
    uint32_t e = el[rng() % 3u], tries, k, n, i, nm = 0, na = 0, en_ = dh.energy;
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
    for (i = 0; i < n; i++) {                           /* the presets whose names fit the role; the not bright ones,
                                                         * and never a bass's sound off the bass (0.6.34) */
        const char *pn = en->presets[i].name;
        if (dbright(pn) || (role != RL_BASS && (dname_has(pn, "BASS") || dname_has(pn, "GROWL") || dname_has(pn, "ACID"))))
            continue;
        if (na < sizeof any) any[na++] = (uint8_t)i;
        for (k = 0; k < 6u && W[role][k]; k++)
            if (dname_has(en->presets[i].name, W[role][k]) && nm < sizeof m) { m[nm++] = (uint8_t)i; break; }
    }
    apply_preset_to(t, nm ? m[rng() % nm] : na ? any[rng() % na] : rng() % n);   /* (the role's, always: surprises
                                                         * come from the twist, not from a wrong sound) */
    for (i = 0; i < 8u; i++) {                          /* (a sixth of each range: still that sound, a bit its own) */
        const param_desc_t *d = track_desc(t, P_E0 + i);
        int32_t span = (d->max - d->min) / 6;
        if (!d->label || d->label[0] == '-' || d->max <= d->min || d->fmt == F_ENUM) continue;
        t->p[P_E0 + i] = (int16_t)dice_by(t->p[P_E0 + i], d->min, d->max, span ? span : 1);
    }
    /* (0.6.34) the envelope for the role and the energy: pads that swell in space, plucked arps and basses, a lead
     * that speaks; the engines' own colour a little warmer (FM6 its modulation down, PHASE its DCW down, ANALOG a
     * little detune on the chords, a sub an octave down now and then) */
    switch (role) {
    case RL_PAD:  t->p[P_ATK] = (int16_t)(en_ == 0u ? rr(60, 100) : rr(20, 70)); t->p[P_REL] = (int16_t)rr(75, 110);
                  t->p[P_SUS] = (int16_t)rr(100, 120); break;
    case RL_TEX:  t->p[P_ATK] = (int16_t)rr(70, 110); t->p[P_REL] = (int16_t)rr(90, 120); break;
    case RL_LEAD: t->p[P_ATK] = (int16_t)rr(2, en_ == 0u ? 40 : 20); t->p[P_REL] = (int16_t)rr(40, 80); break;
    case RL_ARP:  t->p[P_ATK] = (int16_t)rr(0, 8); t->p[P_DEC] = (int16_t)rr(30, 62); t->p[P_SUS] = (int16_t)rr(20, 70);
                  t->p[P_REL] = (int16_t)rr(40, 75); break;
    case RL_BASS: t->p[P_ATK] = 0; t->p[P_DEC] = (int16_t)rr(40, 70); t->p[P_SUS] = (int16_t)rr(50, 105);
                  t->p[P_REL] = (int16_t)rr(18, 40); break;
    }
    if (e == EG_FM) t->p[P_E2] = (int16_t)clamp(t->p[P_E2] - rr(4, 24), -64, 63);
    if (e == EG_PH) t->p[P_E2] = (int16_t)clamp(t->p[P_E2] - rr(0, 20), 20, 127);
    if (e == EG_A && role == RL_PAD) t->p[P_E1] = (int16_t)rr(12, 32);
    if (e == EG_A && role == RL_BASS && chance(30)) t->p[P_E7] = -12;
    {   /* filtered: the cutoff down by role (ANALOG its own filter, the others the part's), some resonance; never flat:
         * the TYPE changes (LP mostly; band-pass, the comb tuned to the note for the arps and the textures), the
         * envelope opens the filter on every sound, a slow LFO of its own shape on some cutoffs (S&H: the IDM's),
         * a little grit on some basses and leads */
        static const uint8_t CUT[6][2] = {{0, 0}, {36, 66}, {48, 70}, {58, 82}, {48, 72}, {44, 76}};
        static const uint8_t ENV[6][2] = {{0, 0}, {18, 48}, {8, 28}, {12, 36}, {22, 50}, {0, 24}};
        static const uint8_t FT[6][4] = {{100, 0, 0, 0}, {100, 0, 0, 0}, {65, 0, 30, 5}, {75, 0, 20, 5}, {55, 0, 30, 15},
                                         {30, 10, 35, 25}};   /* LP HP BP COMB, percent */
        int32_t cut = rr(CUT[role][0], CUT[role][1]), res = rr(role == RL_BASS ? 12 : 20, role == RL_ARP ? 62 : 48);
        uint32_t ft = 0, w = rng() % 100u, acc = 0;
        if (role == RL_DRUM) return;
        for (ft = 0; ft < 3u; ft++) { acc += FT[role][ft]; if (w < acc) break; }
        if (dh.twist == TW_COMB && (role == RL_PAD || role == RL_ARP)) ft = 3;
        t->p[P_FTYPE] = (int16_t)ft;
        if (ft == 2u) cut += 10;                        /* (a band-pass: higher, or it is only a rumble) */
        if (e == EG_A) {
            t->p[P_E4] = (int16_t)(t->p[P_E4] < cut ? t->p[P_E4] : cut);
            t->p[P_E5] = (int16_t)clamp(t->p[P_E5], res, 90);
        } else {
            t->p[P_FCUT] = (int16_t)(role == RL_BASS ? cut + 24 : cut);       /* (a bass: only its fizz taken) */
            t->p[P_FRES] = (int16_t)res;
        }
        t->p[P_ED_FLT] = (int16_t)rr(ENV[role][0], ENV[role][1]);
        if (role == RL_PAD || role == RL_TEX || chance(35)) {
            static const uint8_t LW[4] = {0, 1, 4, 0};  /* SIN TRI S&H */
            t->p[P_LWAVE] = (int16_t)(role == RL_ARP && chance(50) ? 4 : LW[rng() % 4u]);
            t->p[P_LRATE] = (int16_t)(t->p[P_LWAVE] == 4 ? rr(30, 70) : rr(4, 26));
            t->p[P_LD_FLT] = (int16_t)rr(8, role == RL_TEX ? 34 : 24);
        }
        if ((role == RL_BASS || role == RL_LEAD) && chance(35)) {
            t->p[P_DIST] = (int16_t)rr(10, 38);
            t->p[P_DTYPE] = (int16_t)(chance(60) ? 0 : 2);   /* SOFT, or FOLD */
        }
    }
}

/* a part that sits high plays quieter: its level down with its average note above E4 (and its highest) */
static void dlevel(track_t *t, uint32_t len)
{
    uint32_t i, j, cnt = 0;
    int32_t sum = 0, top = 0, m;
    for (i = 0; i < len; i++)
        if (t->step[i].time == ST_NOTE)
            for (j = 0; j < t->step[i].n; j++) { sum += t->step[i].note[j]; cnt++; if (t->step[i].note[j] > top) top = t->step[i].note[j]; }
    if (!cnt) return;
    m = sum / (int32_t)cnt;
    if (m > 64) t->p[P_LEVEL] = (int16_t)clamp(t->p[P_LEVEL] - (m - 64) * 2, 50, 127);
    if (top > 79) t->p[P_LEVEL] = (int16_t)clamp(t->p[P_LEVEL] - (top - 79), 50, 127);
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
static int8_t dice_force_style = -1, dice_force_energy = -1;   /* (a film's script: this genre, this energy; -1 any) */
static void dice_one(void)
{
    const dstyle_t *st = &DSTYLE[dice_force_style >= 0 ? (uint32_t)dice_force_style % NDSTYLE :
                                 chance(25) ? rng() % DSTYLE_IDM : rng() % NDSTYLE];   /* (IDM, DARK IDM more often) */
    uint32_t k, dr = NTRK, role[NTRK], nlive = trk_live < NTRK ? trk_live : NTRK, act[RL_N], nmel, drums, next;
    int16_t root = (int16_t)rr(0, 11), scale = (int16_t)st->scale[rng() % 4u];
    uint32_t pr = rng() % 3u, wet;
    dice_fm6 = 0;
    if (st->prog[pr][0] == 127) pr = 0;
    dh.prog = st->prog[pr];
    for (dh.n = 0; dh.n < 4u && dh.prog[dh.n] != 127; dh.n++) {}
    dh.steps = 16u * st->bars;
    if (dh.steps * dh.n > NSTEP) dh.steps = NSTEP / dh.n;   /* (the whole progression inside the 64 steps) */
    dh.voicing = st->voicing;
    dh.nlast = 0;
    dh.energy = dice_force_energy >= 0 ? (uint32_t)dice_force_energy : (uint32_t)rr(st->e_lo, st->e_hi);
    dh.twist = chance(20) ? 1u + rng() % 5u : TW_NONE;
    /* the orchestration: the genre's melodic roles in their order, as many as the energy asks (SPACE 2..3, MID 3..4,
     * FRANTIC all five), now and then two swapped; the drums as often as the genre plays them (less in SPACE, more
     * FRANTIC); never fewer than two melodic voices, three without drums (or a pad among the two) */
    {
        uint8_t pri[5];
        for (k = 0; k < 5u; k++) pri[k] = st->prio[k];
        if (chance(30)) { uint32_t a = 1u + rng() % 3u; uint8_t x = pri[a]; pri[a] = pri[a + 1u]; pri[a + 1u] = x; }
        nmel = dh.energy == 0u ? (uint32_t)rr(2, 3) : dh.energy == 1u ? (uint32_t)rr(3, 4) : 5u;
        drums = st->drums != DS_NONE &&
                chance((uint32_t)clamp((int32_t)st->drum_pct + (dh.energy == 0u ? -30 : dh.energy == 2u ? 20 : 0), 0, 100));
        if (!drums && nmel < 3u && pri[0] != RL_PAD && pri[1] != RL_PAD) nmel = 3u;
        for (k = 0; k < RL_N; k++) act[k] = 0;
        act[RL_DRUM] = drums;
        for (k = 0; k < nmel; k++) act[pri[k]] = 1;
        /* the tracks: the drums on the DRUM track (none: the fourth, or the last live); the melodic ones in order, the
         * playing roles first (so TRACKS 4 hears them), then the silent ones */
        for (k = 0; k < NTRK && dr == NTRK; k++)
            if (drum_track(&trk[k])) dr = k;
        if (dr == NTRK) { dr = nlive > 3u ? 3u : nlive - 1u; set_engine_of(&trk[dr], ENGI_DRUM); }
        next = 0;
        for (k = 0; k < NTRK; k++) {
            if (k == dr) { role[k] = RL_DRUM; continue; }
            role[k] = next < 5u ? pri[next] : RL_TEX;
            next++;
        }
    }
    for (k = 0; k < NTRK; k++) {                        /* (the scale first: the parts are written in it) */
        trk[k].p[P_ROOT] = root;
        trk[k].p[P_SCALE] = scale;
    }
    sl_root = root; sl_scale = scale;
    wet = (uint32_t)clamp((int32_t)st->wet + (dh.energy == 0u ? 25 : dh.energy == 2u ? -20 : 0), 0, 100);
    for (k = 0; k < NTRK; k++) {
        track_t *t = &trk[k];
        uint32_t r = role[k], len = dh.steps * dh.n, plays = act[r];
        if (len < 16u) len = 16u;
        if (chance(st->odd) && (r == RL_ARP || r == RL_TEX || (r == RL_LEAD && chance(40))))
            len = (uint32_t)rr(5, 7) * 2u + (chance(50) ? 1u : 0u);   /* (an odd loop against the others: 10 .. 15) */
        if (dh.twist == TW_POLY && r == RL_LEAD) len = chance(50) ? 12u : 20u;   /* (a lead in another metre) */
        if (r == RL_DRUM) dice_kit(t, st->kit[rng() & 1u]);
        else dice_sound(t, r, r == RL_BASS ? st->e_bass : r == RL_PAD ? st->e_pad : r == RL_LEAD ? st->e_lead :
                              r == RL_ARP ? st->e_arp : st->e_tex);
        t->p[P_ROOT] = root; t->p[P_SCALE] = scale;
        t->p[P_VOICE] = (int16_t)(r == RL_PAD ? V_POLY : r == RL_ARP ? (chance(50) ? V_POLY : V_MONO) : V_LEGATO);
        t->p[P_SGATE] = (int16_t)(r == RL_BASS ? (st->bass == BS_PUMP || st->bass == BS_BLIP || st->bass == BS_ROLL ||
                                                  st->bass == BS_RUMBLE || st->bass == BS_PUNK ? 40 : 75) :
                                  r == RL_PAD ? (st->pads == PS_DUB || st->pads == PS_STAB ? rr(25, 45) : 120) :
                                  r == RL_ARP ? (st->arp == AR_GLASS ? rr(60, 80) : rr(30, 55)) : r == RL_LEAD ? 85 : 120);
        t->p[P_SSWING] = 0;
        t->p[P_LEVEL] = (int16_t)(r == RL_DRUM ? 106 : r == RL_BASS ? 102 : r == RL_LEAD ? 88 : r == RL_TEX ? 70 : 82);
        t->p[P_PAN] = (int16_t)(r == RL_ARP ? rr(-30, -10) : r == RL_TEX ? rr(10, 30) : r == RL_PAD ? rr(-8, 8) :
                                r == RL_LEAD ? rr(-6, 12) : 0);
        /* the sends: by role, scaled by how wet the genre and the energy are; now and then one soaked, one dry */
        t->p[P_REV] = (int16_t)((r == RL_PAD || r == RL_TEX ? rr(45, 95) : r == RL_LEAD ? rr(30, 70) : r == RL_ARP ? rr(20, 60) :
                                 r == RL_DRUM ? rr(4, 20) : rr(0, 8)) * (40 + (int32_t)wet) / 100);
        t->p[P_DLY] = (int16_t)((r == RL_LEAD || r == RL_ARP ? rr(25, 70) : r == RL_PAD && st->pads == PS_DUB ? rr(55, 95) :
                                 r == RL_DRUM ? rr(0, 10) : 0) * (40 + (int32_t)wet) / 100);
        if (r != RL_DRUM && r != RL_BASS && chance(25)) {
            if (chance(50)) { t->p[P_REV] = (int16_t)rr(85, 115); t->p[P_DLY] = (int16_t)rr(60, 95); }
            else { t->p[P_REV] = (int16_t)rr(0, 10); t->p[P_DLY] = 0; }
        }
        if (r == RL_PAD && chance(40)) t->p[P_CHOR] = (int16_t)rr(20, 60);
        load_begin(t, LOAD_PAT);
        track_defaults_steps(t);
        if (plays) {
            switch (r) {
            case RL_DRUM: dice_drums(t, st, len); break;
            case RL_BASS: dice_bass(t, st, len); break;
            case RL_PAD: dice_pads(t, st, len); break;
            case RL_LEAD: dice_lead(t, st, len); break;
            case RL_ARP: dice_arp(t, st, len); break;
            default: dice_texture(t, st, len); break;
            }
        }
        t->p[P_SLEN] = (int16_t)len;
        if (r != RL_DRUM) dlevel(t, len);
        load_end(t);
        pat_sig[k] = steps_sig(t);
    }
    if (dh.twist == TW_PHASE) {                         /* the twist: the arp (or the lead) copied onto a silent track,
                                                         * a step shorter: Reich's phase, the two drifting apart */
        uint32_t src = NTRK, dst = NTRK, i;
        for (k = 0; k < NTRK; k++) {
            if (src == NTRK && (role[k] == RL_ARP || role[k] == RL_LEAD) && act[role[k]]) src = k;
            else if (dst == NTRK && role[k] != RL_DRUM && !act[role[k]] && k < nlive) dst = k;
        }
        if (src < NTRK && dst < NTRK && trk[src].p[P_SLEN] > 4) {
            track_t *d = &trk[dst];
            load_begin(d, LOAD_PAT);
            for (i = 0; i < NSTEP; i++) d->step[i] = trk[src].step[i];
            d->p[P_SLEN] = (int16_t)(trk[src].p[P_SLEN] - 1);
            d->p[P_PAN] = (int16_t)-trk[src].p[P_PAN];
            d->p[P_LEVEL] = (int16_t)(trk[src].p[P_LEVEL] - 8);
            load_end(d);
            pat_sig[dst] = steps_sig(d);
        }
    }
    if (!(ui_prefs & PREF_BPM_LOCK)) {                  /* the tempo: SPACE in the genre's lower half, FRANTIC its upper;
                                                         * always at least 3 BPM away */
        int32_t old = song.g[G_BPM], lo = st->bpm_lo, hi = st->bpm_hi, mid = (lo + hi) / 2, v, t;
        if (dh.energy == 0u) hi = mid; else if (dh.energy == 2u) lo = mid;
        v = rr(lo, hi);
        for (t = 0; t < 8 && (v - old < 3 && old - v < 3); t++) v = rr(lo, hi);
        if (v - old < 3 && old - v < 3) v = old + (old < 150 ? 5 : -5);
        song.g[G_BPM] = (int16_t)v;
    }
    song.g[G_SWING] = (int16_t)clamp((int32_t)st->swing + rr(-4, 4), 0, 100);
    {   /* the space: the delay the genre's time or another musical one, darker repeats, more feedback when wet */
        static const uint8_t DT[6] = {0, 1, 4, 5, 2, 0};   /* 1/4 1/8 8T 16T 1/16 */
        song.g[G_DTIME] = chance(70) ? st->dtime[rng() & 1u] : DT[rng() % 6u];
    }
    song.g[G_DFDBK] = (int16_t)clamp(rr(25, 55) + (int32_t)wet / 3, 0, 95);
    song.g[G_DMIX] = (int16_t)rr(70, 110);
    song.g[G_DCOLOR] = (int16_t)rr(28, 58);
    {   /* the GRAIN delay's pitch: down an octave, a fourth or up a fifth, often on IDM (always with the twist) */
        static const int8_t DP[4] = {-12, -5, 7, -12};
        song.g[G_DPIT] = dh.twist == TW_GRAIN || chance(st->drums == DS_IDM ? 50u : 12u) ? DP[rng() % 4u] : 0;
    }
    song.g[G_DSPRY] = song.g[G_DPIT] ? (int16_t)rr(0, 50) : 0;
    {   /* the reverb: the genre's, or any of the dark four (no SHIMMER); bigger when wet */
        static const uint8_t RT[4] = {0, 1, 3, 4};
        song.g[G_RTYPE] = chance(70) ? st->rtype[rng() & 1u] : RT[rng() % 4u];
    }
    song.g[G_RSIZE] = (int16_t)clamp(rr(40, 80) + (int32_t)wet / 2, 0, 127);
    song.g[G_RRATE] = (int16_t)rr(10, 60);
    song.g[G_RWIDE] = (int16_t)rr(20, 100);
    song.g[G_RDAMP] = (int16_t)rr(55, 100);
    song.g[G_RFILT] = (int16_t)rr(-40, -8);
    song.g[G_RMOD] = (int16_t)rr(0, 80);
    song.g[G_RPRE] = (int16_t)(song.g[G_RTYPE] == 3 ? rr(0, 4) : song.g[G_RTYPE] == 2 || song.g[G_RTYPE] == 4 ? rr(-12, 0) : rr(0, 30));
    song.g[G_CRATE] = (int16_t)rr(10, 60);
    song.g[G_CDEPTH] = (int16_t)rr(20, 80);
    song.g[G_STRN] = 0;
    song.g[G_SLPQ] = 1;
    macro_dice(3);
    ui_say("DICED: ", st->name);
    str_cpy(ui.msg2, dh.energy == 0u ? "SPACE  UNDO:GLO+G4" : dh.energy == 2u ? "FRANTIC UNDO:GLO+G4" : "UNDO: GLO + G4",
            sizeof ui.msg2);
    ui.force = 1;
}
