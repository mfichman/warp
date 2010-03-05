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
0 => int cur_beat;
0 => int cur_frac;

Event downbeat_e;
Event beat_e;

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

recv.event( "/loop/load, i s i i" ) @=> OscEvent @ load_loop_e;
recv.event( "/loop/start, i f" ) @=> OscEvent @ start_loop_e;
recv.event( "/loop/stop, i" ) @=> OscEvent @ stop_loop_e;

recv.event( "/server/start, i" ) @=> OscEvent @ start_server_e;
recv.event( "/server/stop, i" ) @=> OscEvent @ stop_server_e;

////////////
// CLASSES
////////////

class BeatLoop {
    SndBuf @ sndbuf;
    int bpm;
    1 => float gain;
    int n_beats;
    0 => int is_playing;

    Shred @ loop_s;

    public void load(string path_name, int _bpm_, int _n_beats_) {
        new SndBuf @=> sndbuf;
        path_name => sndbuf.read;
        _bpm_ => bpm;
        _n_beats_ => n_beats;
        <<< "loop playing adjusted for ", bpm, " bpm." >>>;

        sndbuf => master_gain;
        0 => sndbuf.rate;
    }

    public void start(float gain) {
        if (is_playing) return;
        gain => sndbuf.gain;
        spork ~ loop_f() @=> loop_s;
    }

    public void stop() {
        if (!is_playing) return;
        0 => is_playing;
        loop_s.exit();
    }

    private void loop_back() {
        g_bpm $ float / bpm => sndbuf.rate;
        0 => sndbuf.pos;
    }

    private void loop_f() {
        if (is_playing) return;
        1 => is_playing;
        downbeat_e => now;
        while(is_playing) {
            if (cur_beat % n_beats == 0) {
                loop_back();
            }
            beat_e => now;
        }
    }
}

class Metronome {
    0 => int is_on;
    Shred @ loop_s;

    // sets the pulse for the application
    private void loop_f() {
        while (is_on) {
            downbeat_e.broadcast();
            for (0 => cur_beat; cur_beat < g_n_beats; cur_beat++) {
                beat_e.broadcast();
                xmit.startMsg("/", "i");
                cur_beat => xmit.addInt;
                <<< "beat: ", cur_beat >>>;
                for (0 => cur_frac; cur_frac < 4; cur_frac++) {
                    1::minute / g_bpm / 4 => now;
                }
            }
        }
    }

    public void start() {
        if (is_on) return;
        1 => is_on;
        spork ~ loop_f() @=> loop_s;
        me.yield();
    }

    public void stop() {
        if (!is_on) return;
        0 => is_on;
        loop_s.exit();
    }
}

class BeatServer {
    Metronome metronome;
    BeatLoop loops[20];
    0 => int n_loops;

    Shred @ start_s;
    Shred @ stop_s;
    Shred @ load_loop_s;
    Shred @ start_loop_s;
    Shred @ stop_loop_s;

    public void addListeners() {
        <<< "adding listeners...">>>;
        spork ~ start_f() @=> start_s;
        spork ~ stop_f() @=> stop_s;
        spork ~ load_loop_f() @=> load_loop_s;
        spork ~ start_loop_f() @=> start_loop_s;
        spork ~ stop_loop_f() @=> stop_loop_s;
        //<<< "before listener yield...">>>;
        me.yield();
        //<<< "after listener yield...">>>;
    }

    private void start_loop_f() {
        while(true) {
            start_loop_e => now;
            while (start_loop_e.nextMsg()) {
                start_loop_e.getInt() => int index;
                start_loop_e.getFloat() => float gain;

                loops[index].start(gain);
            }
        }
    }

    private void stop_loop_f() {
        while(true) {
            stop_loop_e => now;
            while (start_loop_e.nextMsg()) {
                stop_loop_e.getInt() => int index;
                stop_loop_e.getFloat() => float gain;

                loops[index].stop();
            }
        }
    }

    // music event handlers:
    private void load_loop_f() {
        while(true) {
            load_loop_e => now;
            while( load_loop_e.nextMsg() )
            { 
                int index;
                string path_name;
                int bpm;
                int n_beats;

                load_loop_e.getInt() => index;
                load_loop_e.getString() => path_name;
                load_loop_e.getInt() => bpm;
                load_loop_e.getInt() => n_beats;

                loops[index].load(path_name, bpm, n_beats);
            }
        }
    }

    // server start and stop event handlers:
    private void start_f() {
        while(true) {
            start_server_e => now;
            start_server_e.nextMsg();
            start_server_e.getInt() => g_bpm;
            <<< "server started at ", g_bpm, " bpm" >>>;

            // give other processes a chance to finish:
            // me.yield();
            metronome.start();
        }
    }

    private void stop_f() {
        while(true) {
            stop_server_e => now;
            stop_server_e.nextMsg();
            // give other processes a chance to finish:
            // me.yield();
            stop_loops();
            metronome.stop();
        }
    }

    private void stop_loops() {
        for(0 => int i; i < 20; i++) {
            if(loops[i] != NULL) {
                loops[i].stop();
            }
        }
    }
}

//////////////
// INSTANCES
//////////////

BeatServer beat_server;

beat_server.addListeners();

<<< "beat server ready", "" >>>;

1::day => now;
