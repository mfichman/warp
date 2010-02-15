////////////
// GLOBALS
////////////
SndBuf loops[0];
120 => float bpm;
8 => int n_beats;
60.0 / bpm * n_beats::second => dur length;

////////////////
// BEAT SENDER
////////////////

// send object
OscSend xmit;

// aim the transmitter
xmit.setHost( "localhost", 7000 );

fun void beat_loop {
    while (true) {
        for (0 => int i; i < n_beats; i++) {
            xmit.startMsg("/", "i");
            i => xmit.addInt;
            <<<"beat","">>>;
            length / n_beats => now;
        }
    }
}


/////////
// PLAY
/////////

spork ~ beat_loop();
1::day => now;
