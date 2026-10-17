/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* (JIANT 0.9.4) Three engines draw their own creature where every other engine draws its being (ui_being.c): EDIT's
 * SOUND (ui_lzedit.c graph_sound_lz, on the left) and HOME's BEING (ui_lzhome.c, large in the middle); DRUM-X's kit
 * on EDIT > KIT and SOUND 1..4 (the whole panel), the colony staying on KIT 2, KIT 3 and X-MOD. Line art only,
 * antialiased (ui_organic.c og_line), in the beings' thermal colours (bg_col: heat_col; GREY and MONO their greys),
 * from small point tables (int8) and the synthesis state the UI already reads; nothing in the audio path. Each moves
 * only with its sound: in silence it rests (its time runs with the level, as the beings').
 *   VOICE   a sagittal CT of a head in profile (the face left): skin, the skull's two tables, the brain's folds, the
 *           corpus callosum, the cerebellum, the spine, the nasal cavity and its turbinates, the palate, the velum, the
 *           tongue, the mandible, the lips, the pharynx, the larynx and the trachea, each as dense as it is in a scan
 *           (bone hottest, air black). The vocal tract sings: the tongue's height from F1 and its front / back from F2
 *           (the vowel sounding: sing_view's formants with WORDS on, VOWEL / VOWEL2 / TALK without), the jaw open with
 *           F1, the lips rounded (o u w sh) or closed (p b m), the lower lip on the teeth (f v), the tongue's tip, blade
 *           or body on the place of the consonant (sing.c PHON[].place), the velum down for m n ng, the vocal folds
 *           shaking and the trachea's rings pulsing with the voicing, turbulence at the constriction for the frication
 *           (s sh f x, the bursts) and at the glottis for the aspiration (h, BREATH); a scan line sweeps down slowly.
 *   ANIMAL  a creature between a bird, an insect and a mammal: its outline, legs, wing, antennae / ears / crest are
 *           the same points in the three (VX_AN), mixed by weights that glide (a metamorphosis, ~0.6 s) to the MODE's
 *           animal (CMOS WHSTL a bird, PULSE WING an insect, GLOTT a mammal), leaned by SIZE past its presets' sizes
 *           (smaller: towards the insect; larger: the mammal) and stirred by CHAOS. With the call (each voice's syllable envelope, CMOS's
 *           chirp gate, WHSTL's syrinx amplitude): the beak / jaw opens and the throat (syrinx, larynx) swells, the
 *           wings buzz at the pulse rate (PULSE, WING), the legs stridulate, the ribs breathe (GLOTT).
 *   DRUM-X  the kit as a colony of small beings with eyes and a mouth, standing as a drum kit does: the kick large in
 *           the middle (its core hot), the snare flat on its stand, the toms on little legs, the hi-hat two shells (CH
 *           together, OH the top one lifted, its mouth open), the crash on a long stem and the cowbell a small head (BELL),
 *           the clap two little ones clapping together, the rim (RS) a spark on the snare's edge. A hit squashes its
 *           being (springing back), shuts its eyes and opens its mouth in a shout, as hard as the hit (the lane's own
 *           envelope: its velocity, its accent; it recovers with the sound's DECAY): a GHOST a small squash, a FLAM two
 *           (its hit to come a shadow), a ratchet repeated squashes. On EDIT > KIT (EDIT 1) the sound last struck named
 *           over it; on SOUND 1..4 the sound edited lit and named; HOME's BEING for a DRUM track. KIT 2, KIT 3 and
 *           X-MOD keep the colony of 0.5 (ui_being.c graph_colony: a being per sound, in two rows).
 * State: vxs (in the pool: no global of the RAM region moves), 160 bytes. MENU > ANIM OFF: they rest. */
#define VX_POOL __attribute__((section(".pool")))
typedef struct { int32_t cx, cy, s; } vxf_t;            /* a unit (Q4: 16) -> Q4 px: c + u * s / 256 */
static struct {
    uint32_t tm[3], fr[3];                               /* each one's time (it runs with its sound), the frame it ran */
    int32_t en[3];                                       /* its level now (0..256) */
    int32_t scan, scy;                                   /* VOICE: the scan line (Q4 units; as drawn, Q4 px: warmer there) */
    int16_t js, jc;                                      /* .. the jaw's turn (sin, cos Q15) about the condyle */
    int16_t art[11];                                     /* VOICE: the articulation, slewed (vx_voice) */
    int16_t w[3];                                        /* ANIMAL: bird insect mammal weights (0..256) */
    uint8_t age[8], last;                                /* DRUM-X: each lane's age seen; the lane struck last */
    uint32_t hitm[8], tl[8], kms;                        /* .. its last hit (fm1_ms), its being's time; the ms drawn */
    uint8_t kc;                                          /* .. drawing it (its colours: vx_col) */
    uint8_t init;
} vxs VX_POOL;

/* DRUM-X's beings in the thermal ramp of the mock-up the kit was chosen from (cyan blue violet red orange yellow white:
 * the 0.6's, the JIANT palette); every other palette as the beings (bg_col) */
static const uint8_t VK_HP[8] = {0, 26, 51, 82, 118, 154, 200, 255};
static const uint16_t VK_HC[8] = {0x13D4u, 0x2DFCu, 0x4B5Fu, 0x8B7Fu, 0xFA49u, 0xFC43u, 0xFE87u, 0xFFFFu};
static uint16_t vx_col(int32_t h)
{
    uint32_t k = 0;
    if (h < 0)                                           /* (a muted sound: dim, as the colony's) */
        return ux.mono || settings.palette == UI_BW_INDEX ? ux_gray(ux_luma(T_DIM) >> 3) : T_DIM;
    h = clamp(h, 0, 255);
    if (!vxs.kc || settings.palette != UI_JIANT_INDEX)
        return bg_col(h);
    while (k < 6u && h > VK_HP[k + 1u]) k++;
    return ux_mix(VK_HC[k], VK_HC[k + 1u], (h - VK_HP[k]) * 100 / (VK_HP[k + 1u] - VK_HP[k]));
}
static void vx_line(const vxf_t *f, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t h)
{
    int32_t X0 = f->cx + ((x0 * f->s) >> 8), Y0 = f->cy + ((y0 * f->s) >> 8);
    int32_t X1 = f->cx + ((x1 * f->s) >> 8), Y1 = f->cy + ((y1 * f->s) >> 8), d = (Y0 + Y1) / 2 - vxs.scy;
    if (d < 0) d = -d;
    if (d < 7 * 16 && vxs.scy > 0 && !vxs.kc)            /* (the scan line warms what it crosses: VOICE) */
        h += (7 * 16 - d) * 70 / (7 * 16);
    og_line(X0, Y0, X1, Y1, vx_col(h));
}
/* fixed segments (units) */
typedef struct { int8_t x0, y0, x1, y1; uint8_t h; } vxseg_t;
static void vx_segs(const vxf_t *f, const vxseg_t *g, uint32_t n)
{
    for (; n--; g++)
        vx_line(f, g->x0 * 16, g->y0 * 16, g->x1 * 16, g->y1 * 16, g->h);
}
/* a smooth curve through n points (Q4 units, x y pairs): quadratic arcs from midpoint to midpoint, each point their
 * control; closed or open (from the first point to the last) */
static void vx_curve(const vxf_t *f, const int32_t *p, uint32_t n, int closed, int32_t h)
{
    uint32_t i, k, e = closed ? n : n - 1u;
    int32_t px = 0, py = 0;
    for (i = 0; i < e; i++) {
        const int32_t *a = &p[2u * (i ? i - 1u : (closed ? n - 1u : 0u))], *b = &p[2u * i], *c = &p[2u * ((i + 1u) % n)];
        int32_t x0 = (a[0] + b[0]) / 2, y0 = (a[1] + b[1]) / 2, x1 = (b[0] + c[0]) / 2, y1 = (b[1] + c[1]) / 2;
        if (!closed && i == 0u) { x0 = b[0]; y0 = b[1]; }
        if (!closed && i + 1u == e) { x1 = c[0]; y1 = c[1]; }
        if (i == 0u) { px = x0; py = y0; }
        for (k = 1; k <= 3u; k++) {
            int32_t t = (int32_t)k * 85, u = 256 - t;   /* (t / 256: thirds) */
            int32_t x = (u * u * x0 + 2 * u * t * b[0] + t * t * x1) >> 16, y = (u * u * y0 + 2 * u * t * b[1] + t * t * y1) >> 16;
            if (k == 3u) { x = x1; y = y1; }
            vx_line(f, px, py, x, y, h);
            px = x;
            py = y;
        }
    }
}
/* a table's n points (int8 units) scaled k / 256 about (ax, ay) (units), into p (Q4 units) */
static void vx_pts(int32_t *p, const int8_t *t, uint32_t n, int32_t ax, int32_t ay, int32_t k)
{
    uint32_t i;
    for (i = 0; i < n; i++) {
        p[2u * i] = ax * 16 + (((t[2u * i] - ax) * 16 * k) >> 8);
        p[2u * i + 1u] = ay * 16 + (((t[2u * i + 1u] - ay) * 16 * k) >> 8);
    }
}
static void vx_tab(const vxf_t *f, const int8_t *t, uint32_t n, int closed, int32_t ax, int32_t ay, int32_t k, int32_t h)
{
    int32_t p[2 * 24];
    vx_pts(p, t, n, ax, ay, k);
    vx_curve(f, p, n, closed, h);
}
/* an ellipse (Q4 units), n sides, its top at angle a0 .. a1 (16-bit turns) */
static void vx_arc(const vxf_t *f, int32_t cx, int32_t cy, int32_t rx, int32_t ry, uint32_t a0, uint32_t a1, uint32_t n,
                   int32_t h)
{
    uint32_t i;
    int32_t px = 0, py = 0;
    for (i = 0; i <= n; i++) {
        uint32_t a = a0 + (uint32_t)(((uint64_t)(a1 - a0) * i) / n);
        int32_t x = cx + ((rx * og_cos(a)) >> 15), y = cy + ((ry * og_sin(a)) >> 15);
        if (i) vx_line(f, px, py, x, y, h);
        px = x;
        py = y;
    }
}
static uint32_t vx_rnd(uint32_t *r) { *r ^= *r << 13; *r ^= *r >> 17; *r ^= *r << 5; return *r; }
/* the level of track t now (0..256: its part's peak, slewed: a hit at once, a quick fall), k: which drawing; its time
 * runs with it (once a frame) */
static int32_t vx_level(const track_t *t, uint32_t k, int32_t en_home)
{
    uint32_t dt = ui.frame - vxs.fr[k];
    int32_t v;
    if (!dt)
        return vxs.en[k];
    if (dt > 8u) dt = 8u;
    vxs.fr[k] = ui.frame;
    if (ui_prefs & PREF_ANIM_OFF)
        return vxs.en[k] = 0;                            /* (ANIM OFF: at rest) */
    if (en_home >= 0)
        v = en_home;                                     /* (HOME: its own, eco_step's) */
    else {
        v = ((track_t *)t)->peak * 256 / 7000;
        ((track_t *)t)->peak = 0;
        v = v > 256 ? 256 : v;
        v = v > vxs.en[k] ? v : vxs.en[k] + (v - vxs.en[k] - 2) / 3;
    }
    v = v > 4 ? v : 0;                                   /* (HOME's level rests a little above 0: silence) */
    vxs.en[k] = v;
    if (v)
        vxs.tm[k] += dt * (uint32_t)(120 + v * 4);
    return v;
}

/* ------------------------------------------------------------------------------------------------- VOICE --- */
/* the head in units (x right = back of the head, y down; the face left), ~92 x 112 */
static const int8_t VH_SKULL[] = {-34, -12, -32, -27, -23, -40, -7, -48, 12, -47, 27, -39, 37, -26, 41, -10, 39, 3, 32, 10};
static const int8_t VH_SKIN[] = {-37, -20, -31, -35, -19, -48, 0, -54, 19, -52, 33, -43, 43, -28, 46, -11, 44, 3, 38, 15,
                                 35, 27, 38, 40, 42, 58};
static const int8_t VH_BASE[] = {-31, -9, -19, -8, -9, -7, -6, -3, -2, -3, -1, -7, 3, -4, 9, 7};   /* fossa, sella, clivus */
static const int8_t VH_CEREB[] = {18, -2, 24, -7, 33, -6, 37, 2, 33, 9, 24, 10};                    /* cerebellum */
static const int8_t VH_FACE[] = {-37, -20, -40, -9, -37, -4, -41, 6, -47, 16, -45, 21, -40, 22};      /* brow .. nose */
static const int8_t VH_NAS[] = {-37, -3, -26, -7, -12, -6, -10, 4, -14, 18};                       /* the nasal cavity's roof */
static const int8_t VH_CONCH[3][4] = {{-31, 2, 9, 3}, {-29, 9, 11, 3}, {-28, 15, 12, 2}};          /* turbinates x y w h */

/* the articulation (vxs.art): 0 tongue x (front -), 1 its height (palate 19 .. low 32), 2 the tip's lift, 3 the jaw
 * (Q8 of its full opening), 4 lips round, 5 lips closed, 6 velum down, 7 lower lip to the teeth, 8 the constriction's
 * x, 9 its y (Q4 units), 10 how closed it is (0 open .. 256 shut) */
static void vx_vowel_xy(int32_t f1, int32_t f2, int32_t *x, int32_t *y)
{
    *x = -11 * 16 - clamp(f2 - 800, 0, 1500) * 15 * 16 / 1500;      /* F2 800 .. 2300 Hz: back .. front */
    *y = 20 * 16 + clamp(f1 - 270, 0, 500) * 11 * 16 / 500;         /* F1 270 .. 770: high .. low */
}
static void vx_voice_target(const track_t *t, int16_t *a, int32_t *voi, int32_t *asp, int32_t *fri)
{
    int32_t f1, f2, x, y, i;
    uint32_t pl = PL_NONE, cls = PC_VOW, rnd = 0;
    if (t->p[P_VLANG]) {
        const sing_t *s = sing_view(t);
        uint32_t c = sing_phon(s);
        f1 = s->f[0];
        f2 = s->f[1];
        cls = PHON[c].cls;
        pl = PHON[c].place & 0x7Fu;
        rnd = PHON[c].place & PL_RND;
        if (cls == PC_ASP || cls == PC_SIL) pl = PL_NONE;
        *voi = s->gv > 0 ? s->gv >> 7 : 0;
        *asp = s->ga > 0 ? s->ga >> 7 : 0;
        *fri = s->gf > 0 ? s->gf >> 6 : 0;
        if (cls == PC_PLO && s->st == 0u) *fri = 0;
        if (cls == PC_SIL) *voi = 0;
    } else {                                             /* VOWEL -> VOWEL2 over TALK (as formant_render) */
        int32_t v0 = t->p[P_E0], v1 = t->p[P_E1], k = 0, j;
        uint32_t g = 0;
        for (j = 0; j < (int32_t)NVOICE; j++)
            if (t->v[j].active && (uint32_t)(t->v[j].s[7] & FMT_TALK_MASK) > g) g = (uint32_t)t->v[j].s[7] & FMT_TALK_MASK;
        if (t->p[P_E2]) { k = (int32_t)(g >> 9); k = (k * (65536 - k)) >> 15; }
        v0 += ((v1 - v0) * k) >> 15;
        {
            int32_t xx = clamp(v0, 0, 127) * 4 * 256 / 127, ii = xx >> 8, fr = xx & 255;
            if (ii >= 4) { ii = 3; fr = 256; }
            f1 = VOWEL_F[ii][0] + (((VOWEL_F[ii + 1][0] - VOWEL_F[ii][0]) * fr) >> 8);
            f2 = VOWEL_F[ii][1] + (((VOWEL_F[ii + 1][1] - VOWEL_F[ii][1]) * fr) >> 8);
            rnd = v0 > 80;
        }
        *voi = 256 - t->p[P_E5] * t->p[P_E5] / 64;
        *asp = t->p[P_E5] * 2;
        *fri = 0;
    }
    vx_vowel_xy(f1, f2, &x, &y);
    a[0] = (int16_t)x;
    a[1] = (int16_t)y;
    a[2] = 0;
    a[3] = (int16_t)clamp((f1 - 250) * 256 / 520, 0, 256);
    a[4] = (int16_t)(rnd ? 256 : 0);
    a[5] = 0;
    a[6] = (int16_t)(cls == PC_NAS ? 256 : 0);
    a[7] = 0;
    a[8] = (int16_t)(-30 * 16);                          /* (the constriction: the teeth by default) */
    a[9] = (int16_t)(27 * 16);
    a[10] = (int16_t)(pl == PL_NONE ? 0 : cls == PC_PLO || cls == PC_NAS || cls == PC_AFF ? 256 : cls == PC_FRI ? 190 : 110);
    switch (pl) {                                        /* the place: what touches what */
    case PL_LAB: a[5] = 256; a[3] = 40; a[8] = -40 * 16; a[9] = 30 * 16; break;
    case PL_LABDEN: a[7] = 256; a[3] = 50; a[8] = -38 * 16; a[9] = 28 * 16; break;
    case PL_DEN: a[2] = 300; a[0] = -26 * 16; a[3] = 70; a[8] = -36 * 16; a[9] = 27 * 16; break;
    case PL_ALV: a[2] = 256; a[3] = 60; a[8] = -31 * 16; a[9] = 22 * 16; break;
    case PL_POST: a[2] = 170; a[0] = -24 * 16; a[1] = 21 * 16; a[3] = 60; a[8] = -27 * 16; a[9] = 21 * 16; break;
    case PL_PAL: a[0] = -24 * 16; a[1] = 19 * 16; a[3] = 50; a[8] = -23 * 16; a[9] = 20 * 16; break;
    case PL_VEL: a[0] = -12 * 16; a[1] = 19 * 16; a[3] = 60; a[8] = -12 * 16; a[9] = 21 * 16; break;
    case PL_UVU: a[0] = -9 * 16; a[1] = 21 * 16; a[8] = -8 * 16; a[9] = 24 * 16; break;
    case PL_GLO: a[8] = -5 * 16; a[9] = 51 * 16; break;
    default: break;
    }
    for (i = 0; i < 11; i++)
        a[i] = (int16_t)clamp(a[i], -0x7000, 0x7000);
}
/* the head's fixed lines: the stem, the cord, the palate (bone), the upper incisor, the epiglottis, the pharynx's back
 * wall, the trachea's walls, the oesophagus; the mandible (its symphysis to the angle and the ramus, its inner edge, the
 * lower incisor: turned with the jaw); a vertebra's outline */
static const vxseg_t VH_SEG[] = {{3, -9, 13, 9, 120}, {9, -9, 19, 9, 120}, {14, 9, 17, 62, 100}, {19, 9, 21, 62, 100},
                                 {-38, 19, -14, 19, 226}, {-37, 20, -15, 20, 190}, {-37, 21, -36, 27, 248}, {-35, 21, -35, 27, 220},
                                 {-6, 42, -9, 37, 170}, {0, 14, 1, 50, 96}, {-10, 56, -10, 66, 110}, {-2, 55, -2, 66, 96},
                                 {2, 51, 3, 66, 80}};
static const int8_t VH_MAND[] = {-34, 33, -32, 46, -27, 51, -12, 49, 6, 42, 13, 10, -31, 36, -28, 45, -22, 47, -36, 29, -35, 35};
static const int8_t VH_VERT[] = {-3, 0, 3, 0, 4, 1, 4, 5, 3, 6, -3, 6, -4, 5, -4, 1, -3, 0};
/* n points (Q4 units) turned with the jaw about the condyle (14, 6) */
static void vx_jaw(int32_t *p, uint32_t n)
{
    for (; n--; p += 2) {
        int32_t x = p[0] - 14 * 16, y = p[1] - 6 * 16;
        p[0] = 14 * 16 + ((x * vxs.jc - y * vxs.js) >> 15);
        p[1] = 6 * 16 + ((x * vxs.js + y * vxs.jc) >> 15);
    }
}
static void vx_voice(const track_t *t, int32_t cx, int32_t cy, int32_t s, int32_t en_home)
{
    vxf_t f = {cx, cy, s};
    int16_t tg[11], *a = vxs.art;
    int32_t en = vx_level(t, 0, en_home), voi, asp, fri, i, p[2 * 16], q[2 * 9], jaw, hb;
    uint32_t tm = vxs.tm[0], r;
    vx_voice_target(t, tg, &voi, &asp, &fri);
    if (!vxs.init) { vxs.init = 1; for (i = 0; i < 11; i++) a[i] = tg[i]; }
    if (en || t->p[P_VLANG])                             /* (it moves as it sings; still in silence) */
        for (i = 0; i < 11; i++) a[i] = (int16_t)(a[i] + (tg[i] - a[i]) / 2);
    voi = voi * en >> 8;
    asp = asp * en >> 8;
    fri = fri * en >> 8;
    hb = en * 50 / 256;                                  /* (the sound warms the tract) */
    if (en) vxs.scan += 5 + en / 64;                     /* (the scan line runs with the sound) */
    if (vxs.scan > 66 * 16 || vxs.scan < -56 * 16) vxs.scan = -56 * 16;
    vxs.scy = cy + ((vxs.scan * s) >> 8);
    og_line_a(cx - 46 * s / 16, vxs.scy, cx + 46 * s / 16, vxs.scy, vx_col(60 + en / 6), 110);   /* (faint) */
    jaw = a[3];
    vxs.js = (int16_t)(jaw * 5000 / 256);                /* (the jaw: up to ~9 degrees) */
    vxs.jc = (int16_t)(32767 - ((vxs.js * vxs.js) >> 16));
    /* skin: the head's outline (cold), its inner band */
    vx_tab(&f, VH_SKIN, sizeof VH_SKIN / 2, 0, 0, 0, 256, 40);
    vx_tab(&f, VH_SKIN, sizeof VH_SKIN / 2, 0, 2, -6, 236, 70);
    vx_tab(&f, VH_FACE, sizeof VH_FACE / 2, 0, 0, 0, 256, 40);
    /* the lips, the chin, the throat: the jaw's (rounded: forward and pursed; closed: together; f v: on the teeth) */
    {
        int32_t pr = a[4] * 3 / 256 * 16, ap = (256 - a[5]) * (2 + jaw * 6 / 256) / 256 * 16 - a[4] * 16 / 256;
        int32_t lx = -41 * 16 - pr, ly = 31 * 16 + ap - a[7] * 3 / 256 * 16;
        int32_t u[2 * 4] = {-40 * 16, 22 * 16, -42 * 16 - pr, 25 * 16, lx + 16, 27 * 16 + ap / 3, -37 * 16, 30 * 16};
        int32_t l[2 * 7] = {lx + 16 + a[7] * 5 / 256 * 16, ly + 16, -40 * 16 - pr, 37 * 16, -36 * 16, 41 * 16, -37 * 16, 49 * 16,
                            -30 * 16, 55 * 16, -18 * 16, 54 * 16};
        vx_jaw(l, 6);
        l[12] = -12 * 16; l[13] = 57 * 16;
        vx_curve(&f, u, 4, 0, 46 + hb);                  /* the upper lip */
        vx_curve(&f, l, 5, 0, 46 + hb);                  /* the lower lip, the chin */
        vx_curve(&f, l + 8, 3, 0, 40);                   /* the throat */
        vx_line(&f, -12 * 16, 57 * 16, -12 * 16, 66 * 16, 40);
    }
    /* the skull: its outer table, the diploe, the inner table (bone: hottest); the base, the clivus */
    vx_tab(&f, VH_SKULL, sizeof VH_SKULL / 2, 0, 2, -12, 256, 236);
    vx_tab(&f, VH_SKULL, sizeof VH_SKULL / 2, 0, 2, -12, 243, 180);
    vx_tab(&f, VH_SKULL, sizeof VH_SKULL / 2, 0, 2, -12, 230, 226);
    vx_tab(&f, VH_BASE, sizeof VH_BASE / 2, 0, 0, 0, 256, 214);
    /* the brain: its folded cortex under the inner table (gyri), the white matter's edge inside, a few sulci, the
     * corpus callosum; the cerebellum's folia */
    {
        int32_t px = 0, py = 0, qx = 0, qy = 0;
        for (i = 0; i <= 36; i++) {                      /* (from the frontal lobe's floor over the top to the occiput) */
            uint32_t ang = 0x8A00u - (uint32_t)i * 0x0420u;
            int32_t w = og_sin((uint32_t)i * 0x2E00u + (tm >> 6)), c = og_cos(ang), sn = og_sin(ang);
            int32_t rx = 28 * 16 + ((w * 36) >> 15), ry = 19 * 16 + ((w * 30) >> 15);
            int32_t x = 4 * 16 + ((rx * c) >> 15), y = -21 * 16 - ((ry * sn) >> 15);
            int32_t x2 = 4 * 16 + (((rx * 3 / 4 - ((w * 30) >> 15)) * c) >> 15);
            int32_t y2 = -19 * 16 - (((ry * 3 / 4 - ((w * 26) >> 15)) * sn) >> 15);
            if (i) {
                vx_line(&f, px, py, x, y, 142 + hb / 2);
                if (i > 3 && i < 34) vx_line(&f, qx, qy, x2, y2, 100 + hb / 3);
            }
            if (i % 4 == 2 && i < 34)                    /* (a sulcus: from a valley inwards) */
                vx_line(&f, x, y, (x * 3 + x2) / 4, (y * 3 + y2) / 4 + 8, 116);
            px = x; py = y; qx = x2; qy = y2;
        }
        vx_arc(&f, 5 * 16, -15 * 16, 14 * 16, 6 * 16, 0x8000u, 65536u, 10, 176);   /* corpus callosum */
        vx_arc(&f, 5 * 16, -15 * 16, 12 * 16, 4 * 16, 0x8800u, 0xF800u, 8, 128);
        vx_tab(&f, VH_CEREB, sizeof VH_CEREB / 2, 1, 27, 2, 256, 140);
        for (i = 0; i < 3; i++)
            vx_arc(&f, 27 * 16, 2 * 16, (8 - 2 * i) * 16, (6 - 2 * i) * 16, 0xA000u, 90112u, 6, 112);
    }
    vx_segs(&f, VH_SEG, NELEM(VH_SEG));
    /* the spine: C2's dens, the bodies, the spinous processes (bone) */
    vx_arc(&f, 8 * 16, 11 * 16, 2 * 16, 3 * 16, 0, 65536u, 8, 214);
    for (i = 0; i < 6; i++) {
        int32_t y = 15 + i * 8, x = 7 + i / 2;
        uint32_t k;
        for (k = 0; k < 8u; k++)
            vx_line(&f, (x + VH_VERT[2 * k]) * 16, (y + VH_VERT[2 * k + 1]) * 16, (x + VH_VERT[2 * k + 2]) * 16,
                    (y + VH_VERT[2 * k + 3]) * 16, 224);
        vx_arc(&f, (x + 20) * 16, (y + 3) * 16, 3 * 16, 2 * 16, 0, 65536u, 6, 200);
    }
    /* the nasal cavity, its turbinates (warm with the air through them: m n ng) */
    vx_tab(&f, VH_NAS, sizeof VH_NAS / 2, 0, 0, 0, 256, 150);
    for (i = 0; i < 3; i++)
        vx_arc(&f, VH_CONCH[i][0] * 16 + VH_CONCH[i][2] * 8, VH_CONCH[i][1] * 16, VH_CONCH[i][2] * 8, VH_CONCH[i][3] * 8, 0x8000u,
               65536u + 0x2000u, 6, 128 + (a[6] * en >> 9));
    /* the velum: from the palate's end, raised to the pharynx's wall, down (open to the nose) for m n ng */
    {
        int32_t d = a[6] * 5 / 256 * 16, vx2 = -3 * 16 - d, vy2 = 22 * 16 + d;
        int32_t v[2 * 6] = {-14 * 16, 19 * 16, -7 * 16, 19 * 16 + d * 3 / 5, vx2, vy2, -14 * 16, 21 * 16, -8 * 16, 22 * 16 + d * 3 / 5,
                            vx2 - 16, vy2 + 16};
        vx_curve(&f, v, 3, 0, 160 + hb);
        vx_curve(&f, v + 6, 3, 0, 140 + hb);
    }
    /* the mandible (bone), the lower incisor: turned with the jaw */
    vx_pts(p, VH_MAND, 11, 0, 0, 256);
    vx_jaw(p, 11);
    vx_curve(&f, p, 6, 0, 230);
    vx_curve(&f, p + 12, 3, 0, 200);
    vx_line(&f, p[18], p[19], p[20], p[21], 248);
    /* the tongue: tip, blade, the body's top (F1 F2), the root down to the epiglottis; its floor to the hyoid */
    {
        int32_t bx = a[0], by = a[1], tl = a[2], tx, ty, rx = -7 * 16 + (bx + 18 * 16) / 5, ry = 34 * 16;
        int32_t fl[2 * 4] = {-30 * 16, 36 * 16, -24 * 16, 44 * 16, -14 * 16, 47 * 16, -7 * 16, 47 * 16};
        q[0] = -30 * 16; q[1] = 36 * 16; q[2] = -33 * 16 + tl * 2 / 256 * 16; q[3] = 30 * 16;
        vx_jaw(q, 2);
        vx_jaw(fl, 2);
        tx = q[2];
        ty = q[3] - tl * 8 / 256 * 16;
        q[3] = ty;
        q[4] = (tx + bx) / 2; q[5] = (ty + by) / 2 - tl * 2 / 256 * 16 + 16;
        q[6] = bx; q[7] = by + jaw * 3 * 16 / 256;
        q[8] = (bx + rx) / 2 + 2 * 16; q[9] = (by + ry) / 2 + jaw * 2 * 16 / 256;
        q[10] = rx; q[11] = ry; q[12] = -6 * 16; q[13] = 42 * 16;
        vx_curve(&f, q, 7, 0, 186 + hb);
        for (i = 1; i < 6; i++) {                         /* its inner band */
            p[2 * i] = q[2 * i] + (i > 4 ? -8 : 24);
            p[2 * i + 1] = q[2 * i + 1] + 3 * 16;
        }
        vx_curve(&f, p + 2, 5, 0, 150 + hb);
        vx_curve(&f, fl, 4, 0, 120);
        for (i = 0; i < 3; i++) {                        /* its fibres: a fan up from its floor (genioglossus) */
            int32_t x0 = (fl[2] * (3 - i) + fl[4] * i) / 3, y0 = (fl[3] * (3 - i) + fl[5] * i) / 3 - 2 * 16;
            vx_line(&f, x0, y0, (x0 + q[2 * (i + 2)] + 16) / 2 + 16, (y0 + q[2 * (i + 2) + 1] + 5 * 16) / 2, 84 + hb / 2);
        }
        vx_arc(&f, -7 * 16, 48 * 16, 2 * 16, 16, 0, 65536u, 6, 206);   /* the hyoid */
    }
    /* the larynx: the thyroid, the vocal folds shaking with the voicing; the trachea's rings pulsing up */
    {
        int32_t sh = voi > 8 ? ((og_sin(ui.frame * 0x5300u) * voi) >> 20) : 0, k, g = 180 + voi / 4;
        vx_arc(&f, -11 * 16, 50 * 16, 3 * 16, 5 * 16, 0x4000u, 0xC000u, 6, 170);
        vx_line(&f, -9 * 16, 52 * 16 - sh, -3 * 16, 51 * 16 + sh, g);
        vx_line(&f, -9 * 16, 54 * 16 + sh, -3 * 16, 53 * 16 - sh, g - 30);
        for (k = 0; k < 4; k++) {
            int32_t pu = voi ? ((og_sin(tm * 3u - (uint32_t)k * 0x3000u) + 32768) * voi) >> 16 : 0;
            vx_arc(&f, -6 * 16, (57 + k * 3) * 16, 5 * 16 + pu / 40, 16, 0x4000u, 0xC000u, 4, 130 + pu / 3);
        }
    }
    /* the constriction: where the tract closes, a hot mark (p t k: shut; s sh f: a slit; l r y w: narrowed) */
    if (a[10] > 60)
        og_node(cx + ((a[8] * s) >> 8), cy + ((a[9] * s) >> 8), 1 + a[10] / 128, vx_col(150 + a[10] / 3 + hb));
    /* the air: turbulence from the constriction out through the mouth (frication), at the glottis (aspiration) */
    r = ui.frame * 2654435761u + 1u;
    for (i = 0; i < fri / 10; i++) {                     /* (a jet: from the slit forward and out between the teeth) */
        int32_t d = (int32_t)(vx_rnd(&r) & 255u), x = a[8] - d * 18 * 16 / 256;
        int32_t y = a[9] + (29 * 16 - a[9]) * clamp(d * 2, 0, 256) / 256 + ((int32_t)((r >> 8) & 63u) - 32) * (16 + d / 8) / 64;
        if (x > -52 * 16)
            vx_line(&f, x, y, x - 2 * 16 - (int32_t)((r >> 16) & 15u), y + (int32_t)((r >> 20) & 15u) - 8, 236 - d / 3);
    }
    for (i = 0; i < asp / 24; i++) {                     /* (breath at the folds, rising) */
        int32_t d = (int32_t)(vx_rnd(&r) & 127u), x = -6 * 16 + (int32_t)((r >> 8) & 63u) - 32, y = 52 * 16 - d * 2;
        vx_line(&f, x, y, x + (int32_t)((r >> 16) & 15u) - 8, y - 2 * 16, 190 - d / 2);
    }
    vxs.scy = -0x7FFF;
}

/* ------------------------------------------------------------------------------------------------ ANIMAL --- */
/* the three animals, the same points in each (units, facing left, ~100 x 70): the outline (16, from the beak / snout
 * / mouthparts' tip round the head, the back, the tail, the belly, the throat, the chin), four limbs (hip knee foot:
 * a bird's two legs, the others folded into its belly; an insect's front, middle and jumping legs; a mammal's four), the
 * wing (5: a bird's folded wing, an insect's forewings, a mammal's ribcage), two head strokes (3: a crest, antennae,
 * ears), the eye, the mouth's corner, the throat's organ (syrinx, tymbal, larynx) */
enum { VA_OUT = 0, VA_LIMB = 32, VA_WING = 56, VA_HEAD = 66, VA_EYE = 78, VA_MTH = 80, VA_ORG = 82, VA_NP = 84 };
static const int8_t VX_AN[3][VA_NP] = {
    {   /* a bird (a songbird perched) */
        -48, -15, -34, -24, -26, -29, -17, -24, -8, -15, 8, -11, 22, -7, 30, -6, 52, 3, 30, 4, 18, 11, 2, 15, -12, 9, -20, -1,
        -26, -8, -38, -12,
        2, 13, 7, 23, 1, 33, 8, 12, 13, 23, 8, 33, 10, 12, 10, 12, 10, 12, 12, 11, 12, 11, 12, 11,
        -15, -12, 4, -14, 30, -3, 12, 4, -8, 2,
        -27, -28, -26, -35, -19, -38, -23, -27, -18, -32, -13, -33,
        -32, -20, -36, -14, -16, 2},
    {   /* an insect (a cricket) */
        -46, 3, -44, -8, -37, -13, -29, -11, -21, -13, 0, -11, 20, -9, 36, -5, 50, 0, 37, 4, 22, 8, 2, 8, -18, 7, -28, 6,
        -35, 7, -43, 7,
        -24, 6, -33, 16, -40, 25, -14, 7, -10, 19, -18, 28, 2, 6, 24, -12, 36, 26, -8, 7, -2, 18, 4, 27,
        -22, -13, 0, -16, 42, -7, 30, -2, -16, -7,
        -43, -9, -32, -36, 8, -46, -41, -9, -22, -32, 24, -38,
        -38, -5, -42, 4, 18, -2},
    {   /* a mammal (a fox) */
        -52, -6, -38, -17, -29, -22, -20, -17, -10, -15, 8, -16, 22, -15, 30, -15, 52, -19, 35, -2, 22, 6, 2, 8, -14, 6, -22, 2,
        -31, -1, -45, -1,
        -14, 4, -17, 18, -15, 32, -8, 4, -9, 18, -6, 32, 18, 4, 26, 17, 20, 32, 23, 4, 31, 17, 26, 32,
        -14, -9, 0, -11, 12, -7, 6, 5, -10, 4,
        -28, -21, -26, -33, -20, -20, -24, -20, -19, -31, -15, -17,
        -36, -13, -38, -3, -27, 2},
};
static const uint8_t VA_EYE_R[3] = {3, 5, 2};
/* MODE -> the animal on the line insect 0 .. bird 128 .. mammal 256; the middle of its presets' SIZE */
static const uint8_t VA_POS[AN_NMODE] = {128, 0, 128, 0, 255}, VA_MID[AN_NMODE] = {64, 30, 50, 70, 64};   /* (.. its presets' SIZE) */
static void vx_animal(const track_t *t, int32_t cx, int32_t cy, int32_t s, int32_t en_home)
{
    vxf_t f = {cx, cy, s};
    const int16_t *p = t->p;
    uint32_t mode = an_mode(p), i, j;
    int32_t en = vx_level(t, 1, en_home), act = 0, w[3], m, tgt, pt[2 * 42], q[2 * 16], op, flap, hb;
    uint32_t tm = vxs.tm[1];
    for (i = 0; i < NVOICE; i++) {                       /* the call now: each voice's level (its syllable's, CMOS's gate) */
        const voice_t *v = &t->v[i];
        int32_t a;
        if (!v->active) continue;
        a = v->env_out > 0 ? v->env_out >> 7 : 0;
        if (mode == AN_WHSTL) a = a * clamp((int32_t)v->ph[1], 0, 24000) / 16000;
        if (a > act) act = a;
    }
    act = clamp(act, 0, 256) * clamp(en * 2, 0, 256) >> 8;
    tgt = VA_POS[mode];
    if (mode != AN_CMOS) {                               /* SIZE (CMOS: CAP there) past the mode's own animals' sizes */
        int32_t d = p[P_E3] - VA_MID[mode];
        d = d > 12 ? d - 12 : d < -12 ? d + 12 : 0;
        tgt += d * 6 / 5;
        if (mode != AN_GLOTT)
            tgt += (og_sin(tm >> 2) * p[P_E2]) >> 17;   /* (CHAOS stirs it) */
    }
    tgt = clamp(tgt, 0, 256);
    m = vxs.w[0];
    if (!vxs.w[1]) { vxs.w[1] = 1; m = tgt; }           /* (the first look: at once) */
    if (vxs.w[2] != (int16_t)ui.frame) {                 /* (the metamorphosis glides: once a frame) */
        m += (tgt - m) / 5 + (tgt > m) - (tgt < m);
        vxs.w[2] = (int16_t)ui.frame;
    }
    vxs.w[0] = (int16_t)m;
    w[0] = m >= 128 ? (256 - m) * 2 : m * 2;             /* bird */
    w[1] = m < 128 ? (128 - m) * 2 : 0;                  /* insect */
    w[2] = m > 128 ? (m - 128) * 2 : 0;                  /* mammal */
    if (w[0] > 256) w[0] = 256;
    hb = act * 60 / 256;
    for (i = 0; i < VA_NP / 2; i++)                      /* the animal now: the three mixed (Q4 units) */
        for (j = 0; j < 2u; j++)
            pt[2 * i + j] = (VX_AN[0][2 * i + j] * w[0] + VX_AN[1][2 * i + j] * w[1] + VX_AN[2][2 * i + j] * w[2]) >> 4;
    /* the call: the beak / mouth opens (the chin down about the mouth's corner), wings, legs, ribs move */
    op = act * (8 + (mode == AN_GLOTT ? p[P_E1] / 16 : 0)) / 256 * 16;
    pt[2 * 15 + 1] += op * (w[0] + w[2]) / 256 + op / 3;
    pt[2 * 14 + 1] += op / 3;
    flap = 0;
    if (mode == AN_PULSE) flap = act ? (int32_t)((ui.frame & 1u) ? 1 : -1) * (2600 + act * 20) : 0;
    else if (mode == AN_WING) flap = act ? og_sin(ui.frame * 0x5100u) * (act * 26) >> 15 : 0;
    else if (act > 40) flap = og_sin(ui.frame * 0x2400u) * (act / 4) >> 9;
    flap -= (w[1] * 3000) >> 8;                           /* (an insect's wings raised a little, as it sings) */
    {   /* the body: three isotherms, the outer cold */
        static const int16_t SC[3] = {256, 170, 96}, HT[3] = {40, 116, 176};
        int32_t breath = w[2] ? ((og_sin(tm >> 1) + 32768) >> 12) * w[2] / 256 : 0;
        for (j = 0; j < 3u; j++) {
            for (i = 0; i < 16u; i++) {
                q[2 * i] = 2 * 16 + (((pt[2 * i] - 2 * 16) * SC[j]) >> 8);
                q[2 * i + 1] = ((pt[2 * i + 1] * (SC[j] + (j ? breath : breath / 2))) >> 8);
            }
            vx_curve(&f, q, 16, 1, HT[j] + hb + en / 8);
        }
    }
    /* the limbs: hip, knee, foot (a bird's feet with toes; an insect's legs shaking as it stridulates) */
    for (i = 0; i < 4u; i++) {
        int32_t *l = &pt[VA_LIMB + 6 * i], jx = 0;
        if (mode == AN_PULSE && act && i == 2u) jx = (ui.frame & 1u) ? 2 * 16 : -2 * 16;
        q[0] = l[0]; q[1] = l[1]; q[2] = l[2] + jx; q[3] = l[3] - jx / 2; q[4] = l[4]; q[5] = l[5];
        if ((q[0] - q[4]) * (q[0] - q[4]) + (q[1] - q[5]) * (q[1] - q[5]) < 4 * 256)
            continue;                                    /* (folded away) */
        vx_line(&f, q[0], q[1], q[2], q[3], i & 1u ? 70 : 120);
        vx_line(&f, q[2], q[3], q[4], q[5], i & 1u ? 70 : 120);
        if (w[2] > 96)                                   /* (a mammal's paws) */
            vx_line(&f, q[4], q[5], q[4] - 4 * 16 * w[2] / 256, q[5], i & 1u ? 70 : 120);
        if (w[0] > 96 && i < 2u) {                       /* (toes) */
            vx_line(&f, q[4], q[5], q[4] - 6 * 16 * w[0] / 256, q[5] + 16, 110);
            vx_line(&f, q[4], q[5], q[4] + 4 * 16 * w[0] / 256, q[5] + 16, 110);
        }
    }
    /* the wing (turned about its root by the flap; WING mode: a blur of two) / the ribcage (breathing) */
    {
        int32_t k, n = mode == AN_WING && act ? 2 : 1, c0 = og_cos((uint32_t)flap & 0xFFFFu), s0 = og_sin((uint32_t)flap & 0xFFFFu);
        for (k = 0; k < n; k++) {
            int32_t ox = pt[VA_WING], oy = pt[VA_WING + 1], cs = k ? c0 : c0, sn = k ? -s0 : s0;
            for (i = 0; i < 5u; i++) {
                int32_t x = pt[VA_WING + 2 * i] - ox, y = pt[VA_WING + 2 * i + 1] - oy;
                int32_t sc = 256 + (w[2] * ((og_sin(tm >> 1) >> 9) + act / 2)) / 256 / 2;
                q[2 * i] = ox + (((x * cs - y * sn) >> 15) * sc >> 8);
                q[2 * i + 1] = oy + (((x * sn + y * cs) >> 15) * sc >> 8);
            }
            vx_curve(&f, q, 5, 1, (k ? 90 : 150) + hb);
            for (i = 1; i < 4u; i++)                     /* (veins / feathers / ribs) */
                vx_line(&f, q[0] + (q[2 * i] - q[0]) / 5, q[1] + (q[2 * i + 1] - q[1]) / 5,
                        q[2 * i] - (q[2 * i] - q[0]) / 6, q[2 * i + 1] - (q[2 * i + 1] - q[1]) / 6, 104 + hb);
        }
    }
    /* the head's strokes: crest, antennae (trembling with the call), ears */
    for (i = 0; i < 2u; i++) {
        int32_t *h = &pt[VA_HEAD + 6 * i], tr = w[1] && act ? (og_sin(ui.frame * 0x3700u + i * 0x5000u) * act) >> 17 : 0;
        q[0] = h[0]; q[1] = h[1]; q[2] = h[2] + tr; q[3] = h[3]; q[4] = h[4] + tr * 2; q[5] = h[5] - tr;
        vx_curve(&f, q, 3, 0, i ? 96 : 140);
    }
    /* the eye (hot), the mouth's line, the throat's organ swelling with the call; song arcs from the mouth */
    vx_arc(&f, pt[VA_EYE], pt[VA_EYE + 1], (VA_EYE_R[0] * w[0] + VA_EYE_R[1] * w[1] + VA_EYE_R[2] * w[2]) >> 4,
           (VA_EYE_R[0] * w[0] + VA_EYE_R[1] * w[1] + VA_EYE_R[2] * w[2]) >> 4, 0, 65536u, 8, 236);
    og_node(cx + ((pt[VA_EYE] * s) >> 8), cy + ((pt[VA_EYE + 1] * s) >> 8), 1, vx_col(250));
    vx_line(&f, pt[VA_MTH], pt[VA_MTH + 1], pt[0], pt[1], 150);
    vx_line(&f, pt[VA_MTH], pt[VA_MTH + 1], pt[2 * 15], pt[2 * 15 + 1], 150);
    {
        int32_t r = (2 * 16 + act * 3 * 16 / 256) * (w[0] + w[2]) / 256;
        if (r > 16)
            vx_arc(&f, pt[VA_ORG], pt[VA_ORG + 1], r, r * 3 / 4, 0, 65536u, 10, 180 + hb);
        if (w[1] > 40)                                   /* (the abdomen's segments: the tymbal pulsing) */
            for (i = 0; i < 4u; i++) {
                int32_t x = (pt[2 * 6] * (int32_t)(i + 1) + pt[2 * 5] * (int32_t)(4 - i)) / 5, sp = act ? (int32_t)((ui.frame + i) & 1u) * 16 : 0;
                vx_line(&f, x + sp, pt[2 * 5 + 1] + 2 * 16, x - 2 * 16 + sp, pt[2 * 11 + 1] - 2 * 16, 60 + (w[1] * (90 + hb) >> 8));
            }
    }
    if (act > 24)                                        /* the call: arcs out of the mouth, as loud as it is */
        for (i = 0; i < 3u; i++) {
            int32_t ph = (int32_t)((tm >> 4) + i * 85u) & 255, r = (6 + ph / 12) * 16;
            vx_arc(&f, pt[0] - 2 * 16, pt[1] + op / 2, r, r, 0x6000u, 0xA000u, 5, (act * (255 - ph)) >> 8);
        }
}

/* ------------------------------------------------------------------------------------------------ DRUM-X --- */
/* the kit as a colony of small beings (units ~124 x 90: x right, y down, the floor at 40), each drum one with eyes and
 * a mouth: the kick large in the middle, the snare flat on its stand, the toms on little legs, the hi-hat two shells,
 * the crash on a long stem, the cowbell a small head over the kick, the clap two little ones clapping */
enum { VK_KICK, VK_SNARE, VK_CLAP, VK_HATC, VK_HATO, VK_TOM, VK_RIM, VK_BELL };
/* 256 * 2^(-ms / half), Q8 (the squash's fall) */
static int32_t vx_fall(uint32_t ms, uint32_t half)
{
    uint32_t n = ms / half;
    int32_t v;
    if (n > 8u) return 0;
    v = 256 >> n;
    return v - (int32_t)((uint32_t)v * (ms % half) / (2u * half));
}
/* a being's outline (ui_being.c bg_outline's, its lobes 3, the spikes' sharpness 4), squashed: at (x, y), radius r
 * (Q4 units), x stretched by ax / 256, then scaled sx sy / 256 about x and its foot (by: y's place after the squash) */
typedef struct { int32_t x, y, by, ax, sx, sy, sp, len, wob, rip; } vxb_t;
static void vx_blob(const vxf_t *f, const vxb_t *b, int32_t r, int outer, uint32_t t, uint32_t nv, int32_t h)
{
    uint32_t i;
    int32_t px = 0, py = 0;
    for (i = 0; i <= nv; i++) {
        uint32_t th = (i % nv) * 65536u / nv;
        int32_t rr = r + ((((r * b->wob) >> 8) * ((9175 * og_sin(3u * th + t) + 3932 * og_sin(5u * th - 2u * t)) >> 15)) >> 15), x, y;
        if (outer) {
            int32_t c2 = og_sin(((uint32_t)b->sp * th >> 1) + 0x4000u), p = c2 > 0 ? c2 : 0;
            p = (p * p) >> 15;
            p = (p * p) >> 15;                           /* (c2^4) */
            rr += (((((r * b->len) >> 8) * p) >> 15) * 26) >> 4;
            rr += ((((r * b->rip) >> 8) * og_sin(th * 18u + 3u * t)) >> 15) / 14;
        }
        x = b->x + (((((rr * og_cos(th)) >> 15) * b->ax) >> 8) * b->sx >> 8);
        y = b->by + ((((rr * og_sin(th)) >> 15) * b->sy) >> 8);
        if (i) vx_line(f, px, py, x, y, h);
        px = x;
        py = y;
    }
}
/* a drum's being at (x, y), rx x ry (Q4 units): e its hit now (0..256: the lane's envelope), age since it (ms), t its
 * time (it runs with the sound), sp its spikes, len their length, legs (Q4 units, 0: none); lit 1 the sound edited, -1
 * muted. Struck it squashes (springing back with the hit's age), shuts its eyes and shouts (the mouth open) */
static void vx_bug(const vxf_t *f, int32_t x, int32_t y, int32_t rx, int32_t ry, int32_t e, uint32_t age, uint32_t t,
                   int32_t sp, int32_t len, int32_t legs, int32_t lit)
{
    vxb_t b;
    int32_t sq = e ? (((e * og_cos(age * 229u)) >> 15) * vx_fall(age, 115u)) >> 8 : 0;   /* (22 rad/s, e^-6t) */
    int32_t k, ey, ex, mo, hb = lit > 0 ? 50 : 0, ho = lit < 0 ? -1 : 41 + e * 77 / 256 + hb;
    int32_t hi = lit < 0 ? -1 : 92 + e * 90 / 256 + hb, he = lit < 0 ? -1 : 243;
    uint32_t nv = (uint32_t)clamp(((rx + ry) * f->s >> 12) * 2, 20, 64);
    if (nv < (uint32_t)sp * 3u) nv = (uint32_t)sp * 3u;
    if (sys_load) nv = nv * (sys_load == LOAD_ECO ? 1u : 2u) / (sys_load == LOAD_ECO ? 2u : 3u);   /* (LITE, ECO) */
    b.x = x;
    b.y = y;
    b.sx = 256 + sq * 90 / 256;
    b.sy = 256 - sq * 102 / 256;
    b.by = y + ry * (256 - b.sy) / 256;
    b.ax = rx * 256 / ry;
    b.sp = sp;
    b.len = len + e * 60 / 256;
    b.wob = 40 + e * 120 / 256;
    b.rip = e * 100 / 256;
    if (legs)
        for (k = -1; k <= 1; k += 2) {
            int32_t x1 = x + k * rx * 7 / 10, y1 = y + ry + legs * 6 / 10;
            vx_line(f, x + k * rx / 2, y + ry * 7 / 10, x1, y1, lit < 0 ? -1 : 56 + hb);
            vx_line(f, x1, y1, x + k * rx * 6 / 10, y + ry + legs, lit < 0 ? -1 : 56 + hb);
        }
    vx_blob(f, &b, ry, 1, t, nv, ho);                    /* the membrane (cold), the core */
    vx_blob(f, &b, ry * 66 / 100, 0, t, nv * 2u / 3u, hi);
    ey = b.by - ry * 15 * b.sy / 25600;
    ex = rx * 32 * b.sx / 25600;
    for (k = -1; k <= 1; k += 2) {                       /* the eyes: open, shut on a hard hit */
        if (e > 90)
            vx_line(f, x + k * ex - 22, ey, x + k * ex + 22, ey, he);
        else {
            int32_t r = (ry * 12 / 100 * f->s) >> 12, X = f->cx + (((x + k * ex) * f->s) >> 8), Y = f->cy + ((ey * f->s) >> 8);
            if (r > 1)
                og_node(X, Y, r, vx_col(he));
            else {                                       /* (a small one: a 2 x 2 dot) */
                uint16_t c = vx_col(he);
                X = (X - 8) >> 4;
                Y = (Y - 8) >> 4;
                og_plot(X, Y, c, 256); og_plot(X + 1, Y, c, 256); og_plot(X, Y + 1, c, 256); og_plot(X + 1, Y + 1, c, 256);
            }
        }
    }
    mo = e * ry * 4 / 2560;                              /* the mouth: a line, a shout as loud as the hit */
    if (mo > 6)
        vx_arc(f, x, ey + ry * 32 * b.sy / 25600 + mo / 2, ry * 18 / 100 + mo / 5, mo, 0, 65536u, 12, 205);
    else
        vx_line(f, x - ry * 15 / 100, ey + ry * 3 / 10, x + ry * 15 / 100, ey + ry * 3 / 10, lit < 0 ? -1 : 154);
}
/* the kit at (cx, cy) Q4 px, s: a unit's px (Q8); sel: the sound edited (lit), -1 none */
static void vx_kit(const track_t *t, int32_t cx, int32_t cy, int32_t s, int32_t en_home, int32_t sel)
{
    vxf_t f = {cx, cy, s};
    drum_lane_t *K = drum_kit_of(t);
    int32_t e[8], lt[8], en = vx_level(t, 2, en_home), i, k;
    uint32_t ag[8], now = fm1_ms, dt = now - vxs.kms;
    if (dt > 100u) dt = 100u;                            /* (a pause: no jump) */
    vxs.kms = now;
    vxs.kc = 1;
    for (i = 0; i < 8; i++) {                            /* each lane's hit now: its envelope (velocity, accent, decay) */
        int32_t a = K ? K[i].age : 255;
        lt[i] = dx_lane_muted((uint32_t)i) ? -1 : i == sel;
        e[i] = K && K[i].x.live && lt[i] >= 0 && !(ui_prefs & PREF_ANIM_OFF) ? clamp(K[i].x.ea >> 22, 0, 256) : 0;
        if (K && K[i].dm == 2u) e[i] = e[i] * 2 / 3;   /* (GHOST: a small squash) */
        if (K && a < vxs.age[i] && e[i]) {               /* a new hit (a FLAM's two, a ratchet's each) */
            vxs.hitm[i] = now;
            vxs.last = (uint8_t)i;
        }
        vxs.age[i] = (uint8_t)a;
        ag[i] = now - vxs.hitm[i] < 4000u ? now - vxs.hitm[i] : 4000u;
        if (en || e[i])                                  /* (its time runs with the sound: in silence it rests) */
            vxs.tl[i] += dt * (uint32_t)(60 * 256 + e[i] * 300) * 30u / 256000u;
    }
    vx_line(&f, -60 * 16, 40 * 16, 60 * 16, 40 * 16, 26);   /* the floor */
    /* the crash (BELL) on its stem */
    vx_line(&f, 46 * 16, -24 * 16, 48 * 16, 40 * 16, 41);
    vx_bug(&f, 44 * 16, -30 * 16, 16 * 16, 4 * 16, e[VK_BELL], ag[VK_BELL], vxs.tl[VK_BELL] + 3000u, 9, 40, 0, lt[VK_BELL]);
    /* the hi-hat: two shells on their stand, the top one lifting (OH: its mouth open), together on CH */
    {
        int32_t op = e[VK_HATO] > 5 ? 48 + e[VK_HATO] * 80 / 256 : e[VK_HATC] > 5 ? 0 : 24;
        vx_line(&f, -52 * 16, -2 * 16, -52 * 16, 40 * 16, 41);
        vx_bug(&f, -52 * 16, 0, 12 * 16, 54, e[VK_HATC] * 3 / 5, ag[VK_HATC], vxs.tl[VK_HATC], 12, 10, 0, lt[VK_HATC]);
        vx_bug(&f, -52 * 16, -5 * 16 - op, 12 * 16, 54, e[VK_HATC] > e[VK_HATO] ? e[VK_HATC] : e[VK_HATO],
               ag[VK_HATC] < ag[VK_HATO] ? ag[VK_HATC] : ag[VK_HATO], vxs.tl[VK_HATO] + 900u, 12, 10, 0,
               lt[VK_HATO] < 0 && lt[VK_HATC] < 0 ? -1 : lt[VK_HATO] > 0 || lt[VK_HATC] > 0);
    }
    /* the toms (TOM): two on little legs over the kick, the floor tom on longer ones */
    vx_bug(&f, -15 * 16, -14 * 16, 9 * 16, 7 * 16, e[VK_TOM], ag[VK_TOM], vxs.tl[VK_TOM] + 200u, 5, 20, 6 * 16, lt[VK_TOM]);
    vx_bug(&f, 15 * 16, -14 * 16, 9 * 16, 7 * 16, e[VK_TOM] * 3 / 4, ag[VK_TOM] + 30u, vxs.tl[VK_TOM] + 400u, 5, 20, 6 * 16, lt[VK_TOM]);
    vx_bug(&f, 40 * 16, 18 * 16, 11 * 16, 10 * 16, e[VK_TOM] / 2, ag[VK_TOM] + 60u, vxs.tl[VK_TOM] + 600u, 5, 20, 12 * 16, lt[VK_TOM]);
    /* the kick, large in the middle; the snare flat on its stand */
    vx_bug(&f, 0, 16 * 16, 22 * 16, 20 * 16, e[VK_KICK], ag[VK_KICK], vxs.tl[VK_KICK] + 1000u, 4, 14, 4 * 16, lt[VK_KICK]);
    vx_bug(&f, -34 * 16, 10 * 16, 13 * 16, 6 * 16, e[VK_SNARE], ag[VK_SNARE], vxs.tl[VK_SNARE] + 1400u, 14, 12, 22 * 16, lt[VK_SNARE]);
    /* the rim (RS): a spark on the snare's edge, its rays turning as it rings */
    if (e[VK_RIM] > 5 || lt[VK_RIM] > 0)
        for (k = 0; k < 6; k++) {
            uint32_t a = (uint32_t)k * 10923u + ag[VK_RIM] * 63u;
            int32_t r1 = 3 * 16 + e[VK_RIM] * 4 * 16 / 256;
            vx_line(&f, -47 * 16 + ((32 * og_cos(a)) >> 15), 8 * 16 + ((32 * og_sin(a)) >> 15), -47 * 16 + ((r1 * og_cos(a)) >> 15),
                    8 * 16 + ((r1 * og_sin(a)) >> 15), 154 + e[VK_RIM] * 102 / 256);
        }
    og_node(cx + ((-47 * 16 * s) >> 8), cy + ((8 * 16 * s) >> 8), 1, vx_col(lt[VK_RIM] < 0 ? -1 : 102 + e[VK_RIM] * 154 / 256));
    /* the cowbell (BELL): a small head over the kick */
    vx_bug(&f, 0, -30 * 16, 6 * 16, 5 * 16, e[VK_BELL], ag[VK_BELL], vxs.tl[VK_BELL] + 1800u, 3, 30, 0, lt[VK_BELL]);
    /* the clap: two little ones, apart, clapping together */
    {
        int32_t g = 8 * 16 - e[VK_CLAP] * 6 * 16 / 256;
        vx_bug(&f, -27 * 16 - g, -33 * 16, 4 * 16, 4 * 16, e[VK_CLAP], ag[VK_CLAP], vxs.tl[VK_CLAP] + 2200u, 5, 20, 0, lt[VK_CLAP]);
        vx_bug(&f, -27 * 16 + g, -33 * 16, 4 * 16, 4 * 16, e[VK_CLAP], ag[VK_CLAP], vxs.tl[VK_CLAP] + 2600u, 5, 20, 0, lt[VK_CLAP]);
    }
    /* a FLAM's hit to come: the being's shadow, doubled */
    for (i = 1; i < 8; i++)
        if (K && K[i].fl) {
            static const int8_t PX[8] = {0, -34, -27, -52, -52, -15, -47, 44}, PY[8] = {16, 10, -33, 0, -8, -14, 8, -30};
            vx_arc(&f, PX[i] * 16 + 2 * 16, PY[i] * 16 + 16, 6 * 16, 3 * 16, 0, 65536u, 12, 64);
        }
    vxs.kc = 0;
}

/* ------------------------------------------------------------------------------------------------ the glue --- */
/* the engine's own creature at (cx, cy) px for a being of radius r px (EDIT 22, HOME 30), en_home: HOME's level for the
 * track (-1: its own); 0 when the engine has none (its being then) */
static int vx_draw(const track_t *t, int32_t cx, int32_t cy, int32_t r, int32_t en_home)
{
    const engine_t *e = ENGINES[t->eng_req % NENGINES];
    if (e == &ENG_FORMANT) {
        vx_voice(t, cx * 16 + 3 * 16, cy * 16 - 8 * 16, 244, en_home);   /* (one size: the head fills the height) */
        return 1;
    }
    if (t->eng_req % NENGINES == ENGI_DRUM) {
        vx_kit(t, cx * 16, cy * 16 + 4 * 16, r * 256 / 26, en_home, -1);
        return 1;
    }
    if (e == &ENG_ANIMAL) {
        vx_animal(t, cx * 16 + 2 * 16, cy * 16 + 2 * 16, r * 256 / 21, en_home);
        return 1;
    }
    return 0;
}
/* EDIT > KIT (lane -1) and SOUND 1..4 (a DRUM track): the kit large on the panel, KIT over it on the left; on the right
 * the sound last struck (KIT) or the sound edited, lit (SOUND: lane) */
static void vx_kit_page(const track_t *t, int32_t lane)
{
    char b[16];
    uint32_t l;
    vx_kit(t, 120 * 16, 73 * 16, 368, -1, lane);
    l = lane < 0 ? vxs.last & 7u : (uint32_t)lane & 7u;
    str_cpy(b, drum_lane_abbr(t, l), sizeof b);
    str_cpy(b + str_len(b), " \xB7 ", sizeof b - str_len(b));
    str_cpy(b + str_len(b), drum_lane_name(t, l), sizeof b - str_len(b));
    cv_text(8, 6 - AF_X_CAP_Y, &AF_X, "KIT", T_THEME);
    cv_text_r(232, 6 - AF_X_CAP_Y, &AF_X, b, lane < 0 ? T_TEXT : T_THEME, T_SURF);
}
