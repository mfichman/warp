////////////////
// BASIC PATCH
////////////////

Gain master_gain => dac;
.4 => master_gain.gain;

////////////
// GLOBALS
////////////

120 => int g_bpm;
16 => int g_n_beats; // 4 measures
0 => int g_server_started;
0 => int cur_beat;

Event downbeat_e;
Event beat_e;

////////////
// CLASSES
////////////

class BeatLoop {
    SndBuf sndbuf;
    int bpm;
    int n_beats;
    0 => int is_playing;

    public void load(string path_name) {
        sndbuf => master_gain;
        path_name => sndbuf.read;
        0 => sndbuf.rate;
    }

    private void loop_back() {
        g_bpm $ float / bpm => sndbuf.rate;
        0 => sndbuf.pos;
    }

    public void start() {
        <<< "loop playing adjusted for ", bpm, " bpm." >>>;
        1 => is_playing;
        downbeat_e => now;
        loop_back();
        me.yield(); // ignore the concurrent beat event
        while(is_playing) {
            beat_e => now;
            if (cur_beat % n_beats == 0) {
                loop_back();
            }
        }
    }

    public void stop() {
        0 => is_playing;
    }
}

//////////////
// INSTANCES
//////////////

BeatLoop g_loops[0]; // this is a map

////////////////
// BEAT SENDER
////////////////

// send object
OscSend xmit;

// aim the transmitter
xmit.setHost( "localhost", 7000 );

/////////////////
// OSC LISTENER
/////////////////

// create our OSC receiver
OscRecv recv;
// use port 6449 (or whatever)
6449 => recv.port;
// start listening (launch thread)
recv.listen();

recv.event( "/loop/start, s s i i" ) @=> OscEvent @ loop_start_e;
recv.event( "/server/start, i" ) @=> OscEvent @ start_server_e;
recv.event( "/server/stop, i" ) @=> OscEvent @ stop_server_e;

////////////
// METHODS
////////////

// sets the pulse for the application
fun void metronome() {
    while (g_server_started) {
        downbeat_e.broadcast();
        for (0 => cur_beat; cur_beat < g_n_beats; cur_beat++) {
            beat_e.broadcast();
            xmit.startMsg("/", "i");
            cur_beat => xmit.addInt;
            <<<"beat: ", cur_beat,"">>>;
            1::minute / g_bpm => now;
        }
    }
}

fun void start_server_listener() {
    while(true) {
        start_server_e => now;
        start_server_e.nextMsg();
        start_server_e.getInt() => g_bpm;
        <<< "server started at ", g_bpm, " bpm" >>>;

        // give other processes a chance to finish:
        me.yield();

        1 => g_server_started;
        metronome();
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
            <<< "got (via OSC): " >>>;
            <<< "name: ", name >>>;
            <<< "path: ",  path_name >>>;
            <<< "bpm: ",  bpm >>>;
            <<< "n_beats: ", n_beats >>>;

            BeatLoop bl;
            bl.load(path_name);
            bpm => bl.bpm;
            n_beats => bl.n_beats;
            bl @=> g_loops[name];

            spork ~ bl.start();
        }
    }
}


/////////
// PLAY
/////////

spork ~ start_server_listener();
spork ~ stop_server_listener();
spork ~ loop_start_listener();
me.yield();
1::day => now;
