// The Birds Sound Recreation #1


s.meter
s.boot

o = Synth(\organic, [\freq, exprand(1400,2000), \decay, exprand(3,4), \lowHarmony, exprand(500,800), \attackNoise, -0.01]);
o.free
o.plot

a = Synth(\organic, [\freq, exprand(4000,5240), \decay, 3.7, \lowHarmony, 3000]);


x = {SinOsc.ar(SinOsc.ar(PinkNoise.ar() * 40)) * 30 ! 2}.play
x.free


x = {(SinOsc.ar(60.midicps * 0.9) + SinOsc.ar(60.midicps * exprand(2,4)) * 1.002 + SinOsc.ar(60.midicps * exprand(3,6))) * 0.998 * Line.ar(0.3, 0, exprand(1.3,1.6)) ! 2}.play
x = {(SinOsc.ar(64.midicps) + SinOsc.ar(64.midicps / 2) * 1.002 + SinOsc.ar(64.midicps / 3)) * 0.998 * Line.ar(0.3, 0, exprand(0.3,0.6)) ! 2}.play
x = {(SinOsc.ar(67.midicps) + SinOsc.ar(67.midicps / 2) * 1.002 + SinOsc.ar(67.midicps / 3)) * 0.998 * Line.ar(0.3, 0, exprand(0.3,0.6)) ! 2}.play
x.free

y = {BPF.ar(PinkNoise.ar() + LFNoise2.ar(400) * 0.1, Line.ar(60, 300, 3.0)) ! 2}.play
y.free

x = {LPF.ar(PinkNoise.ar(), exprand(200,400)) * EnvGen.ar(Env.new([0.2, 1.4, 0], [0.1, 1.4, 0.2]), doneAction: Done.freeSelf) ! 2}.play
x.free
w = { Pbind(\instrument, \organic, \freq, Pexprand.new(1200, 2500, inf)) }


y = {LPF.ar(PinkNoise.ar(PinkNoise.ar()) * LFNoise2.ar(LFNoise1.ar(200)) * Line.ar(0,0.3, 0.5), 300) ! 2}.play

y.plot

y.free
x.free

w.play

v = {SinOsc.ar(EnvGen.ar(Env.new([30, 240, 120, 0], [0.3, 0.6, 0.5, 0.3]), doneAction: Done.freeSelf) * EnvGen.ar(Env.perc) * 0.2) ! 2}.play

v.free

SynthDef(\organic, {
	arg freq = 440, decay = 3, lowHarmony = 300, attackNoise = 1500;
	var sig, freqEnv;
	freqEnv = freq * EnvGen.ar(Env.new([0.4, 1.0, 0.8, 0], [0.03,  decay * 0.7, 3, 5], 'lin'), 1.0);
	//freqEnv = freq;
	sig = Saw.ar(freqEnv, 0.4) + Saw.ar(freqEnv * 0.5 * 1.003) * 0.4 + Saw.ar(freqEnv * 0.25 * 0.998) * 0.3 + Saw.ar(freqEnv * 0.125 * 0.998) * 0.2;
	sig = sig * (LFNoise1.ar(Line.ar(100, attackNoise, 4)) * EnvGen.ar(Env.perc, doneAction:  Done.freeSelf));
	sig = RLPF.ar(sig, 600, 0.2);

	sig = sig * 2.8;

	//sig = sig + BrownNoise.ar(Line.ar(50, 500, 2.0)) * Line.ar(0.0, 0.4, 0.2);

	sig = sig * EnvGen.ar(Env.new([0.5, 1.0, 0.8, 0.1], [0.02, decay, 2.9, 0.5]), 1.0, 1.0, 0.3, 1.0, doneAction: Done.freeSelf);
	//sig = sig + PinkNoise.ar(0.8);
	sig = BPF.ar(sig, lowHarmony) + BPF.ar(sig, 1000) + BPF.ar(sig, 4000);
	Out.ar(0, sig) + Out.ar(1, sig);
}
).add
