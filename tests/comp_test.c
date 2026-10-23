/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* STEP COMPONENTS v2 (0.9.9-seq2d: core.h CA_* CC_*, seq.c comp_run / comp_migrate, project.c x bit 7, SEQ > COMP):
 * on the real sequencer, project and UI sources (with the stubs of tests/ui_test.c):
 *   CYCLE      four notes muted !1:4 .. !4:4 and a TRANSPOSE +1 R4 at 1:4 over the bar: 16 bars, each its own, then
 *              the same 16 again; the SEQ walker: a note further each pass.
 *   ACTIONS    RATCHET over its LENGTH (and its DECAY), STUTTER and MUTE replacing their steps, ARP over its LENGTH,
 *              JUMP, OCT; conditions N:M, FIRST, PRE.
 *   MONO/POLY  C2's condition ELSE (MONO: C1 wins, C2 when C1 did not act) against POLY (both stack).
 *   FORMAT     a v2 project round trip; a project of before seq2d (its CP_* and EVERY, no x bit 7) loads migrated:
 *              ARP over its TIEs, ARP SEQ the walker, TRIG 1:2 MUTE !1:2, OCT with EVERY 1:4 OCT+ 1:4, ECHO RATCHET.
 *   UI         SEQ > COMP / COMP 2: KNOB 1 ACTION, KNOB 2 LENGTH, KNOB 3 CONDITION (ELSE on COMP 2 alone), KNOB 4 VALUE.
 * Built and run by tests/run_tests.sh. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"

/* track 1 alone: a pattern of len steps of the notes nts (0: a REST), C major, GATE 64, no swing; the transport on */
static track_t *pat(uint32_t len, const uint8_t *nts)
{
    track_t *t;
    uint32_t i;
    ui_power_on();
    t = &trk[0];
    track_defaults_steps(t);
    t->p[P_SLEN] = (int16_t)len;
    t->p[P_SGATE] = 64;
    t->p[P_SSWING] = 0;
    song.g[G_SWING] = 0;
    t->p[P_ROOT] = 0; t->p[P_SCALE] = 1;
    for (i = 0; i < len; i++)
        t->step[i] = nts[i] ? (step_t){{nts[i]}, 1, ST_NOTE, 0, 100} : (step_t){{0}, 0, ST_REST, 0, 0};
    seq_start();
    return t;
}
static uint32_t step_len(const track_t *t) { return step_samples(t, div_samples((uint32_t)t->p[P_SDIV]), 0); }
/* the notes the voices start over `steps` steps, in order (st: the step each started in) */
static uint32_t run_at(track_t *t, uint32_t steps, uint8_t *nt, uint16_t *st, uint32_t max)
{
    uint32_t b, n = 0, a, k, blocks = steps * step_len(t) / CTL - 1u;
    for (b = 0; b < blocks; b++) {
        uint32_t v0 = vage;
        seq_tick(t, CTL);
        for (a = v0 + 1u; a <= vage; a++)
            for (k = 0; k < NVOICE; k++)
                if (t->v[k].age == a && n < max) {
                    if (st) st[n] = (uint16_t)(b * CTL / step_len(t));
                    nt[n++] = t->v[k].note;
                }
    }
    return n;
}
static uint32_t run(track_t *t, uint32_t steps, uint8_t *nt, uint32_t max) { return run_at(t, steps, nt, 0, max); }
static int seq_is(const uint8_t *nt, uint32_t n, const uint8_t *want, uint32_t m)
{
    uint32_t i;
    if (n != m) {
        printf("ui:   %u notes:", n);
        for (i = 0; i < n && i < 24u; i++) printf(" %u", nt[i]);
        printf("\n");
        return 0;
    }
    for (i = 0; i < n; i++)
        if (nt[i] != want[i]) return 0;
    return 1;
}
static void comp_set(step_t *s, uint32_t k, uint32_t a, uint32_t c, uint32_t nm, uint32_t len, uint32_t v)
{
    if (k) { s->comp2 = CO_MAKE(a, c); s->carg2 = (uint8_t)v; }
    else { s->comp = CO_MAKE(a, c); s->carg = (uint8_t)v; }
    s->clen[k] = (uint8_t)(len - 1u);
    s->cnm[k] = (uint8_t)nm;
}

static int cycle(void)
{
    int bad = 0, ok = 1;
    static uint8_t nt[512];
    static uint16_t at[512];
    uint32_t n, i, b;
    track_t *t;
    /* (the mock-up's example (a)) steps 1 5 9 13: C4 G4 D5 A5 (degrees 0 4 8 12 of C major), each MUTE'd but on its own
     * bar of four (!1:4 .. !4:4); step 1's C2 TRANSPOSE +1 degree, RANGE 4, at 1:4, over the bar (LENGTH 16; POLY) */
    {
        static const uint8_t P[16] = {60, 0, 0, 0, 67, 0, 0, 0, 74, 0, 0, 0, 81, 0, 0, 0};
        uint8_t bar[32];
        t = pat(16, P);
        for (i = 0; i < 4u; i++)
            comp_set(&t->step[4u * i], 0, CA_MUTE, CC_NNM, CNM_MAKE(i + 1u, 4u), 1, 0);
        comp_set(&t->step[0], 1, CA_TRANS, CC_NM, CNM_MAKE(1u, 4u), 16, CV_TR_MAKE(1, 4u));
        seq_start();
        n = run_at(t, 32u * 16u, nt, at, sizeof nt);
        ok = n == 32u;
        for (b = 0; b < 32u && b < n; b++) {
            bar[b] = nt[b];
            ok &= at[b] / 16u == b;                     /* (one note a bar) */
            ok &= nt[b] == (uint8_t)scale_steps(t, 60, (int32_t)(4u * (b % 4u) + (b / 4u) % 4u));
        }
        for (b = 0; b < 16u && ok; b++)
            for (i = 0; i < b; i++)
                ok &= bar[i] != bar[b];
        for (b = 16; b < 32u && ok; b++)
            ok &= bar[b] == bar[b - 16u];
        if (!ok) { printf("ui:   bars:"); for (b = 0; b < n && b < 32u; b++) printf(" %u", nt[b]); printf("\n"); }
        bad += check("CYCLE: four notes !1:4 .. !4:4 + TRANSPOSE +1 R4 at 1:4: 16 bars, all different, then again", ok);
    }
    {   /* SEQ: step 1 walks the pattern's notes, a note a pass (60 64 67 71: pass 2 plays 64 there ..); BOUNCE back */
        static const uint8_t P[4] = {60, 64, 67, 71};
        static const uint8_t W[16] = {60, 64, 67, 71, 64, 64, 67, 71, 67, 64, 67, 71, 71, 64, 67, 71};
        static const uint8_t WB[24] = {60, 64, 67, 71, 64, 64, 67, 71, 67, 64, 67, 71, 71, 64, 67, 71, 67, 64, 67, 71,
                                       64, 64, 67, 71};
        t = pat(4, P);
        comp_set(&t->step[0], 0, CA_SEQ, CC_ALWAYS, 0, 1, 0);
        seq_start();
        n = run(t, 16, nt, sizeof nt);
        ok = seq_is(nt, n, W, 16);
        t = pat(4, P);
        comp_set(&t->step[0], 0, CA_SEQ, CC_ALWAYS, 0, 1, CV_SEQ_BOUNCE);
        seq_start();
        n = run(t, 24, nt, sizeof nt);
        ok &= seq_is(nt, n, WB, 24);
        bad += check("SEQ walker: a note of the pattern further each pass (LOOP; BOUNCE turns back at the last)", ok);
    }
    return bad;
}

static int actions(void)
{
    int bad = 0, ok;
    uint8_t nt[64];
    uint32_t n;
    track_t *t;
    static const uint8_t P[4] = {60, 62, 64, 65};
    {   /* RATCHET x2 over 3 steps: the three split, the fourth as written; x3 at 1:2: every other pass */
        static const uint8_t W[7] = {60, 60, 62, 62, 64, 64, 65};
        static const uint8_t W2[10] = {60, 60, 60, 62, 64, 65, 60, 62, 64, 65};
        t = pat(4, P);
        comp_set(&t->step[0], 0, CA_RATCH, CC_ALWAYS, 0, 3, 0);
        seq_start();
        n = run(t, 4, nt, sizeof nt);
        ok = seq_is(nt, n, W, 7);
        t = pat(4, P);
        comp_set(&t->step[0], 0, CA_RATCH, CC_NM, CNM_MAKE(1u, 2u), 1, 1);
        seq_start();
        n = run(t, 8, nt, sizeof nt);
        ok &= seq_is(nt, n, W2, 10);
        bad += check("RATCHET x2 over its LENGTH of 3 steps (each split); x3 on 1:2 (every other pass)", ok);
    }
    {   /* DECAY: x4 DOWN, each part softer; UP louder */
        uint32_t b, k, nv = 0, a;
        uint8_t vel[8];
        t = pat(4, P);
        comp_set(&t->step[0], 0, CA_RATCH, CC_ALWAYS, 0, 1, 2u | 1u << 3);
        seq_start();
        for (b = 0; b + 1u < step_len(t) / CTL; b++) {
            uint32_t v0 = vage;
            seq_tick(t, CTL);
            for (a = v0 + 1u; a <= vage; a++)
                for (k = 0; k < NVOICE; k++)
                    if (t->v[k].age == a && nv < 8u) vel[nv++] = t->v[k].vel;
        }
        ok = nv == 4u && vel[0] > vel[1] && vel[1] > vel[2] && vel[2] > vel[3];
        bad += check("RATCHET x4 DECAY DOWN: four parts, each softer", ok);
    }
    {   /* STUTTER: step 1 again over 3 steps (60 60 60 65); every 2 steps over 4 (60 . 60 .) */
        static const uint8_t W[4] = {60, 60, 60, 65}, W2[2] = {60, 60};
        t = pat(4, P);
        comp_set(&t->step[0], 0, CA_STUT, CC_ALWAYS, 0, 3, 0);
        seq_start();
        n = run(t, 4, nt, sizeof nt);
        ok = seq_is(nt, n, W, 4);
        t = pat(4, P);
        comp_set(&t->step[0], 0, CA_STUT, CC_ALWAYS, 0, 4, 1);
        seq_start();
        n = run(t, 4, nt, sizeof nt);
        ok &= seq_is(nt, n, W2, 2);
        bad += check("STUTTER: its step replacing the next ones over its LENGTH (every step; every other step)", ok);
    }
    {   /* MUTE: steps 2 and 3 silent (LENGTH 2); FIRST: only the first pass; PRE: step 2 follows step 1's 1:2 */
        static const uint8_t W[2] = {60, 65}, W2[6] = {60, 65, 60, 62, 64, 65}, W3[6] = {64, 65, 60, 62, 64, 65};
        t = pat(4, P);
        comp_set(&t->step[1], 0, CA_MUTE, CC_ALWAYS, 0, 2, 0);
        seq_start();
        n = run(t, 4, nt, sizeof nt);
        ok = seq_is(nt, n, W, 2);
        t = pat(4, P);
        comp_set(&t->step[1], 0, CA_MUTE, CC_FIRST, 0, 2, 0);
        seq_start();
        n = run(t, 8, nt, sizeof nt);
        ok &= seq_is(nt, n, W2, 6);
        t = pat(4, P);
        comp_set(&t->step[0], 0, CA_MUTE, CC_NM, CNM_MAKE(1u, 2u), 1, 0);
        comp_set(&t->step[1], 0, CA_MUTE, CC_PRE, 0, 1, 0);
        seq_start();
        n = run(t, 8, nt, sizeof nt);
        ok &= seq_is(nt, n, W3, 6);
        bad += check("MUTE: its LENGTH of steps silent; FIRST the first pass only; PRE after the step before's 1:2", ok);
    }
    {   /* ARP: a chord's x2 UP over 2 steps (the second replaced): 60 64 | 67 60, then 64 65 */
        static const uint8_t W[6] = {60, 64, 67, 60, 64, 65};
        t = pat(4, P);
        t->step[0] = (step_t){{60, 64, 67}, 3, ST_NOTE, 0, 100};
        comp_set(&t->step[0], 0, CA_ARP, CC_ALWAYS, 0, 2, CP_ARG(0u, 0u, 0u));
        seq_start();
        n = run(t, 4, nt, sizeof nt);
        ok = seq_is(nt, n, W, 6);
        bad += check("ARP x2 UP over its LENGTH of 2 (the step after replaced), the rest as written", ok);
    }
    {   /* JUMP: step 2 then step 4 (60 62 65 ..); OCT+ 2 octaves, OCT- 1 */
        static const uint8_t W[6] = {60, 62, 65, 60, 62, 65}, W2[4] = {84, 50, 64, 65};
        t = pat(4, P);
        comp_set(&t->step[1], 0, CA_JUMP, CC_ALWAYS, 0, 1, 3);
        seq_start();
        n = run(t, 6, nt, sizeof nt);
        ok = seq_is(nt, n, W, 6);
        t = pat(4, P);
        comp_set(&t->step[0], 0, CA_OCTU, CC_ALWAYS, 0, 1, 1);
        comp_set(&t->step[1], 0, CA_OCTD, CC_ALWAYS, 0, 1, 0);
        seq_start();
        n = run(t, 4, nt, sizeof nt);
        ok &= seq_is(nt, n, W2, 4);
        bad += check("JUMP to step 4 (once a pass); OCT+ 2 octaves, OCT- 1 and back", ok);
    }
    return bad;
}

static int mono_poly(void)
{
    int bad = 0, ok;
    uint8_t nt[64];
    uint32_t n;
    track_t *t;
    static const uint8_t P[4] = {60, 62, 64, 65};
    static const uint8_t WP[5] = {72, 72, 62, 64, 65}, WM[4] = {72, 62, 64, 65}, WE[10] = {60, 60, 62, 64, 65, 72, 62, 64,
                                                                                          65, 60};
    /* POLY: C1 OCT+ and C2 RATCHET x2 on step 1: both, two notes an octave up */
    t = pat(4, P);
    comp_set(&t->step[0], 0, CA_OCTU, CC_ALWAYS, 0, 1, 0);
    comp_set(&t->step[0], 1, CA_RATCH, CC_ALWAYS, 0, 1, 0);
    seq_start();
    n = run(t, 4, nt, sizeof nt);
    ok = seq_is(nt, n, WP, 5);
    bad += check("POLY: C1 OCT+ and C2 RATCHET x2 on a step stack (72 72)", ok);
    /* MONO (C2 ELSE): C1 wins (72 once); C1 at 2:2: on the first pass C2's RATCHET (60 60), on the second C1's */
    t = pat(4, P);
    comp_set(&t->step[0], 0, CA_OCTU, CC_ALWAYS, 0, 1, 0);
    comp_set(&t->step[0], 1, CA_RATCH, CC_ELSE, 0, 1, 0);
    seq_start();
    n = run(t, 4, nt, sizeof nt);
    ok = seq_is(nt, n, WM, 4);
    t = pat(4, P);
    comp_set(&t->step[0], 0, CA_OCTU, CC_NM, CNM_MAKE(2u, 2u), 1, 0);
    comp_set(&t->step[0], 1, CA_RATCH, CC_ELSE, 0, 1, 0);
    seq_start();
    n = run(t, 9, nt, sizeof nt);
    ok &= n >= 9u && !memcmp(nt, WE, 9);
    bad += check("MONO (C2 ELSE): one acts, C1 first (72); C2 when C1's condition fails (60 60, then 72)", ok);
    /* the UI: COMP's footer column says MONO */
    ui_power_on();
    TSEL->step[0] = (step_t){{60}, 1, ST_NOTE, 0, 100};
    comp_set(&TSEL->step[0], 0, CA_OCTU, CC_ALWAYS, 0, 1, 0);
    comp_set(&TSEL->step[0], 1, CA_RATCH, CC_ELSE, 0, 1, 0);
    go_page(GR_COMP); frame();
    bad += check("  COMP shows the step MONO beside its CONDITION", strstr(ui.col[2], "MONO") != 0);
    return bad;
}

static int format(void)
{
    int bad = 0, ok;
    uint8_t nt[64];
    uint32_t n;
    track_t *t;
    project_store_t st;
    /* v2: a round trip keeps ACTION, CONDITION, VALUE, LENGTH and N:M of both */
    ui_power_on();
    trk[0].step[3] = (step_t){{60, 64}, 2, ST_NOTE, 0, 100};
    comp_set(&trk[0].step[3], 0, CA_TRANS, CC_NM, CNM_MAKE(3u, 16u), 16, CV_TR_MAKE(-2, 5u));
    comp_set(&trk[0].step[3], 1, CA_LOCK, CC_ELSE, 0, 7, 0);
    trk[1].step[5] = (step_t){{0}, 0, ST_REST, 0, 0};
    comp_set(&trk[1].step[5], 0, CA_MUTE, CC_P25, 0, 32, 1);
    project_capture(&proj_scratch);
    ok = proj_pack(&st, &proj_scratch);
    memset(&trk[0].step[3], 0, sizeof trk[0].step[3]);
    memset(&trk[1].step[5], 0, sizeof trk[1].step[5]);
    ok &= proj_import(&proj_scratch, &st, sizeof st) && !project_restore_runtime(&proj_scratch);
    ok &= trk[0].step[3].comp == CO_MAKE(CA_TRANS, CC_NM) && trk[0].step[3].carg == CV_TR_MAKE(-2, 5u) &&
          trk[0].step[3].clen[0] == 15u && trk[0].step[3].cnm[0] == CNM_MAKE(3u, 16u) &&
          trk[0].step[3].comp2 == CO_MAKE(CA_LOCK, CC_ELSE) && trk[0].step[3].clen[1] == 6u &&
          trk[1].step[5].comp == CO_MAKE(CA_MUTE, CC_P25) && trk[1].step[5].carg == 1u && trk[1].step[5].clen[0] == 31u;
    bad += check("FORMAT: a project keeps components v2 (x bit 7: LENGTH and N:M of both), a REST's too", ok);
    /* before seq2d: CP_* as such (no x bit 7) migrate as the project loads */
    ui_power_on();
    trk[0].p[P_SLEN] = 8;
    trk[0].step[0] = (step_t){{60, 64, 67}, 3, ST_NOTE, 0, 100};
    trk[0].step[0].comp = CP_ARPN; trk[0].step[0].carg = CP_ARG(0u, 0u, 0u);   /* x2 UP over the two TIEs after it */
    trk[0].step[1] = (step_t){{0}, 0, ST_TIE, 0, 0};
    trk[0].step[2] = (step_t){{0}, 0, ST_TIE, 0, 0};
    trk[0].step[3] = (step_t){{62}, 1, ST_NOTE, 0, 100};
    trk[0].step[3].comp = CP_TRIG; trk[0].step[3].carg = 0;                     /* 1:2 */
    trk[0].step[4] = (step_t){{64}, 1, ST_NOTE, 0, 100};
    trk[0].step[4].comp = CP_OCT; trk[0].step[4].carg = 0; ST_EVERY_SET(&trk[0].step[4], EV_1_4);
    trk[0].step[5] = (step_t){{65}, 1, ST_NOTE, 0, 100};
    trk[0].step[5].comp = CP_ARPQ; trk[0].step[5].carg = CP_ARG(0u, 0u, 0u);
    trk[0].step[5].comp2 = CP_ECHO; trk[0].step[5].carg2 = 1;                 /* x3, fading */
    trk[0].step[6] = (step_t){{67}, 1, ST_NOTE, 0, 100};
    trk[0].step[6].comp = CP_FILL;
    trk[0].step[7] = (step_t){{69}, 1, ST_NOTE, 0, 100};
    trk[0].step[7].comp = CP_WALK; trk[0].step[7].carg = CP_WARG(1u, 0u, 0u);  /* UP 3 degrees, a move every 2 loops */
    proj_comp_legacy = 1;
    project_capture(&proj_scratch);
    ok = proj_pack(&st, &proj_scratch);
    proj_comp_legacy = 0;
    ok &= proj_import(&proj_scratch, &st, sizeof st) && !project_restore_runtime(&proj_scratch);
    t = &trk[0];
    ok &= t->step[0].comp == CO_MAKE(CA_ARP, CC_ALWAYS) && t->step[0].carg == CP_ARG(0u, 0u, 0u) && t->step[0].clen[0] == 2u;
    ok &= t->step[3].comp == CO_MAKE(CA_MUTE, CC_NNM) && t->step[3].cnm[0] == CNM_MAKE(1u, 2u) && !t->step[3].clen[0];
    ok &= t->step[4].comp == CO_MAKE(CA_OCTU, CC_NM) && t->step[4].cnm[0] == CNM_MAKE(1u, 4u) && !(t->step[4].cwhen & 15u);
    ok &= t->step[5].comp == CO_MAKE(CA_SEQ, CC_ALWAYS) && t->step[5].comp2 == CO_MAKE(CA_RATCH, CC_ALWAYS) &&
          CV_RAT_HITS(t->step[5].carg2) == 3u && CV_RAT_DEC(t->step[5].carg2) == 1u;
    ok &= t->step[6].comp == CO_MAKE(CA_MUTE, CC_NFILL);
    ok &= t->step[7].comp == CO_MAKE(CA_TRANS, CC_NM) && t->step[7].cnm[0] == CNM_MAKE(1u, 2u) &&
          CV_TR_MOVE(t->step[7].carg) == 1 && CV_TR_RANGE(t->step[7].carg) == 4u;
    bad += check("FORMAT: a project of before seq2d migrates: ARP over its TIEs, TRIG -> MUTE !1:2, EVERY 1:4 -> OCT+ 1:4, "
                 "ARP SEQ -> SEQ, ECHO -> RATCHET x3 DOWN, FILL -> MUTE !FILL, WALK -> TRANSPOSE R4 at 1:2", ok);
    {   /* .. and plays as it did: the arpeggio 60 64 | 67 60 | 64 67 over the TIEs, 62 on the first pass (TRIG 1:2), 76 */
        static const uint8_t W[7] = {60, 64, 67, 60, 64, 67, 62};
        t->p[P_SGATE] = 64; t->p[P_SSWING] = 0; song.g[G_SWING] = 0;
        seq_start();
        n = run(t, 5, nt, sizeof nt);
        ok = n == 8u && !memcmp(nt, W, 7) && nt[7] == 76u;
        bad += check("  .. and plays the arpeggio over its TIEs, the TRIG's step, the OCT on loop 1 of 4 as before", ok);
    }
    {   /* in memory (GEN, DICE): comp_legacy then comp_migrate */
        step_t s[4];
        memset(s, 0, sizeof s);
        s[0].time = ST_NOTE; s[0].n = 1; s[0].note[0] = 60;
        comp_legacy(&s[0], 0, CP_OCT, 4);                 /* ALT: up on odd loops, down on the others, both slots */
        comp_legacy(&s[1], 0, CP_TONAL, 9);
        comp_migrate(s, 4);
        ok = s[0].comp == CO_MAKE(CA_OCTU, CC_NM) && s[0].cnm[0] == CNM_MAKE(1u, 2u) &&
             s[0].comp2 == CO_MAKE(CA_OCTD, CC_NM) && s[0].cnm[1] == CNM_MAKE(2u, 2u) &&
             s[1].comp == CO_MAKE(CA_TRANS, CC_ALWAYS) && CV_TR_MOVE(s[1].carg) == 2 && CV_TR_RANGE(s[1].carg) == 1u &&
             s[1].clen[0] == 2u;
        bad += check("  in memory: OCT ALT into both slots (1:2 up, 2:2 down), TONAL a FIX TRANSPOSE to the loop's end", ok);
    }
    return bad;
}

static int comp_ui(void)
{
    int bad = 0, ok;
    track_t *t;
    ui_power_on();
    t = TSEL;
    t->step[0] = (step_t){{60, 64, 67}, 3, ST_NOTE, 0, 100};
    go_page(GR_COMP); frame();
    ok = str_eq(cur_page()->title, "COMP") && comp_view() == CA_NONE;
    turn(EN_K1, 1);                                       /* OFF -> ARP: four steps, UP x4 1 OCT */
    ok &= CO_ACT(t->step[0].comp) == CA_ARP && t->step[0].clen[0] == 3u && t->step[0].carg == CP_ARG(0u, 2u, 0u);
    turn(EN_K2, 2);                                       /* LENGTH 6 */
    ok &= t->step[0].clen[0] == 5u;
    turn(EN_K3, 1);                                       /* 25 % */
    ok &= CO_COND(t->step[0].comp) == CC_P25 && strstr(ui.col[2], "25%") != 0;
    turn(EN_K3, 11);                                      /* past ELSE (COMP 2's): 1:2 */
    ok &= CO_COND(t->step[0].comp) == CC_NM && t->step[0].cnm[0] == CNM_MAKE(1u, 2u);
    turn(EN_K3, 3);                                       /* 2:2 1:3 2:3 */
    ok &= t->step[0].cnm[0] == CNM_MAKE(2u, 3u) && strstr(ui.col[2], "2:3") != 0;
    turn(EN_K4, 1);                                       /* x4 -> x6 (RATE first) */
    ok &= CP_RATE(t->step[0].carg) == 3u && CP_ORD(t->step[0].carg) == 0u;
    turn(EN_K1, 4);                                       /* .. RATCH: the VALUE its own, the CONDITION kept */
    ok &= CO_ACT(t->step[0].comp) == CA_RATCH && CO_COND(t->step[0].comp) == CC_NM && t->step[0].carg == 0u;
    turn(EN_K1, -4); turn(EN_K1, -1);                     /* OFF: gone */
    ok &= !t->step[0].comp && !t->step[0].clen[0];
    bad += check("SEQ > COMP: KNOB 1 ACTION, KNOB 2 LENGTH, KNOB 3 CONDITION (ELSE skipped), KNOB 4 VALUE, OFF clears", ok);
    ui.page++; page_entered(); frame();
    ok = str_eq(cur_page()->title, "COMP 2") && comp_slot() == 1u;
    turn(EN_K1, 4); turn(EN_K1, 3);                       /* MUTE */
    turn(EN_K3, 4); turn(EN_K3, 4); turn(EN_K3, 4);   /* ELSE: MONO */
    ok &= CO_ACT(t->step[0].comp2) == CA_MUTE && CO_COND(t->step[0].comp2) == CC_ELSE && !t->step[0].comp;
    bad += check("SEQ > COMP 2: the step's C2 (MUTE), ELSE its condition (MONO)", ok);
    return bad;
}

int main(void)
{
    int bad = cycle() + actions() + mono_poly() + format() + comp_ui();
    printf("%s\n", bad ? "COMP TEST FAILED" : "comp tests passed");
    return bad != 0;
}
