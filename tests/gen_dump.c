/* the generator audited: every genre as text (seed, energy from argv) */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
static const char *NN[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
static const char *RLN[RL_N] = {"DRUM", "BASS", "PAD", "LEAD", "ARP", "TEX"};
static void nname(char *b, int n) { sprintf(b, "%s%d", NN[n % 12], n / 12 - 1); }
int main(int argc, char **argv)
{
    uint32_t genre = argc > 1 ? (uint32_t)atoi(argv[1]) : 0, seed = argc > 2 ? (uint32_t)atoi(argv[2]) : 1;
    uint32_t energy = argc > 3 ? (uint32_t)atoi(argv[3]) : 50, from = genre ? genre : 1, to = genre ? genre : NGENRE, g, k, i, l;
    uint32_t steps = argc > 4 ? (uint32_t)atoi(argv[4]) : 32;
    ui_power_on(); go_home(); frames(4);
    ui_rec_prefs |= PREF_SIX >> 8; rp_apply();
    for (g = from; g <= to; g++) {
        gp.genre = (uint8_t)g; gp.seed = (uint16_t)seed; gp.energy = (uint8_t)energy; gp.form = 0;
        for (k = 0; k < GP_N; k++) gp.amt[k] = GA_AUTO;
        gp.complex = 50; gp.swing = GA_AUTO; gp.human = 30;
        gen_go();
        printf("\n=== %s  seed %u  bpm %d  key %s %s  swing %d  len %u  amt D%u B%u C%u A%u L%u  ba %u  prog %u  pads %u\n",
               gs.g->name, seed, song.g[G_BPM], NN[gs.root % 12], N_SCALE[gs.scale], song.g[G_SWING], gs.len,
               gs.amt[0], gs.amt[1], gs.amt[2], gs.amt[3], gs.amt[4], gs.ba, (unsigned)(gs.prog - PROG), gs.g->pads);
        for (k = 0; k < NTRK; k++) {
            track_t *t = &trk[k];
            const engine_t *e = ENGINES[t->eng_req % NENGINES];
            uint32_t r = gs.role[k];
            if (!gs.act[r]) { printf("T%u %-4s off\n", k + 1u, RLN[r]); continue; }
            printf("T%u %u%-4s %-7s %-10s A%d D%d S%d R%d cut%d res%d fe%d lfo%d dist%d chor%d gl%d lvl%d rev%d dly%d gate%d hum%d\n",
                   k + 1u, r, RLN[r], e->name, t->preset < e->npresets ? e->presets[t->preset].name : "-",
                   t->p[P_ATK], t->p[P_DEC], t->p[P_SUS], t->p[P_REL], t->eng_req == 0 ? t->p[P_E4] : t->p[P_FCUT],
                   t->eng_req == 0 ? t->p[P_E5] : t->p[P_FRES], t->p[P_ED_FLT], t->p[P_LD_FLT], t->p[P_DIST], t->p[P_CHOR], t->p[P_GLIDE],
                   t->p[P_LEVEL], t->p[P_REV], t->p[P_DLY], t->p[P_SGATE], 127 - t->p[P_HVEL]);
            if (r == RL_DRUM) {
                static const char *LN[8] = {"BD", "SD", "CP", "CH", "OH", "TM", "RS", "CB"};
                for (l = 0; l < 8u; l++) {
                    int any = 0;
                    for (i = 0; i < steps && i < (uint32_t)t->p[P_SLEN]; i++) any |= t->step[i].time == ST_NOTE && (t->step[i].hit >> l) & 1u;
                    if (!any) continue;
                    printf("   %s ", LN[l]);
                    for (i = 0; i < steps && i < (uint32_t)t->p[P_SLEN]; i++) {
                        const step_t *s = &t->step[i];
                        int on = s->time == ST_NOTE && (s->hit >> l) & 1u;
                        if (i && i % 16u == 0u) putchar('|');
                        putchar(!on ? '.' : (s->acc >> l) & 1u ? 'X' : s->vel >= 90 ? 'x' : s->vel >= 60 ? 'o' : ',');
                    }
                    putchar('\n');
                }
            } else {
                printf("   ");
                for (i = 0; i < steps && i < (uint32_t)t->p[P_SLEN]; i++) {
                    const step_t *s = &t->step[i];
                    char b[8];
                    if (i && i % 16u == 0u) printf("| ");
                    if (s->time == ST_NOTE) {
                        nname(b, s->note[0]);
                        if (s->n > 1u) printf("%s+%u%s ", b, s->n - 1u, s->flags & SF_ACCENT ? "!" : s->flags & SF_SLIDE ? "~" : "");
                        else printf("%s%s%s ", b, s->flags & SF_ACCENT ? "!" : "", s->flags & SF_SLIDE ? "~" : "");
                        if (s->comp) printf("[c%u] ", s->comp);
                    } else if (s->time == ST_TIE) printf("= ");
                    else printf(". ");
                }
                putchar('\n');
            }
        }
    }
    return 0;
}
