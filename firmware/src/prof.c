/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Juan Ignacio Antonucci (Juan Jiant) */
/* (0.9.14) MENU > ABOUT > PROFILE: where the audio's time goes, measured on the FM-1 itself (TIMER4, 24 MHz, read by
 * either core: hal/fm1_multicore.h fm1_mc_ticks), so the load is no longer estimated from the host's instruction
 * counts (they do not see the flash cache: 0.9.10's crackle, 0.9.13's EXPLORE budget). Each block (CTL samples,
 * 725 us = PRF_BLK ticks) the sections below are timed where they run; a second of them (344 halves) is summed and
 * kept, the last second while stopped and the last while playing apart (prf_win[0] / [1]): the mean and the worst
 * block of each, in % of a block (ui_menu.c prof_document). Off until PROFILE is first opened (prf_on): the marks
 * still read the timer (a load each), only the sums wait for it. The host has no timer: all 0 (tests set them) */
enum {
    PRF_SEQ,                     /* events_block, the FX layer's keys, the matrix's master, the split (mix_block's head) */
    PRF_T1,                      /* T1..T6: the part's render, mix and sends (fx.c mix_part_t), on the core it ran on */
    PRF_FX = PRF_T1 + NPART,     /* the buses (fx_run_t): */
    PRF_REV,                     /* .. of it the reverb (rev_run) */
    PRF_WAIT,                    /* core 0 waiting for core 1 (mc_wait) */
    PRF_POST,                    /* the FX layer, the SLICER, DJ FILTER, CLIP (after the parts) */
    PRF_MST,                     /* the leveler and the master (master_out) */
    PRF_USB,                     /* USB audio: the host's in, the tap out (audio_block) */
    PRF_OUT,                     /* the scope, the metronome, 24-bit (audio_block) */
    PRF_C1,                      /* core 1 busy (mc_job) */
    PRF_NB,                      /* (the sections a block) */
    PRF_IRQ = PRF_NB,            /* a half: TIMER5 nested in the audio ISR (the keys, USB) */
    PRF_HALF,                    /* .. the whole render (ABOUT's AUDIO LOAD, TIMER5 left out) */
    PRF_N
};
#define PRF_BLK ((uint32_t)CTL * 544u)              /* a block in TIMER4 ticks (audio.c DAC_TICKS) */
#define PRF_HALVES 344u                              /* a second (2.9 ms halves) */
typedef struct {
    uint32_t sum[PRF_N], max[PRF_N];                 /* ticks: the second's sum, its worst block (half) */
    uint32_t blocks, c1[NPART];                      /* blocks summed; each part's blocks on core 1 */
    uint32_t t5, lat, late, fxrest, frames;          /* TIMER5 in all (ticks); the ISR's latest start (ticks past its
                                                      * period); halves late; blocks the buses rested; UI frames */
} prf_win_t;
#define PRF_POOL __attribute__((section(".pool")))   /* (zeroed at boot; and out of the compiler's merged globals:
                                                     * new ones there reshuffled them, +1.1 KB of code elsewhere) */
static prf_win_t prf_cur PRF_POOL, prf_win[2] PRF_POOL;                /* this second; the last whole one stopped [0], playing [1] */
static uint32_t prf_blk[PRF_NB] PRF_POOL;                     /* this block's (either core writes its sections, core 0 sums) */
static volatile uint8_t prf_on PRF_POOL, prf_seq PRF_POOL;             /* measuring (PROFILE opened); a second published (the redraw) */
static uint8_t prf_have PRF_POOL;                             /* bit k: prf_win[k] holds a second */
#if FELUCCA_DUAL_CORE
#define PRF_T() fm1_mc_ticks()
#else
#ifndef PRF_T
#define PRF_T() 0u
#endif
#endif
/* (prf_block, prf_half: audio.c) */
