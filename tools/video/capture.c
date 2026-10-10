/* SPDX-License-Identifier: GPL-3.0-only
 * (JIANT 0.6.7) the presentation video's footage: the firmware's own UI and audio, from the host build of the tests
 * (tests/ui_test.c): a demo song at 132 BPM (A minor, six tracks) played by the real engines, soft at first, industrial
 * at the end, its screen captured at 30 fps as the script below goes through the pages, effects, layers and settings
 * beat by beat. The captions the video shows go to scenes.txt (time, tag, title, subtitle), from the same script.
 *   cc -O2 -w -Ibuild/gen -Ifirmware/src -Itests -o build/video/capture tools/video/capture.c -lm
 *   build/video/capture build/video        -> audio.wav (44.1 kHz stereo), screen.rgb (240 x 240 RGB24 a frame)
 * tools/video/compose.py makes the 1080p video from them. */
#define UI_TEST_NO_MAIN 1
#define FELUCCA_VERSION "v0.6.7"
#include "ui_test.c"

#define FPS 30u
#define BPM 132u
#define BARS 38u
#define BEAT_S (60.0 / BPM)
#define SECS (BARS * 4u * 60.0 / BPM)
#define SPLASH_BEATS 8u                             /* the power-on splash: the first two bars */
#define B(bar, q) ((bar) * 4u + (q))                /* the beat q of a bar */
static FILE *wav, *rgb, *cap;
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
static void drums(const char *const rows[8])      /* one row a lane (BD SD CP CH OH TM RS CB): 'x' a hit, 'X' accented */
{
    static const uint8_t NOTE[8] = {36, 38, 39, 42, 46, 45, 37, 56};
    uint32_t i, l;
    for (i = 0; i < 16u; i++) {
        uint8_t n[4]; uint32_t c = 0, acc = 0;
        for (l = 0; l < 8u; l++)
            if (rows[l] && rows[l][i] != '.' && c < 4u) { n[c++] = NOTE[l]; acc |= rows[l][i] == 'X'; }
        put(3, i, c, n, c ? ST_NOTE : ST_REST, acc ? SF_ACCENT : 0u);
        step_to_grid(&trk[3].step[i]);
    }
}
static const char *const SOFT[8] = {"X.......x.......", 0, 0, "..x...x...x...x.", 0, 0, ".......x......x.", 0};
static const char *const GROOVE[8] = {"X...X...X...X...", 0, "....x.......x...", "xx.xxx.xxx.xxx.x", "..x...x...x...x.", 0, 0, 0};
static const char *const INDUS[8] = {"X...X..xX...X.xx", "....x.......x.xx", "....x.......x...", "xxxxxxxxxxxxxxxx",
                                     0, "..x.....x.x.....", 0, ".x.....x.....x.."};

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
    song.g[G_BPM] = BPM;
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
    for (i = 0; i < 16u; i++)                                 /* the acid's cutoff, automated (AUTOMATION) */
        motion_set_event(&trk[0], i, P_E4, (int16_t)(30 + (i * 53u) % 70u));   /* (ANALOG: E4 CUT) */
    motion_set_enabled(&trk[0], 1);
    trk[1].p[P_SLEN] = 64; trk[1].p[P_SGATE] = 120;
    for (i = 0; i < 64u; i++) {
        const uint8_t *c = i < 16u ? AM : i < 32u ? FM : i < 48u ? CM : GM;
        put(1, i, i % 16u == 0u ? 4u : 0u, c, i % 16u == 0u ? ST_NOTE : i % 16u < 15u ? ST_TIE : ST_REST, 0);
    }
    trk[1].p[P_REV] = 90; trk[1].p[P_LEVEL] = 90;
    for (i = 0; i < 16u; i++) {
        uint8_t n = HOOK[i];
        put(2, i, n ? 1u : 0u, &n, n ? ST_NOTE : ST_REST, 0);
    }
    trk[2].p[P_DLY] = 70; trk[2].p[P_LEVEL] = 80;
    drums(SOFT);
    for (i = 0; i < 16u; i++) {
        uint8_t n = ARP[i];
        put(4, i, 1, &n, ST_NOTE, 0);
    }
    trk[4].p[P_SDIV] = 2; trk[4].p[P_REV] = 70; trk[4].p[P_DLY] = 40; trk[4].p[P_LEVEL] = 70;
    {   uint8_t n = 45; put(5, 0, 1, &n, ST_NOTE, 0); for (i = 1; i < 16u; i++) put(5, i, 0, &n, ST_TIE, 0); }
    trk[5].p[P_LEVEL] = 60; trk[5].p[P_REV] = 100;
    song.g[G_RTYPE] = 0; song.g[G_RSIZE] = 105;
    song.g[G_DTIME] = 3;
    chain_config.count = 4;                                    /* (the SONG page: a chain to look at) */
    chain_config.row[0] = (chain_row_t){0, 4}; chain_config.row[1] = (chain_row_t){1, 8};
    chain_config.row[2] = (chain_row_t){2, 4}; chain_config.row[3] = (chain_row_t){0, 8};
    transport_req = 1;
}

static void caption(uint32_t q, const char *tag, const char *title, const char *sub)   /* (lines split by '|') */
{
    fprintf(cap, "%.4f\t%s\t%s\t%s\n", q * BEAT_S, tag, title, sub);
}
static void page(uint32_t sel, const char *title) { song.sel = (uint8_t)sel; go_title(title); }
static void slicer(uint32_t mode, uint32_t mask)
{
    uint32_t k;
    song.g[G_SLMODE] = (int16_t)mode; song.g[G_SLPAT] = 9; song.g[G_SLRATE] = 1; song.g[G_SLDEP] = 127;
    for (k = 0; k <= NTRK; k++) song.g[G_SLT1 + k] = (int16_t)((mask >> k) & 1u);
}
static void hold_btn(uint32_t label, int on)
{
    if (on) { fm1_in.buttons |= 1u << panel.btn[label]; host_pressed |= 1u << panel.btn[label]; } else fm1_in.buttons &= ~(1u << panel.btn[label]);
}

/* the script: at each beat q (bar q / 4); the captions go with it */
static void at_beat(uint32_t q)
{
    static const uint8_t RTYPE[4] = {0, 1, 2, 4};              /* ROOM SPRING SHIMR CLOUD (then RESO) */
    static const char *const MONT[8] = {"DIST", "SLICER", "REVERB", "MSEQ", "SOUND", "FILTER", "X-MOD", "DLY 2"};
    static const uint8_t MONT_SEL[8] = {3, 3, 0, 0, 3, 0, 3, 2};
    static const char *const WORDS[8] = {"5 REVERBS", "GRAIN DELAY", "5 DISTORTIONS", "SLICER BUS", "MSEQ", "MACROS",
                                         "AUTOMATION", "DRUM-X"};
    uint32_t bar = q / 4u, k;

    /* the soft start: pad and wind under the splash, then the bell; the engines */
    if (q == B(0, 0)) { trk[1].p[P_MUTE] = 0; trk[5].p[P_MUTE] = 0; }
    if (q == B(2, 0)) { trk[4].p[P_MUTE] = 0; go_home(); }
    if (q == B(3, 0)) { page(1, "EDIT 1"); caption(q, "SOUND", "6 TRACKS|8 ENGINES", "ANALOG · FM6 · PHASE · LOFI|VOICE · WHEEL · NOISE · DRUM-X|all synthesis, no samples"); }
    if (q == B(3, 2)) page(4, "EDIT 1");
    if (q == B(4, 0)) page(5, "EDIT 1");
    if (q == B(4, 2)) { trk[2].p[P_MUTE] = 0; page(2, "EDIT 1"); }
    /* the reverbs, one every two beats, then RESO following the acid */
    if (q >= B(5, 0) && q < B(7, 0) && q % 2u == 0u) {
        song.g[G_RTYPE] = RTYPE[(q - B(5, 0)) / 2u];
        if (q == B(5, 0)) { page(1, "REVERB"); caption(q, "FX", "5 REVERBS", "ROOM · SPRING · SHIMMER|CLOUD (granular)|RESO (tuned to the scale)"); }
    }
    if (q == B(7, 0)) {
        trk[0].p[P_MUTE] = 0; song.g[G_RTYPE] = 3; song.g[G_RDAMP] = 1; song.g[G_RPRE] = 2;
        caption(q, "FX", "RESO REVERB", "it rings in the scale|FOLW: follows a track's notes|PITCH: up the chord");
    }
    /* DRUM-X: its pages, the MORPH swept from kit A to kit B */
    if (q == B(8, 0)) { trk[3].p[P_MUTE] = 0; page(3, "EDIT 1"); caption(q, "DRUMS", "DRUM-X", "8 synthesized voices|MORPH: from kit A to kit B"); }
    if (q >= B(8, 0) && q < B(12, 0)) trk[3].p[P_E0] = (int16_t)((q - B(8, 0)) * 127u / 15u);
    if (q == B(9, 0)) { dx_ui_lane = 0; page(3, "SOUND"); caption(q, "DRUMS", "DRUM-X", "every voice its own:|pitch · pitch mod · decay|noise · color · wave"); }
    if (q == B(9, 2)) dx_ui_lane = 1;
    if (q == B(10, 0)) { dx_ui_lane = 3; page(3, "SOUND 2"); }
    if (q == B(10, 2)) dx_ui_lane = 2;
    if (q == B(11, 0)) { page(3, "EDIT 2"); caption(q, "DRUMS", "DRUM-X", "the kit's NOISE · FM|FOLD (a wavefolder) · DRIVE"); }
    if (q >= B(11, 0) && q < B(12, 0)) { trk[3].p[P_E6] = (int16_t)((q - B(11, 0)) * 25u); trk[3].p[P_E5] = (int16_t)((q - B(11, 0)) * 15u); }
    if (q == B(12, 0)) {
        dx_mot[XM_RATE] = 5; dx_mot[XM_DPTH] = 90; dx_mot[XM_SHPE] = 4; dx_mot[XM_RAND] = 40;
        trk[3].p[P_E6] = 10; trk[3].p[P_E5] = 0;
        page(3, "X-MOD"); caption(q, "DRUMS", "X-MOD", "an LFO on the MORPH|and a random one per hit");
    }
    /* filters on the acid */
    if (q == B(13, 0)) { page(0, "FILTER"); caption(q, "SOUND", "FILTERS", "LP · HP · BP · COMB|reso and drive|on every engine"); }
    if (q >= B(13, 0) && q < B(15, 0) && q % 2u == 0u) trk[0].p[P_FTYPE] = (int16_t)((q - B(13, 0)) / 2u % 4u);
    if (q == B(15, 0)) trk[0].p[P_FTYPE] = 0;
    /* the grain delay */
    if (q == B(15, 0)) { song.g[G_DPIT] = 7; song.g[G_DFDBK] = 80; page(2, "DLY 2"); caption(q, "FX", "GRAIN DELAY", "pitch-shifted repeats|±12 semitones · spray"); }
    if (q == B(16, 0)) song.g[G_DPIT] = 12;
    if (q == B(16, 2)) song.g[G_DSPRY] = 70;
    /* modulation */
    if (q == B(17, 0)) {
        song.g[G_DPIT] = 0; song.g[G_DSPRY] = 0; song.g[G_DFDBK] = 60;
        trk[0].p[P_LRATE] = 75; trk[0].p[P_LWAVE] = 1; trk[0].p[P_LD_FLT] = 30;
        page(0, "LFO"); caption(q, "MOD", "MODULATION", "LFOs · envelopes that loop");
    }
    if (q == B(17, 2)) { trk[0].p[P_ELOOP] = 1; page(0, "ENV DEST"); }
    if (q == B(18, 0)) {
        trk[0].p[P_M1SRC] = MS_LFO; trk[0].p[P_M1DST] = MD_CUT; trk[0].p[P_M1AMT] = 40;
        page(0, "MOD"); caption(q, "MOD", "MOD MATRIX", "14 sources × 22 destinations|macros · master · DRUM-X");
    }
    if (q == B(18, 2)) {
        trk[0].p[P_M2SRC] = MS_STEP; trk[0].p[P_M2DST] = MD_DIST; trk[0].p[P_M2AMT] = 50;
        for (k = 0; k < 16u; k++) trk[0].p[P_MS0 + k] = (int16_t)((k * 37u) % 128u);
        page(0, "MSEQ"); caption(q, "MOD", "MSEQ", "a 16-step modulation|sequencer on every track");
    }
    if (q == B(19, 0)) { page(0, "AUTOMATION"); caption(q, "SEQ", "AUTOMATION", "knob moves recorded per step|parameter locks"); }
    /* it gets harder: the groove, the distortions */
    if (q == B(20, 0)) {
        drums(GROOVE); trk[3].p[P_DIST] = 60; song.g[G_DUCK] = 35;
        page(3, "DIST"); caption(q, "FX", "5 DISTORTIONS", "SOFT · HARD · FOLD|CRUSH · RECT|on every track");
    }
    if (q >= B(20, 0) && q < B(22, 0) && q % 2u == 0u) trk[3].p[P_DTYPE] = (int16_t)((q - B(20, 0)) / 2u % 5u);
    if (q == B(22, 0)) {
        trk[3].p[P_DTYPE] = 1;
        slicer(2, 0x09u);                                       /* the acid and the drums */
        song.g[G_SLPLEN] = 4; song.g[G_SLP0] = 0; song.g[G_SLP0 + 1] = 7; song.g[G_SLP0 + 2] = 12; song.g[G_SLP0 + 3] = -5;
        page(3, "SLICER"); caption(q, "FX", "SLICER BUS", "stutter · gate, post FX|on any of the 6 tracks");
    }
    if (q == B(23, 0)) { page(3, "SL PITCH"); caption(q, "FX", "SLICER BUS", "a 16-step pitch|sequencer for the slices"); }
    /* the sequencers */
    if (q == B(24, 0)) { slicer(0, 0); page(0, "STEP"); caption(q, "SEQ", "SEQUENCERS", "piano roll · chance · shift|arpeggiator · scales"); }
    if (q == B(24, 2)) page(0, "CHANCE");
    if (q == B(25, 0)) page(0, "SHIFT");
    if (q == B(25, 2)) { trk[4].p[P_AMODE] = 1; page(4, "ARP"); }
    if (q == B(26, 0)) { trk[4].p[P_AMODE] = 0; page(0, "SCL"); }
    if (q == B(26, 2)) page(0, "PATTERN");
    /* the macros, then the punch-ins */
    if (q == B(27, 0)) { go_home(); hold_btn(B_LFO, 1);   /* (the layers: from HOME) */
        caption(q, "PERFORM", "MACROS", "4 macros over what's playing|SELECT rolls new routes"); }
    if (q >= B(27, 0) && q < B(28, 0)) for (k = 0; k < 4u; k++) macro_v[k] = (uint8_t)clamp((int32_t)((q - B(27, 0) + 1u) * (16u + 6u * k)), 0, 127);
    if (q == B(27, 2)) { macro_dice(1); ui_message("MACROS DICED"); }
    if (q == B(27, 3)) { hold_btn(B_LFO, 0); for (k = 0; k < 4u; k++) macro_v[k] = 0; }   /* (let go: latched) */
    if (q == B(28, 0)) hold_btn(B_LFO, 1);                     /* .. LFO again: DONE */
    if (q == B(28, 1)) hold_btn(B_LFO, 0);
    if (q == B(28, 2)) {
        go_home(); hold_btn(B_FX, 1); caption(q, "PERFORM", "PUNCH-IN FX", "hold FX, play the keys:|repeat · octave · stutter");
    }
    if (bar == 28u && q % 4u == 3u) perf_press(PF_R16, 1);
    if (bar == 29u) { perf_press(PF_R16, q % 4u == 0u); perf_press(PF_OCTU, q % 4u == 1u || q % 4u == 2u); perf_press(PF_S16, q % 4u >= 2u); }
    /* industrial */
    if (q == B(30, 0)) {
        hold_btn(B_FX, 0); perf_press(PF_S16, 0); perf_press(PF_OCTU, 0); perf_press(PF_R16, 0);
        drums(INDUS); trk[3].p[P_E0] = 127; trk[3].p[P_E7] = 110; trk[3].p[P_E6] = 70; trk[3].p[P_E5] = 50;
        trk[3].p[P_DTYPE] = 3; trk[3].p[P_DIST] = 70;
        trk[0].p[P_DTYPE] = 2; trk[0].p[P_DIST] = 80; trk[0].p[P_E5] = 105;   /* (RES) */
        sound(5, "NOISE", "METAL");
        for (k = 0; k < 16u; k++) { uint8_t n = 57; put(5, k, k % 4u == 2u || k % 8u == 7u ? 1u : 0u, &n, k % 4u == 2u || k % 8u == 7u ? ST_NOTE : ST_REST, 0); }
        trk[5].p[P_ROOT] = 9; trk[5].p[P_SCALE] = 2; trk[5].p[P_SLEN] = 16; trk[5].p[P_MUTE] = 0; trk[5].p[P_LEVEL] = 70;
        trk[2].p[P_MUTE] = 1;
        song.g[G_CLIP] = 75; song.g[G_PUNCH] = 90; song.g[G_DUCK] = 55;
        page(3, "MASTER"); caption(q, "MIX", "MASTER", "clipper · drum punch|sidechain duck · limiter");
    }
    if (q == B(31, 0)) { go_title("MIXER"); caption(q, "MIX", "6-CHANNEL|MIXER", "level · pan · send · mute|thermal meters"); }
    if (q == B(32, 0)) { go_title("SONG"); caption(q, "SEQ", "SONGS", "8 songs × 4 rows of sections|chains · scenes"); }
    if (q == B(33, 0)) { slicer(2, 0x08u); go_home(); caption(q, "", "ALL SYNTHESIS", "6 tracks · no samples|on a pocket synth"); }
    if (q == B(33, 2)) slicer(0, 0);
    if (q >= B(34, 0) && q < B(36, 0)) {                        /* the montage: a page and a word a beat */
        k = q - B(34, 0);
        page(MONT_SEL[k], MONT[k]);
        if (k == 1u) slicer(2, 0x09u); else if (k == 2u) slicer(0, 0);
        if (k == 5u) slicer(2, 0x08u); else if (k == 6u) slicer(0, 0);
        caption(q, "", WORDS[k], "");
    }
    if (q == B(35, 3)) { trk[3].p[P_REV] = 110; trk[0].p[P_REV] = 90; }
    /* the end: everything stops but the pad, the reverb rings */
    if (q == B(36, 0)) {
        trk[0].p[P_MUTE] = trk[3].p[P_MUTE] = trk[4].p[P_MUTE] = trk[5].p[P_MUTE] = 1;
        song.g[G_RTYPE] = 2; go_home(); caption(q, "", "", "");
    }
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
    uint32_t f, total = (uint32_t)(FPS * SECS), beat_done = 0xFFFFFFFFu, nwav = (uint32_t)(FS * SECS);
    const char *dir = argc > 1 ? argv[1] : "build/video";
    snprintf(p, sizeof p, "%s/audio.wav", dir); wav = fopen(p, "wb");
    snprintf(p, sizeof p, "%s/screen.rgb", dir); rgb = fopen(p, "wb");
    snprintf(p, sizeof p, "%s/scenes.txt", dir); cap = fopen(p, "w");
    if (!wav || !rgb || !cap) { fprintf(stderr, "cannot write in %s\n", dir); return 1; }
    wav_hdr(wav, nwav);                         /* (hostsim.c) */
    fprintf(cap, "# %u fps, %u BPM, %u bars, splash %.4f s, end %.4f s\n", FPS, BPM, BARS, SPLASH_BEATS * BEAT_S, SECS);
    song_build();
    for (f = 0; f < total; f++) {
        double t = (double)f / FPS;
        uint32_t want = (uint32_t)((uint64_t)(f + 1u) * FS / FPS), beat = (uint32_t)(t / BEAT_S);
        if (beat != beat_done) { beat_done = beat; at_beat(beat); }
        while (nsamp < want && nsamp < nwav) {                          /* the audio up to this frame's end */
            int32_t o[2 * CTL];
            int16_t s[2 * CTL];
            uint32_t i, n = CTL;
            mix_block(o, CTL);
            if (nsamp + n > nwav) n = nwav - nsamp;
            for (i = 0; i < 2u * n; i++) s[i] = (int16_t)clamp(o[i], -32768, 32767);
            fwrite(s, 2, 2u * n, wav);
            nsamp += n;
        }
        host_ticks += 1000000u / FPS; fm1_ms += 1000u / FPS;
        if (t < SPLASH_BEATS * BEAT_S) {                                   /* the power-on splash: the signature written */
            if (f == 0) { spl_anim = 1; draw_splash(); }
            splash_write((uint32_t)(t * 1000.0));
        } else {
            static int first = 1;
            if (first) { ui.force = 1; first = 0; }
            ui_input();
            ui_draw();
        }
        put_screen();
    }
    fclose(wav); fclose(rgb); fclose(cap);
    printf("%u frames, %u samples\n", total, nsamp);
    return 0;
}
