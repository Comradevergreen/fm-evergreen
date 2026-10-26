# FM Evergreen: goals, rules, backlog

Status: **reviewed by the owner on 2026-10-27.** No open questions.

FM Evergreen is a fork of [JIANT FM](https://github.com/juanjiant-bit/JIANT) for the M-VAVE FM-1. JIANT is a fork of
Felucca and takes its song system from SLOOP.

## 0. Base

| | |
| --- | --- |
| On the owner's FM-1 | **0.9.9-par9**, `1502f55` = `upstream/port-p2` |
| This repo's `main` | `75a17b7` = `upstream/main`: the 0.9.18 line plus installer commits. It is **not** what the device runs |
| Split point | `6968b20` (0.9.9). `port-p2` has 133 commits since, `main` has 192, and 88 firmware files differ |
| Toolchain on this Mac | none yet (`~/.jieli/toolchain`, the AC79 SDK and `emcc` are missing). See [BUILDING.md](BUILDING.md). The host tests run without it: generate `build/gen` with `tools/build.py`'s `generate()` (needs Python's `pillow` and `fonttools`), then compile the tests with `cc` |
| Host tests on the base (2026-10-26, arm64 Mac) | Pass: `project_test`, `motion_test`, `ui_test`, `drumx_test`. **Fail before any change of ours:** (1) `regress`: the 12 STRING goldens are stale upstream (STRING's sound changed in par6, `8d99120` / `63b59cf`; `tests/golden.txt` was last updated in par1, `fd95915`); its CPU budget entries are x86 CI baselines and mean nothing on arm64. (2) `ratchet_test`: "the piano roll draws an x2 step in two parts", cause not yet looked at |

**Decided (2026-10-26):** Evergreen is based on `upstream/port-p2` (par9, `1502f55`), because it is what runs on the
device and what has been tested there. The work branch is `evergreen`. Pieces of 0.9.18 come in by cherry-pick, only
when needed.

## 1. Goals

1. **Lean.** Remove the randomize and generate features that do not help you play or edit (§3).
2. **UX first.** Every function sits where you would look for it. No extra screens, no long button combos, no odd routing.
3. **Sound design from scratch.** Make a synth patch in one obvious path: INIT, choose the engine, tweak, save.
4. **Every parameter explains itself.** One shared popup gives the full name, the value with its unit and one line on
   what the parameter does.
5. **One sound library.** Factory and user sounds live in one place. Any sound can be overwritten, or copied to a new
   slot.
6. **A full drum lane.** Each step of each drum instrument gets its own velocity and probability.
7. **Correct timing and data.** Fix the ENV page on DRUM-X and FM6, and the leftover p-locks.

## 2. Rules

### The reference

The [FM-1 Firmware Developers' Wiki](https://sbegueria.github.io/fm1-dev-wiki/) is **the** reference for hardware,
boot, flash, audio, performance and recovery. Read the relevant page before touching that area.

- If the code disagrees with the wiki, check the claim, then fix whichever is wrong.
- When a finding of ours settles one of the wiki's
  [Open Questions](https://sbegueria.github.io/fm1-dev-wiki/Open-Questions.html), contribute it back
  ([Contributing](https://sbegueria.github.io/fm1-dev-wiki/Contributing.html)).
- Tag hardware facts in comments and docs as the wiki does: **[HW] [SDK] [RE] [CODE] [?]**.

### UX

1. **One home per function.** Each function has one obvious place. A shortcut is allowed only if it is shown on screen.
2. **Shallow gestures.** At most one hold plus one press or turn. A held layer always shows its key map (as it does now).
3. **No riddles.** A short name on a card must have a full name and a description in the popup (goal 4).
4. **One meaning per gesture.** OCT+ always confirms, OCT− always cancels or goes back, SELECT always walks pages,
   everywhere.
5. **Information beats decoration.** While you touch a control, the middle of the screen is for that control.
   Creatures and animation give way.
6. **No lying controls.** Hide a parameter that does nothing on the current engine, and show it in the units it really
   uses. Today the ENV page shows ATK in ms on DRUM-X, which ignores it, and on FM6, where it is a rate offset.
7. **Destructive means confirm and undo.** Clearing, overwriting and initializing ask first, and UNDO can bring the
   result back.
8. **Remove before you add.** Flash is nearly full: par9 has 1128 B free. Every new feature says what it replaces or
   what it costs.

### Engineering

1. **Reproduce bugs first.** Show the bug in a host test (`tests/`) or the emulator before changing production code.
2. **Audio safety** (the wiki's [Programming Guide](https://sbegueria.github.io/fm1-dev-wiki/Programming-Guide.html)
   and [Performance](https://sbegueria.github.io/fm1-dev-wiki/Performance.html)):
   - measure the worst audio block, not the average;
   - no `double` math;
   - no flash access from the audio path; save only when stopped;
   - spread bursts of work over several blocks;
   - run slow things (envelopes, LFOs, smoothing) at control rate;
   - check the audio load with the busiest screen showing, because drawing shares the CPU with the audio.
3. **Formats: no migrations for now** (owner, 2026-10-27: there are no saved projects). Projects and presets from
   before may be dropped.
   - A change to `step_t` or to the preset layout still bumps the format version, so old data in flash is refused
     cleanly instead of being misread.
   - Revisit this rule before the first public release.
4. **Gates before release.**
   - `tests/run_tests.sh` passes before every build.
   - Every build is tried on the FM-1 before it is called stable.
   - Know the recovery path first: see the wiki's [Recovery](https://sbegueria.github.io/fm1-dev-wiki/Recovery.html) page.
5. **One change, one reason.** A removal, a UX change and a bug fix go in separate commits.

## 3. Random and generative inventory (decided 2026-10-27)

### A. Generators (they write music for you)

| # | Feature | How it's reached | Code | Decision |
| --- | --- | --- | --- | --- |
| A1 | **GEN**, the song generator: 43 genres, EXPLORE, RANDOM, seed, energy, form | HOME > GEN 1–3, HOME + SELECT | `gen.c` (2791 lines), `gen_tables.c`, `song_dice.c`; tests `gen_test.c`, `dice_metrics.c`, `gen_dump.c`; `tools/dice_render.c` | **Remove.** This is the biggest flash win |
| A2 | **DICE ALL**, a whole project rolled at once (and MENU > CONTROL > BPM LOCK, which only affects it) | from GEN | `gen.c dice_all` | **Remove** with A1 |
| A3 | **SEQ + SELECT**, the sequence dice | any SEQ page | `gen.c dice_seq` | **Remove** |
| A4 | SEQ TOOLS **RANDOM** (C4) and **COOK** (D4, moves 10–25 % of the hits or notes at random) | SEQ held | `ui_tools.c` | **Remove** |
| A5 | SEQ TOOLS lane **RND** (C5) | SEQ held, DRUM track | `ui_tools.c` | **Remove** |
| A6 | SEQ TOOLS **BEAT** (E4: four on the floor, backbeat, 8th hats). It is a template, not random | SEQ held, DRUM track | `ui_tools.c` | **Remove** |
| A7 | The **RAND** component action (each note moved up or down the scale at random) and the global COMPS lane's RANDOM | SEQ > COMP, GLO > COMPS | `seq.c CA_RAND` | **Remove**. The % conditions stay (D1) |
| A8 | **PLAYHEADS** (was FUGUE): 3 extra playheads with rate, direction (including RND) and interval | SEQ > PLAYHEADS | `fugue.c`, a SEQ page | **Keep** |
| A9 | VOICE **lyric dice**: W1–W16 phrases and the NAME screen's KNOB 3 | VOICE WORDS, NAME | `sing.c`, `ui_name.c` | **Keep** W1–W16 (a fixed phrase bank); **remove** the random throw |

### B. Sound dice

| # | Feature | How it's reached | Code | Decision |
| --- | --- | --- | --- | --- |
| B1 | **Sound and kit dice**: a random factory preset plus up to ±25 % on every parameter, with KEEP DICED? | EDIT + SELECT | `ui_input.c`, `ui_layer.c` | **Remove** |
| B2 | **LFO dice** | LFO + SELECT | same | **Remove** |
| B3 | **FX dice** | FX + SELECT | same | **Remove** |
| B4 | **Macro dice**: F3 rolls new macro routes | LFO held or latched | `mod.c`, `ui_layer.c` | **Remove** |

Removing B1–B4 also removes the KEEP DICED / BEFORE-NOW dialog and frees four `X + SELECT` combos.

### C. Self-changing music

| # | Feature | How it's reached | Code | Decision |
| --- | --- | --- | --- | --- |
| C1 | **MUTATE / MUTATE 2**: notes, rhythm, sound or a parameter change by themselves each bar. Comes with FREEZE (GLO + A4), REVERT (SEQ + D5), REVERT ALL (GLO + B4), KEEP and the "BACK TO BEFORE MUTATE?" notice | SEQ > MUTATE | `project.c mut_*`, the anchor | **Remove.** This frees 2 SEQ pages and 3 layer keys |

### D. Variation each time a step plays (performance)

| # | Feature | How it's reached | Decision |
| --- | --- | --- | --- |
| D1 | **Step probability** (`step_t.probability`). It exists, but **no page on the device edits it** (only the dice and the web editor). Component conditions also offer 25 / 50 / 75 % | none | **Keep and expose**, per lane on drums (§4.6) |
| D2 | Track **CHANCE**: scales every step's probability | SEQ > TRACK | **Keep** |
| D3 | **HUMAN**: random velocity, timing, length and envelope a little each pass | SEQ > TRACK | **Keep** (it is musical, and 0 % is off) |
| D4 | The ARP's **RND** order, and RND in step-record ARP | ARP, STEP | **Keep** (a standard arp mode) |
| D5 | Punch-in FX **F5 RANDOM** (random pitch while held) | FX held | **Remove**. The key gets something useful (§4.2) |
| D6 | Mod source **RAND** (per note), the LFO **S&H** wave, the NOISE source | LFO / MOD | **Keep** (standard modulation) |
| D7 | DRUM-X **X-MOD RAND** and **S&H** | EDIT > X-MOD | **Keep** (sound design) |

### E. Randomness inside the engines: keep

ANALOG DRIFT, NOISE DRIFT, ANIMAL CHAOS, VOICE jitter / shimmer / RAND, WING DRIFT, LOFI BYTE VAR. These are timbre
parameters, not features.

### F. Decoration: UX review, not a removal

HOME's ECO and BEING pages, with the creatures, granules and faces. Once GEN is gone, HOME is ECO · MACROS · BEING ·
ROUTES. Whether HOME should become the information screen instead (rule UX 5) is decided in §4.2.

## 4. Backlog

All code refs are to `upstream/port-p2` (par9).

### 4.1 Remove the random and generative features

The rows marked Remove in §3. Each removal:

- deletes its code, its pages, the combos that reach it and its tests;
- needs no migration (Eng 3): its project fields are dropped together with the format version bump;
- updates `docs/MANUAL.md`;
- reports the flash it freed.

### 4.2 UX rules audit

Walk every section's chain (`tools/gen_nav_map.py` draws it) and list each place that breaks a rule in §2. Fix them in
priority order. This includes:

- what to give the freed keys and combos: punch-in F5, the four `X + SELECT` combos, GLO A4 / B4, SEQ D5;
- HOME's role (§3 F).

### 4.3 Sound design from scratch

What exists today:

- **INIT** is hidden: hold EDIT and press F3. It resets the track's engine to its defaults, dry.
- **The engine** is picked on **SAVE > PRESETS (KNOB 2 ENG)** or by browsing presets with the PRESETS encoder. Picking
  the engine you play in the SAVE section is a routing problem.
- There is no "new sound" flow.

Plan:

1. An EDIT page **ENGINE** (or EDIT's first card): choose an engine and get it INIT'd.
2. INIT is visible on the EDIT page, not only in the hold layer.
3. Then save into the sound library (§4.8).

### 4.4 Parameter popup, after a UI architecture study

**First a study** (owner, 2026-10-27): could a redesigned UI architecture make the popup, and the UI as a whole, cheaper
in flash and code? Study the current drawing and page code (`ui_*.c`, `params.c` pages, `gfx.c`). Write down the
options and their cost, then decide before building the popup.

The popup (inspiration: Deluge community firmware). When a knob turns, a card shows over the middle of the screen for
about 1.2 s after the last turn, with:

- the full name (PM.TYPE becomes PHASE-MOD TYPE);
- the value, large, with its unit;
- a range bar;
- one plain line on what it does.

The same component carries the messages, confirmations and save results, so there is one popup style. MENU > DISPLAY
gets HINTS ON / OFF.

Cost: there are 95 core parameters (`params.c`), plus each engine's own labels (`ui_dx.c`, the EDIT pages), at roughly
32–48 B of text each, so **several KB of flash**. It needs §4.1 first.

### 4.5 ENV page on DRUM-X and FM6

The owner's report is about DRUM-X's and FM6's behaviour (2026-10-27).

What the code does:

- **DRUM-X** ignores the ADSR (`eng_drum.c drum_amp`): the ENV page does nothing there.
- **FM6**: the ENV page's ATK, DEC and REL do not set times. They slow the carriers' DX rates, for example R1 by
  `ATK*90/127` (`eng_fm6.c:355`), while the page shows ms.
- **STRING** also uses its own envelope (`ownenv`).

The main ADSR on the other engines is fine:

- `voice.c env_tick` is a linear ramp at control rate (64 samples, 1.45 ms), interpolated within each block, from a
  1 ms–10 s exponential table (`tools/gen_tables.py`).
- The displayed ms match the real time (2 ms → 2.9 ms; 104 ms → 104.5 ms; 10 s → 10 s).
- Anything under ~1.5 ms is one control tick.
- Smaller points to check later: retrigger and legato start the attack from the current level (`voice.c:872`), and a
  step shorter than the attack releases before the peak.

Decided (owner, 2026-10-27):

- **DRUM-X:** hide the ENV page. Each drum sound has its own DECAY.
- **FM6:** show ATK, DEC and REL as what they are, offsets on the carriers (for example "+40 SLOWER"), not ms.
- **STRING:** check it the same way and apply the same rule (UX 6).

### 4.6 Drum lane velocity and probability

Today a step stores one `vel` and one `probability` for all 8 lanes, plus an accent bit per lane (`core.h step_t`).

Plan:

- per-lane velocity and probability (one possible layout: 4 bits each, 8 B a step);
- edited on the grid by holding a step and turning a knob, in the same gesture as p-locks;
- shown as bar height and dimming on the grid.

Cost:

- RAM is NSTEP 64 × 6 tracks × 8 B = 3 KB if every track gets it. A drum-only side table costs less.
- The format version goes up; there is no migration (Eng 3).

### 4.7 Drum steps keep their p-locks (root cause found, not yet reproduced)

The cause: `ui.c grid_hit()` (about line 1090) calls `step_clear(s)` when a step's last hit is removed, but never
`motion_clear_locks(t, i)`. The locks stay in the shared motion store and come back when the step is hit again.

Fix:

1. Write a failing regression test (`tests/motion_test.c` or `tests/ui_test.c`).
2. Clear the step's locks when the step becomes empty.
3. Check every other path that empties a step: `ui.c:1303/1323`, `ui_tools.c`, live record.

Note: locks are per step, not per lane, and the whole section shares 64 records.

### 4.8 One sound library

Today the sounds are split:

- SAVE > USER holds 32 user slots (`UP_SLOTS`);
- SAVE > PRESETS holds the factory sounds, which are constant tables in the firmware (`eng_*.c *_PRESETS`);
- the PRESETS encoder lists factory first, then user;
- a user preset also stores a 16-step pattern;
- a DRUM-X kit cannot be saved as a preset (it lives in project sections).

Decided (owner, 2026-10-27):

- **One library**, browsed in one place.
- **Every sound can be overwritten**, factory ones included, just like user ones.
- **SAVE** overwrites the sound in its slot. **COPY TO NEW SLOT** stores the sound being edited in a new slot, with
  NAME.
- Kits are saved like sounds.
- Sound presets no longer store a pattern.

How: the factory tables stay in the firmware. Overwriting a factory sound stores your copy in flash, which takes that
slot's place. The flash space this needs is measured once the build works.

**RESTORE FACTORY** for an overwritten factory slot (owner, 2026-10-27): yes, if the overhead is small. It is expected
to be small: it deletes the stored copy, as the user slots' ERASE does today. Drop it if it turns out to cost real
flash or code.

## 5. Work order

1. **Build setup** (~30 min):
   - start Docker (BUILDING.md: the JieLi toolchain is Linux x86-64 and runs in a container);
   - `tools/get_toolchain.sh`;
   - clone the AC79 SDK (BUILDING.md).

   This is needed to build, to measure flash and to make a package to install on the FM-1.
2. **P-lock bug** (§4.7): first a failing test, then the fix.
3. **Removals** (§4.1): one commit per group (A, B, C, D5), each reporting the flash freed. Then try a build on the FM-1.
4. **ENV on DRUM-X and FM6** (§4.5), **sound from scratch** (§4.3), **one library**
   (§4.8).
5. **Drum lane velocity and probability** (§4.6), then the **UI architecture study** and the **popup** (§4.4), then
   the **UX audit** (§4.2).
