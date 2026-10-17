/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* VOICE words test (src/sing.c, src/eng_formant.c, JIANT 0.8.5) on the host, through hostsim.c as regress.c.
 *   build/host/voice_test [DEMODIR]          (run_tests.sh: build/voice_demo)
 *   build/host/voice_test --g2p LANG WORDS..  (prints each word's phonemes, | a syllable)
 * 1. reading: Spanish by rules (a word list: phonemes and syllables), Japanese romaji -> morae, English through the
 *    dictionary (and its letter rules for a word it lacks); every bank word's syllables as gen_lyrics.py counted them.
 * 2. the generator: the same phrase for the same seed, another for another seed, as many syllables as the pattern's
 *    notes, every word a real one of the bank.
 * 3. the sound: the vowels' formants (an LPC-free peak pick on the spectrum of a held vowel) inside their ranges;
 *    s / sh / f frication bands; a plosive's closure and burst; no click at syllable changes; no DC; no clipping at
 *    the corners; LANG OFF bit for bit the vowels of before (the goldens hold that too).
 * 4. the lyric: notes sing the next syllable, a TIE holds it, "-" holds the vowel, POS jumps, LOOP restarts; a
 *    project round trip with lyrics, a project without them loads none.
 * 5. cost: host ns per sample and voice, vowels against words (the counts: tests/cpu_vg.c eng/VOICE.*).
 * 6. demos into DEMODIR: Spanish, English, Japanese phrases on a melody, the dice's lyrics, every word preset. */
#define main hostsim_main
#include "hostsim.c"
#undef main

static int bad;
static void check(const char *what, int ok)
{
    printf("voice: %-100s %s\n", what, ok ? "ok" : "FAIL");
    bad += !ok;
}
static uint32_t eng_voice(void)
{
    uint32_t e;
    for (e = 0; e < NENGINES; e++)
        if (ENGINES[e] == &ENG_FORMANT)
            return e;
    return 0;
}
static int32_t preset_of(const char *name)
{
    uint32_t k;
    for (k = 0; k < ENG_FORMANT.npresets; k++)
        if (str_eq(ENG_FORMANT.presets[k].name, name))
            return (int32_t)k;
    printf("voice: no preset %s\n", name);
    bad++;
    return 0;
}

/* a word's phonemes as text: "|k o |r a |s o n" */
static void g2p_str(uint32_t lang, const char *w, char *out, uint32_t max)
{
    uint8_t ph[SW_MAX];
    char nw[24];
    uint32_t ns, n = sy_word(lang, nw, sy_norm(w, (uint32_t)strlen(w), nw, sizeof nw), ph, &ns), k, m = 0;
    out[0] = 0;
    for (k = 0; k < n; k++) {
        m += (uint32_t)snprintf(out + m, max - m, "%s%s%s", k ? " " : "", ph[k] & SYB ? "|" : "", PH_NAME[ph[k] & 0x3Fu]);
        if (m >= max)
            break;
    }
}

/* ------------------------------------------------------------------------------------------- rendering --- */
typedef struct { uint8_t note, len; } nt_t;   /* a note (0: a rest), its length in 1/8 beats at 100 BPM */
/* track 0: preset pi, the lyric text; then the notes, each a syllable; the mix into w (if w), the voice sum into y
 * (if y: n samples at most); returns the frames */
static uint32_t sing_phrase(int32_t pi, const char *text, const nt_t *nt, uint32_t nn, FILE *w, double *y, uint32_t ymax)
{
    static int32_t o[2 * CTL];
    uint32_t k, i, frames = 0, eighth = FS * 60u / 100u / 2u;
    memset(trk, 0, sizeof trk);
    memset(sing, 0, sizeof sing);
    host_tracks_init();
    host_preset(&trk[0], eng_voice(), (uint32_t)pi);
    lyr_set(0, text);
    if (text && text[0])
        trk[0].p[P_VLYR] = 0;                           /* TEXT */
    if (getenv("DRY"))
        trk[0].p[P_CHOR] = trk[0].p[P_DLY] = trk[0].p[P_REV] = 0;
    for (k = 0; k <= nn; k++) {
        uint32_t len = k < nn ? nt[k].len * eighth : FS, note = k < nn ? nt[k].note : 0;
        if (note)
            trk_note_on(&trk[0], note, 100);
        for (i = 0; i < len; i += CTL) {
            uint32_t j;
            if (note && i + CTL >= len - len / 8u && i < len - len / 8u)
                trk_note_off(&trk[0], note);
            mix_block(o, CTL);
            if (getenv("VDEBUG")) {                   /* each phoneme as it begins */
                static uint32_t last;
                const sing_t *s = &sing[0];
                uint32_t key = (uint32_t)s->nsy << 16 | (uint32_t)s->i << 8 | s->st;
                if (key != last)
                    printf("%6.3f  #%-3u %-12s %-3s st%u  F %4d %4d %4d  ff %4d  v %5d a %5d f %5d\n",
                           (double)frames / FS, s->nsy, s->w, PH_NAME[sing_phon(s)], s->st, s->f[0], s->f[1], s->f[2],
                           s->ff, s->gv, s->ga, s->gf);
                last = key;
            }
            for (j = 0; j < CTL; j++) {
                if (w)
                    wav_put(w, o[2 * j], o[2 * j + 1]);
                if (y && frames + j < ymax)
                    y[frames + j] = (o[2 * j] + o[2 * j + 1]) / 65536.0;
            }
            frames += CTL;
        }
    }
    return frames;
}
static void demo_wav(const char *dir, const char *name, int32_t pi, const char *text, const nt_t *nt, uint32_t nn)
{
    char path[512];
    uint32_t frames;
    FILE *w;
    snprintf(path, sizeof path, "%s/%s.wav", dir, name);
    if (!(w = fopen(path, "wb")))
        return;
    wav_hdr(w, 0);
    frames = sing_phrase(pi, text, nt, nn, w, 0, 0);
    fseek(w, 0, SEEK_SET);
    wav_hdr(w, frames);
    fclose(w);
}

/* ------------------------------------------------------------------------------------------ the checks --- */
#define NFFT 4096
static void fft(double *re, double *im, uint32_t n)
{
    uint32_t i, j, k, m;
    for (i = 1, j = 0; i < n; i++) {
        uint32_t bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j) {
            double t = re[i]; re[i] = re[j]; re[j] = t;
            t = im[i]; im[i] = im[j]; im[j] = t;
        }
    }
    for (m = 2; m <= n; m <<= 1) {
        double a = -2.0 * M_PI / m;
        for (k = 0; k < n; k += m)
            for (j = 0; j < m / 2u; j++) {
                double wr = cos(a * j), wi = sin(a * j);
                double xr = re[k + j + m / 2u] * wr - im[k + j + m / 2u] * wi;
                double xi = re[k + j + m / 2u] * wi + im[k + j + m / 2u] * wr;
                re[k + j + m / 2u] = re[k + j] - xr; im[k + j + m / 2u] = im[k + j] - xi;
                re[k + j] += xr; im[k + j] += xi;
            }
    }
}
/* the power spectrum of y[0..n) (Welch: Hann windows of NFFT, half overlapping), into P[NFFT / 2] */
static void spectrum(const double *y, uint32_t n, double *P)
{
    static double re[NFFT], im[NFFT];
    uint32_t a, i, w = 0;
    for (i = 0; i < NFFT / 2u; i++)
        P[i] = 0;
    for (a = 0; a + NFFT <= n; a += NFFT / 2u, w++) {
        for (i = 0; i < NFFT; i++) {
            re[i] = y[a + i] * (0.5 - 0.5 * cos(2.0 * M_PI * i / NFFT));
            im[i] = 0;
        }
        fft(re, im, NFFT);
        for (i = 0; i < NFFT / 2u; i++)
            P[i] += re[i] * re[i] + im[i] * im[i];
    }
    if (!w && n) {                                       /* shorter than a window: one, zero-padded */
        for (i = 0; i < NFFT; i++) {
            re[i] = i < n ? y[i] * (0.5 - 0.5 * cos(2.0 * M_PI * i / n)) : 0;
            im[i] = 0;
        }
        fft(re, im, NFFT);
        for (i = 0; i < NFFT / 2u; i++)
            P[i] = re[i] * re[i] + im[i] * im[i];
    }
}
static double bin_hz(uint32_t k) { return (double)k * FS / NFFT; }
/* the strongest frequency of P (smoothed over +-sm bins) in lo .. hi Hz */
static double peak_in(const double *P, double lo, double hi, uint32_t sm)
{
    uint32_t k, best = 0;
    double bv = -1;
    for (k = (uint32_t)(lo * NFFT / FS); k <= (uint32_t)(hi * NFFT / FS) && k + sm < NFFT / 2u; k++) {
        double s = 0;
        uint32_t j;
        for (j = k - sm; j <= k + sm; j++)
            s += P[j];
        if (s > bv) { bv = s; best = k; }
    }
    return bin_hz(best);
}
static double band(const double *P, double lo, double hi)
{
    uint32_t k;
    double s = 0;
    for (k = (uint32_t)(lo * NFFT / FS); k < (uint32_t)(hi * NFFT / FS) && k < NFFT / 2u; k++)
        s += P[k];
    return s;
}
static double centroid(const double *P)
{
    uint32_t k;
    double s = 0, w = 0;
    for (k = 1; k < NFFT / 2u; k++) {
        s += P[k] * bin_hz(k);
        w += P[k];
    }
    return w > 0 ? s / w : 0;
}
static double rms(const double *y, uint32_t a, uint32_t b)
{
    double s = 0;
    uint32_t i;
    for (i = a; i < b; i++)
        s += y[i] * y[i];
    return b > a ? sqrt(s / (b - a)) : 0;
}

/* track 0 VOICE, preset pi (LANG as it has it, or lang), the text (TEXT); ATK 0, SUS full, a dry voice */
static void vsetup(int32_t pi, uint32_t lang, const char *text)
{
    memset(trk, 0, sizeof trk);
    memset(sing, 0, sizeof sing);
    formant_nz = 0x2545F491;                             /* (the breath's noise: from its start, a render repeats) */
    host_tracks_init();
    host_preset(&trk[0], eng_voice(), (uint32_t)pi);
    if (lang)
        trk[0].p[P_VLANG] = (int16_t)lang;
    lyr_set(0, text ? text : "");
    trk[0].p[P_VLYR] = 0;
    trk[0].p[P_ATK] = 0;
    trk[0].p[P_SUS] = 127;
    trk[0].p[P_CHOR] = trk[0].p[P_DLY] = trk[0].p[P_REV] = 0;
}
/* n samples of the track's voices into y (0: dropped) */
static void vrender(double *y, uint32_t n)
{
    static int32_t b[CTL];
    uint32_t i, k;
    for (i = 0; i < n; i += CTL) {
        track_render(&trk[0], b, CTL);
        for (k = 0; k < CTL && i + k < n; k++)
            if (y)
                y[i + k] = b[k] / 32768.0;
    }
}

static void check_reading(void)
{
    static const char *const ES[][2] = {
        {"corazon", "|k o |r a |s o n"}, {"guerra", "|g e |rr a"}, {"queso", "|k e |s o"}, {"cielo", "|s y e |l o"},
        {"ciudad", "|s y u |d a d"}, {"playa", "|p l a |y a"}, {"estrella", "|e s |t r e |y a"}, {"honra", "|o n |rr a"},
        {"israel", "|i s |rr a |e l"}, {"muy", "|m w i"}, {"hoy", "|o i"}, {"agua", "|a |g w a"}, {"gente", "|x e n |t e"},
        {"jardin", "|x a r |d i n"}, {"extranyo", "|e k s |t r a |ny o"}, {"perro", "|p e |rr o"}, {"pero", "|p e |r o"},
        {"caer", "|k a |e r"}, {"leon", "|l e |o n"}, {"cuidado", "|k w i |d a |d o"}, {"hielo", "|y e |l o"},
        {"chocolate", "|ch o |k o |l a |t e"}, {"vaca", "|b a |k a"}, {"zapato", "|s a |p a |t o"}, {"llave", "|y a |b e"},
        {"atlas", "|a |t l a s"}, {"obstaculo", "|o b s |t a |k u |l o"}, {"aire", "|a i |r e"}, {"auto", "|a u |t o"},
        {"y", "|i"}, {"rey", "|rr e i"}, {"ninyo", "|n i |ny o"}, {"Quiero", "|k y e |r o"}, {"guitarra", "|g i |t a |rr a"},
        {"pinguino", "|p i n |g i |n o"}, {"xilofono", "|k s i |l o |f o |n o"}, {"mi_amor", "|m y a |m o r"},
        {"alrededor", "|a l |rr e |d e |d o r"}, {"pajaro", "|p a |x a |r o"}, {"cantar", "|k a n |t a r"},
    };
    static const char *const JA[][2] = {
        {"konnichiwa", "|k o |N |n i |ch i |w a"}, {"arigatou", "|a |r i |g a |t o |o"}, {"kitto", "|k i |_ t o"},
        {"shinjiru", "|sh i |N |j i |r u"}, {"tsunami", "|ts u |n a |m i"}, {"kyou", "|k y o |o"}, {"fuji", "|f u |j i"},
        {"wo", "|o"}, {"hito", "|h i |t o"}, {"sayonara", "|s a |y o |n a |r a"}, {"si", "|sh i"}, {"ti", "|ch i"},
        {"tu", "|ts u"}, {"hu", "|f u"}, {"chotto", "|ch o |_ t o"}, {"matcha", "|m a |_ ch a"}, {"kanji", "|k a |N |j i"},
        {"onna", "|o |N |n a"}, {"desu", "|d e |s u"}, {"ryokou", "|r y o |k o |o"}, {"Sakura", "|s a |k u |r a"},
    };
    static const char *const EN[][2] = {
        {"love", "|l ah v"}, {"my", "|m aa ih"}, {"time", "|t aa ih m"}, {"beautiful", "|b y u |t ax |f ax l"},
        {"water", "|w ao |t er"}, {"strong", "|s t r ao ng"}, {"the", "|dh ax"}, {"Dont", "|d o uh n t"},
    };
    char b[256], what[160];
    uint32_t k, ok = 1, l, n;
    for (k = 0; k < NELEM(ES); k++) {
        g2p_str(LG_ES, ES[k][0], b, sizeof b);
        if (strcmp(b, ES[k][1])) { printf("voice:   ES %-12s %s (want %s)\n", ES[k][0], b, ES[k][1]); ok = 0; }
    }
    snprintf(what, sizeof what, "Spanish by rules: %u words, their phonemes and syllables (seseo, ll y, rr, diphthongs, clusters)",
             (unsigned)NELEM(ES));
    check(what, ok);
    for (k = 0, ok = 1; k < NELEM(JA); k++) {
        g2p_str(LG_JA, JA[k][0], b, sizeof b);
        if (strcmp(b, JA[k][1])) { printf("voice:   JA %-12s %s (want %s)\n", JA[k][0], b, JA[k][1]); ok = 0; }
    }
    snprintf(what, sizeof what, "Japanese romaji -> morae: %u words (moraic n, doubled consonants, shi chi tsu fu, kunrei, ou)",
             (unsigned)NELEM(JA));
    check(what, ok);
    for (k = 0, ok = 1; k < NELEM(EN); k++) {
        g2p_str(LG_EN, EN[k][0], b, sizeof b);
        if (strcmp(b, EN[k][1])) { printf("voice:   EN %-12s %s (want %s)\n", EN[k][0], b, EN[k][1]); ok = 0; }
    }
    check("English through the dictionary (CMU, syllabified): love my time beautiful water strong the don't", ok);
    {   /* every dictionary word found, with at least a syllable; an unknown word by the letter rules */
        uint32_t e = 0, nw = 0, ns, len;
        char w[24];
        ok = 1;
        while (e < sizeof LY_EN_DICT && nw < 3000u) {
            uint32_t pre = LY_EN_DICT[e++], wl = pre;
            while (e < sizeof LY_EN_DICT && LY_EN_DICT[e] >= 'a' && LY_EN_DICT[e] <= 'z')
                w[wl++] = (char)LY_EN_DICT[e++];
            while (e < sizeof LY_EN_DICT && LY_EN_DICT[e] >= 0x80u)
                e++;
            w[wl] = 0;
            nw++;
            ok &= sy_en_find(w, wl, &len) != 0 && len > 0;
        }
        ok &= nw == LY_EN_N;
        g2p_str(LG_EN, "zorblat", b, sizeof b);
        ok &= sy_en_find("zorblat", 7, &len) == 0 && !strcmp(b, "|z aa r |b l ae t");
        (void)ns;
        snprintf(what, sizeof what, "English dictionary: all %u words found; a word it lacks by letter rules (zorblat: %s)",
                 (unsigned)LY_EN_N, b);
        check(what, ok);
    }
    {   /* the banks: every word's syllables as gen_lyrics.py counted them (the generator fits phrases by them) */
        static const char *const LN[4] = {"", "ES", "EN", "JA"};
        uint32_t bad_w = 0, tot = 0;
        for (l = LG_ES; l <= LG_JA; l++) {
            const ly_bank_t *bk = &LY_BANK[l - 1u];
            uint32_t nwds = l == LG_ES ? LY_ES_NW : l == LG_EN ? LY_EN_NW : LY_JA_NW;
            for (k = 0; k < nwds; k++) {
                uint8_t ph[SW_MAX];
                char nw2[24];
                const char *w = bk->txt + bk->off[k];
                (void)sy_word(l, nw2, sy_norm(w, (uint32_t)strlen(w), nw2, sizeof nw2), ph, &n);
                tot++;
                if (n != (bk->meta[k] & 15u)) {
                    if (bad_w++ < 8u)
                        printf("voice:   %s %s: %u syllables here, %u in the bank\n", LN[l], w, n, bk->meta[k] & 15u);
                }
            }
        }
        snprintf(what, sizeof what, "the banks: all %u words' syllables as the generator counts them (ES %u EN %u JA %u words)",
                 tot, (unsigned)LY_ES_NW, (unsigned)LY_EN_NW, (unsigned)LY_JA_NW);
        check(what, !bad_w);
    }
}

/* the generator's text for a track: nph phrases of W seed / S seed in lang, theme; into out (words, " / " a phrase) */
static uint32_t gen_text(uint32_t lang, uint32_t lyr, uint32_t theme, uint32_t nph, uint32_t pl, char *out, uint32_t max,
                         uint32_t *syl)
{
    static sing_t g;
    uint32_t m = 0, ph, k, all = 1;
    memset(&g, 0, sizeof g);
    memset(trk, 0, sizeof trk);
    host_tracks_init();
    host_preset(&trk[0], eng_voice(), 7);
    trk[0].p[P_E1] = (int16_t)(theme * 16u + 8u);
    trk[0].p[P_SLEN] = 16;
    for (k = 0; k < 16u; k++) {                          /* pl NOTE steps */
        trk[0].step[k].time = k < pl ? ST_NOTE : ST_REST;
        trk[0].step[k].n = k < pl;
        trk[0].step[k].note[0] = 60;
    }
    out[0] = 0;
    for (ph = 0; ph < nph; ph++) {
        uint32_t s = 0;
        do {
            uint8_t b[SW_MAX];
            char nw[24];
            uint32_t ns, i;
            sy_gen_word(&trk[0], &g, lang, lyr);
            (void)sy_word(lang, nw, sy_norm(g.w, sizeof g.w, nw, sizeof nw), b, &ns);
            s += ns;
            for (i = 0; g.w[i] && m + 2u < max; i++)
                out[m++] = g.w[i];
            if (m + 2u < max)
                out[m++] = ' ';
        } while (g.prem && s < 200u);
        all &= s == pl;
        if (m + 3u < max) { out[m++] = '/'; out[m++] = ' '; }
    }
    out[m] = 0;
    *syl = all;
    return m;
}
static void check_generator(void)
{
    char a[600], b[600], c[600], what[700];
    uint32_t ok, oks, l, s1, s2, s3;
    static const char *const LN[4] = {"", "ES", "EN", "JA"};
    for (l = LG_ES; l <= LG_JA; l++) {
        gen_text(l, 3, 4, 3, 7, a, sizeof a, &s1);
        gen_text(l, 3, 4, 3, 7, b, sizeof b, &s2);
        gen_text(l, 4, 4, 3, 7, c, sizeof c, &s3);
        ok = !strcmp(a, b) && strcmp(a, c) != 0;
        oks = s1 && s2 && s3;
        snprintf(what, sizeof what, "generator %s: W3 the same lyric twice, W4 another; phrases of the pattern's 7 notes", LN[l]);
        check(what, ok && oks);
        printf("voice:   %s W3 ANY: %.160s\n", LN[l], a);
    }
    gen_text(LG_ES, 1, 0, 2, 12, a, sizeof a, &s1);
    gen_text(LG_ES, 1, 7, 2, 12, b, sizeof b, &s2);
    check("generator: another THEME, other words (LOVE against FIRE); 12-note phrases fit", strcmp(a, b) != 0 && s1 && s2);
    printf("voice:   ES W1 LOVE: %.160s\n", a);
    printf("voice:   ES W1 FIRE: %.160s\n", b);
    gen_text(LG_JA, LYR_NW + 2u, 4, 2, 9, a, sizeof a, &s1);
    gen_text(LG_EN, LYR_NW + 1u, 4, 1, 9, b, sizeof b, &s2);
    check("scat S2 JA / S1 EN: syllables of the language, phrases fit", s1 && s2 && a[0] && b[0]);
    printf("voice:   JA S2: %.120s\n", a);
    printf("voice:   EN S1: %.120s\n", b);
    {   /* every word of W a real one: in the bank (or the vocable) */
        uint32_t k, w, bad_w = 0;
        for (l = LG_ES; l <= LG_JA; l++)
            for (k = 1; k <= 4u; k++) {
                char *p, *q;
                gen_text(l, k, 4, 4, 11, a, sizeof a, &s1);
                for (p = a; *p; p = q) {
                    const ly_bank_t *bk = &LY_BANK[l - 1u];
                    uint32_t nwds = l == LG_ES ? LY_ES_NW : l == LG_EN ? LY_EN_NW : LY_JA_NW, found = 0, n;
                    while (*p == ' ' || *p == '/') p++;
                    for (q = p; *q && *q != ' '; q++)
                        ;
                    n = (uint32_t)(q - p);
                    if (!n) break;
                    for (w = 0; w < nwds && !found; w++) {
                        const char *t = bk->txt + bk->off[w];
                        found = strlen(t) == n && (!strncmp(t, p, n) ||
                                                   (l == LG_ES && !strncmp(t, p, n - 1u) && t[n - 1u] == 'o' && p[n - 1u] == 'a'));
                    }
                    found |= (n == 2u && (!strncmp(p, "la", 2) || !strncmp(p, "oh", 2) || !strncmp(p, "ra", 2)));
                    bad_w += !found;
                }
            }
        check("generator: every word of W1..W4 in ES EN JA a bank word (the adjective's -a after a feminine noun)", !bad_w);
    }
}

/* the steady part of a held syllable (text) at note 60 (formants as the table: SHIFT 0, key tracking at C4) */
static void sustain(int32_t pi, uint32_t lang, const char *text, int16_t breath, double *y, uint32_t n)
{
    vsetup(pi, lang, text);
    trk[0].p[P_E3] = 0;
    if (breath >= 0)
        trk[0].p[P_E5] = breath;
    trk_note_on(&trk[0], 60, 100);
    vrender(0, FS / 4);                                  /* past the onset */
    vrender(y, n);
}
static void check_sound(void)
{
    static double y[FS * 2], P[NFFT / 2];
    static const struct { const char *t; double f1lo, f1hi, f2lo, f2hi; } V[] = {
        {"a", 640, 880, 1120, 1480}, {"e", 400, 570, 1650, 2080}, {"i", 240, 380, 2000, 2500}, {"o", 420, 590, 780, 1040},
        {"u", 270, 420, 660, 960},
    };
    char what[200];
    uint32_t k, ok = 1, n = FS;
    int32_t pi = preset_of("CANTOR ES");
    for (k = 0; k < NELEM(V); k++) {                     /* whispered: the noise draws the cascade's response */
        double f1, f2;
        sustain(pi, LG_ES, V[k].t, 127, y, n);
        spectrum(y, n, P);
        f1 = peak_in(P, 200, 950, 4);
        f2 = peak_in(P, f1 + 250, 2800, 4);
        printf("voice:   vowel %s: F1 %4.0f F2 %4.0f Hz\n", V[k].t, f1, f2);
        ok &= f1 >= V[k].f1lo && f1 <= V[k].f1hi && f2 >= V[k].f2lo && f2 <= V[k].f2hi;
    }
    check("vowels a e i o u: F1 / F2 of the sound in their ranges (Spanish, adult male, Quilis / Hillenbrand)", ok);
    {   /* sung (voiced) a: its spectral envelope's F1 near the table too */
        double f1;
        sustain(pi, LG_ES, "a", 0, y, n);
        spectrum(y, n, P);
        f1 = peak_in(P, 400, 1000, 1);
        ok = f1 > 600 && f1 < 900;
        snprintf(what, sizeof what, "voiced a at C4: the strongest harmonic near F1 (%.0f Hz)", f1);
        check(what, ok);
    }
    {   /* fricatives: s high, sh lower, f weak and broad */
        double cs, ps, csh, psh, rs, rsh, rf, rv;
        vsetup(pi, LG_ES, "sa");
        trk_note_on(&trk[0], 60, 100);
        vrender(y, FS / 2);
        spectrum(y + (uint32_t)(0.015 * FS), (uint32_t)(0.05 * FS), P);
        cs = centroid(P);
        ps = peak_in(P, 1500, 9000, 6);
        rs = rms(y, (uint32_t)(0.015 * FS), (uint32_t)(0.065 * FS));
        rv = rms(y, (uint32_t)(0.2 * FS), (uint32_t)(0.4 * FS));
        vsetup(preset_of("UTA JA"), LG_JA, "sha");
        trk[0].p[P_E3] = 0;
        trk_note_on(&trk[0], 60, 100);
        vrender(y, FS / 2);
        spectrum(y + (uint32_t)(0.015 * FS), (uint32_t)(0.06 * FS), P);
        csh = centroid(P);
        psh = peak_in(P, 1500, 9000, 6);
        rsh = rms(y, (uint32_t)(0.015 * FS), (uint32_t)(0.07 * FS));
        vsetup(pi, LG_ES, "fa");
        trk_note_on(&trk[0], 60, 100);
        vrender(y, FS / 2);
        rf = rms(y, (uint32_t)(0.015 * FS), (uint32_t)(0.065 * FS));
        printf("voice:   s: centroid %.0f Hz, peak %.0f Hz, %.1f dB of the vowel; sh: centroid %.0f, peak %.0f, %.1f dB; f %.1f dB\n",
               cs, ps, 20 * log10(rs / rv), csh, psh, 20 * log10(rsh / rv), 20 * log10(rf / rv));
        check("fricatives: s noise 4.5 .. 8 kHz, sh 2.2 .. 3.6 kHz, both 8 .. 24 dB under the vowel, f weaker than s",
              ps > 4500 && ps < 8000 && cs > 4000 && psh > 2200 && psh < 3600 && csh < cs && rs < rv / 2.5 && rs > rv / 16 &&
              rsh < rv / 2.5 && rsh > rv / 16 && rf < rs);
    }
    {   /* plosives: a closure (silence), a burst, the vowel */
        uint32_t i, cl = 0, bu = 0, frame = FS / 1000u;
        double rv, rmax = 0;
        vsetup(pi, LG_ES, "ta");
        trk_note_on(&trk[0], 60, 100);
        vrender(y, FS / 2);
        rv = rms(y, (uint32_t)(0.2 * FS), (uint32_t)(0.4 * FS));
        for (i = 0; i + frame < (uint32_t)(0.12 * FS); i += frame) {
            double r = rms(y, i, i + frame);
            if (!bu && r < rv / 20)
                cl++;                                    /* the closure: ms near silence */
            if (cl >= 25u && !bu && r > rv / 6)
                bu = i / frame;                          /* the burst after it */
            if (r > rmax) rmax = r;
        }
        snprintf(what, sizeof what, "plosive t: a closure (%u ms under -26 dB), then the burst at %u ms, then the vowel", cl, bu);
        check(what, cl >= 25u && cl <= 70u && bu >= 25u && bu <= 80u);
    }
    {   /* no click: legato syllables (vowels, nasals, liquids), the sample-to-sample jump never far past the steady one */
        static const char *const T[] = {"a e i o u a", "mama nana lala", "ya wa ya wa"};
        uint32_t t, i, worst_ok = 1;
        for (t = 0; t < NELEM(T); t++) {
            double dmax = 0, dsteady = 0;
            vsetup(preset_of("CANTOR ES"), LG_ES, T[t]);
            trk[0].p[P_VOICE] = V_LEGATO;
            for (k = 0; k < 6u; k++) {
                static double z[FS / 4];
                trk_note_on(&trk[0], 60 + (k & 1u) * 2u, 100);
                if (k) trk_note_off(&trk[0], 60 + ((k - 1u) & 1u) * 2u);
                vrender(z, FS / 4);
                for (i = 2; i < FS / 4; i++) {
                    double d2 = fabs(z[i] - 2 * z[i - 1] + z[i - 2]);
                    if (i < FS / 8) { if (d2 > dmax) dmax = d2; }
                    else if (d2 > dsteady) dsteady = d2;
                }
            }
            printf("voice:   \"%s\": the largest second difference across the changes %.4f, steady %.4f\n", T[t], dmax, dsteady);
            worst_ok &= dmax < dsteady * 2.0;
        }
        check("no click at syllable changes (legato vowels, nasals, liquids, glides): the jumps as the steady sound's", worst_ok);
    }
}

/* every word preset: sounds, no DC, ends, the same twice; at the corners no clipping */
static void check_presets(void)
{
    static double y[FS * 3];
    uint32_t k, ok = 1, okdc = 1, oke = 1, okr = 1, okc = 1;
    double worst = 0, peak = 0;
    for (k = getenv("VDC") ? 0u : 7u; k < ENG_FORMANT.npresets; k++) {   /* (VDC: the vowel presets too, for comparison) */
        double m = 0, r;
        uint32_t i, j, live = 0;
        vsetup((int32_t)k, 0, "la sol mar canta");
        trk[0].p[P_VLYR] = ENG_FORMANT.presets[k].xp >> 2;
        for (j = 0; j < 4u; j++) {
            trk_note_on(&trk[0], 60 + j * 2u, 100);
            vrender(y + j * (FS / 2), FS / 2);
            trk_note_off(&trk[0], 60 + j * 2u);
        }
        vrender(y + 2 * FS, FS);
        for (i = 0; i < 2u * FS; i++) m += y[i];
        m /= 2.0 * FS;
        r = rms(y, 0, 2u * FS);
        ok &= r > 0.01;
        if (fabs(m) / VOICE_FS * 32768.0 > worst) worst = fabs(m) / VOICE_FS * 32768.0;
        if (getenv("VDC"))
            printf("voice:   DC %s %.3f %%\n", ENG_FORMANT.presets[k].name, fabs(m) / VOICE_FS * 32768.0 * 100.0);
        okdc &= fabs(m) < 0.015 * VOICE_FS / 32768.0;
        for (i = 0; i < NVOICE; i++) live += trk[0].v[i].active;
        oke &= !live;
        {   /* the same twice */
            static double z[FS];
            vsetup((int32_t)k, 0, "la sol mar canta");
            trk[0].p[P_VLYR] = ENG_FORMANT.presets[k].xp >> 2;
            trk_note_on(&trk[0], 60, 100);
            vrender(z, FS / 2);
            okr &= !memcmp(z, y, (FS / 2) * sizeof z[0]);
        }
    }
    check("every word preset sounds", ok);
    {
        char what[160];
        snprintf(what, sizeof what, "no DC: worst |mean| %.2f %% of a voice's full scale (the vowel presets: up to 3.2 %%, CHANT)",
                 worst * 100.0);
        check(what, okdc);
    }
    check("every word preset's voices end after its release", oke);
    check("a phrase from silence renders the same samples again (every word preset)", okr);
    {   /* the corners: 4 voices, velocity 127, LEVEL 127, BUZZ / Q / BREATH at their ends, fricatives and plosives */
        static int32_t o[2 * CTL];
        static const int16_t C[4][3] = {{127, 127, 0}, {0, 127, 127}, {127, 0, 0}, {127, 127, 127}};
        uint32_t c, i, j;
        for (c = 0; c < 4u; c++) {
            vsetup(preset_of("WORD CHOIR"), LG_ES, "sha cha ta pa ka ssss");
            trk[0].p[P_E4] = C[c][0]; trk[0].p[P_E6] = C[c][1]; trk[0].p[P_E5] = C[c][2];
            trk[0].p[P_LEVEL] = 127;
            trk[0].p[P_VOICE] = V_POLY;
            for (j = 0; j < 4u; j++)
                trk_note_on(&trk[0], 48 + j * 7u, 127);
            for (i = 0; i < FS * 2u; i += CTL) {
                mix_block(o, CTL);
                for (j = 0; j < 2u * CTL; j++)
                    if (abs(o[j]) > peak) peak = abs(o[j]);
                if (i % (FS / 3u) < CTL)
                    for (j = 0; j < 4u; j++) { trk_note_off(&trk[0], 48 + j * 7u); trk_note_on(&trk[0], 48 + j * 7u, 127); }
            }
            okc &= peak < 32767;
        }
        {
            char what[160];
            snprintf(what, sizeof what, "no clipping at the corners (4 voices, velocity 127, LEVEL 127): mix peak %.0f", peak);
            check(what, okc);
        }
    }
}

/* the lyric as notes play it: a note the next syllable, a TIE (no note) holds it, "-" holds the vowel, POS, empty TEXT */
static void check_lyric(void)
{
    static const char *const W[4] = {"la", "le", "li", "lo"};
    uint32_t k, ok = 1;
    vsetup(preset_of("CANTOR ES"), LG_ES, "la le, li lo! ma");
    for (k = 0; k < 4u; k++) {
        const sing_t *s = sing_view(&trk[0]);
        trk_note_on(&trk[0], 60, 100);
        vrender(0, FS / 5);
        ok &= s->nsy == k + 1u && !strcmp(s->w, W[k]) && s->ph[s->nu] == (k == 0 ? PH_A : k == 1 ? PH_E : k == 2 ? PH_I : PH_O);
        trk_note_off(&trk[0], 60);
        vrender(0, FS / 10);
    }
    check("each note sings the next syllable (la le li lo; the punctuation skipped)", ok);
    {   /* a held note (a TIE) holds the syllable */
        const sing_t *s = sing_view(&trk[0]);
        trk_note_on(&trk[0], 62, 100);
        vrender(0, FS);
        ok = s->nsy == 5u && !strcmp(s->w, "ma") && s->i == s->nu;
        trk_note_off(&trk[0], 62);
        vrender(0, FS / 10);
        trk_note_on(&trk[0], 62, 100);                  /* the text wraps round */
        vrender(0, FS / 10);
        ok &= !strcmp(s->w, "la") && s->nsy == 6u;
        check("a held note holds its vowel (no syllable moves on); the text starts over at its end", ok);
    }
    {   /* "-": the vowel again, no consonant */
        const sing_t *s;
        vsetup(preset_of("CANTOR ES"), LG_ES, "sol - mar");
        s = sing_view(&trk[0]);
        trk_note_on(&trk[0], 60, 100); vrender(0, FS / 5); trk_note_off(&trk[0], 60); vrender(0, FS / 50);
        trk_note_on(&trk[0], 62, 100); vrender(0, FS / 50);
        ok = s->nsy == 2u && s->ph[s->nu] == PH_O && s->i == s->nu;
        trk_note_off(&trk[0], 62); vrender(0, FS / 5);
        ok &= s->i > s->nu;                              /* the release: sol's l at last */
        trk_note_on(&trk[0], 64, 100); vrender(0, FS / 5);
        ok &= !strcmp(s->w, "mar") && s->nsy == 3u;
        check("\"-\" holds the vowel one more note (sol - mar: o, o, then the l at the release, then mar)", ok);
    }
    {   /* POS: a syllable of the lyric, as it changes */
        const sing_t *s;
        vsetup(preset_of("CANTOR ES"), LG_ES, "uno dos tres cuatro");
        s = sing_view(&trk[0]);
        trk[0].p[P_E0] = 4;                              /* the 4th syllable: tres */
        trk_note_on(&trk[0], 60, 100); vrender(0, FS / 10); trk_note_off(&trk[0], 60); vrender(0, FS / 20);
        ok = !strcmp(s->w, "tres");
        trk_note_on(&trk[0], 60, 100); vrender(0, FS / 10); trk_note_off(&trk[0], 60); vrender(0, FS / 20);
        ok &= !strcmp(s->w, "cuatro") && s->wsy == 1u;   /* (POS held: on from there) */
        trk[0].p[P_E0] = 0;
        trk_note_on(&trk[0], 60, 100); vrender(0, FS / 10); trk_note_off(&trk[0], 60); vrender(0, FS / 20);
        ok &= !strcmp(s->w, "cuatro") && s->wsy == 2u;
        trk[0].p[P_E0] = 1;
        trk_note_on(&trk[0], 60, 100); vrender(0, FS / 10);
        ok &= !strcmp(s->w, "uno") && s->wsy == 1u;
        check("POS: a note sings that syllable of the lyric (as POS changes: a lock, automation), then on from it", ok);
    }
    {   /* an empty TEXT sings the dice's words; LANG OFF after words: the vowels as fresh */
        static double y1[FS / 2], y2[FS / 2];
        const sing_t *s;
        vsetup(preset_of("CANTOR ES"), LG_ES, "");
        s = sing_view(&trk[0]);
        trk_note_on(&trk[0], 60, 100); vrender(0, FS / 10);
        ok = s->w[0] && s->nsy == 1u;
        check("an empty TEXT sings W1 (the dice) rather than nothing", ok);
        vsetup(preset_of("VOX LEAD"), 0, "");
        trk_note_on(&trk[0], 60, 100);
        vrender(y1, FS / 2);
        vsetup(preset_of("CANTOR ES"), LG_ES, "sha la");
        trk_note_on(&trk[0], 60, 100); vrender(0, FS / 3); trk_note_off(&trk[0], 60); vrender(0, FS);
        trk[0].p[P_VLANG] = 0;
        host_preset(&trk[0], eng_voice(), (uint32_t)preset_of("VOX LEAD"));
        trk[0].p[P_ATK] = 0; trk[0].p[P_SUS] = 127; trk[0].p[P_CHOR] = trk[0].p[P_DLY] = trk[0].p[P_REV] = 0;
        trk[0].v[0].active = 0; trk[0].v[0].env = trk[0].v[0].env_out = 0; trk[0].v[0].stage = 0;
        formant_nz = 0x2545F491;
        trk_note_on(&trk[0], 60, 100);
        vrender(y2, FS / 2);
        check("LANG OFF after singing words: the vowels bit for bit as from a fresh track", !memcmp(y1, y2, sizeof y1));
    }
    {   /* LOOP: with the sequencer, the lyric from its start every pattern loop (LOOP 1), on through it (FREE) */
        uint32_t loops, okl = 1, okf = 1, i;
        static int32_t o[2 * CTL];
        for (loops = 0; loops < 2u; loops++) {
            const sing_t *s;
            uint16_t first[3];
            vsetup(preset_of("CANTOR ES"), LG_ES, "la le li lo lu ma me mi mo mu");
            s = sing_view(&trk[0]);
            trk[0].p[P_E7] = loops ? 0 : 30;              /* LOOP 1 (30: the second name), FREE */
            trk[0].p[P_SLEN] = 4;
            trk[0].p[P_SDIV] = 2;
            for (k = 0; k < 4u; k++) {
                trk[0].step[k].time = ST_NOTE; trk[0].step[k].n = 1; trk[0].step[k].note[0] = 60; trk[0].step[k].vel = 100;
            }
            song.g[G_BPM] = 120;                          /* (1/16 at 120: a step 125 ms, the loop 0.5 s) */
            seq_start();
            for (i = 0; i < FS / 16u; i += CTL)           /* into the first step */
                mix_block(o, CTL);
            for (k = 0; k < 3u; k++) {                    /* three loops: the syllables at each loop's first step */
                first[k] = s->nsy;
                for (i = 0; i < FS / 2u; i += CTL)
                    mix_block(o, CTL);
            }
            seq_stop();
            printf("voice:   LOOP %s: syllables at the loops' first steps %u %u %u\n", loops ? "FREE" : "1", first[0], first[1],
                   first[2]);
            if (!loops) okl = first[0] == 1u && first[1] == 1u && first[2] == 1u;
            else okf = first[0] == 1u && first[1] == 5u && first[2] == 9u;
        }
        check("LOOP 1: the lyric from its start every loop of the pattern; FREE: on through it", okl && okf);
    }
}

static void check_cost(void)
{
    static double y[FS];
    uint64_t t0, t1, t2, t3;
    uint32_t j;
    vsetup(preset_of("VOX LEAD"), 0, "");
    trk[0].p[P_VOICE] = V_POLY;
    for (j = 0; j < 4u; j++) trk_note_on(&trk[0], 60 + j * 3u, 100);
    t0 = now_ns();
    vrender(y, FS);
    t1 = now_ns();
    vsetup(preset_of("CANTOR ES"), LG_ES, "la noche es azul y el mar canta para ti");
    trk[0].p[P_VOICE] = V_POLY;
    for (j = 0; j < 4u; j++) trk_note_on(&trk[0], 60 + j * 3u, 100);
    t2 = now_ns();
    vrender(y, FS);
    t3 = now_ns();
    printf("voice: cost, host ns per sample and voice (4 voices; information): vowels %.1f, words %.1f\n",
           (double)(t1 - t0) / FS / 4.0, (double)(t3 - t2) / FS / 4.0);
}

/* --------------------------------------------------------------------------------------------------- demos --- */
static void demos(const char *dir)
{
    /* Spanish: 14 syllables (la-no-che-es-a-zul-y-el-mar-can-ta-pa-ra-ti) on a G major line */
    static const nt_t ES[] = {{67, 2}, {69, 2}, {71, 4}, {69, 2}, {67, 2}, {64, 4}, {62, 2}, {64, 2}, {67, 6}, {0, 2},
                              {67, 2}, {69, 2}, {71, 2}, {72, 2}, {71, 4}, {67, 8}};
    /* English: "you are my sunshine, my only sunshine" (you-are-my-sun-shine-my-on-ly-sun-shine) */
    static const nt_t EN[] = {{55, 2}, {60, 2}, {62, 2}, {64, 6}, {64, 4}, {0, 2}, {64, 2}, {62, 2}, {64, 2}, {60, 6},
                              {60, 6}};
    /* Japanese: "sakura sakura yayoi no sora wa" (sa-ku-ra sa-ku-ra ya-yo-i no so-ra wa) */
    static const nt_t JA[] = {{69, 3}, {69, 3}, {71, 6}, {69, 3}, {69, 3}, {71, 6}, {69, 3}, {71, 3}, {72, 3}, {71, 3},
                              {69, 3}, {71, 2}, {69, 2}, {65, 6}};
    /* the dice: 16 notes */
    static const nt_t RN[] = {{62, 2}, {64, 2}, {65, 2}, {67, 2}, {69, 3}, {67, 1}, {65, 2}, {64, 4}, {0, 2}, {62, 2},
                              {64, 2}, {65, 2}, {69, 2}, {67, 3}, {65, 1}, {64, 2}, {62, 6}};
    uint32_t k;
    char nm[64];
    demo_wav(dir, "es_phrase", preset_of("CANTOR ES"), "la noche es azul y el mar canta para ti", ES, NELEM(ES));
    demo_wav(dir, "es_phrase_cantora", preset_of("CANTORA ES"), "la noche es azul y el mar canta para ti", ES, NELEM(ES));
    demo_wav(dir, "en_phrase", preset_of("SINGER EN"), "you are my sunshine my only sunshine", EN, NELEM(EN));
    demo_wav(dir, "ja_phrase", preset_of("UTA JA"), "sakura sakura yayoi no sora wa", JA, NELEM(JA));
    demo_wav(dir, "rnd_es_W1", preset_of("CANTOR ES"), "", RN, NELEM(RN));
    demo_wav(dir, "rnd_en_W3", preset_of("SINGER EN"), "", RN, NELEM(RN));
    demo_wav(dir, "rnd_ja_W4", preset_of("UTA JA"), "", RN, NELEM(RN));
    demo_wav(dir, "rnd_en_scat_S1", preset_of("SCAT EN"), "", RN, NELEM(RN));
    for (k = 7; k < ENG_FORMANT.npresets; k++) {         /* every word preset, its own dice */
        uint32_t i;
        snprintf(nm, sizeof nm, "preset_%02u_", k);
        for (i = 0; ENG_FORMANT.presets[k].name[i] && i < 20u; i++)
            nm[10 + i] = ENG_FORMANT.presets[k].name[i] == ' ' ? '_' : ENG_FORMANT.presets[k].name[i];
        nm[10 + i] = 0;
        demo_wav(dir, nm, (int32_t)k, "", RN, NELEM(RN));
    }
    printf("voice: demos in %s\n", dir);
}

int main(int argc, char **argv)
{
    if (argc > 2 && !strcmp(argv[1], "--g2p")) {
        uint32_t lang = !strcmp(argv[2], "JA") ? LG_JA : !strcmp(argv[2], "EN") ? LG_EN : LG_ES;
        int a;
        char b[256];
        for (a = 3; a < argc; a++) {
            g2p_str(lang, argv[a], b, sizeof b);
            printf("%-16s %s\n", argv[a], b);
        }
        return 0;
    }
    check_reading();
    check_generator();
    check_sound();
    check_presets();
    check_lyric();
    check_cost();
    if (argc > 1)
        demos(argv[1]);
    printf("voice: %s\n", bad ? "FAILED" : "all checks ok");
    return bad != 0;
}
