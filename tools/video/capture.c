/* SPDX-License-Identifier: GPL-3.0-only
 * (JIANT 0.6.11) the presentation video's footage: the firmware's own UI and audio, from the host build of the tests
 * (tests/ui_test.c): an electro song at 125 BPM (A minor, six tracks: a talking robot on VOICE, a sync bass on ANALOG,
 * a computer arp on LOFI, DRUM-X, an FM6 pad, NOISE zaps) played by the real engines, soft at first, electro techno at
 * the peak, its screen captured at 30 fps as the script below goes through what JIANT FM has, beat by beat. The
 * captions go to scenes.txt (time, tag, title, subtitle, camera: 0 the screen, 1 a close-up of its living panel), from
 * the same script.
 *   cc -O2 -w -Ibuild/gen -Ifirmware/src -Itests -o build/video/capture tools/video/capture.c -lm
 *   build/video/capture build/video        -> audio.wav (44.1 kHz stereo), screen.rgb (240 x 240 RGB24 a frame)
 * tools/video/compose.py makes the 1080p video from them. */
#define UI_TEST_NO_MAIN 1
#define FELUCCA_VERSION "v0.6.14"
#include "ui_test.c"

#define FPS 30u
#define BPM 125u
#define BARS 72u
#define BEAT_S (60.0 / BPM)
#define SECS (BARS * 4u * 60.0 / BPM)
#define SPLASH_BEATS 8u                             /* the power-on splash: the first two bars */
#define B(bar, q) ((bar) * 4u + (q))                /* the beat q of a bar */
enum { T_VOX, T_BASS, T_ARP, T_DRUM, T_PAD, T_ZAP };   /* the six tracks */
static const char *const ONLY = "ONLY ON JIANT FM";
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
        put(T_DRUM, i, c, n, c ? ST_NOTE : ST_REST, acc ? SF_ACCENT : 0u);
        step_to_grid(&trk[T_DRUM].step[i]);
    }
}
/* the beats: a soft pulse, the electro break (syncopated kick, claps on 2 and 4), electro techno at the peak */
static const char *const SOFTK[8] = {"X.......X.......", 0, 0, "....x.......x...", 0, 0, 0, 0};
static const char *const ELECTRO[8] = {"X..X..X...X.X...", 0, "....X.......X...", "x.x.x.x.x.x.x.x.", 0, 0, "......x.......x.",
                                       "...x.......x...."};
static const char *const PEAK[8] = {"X..X..X.X.X..X..", 0, "....X.......X..X", "xxxxxxxxxxxxxxxx", 0, 0, 0,
                                    "..x.....x.....x."};
static const char *const FLOOR[8] = {"X...X...X...X...", 0, "....X.......X...", "xx.xxx.xxx.xxx.x", "..x...x...x...x.",
                                     ".........x....x.", 0, 0};

/* the voice bank (fm6_vbank.c) as a sent .syx would leave it: 32 voices, here the factory patches renamed */
static int vbank_read(uint32_t k, uint8_t *pk)
{
    static const char *const NM[4] = {"LASER BELL", "ROBO BRASS", "TIN CHOIR ", "NEON KEYS "};
    memcpy(pk, FM6_FACTORY[k % FM6_NFACTORY], FM6_PACKED);
    memcpy(pk + 118, NM[k % 4u], 10);
    return 0;
}

static void song_build(void)
{
    static const uint8_t ROOT[4] = {33, 29, 36, 31};                 /* Am F C G, a bar each */
    static const int8_t BOFS[16] = {0, -1, 12, 0, -1, 0, 12, -1, 0, -1, 7, -1, 12, -1, 7, -1};   /* the electro bass */
    static const uint8_t CH[4][4] = {{57, 60, 64, 69}, {53, 57, 60, 65}, {55, 60, 64, 67}, {55, 59, 62, 67}};
    static const uint8_t AIX[16] = {0, 1, 2, 3, 2, 1, 0, 1, 2, 3, 2, 1, 0, 1, 2, 3};   /* the computer arp */
    /* the robot's melody (64 steps: note, 0 rest, 1 hold) */
    static const uint8_t MEL[64] = {
        76, 1, 1, 1, 74, 1, 72, 1, 1, 1, 69, 1, 1, 1, 1, 1,
        72, 0, 72, 0, 69, 0, 72, 1, 1, 1, 74, 0, 72, 1, 1, 1,
        76, 1, 1, 1, 79, 0, 76, 0, 74, 1, 1, 1, 72, 0, 74, 0,
        71, 1, 1, 1, 74, 0, 0, 0, 71, 0, 67, 1, 1, 1, 1, 1};
    uint32_t i, k;
    ui_power_on();
    settings.palette = UI_JIANT_INDEX; palette_set(UI_JIANT_INDEX);
    song.g[G_BPM] = BPM;
    song.master_q12 = 4096;                                  /* (the pot: ui_power_on leaves 0) */
    sound(T_VOX, "VOICE", "VOX LEAD");
    sound(T_BASS, "ANALOG", "FAT BASS");
    sound(T_ARP, "LOFI", "8BIT KEYS");
    sound(T_DRUM, "DRUM", "DRUM-X");
    sound(T_PAD, "FM6", "PAD");
    sound(T_ZAP, "NOISE", "WIND");
    for (k = 0; k < NTRK; k++) {
        track_defaults_steps(&trk[k]);
        trk[k].p[P_ROOT] = 9; trk[k].p[P_SCALE] = 2;          /* A minor */
        trk[k].p[P_SLEN] = 16;
        trk[k].p[P_MUTE] = 1;
    }
    trk[T_VOX].p[P_SLEN] = 64;
    for (i = 0; i < 64u; i++) {
        uint8_t n = MEL[i];
        put(T_VOX, i, n > 1u ? 1u : 0u, &n, n > 1u ? ST_NOTE : n ? ST_TIE : ST_REST, 0);
    }
    trk[T_VOX].p[P_REV] = 55; trk[T_VOX].p[P_DLY] = 45; trk[T_VOX].p[P_LEVEL] = 96;
    trk[T_BASS].p[P_SLEN] = 64;
    for (i = 0; i < 64u; i++) {
        uint8_t n = (uint8_t)(ROOT[i / 16u] + BOFS[i % 16u]);
        put(T_BASS, i, BOFS[i % 16u] >= 0 ? 1u : 0u, &n, BOFS[i % 16u] >= 0 ? ST_NOTE : ST_REST, i % 4u == 0u ? SF_ACCENT : 0u);
    }
    trk[T_BASS].p[P_LEVEL] = 92;
    trk[T_ARP].p[P_SLEN] = 64;
    for (i = 0; i < 64u; i++) {
        uint8_t n = CH[i / 16u][AIX[i % 16u]];
        put(T_ARP, i, 1, &n, ST_NOTE, 0);
    }
    trk[T_ARP].p[P_DLY] = 55; trk[T_ARP].p[P_LEVEL] = 70; trk[T_ARP].p[P_PAN] = -10;
    trk[T_PAD].p[P_SLEN] = 64; trk[T_PAD].p[P_SGATE] = 120;
    for (i = 0; i < 64u; i++)
        put(T_PAD, i, i % 16u == 0u ? 4u : 0u, CH[i / 16u], i % 16u == 0u ? ST_NOTE : i % 16u < 15u ? ST_TIE : ST_REST, 0);
    trk[T_PAD].p[P_REV] = 90; trk[T_PAD].p[P_CHOR] = 70; trk[T_PAD].p[P_LEVEL] = 84;
    {   uint8_t n = 45; put(T_ZAP, 0, 1, &n, ST_NOTE, 0); for (i = 1; i < 16u; i++) put(T_ZAP, i, 0, &n, ST_TIE, 0); }
    trk[T_ZAP].p[P_LEVEL] = 58; trk[T_ZAP].p[P_REV] = 100;
    drums(SOFTK);
    trk[T_DRUM].p[P_REV] = 12;
    song.g[G_RTYPE] = 0; song.g[G_RSIZE] = 100;
    song.g[G_DTIME] = 3;
    chain_config.count = 4;                                    /* (the SONG page: a chain to look at) */
    chain_config.row[0] = (chain_row_t){0, 4}; chain_config.row[1] = (chain_row_t){1, 8};
    chain_config.row[2] = (chain_row_t){2, 4}; chain_config.row[3] = (chain_row_t){0, 8};
    fm6_bank_used = 0xFFFFFFFFu; fm6_bank_read = vbank_read;  /* (a bank sent from the editor) */
    transport_req = 1;
}

static void caption(uint32_t q, const char *tag, const char *title, const char *sub, int zoom)   /* (lines split by '|') */
{
    fprintf(cap, "%.4f\t%s\t%s\t%s\t%d\n", q * BEAT_S, tag, title, sub, zoom);
}
static void page(uint32_t sel, const char *title) { song.sel = (uint8_t)sel; go_title(title); }
static void home(uint32_t sel) { song.sel = (uint8_t)sel; go_home(); }
static void slicer(uint32_t mode, uint32_t mask)
{
    uint32_t k;
    song.g[G_SLMODE] = (int16_t)mode; song.g[G_SLPAT] = 9; song.g[G_SLRATE] = 1; song.g[G_SLDEP] = 127;
    for (k = 0; k <= NTRK; k++) song.g[G_SLT1 + k] = (int16_t)((mask >> k) & 1u);
}
static void hold_btn(uint32_t label, int on)
{
    if (on) { fm1_in.buttons |= 1u << panel.btn[label]; host_pressed |= 1u << panel.btn[label]; }
    else fm1_in.buttons &= ~(1u << panel.btn[label]);
}
static void enc_turn(uint32_t role, int32_t s) { host_enc[panel.enc[role]] += s * panel.dir[role]; }
static void mute(uint32_t k, int m) { trk[k].p[P_MUTE] = (int16_t)m; }

/* the script: at each beat q (bar q / 4); the captions go with it */
static void at_beat(uint32_t q)
{
    static const char *const WORDS[16] = {"DRUM-X", "SLICER BUS", "RESO", "CLOUD", "SHIMMER", "GRAIN DELAY", "AUTOPAN",
                                          "MSEQ", "MACROS", "DICE", "PUNCH-IN", "SONGS", "6 TRACKS", "FM6 OPS",
                                          "VOICE BANK", "ENV LOOP"};
    static const char *const MPG[16] = {"SOUND", "SLICER", "REVERB", "REVERB", "REVERB", "DLY 2", "AUTOPAN", "MSEQ",
                                        "MOD", "MOD", "FX", "SONG", "MIXER", "OPERATOR", "EDIT 2", "ENV DEST"};
    static const uint8_t MSEL[16] = {T_DRUM, T_DRUM, T_VOX, T_VOX, T_VOX, T_ARP, T_ARP, T_VOX, T_VOX, T_VOX, T_VOX, T_VOX,
                                     T_VOX, T_PAD, T_PAD, T_PAD};
    static const uint8_t MRT[16] = {0, 0, 3, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint32_t bar = q / 4u, k;

    /* ---- the intro: the pad and the wind under the splash, then the robot and the computer arp */
    if (q == B(0, 0)) { mute(T_PAD, 0); mute(T_ZAP, 0); }
    if (q == B(2, 0)) {
        mute(T_ARP, 0); home(T_ARP);
        caption(q, ONLY, "THE BEINGS", "every track a living creature|they breathe with the sound", 1);
    }
    if (q == B(4, 0)) {
        mute(T_VOX, 0); page(T_VOX, "EDIT 1");
        caption(q, ONLY, "THE ROBOT", "VOICE: formant synthesis|its own being on every engine page", 1);
    }
    if (q == B(6, 0)) { page(T_ARP, "EDIT 1"); caption(q, ONLY, "LOFI", "bytebeat and floatbeat machines|8-bit keys and chips", 1); }
    if (q == B(7, 0)) { trk[T_ARP].p[P_E0] = (int16_t)(trk[T_ARP].p[P_E0] ^ 1); }
    if (q == B(8, 0)) {
        mute(T_BASS, 0); trk[T_BASS].p[P_E0] = 5;           /* the bass: ANALOG's SYNC */
        page(T_BASS, "EDIT 1"); caption(q, ONLY, "ANALOG", "SYNC · RING · SAW3|two oscillators, INT ±24 st", 1);
    }
    if (q == B(9, 0)) {
        trk[T_BASS].p[P_FTYPE] = 0; page(T_BASS, "FILTER");
        caption(q, ONLY, "FILTERS", "LP · HP · BP · COMB|on every engine", 0);
    }
    if (q >= B(9, 0) && q < B(10, 0)) trk[T_BASS].p[P_FTYPE] = (int16_t)(q - B(9, 0));
    /* ---- the drums: the soft pulse, then DRUM-X's pages */
    if (q == B(10, 0)) {
        trk[T_BASS].p[P_FTYPE] = 0; mute(T_DRUM, 0); page(T_DRUM, "EDIT 1");
        caption(q, ONLY, "DRUM-X", "8 synthesized voices, no samples|a living colony on screen", 1);
    }
    if (q >= B(10, 0) && q < B(14, 0)) trk[T_DRUM].p[P_E0] = (int16_t)((q - B(10, 0)) * 60u / 15u);
    if (q == B(11, 0)) { dx_ui_lane = 0; page(T_DRUM, "SOUND"); caption(q, ONLY, "DRUM-X", "every voice its own|pitch · decay · noise · color · wave", 1); }
    if (q == B(11, 2)) dx_ui_lane = 3;
    if (q == B(12, 0)) {
        drums(ELECTRO); trk[T_DRUM].p[P_E0] = 30;
        dx_mot[XM_RATE] = 5; dx_mot[XM_DPTH] = 70; dx_mot[XM_SHPE] = 1; dx_mot[XM_RAND] = 30;
        page(T_DRUM, "X-MOD"); caption(q, ONLY, "X-MOD", "the kit's own LFO on MORPH|and a random one per hit", 1);
    }
    if (q == B(13, 0)) { page(T_DRUM, "EDIT 2"); caption(q, ONLY, "MORPH", "from kit A to kit B|NOISE · FM · FOLD · DRIVE", 1); }
    if (q == B(14, 0)) { mute(T_ZAP, 1); home(T_DRUM); caption(q, ONLY, "6 TRACKS", "eight engines, all synthesis|six beings at once", 1); }
    /* ---- the groove: modulation, effects, sequencers */
    if (q == B(16, 0)) {
        trk[T_PAD].p[P_ELOOP] = 1; trk[T_PAD].p[P_ED_FLT] = 30; page(T_PAD, "ENV DEST");
        caption(q, ONLY, "ENVELOPES THAT LOOP", "an ADSR held cycles|like a shaped LFO", 0);
    }
    if (q == B(17, 0)) {
        trk[T_ARP].p[P_M1SRC] = MS_LFO; trk[T_ARP].p[P_M1DST] = MD_CUT; trk[T_ARP].p[P_M1AMT] = 30;
        page(T_ARP, "MOD"); caption(q, ONLY, "MOD MATRIX", "4 slots a track|macros · MSEQ · the master", 0);
    }
    if (q == B(18, 0)) {                                      /* the robot talks: MSEQ on its vowel */
        trk[T_VOX].p[P_M1SRC] = MS_STEP; trk[T_VOX].p[P_M1DST] = MD_E1; trk[T_VOX].p[P_M1AMT] = 60;
        trk[T_VOX].p[P_MSLEN] = 8; trk[T_VOX].p[P_MSDIV] = 3;
        for (k = 0; k < 16u; k++) trk[T_VOX].p[P_MS0 + k] = (int16_t)((k * 47u + 20u) % 128u);
        page(T_VOX, "MSEQ"); caption(q, ONLY, "MSEQ", "a 16-step modulation sequencer|here: the robot's vowels", 0);
    }
    if (q == B(19, 0)) { page(T_VOX, "EDIT 1"); caption(q, ONLY, "THE ROBOT TALKS", "the being follows the vowels", 1); }
    if (q == B(20, 0)) { home(T_VOX); hold_btn(B_LFO, 1); caption(q, ONLY, "MACROS · DICE", "4 macros over what's playing|SELECT rolls new routes", 0); }
    if (q >= B(20, 0) && q < B(21, 0)) for (k = 0; k < 4u; k++) macro_v[k] = (uint8_t)clamp((int32_t)((q - B(20, 0) + 1u) * (12u + 5u * k)), 0, 127);
    if (q == B(20, 2)) { macro_dice(1); ui_message("MACROS DICED"); }
    if (q == B(20, 3)) { hold_btn(B_LFO, 0); for (k = 0; k < 4u; k++) macro_v[k] = 0; }
    if (q == B(21, 0)) hold_btn(B_LFO, 1);                     /* (LFO again: DONE) */
    if (q == B(21, 1)) hold_btn(B_LFO, 0);
    if (q == B(22, 0)) {
        trk[T_ARP].p[P_APDEP] = 110; trk[T_ARP].p[P_APRATE] = 5; trk[T_ARP].p[P_APSYNC] = 1; trk[T_ARP].p[P_APWAVE] = 1;
        page(T_ARP, "AUTOPAN"); caption(q, ONLY, "AUTOPAN", "on the beat or free|SIN · TRI · SAW · SQR · S&H", 0);
    }
    if (q == B(23, 0)) { song.g[G_CSTER] = 50; page(T_PAD, "CHORUS"); caption(q, ONLY, "CHORUS STEREO", "from mono to twice as wide", 0); }
    if (q == B(24, 0)) {
        trk[T_BASS].p[P_DIST] = 50; page(T_BASS, "DIST");
        caption(q, ONLY, "5 DISTORTIONS", "SOFT · HARD · FOLD · CRUSH · RECT|on every track", 0);
    }
    if (q >= B(24, 0) && q < B(26, 0) && q % 2u == 0u) trk[T_BASS].p[P_DTYPE] = (int16_t)((q - B(24, 0)) / 2u % 5u);
    if (q == B(26, 0)) {
        trk[T_BASS].p[P_DTYPE] = 2; trk[T_BASS].p[P_DIST] = 30;
        song.g[G_DPIT] = 7; song.g[G_DFDBK] = 75; page(T_ARP, "DLY 2");
        caption(q, ONLY, "GRAIN DELAY", "repeats pitched ±12 semitones|grain spray", 0);
    }
    if (q == B(27, 0)) song.g[G_DPIT] = 12;
    if (q == B(28, 0)) {
        song.g[G_DPIT] = 0; song.g[G_DFDBK] = 60;
        page(T_VOX, "STEP"); caption(q, ONLY, "SEQUENCERS", "piano roll · chance · ratchets|shift · parameter locks", 0);
    }
    if (q == B(28, 2)) page(T_VOX, "CHANCE");
    if (q == B(29, 0)) page(T_VOX, "SHIFT");
    if (q == B(29, 2)) page(T_VOX, "AUTOMATION");
    if (q == B(30, 0)) { page(T_DRUM, "STEP"); caption(q, ONLY, "SEQ TOOLS", "reverse · shift · random · cook|beat · clear automation", 0); }
    if (q == B(30, 1)) hold_btn(B_SEQ, 1);
    if (q == B(31, 3)) hold_btn(B_SEQ, 0);
    if (q == B(32, 0)) { home(T_BASS); caption(q, ONLY, "THE BEINGS", "they swell and heat up|with every hit", 1); }
    /* ---- the breakdown: the drums and the bass out, the reverbs */
    if (q == B(34, 0)) {
        mute(T_DRUM, 1); mute(T_BASS, 1); trk[T_ARP].p[P_LEVEL] = 56;
        song.g[G_RTYPE] = 3; song.g[G_RDAMP] = 1; song.g[G_RPRE] = 2; trk[T_VOX].p[P_REV] = 90;
        page(T_VOX, "REVERB"); caption(q, ONLY, "RESO REVERB", "it rings in the scale|and follows the robot's notes", 0);
    }
    if (q == B(36, 0)) { song.g[G_RTYPE] = 2; caption(q, ONLY, "5 REVERBS", "ROOM · SPRING · SHIMMER|RESO · CLOUD", 0); }
    if (q == B(37, 0)) song.g[G_RTYPE] = 4;
    if (q == B(38, 0)) {
        song.g[G_RTYPE] = 3; page(T_PAD, "OPERATOR");
        caption(q, ONLY, "FM6 · FULLY EDITABLE", "six operators on the device|ratio · level · envelopes", 0);
    }
    if (q >= B(38, 0) && q < B(39, 0)) enc_turn(EN_K1, 1);
    if (q == B(39, 0)) page(T_PAD, "OP ENV");
    if (q >= B(39, 0) && q < B(40, 0)) enc_turn(EN_K2, 1);
    if (q == B(40, 0)) {
        trk[T_PAD].p[P_E7] = (int16_t)(FM6_OWN + 1u); page(T_PAD, "EDIT 2");
        caption(q, ONLY, "VOICE BANK", "a 32-voice DX7 cartridge|from the web editor, SLOT B1–B32", 0);
    }
    if (q == B(41, 0)) trk[T_PAD].p[P_E7] = (int16_t)(FM6_OWN + 3u);
    /* ---- the build and the peak: electro techno */
    if (q == B(42, 0)) {
        mute(T_DRUM, 0); mute(T_BASS, 0); drums(ELECTRO); trk[T_ARP].p[P_LEVEL] = 70; trk[T_VOX].p[P_REV] = 55;
        slicer(2, (1u << T_DRUM) | (1u << T_VOX));
        song.g[G_SLPLEN] = 4; song.g[G_SLP0] = 0; song.g[G_SLP0 + 1] = 12; song.g[G_SLP0 + 2] = 7; song.g[G_SLP0 + 3] = -5;
        page(T_DRUM, "SLICER"); caption(q, ONLY, "SLICER BUS", "stutter and gate after the effects|on any of the 6 tracks", 0);
    }
    if (q == B(43, 0)) { page(T_DRUM, "SL PITCH"); caption(q, ONLY, "SLICER BUS", "its own 16-step pitch sequencer", 0); }
    if (q == B(44, 0)) { slicer(0, 0); drums(PEAK); home(T_VOX); hold_btn(B_FX, 1); caption(q, ONLY, "PUNCH-IN FX", "hold FX, play the keys|repeat · octave · stutter", 0); }
    if (bar == 44u && q % 4u == 3u) perf_press(PF_R16, 1);
    if (bar == 45u) { perf_press(PF_R16, q % 4u == 0u); perf_press(PF_OCTU, q % 4u == 1u || q % 4u == 2u); perf_press(PF_S16, q % 4u >= 2u); }
    if (q == B(46, 0)) {
        hold_btn(B_FX, 0); perf_press(PF_S16, 0); perf_press(PF_OCTU, 0); perf_press(PF_R16, 0);
        song.g[G_CLIP] = 55; song.g[G_PUNCH] = 80; song.g[G_DUCK] = 40;
        page(T_DRUM, "MASTER"); caption(q, ONLY, "MASTER", "clipper · drum punch|sidechain duck · leveler", 0);
    }
    if (q == B(47, 0)) {
        trk[T_DRUM].p[P_E0] = 110; trk[T_DRUM].p[P_E7] = 70; trk[T_DRUM].p[P_E6] = 40;
        dx_mot[XM_RATE] = 6; dx_mot[XM_DPTH] = 100; dx_mot[XM_SHPE] = 4;
        page(T_DRUM, "EDIT 1"); caption(q, ONLY, "DRUM-X", "the colony morphs to kit B|harder, louder", 1);
    }
    if (q == B(48, 0)) {
        drums(FLOOR); sound(T_ZAP, "NOISE", "ARCADE");
        for (k = 0; k < 16u; k++) { uint8_t n = 84; put(T_ZAP, k, k == 3u || k == 11u ? 1u : 0u, &n, k == 3u || k == 11u ? ST_NOTE : ST_REST, 0); }
        trk[T_ZAP].p[P_ROOT] = 9; trk[T_ZAP].p[P_SCALE] = 2; trk[T_ZAP].p[P_SLEN] = 16; mute(T_ZAP, 0); trk[T_ZAP].p[P_LEVEL] = 55;
        home(T_DRUM); caption(q, ONLY, "ALL SYNTHESIS", "no samples|six beings, all alive", 1);
    }
    if (q == B(50, 0)) { go_title("MIXER"); caption(q, ONLY, "6-CHANNEL MIXER", "level · pan · send · mute|thermal meters", 0); }
    if (q == B(52, 0)) { go_title("SONG"); caption(q, ONLY, "SONGS", "8 songs × 4 sections|chains · scenes, SLOOP-style", 0); }
    if (q == B(54, 0)) { home(T_PAD); hold_btn(B_ENV, 1); caption(q, ONLY, "PAGE JUMP", "hold a page button|turn PRESETS: straight to a page", 0); }
    if (q > B(54, 0) && q < B(55, 3)) enc_turn(EN_PRESET, q % 2u ? 1 : -1);
    if (q == B(55, 3)) hold_btn(B_ENV, 0);
    if (q >= B(56, 0) && q < B(60, 0)) {                      /* the montage: a page and a word a beat */
        k = q - B(56, 0);
        page(MSEL[k], MPG[k]);
        if (MRT[k]) song.g[G_RTYPE] = MRT[k];
        if (k == 1u) slicer(2, (1u << T_DRUM)); else if (k == 2u) slicer(0, 0);
        caption(q, "", WORDS[k], "", 0);
    }
    if (q == B(60, 0)) { song.g[G_RTYPE] = 3; drums(PEAK); home(T_DRUM); caption(q, "", "JIANT FM", "a living synth|in your pocket", 1); }
    if (q == B(62, 0)) { slicer(2, (1u << T_DRUM) | (1u << T_BASS)); }
    if (q == B(62, 2)) slicer(0, 0);
    if (q == B(63, 3)) { trk[T_DRUM].p[P_REV] = 100; trk[T_BASS].p[P_REV] = 80; }
    /* ---- the outro: the beat stops, the robot sings alone in RESO */
    if (q == B(64, 0)) {
        mute(T_DRUM, 1); mute(T_BASS, 1); mute(T_ZAP, 1); mute(T_ARP, 1); song.g[G_CLIP] = 0;
        home(T_VOX); caption(q, ONLY, "THE BEINGS", "rest", 1);
    }
    if (q == B(66, 0)) { caption(q, "", "", "", 0); }        /* (the outro card) */
    if (q == B(70, 0)) mute(T_VOX, 1);
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
    fprintf(cap, "# %u fps, %u BPM, %u bars, splash %.4f s, end %.4f s, beat %.6f s\n", FPS, BPM, BARS, SPLASH_BEATS * BEAT_S,
            SECS, BEAT_S);
    song_build();
    for (f = 0; f < total; f++) {
        double t = (double)f / FPS;
        uint32_t want = (uint32_t)((uint64_t)(f + 1u) * FS / FPS), beat = (uint32_t)(t / BEAT_S);
        if (beat != beat_done) { beat_done = beat; at_beat(beat); }
        while (nsamp < want && nsamp < nwav) {          /* the audio up to this frame's end */
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
        if (t < SPLASH_BEATS * BEAT_S) {                 /* the power-on splash: the signature written */
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
