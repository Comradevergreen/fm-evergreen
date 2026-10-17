/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* (0.8.5) LENTE across the system, as the approved HTML mock-up (lente_sistema.html): every domain a lens of three
 * levels SELECT zooms through (no wrap), a level's sets (its pages) a button's tap goes round, the level as three tabs
 * on top, "LENTE . LEVEL . SET  k/n" at the foot. SEQ, SAVE and FX keep their own (ui.c lens_*, save_*, fx_*); here
 * the rest. Included by ui.c.
 *   HOME   ECO (the six beings: HOME / MACROS) . BEING (the track's being, its knobs as organs: HOME / ROUTES) . GEN
 *          (the generator: GEN 1/3 .. 3/3, the CF_DICE_ALL screen as before)
 *   EDIT   SOUND (EDIT 1 / EDIT 2; DRUM: KIT KIT 2 KIT 3) . TONE (FILTER, MORPH / RETRIG / ROTOR, VOICE, VOICE 2, WORDS; DRUM:
 *          X-MOD FILTER STEREO LEVELS LEVELS 2 VOICE) . the detail: FM6 OPERATOR (OPERATOR OP ENV OPERATOR 2), DRUM SOUND
 *          (SOUND 1..4), the others FOCUS (an EDIT parameter large with the MOD slot that moves it)
 *   ENV    TRACKS (the six envelopes: ENV / DEST) . ENV (ENV / ENV DEST) . STAGE (one stage, its time)
 *   LFO    MOD (the matrix as a net) . LFO (LFO / LFO DEST) . MSEQ (MSEQ / MSEQ 2)
 *   SCL    KEY (SCL) . CHORD . ARP (ARP / ARP 2); SCL taps KEY <-> CHORD, ARP goes to ARP and taps ARP <-> ARP 2
 *   GLO    MIX (MIXER / SENDS) . GLOBAL . SYSTEM; a GLO tap the next level round, as before
 * The pages, their knobs and every parameter are PAGES' (new: STAGE, FOCUS, VOICE 2, SENDS, appended). A page put by
 * code brings its level (pd_level); each domain remembers its level and each level's set (ui.pdlv, ui.pdset). */
enum { LZ_SEQ, LZ_SAVE, LZ_FX, LZ_HOME, LZ_EDIT, LZ_ENV, LZ_MOD, LZ_KEY, LZ_GLO, LZ_MENU, LZ_LAYER, LZ_DLG, LZ_NONE };
enum { PD_EDIT, PD_ENV, PD_MOD, PD_KEY, PD_GLO, PD_N };
#define PD_SETS 8u
static const uint8_t PD_DEF_LV[PD_N] = {0, 1, 1, 0, 0};   /* (the mock-up: ENV and LFO open on their middle level) */
static const char *const PD_LV[PD_N][3] = {{"SOUND", "TONE", "FOCUS"}, {"TRACKS", "ENV", "STAGE"}, {"MOD", "LFO", "MSEQ"},
                                           {"KEY", "CHORD", "ARP"}, {"MIX", "GLOBAL", "SYSTEM"}};
/* the fixed domains' sets: name, page title (0: the level's end) */
static const char *const PD_TAB[PD_N - 1][3][5] = {
    {{"ENV", "ENV", "DEST", "ENV DEST", 0}, {"ENV", "ENV", "ENV DEST", "ENV DEST", 0}, {"STAGE", "STAGE", 0}},
    {{"MOD", "MOD", 0}, {"LFO", "LFO", "LFO DEST", "LFO DEST", 0}, {"MSEQ", "MSEQ", "MSEQ 2", "MSEQ 2", 0}},
    {{"SCL", "SCL", 0}, {"CHORD", "CHORD", 0}, {"ARP", "ARP", "ARP 2", "ARP 2", 0}},
    {{"MIXER", "MIXER", "SENDS", "SENDS", 0}, {"GLOBAL", "GLOBAL", 0}, {"SYSTEM", "SYSTEM", 0}}};
static uint32_t pd_of_fam(uint32_t f)
{
    return f == FAM_EDIT ? PD_EDIT : f == FAM_ENV ? PD_ENV : f == FAM_LFO ? PD_MOD : f == FAM_SCL || f == FAM_ARP ? PD_KEY :
           f == FAM_GLO || f == FAM_TRK ? PD_GLO : PD_N;
}
static uint32_t pd_find(uint32_t pd, const char *title)  /* the visible page of the domain so titled (NPAGES: none) */
{
    uint32_t i;
    for (i = 0; i < NPAGES; i++)
        if (pd_of_fam(PAGES[i].fam) == pd && str_eq(PAGES[i].title, title) && page_visible(i))
            return i;
    return NPAGES;
}
static uint32_t edit_kind(void)                          /* 0 a synth, 1 FM6, 2 DRUM, 3 DIGITAL (FELUCCA_FM4) */
{
    uint32_t e = TSEL->eng_req % NENGINES;
    return e == ENGI_FM6 ? 1u : e == ENGI_DRUM ? 2u : FELUCCA_FM4 && e == ENGI_DIGITAL ? 3u : 0u;
}
/* level lv's sets in domain pd: their pages (pg) and names (nm, may be 0); how many (only the pages shown now) */
static uint32_t pd_sets(uint32_t pd, uint32_t lv, uint8_t *pg, const char **nm)
{
    static const char *const ED[4][3][8] = {   /* (0.8.5: VOICE's WORDS in TONE, after VOICE 2) */
        {{"EDIT 1", "EDIT 2", 0}, {"FILTER", "MORPH", "RETRIG", "ROTOR", "VOICE", "VOICE 2", "WORDS", 0}, {"FOCUS", 0}},
        {{"EDIT 1", "EDIT 2", 0}, {"FILTER", "VOICE", "VOICE 2", 0}, {"OPERATOR", "OP ENV", "OPERATOR 2", 0}},
        {{"EDIT 1", "EDIT 2", "KIT 3", 0}, {"X-MOD", "FILTER", "STEREO", "LEVELS", "LEVELS 2", "VOICE", 0},
         {"SOUND", "SOUND 2", "SOUND 3", "SOUND 4", 0}},
        {{"EDIT 1", "EDIT 2", 0}, {"FILTER", "VOICE", "VOICE 2", 0}, {"OP1 ENV", "OP2 ENV", "OP3 ENV", "OP4 ENV", 0}}};
    uint32_t n = 0, k, i;
    lv %= 3u;
    if (pd == PD_EDIT) {
        const char *const *t = ED[edit_kind()][lv];
        for (k = 0; t[k] && n < PD_SETS; k++)
            if ((i = pd_find(pd, t[k])) < NPAGES) {
                pg[n] = (uint8_t)i;
                if (nm) nm[n] = t[k];
                n++;
            }
        if (edit_kind() == 3u && lv == 2u && (i = pd_find(pd, "OP LEVEL")) < NPAGES && n < PD_SETS) {   /* (DIGITAL: + OP LEVEL) */
            pg[n] = (uint8_t)i;
            if (nm) nm[n] = "OP LEVEL";
            n++;
        }
        return n;
    }
    if (pd >= PD_N)
        return 0;
    for (k = 0; PD_TAB[pd - 1u][lv][k] && n < PD_SETS; k += 2u)
        if ((i = pd_find(pd, PD_TAB[pd - 1u][lv][k + 1u])) < NPAGES) {
            pg[n] = (uint8_t)i;
            if (nm) nm[n] = PD_TAB[pd - 1u][lv][k];
            n++;
        }
    return n;
}
static int32_t pd_k_in(uint32_t pd, uint32_t lv, uint32_t page)   /* the page's place among level lv's sets, -1 none */
{
    uint8_t pg[PD_SETS];
    uint32_t n = pd_sets(pd, lv, pg, 0), k;
    for (k = 0; k < n; k++)
        if (pg[k] == page)
            return (int32_t)k;
    return -1;
}
#define PD_LVGET(pd) ((uint32_t)(ui.pdlv[pd] ^ PD_DEF_LV[pd]) % 3u)   /* (0, power-on: the default) */
#define PD_LIVE (1u << PD_EDIT | 1u << PD_ENV | 1u << PD_MOD | 1u << PD_KEY | 1u << PD_GLO)                                     /* the domains drawn as lenses (bit PD_*) */
static int pd_on(void) { return !ui.home && !page_over && ((PD_LIVE >> pd_of_fam(PAGES[ui.page].fam)) & 1u); }
static uint32_t pd_cur(void) { return pd_of_fam(PAGES[ui.page].fam); }
/* the level shown: the one remembered when the page is in it (ENV is in TRACKS and in ENV), else the page's own */
static uint32_t pd_level(void)
{
    uint32_t pd = pd_cur(), l = PD_LVGET(pd), k;
    if (pd >= PD_N || pd_k_in(pd, l, ui.page) >= 0)
        return l;
    for (k = 0; k < 3u; k++) {
        uint32_t m = (PD_DEF_LV[pd] + k) % 3u;
        if (pd_k_in(pd, m, ui.page) >= 0) {
            ui.pdlv[pd] = (uint8_t)(m ^ PD_DEF_LV[pd]);
            return m;
        }
    }
    return l;
}
static const char *pd_level_name(uint32_t pd, uint32_t l)
{
    static const char *const DETAIL[4] = {"FOCUS", "OPERATOR", "SOUND", "OPS"};
    return pd == PD_EDIT && l == 2u ? DETAIL[edit_kind()] : PD_LV[pd % PD_N][l % 3u];
}
static void pd_show(uint32_t pd, uint32_t lv, uint32_t k)   /* level lv's set k (round) shown, remembered */
{
    uint8_t pg[PD_SETS];
    uint32_t n = pd_sets(pd, lv, pg, 0);
    if (!n)
        return;
    k %= n;
    ui.pdlv[pd] = (uint8_t)(lv ^ PD_DEF_LV[pd]);
    ui.pdset[pd][lv % 3u] = (uint8_t)k;
    ui.page = pg[k];
    ui.fam_last[PAGES[ui.page].fam] = ui.page;
    ui.home = 0;
    page_entered();
    page_flash();
}
static void pd_zoom(int32_t s)                          /* SELECT: a level a detent, at the set it was left at */
{
    uint32_t pd = pd_cur(), lv = pd_level();
    int32_t to = clamp((int32_t)lv + s, 0, 2);
    if ((uint32_t)to != lv)
        pd_show(pd, (uint32_t)to, ui.pdset[pd][to]);
}
static void pd_cycle(void)                              /* a tap on its page: the level's next set */
{
    uint32_t pd = pd_cur(), lv = pd_level();
    int32_t k = pd_k_in(pd, lv, ui.page);
    pd_show(pd, lv, (uint32_t)(k + 1));
}
/* a domain's button (open_family): on its pages the level's next set (SCL: KEY <-> CHORD; ARP: the ARP level, then its
 * sets; GLO: the next set, then the next level round); from elsewhere the level and set it was left at (GLO: MIX; SCL: KEY or CHORD; ARP:
 * ARP). 1: done */
static int pd_button(uint32_t fam)
{
    uint32_t pd = pd_of_fam(fam), here = pd_on() && pd_cur() == pd, lv;
    if (pd >= PD_N)
        return 0;
    lv = here ? pd_level() : PD_LVGET(pd);
    if (!here && fam != FAM_ARP && fam != FAM_GLO && fam != FAM_TRK) {   /* (from elsewhere: its last page, where it is) */
        uint32_t p = ui.fam_last[fam], m;
        int32_t k;
        for (m = 0; p < NPAGES && m < 3u; m++) {      /* (from the level it was left at: ENV is in TRACKS and ENV) */
            uint32_t l = (lv + m) % 3u;
            if ((fam != FAM_SCL || l < 2u) && (k = pd_k_in(pd, l, p)) >= 0) {
                pd_show(pd, l, (uint32_t)k);
                return 1;
            }
        }
    }
    if (fam == FAM_SCL)
        lv = here ? (lv == 0u ? 1u : 0u) : lv == 2u ? 0u : lv;
    else if (fam == FAM_ARP) {
        if (here && lv == 2u) {
            pd_cycle();
            return 1;
        }
        lv = 2u;
    } else if (fam == FAM_GLO || fam == FAM_TRK) {   /* (GLO: MIXER -> SENDS -> GLOBAL -> SYSTEM -> MIXER) */
        uint8_t pg[PD_SETS];
        int32_t k = pd_k_in(pd, lv, ui.page);
        if (here && k >= 0 && (uint32_t)k + 1u < pd_sets(pd, lv, pg, 0)) {
            pd_show(pd, lv, (uint32_t)k + 1u);
            return 1;
        }
        lv = here ? (lv + 1u) % 3u : 0u;
        ui.pdset[pd][lv] = 0;
    }
    else if (here) {
        pd_cycle();
        return 1;
    }
    pd_show(pd, lv, ui.pdset[pd][lv]);
    return 1;
}
/* the set's place: its name (b holds 24), k (0..) of n */
static void pd_set_name(char *b, uint32_t *k, uint32_t *n)
{
    uint8_t pg[PD_SETS];
    const char *nm[PD_SETS];
    uint32_t pd = pd_cur(), lv = pd_level();
    int32_t i;
    *n = pd_sets(pd, lv, pg, nm);
    i = pd_k_in(pd, lv, ui.page);
    *k = i < 0 ? 0u : (uint32_t)i;
    str_cpy(b, i < 0 ? PAGES[ui.page].title : nm[i], 24);
    if (pd == PD_EDIT && lv == 0u && i >= 0 && edit_kind() != 2u) {   /* (EDIT 1 . OSC: the engine's page name) */
        const char *pt = ENGINES[TSEL->eng_req % NENGINES]->page_title[i & 1];
        if (pt && pt[0] && pt[0] != '-') {
            str_cpy(b + str_len(b), " \xB7 ", 4);
            str_cpy(b + str_len(b), pt, 24 - str_len(b));
        }
    }
    if (pd == PD_EDIT && lv == 0u && i >= 0 && edit_kind() == 2u)    /* (DRUM: KIT, KIT 2, KIT 3) */
        str_cpy(b, i == 0 ? "KIT" : i == 1 ? "KIT 2" : "KIT 3", 24);
}

/* HOME: ECO and BEING are HOME itself (ui.home) at ui.hlv, GEN the generator's screen (CF_DICE_ALL, gen.c); each level
 * remembers its set (ui.hset: 0 HOME, 1 MACROS / ROUTES) */
static const char *const HOME_LV[3] = {"ECO", "BEING", "GEN"};
static uint32_t gen_page(void);                          /* (gen.c: the GEN screen's page, 0..2) */
static uint32_t home_level(void) { return ui.confirm == CF_DICE_ALL ? 2u : ui.hlv ? 1u : 0u; }
static uint32_t home_set(void) { return ui.hset[home_level() & 1u] & 1u; }
static int home_macros(void) { return ui.home && !ui.menu && ui.confirm != CF_DICE_ALL && home_set(); }   /* MACROS / ROUTES: M1..M4 */
/* a HOME tap on HOME: the level's other set (from elsewhere: go_home, at the level and set left) */
static void home_cycle(void)
{
    ui.hset[home_level() & 1u] ^= 1u;
    ui.force = 1;
    page_flash();
}

/* EDIT > FOCUS (a synth's detail): the parameter shown (E1..E8 that has a name: 0..7), the MOD slot aimed at it (its
 * source set; -1 none) */
static int focus_named(uint32_t k)
{
    const param_desc_t *d = track_desc(TSEL, P_E0 + (k & 7u));
    return d && d->label && d->label[0] != '-' && d->max > d->min;
}
static uint32_t focus_k(void)
{
    uint32_t k = ui.focus & 7u, n;
    for (n = 0; n < 8u && !focus_named(k); n++)
        k = (k + 1u) & 7u;
    return k;
}
static int32_t focus_slot(uint32_t k)
{
    uint32_t j;
    for (j = 0; j < 4u; j++)
        if (TSEL->p[P_M1SRC + 3u * j] && TSEL->p[P_M1DST + 3u * j] == (int16_t)(MD_E1 + k))
            return (int32_t)j;
    return -1;
}
