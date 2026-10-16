# JIANT Probe

Phase 0 of the stems app (the design: one Android screen that answers, on a given phone, what the app depends on):

1. **Devices**: every audio input and output Android sees, with the channel counts and index masks each declares,
   and the MIDI devices (the FM-1's USB-MIDI).
2. **Record 10 s of the 8 channels** with the FM-1 in MENU > SYSTEM > USB > STEMS: asks Android for 8 channels (then 6,
   4, 2) by channel index mask at 44.1 kHz, reports what it got, the level of each channel, and saves the take as a
   multichannel WAV in Downloads/JIANT. 8 channels: the app can use the normal API (Oboe / AAudio); fewer: it needs a
   USB driver of its own.
3. **The FM-1 and a USB microphone at once** (through a USB-C hub): whether Android opens two USB audio inputs together.
4. **The phone's microphone into the FM-1** with the FM-1 in USB > IN+OUT: 5 s of the mic played to the USB output.

Everything goes into a plain-text report (COPY / SHARE). Built by GitHub Actions (.github/workflows/emulator.yml for
the site, android.yml on the branches): `gradle -p android/probe assembleDebug`. No dependencies beyond the Android
SDK; Java 17, minSdk 29, targetSdk 34.
