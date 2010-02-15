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

fun void beat_loop() {
    while (true) {
        for (0 => int i; i < n_beats; i++) {
            xmit.startMsg("/", "i");
            i => xmit.addInt;
            <<<"beat","">>>;
            length / n_beats => now;
        }
    }
}

//////////////////
// BEAT LISTENER
//////////////////

// create our OSC receiver
OscRecv recv;
// use port 6449 (or whatever)
6449 => recv.port;
// start listening (launch thread)
recv.listen();

recv.event( "/loop/start, s s" ) @=> OscEvent @ loop_start_e;

fun void loop_start_listener() {
    while( true )
    {
        // wait for event to arrive
        loop_start_e => now;

        // grab the next message from the queue. 
        while( loop_start_e.nextMsg() )
        { 
            string id;
            string path_name;

            // getFloat fetches the expected float (as indicated by "i f")
            loop_start_e.getString() => id;
            loop_start_e.getString() => path_name;

            // print
            <<< "got (via OSC):", id, path_name >>>;
        }
    }
}


/////////
// PLAY
/////////

spork ~ beat_loop();
spork ~ loop_start_listener();
1::day => now;
