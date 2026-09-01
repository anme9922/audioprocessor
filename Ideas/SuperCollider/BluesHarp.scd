// Blues Harp - amplified diatonic harmonica (cross-harp / 2nd position)
// ---------------------------------------------------------------------------
// Model (Farnell: exciter + resonator + noise air, subtractive & nonlinear):
//   EXCITER   - a free reed: airflow chopped by a swinging metal tongue. Modelled
//               as two slightly detuned Pulse+Saw "reeds" whose duty cycle narrows
//               with breath pressure (harder blow -> brighter, more nasal bite).
//   AIR       - turbulent breath: a chiff on the attack + an ever-present rasp.
//   RESONATOR - body formants (comb + cover plates) and a hand-cupping "wah"
//               lowpass that closes the harp to a dark vocal honk.
//   DRIVE     - the Chicago tone: bullet-mic mid-honk into tube-amp saturation.
// The four blues controls -- bend, cup, breath, vibrato -- are the expressive
// heart; drive takes you from acoustic to amplified.
// ---------------------------------------------------------------------------

s.boot;
s.meter;

// ---- single notes ---------------------------------------------------------
// Clean acoustic draw note (key-of-C harp, the "4 draw" D ~ 587 Hz)
h = Synth(\bluesHarp, [\freq, 587, \breath, 0.5, \cup, 0.2, \drive, 0.0]);
h.set(\gate, 0);

// The signature bent blue note: amplified, cupped, pulled down a whole tone
h = Synth(\bluesHarp, [\freq, 587, \breath, 0.8, \bend, 1.0, \bendDepth, 2.0,
                       \cup, 0.6, \drive, 0.6, \vibrato, 0.5]);
h.set(\gate, 0);

// Wail it: high, hard breath, heavy overdrive + fast throat vibrato
h = Synth(\bluesHarp, [\freq, 880, \breath, 1.0, \cup, 0.45, \drive, 0.85,
                       \vibrato, 0.7, \vibRate, 6.5, \pan, -0.2]);
h.set(\gate, 0);

// Live bending with a controller: hold the note, then sweep \bend by hand
h = Synth(\bluesHarp, [\freq, 587, \breath, 0.85, \drive, 0.5, \cup, 0.4]);
h.set(\bend, 1.0);     // pull down...
h.set(\bend, 0.0);     // ...and release back up (the \bend arg is lag-smoothed)
h.set(\gate, 0);

// ---- a slow expressive cross-harp lick (C harp => key of G blues) ----------
// G blues scale: G Bb C Db D F  (midinotes 55 58 60 61 62 65)
(
Pbind(
    \instrument, \bluesHarp,
    \scale, nil,
    \midinote, Pseq([62, 65, 62, 61, 60, 58, 55, 58, 60], 1),  // a falling blues phrase
    \dur,      Pseq([0.5, 0.5, 0.5, 0.25, 0.75, 0.5, 1.0, 0.5, 1.5], 1),
    \legato,   0.95,
    \breath,   Pseq([0.6, 0.7, 0.6, 0.8, 0.7, 0.65, 0.85, 0.6, 0.9], 1),
    \bend,     Pseq([0.0, 0.0, 0.0, 0.6, 0.0, 0.0, 0.4, 0.0, 0.0], 1),  // bend the b3 & root
    \cup,      0.45,
    \drive,    0.55,
    \vibrato,  Pseq([0.2, 0.2, 0.2, 0.3, 0.3, 0.3, 0.6, 0.4, 0.7], 1),
    \amp,      0.32
).play;
)

// ---------------------------------------------------------------------------
SynthDef(\bluesHarp, {
    // ---- interface: expressive, orthogonal harp controls --------------------
    arg freq = 440, amp = 0.3, pan = 0, gate = 1,
        breath = 0.6,      // effort macro: loudness + brightness + air + reed bite (0..1)
        bend = 0.0,        // draw-bend: 0 = on pitch, 1 = full bend DOWN (live-settable)
        bendDepth = 1.5,   // semitones a full bend pulls down (0.5..3 musical)
        cup = 0.3,         // hand cupping: 0 open/bright -> 1 closed/dark "wah" (0..1)
        drive = 0.2,       // 0 = clean acoustic -> 1 = amplified Chicago overdrive
        vibrato = 0.35,    // throat/hand vibrato depth (0..1)
        vibRate = 5.5,     // vibrato speed, Hz (4..7 musical)
        detune = 0.5,      // two-reed beating shimmer (0..1)
        atk = 0.025, rel = 0.12;

    var bendRatio, vibDelay, vibLfo, pitchVib, jitter, pitch, reedFreqs;
    var pw, reeds, source, air, airEnv, body, cupFreq, sig, drv, env, trem;

    // ---- PITCH: bend + eased-in vibrato + organic micro-wander --------------
    // Draw-bend pulls the reed down up to bendDepth semitones -> the blue note.
    bendRatio = (bend.lag(0.06) * bendDepth.neg).midiratio;
    // Vibrato settles in after the attack, like a player leaning into the note.
    vibDelay  = EnvGen.kr(Env.new([0, 0, 1], [0.28, 0.25], 'sin'));
    vibLfo    = SinOsc.kr(vibRate, 0, vibrato * vibDelay);
    pitchVib  = (vibLfo * 0.18).midiratio;            // ~ +/-18 cents at full depth
    // Breath instability: a tiny live wander so it never sits dead still.
    jitter    = LFNoise2.kr(6).range(0.997, 1.003);
    pitch     = (freq * bendRatio * pitchVib * jitter).clip(20, 18000);

    // ---- SOURCE: two free reeds, pressure-driven pulse width ----------------
    // Free reed = airflow chopped by a swinging tongue -> Pulse+Saw, rich harmonics.
    // Harder breath narrows the duty cycle -> brighter, more nasal reed bite.
    pw        = breath.linlin(0, 1, 0.5, 0.18) + LFNoise2.kr(8).range(-0.02, 0.02);
    reedFreqs = pitch * [1.0, 1.0 + (detune * 0.006)];   // slight detune -> beating shimmer
    reeds     = (Pulse.ar(reedFreqs, pw) * 0.55) + (Saw.ar(reedFreqs) * 0.45);
    source    = reeds.sum * 0.5;

    // ---- BREATH NOISE: the air chiff on attack + a thin sustained rasp -------
    airEnv    = EnvGen.kr(Env.perc(0.004, 0.18)) + 0.3;  // burst, then a hiss floor
    air       = (BPF.ar(PinkNoise.ar, (pitch * 3).clip(200, 12000), 0.6) * 2)
              + (HPF.ar(WhiteNoise.ar, 2000) * 0.15);
    source    = source + (air * airEnv * (0.12 + (breath * 0.35)));

    // ---- BODY FORMANTS: the reedy honk of comb + cover plates ---------------
    body      = ((BPF.ar(source, 1100, 0.5) * 1.6)
              +  (BPF.ar(source, 2300, 0.45) * 1.1)
              +  (source * 0.7)) * 0.5;

    // ---- HAND CUPPING: the resonant "wah" lowpass ---------------------------
    // Open hands = bright; closing cups the harp to a dark, vocal honk. A draw
    // bend also closes the throat a little, so the tone darkens with the bend.
    cupFreq   = (cup + (bend * 0.2)).linexp(0, 1.2, 7500, 650).clip(300, 9000);
    sig       = RLPF.ar(body, cupFreq, 0.35)
              + (BPF.ar(body, cupFreq, 0.2) * cup * 2);   // moving cupped-hand vowel peak

    // ---- DRIVE: bullet-mic + tube-amp overdrive (Chicago blues) -------------
    // Mid-honk boost INTO the saturator, then tame the fizz above it.
    drv       = sig + (BPF.ar(sig, 1500, 0.7) * drive * 2);
    drv       = (drv * (1 + (drive * 12))).tanh;
    drv       = LPF.ar(drv, (8000 - (drive * 3500)).clip(2000, 9000));
    sig       = XFade2.ar(sig, drv, drive.linlin(0, 1, -1, 0.9));

    // ---- AMPLITUDE: breathy attack, sustained body, travelling tremolo ------
    env       = EnvGen.ar(Env.adsr(atk, 0.08, 0.9, rel, curve: -2), gate, doneAction: Done.freeSelf);
    // Throat/hand tremolo rides with the vibrato (the loudness pulses too).
    trem      = 1 - (vibrato * vibDelay * 0.3 * SinOsc.kr(vibRate, pi/2).range(0, 1));

    sig       = sig * env * trem * (0.5 + (breath * 0.7));
    sig       = LeakDC.ar(sig);
    sig       = Pan2.ar(sig.softclip, pan);
    Out.ar(0, sig * amp);
}).add;
