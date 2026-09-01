// The Birds - Scary Screaming Crow/Bird SFX
// Inspired by Oskar Sala's Mixtur-Trautonium work on Hitchcock's "The Birds" (1963).
// Core trick (same as \organic): the Trautonium SUBHARMONIC series - undertones
// f/1, f/2, f/3, f/4 instead of the usual overtone series - run through formant
// filters, with hand-played glissando swoops and a noisy screech on top.

s.boot;
s.meter;

// ---- single screams ----------------------------------------------------------
// A nervous crow caw
b = Synth(\scaryBird, [\freq, 760, \dur, 1.1, \gliss, 1.9, \screech, 0.55, \grit, 0.45, \rough, 0.5]);

// High, panicked screech
b = Synth(\scaryBird, [\freq, 1300, \dur, 0.8, \gliss, 2.6, \screech, 0.8, \grit, 0.6, \rough, 0.7, \pan, -0.4]);

// Low, ugly croak (heavy register cracks + growl)
b = Synth(\scaryBird, [\freq, 420, \dur, 1.6, \gliss, 1.3, \screech, 0.35, \grit, 0.7, \rough, 0.85, \pan, 0.5]);

// ---- a whole menacing flock --------------------------------------------------
(
f = Routine({
	40.do {
		Synth(\scaryBird, [
			\freq,    exprand(380, 1700),
			\dur,     exprand(0.5, 1.8),
			\gliss,   exprand(1.2, 2.8),
			\screech, rrand(0.3, 0.85),
			\grit,    rrand(0.3, 0.7),
			\rough,   rrand(0.4, 0.9),
			\pan,     rrand(-0.9, 0.9),
			\amp,     rrand(0.2, 0.45)
		]);
		exprand(0.04, 0.5).wait;
	};
}).play;
)
f.stop;

// ------------------------------------------------------------------------------
SynthDef(\scaryBird, {
	arg freq = 800, dur = 1.2, gliss = 1.8, screech = 0.6, grit = 0.45, rough = 0.55, pan = 0, amp = 0.5;
	var pitchEnv, jitter, crack, pitch, sub1, sub2, sub3, sub4, source;
	var roughAM, breath, fglide, f1, f2, f3, formant, sig, env, ampNoise;

	// --- PITCH: glissando swoop + organic instability ---------------------------
	// Trautonium-style hand glissando: swoop up then collapse.
	pitchEnv = freq * EnvGen.ar(
		Env.new([1.0, gliss, 0.7, 1.5, 0.4], [0.05, 0.3, 0.35, dur * 0.4], 'exp')
	);
	// Smooth panicked flutter (live, breathing vibrato - not a clean LFO).
	jitter = LFNoise2.ar(LFNoise1.kr(7).range(16, 50)).exprange(0.9, 1.11);
	// Sudden register "cracks" - the voice breaking to a subharmonic or up an octave.
	// This stepped jump is a key nonlinear cue that reads as a real distressed animal.
	crack = Select.kr(
		TIRand.kr(0, 3, Dust.kr(LinLin.kr(rough, 0, 1, 1, 8))) * (rough > 0.25),
		[1, 0.5, 2, 0.5]
	).lag(0.008);
	pitch = (pitchEnv * jitter * crack).clip(20, 18000);

	// --- SOURCE: detuned subharmonic (undertone) stack --------------------------
	// f/1, f/2, f/3, f/4 detuned so partials beat and snarl (the Trautonium core).
	sub1 = Saw.ar(pitch)             * 0.5;
	sub2 = Saw.ar(pitch / 2 * 1.004) * 0.6;
	sub3 = Saw.ar(pitch / 3 * 0.995) * 0.5;
	sub4 = Saw.ar(pitch / 4 * 1.003) * 0.4;
	source = sub1 + sub2 + sub3 + sub4;

	// Vocal-fry / growl: audio-rate amplitude roughness at a wandering subharmonic.
	// This biphonation is what most separates an organic scream from a synth tone.
	roughAM = SinOsc.ar(pitch * LFNoise1.kr(5).exprange(0.16, 0.42)).range(1 - rough, 1.0);
	source = source * roughAM;

	// Waveshaping grit - the harsh, torn edge of the scream.
	source = (source * (1 + (grit * 9))).tanh;

	// Turbulent breath rasp mixed into the source (air forced through the throat).
	breath = BrownNoise.ar * LFNoise2.ar(80).range(0.0, 1.0) * (0.3 + (grit * 0.5));
	source = source + breath;

	// --- FORMANTS: moving vowel-like resonances (you liked these) ----------------
	// Gliding centre freqs morph the "vowel" over the call - reads as a living throat.
	fglide = EnvGen.ar(Env.new([1.0, 1.5, 0.85], [dur * 0.4, dur * 0.6], 'exp'));
	f1 = (640 * fglide).clip(120, 18000);
	f2 = (1200 * fglide).clip(120, 18000);
	f3 = (2700 * fglide).clip(120, 18000);
	formant = BPF.ar(source, f1, 0.22) * 3.0
	        + BPF.ar(source, f2, 0.20) * 2.2
	        + BPF.ar(source, f3, 0.16) * 1.6;
	sig = (source * (1 - screech)) + (formant * screech);

	// --- AMPLITUDE: not a clean envelope ----------------------------------------
	// Sharp attack, screaming body, quick guttural release...
	env = EnvGen.ar(
		Env.new([0, 1.0, 0.75, 0.9, 0], [0.02, dur * 0.35, dur * 0.4, dur * 0.23], ['sin', 'lin', 'lin', 'exp']),
		doneAction: Done.freeSelf
	);
	// ...with fast amplitude shudder so the loudness itself trembles.
	ampNoise = LFNoise2.ar(LFNoise1.kr(9).range(12, 35)).range(1 - (rough * 0.5), 1.0);
	sig = sig * env * ampNoise * amp;

	sig = RLPF.ar(sig, 6500, 0.6);
	sig = Pan2.ar(sig.softclip, pan);
	Out.ar(0, sig);
}).add;
