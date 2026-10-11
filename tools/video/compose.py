# SPDX-License-Identifier: GPL-3.0-only
# (JIANT 0.6.7) the presentation video: tools/video/capture.c's footage (the FM-1's screen at 30 fps and its audio)
# composed at 1920 x 1080 in the project's look (black, violet to orange, Chakra Petch), scene titles timed to the
# capture's script (its scenes.txt), a glitch at each cut, the screen's glow pulsing with the audio. Pipes the frames to
# ffmpeg.
#   python3 tools/video/compose.py build/video          -> build/video/jiant-fm.mp4
import subprocess, sys, wave
from pathlib import Path
import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont

ROOT = Path(__file__).resolve().parents[2]
D = Path(sys.argv[1] if len(sys.argv) > 1 else ROOT / 'build/video')
W, H, FPS = 1920, 1080, 30
SC = 4                                             # 240 -> 960, nearest: the pixels stay sharp
SX, SY = W - 960 - 80, 60                          # the screen's place after the intro
VIOLET, ORANGE, WHITE, DIM = (184, 112, 255), (255, 122, 0), (240, 236, 248), (120, 110, 140)
FONT = ROOT / 'assets/fonts'
def font(w, s): return ImageFont.truetype(str(FONT / f'ChakraPetch-{w}.ttf'), s)
F_BIG, F_MID, F_SUB, F_SMALL = font('Bold', 92), font('SemiBold', 54), font('Medium', 34), font('Medium', 24)

# the captions, from the capture's script (scenes.txt: start s, tag, title lines, subtitle lines; '|' splits lines;
# an empty title ends the captions: the outro)
_hdr, *_rows = (D / 'scenes.txt').read_text().splitlines()
SPLASH = float(_hdr.split('splash ')[1].split(' s')[0])
END = float(_hdr.split('end ')[1].split(' s')[0])
N = int(END * FPS)
_cap = []
for r in _rows:
    t0, tg, ti, su = (r.split('\t') + ['', '', ''])[:4]
    _cap.append((float(t0), tg, [x for x in ti.split('|') if x], [x for x in su.split('|') if x]))
OUT = next(c[0] for c in _cap if not c[2])         # the outro: from the first empty caption
SCENES = []                                        # (start, end, tag, title, subtitle, same title as before)
for i, (t0, tg, ti, su) in enumerate(_cap):
    if not ti: break
    t1 = _cap[i + 1][0] if i + 1 < len(_cap) else OUT
    SCENES.append((t0, t1, tg, ti, su, i > 0 and _cap[i - 1][2] == ti))
CUTS = [SPLASH] + [sc[0] for sc in SCENES] + [OUT]

def grad(w, h):                                    # violet -> orange, left to right
    t = np.linspace(0, 1, w)[None, :, None]
    g = np.array(VIOLET)[None, None] * (1 - t) + np.array(ORANGE)[None, None] * t
    return np.repeat(g, h, 0).astype(np.uint8)

def text_layer(lines, f, color, gap=1.05, gradient=False):
    asc, desc = f.getmetrics()
    lh = int((asc + desc) * gap)
    wmax = max(int(f.getlength(l)) for l in lines) + 8
    m = Image.new('L', (wmax, lh * len(lines) + 8))
    d = ImageDraw.Draw(m)
    for i, l in enumerate(lines): d.text((0, i * lh), l, font=f, fill=255)
    rgb = Image.fromarray(grad(*m.size)) if gradient else Image.new('RGB', m.size, color)
    out = Image.new('RGBA', m.size); out.paste(rgb, (0, 0), m)
    return out

def ease(x): x = min(max(x, 0.0), 1.0); return 1 - (1 - x) ** 3

# --- the footage and the audio's loudness a frame (the glow's pulse)
scr = np.memmap(D / 'screen.rgb', dtype=np.uint8, mode='r').reshape(-1, 240, 240, 3)
with wave.open(str(D / 'audio.wav')) as w:
    a = np.frombuffer(w.readframes(w.getnframes()), dtype=np.int16).reshape(-1, 2).astype(np.float32)
spf = 44100 // FPS
rms = np.array([np.sqrt((a[i * spf:(i + 1) * spf] ** 2).mean() + 1) for i in range(N)])
pulse = np.clip((rms - np.percentile(rms, 20)) / (np.percentile(rms, 99) - np.percentile(rms, 20)), 0, 1)
env = np.zeros(N)                                  # fast attack, slow release
for i in range(N): env[i] = max(pulse[i], env[i - 1] * 0.86 if i else 0)

# --- the static layers
bg = np.zeros((H, W, 3), np.float32)
bg[:, :] = (6, 4, 10)
bg[::48, :] += 7; bg[:, ::48] += 7                 # a faint grid
yy, xx = np.mgrid[0:H, 0:W]
vign = np.clip(1.2 - np.hypot((xx - W / 2) / W, (yy - H / 2) / H) * 1.6, 0.25, 1)[..., None]
bg *= vign
glow_m = Image.new('L', (960 + 240, 960 + 240))
ImageDraw.Draw(glow_m).rounded_rectangle((120, 120, 120 + 960, 120 + 960), 28, fill=255)
glow_m = np.asarray(glow_m.filter(ImageFilter.GaussianBlur(46)), np.float32)[..., None] / 255
glow_c = glow_m * np.array(VIOLET, np.float32) * 0.55 + glow_m ** 3 * np.array(ORANGE, np.float32) * 0.25
sig = Image.open(ROOT / 'docs/cheatsheet/signature.png').convert('RGB')

brand = text_layer(['JIANT FM'], F_SMALL, WHITE)
tag = text_layer(['M-VAVE FM-1 · v0.6.10'], F_SMALL, DIM)
LAYERS = [(text_layer([sc[2] or ' '], F_SMALL, ORANGE), text_layer(sc[3], F_BIG, WHITE, 0.98, gradient=True),
           text_layer(sc[4] or [' '], F_SUB, WHITE, 1.3)) for sc in SCENES]
intro_t = text_layer(['JIANT FM'], font('Bold', 150), WHITE, gradient=True)
intro_s = text_layer(['alternative firmware for the M-VAVE FM-1'], F_SUB, WHITE)
out_l = [text_layer(['JIANT FM'], font('Bold', 120), WHITE, gradient=True),
         text_layer(['try it in your browser'], F_MID, WHITE),
         text_layer(['juanjiant-bit.github.io/JIANT'], F_SUB, ORANGE),
         text_layer(['emulator · web installer · editor · cheat sheets'], F_SMALL, DIM)]

def paste(img, layer, x, y, alpha=1.0, reveal=1.0):
    if alpha <= 0 or reveal <= 0: return
    L = layer
    if reveal < 1: L = L.crop((0, 0, max(1, int(L.width * reveal)), L.height))
    if alpha < 1:
        L = L.copy(); L.putalpha(L.getchannel('A').point(lambda v: int(v * alpha)))
    img.alpha_composite(L, (int(x), int(y)))

def glitch(arr, x0, y0, w, h, amt, rng):
    reg = arr[y0:y0 + h, x0:x0 + w]
    for _ in range(int(10 * amt) + 2):
        y = rng.integers(0, h - 8); bh = rng.integers(4, 60)
        reg[y:y + bh] = np.roll(reg[y:y + bh], rng.integers(-60, 60) * amt, axis=1)
    sh = int(14 * amt)
    reg[..., 0] = np.roll(reg[..., 0], sh, axis=1); reg[..., 2] = np.roll(reg[..., 2], -sh, axis=1)

PREVIEW = [int(x) for x in sys.argv[2].split(',')] if len(sys.argv) > 2 else None   # frame numbers -> PNGs
ff = None if PREVIEW else subprocess.Popen(['ffmpeg', '-y', '-loglevel', 'error', '-f', 'rawvideo', '-pix_fmt', 'rgb24', '-s', f'{W}x{H}',
                       '-r', str(FPS), '-i', '-', '-i', str(D / 'audio.wav'),
                       '-af', f'afade=t=in:d=0.3,afade=t=out:st={END - 1.6:.2f}:d=1.6',
                       '-c:v', 'libx264', '-preset', 'slow', '-crf', '18', '-pix_fmt', 'yuv420p',
                       '-c:a', 'aac', '-b:a', '256k', '-movflags', '+faststart', '-shortest',
                       str(D / 'jiant-fm.mp4')], stdin=subprocess.PIPE)
rng = np.random.default_rng(7)
for f in (PREVIEW or range(N)):
    t = f / FPS
    # the screen's place: centred during the splash, sliding right as the intro title comes, back left for the outro
    k = ease((t - (SPLASH - 1.2)) / 1.0)
    ko = ease((t - OUT) / 0.9)
    sx = int((W - 960) / 2 * (1 - k) + SX * k)
    sx = int(sx * (1 - ko) + 120 * ko)
    sy = int(60 * (1 - k) + SY * k)
    fr = bg.copy()
    g = 0.35 + 0.9 * env[f]
    gx, gy = sx - 120, sy - 120
    y0, y1, x0, x1 = max(gy, 0), min(gy + 1200, H), max(gx, 0), min(gx + 1200, W)
    fr[y0:y1, x0:x1] += glow_c[y0 - gy:y1 - gy, x0 - gx:x1 - gx] * g
    fr[sy:sy + 960, sx:sx + 960] = np.repeat(np.repeat(scr[f], SC, 0), SC, 1)
    arr = np.clip(fr, 0, 255).astype(np.uint8)
    # a glitch on the screen for a few frames at each cut (and a light one on strong beats)
    dc = min(abs(t - c) for c in CUTS)
    if dc < 0.17: glitch(arr, sx, sy, 960, 960, 1 - dc / 0.17, rng)
    elif pulse[f] > 0.97 and rng.random() < 0.25: glitch(arr, sx, sy, 960, 960, 0.25, rng)
    img = Image.fromarray(arr).convert('RGBA')
    # frame chrome: brand, tag, progress line
    if SPLASH - 0.6 < t < OUT + 0.2:
        a_ = min(ease((t - SPLASH + 0.6) / 0.5), 1 - ease((t - OUT + 0.3) / 0.5))
        paste(img, brand, 80, 50, a_); paste(img, tag, SX - 40 - tag.width, H - 62, a_ * 0.9)
    pw = int(W * (f + 1) / N)
    img.paste(Image.fromarray(grad(W, 4)[:, :pw]), (0, H - 4))
    # intro title
    t_in = SPLASH - 1.1                            # the intro title, until the first scene
    if t_in < t < SCENES[0][0]:
        a_ = ease((t - t_in - 0.1) / 0.4) * (1 - ease((t - SCENES[0][0] + 0.4) / 0.35))
        paste(img, intro_t, 80, 340, a_, ease((t - t_in) / 0.5))
        paste(img, intro_s, 86, 540, a_ * 0.9, ease((t - t_in - 0.4) / 0.6))
    # scene titles: quick ones (the montage) come in faster; the same title again only changes the subtitle
    for i, (s0, s1, tg, _, _, same) in enumerate(SCENES):
        if s0 <= t < s1:
            num, ti, su = LAYERS[i]
            d = min(0.45, (s1 - s0) * 0.35)
            nxt_same = i + 1 < len(SCENES) and SCENES[i + 1][5]
            a_ = 1.0 if nxt_same else 1 - ease((t - (s1 - d * 0.7)) / (d * 0.6))
            e = 1.0 if same else ease((t - s0) / d)
            dx = -40 * (1 - e)
            paste(img, num, 80, 300, a_ * e)
            rw = int(560 * (1.0 if same else ease((t - s0 - 0.05) / d)))
            if rw > 0 and a_ > 0: img.alpha_composite(Image.fromarray(grad(rw, 5)).convert('RGBA'), (80, 345))
            paste(img, ti, 80 + dx, 372, a_, 1.0 if same else ease((t - s0 - 0.05) / d))
            sa = ease((t - s0) / 0.25) if same else 1.0
            paste(img, su, 84 + dx * 0.5, 380 + ti.height + 24, a_ * 0.92 * sa, ease((t - s0 - (0.1 if same else d)) / 0.7))
    # outro
    if t >= OUT:
        ox = 1180
        a_ = ease((t - OUT - 0.4) / 0.5)
        sg = Image.fromarray(np.minimum(np.asarray(sig, np.float32) * a_, 255).astype(np.uint8))
        base = np.asarray(img.convert('RGB'))[120:120 + sg.height, ox:ox + sg.width]
        img.paste(Image.fromarray(np.maximum(base, np.asarray(sg))), (ox, 120))
        y = 120 + sig.height + 30
        for j, L in enumerate(out_l):
            paste(img, L, ox, y, ease((t - OUT - 0.7 - j * 0.18) / 0.4), ease((t - OUT - 0.7 - j * 0.18) / 0.5))
            y += L.height + (18 if j else 4)
    out = np.asarray(img.convert('RGB'))
    if t > END - 1.0: out = (out * (1 - ease((t - END + 1.0) / 0.95))).astype(np.uint8)
    if PREVIEW: Image.fromarray(out).save(D / f'preview-{f:04d}.png'); continue
    ff.stdin.write(out.tobytes())
    if f % 300 == 0: print(f'{f}/{N}', flush=True)
if ff: ff.stdin.close(); ff.wait(); print(D / 'jiant-fm.mp4')
