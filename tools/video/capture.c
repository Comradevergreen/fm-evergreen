/* SPDX-License-Identifier: GPL-3.0-only
 * (JIANT 0.6.7) the presentation video's footage: the firmware's own UI and audio, from the host build of the tests
 * (tests/ui_test.c): a 60 s demo song at 120 BPM (A minor, six tracks) played by the real engines, its screen captured
 * at 30 fps as the script below changes pages, layers and settings bar by bar.
 *   cc -O2 -w -Ibuild/gen -Ifirmware/src -Itests -o build/video/capture tools/video/capture.c -lm
 *   build/video/capture build/video        -> audio.wav (44.1 kHz stereo), screen.rgb (240 x 240 RGB24 a frame)
 * tools/video/compose.py makes the 1080p video from them. */
#define UI_TEST_NO_MAIN 1
#define FELUCCA_VERSION "v0.6.7"
#include "ui_test.c"

#define FPS 30u
#define SECS 60u
#define BAR_S 2.0                                   /* (120 BPM, 4/4) */
static FILE *wav, *rgb;
static uint32_t nsamp;

static uint32_t eng_of(const char *name)
{
    uint32_t e;
    for (e = 0; e < NENGINES; e++) if (str_eq(ENGINES[e]->name, name)) return e;
    return 0;
}
static uint32_t pre_of(uint32_t e, const char *name)
{
    uint32_t i;
    for (i = 0; i < ENGINES[e]->npresets; i++) if (str_eq(ENGINES[e]->presets[i].name, name)) return i;
    return 0;
}
static void sound(uint32_t k, const char *eng, const char *pre)
{
    uint32_t e = eng_of(eng);
    set_engine_of(&trk[k], e);
    apply_preset_to(&trk[k], pre_of(e, pre));
    trk[k].engine = trk[k].eng_req;
}
static void put(uint32_t k, uint32_t i, uint32_t n, const uint8_t *notes, uint32_t time, uint32_t flags)
{
    step_t *s = &trk[k].step[i];
    uint32_t j;
    s->n = (uint8_t)n;
    for (j = 0; j < 4u; j++) s->note[j] = j < n ? notes[j] : 0;
    s->time = (uint8_t)time;
    s->flags = (uint8_t)flags;
    s->vel = n ? 100 : 0;
}
static void song_build(void)
{
    static const uint8_t ACID[16] = {45, 45, 57, 45, 0, 48, 45, 55, 45, 0, 57, 52, 45, 48, 0, 50};
    static const uint8_t ACIDF[16] = {1, 0, 2, 0, 0, 0, 1, 2, 0, 0, 1, 0, 0, 2, 0, 1};
    static const uint8_t AM[4] = {57, 60, 64, 67}, FM[4] = {53, 57, 60, 64}, CM[4] = {55, 60, 64, 67}, GM[4] = {55, 59, 62, 67};
    static const uint8_t HOOK[16] = {69, 0, 0, 72, 0, 0, 76, 0, 0, 0, 74, 0, 72, 0, 0, 0};
    static const uint8_t ARP[16] = {57, 64, 69, 72, 57, 64, 69, 76, 53, 60, 65, 69, 55, 62, 67, 71};
    uint32_t i, k;
    ui_power_on();
    settings.palette = UI_JIANT_INDEX; palette_set(UI_JIANT_INDEX);
    song.g[G_BPM] = 120;
    song.master_q12 = 4096;                                  /* (the pot: ui_power_on leaves 0) */
    sound(0, "ANALOG", "ACID");
    sound(1, "FM6", "PAD");
    sound(2, "LOFI", "FLOAT FM");
    sound(3, "DRUM", "DRUM-X");
    sound(4, "PHASE", "BELL");
    sound(5, "NOISE", "WIND");
    for (k = 0; k < NTRK; k++) {
        track_defaults_steps(&trk[k]);
        trk[k].p[P_ROOT] = 9; trk[k].p[P_SCALE] = 2;          /* A minor */
        trk[k].p[P_SLEN] = 16;
        trk[k].p[P_MUTE] = 1;
    }
    for (i = 0; i < 16u; i++) {
        uint8_t n = ACID[i];
        put(0, i, n ? 1u : 0u, &n, n ? ST_NOTE : ST_REST, ACIDF[i]);
    }
    trk[1].p[P_SLEN] = 64; trk[1].p[P_SGATE] = 120;
    for (i = 0; i < 64u; i++) {
        const uint8_t *c = i < 16u ? AM : i < 32u ? FM : i < 48u ? CM : GM;
        put(1, i, i % 16u == 0u ? 4u : 0u, c, i % 16u == 0u ? ST_NOTE : i % 16u < 15u ? ST_TIE : ST_REST, 0);
    }
    trk[1].p[P_REV] = 90; trk[1].p[P_LEVEL] = 92;
    for (i = 0; i < 16u; i++) {
        uint8_t n = HOOK[i];
        put(2, i, n ? 1u : 0u, &n, n ? ST_NOTE : ST_REST, 0);
    }
    trk[2].p[P_DLY] = 60; trk[2].p[P_LEVEL] = 80;
    for (i = 0; i < 16u; i++) {                               /* kick, snare, hats */
        uint8_t n[4]; uint32_t c = 0;
        if (i % 4u == 0u || i == 10u) n[c++] = 36;
        if (i == 4u || i == 12u) n[c++] = 38;
        if (i % 2u == 0u) n[c++] = i == 14u ? 46 : 42;
        put(3, i, c, n, c ? ST_NOTE : ST_REST, i % 4u == 0u ? SF_ACCENT : 0u);
        step_to_grid(&trk[3].step[i]);
    }
    for (i = 0; i < 16u; i++) {
        uint8_t n = ARP[i];
        put(4, i, 1, &n, ST_NOTE, 0);
    }
    trk[4].p[P_SDIV] = 2; trk[4].p[P_REV] = 70; trk[4].p[P_LEVEL] = 70;
    {   uint8_t n = 45; put(5, 0, 1, &n, ST_NOTE, 0); for (i = 1; i < 16u; i++) put(5, i, 0, &n, ST_TIE, 0); }
    trk[5].p[P_LEVEL] = 60; trk[5].p[P_REV] = 100;
    song.g[G_RTYPE] = 3; song.g[G_RSIZE] = 105;               /* RESO, ringing */
    song.g[G_DTIME] = 3;
    chain_config.count = 4;                                    /* (the SONG page: a chain to look at) */
    chain_config.row[0] = (chain_row_t){0, 4}; chain_config.row[1] = (chain_row_t){1, 8};
    chain_config.row[2] = (chain_row_t){2, 4}; chain_config.row[3] = (chain_row_t){0, 8};
    transport_req = 1;
}
/* the script: at bar b (2 s each) */
static void at_bar(uint32_t b)
{
    uint32_t k;
    switch (b) {
    case 0: trk[1].p[P_MUTE] = 0; trk[5].p[P_MUTE] = 0; break;                  /* pad, wind: the splash */
    case 2: trk[0].p[P_MUTE] = 0; go_home(); break;                             /* the acid: HOME */
    case 4: trk[3].p[P_MUTE] = 0; trk[5].p[P_MUTE] = 1; song.sel = 0; go_title("EDIT 1"); break;   /* ANALOG */
    case 5: song.sel = 1; go_title("EDIT 1"); break;                            /* FM6 */
    case 6: trk[2].p[P_MUTE] = 0; song.sel = 2; go_title("EDIT 1"); break;      /* LOFI FLOAT */
    case 7: song.sel = 3; go_title("EDIT 1"); break;                            /* DRUM-X */
    case 10:                                                                     /* the SLICER on the whole mix */
        song.g[G_SLMODE] = 2; song.g[G_SLPAT] = 9; song.g[G_SLRATE] = 1; song.g[G_SLDEP] = 127;
        for (k = 0; k <= NTRK; k++) song.g[G_SLT1 + k] = 1;
        song.g[G_SLPLEN] = 4; song.g[G_SLP0] = 0; song.g[G_SLP0 + 1] = 7; song.g[G_SLP0 + 2] = 12; song.g[G_SLP0 + 3] = -5;
        go_title("SLICER"); break;
    case 12: go_title("SL PITCH"); break;
    case 13: song.g[G_SLMODE] = 0; song.g[G_RDAMP] = 1; song.g[G_RPRE] = 2; trk[4].p[P_MUTE] = 0;
        go_title("REVERB"); break;                                               /* RESO following the bass */
    case 16: go_title("MIXER"); break;
    case 19: go_title("SONG"); break;
    case 22: fm1_in.buttons |= 1u << panel.btn[B_FX]; break;                     /* the FX layer, held */
    case 25: fm1_in.buttons &= ~(1u << panel.btn[B_FX]); perf_press(PF_R16, 0); perf_press(PF_OCTU, 0);
        trk[0].p[P_M1SRC] = MS_STEP; trk[0].p[P_M1DST] = MD_CUT; trk[0].p[P_M1AMT] = 50;
        for (k = 0; k < 16u; k++) trk[0].p[P_MS0 + k] = (int16_t)((k * 37u) % 128u);
        song.sel = 0; go_title("MSEQ"); break;
    case 28: trk[3].p[P_MUTE] = 1; trk[0].p[P_MUTE] = 1; trk[2].p[P_MUTE] = 1; go_home(); break;   /* the outro */
    }
}
static void at_beat(uint32_t beat)                  /* (bars 22..24: punch-ins on the FX layer) */
{
    uint32_t b = beat / 4u, q = beat % 4u;
    if (b == 23u) perf_press(PF_R16, q == 2u);
    if (b == 24u) { perf_press(PF_OCTU, q < 2u); perf_press(PF_S16, q >= 2u); }
    if (b == 25u && q == 0u) perf_press(PF_S16, 0);
    if (b >= 7u && b < 10u) trk[3].p[P_E0] = (int16_t)((beat - 28u) * 127u / 12u);   /* DRUM-X MORPH swept */
}
static void put_screen(void)
{
    static uint8_t buf[240u * 240u * 3u];
    uint32_t i;
    for (i = 0; i < 240u * 240u; i++) {
        uint16_t c = swap16(host_screen[i]);
        buf[3u * i] = (uint8_t)((c >> 11) * 255u / 31u);
        buf[3u * i + 1u] = (uint8_t)(((c >> 5) & 63u) * 255u / 63u);
        buf[3u * i + 2u] = (uint8_t)((c & 31u) * 255u / 31u);
    }
    fwrite(buf, 1, sizeof buf, rgb);
}
int main(int argc, char **argv)
{
    char p[512];
    uint32_t f, total = FPS * SECS, beat_done = 0xFFFFFFFFu, bar_done = 0xFFFFFFFFu;
    const char *dir = argc > 1 ? argv[1] : "build/video";
    snprintf(p, sizeof p, "%s/audio.wav", dir); wav = fopen(p, "wb");
    snprintf(p, sizeof p, "%s/screen.rgb", dir); rgb = fopen(p, "wb");
    if (!wav || !rgb) { fprintf(stderr, "cannot write in %s\n", dir); return 1; }
    wav_hdr(wav, (uint32_t)FS * SECS);          /* (hostsim.c) */
    song_build();
    for (f = 0; f < total; f++) {
        double t = (double)f / FPS;
        uint32_t want = (uint32_t)((uint64_t)(f + 1u) * FS / FPS), bar = (uint32_t)(t / BAR_S), beat = (uint32_t)(t / 0.5);
        if (bar != bar_done) { bar_done = bar; at_bar(bar); }
        if (beat != beat_done) { beat_done = beat; at_beat(beat); }
        while (nsamp < want) {                          /* the audio up to this frame's end */
            int32_t o[2 * CTL];
            int16_t s[2 * CTL];
            uint32_t i, n = CTL;
            mix_block(o, CTL);
            if (nsamp + n > (uint32_t)FS * SECS) n = (uint32_t)FS * SECS - nsamp;
            for (i = 0; i < 2u * n; i++) s[i] = (int16_t)clamp(o[i], -32768, 32767);
            fwrite(s, 2, 2u * n, wav);
            nsamp += CTL;
        }
        host_ticks += 1000000u / FPS; fm1_ms += 1000u / FPS;
        if (t < 3.6) {                                   /* the power-on splash: the signature written */
            if (f == 0) { spl_anim = 1; draw_splash(); }
            splash_write((uint32_t)(t * 1000.0));
        } else {
            if (f == (uint32_t)(3.6 * FPS)) ui.force = 1;
            ui_input();
            ui_draw();
        }
        put_screen();
    }
    fclose(wav); fclose(rgb);
    printf("%u frames, %u samples\n", total, nsamp);
    return 0;
}
