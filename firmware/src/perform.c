/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
 * Portions Copyright (C) 2026 Juan Ignacio Antonucci (Juan Jiant) */
/* PERFORM: the effects of the FX hold layer, on the master. A key pressed while FX is held belongs to the
 * layer (seq.c keyboard_block): it plays no note, sends no MIDI, records nothing, and holds its effect until
 * it is let go (ui_input.c opens the layer and shows the map). The 5 white keys from the left (F3 .. C4):
 *   REPEAT 1/8 1/16 1/32, (0.9.14) GLITCH, TAPE STOP; then (JIANT) the 10 punch-in MIDI effects D4 .. F5 (pfx.c); black
 *   keys 1..6 (F#3 .. F#4): tracks 1..6 muted while held (not P_MUTE, never saved); A#4 the MIDI effects' tracks;
 *   (0.9.14) C#5 D#5 F#5 the SWEEPS: FILTER, CRUSH, THROW (G#4 none).
 * TONIC keeps only the audio stutters (REPEAT): REVERSE, FREEZE and OCT UP / DN (the harmonizer) were removed; the
 * punch-in effects of TONIC act on the notes instead (FELUCCA-TONIC-SPEC.md).
 * The buffer effects (REPEAT, GLITCH, TAPE) start on the next 1/16 (at once while stopped) and end when let go, with a
 * 2.9 ms ramp; of them the last pressed plays, and letting it go returns to the one held before.
 *   GLITCH: the master cut on the transport's 1/16s, each one (a hash of its place in a 2-bar cycle: the same every
 *     time, so it grooves; the downbeats mostly left alone) played live, or the last 1/16 .. 1/64 repeated, the last
 *     1/16 reversed, an octave up or down, or gated at 1/32 .. 1/64; every slice windowed (SL_RAMP), live in and out
 *     by the buffer's share (no click). Below ~82 BPM only its gates and the live slices (the loop holds 371 ms).
 *   TAPE STOP: the master read from the loop at a speed falling from 1 to 0 over a beat (at most 680 ms: the loop),
 *     the pitch with it, faded out over its last quarter; silent then while held.
 * The SWEEPS (0.9.14; until 0.9.13 the knob macros FILTER CRUSH THROW and the keys LPF HPF): held, their mix rises
 * from dry to wet over 2 bars at the tempo, on an S curve (slow out of dry, slow into wet); let go, back to dry over a
 * beat (1/4 bar) on the same curve. FILTER: the master DJ FILTER's high-pass opening (fx.c djf_block, ~32 Hz .. ~6 kHz);
 * CRUSH: the bits and the rate reduced (4 .. 11 bits off, a sample held 1 .. 8), its mix with them; THROW: the dry mix
 * (and the SLICER's bus) into the delay and reverb sends, up to all of it.
 * The knobs with FX (0.9.14): the PERFORMANCE MACROS on the whole project, built up turning right: STEPS (every
 * sequence's window 1/2 1/4 1/8 1 step, from where it plays: seq_live.c LOOP), TRANSPOSE (+2ND +3RD +4TH +5TH +6TH
 * along the scale, +OCT, -OCT: seq.c strn_note; not the kits), MUTATE (project.c perf_mut_poll: the patterns mutated
 * around an anchor, every bar), FILTER (the DJ FILTER's low-pass closing). Any knob turned back (two detents) puts all
 * four off at once: the music as it was. Never saved, never recorded.
 * MENU > FX LATCH ON (#40, a hand that cannot hold FX, a key and a knob together): a key pressed with FX turns its
 * effect on, the next press turns it off (perf_latched; letting the key or FX go does nothing), FX + OCT- turns
 * everything off (ui_layer.c), so does the menu or a dialog.
 * Chain: the buffer effect -> CRUSH, after the master level and before master_out (the limiter); THROW and the mutes
 * act before the buses (fx.c mix_block / mix_part), FILTER in the DJ FILTER.
 * The buffer: the SLICER's recordings (sl_buf, 32 KB) borrowed as one stereo loop of 8192 frames at 22.05 kHz
 * (371 ms). While borrowed, STUT tracks play live; their recordings are dropped afterwards. A REPEAT 1/8 longer
 * than the loop at the tempo (below 81 BPM) does nothing (the map shows it dimmed).
 * Idle (no key, no ramp left) every stage is skipped: the output is bit-identical. */
enum { PF_R8, PF_R16, PF_R32, PF_GLT, PF_TAPE,
       PF_OCTD, PF_OCTU, PF_HALF, PF_DSHT, PF_DLNG, PF_S8, PF_S16, PF_ATK, PF_ARP, PF_RND,   /* the MIDI ones: pfx.c */
       PF_SFLT, PF_SCRU, PF_STHR,                          /* (0.9.14) the sweeps: C#5 D#5 F#5 */
       PF_M1, PF_N = PF_M1 + NTRK, PF_TGT = PF_N + 1,     /* PF_TGT: A#4, the MIDI effects' tracks (pfx_tgt) */
       PF_CLR = PF_N + 2 };                                /* PF_CLR: G5, erases the punch-in lane (pfx.c) */
#define PF_BIT(e) (1u << (e))
#define PF_REPEAT 0x7u                                         /* the three REPEAT rates */
#define PF_BUF 0x1Fu                                           /* the buffer effects: REPEAT, GLITCH, TAPE */
#define PF_Q PF_BUF                                            /* start on the 1/16 */
#define PF_SWEEP (7u << PF_SFLT)
#define PF_MUTE (((1u << NTRK) - 1u) << PF_M1)
#define PF_MIDI (((1u << (PF_RND + 1)) - 1u) & ~((1u << PF_OCTD) - 1u))   /* PF_OCTD .. PF_RND: on the notes (pfx.c) */
#define PF_LANE (PF_MIDI | PF_SWEEP | PF_BIT(PF_GLT) | PF_BIT(PF_TAPE))  /* (0.9.14) what the punch-in lane records */
#define PB_FRAMES SL_FR                       /* stereo frames in sl_buf (a power of 2: the mask), 371 ms at 22.05 kHz */
_Static_assert(PB_FRAMES <= SL_FR && !(PB_FRAMES & (PB_FRAMES - 1u)), "the REPEAT loop in sl_buf");
#define PB_MAX (2u * PB_FRAMES)               /* the longest loop, 44.1 kHz samples */
#define GL_SPAN 7680u                         /* GLITCH: the longest slice it reads back (the loop holds two and a margin) */
#define TP_MAX 30000u                         /* TAPE STOP: its longest stop (it falls T / 4 frames behind) */
enum { BM_NONE, BM_LOOP, BM_GLT, BM_TAPE };
enum { GA_PASS, GA_RPT, GA_REV, GA_UP, GA_DN, GA_GATE };

static volatile uint32_t perf_mask;   /* main: the FX button's bit while its layer may own keys, 0 = none */
static volatile uint32_t kb_mask;     /* main: the button bits whose hold makes keys a layer's (ui_layer.c), 0 = none */
static volatile uint8_t perf_solo;    /* main: tracks soloed in the GLO layer (the others muted as by a black key) */
static volatile uint32_t scene_pfx;  /* (JIANT) the punch-in MIDI effects a song row's scene holds (song_play.c), PF bits */
static volatile uint8_t perf_kill;    /* main: every effect off (the menu, a dialog, UBOOT) */
static volatile uint8_t perf_m[4];    /* main (0.9.14): the PERFORMANCE MACROS, 0 OFF: STEPS 0..12 (three detents a
                                       * stage), TRANSPOSE 0..21 (three an interval), MUTATE, FILTER 0..100 (PM_MAX) */
static const uint8_t PM_MAX[4] = {12, 21, 100, 100};
static volatile int8_t perf_djf;      /* ISR: the FX layer's DJ FILTER (fx.c djf_v): - LP (FILTER macro), + HP (sweep) */
static volatile uint32_t kb_layer;    /* ISR: the keys held that are the layer's, a bit per key */
static volatile uint32_t perf_held;   /* ISR: the effects held (PF_*) */
static volatile uint8_t perf_latch_on;   /* main: MENU > FX LATCH ON (#40): a key with FX toggles its effect */
static volatile uint32_t perf_latched;   /* ISR: the effects latched (FX LATCH; main clears it: OCT-, the menu) */
static volatile uint32_t perf_act;    /* ISR: .. running (a 1/16 one from its start on) */
static uint32_t pfx_lfx;              /* ISR (0.9.14): the audio effects the punch-in lane plays this block (pfx.c) */
static uint32_t perf_ord[PF_N], perf_seq;   /* press order: the last pressed buffer effect plays */

static struct {
    uint32_t ph, P, split;             /* samples since play; the 1/16; this block: where a 1/16 starts */
    uint32_t act, act0;                /* running after / before split */
    uint8_t busy, sync, mute;          /* sync: a 1/16 starts with the next block; mute: tracks to ramp */
    /* the buffer */
    uint8_t mode, src, next;           /* BM_*, the effect it plays (PF_N none), one waiting for the fade */
    uint8_t state, odd;                /* LOOP: 0 recording, 1 playing; the sample of a pair */
    uint32_t len, wr, rp;              /* loop length (samples), frames written, read (samples into the loop) */
    uint32_t xrp, xlen, xn;            /* the loop it switched from, faded over xn samples */
    int32_t w, tw, hl, hr;             /* the buffer's share and its target, Q15; the first of a pair */
    /* GLITCH: the slice's action, segment, samples into it and segments past, its 1/16 (count), the loop's sample at
     * its start, samples into the 1/16; TAPE: the read place (samples, Q16), the speed (Q24) and its fall a sample */
    uint8_t ga;
    uint32_t gl, gu, gk, gs, g0, gph;
    uint32_t tp, tv, td1;
    /* the sweeps (Q24 progress: 0 dry), CRUSH, THROW, mutes */
    uint32_t sw[3];
    int32_t ss[3];                     /* .. on the S curve, Q15 */
    int32_t cw, chl, chr;              /* CRUSH share, held sample */
    uint32_t cn;
    int32_t td;                        /* THROW: the share of the dry mix sent, Q15 */
    int32_t mg[NTRK];                  /* mute gains, Q15 (32768 = open) */
} pf = {.src = PF_N, .next = PF_N, .mg = {[0 ... NTRK - 1] = 32768}};   /* (every track open: 0.6, six) */

/* the loop length of a REPEAT at the tempo, 44.1 kHz samples; 0 = not one */
static const uint8_t PF_DEN[PF_R32 + 1] = {2, 4, 8};
static uint32_t perf_len(uint32_t e) { return e <= PF_R32 ? beat_samples() / PF_DEN[e] : 0u; }
/* the effects that can run at the tempo: a REPEAT longer than the loop cannot */
static uint32_t perf_avail(void)
{
    uint32_t e, m = ~0u, b = beat_samples();
    for (e = PF_R8; e <= PF_R32; e++)
        if (b > PB_MAX * PF_DEN[e])
            m &= ~PF_BIT(e);
    return m;
}

/* seq.c keyboard_block: a layer key down / up (e: PF_*, PF_N = no effect) */
static volatile uint8_t pfx_tgt;      /* main / keys: the MIDI effects' tracks, 0 ALL, 1 SYN, 2 DRM (pfx.c) */
static volatile uint8_t pfx_clr;      /* keys: G5 held (pfx.c: the lane erased where it passes, or all of it) */
static uint32_t pfx_lph;              /* samples into the punch-in lane (pfx.c): from the transport's start, a section's */
static uint8_t pfx_lane[32], pfx_ltgt;   /* the section's punch-in lane (pfx.c), its tracks (as pfx_tgt) */
static void perf_press(uint32_t e, int down)
{
    if (e == PF_TGT && down)
        pfx_tgt = (uint8_t)((pfx_tgt + 1u) % 3u);
    if (e == PF_CLR)
        pfx_clr = (uint8_t)(down ? 1u : 0u);
    if (e >= PF_N)
        return;
    if (perf_latch_on) {                  /* FX LATCH: a press turns it on or off, letting go does nothing */
        if (down) {
            perf_latched ^= PF_BIT(e);
            perf_ord[e] = ++perf_seq;
        }
        return;
    }
    if (down) {
        perf_held |= PF_BIT(e);
        perf_ord[e] = ++perf_seq;
    } else {
        perf_held &= ~PF_BIT(e);
    }
}

static void perf_start(void) { pf.ph = 0; pf.sync = 1; pfx_lph = 0; }   /* seq_start: a 1/16 starts with the transport */

/* the SLICER's recordings dropped (when the buffer is taken and given back) */
static void perf_drop_slicer(void) { slicer_drop(); }

/* the buffer effect to play: the last pressed of those running */
static uint32_t perf_pick(uint32_t a)
{
    uint32_t e, best = PF_N;
    a &= PF_BUF;
    for (e = 0; e < PF_M1; e++)
        if (((a >> e) & 1u) && (best == PF_N || perf_ord[e] - perf_ord[best] < 0x80000000u))
            best = e;
    return best;
}

static void perf_glt_step(void);
static void perf_buf_start(uint32_t e)
{
    if (!sl_lent) {
        sl_lent = 1;
        perf_drop_slicer();
    }
    pf.src = (uint8_t)e;
    pf.next = PF_N;
    pf.wr = pf.rp = 0;
    pf.odd = 0;
    pf.xn = 0;
    pf.mode = e == PF_GLT ? BM_GLT : e == PF_TAPE ? BM_TAPE : BM_LOOP;
    pf.state = 0;
    pf.len = perf_len(e);
    memset(sl_buf[PB_FRAMES - 2u], 0, 2u * sizeof sl_buf[0]);   /* (what GLITCH / TAPE read before the start: quiet) */
    if (e == PF_GLT) {                    /* its first 1/16: the transport's (this block's split starts the next one) */
        pf.gs = song.playing && clk_pos != CLK_START ? clk_n + (pf.split ? 1u : 0u) : 0u;
        perf_glt_step();
    }
    pf.tp = (PB_MAX - 4u) << 16;          /* TAPE: two frames behind (quiet), at full speed falling to 0 over a beat */
    pf.tv = 1u << 24;
    {
        uint32_t t = beat_samples();
        pf.td1 = (1u << 24) / (t < TP_MAX ? (t ? t : 1u) : TP_MAX);
    }
}

/* the buffer effect to play becomes e (at a 1/16): a shorter REPEAT of what is
 * recorded switches with a cross-fade, anything else fades the old one out first */
static void perf_buf_select(uint32_t e)
{
    if (e == pf.src && pf.next == PF_N)
        return;
    if (e != PF_N && pf.mode == BM_LOOP && pf.state && e <= PF_R32 && pf.src != PF_N &&
        perf_len(e) <= 2u * pf.wr) {
        pf.xrp = pf.rp;
        pf.xlen = pf.len;
        pf.xn = SL_RAMP;
        pf.src = (uint8_t)e;
        pf.next = PF_N;
        pf.len = perf_len(e);
        pf.rp = 0;
        return;
    }
    if (pf.mode == BM_NONE || !pf.w) {
        if (e == PF_N) {
            pf.src = PF_N;
            pf.next = PF_N;
        } else {
            perf_buf_start(e);
        }
        return;
    }
    pf.src = PF_N;                                  /* fade out, then the next one (perf_seg) */
    pf.next = (uint8_t)e;
}

/* each block, after the keys (seq.c) and before the parts: what runs, the 1/16, the sweeps, the mutes.
 * Returns 1 when a stage has something to do */
/* (0.6.19) a track let go from its mute, silent (its voices ended at 0, perf_hush_end): nothing to ramp back, open at
 * once; else mix_part never reached perf_mute again (no signal) and the FX layer stayed busy every block */
static __attribute__((noinline)) void perf_unhush(void)   /* (out of perf_begin: target_budget.py) */
{
    uint32_t k, i, on;
    for (k = 0; k < NTRK; k++)
        if (!pf.mg[k] && !((pf.act >> (PF_M1 + k)) & 1u)) {
            for (i = on = 0; i < NVOICE; i++)
                on |= trk[k].v[i].active;
            if (!on)
                pf.mg[k] = 32768;
        }
}
/* (0.9.14) the sweeps: held, 2 bars from dry to wet; let go, a beat back; their S curve (3 p^2 - 2 p^3) */
static __attribute__((noinline)) void perf_sweeps(uint32_t n)
{
    uint32_t k, b = beat_samples(), up = (n << 24) / (8u * (b ? b : 1u)), dn = (n << 24) / (b ? b : 1u);
    for (k = 0; k < 3u; k++) {
        uint32_t x = pf.sw[k], p;
        if ((pf.act >> (PF_SFLT + k)) & 1u)
            x = x + up < (1u << 24) ? x + up : 1u << 24;
        else
            x = x > dn ? x - dn : 0u;
        pf.sw[k] = x;
        p = x >> 9;                                 /* (Q15) */
        pf.ss[k] = (int32_t)(((p * p) >> 15) * (3u * 32768u - 2u * p) >> 15);
    }
}
static __attribute__((noinline)) int perf_begin(uint32_t n)
{
    uint32_t held = perf_kill ? 0u : (perf_held | perf_latched | pfx_lfx | (perf_solo ? (~(uint32_t)perf_solo & ((1u << NTRK) - 1u)) << PF_M1 : 0u)) & perf_avail() & ~PF_MIDI;
    uint32_t q, k, ph0, bnd;
    int32_t lp = -(int32_t)(perf_m[3] * 64u / 100u);
    if (!held && !pf.busy) {                        /* idle: the clock only */
        pf.ph += n;
        pf.sync = 0;
        perf_act = 0;
        trk_hush = 0;
        perf_djf = (int8_t)lp;
        return 0;
    }
    pf.P = beat_samples() >> 2;
    if (pf.sync) {
        pf.ph = 0;
        pf.sync = 0;
    }
    if (song.playing && clk_pos != CLK_START)       /* (JIANT) playing: the transport's 1/16 (seq.c click_tick, through
                                                     * tempo changes and an external clock), so a REPEAT starts on the grid */
        pf.P = div_samples(2);
    ph0 = (song.playing && clk_pos != CLK_START ? clk_pos : pf.ph) % pf.P;   /* the 1/16 in this block (n: none) */
    bnd = ph0 ? (pf.P - ph0 < n ? pf.P - ph0 : n) : 0u;
    pf.ph += n;
    pf.act &= held;                                 /* let go: at once */
    pf.act |= held & (~PF_Q | pfx_lfx);             /* (the lane's: on its own step, already the 1/16's) */
    pf.act0 = pf.act;
    q = held & PF_Q & ~pf.act;
    pf.split = n;
    if (q && (!song.playing || bnd < n)) {
        pf.split = song.playing ? bnd : 0u;
        pf.act |= q;
    }
    perf_act = pf.act;
    perf_sweeps(n);
    k = (uint32_t)(pf.ss[0] * 63) >> 15;            /* FILTER: the high-pass, else the macro's low-pass */
    perf_djf = (int8_t)(k && (int32_t)k >= -lp ? (int32_t)k : lp);
    pf.mute = 0;
    perf_unhush();
    for (k = 0; k < NTRK; k++)
        if (((held >> (PF_M1 + k)) & 1u) || pf.mg[k] != 32768)
            pf.mute |= (uint8_t)(1u << k);
    trk_hush = (uint8_t)((pf.act >> PF_M1) & ((1u << NTRK) - 1u));   /* (0.6.15) muted: no new notes (voice.c) */
    pf.busy = 1;
    return 1;
}

/* a track muted by its black key: its signal ramps to 0 and back (fx.c mix_part, after the SLICER) */
static __attribute__((noinline)) void perf_hush_end(uint32_t k)   /* (out of perf_mute's loop: target_budget.py) */
{
    uint32_t i;
    for (i = 0; i < NVOICE; i++) {
        voice_t *v = &trk[k].v[i];
        v->active = v->gate = 0;
        v->stage = 0;
        v->env = v->env_out = 0;
    }
}
static __attribute__((noinline)) void perf_mute(uint32_t k, int32_t *b, uint32_t n)
{
    uint32_t i;
    int32_t g = pf.mg[k], t = (pf.act >> (PF_M1 + k)) & 1u ? 0 : 32768;
    for (i = 0; i < n; i++) {
        g += clamp(t - g, -SL_SLOPE, SL_SLOPE);
        b[i] = mulq16(b[i], (uint32_t)g << 1);
    }
    pf.mg[k] = g;
    if (!g && !t)                                       /* (0.6.15) silent: its voices end, the track costs nothing
                                                         * while muted (a MIDI mute: no new notes, trk_hush) */
        perf_hush_end(k);
}

/* before the buses: THROW (its sweep) adds the dry mix (and the SLICER's bus, bl / br) to the delay and reverb sends */
static __attribute__((noinline)) void perf_pre(const int32_t *ml, const int32_t *mr, const int32_t *bl, const int32_t *br,
                                               int32_t *sd, int32_t *sr, uint32_t n)
{
    uint32_t i;
    int32_t m = pf.ss[2];
    for (i = 0; i < n; i++) {
        pf.td += clamp(m - pf.td, -SL_SLOPE, SL_SLOPE);
        if (pf.td) {
            int32_t x = mulq16((ml[i] + mr[i] + bl[i] + br[i]) >> 1, (uint32_t)pf.td << 1);
            sd[i] += x;
            sr[i] += x;
        }
    }
}

/* x towards y by w (Q15, 32768 = y) */
static inline int32_t pf_mix(int32_t x, int32_t y, int32_t w)
{
    return w == 32768 ? y : x + mulq16(y - x, (uint32_t)w << 1);
}

/* the loop: frame f of it, both channels (Q15) */
static inline void pb_put(uint32_t f, int32_t l, int32_t r)
{
    int16_t *p = &sl_buf[0][0] + 2u * (f & (PB_FRAMES - 1u));
    p[0] = (int16_t)clamp(l, -32768, 32767);
    p[1] = (int16_t)clamp(r, -32768, 32767);
}
/* sample s of the frames recorded from 0 (n of them): between two frames on an odd one */
static inline void pb_at(uint32_t s, uint32_t n, int32_t *l, int32_t *r)
{
    uint32_t f = s >> 1 < n ? s >> 1 : n - 1u;    /* (an odd loop's last sample: the frame before) */
    const int16_t *p = &sl_buf[0][0] + 2u * (f & (PB_FRAMES - 1u));
    const int16_t *q;                               /* a frame is the mean of samples 2f, 2f + 1: sample s */
    if (s & 1u)                                     /* lies a quarter frame from it, towards f + 1 or f - 1 */
        q = f + 1u < n ? p + 2 : p;
    else
        q = f ? p - 2 : p;
    *l = (3 * p[0] + q[0]) >> 1;
    *r = (3 * p[1] + q[1]) >> 1;
}
/* (0.9.14) sample s of the loop as a ring (GLITCH, TAPE: frames recorded on and on) */
static __attribute__((noinline)) void pb_ring(uint32_t s, int32_t *l, int32_t *r)
{
    uint32_t f = s >> 1;
    const int16_t *p = &sl_buf[f & (PB_FRAMES - 1u)][0], *q = &sl_buf[(f + (s & 1u ? 1u : ~0u)) & (PB_FRAMES - 1u)][0];
    *l = (3 * p[0] + q[0]) >> 1;
    *r = (3 * p[1] + q[1]) >> 1;
}
/* REPEAT at sample rp of a loop of len, windowed at both ends */
static inline void pb_loop(uint32_t rp, uint32_t len, uint32_t n, int32_t *l, int32_t *r)
{
    uint32_t e = len - 1u - rp;
    pb_at(rp, n, l, r);
    if (rp < e)
        e = rp;
    if (e < (uint32_t)SL_RAMP) {
        *l = (*l * (int32_t)e) >> SL_RAMP_LOG2;
        *r = (*r * (int32_t)e) >> SL_RAMP_LOG2;
    }
}

/* record the pair into frame f (the mean, >> 1: headroom) */
static inline uint32_t pb_rec(uint32_t f, int32_t l, int32_t r)
{
    if (!pf.odd) {
        pf.hl = l;
        pf.hr = r;
        pf.odd = 1;
        return f;
    }
    pf.odd = 0;
    pb_put(f, (pf.hl + l) >> 2, (pf.hr + r) >> 2);
    return f + 1u;
}

/* (0.9.14) GLITCH: the next 1/16's slice, by the hash of its place in two bars */
static __attribute__((noinline)) void perf_glt_step(void)
{
    uint32_t h = ((pf.gs & 31u) + 0x51u) * 0x9E3779B1u, s16 = pf.P ? pf.P : 1u, sp = s16 < GL_SPAN ? s16 : GL_SPAN, a;
    h = (h ^ (h >> 15)) * 0x2C1B3C6Du;
    h ^= h >> 13;
    a = (h >> 8) % 8u;                              /* 0, 1 live; 2, 3 repeat; reverse, up, down, gate */
    if (!(pf.gs & 3u) && (h & 0x30u))
        a = 0;                                      /* (the beat, three times in four) */
    pf.ga = (uint8_t)(a < 2u ? GA_PASS : a < 4u ? GA_RPT : a - 2u);
    pf.g0 = 2u * pf.wr + pf.odd;                    /* (the loop's sample now) */
    if ((s16 + sp > PB_MAX - 512u || pf.g0 < sp) && pf.ga != GA_GATE)
        pf.ga = GA_PASS;                            /* (too slow: what it would read is gone; or not recorded yet) */
    pf.gl = pf.ga == GA_REV || pf.ga == GA_DN ? sp : pf.ga == GA_UP ? sp / 2u
          : (pf.ga == GA_RPT ? sp : s16 / 2u) >> ((h >> 12) % (pf.ga == GA_RPT ? 3u : 2u));
    if (pf.gl < 2u * SL_RAMP)
        pf.gl = 2u * SL_RAMP;
    pf.gu = pf.gk = pf.gph = 0;
}
/* GLITCH / TAPE on one sample: 1 = it plays (yl yr), 0 = live */
static __attribute__((noinline)) int perf_ring(int32_t l, int32_t r, int32_t *yl, int32_t *yr)
{
    uint32_t u = pf.gu, L = pf.gl, s = pf.g0, a = pf.ga, k = pf.gk, e, g = 32768u;
    pf.wr = pb_rec(pf.wr, l, r);
    if (pf.mode == BM_TAPE) {                       /* the place (samples, Q16) on by the speed, falling */
        s = pf.tp >> 16;
        g = pf.tv >> 7 < 32768u ? pf.tv >> 7 : 32768u;   /* (faded over its last quarter) */
        pf.tp += pf.tv >> 8;
        pf.tv = pf.tv > pf.td1 ? pf.tv - pf.td1 : 0u;
    } else {
        if (++pf.gu >= L) {
            pf.gu = 0;
            pf.gk++;
        }
        if (++pf.gph >= pf.P) {
            pf.gs++;
            perf_glt_step();
        }
        if (a == GA_PASS)
            return 0;
        s = a == GA_RPT ? s - L + u                     /* the last L again */
          : a == GA_REV ? s - 1u - u                    /* the last 1/16 backwards */
          : a == GA_UP ? s - 2u * L + 2u * u            /* .. twice as fast, twice */
          : s - L + u / 2u;                             /* .. its first half at half speed */
        e = L - 1u - u < u ? L - 1u - u : u;        /* (each segment windowed) */
        if (e < (uint32_t)SL_RAMP)
            g = e << (15 - SL_RAMP_LOG2);
        if (a == GA_GATE) {
            *yl = (k & 1u) ? 0 : mulq16(l, g << 1);
            *yr = (k & 1u) ? 0 : mulq16(r, g << 1);
            return 1;
        }
    }
    pb_ring(s, yl, yr);
    *yl = mulq16(*yl, g << 1);
    *yr = mulq16(*yr, g << 1);
    return 1;
}

/* the buffer effect faded out: the next one, or the buffer back. Once per effect, out of the per-sample loop
 * (noinline: its calls stay out of perf_block's loop code) */
static __attribute__((noinline)) void perf_buf_done(void)
{
    if (pf.next != PF_N) {
        perf_buf_start(pf.next);
    } else {
        pf.mode = BM_NONE;
        if (sl_lent) {
            sl_lent = 0;
            perf_drop_slicer();
        }
    }
}

/* the buffer effect on one sample: the share pf.w of what it plays, the rest live */
static __attribute__((noinline)) void perf_buf(int32_t *pl, int32_t *pr)   /* (0.9.14: out of perf_block's loop) */
{
    int32_t l = *pl, r = *pr, yl = 0, yr = 0, on = 0;
    if (pf.mode == BM_LOOP) {
        if (!pf.state) {
            pf.wr = pb_rec(pf.wr, l, r);
            if (++pf.rp >= pf.len) {                /* (rp: samples recorded) */
                pf.state = 1;
                pf.rp = 0;
            }
        } else {
            pb_loop(pf.rp, pf.len, pf.wr, &yl, &yr);
            if (pf.xn) {                            /* the loop it switched from, fading out */
                int32_t ol, orr, g = (int32_t)pf.xn;
                pb_loop(pf.xrp, pf.xlen, pf.wr, &ol, &orr);
                yl += (ol * g) >> SL_RAMP_LOG2;
                yr += (orr * g) >> SL_RAMP_LOG2;
                pf.xn--;
                if (++pf.xrp >= pf.xlen)
                    pf.xrp = 0;
            }
            if (++pf.rp >= pf.len)
                pf.rp = 0;
            on = 1;
        }
    } else {
        on = perf_ring(l, r, &yl, &yr);             /* (0.9.14) GLITCH, TAPE */
    }
    pf.tw = on && pf.src != PF_N ? 32768 : 0;
    pf.w += clamp(pf.tw - pf.w, -SL_SLOPE, SL_SLOPE);
    if (pf.w) {
        *pl = pf_mix(l, yl, pf.w);
        *pr = pf_mix(r, yr, pf.w);
    } else if (pf.src == PF_N) {                    /* faded out: the next one, or the buffer back */
        perf_buf_done();
    }
}

static __attribute__((noinline)) void perf_switch(uint32_t a) { perf_buf_select(perf_pick(a)); }

/* after the master level: the buffer effect, CRUSH on l / r (stereo, Q15) */
static __attribute__((noinline)) void perf_block(int32_t *bl, int32_t *br, uint32_t n)
{
    uint32_t i, a = pf.act0, sh, hold;
    int32_t ct = pf.ss[1];
    sh = 4u + (uint32_t)(ct * 7) / 32768u;
    hold = sh - 3u;
    if (pf.split)
        perf_switch(a);
    for (i = 0; i < n; i++) {
        int32_t l = bl[i], r = br[i];
        if (i == pf.split) {
            a = pf.act;
            perf_switch(a);
        }
        if (pf.mode != BM_NONE)
            perf_buf(&l, &r);
        {   /* CRUSH: fewer bits, a held sample (its sweep: both, and its mix) */
            pf.cw += clamp(ct - pf.cw, -SL_SLOPE, SL_SLOPE);
            if (pf.cw) {
                if (++pf.cn >= hold) {
                    pf.cn = 0;
                    pf.chl = ((l >> sh) << sh) + (1 << (sh - 1u));
                    pf.chr = ((r >> sh) << sh) + (1 << (sh - 1u));
                }
                l = pf_mix(l, pf.chl, pf.cw);
                r = pf_mix(r, pf.chr, pf.cw);
            }
        }
        bl[i] = l;
        br[i] = r;
    }
    pf.busy = pf.act || pf.mode != BM_NONE || pf.w || pf.cw || pf.td || pf.mute || pf.sw[0] || pf.sw[1] || pf.sw[2];
}
