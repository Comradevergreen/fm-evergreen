# FM Evergreen: goals, rules, backlog

Status: **draft**, 2026-10-26. Rows marked **DECIDE** wait for the owner.

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

1. **Lean.** Remove the randomize and generate features that do not help you play or edit (inventory in §3).
2. **UX first.** Every function sits where you would look for it. No extra screens, no long button combos, no odd routing.
3. **Sound design from scratch.** Make a synth patch in one obvious path: INIT, choose the engine, tweak, save.
4. **Every parameter explains itself.** One shared popup gives the full name, the value with its unit and one line on
   what the parameter does.
5. **One sound library.** Factory and user sounds are browsed and saved in one place.
6. **A full drum lane.** Each step of each drum instrument gets its own velocity and probability.
7. **Correct timing and data.** Fix the ENV attack and the leftover p-locks.

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
3. **Formats stay compatible.** Old projects and presets keep loading. A change to `step_t` or to the preset layout
   needs a format version bump, a migration and a test (`tests/project_test.c`, `tests/upreset_test.c`).
4. **Gates before release.**
   - `tests/run_tests.sh` passes before every build.
   - Every build is tried on the FM-1 before it is called stable.
   - Know the recovery path first: see the wiki's [Recovery](https://sbegueria.github.io/fm1-dev-wiki/Recovery.html) page.
5. **One change, one reason.** A removal, a UX change and a bug fix go in separate commits.

## 3. Random and generative inventory (DECIDE per row)

The proposals are a first pass. Mark each row Remove, Keep or Rework.

### A. Generators (they write music for you)

| # | Feature | How it's reached | Code | Proposal |
| --- | --- | --- | --- | --- |
| A1 | **GEN**, the song generator: 43 genres, EXPLORE, RANDOM, seed, energy, form | HOME > GEN 1–3, HOME + SELECT | `gen.c` (2791 lines), `gen_tables.c`, `song_dice.c`; tests `gen_test.c`, `dice_metrics.c`, `gen_dump.c`; `tools/dice_render.c` | **Remove.** This is the biggest flash win |
| A2 | **DICE ALL**, a whole project rolled at once (and MENU > CONTROL > BPM LOCK, which only affects it) | from GEN | `gen.c dice_all` | **Remove** with A1 |
| A3 | **SEQ + SELECT**, the sequence dice | any SEQ page | `gen.c dice_seq` | **Remove** |
| A4 | SEQ TOOLS **RANDOM** (C4) and **COOK** (D4, moves 10–25 % of the hits or notes at random) | SEQ held | `ui_tools.c` | **Remove** |
| A5 | SEQ TOOLS lane **RND** (C5) | SEQ held, DRUM track | `ui_tools.c` | **Remove** |
| A6 | SEQ TOOLS **BEAT** (E4: four on the floor, backbeat, 8th hats). It is a template, not random | SEQ held, DRUM track | `ui_tools.c` | DECIDE. Proposed: remove |
| A7 | The **RAND** component action (each note moved up or down the scale at random) and the global COMPS lane's RANDOM | SEQ > COMP, GLO > COMPS | `seq.c CA_RAND` | **Remove**. Keep the % conditions (see D1) |
| A8 | **PLAYHEADS** (was FUGUE): 3 extra playheads with rate, direction (including RND) and interval | SEQ > PLAYHEADS | `fugue.c`, a SEQ page | DECIDE. Proposed: remove (it is generative sequencing) |
| A9 | VOICE **lyric dice**: W1–W16 phrases and the NAME screen's KNOB 3 | VOICE WORDS, NAME | `sing.c`, `ui_name.c` | DECIDE. Proposed: keep W1–W16 (a fixed phrase bank), remove the random throw |

### B. Sound dice

| # | Feature | How it's reached | Code | Proposal |
| --- | --- | --- | --- | --- |
| B1 | **Sound and kit dice**: a random factory preset plus up to ±25 % on every parameter, with KEEP DICED? | EDIT + SELECT | `ui_input.c`, `ui_layer.c` | **Remove** |
| B2 | **LFO dice** | LFO + SELECT | same | **Remove** |
| B3 | **FX dice** | FX + SELECT | same | **Remove** |
| B4 | **Macro dice**: F3 rolls new macro routes | LFO held or latched | `mod.c`, `ui_layer.c` | **Remove** |

Removing B1–B4 also removes the KEEP DICED / BEFORE-NOW dialog and frees four `X + SELECT` combos.

### C. Self-changing music

| # | Feature | How it's reached | Code | Proposal |
| --- | --- | --- | --- | --- |
| C1 | **MUTATE / MUTATE 2**: notes, rhythm, sound or a parameter change by themselves each bar. Comes with FREEZE (GLO + A4), REVERT (SEQ + D5), REVERT ALL (GLO + B4), KEEP and the "BACK TO BEFORE MUTATE?" notice | SEQ > MUTATE | `project.c mut_*`, the anchor | **Remove.** This frees 2 SEQ pages and 3 layer keys |

### D. Variation each time a step plays (performance)

| # | Feature | How it's reached | Proposal |
| --- | --- | --- | --- |
| D1 | **Step probability** (`step_t.probability`). It exists, but **no page on the device edits it** (only the dice and the web editor). Component conditions also offer 25 / 50 / 75 % | none | **Keep and expose**, per lane on drums (§4.6) |
| D2 | Track **CHANCE**: scales every step's probability | SEQ > TRACK | Keep |
| D3 | **HUMAN**: random velocity, timing, length and envelope a little each pass | SEQ > TRACK | DECIDE. Proposed: keep (it is musical, and 0 % is off) |
| D4 | The ARP's **RND** order, and RND in step-record ARP | ARP, STEP | Keep (a standard arp mode) |
| D5 | Punch-in FX **F5 RANDOM** (random pitch while held) | FX held | DECIDE. Proposed: remove and give the key to something useful |
| D6 | Mod source **RAND** (per note), the LFO **S&H** wave, the NOISE source | LFO / MOD | Keep (standard modulation) |
| D7 | DRUM-X **X-MOD RAND** and **S&H** | EDIT > X-MOD | Keep (sound design) |

### E. Randomness inside the engines (keep)

ANALOG DRIFT, NOISE DRIFT, ANIMAL CHAOS, VOICE jitter / shimmer / RAND, WING DRIFT, LOFI BYTE VAR. These are timbre
parameters, not features.

### F. Decoration (UX review, not a removal)

HOME's ECO and BEING pages, with the creatures, granules and faces. Once GEN is gone, HOME is ECO · MACROS · BEING ·
ROUTES. **DECIDE** later whether HOME should be the information screen instead (rule UX 5).

## 4. Backlog: findings so far

All code refs are to `upstream/port-p2` (par9).

### 4.1 Remove the random and generative features

The order follows §3 once it is decided. Each removal has to:

- delete its code, its pages and the combos that reach it;
- turn its project fields into no-ops so old projects still load;
- update `docs/MANUAL.md`;
- report the flash it freed.

### 4.2 UX rules audit

Walk every section's chain (`tools/gen_nav_map.py` draws it) and list each place that breaks a rule in §2. Fix them in
priority order.

### 4.3 Sound design from scratch

What exists today:

- **INIT** is hidden: hold EDIT and press F3. It resets the track's engine to its defaults, dry.
- **The engine** is picked on **SAVE > PRESETS (KNOB 2 ENG)** or by browsing presets with the PRESETS encoder. Picking
  the engine you play in the SAVE section is a routing problem.
- There is no "new sound" flow.

Proposal (DECIDE):

- An EDIT page **ENGINE** (or EDIT's first card): choose an engine and get it INIT'd.
- INIT also visible on the EDIT page, not only in the hold layer.
- Then SAVE AS from the sound library (4.8).

### 4.4 The parameter popup (inspiration: Deluge community firmware)

When a knob turns, show a card over the middle of the screen for about 1.2 s after the last turn:

- the full name (PM.TYPE becomes PHASE-MOD TYPE);
- the value, large, with its unit;
- a range bar;
- one plain line on what it does.

The same component carries the messages, confirmations and save results, so there is one popup style. MENU > DISPLAY
gets HINTS ON / OFF.

Cost: there are 95 core parameters (`params.c`), plus each engine's own labels (`ui_dx.c`, the EDIT pages), at roughly
32–48 B of text each, so **several KB of flash**. It needs 4.1 first.

### 4.5 ENV attack: suspects (not reproduced yet)

Checked:

- The main ADSR (`voice.c env_tick`) is a linear ramp at control rate (64 samples, 1.45 ms). It is interpolated within
  each block, from a 1 ms–10 s exponential table (`tools/gen_tables.py`).
- The displayed ms match the real time (2 ms → 2.9 ms; 104 ms → 104.5 ms; 10 s → 10 s).
- Anything under ~1.5 ms is one control tick.

Suspects:

1. **FM6:** the ENV page's ATK does not set a time. It slows the carriers' DX rate R1 by `ATK*90/127`
   (`eng_fm6.c:355`), while the page shows ms.
2. **DRUM-X** ignores the ADSR (`eng_drum.c drum_amp`). **STRING** and **FM6** use their own envelopes (`ownenv`).
3. **Retrigger and legato** start the attack from the current level (`voice.c:872`), so it is shorter than set.
4. **Short gates:** a step shorter than the attack releases before the peak.
5. **Linear amplitude**, where most synths use a curved (convex) attack, so the onset can feel late or soft.

**Owner question:** which engine, and is it played from the keys or from SEQ? Then capture it with a `voice_test.c`
case or an emulator recording.

### 4.6 Drum lane velocity and probability

Today a step stores one `vel` and one `probability` for all 8 lanes, plus an accent bit per lane (`core.h step_t`).

Proposal:

- per-lane velocity and probability (one possible layout: 4 bits each, 8 B a step);
- edited on the grid by holding a step and turning a knob, in the same gesture as p-locks;
- shown as bar height and dimming on the grid.

Cost:

- RAM is NSTEP 64 × 6 tracks × 8 B = 3 KB if every track gets it. A drum-only side table costs less.
- It changes the project format (rule Eng 3).

### 4.7 Drum steps keep their p-locks (root cause found, not yet reproduced)

The cause: `ui.c grid_hit()` (about line 1090) calls `step_clear(s)` when a step's last hit is removed, but never
`motion_clear_locks(t, i)`. The locks stay in the shared motion store and come back when the step is hit again.

Fix:

1. Clear the step's locks when the step becomes empty.
2. Check every other path that empties a step: `ui.c:1303/1323`, `ui_tools.c`, live record.
3. Add a regression test (`tests/motion_test.c`).

Note: locks are per step, not per lane, and the whole section shares 64 records.

### 4.8 One sound library

Today the sounds are split:

- SAVE > USER holds 32 user slots;
- SAVE > PRESETS holds the factory sounds;
- the PRESETS encoder lists factory first, then user;
- a user preset also stores a 16-step pattern;
- a DRUM-X kit cannot be saved as a preset (it lives in project sections).

Proposal (DECIDE):

- One **LIBRARY** browser per engine. Factory sounds are read-only and marked; user sounds sit beside them.
- **SAVE** = overwrite if it's a user sound, **SAVE AS** = new slot plus NAME.
- Drop the pattern from sound presets.
- Let kits be saved like sounds.

### 4.9 Not reachable

The [Reddit thread](https://www.reddit.com/r/MVaveFM1/comments/1xe8c9x/all_right_stop_collaborate_and_listen/) blocks
automated fetching. The wiki grew out of it, and its notes are used above. To use the thread itself, paste its key points
here.
