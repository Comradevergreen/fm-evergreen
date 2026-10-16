// SPDX-License-Identifier: GPL-3.0-only
// JIANT Probe: phase 0 of the stems app (android/probe/README.md). One screen that answers, on a given phone, the
// questions the app depends on: does Android open the FM-1's eight USB channels (MENU > SYSTEM > USB > STEMS) at
// 44.1 kHz; can it open a USB microphone at the same time as the FM-1; can it play into the FM-1 (USB > IN+OUT); does
// it see the FM-1's USB-MIDI. Everything it finds goes into a plain-text report to copy or share, and the 8-channel
// test is saved as a WAV in Downloads/JIANT.
package fm.jiant.probe;

import android.Manifest;
import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.Typeface;
import android.media.AudioDeviceInfo;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.media.midi.MidiDeviceInfo;
import android.media.midi.MidiManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.provider.MediaStore;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.List;
import java.util.Locale;

public class MainActivity extends Activity {
    private static final int RATE = 44100;
    private static final int BG = 0xFF07070A, FG = 0xFFF4F1EA, DIM = 0xFF8B8697, ORANGE = 0xFFFF7A00, VIOLET = 0xFFB870FF;

    private TextView log;
    private final StringBuilder report = new StringBuilder();
    private final Handler ui = new Handler(Looper.getMainLooper());
    private AudioManager am;
    private volatile boolean busy;

    @Override
    protected void onCreate(Bundle b) {
        super.onCreate(b);
        am = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().setStatusBarColor(BG);

        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setBackgroundColor(BG);
        int pad = dp(16);
        root.setPadding(pad, dp(28), pad, pad);

        TextView title = new TextView(this);
        title.setText("JIANT PROBE");
        title.setTextColor(FG);
        title.setTextSize(24);
        title.setTypeface(Typeface.create("sans-serif-medium", Typeface.BOLD));
        root.addView(title);
        TextView sub = new TextView(this);
        sub.setText("Fase 0 de la app de stems. Enchufá el FM-1 por USB-C, poné MENU › SYSTEM › USB › STEMS y tocá los botones en orden.");
        sub.setTextColor(DIM);
        sub.setTextSize(14);
        sub.setPadding(0, dp(4), 0, dp(12));
        root.addView(sub);

        root.addView(button("1 · DISPOSITIVOS", v -> run(this::listDevices)));
        root.addView(button("2 · GRABAR 10 s DE LOS 8 CANALES", v -> run(this::recordStems)));
        root.addView(button("3 · FM-1 + MIC USB A LA VEZ (3 s)", v -> run(this::twoInputs)));
        root.addView(button("4 · ENVIAR EL MIC AL FM-1 (5 s, modo IN+OUT)", v -> run(this::sendMic)));
        LinearLayout row = new LinearLayout(this);
        row.setOrientation(LinearLayout.HORIZONTAL);
        Button copy = button("COPIAR INFORME", v -> copyReport());
        Button share = button("COMPARTIR", v -> shareReport());
        row.addView(copy, new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1));
        row.addView(share, new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1));
        root.addView(row);

        log = new TextView(this);
        log.setTextColor(FG);
        log.setTextSize(12);
        log.setTypeface(Typeface.MONOSPACE);
        log.setTextIsSelectable(true);
        log.setPadding(0, dp(12), 0, dp(24));
        ScrollView sv = new ScrollView(this);
        sv.addView(log);
        root.addView(sv, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, 0, 1));
        setContentView(root);

        say("JIANT Probe 0.1 · " + Build.MANUFACTURER + " " + Build.MODEL + " · Android " + Build.VERSION.RELEASE
            + " (API " + Build.VERSION.SDK_INT + ")");
        if (checkSelfPermission(Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED)
            requestPermissions(new String[] {Manifest.permission.RECORD_AUDIO}, 1);
    }

    // ------------------------------------------------------------------ UI helpers
    private int dp(int v) { return Math.round(v * getResources().getDisplayMetrics().density); }

    private Button button(String text, View.OnClickListener l) {
        Button bt = new Button(this);
        bt.setText(text);
        bt.setAllCaps(false);
        bt.setTextColor(FG);
        bt.setBackgroundColor(0xFF1A1820);
        bt.setOnClickListener(l);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,
                                                                     LinearLayout.LayoutParams.WRAP_CONTENT);
        lp.setMargins(0, dp(3), dp(3), dp(3));
        bt.setLayoutParams(lp);
        return bt;
    }

    private void say(String s) {
        report.append(s).append('\n');
        ui.post(() -> log.setText(report.toString()));
    }

    private void run(Runnable r) {
        if (busy) { say("(esperá a que termine la prueba anterior)"); return; }
        if (checkSelfPermission(Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            say("Falta el permiso de micrófono: aceptalo y volvé a tocar el botón.");
            requestPermissions(new String[] {Manifest.permission.RECORD_AUDIO}, 1);
            return;
        }
        busy = true;
        new Thread(() -> {
            try { r.run(); } catch (Throwable t) { say("ERROR: " + t); }
            busy = false;
        }).start();
    }

    private void copyReport() {
        ClipboardManager cm = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
        cm.setPrimaryClip(ClipData.newPlainText("JIANT Probe", report.toString()));
        say("(informe copiado)");
    }

    private void shareReport() {
        Intent i = new Intent(Intent.ACTION_SEND);
        i.setType("text/plain");
        i.putExtra(Intent.EXTRA_SUBJECT, "JIANT Probe " + Build.MODEL);
        i.putExtra(Intent.EXTRA_TEXT, report.toString());
        startActivity(Intent.createChooser(i, "Compartir informe"));
    }

    // ------------------------------------------------------------------ devices
    private static String typeName(int t) {
        switch (t) {
            case AudioDeviceInfo.TYPE_USB_DEVICE: return "USB_DEVICE";
            case AudioDeviceInfo.TYPE_USB_HEADSET: return "USB_HEADSET";
            case AudioDeviceInfo.TYPE_USB_ACCESSORY: return "USB_ACCESSORY";
            case AudioDeviceInfo.TYPE_BUILTIN_MIC: return "BUILTIN_MIC";
            case AudioDeviceInfo.TYPE_BUILTIN_SPEAKER: return "BUILTIN_SPEAKER";
            case AudioDeviceInfo.TYPE_BUILTIN_EARPIECE: return "EARPIECE";
            case AudioDeviceInfo.TYPE_WIRED_HEADSET: return "WIRED_HEADSET";
            case AudioDeviceInfo.TYPE_WIRED_HEADPHONES: return "WIRED_HEADPHONES";
            case AudioDeviceInfo.TYPE_BLUETOOTH_SCO: return "BT_SCO";
            case AudioDeviceInfo.TYPE_BLUETOOTH_A2DP: return "BT_A2DP";
            case AudioDeviceInfo.TYPE_TELEPHONY: return "TELEPHONY";
            default: return "type " + t;
        }
    }

    private static boolean isUsb(AudioDeviceInfo d) {
        int t = d.getType();
        return t == AudioDeviceInfo.TYPE_USB_DEVICE || t == AudioDeviceInfo.TYPE_USB_HEADSET
               || t == AudioDeviceInfo.TYPE_USB_ACCESSORY;
    }

    private static boolean isFm1(AudioDeviceInfo d) {
        String n = String.valueOf(d.getProductName()).toUpperCase(Locale.ROOT);
        return isUsb(d) && (n.contains("JIANT") || n.contains("FM") || n.contains("FELUCCA"));
    }

    private void describe(AudioDeviceInfo d) {
        say("  · " + d.getProductName() + "  [" + typeName(d.getType()) + ", id " + d.getId() + "]");
        say("      canales: " + Arrays.toString(d.getChannelCounts())
            + "  máscaras índice: " + hexes(d.getChannelIndexMasks()));
        say("      frecuencias: " + (d.getSampleRates().length == 0 ? "cualquiera" : Arrays.toString(d.getSampleRates()))
            + "  codificaciones: " + Arrays.toString(d.getEncodings()));
    }

    private static String hexes(int[] a) {
        StringBuilder s = new StringBuilder("[");
        for (int i = 0; i < a.length; i++) s.append(i == 0 ? "" : ", ").append("0x").append(Integer.toHexString(a[i]));
        return s.append(']').toString();
    }

    private void listDevices() {
        say("\n== 1 · DISPOSITIVOS");
        say("Entradas:");
        for (AudioDeviceInfo d : am.getDevices(AudioManager.GET_DEVICES_INPUTS)) describe(d);
        say("Salidas:");
        for (AudioDeviceInfo d : am.getDevices(AudioManager.GET_DEVICES_OUTPUTS)) describe(d);
        say("UNPROCESSED soportado: " + am.getProperty(AudioManager.PROPERTY_SUPPORT_AUDIO_SOURCE_UNPROCESSED));
        say("Frecuencia nativa: " + am.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE) + " Hz, buffer "
            + am.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER) + " frames");
        MidiManager mm = (MidiManager) getSystemService(Context.MIDI_SERVICE);
        if (mm == null) { say("MIDI: no disponible"); return; }
        MidiDeviceInfo[] md = mm.getDevices();
        say("MIDI: " + md.length + " dispositivo(s)");
        for (MidiDeviceInfo m : md) {
            Bundle p = m.getProperties();
            say("  · " + p.getString(MidiDeviceInfo.PROPERTY_NAME) + "  [" + (m.getType() == MidiDeviceInfo.TYPE_USB ? "USB" : "tipo " + m.getType())
                + ", entradas " + m.getInputPortCount() + ", salidas " + m.getOutputPortCount() + "]");
        }
    }

    private AudioDeviceInfo findUsbInput(boolean fm1) {
        AudioDeviceInfo any = null;
        for (AudioDeviceInfo d : am.getDevices(AudioManager.GET_DEVICES_INPUTS)) {
            if (!isUsb(d)) continue;
            if (isFm1(d) == fm1) return d;
            if (any == null) any = d;
        }
        return fm1 ? any : null;
    }

    // ------------------------------------------------------------------ 2 · eight channels
    /** opens an AudioRecord of n channels (index mask) on device d; null when Android refuses it */
    private AudioRecord open(AudioDeviceInfo d, int n, boolean indexMask) {
        AudioFormat.Builder fb = new AudioFormat.Builder().setEncoding(AudioFormat.ENCODING_PCM_16BIT).setSampleRate(RATE);
        if (indexMask) fb.setChannelIndexMask((1 << n) - 1);
        else fb.setChannelMask(n == 1 ? AudioFormat.CHANNEL_IN_MONO : AudioFormat.CHANNEL_IN_STEREO);
        int src = "true".equals(am.getProperty(AudioManager.PROPERTY_SUPPORT_AUDIO_SOURCE_UNPROCESSED))
                  ? MediaRecorder.AudioSource.UNPROCESSED : MediaRecorder.AudioSource.MIC;
        try {
            AudioRecord r = new AudioRecord.Builder().setAudioSource(src).setAudioFormat(fb.build())
                                .setBufferSizeInBytes(RATE / 4 * n * 2).build();
            if (r.getState() != AudioRecord.STATE_INITIALIZED) { r.release(); return null; }
            if (d != null) r.setPreferredDevice(d);
            return r;
        } catch (Throwable t) {
            say("      (" + n + " canales" + (indexMask ? ", máscara índice" : "") + ": " + t.getMessage() + ")");
            return null;
        }
    }

    private void recordStems() {
        say("\n== 2 · GRABAR 10 s");
        AudioDeviceInfo d = findUsbInput(true);
        if (d == null) { say("No veo ninguna entrada USB. ¿El FM-1 está enchufado y en USB › STEMS?"); return; }
        say("Entrada: " + d.getProductName() + " (canales que declara: " + Arrays.toString(d.getChannelCounts()) + ")");
        AudioRecord r = null;
        int n = 0;
        for (int want : new int[] {8, 6, 4, 2}) {           /* the most channels Android will give */
            r = open(d, want, true);
            if (r != null) { n = want; break; }
        }
        if (r == null) { r = open(d, 2, false); n = 2; }
        if (r == null) { say("RESULTADO: Android no abrió la entrada USB."); return; }
        say("Pedí " + n + " canales; Android dio " + r.getChannelCount() + " a " + r.getSampleRate() + " Hz.");
        n = r.getChannelCount();
        int frames = RATE * 10;
        short[] all = new short[frames * n];
        short[] buf = new short[RATE / 20 * n];
        r.startRecording();
        AudioDeviceInfo routed = r.getRoutedDevice();
        say("Ruteado a: " + (routed == null ? "?" : routed.getProductName() + " [" + typeName(routed.getType()) + "]"));
        say("Grabando… tocá algo en el FM-1 (que suenen varias pistas).");
        int got = 0, reads = 0, shorts = 0;
        long t0 = System.nanoTime();
        while (got < all.length) {
            if (System.nanoTime() - t0 > 20_000_000_000L) { say("(se cortó a los 20 s: el FM-1 no mandaba audio)"); break; }
            int k = r.read(buf, 0, Math.min(buf.length, all.length - got));
            if (k < 0) { say("read() devolvió " + k); break; }
            if (k < buf.length && got + k < all.length) shorts++;
            System.arraycopy(buf, 0, all, got, k);
            got += k;
            reads++;
        }
        double secs = (System.nanoTime() - t0) / 1e9;
        r.stop();
        r.release();
        say(String.format(Locale.ROOT, "Leí %d frames en %.2f s (%d lecturas, %d cortas).", got / n, secs, reads, shorts));
        int live = 0;
        for (int c = 0; c < n; c++) {
            int pk = 0;
            double e = 0;
            for (int i = c; i < got; i += n) {
                int v = Math.abs(all[i]);
                if (v > pk) pk = v;
                e += (double) all[i] * all[i];
            }
            double rms = Math.sqrt(e / Math.max(1, got / n));
            double db = pk > 0 ? 20 * Math.log10(pk / 32768.0) : -120;
            if (pk > 64) live++;
            say(String.format(Locale.ROOT, "  canal %d: pico %6.1f dBFS  rms %7.1f%s", c + 1, db, rms, pk > 64 ? "" : "  (silencio)"));
        }
        say("Canales con señal: " + live + " de " + n + ".");
        String name = saveWav(all, got, n);
        say(name != null ? "WAV guardado: Descargas/JIANT/" + name : "No pude guardar el WAV.");
        say(n >= 8 ? "RESULTADO: este teléfono abre los 8 canales por la API normal. La app puede usar Oboe/AAudio."
                   : "RESULTADO: Android dio solo " + n + " canales. La app necesita su propio driver USB para los 8.");
    }

    private String saveWav(short[] pcm, int count, int ch) {
        String name = "probe_" + new SimpleDateFormat("yyyyMMdd_HHmmss", Locale.ROOT).format(new Date()) + "_" + ch + "ch.wav";
        try {
            ContentValues cv = new ContentValues();
            cv.put(MediaStore.MediaColumns.DISPLAY_NAME, name);
            cv.put(MediaStore.MediaColumns.MIME_TYPE, "audio/wav");
            cv.put(MediaStore.MediaColumns.RELATIVE_PATH, Environment.DIRECTORY_DOWNLOADS + "/JIANT");
            Uri u = getContentResolver().insert(MediaStore.Downloads.EXTERNAL_CONTENT_URI, cv);
            if (u == null) return null;
            try (OutputStream o = getContentResolver().openOutputStream(u)) {
                int bytes = count * 2;
                byte[] h = new byte[44];
                wr(h, 0, 'R', 'I', 'F', 'F'); le32(h, 4, 36 + bytes); wr(h, 8, 'W', 'A', 'V', 'E');
                wr(h, 12, 'f', 'm', 't', ' '); le32(h, 16, 16); le16(h, 20, 1); le16(h, 22, ch);
                le32(h, 24, RATE); le32(h, 28, RATE * ch * 2); le16(h, 32, ch * 2); le16(h, 34, 16);
                wr(h, 36, 'd', 'a', 't', 'a'); le32(h, 40, bytes);
                o.write(h);
                byte[] b = new byte[count * 2];
                for (int i = 0; i < count; i++) { b[2 * i] = (byte) pcm[i]; b[2 * i + 1] = (byte) (pcm[i] >> 8); }
                o.write(b);
            }
            return name;
        } catch (Throwable t) {
            say("  (" + t.getMessage() + ")");
            return null;
        }
    }

    private static void wr(byte[] h, int o, char a, char b, char c, char d) { h[o] = (byte) a; h[o + 1] = (byte) b; h[o + 2] = (byte) c; h[o + 3] = (byte) d; }
    private static void le16(byte[] h, int o, int v) { h[o] = (byte) v; h[o + 1] = (byte) (v >> 8); }
    private static void le32(byte[] h, int o, int v) { le16(h, o, v); le16(h, o + 2, v >> 16); }

    // ------------------------------------------------------------------ 3 · two USB inputs at once
    private void twoInputs() {
        say("\n== 3 · FM-1 + MIC USB A LA VEZ");
        List<AudioDeviceInfo> usb = new ArrayList<>();
        for (AudioDeviceInfo d : am.getDevices(AudioManager.GET_DEVICES_INPUTS)) if (isUsb(d)) usb.add(d);
        say("Entradas USB que veo: " + usb.size());
        for (AudioDeviceInfo d : usb) say("  · " + d.getProductName() + " [" + typeName(d.getType()) + "]");
        if (usb.size() < 2) {
            say("RESULTADO: Android muestra " + usb.size() + " entrada(s) USB. Con el FM-1 y un mic USB en un hub deberían ser 2;"
                + " si ves 1, este teléfono abre un solo dispositivo de audio USB a la vez.");
            return;
        }
        AudioRecord a = open(usb.get(0), 2, false), b = open(usb.get(1), 1, false);
        if (a == null || b == null) { say("RESULTADO: no pude abrir las dos a la vez."); if (a != null) a.release(); if (b != null) b.release(); return; }
        a.startRecording();
        b.startRecording();
        short[] ba = new short[2048], bb = new short[1024];
        int pa = 0, pb = 0;
        long end = System.currentTimeMillis() + 3000;
        while (System.currentTimeMillis() < end) {
            int ka = a.read(ba, 0, ba.length, AudioRecord.READ_NON_BLOCKING), kb = b.read(bb, 0, bb.length, AudioRecord.READ_NON_BLOCKING);
            for (int i = 0; i < ka; i++) pa = Math.max(pa, Math.abs(ba[i]));
            for (int i = 0; i < kb; i++) pb = Math.max(pb, Math.abs(bb[i]));
            try { Thread.sleep(5); } catch (InterruptedException ignored) { }
        }
        AudioDeviceInfo ra = a.getRoutedDevice(), rb = b.getRoutedDevice();
        a.stop(); b.stop(); a.release(); b.release();
        say("  " + usb.get(0).getProductName() + ": pico " + pa + ", ruteado a " + (ra == null ? "?" : ra.getProductName()));
        say("  " + usb.get(1).getProductName() + ": pico " + pb + ", ruteado a " + (rb == null ? "?" : rb.getProductName()));
        boolean both = ra != null && rb != null && ra.getId() != rb.getId() && pa > 64 && pb > 64;
        say(both ? "RESULTADO: las dos entradas USB funcionan a la vez."
                 : "RESULTADO: no llegó señal de las dos por separado (o Android las mandó al mismo dispositivo).");
    }

    // ------------------------------------------------------------------ 4 · the phone's mic into the FM-1
    private void sendMic() {
        say("\n== 4 · ENVIAR EL MIC AL FM-1");
        AudioDeviceInfo out = null, mic = null;
        for (AudioDeviceInfo d : am.getDevices(AudioManager.GET_DEVICES_OUTPUTS)) if (isUsb(d)) { out = d; break; }
        for (AudioDeviceInfo d : am.getDevices(AudioManager.GET_DEVICES_INPUTS)) if (d.getType() == AudioDeviceInfo.TYPE_BUILTIN_MIC) { mic = d; break; }
        if (out == null) { say("No veo una salida USB. Poné el FM-1 en MENU › SYSTEM › USB › IN+OUT y volvé a probar."); return; }
        say("Salida: " + out.getProductName() + "  ·  micrófono: " + (mic == null ? "el de siempre" : mic.getProductName()));
        AudioRecord r;
        try {
            r = new AudioRecord.Builder().setAudioSource(MediaRecorder.AudioSource.VOICE_RECOGNITION)
                    .setAudioFormat(new AudioFormat.Builder().setEncoding(AudioFormat.ENCODING_PCM_16BIT).setSampleRate(RATE)
                                        .setChannelMask(AudioFormat.CHANNEL_IN_MONO).build())
                    .setBufferSizeInBytes(RATE / 10 * 2).build();
        } catch (Throwable t) { say("No pude abrir el micrófono: " + t.getMessage()); return; }
        if (mic != null) r.setPreferredDevice(mic);
        AudioTrack tr = new AudioTrack.Builder()
                .setAudioFormat(new AudioFormat.Builder().setEncoding(AudioFormat.ENCODING_PCM_16BIT).setSampleRate(RATE)
                                    .setChannelMask(AudioFormat.CHANNEL_OUT_STEREO).build())
                .setBufferSizeInBytes(RATE / 10 * 4).setPerformanceMode(AudioTrack.PERFORMANCE_MODE_LOW_LATENCY).build();
        tr.setPreferredDevice(out);
        r.startRecording();
        tr.play();
        say("Hablá al teléfono: tu voz tendría que salir por el FM-1 (pista T6) durante 5 s.");
        short[] in = new short[256], st = new short[512];
        int pk = 0;
        long end = System.currentTimeMillis() + 5000;
        while (System.currentTimeMillis() < end) {
            int k = r.read(in, 0, in.length);
            if (k <= 0) continue;
            for (int i = 0; i < k; i++) { st[2 * i] = st[2 * i + 1] = in[i]; pk = Math.max(pk, Math.abs(in[i])); }
            tr.write(st, 0, 2 * k);
        }
        AudioDeviceInfo routed = tr.getRoutedDevice();
        r.stop(); r.release(); tr.stop(); tr.release();
        say("Pico del mic: " + pk + "  ·  la salida fue a: " + (routed == null ? "?" : routed.getProductName() + " [" + typeName(routed.getType()) + "]"));
        say(routed != null && isUsb(routed) ? "RESULTADO: el audio salió por USB. Decime si lo escuchaste en el FM-1."
                                            : "RESULTADO: Android no mandó el audio al FM-1.");
    }
}
