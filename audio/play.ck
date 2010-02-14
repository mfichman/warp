// send object
OscSend xmit;

// aim the transmitter
xmit.setHost( "localhost", 7000 );


SndBuf b1 => NRev r => dac;

113 => float bpm;
8 => int n_beats;
60.0 / bpm * n_beats::second => dur length;

"loops/Effected Beat 01.wav" => b1.read;
1 => b1.rate;
.6 => b1.gain;
.0 => r.mix;
while (true) {
    for (0 => int i; i < n_beats; i++) {
	xmit.startMsg("/", "i");
	i => xmit.addInt;
        <<<"beat","">>>;
        length / n_beats => now;
    }

    0 => b1.pos;
    0 => b1.rate;
    30::samp => now;
    1 => b1.rate;
}
1::day => now;
