// BASIC PATCH
Gain master_gain => dac;
.4 => master_gain.gain;

////////////
// GLOBALS
////////////
SndBuf g_loops[0];
120 => float g_bpm;
8 => int g_n_beats;
0 => int g_server_started;

////////////////
// BEAT SENDER
////////////////

// send object
OscSend xmit;

// aim the transmitter
xmit.setHost( "localhost", 7000 );

fun void beat_loop() {
    while (g_server_started) {
        for (0 => int i; i < g_n_beats; i++) {
            xmit.startMsg("/", "i");
            i => xmit.addInt;
            <<<"beat: ", i,"">>>;
            1::minute / g_bpm => now;
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

recv.event( "/loop/start, s s i i" ) @=> OscEvent @ loop_start_e;
recv.event( "/server/start, i" ) @=> OscEvent @ start_server_e;
recv.event( "/server/stop, i" ) @=> OscEvent @ stop_server_e;

fun void start_server_listener() {
    while(true) {
        start_server_e => now;
        start_server_e.nextMsg();
        start_server_e.getInt() => g_bpm;
        1 => g_server_started;
        beat_loop();
    }
}

fun void stop_server_listener() {
    while(true) {
        stop_server_e => now;
        stop_server_e.nextMsg();
        0 => g_server_started;
    }
}

fun void loop_start_listener() {
    while( true )
    {
        // wait for event to arrive
        loop_start_e => now;

        // grab the next message from the queue. 
        while( loop_start_e.nextMsg() )
        { 
            string name;
            string path_name;
            int bpm;
            int n_beats;

            loop_start_e.getString() => name;
            loop_start_e.getString() => path_name;
            loop_start_e.getInt() => bpm;
            loop_start_e.getInt() => n_beats;

            // print
            <<< "got (via OSC):", name, path_name >>>;

            SndBuf sndbuf => master_gain;
            path_name => sndbuf.read;
            1 => sndbuf.rate;
            1 => sndbuf.loop;
            //sndbuf => loops[name];
        }
    }
}


/////////
// PLAY
/////////

spork ~ start_server_listener();
spork ~ stop_server_listener();
spork ~ loop_start_listener();
1::day => now;
