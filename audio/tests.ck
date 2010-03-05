// run this file concurrently with play.ck
// to simulate osc messages and test the main program.

OscSend xmit;
xmit.setHost("localhost", 6449);

1::second => now;
xmit.startMsg("/loop/load", "i s i i");
0 => xmit.addInt;
"loops/Effected Drum Kit 11.wav" => xmit.addString;
110 => xmit.addInt;
8 => xmit.addInt;

xmit.startMsg("/loop/start", "i f"); 
0 => xmit.addInt;
.8 => xmit.addFloat;

xmit.startMsg("/server/start", "i");
120 => xmit.addInt;
<<< "test: sent start signal", "" >>>;

1::second => now;

xmit.startMsg("/sfx/load", "i s");
0 => xmit.addInt;
"sfx/slap.wav" => xmit.addString;

.23::second => now;

xmit.startMsg("/sfx/play", "i f");
0 => xmit.addInt;
1 => xmit.addFloat;

3::second => now;


/*
xmit.startMsg("/server/stop", "i");
1 => xmit.addInt;
<<< "test: sent stop signal", "" >>>;

1::second => now;

xmit.startMsg("/loop/load", "i s i i");
0 => xmit.addInt;
"loops/Effected Drum Kit 11.wav" => xmit.addString;
110 => xmit.addInt;
8 => xmit.addInt;



1::second => now;

xmit.startMsg("/loop/start", "i f"); 
0 => xmit.addInt;
.8 => xmit.addFloat;

xmit.startMsg("/server/start", "i");
180 => xmit.addInt;
1::second => now;

1::second => now;
*/
