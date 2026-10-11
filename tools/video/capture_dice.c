/* SPDX-License-Identifier: GPL-3.0-only
 * (JIANT 0.6.34) the DICE ALL film's footage: the generator thrown live on the host build of the tests (tests/ui_test.c),
 * the real engines playing each piece at its own tempo, the FM-1's screen at 30 fps. A script of throws: each one
 * HOME + SELECT (the dialog), OCT+ (YES), then the piece for a few bars while the camera visits its pages (HOME's beings,
 * the lead's piano roll, the kit, the mixer: what plays and what stays silent); then a montage of quick throws, and a
 * last one, in SPACE, under the outro. The captions (genre, energy, how many tracks, the kit or no drums, the twist) go
 * to scenes.txt, every beat's time to beats.txt (the tempo changes), the intro's words to intro.txt.
 *   cc -O2 -w -Ibuild/gen -Ifirmware/src -Itests -o build/video_dice/capture tools/video/capture_dice.c -lm
 *   build/video_dice/capture build/video_dice      -> audio.wav, screen.rgb, scenes.txt, beats.txt, intro.txt
 *   python3 tools/video/compose.py build/video_dice -> build/video_dice/jiant-fm.mp4 */
#define UI_TEST_NO_MAIN 1
#define FELUCCA_VERSION "v0.6.34"
#include "ui_test.c"

#define FPS 30u
#define SPLASH_S 3.0
static FILE *wav, *rgb, *cap, *bts;
static uint32_t nsamp;
static const char *const ONLY = "DICE ALL";
static const char *const KIT_N[] = {"808 KIT", "HARD KIT", "CLICK KIT", "INDUS KIT", "SOFT KIT", "GLITCH KIT", "ELECTRO KIT"};
static const char *const TW_N[] = {"", "TWIST: PHASE", "TWIST: COMB", "TWIST: GRAIN DOWN", "TWIST: POLYMETER", "TWIST: DRONE"};
enum { G_IDM, G_DIDM, G_DMIN, G_DUB, G_DELEC, G_ACID, G_COLD, G_BROKEN, G_HYPNO, G_BREAKS, G_SOUND, G_HARD, G_INDIE,
       G_PUNK, G_CLASS };
/* the script: genre, energy, bars (0: as many as ~9 s), a quick one (the montage) */
static const struct { int8_t g, e; uint8_t bars, quick; } SCRIPT[] = {
    {G_SOUND, 0, 0, 0}, {G_CLASS, 0, 0, 0}, {G_DIDM, 0, 0, 0}, {G_DUB, 1, 0, 0}, {G_INDIE, 1, 0, 0}, {G_IDM, 2, 0, 0},
    {G_DELEC, 1, 0, 0}, {G_PUNK, 2, 0, 0}, {G_HARD, 2, 0, 0},
    {G_HYPNO, 1, 1, 1}, {G_ACID, 2, 1, 1}, {G_COLD, 1, 1, 1}, {G_BROKEN, 1, 1, 1}, {G_BREAKS, 2, 1, 1}, {G_DMIN, 1, 1, 1},
    {G_SOUND, 0, 4, 0},
};
#define NSCRIPT (sizeof SCRIPT / sizeof SCRIPT[0])

static void hold_btn(uint32_t label, int on)
{
    if (on) { fm1_in.buttons |= 1u << panel.btn[label]; host_pressed |= 1u << panel.btn[label]; }
    else fm1_in.buttons &= ~(1u << panel.btn[label]);
}
static void enc_turn(uint32_t role, int32_t s) { host_enc[panel.enc[role]] += s * panel.dir[role]; }
static void caption(double t, const char *tag, const char *title, const char *sub, int zoom)
{
    fprintf(cap, "%.4f\t%s\t%s\t%s\t%d\n", t, tag, title, sub, zoom);
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
static int plays(uint32_t k)
{
    uint32_t i;
    for (i = 0; i < (uint32_t)trk[k].p[P_SLEN]; i++) if (trk[k].step[i].time == ST_NOTE) return 1;
    return 0;
}
static void sub_line(char *b, size_t n)                 /* "SPACE · 3 TRACKS · NO DRUMS|TWIST: PHASE" */
{
    uint32_t k, act = 0, dr = 0;
    for (k = 0; k < NTRK; k++) if (plays(k)) { act++; dr |= (uint32_t)drum_track(&trk[k]); }
    snprintf(b, n, "%s · %u TRACKS · %s%s%s · %d BPM", dh.energy == 0u ? "SPACE" : dh.energy == 2u ? "FRANTIC" : "MID", act,
             dr ? KIT_N[dh.kit % 7u] : "NO DRUMS", dh.twist ? "|" : "", TW_N[dh.twist % 6u], song.g[G_BPM]);
}
/* the melodic track to watch: of those that play above the bass (their notes' average from E3), the one with the
 * most notes (the lead or the arp, rarely the chords); else any that plays */
static int32_t show_track(void)
{
    uint32_t k, i, best = NTRK, bn = 0;
    for (k = 0; k < NTRK; k++) {
        uint32_t cnt = 0, sum = 0;
        if (!plays(k) || drum_track(&trk[k])) continue;
        for (i = 0; i < (uint32_t)trk[k].p[P_SLEN]; i++)
            if (trk[k].step[i].time == ST_NOTE && trk[k].step[i].n) { cnt++; sum += trk[k].step[i].note[0]; }
        if (cnt && sum / cnt >= 52u && cnt > bn) { bn = cnt; best = k; }
    }
    if (best < NTRK) return (int32_t)best;
    for (k = 0; k < NTRK; k++) if (plays(k) && !drum_track(&trk[k])) return (int32_t)k;
    return 0;
}

int main(int argc, char **argv)
{
    char p[512], sb[96];
    const char *dir = argc > 1 ? argv[1] : "build/video_dice";
    uint32_t f, si = 0, phase = 0, page_k = 0, nbars = 0, total;
    double t_ev = 0, bar_s = 2.0, beat_acc = 0, end_t = 0;
    int first_screen = 1;
    snprintf(p, sizeof p, "%s/audio.wav", dir); wav = fopen(p, "wb");
    snprintf(p, sizeof p, "%s/screen.rgb", dir); rgb = fopen(p, "wb");
    snprintf(p, sizeof p, "%s/scenes.txt", dir); cap = fopen(p, "w");
    snprintf(p, sizeof p, "%s/beats.txt", dir); bts = fopen(p, "w");
    if (!wav || !rgb || !cap || !bts) { fprintf(stderr, "cannot write in %s\n", dir); return 1; }
    wav_hdr(wav, 0);                                    /* (its length written at the end) */
    snprintf(p, sizeof p, "%s/intro.txt", dir);
    { FILE *it = fopen(p, "w"); if (it) { fputs("DICE ALL|one press, a whole new piece", it); fclose(it); } }
    ui_power_on();
    settings.palette = UI_JIANT_INDEX; palette_set(UI_JIANT_INDEX);
    ui_rec_prefs |= PREF_SIX >> 8; rp_apply();          /* TRACKS 6 */
    song.master_q12 = 4096;
    host_ticks = 777u;
    /* the first piece under the splash (no dialog); the rest as the script goes */
    dice_force_style = SCRIPT[0].g; dice_force_energy = SCRIPT[0].e;
    dice_all();
    transport_req = 1;
    bar_s = 4.0 * 60.0 / song.g[G_BPM];
    nbars = (uint32_t)(SPLASH_S / bar_s) + 3u;
    phase = 4; t_ev = SPLASH_S + 2.2;                   /* (its caption after the intro title) */
    total = 0;
    end_t = 1e9;
    for (f = 0; ; f++) {
        double t = (double)f / FPS;
        uint32_t want = (uint32_t)((uint64_t)(f + 1u) * FS / FPS);
        if (t >= end_t) break;
        /* ---- the script */
        if (t >= t_ev) {
            if (phase == 0) {                           /* HOME + SELECT: the dialog */
                song.sel = 0; go_home(); enc_turn(EN_SELECT, 1); fm1_ms += 1200u;
                phase = 1; t_ev = t + (SCRIPT[si].quick ? 0.35 : 0.6);
            } else if (phase == 1) {                    /* OCT+: YES, the piece */
                dice_force_style = SCRIPT[si].g; dice_force_energy = SCRIPT[si].e;
                hold_btn(B_OCTUP, 1);
                phase = 2; t_ev = t + 0.07;
            } else if (phase == 2) {                    /* the button up: the dice (a tap acts on its release) */
                hold_btn(B_OCTUP, 0);
                phase = 4; t_ev = si ? t + 0.1 : SPLASH_S + 2.2;
            } else if (phase == 4) {                    /* the piece's length and caption */
                const char *gname = DSTYLE[(uint32_t)SCRIPT[si].g % NDSTYLE].name;
                bar_s = 4.0 * 60.0 / song.g[G_BPM];
                if (SCRIPT[si].bars) nbars = SCRIPT[si].bars;
                else { nbars = (uint32_t)(9.0 / bar_s + 0.5); if (nbars < 2u) nbars = 2u; if (nbars > 6u) nbars = 6u; }
                sub_line(sb, sizeof sb);
                if (si + 1u == NSCRIPT) caption(t, ONLY, gname, sb, 0);
                else if (SCRIPT[si].quick) caption(t, "", gname, sb, 0);
                else caption(si ? t : SPLASH_S + 2.2, ONLY, gname, sb, 0);
                page_k = 0;
                phase = 3; t_ev = (si ? t : SPLASH_S + 2.2) + bar_s;
                if (si + 1u == NSCRIPT) {               /* the last: the outro over it */
                    caption(t + 2.0 * bar_s, "", "", "", 0);
                    end_t = t + 2.0 * bar_s + 7.0;
                    t_ev = 1e9;
                }
            } else if (phase == 3) {                    /* the camera: a page a bar */
                int32_t k = show_track();
                uint32_t dr = NTRK, j;
                for (j = 0; j < NTRK; j++) if (drum_track(&trk[j]) && plays(j)) dr = j;
                page_k++;
                if (page_k >= nbars - (SCRIPT[si].quick ? 0u : 0u)) {   /* the next throw */
                    si++; phase = 0; t_ev = t;
                    if (si >= NSCRIPT) { si = NSCRIPT - 1u; t_ev = 1e9; }
                    continue;
                }
                switch (page_k % 4u) {
                case 1: song.sel = (uint8_t)k; go_title("STEP"); break;
                case 2: if (dr < NTRK) { song.sel = (uint8_t)dr; go_title("EDIT 1"); } else { song.sel = (uint8_t)k; go_title("EDIT 1"); } break;
                case 3: go_title("MIXER"); break;
                default: song.sel = (uint8_t)k; go_home(); break;
                }
                t_ev = t + bar_s;
            }
        }
        /* ---- the audio up to this frame's end, the beats' times */
        while (nsamp < want) {
            int32_t o[2 * CTL];
            int16_t s[2 * CTL];
            uint32_t i;
            mix_block(o, CTL);
            for (i = 1; i < CTL; i += 2u)
                scope_buf[scope_w++ & (SCOPE_N - 1u)] = (int16_t)clamp(o[2u * i], -32768, 32767);
            for (i = 0; i < 2u * CTL; i++) s[i] = (int16_t)clamp(o[i], -32768, 32767);
            fwrite(s, 2, 2u * CTL, wav);
            nsamp += CTL;
            beat_acc += (double)CTL * song.g[G_BPM] / (60.0 * FS);
            if (beat_acc >= 1.0) { beat_acc -= 1.0; fprintf(bts, "%.4f\n", (double)nsamp / FS); }
        }
        host_ticks += 1000000u / FPS; fm1_ms += 1000u / FPS;
        if (t < SPLASH_S) {
            if (f == 0) { spl_anim = 1; draw_splash(); }
            splash_write((uint32_t)(t * 1000.0));
        } else {
            if (first_screen) { ui.force = 1; first_screen = 0; go_home(); }
            ui_input();
            ui_draw();
        }
        put_screen();
        total++;
    }
    fclose(rgb); fclose(cap); fclose(bts);
    /* the header now that the length is known */
    {
        FILE *w2;
        fclose(wav);
        snprintf(p, sizeof p, "%s/audio.wav", dir);
        w2 = fopen(p, "r+b");
        if (w2) { wav_hdr(w2, nsamp); fclose(w2); }
    }
    {   /* scenes.txt's header line first (compose.py reads it): splash, end, beat */
        FILE *in, *out;
        char path2[600], line[512];
        snprintf(p, sizeof p, "%s/scenes.txt", dir);
        snprintf(path2, sizeof path2, "%s/scenes.tmp", dir);
        in = fopen(p, "r"); out = fopen(path2, "w");
        fprintf(out, "# %u fps, dice film, splash %.4f s, end %.4f s, beat %.6f s\n", FPS, SPLASH_S, (double)total / FPS, 0.5);
        while (in && fgets(line, sizeof line, in)) fputs(line, out);
        if (in) fclose(in);
        fclose(out);
        rename(path2, p);
    }
    printf("%u frames, %u samples\n", total, nsamp);
    return 0;
}
