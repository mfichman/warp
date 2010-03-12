// run this file concurrently with play.ck
// to simulate osc messages and test the main program.

OscSend xmit;
xmit.setHost("localhost", 6449);

1::second => now;
xmit.startMsg("/loop/load", "i s i i");
0 => xmit.addInt;
"loops/Deep Pad 01.wav" => xmit.addString;
120 => xmit.addInt;
4 => xmit.addInt;

xmit.startMsg("/loop/load", "i s i i");
1 => xmit.addInt;
"loops/Disco Pickbass 01 120_C_4.wav" => xmit.addString;
120 => xmit.addInt;
4 => xmit.addInt;

xmit.startMsg("/loop/load", "i s i i");
2 => xmit.addInt;
"beats/Effected Drum Kit 10 80_8.wav" => xmit.addString;
80 => xmit.addInt;
8 => xmit.addInt;


xmit.startMsg("/loop/start", "i f"); 
1 => xmit.addInt;
1 => xmit.addFloat;
xmit.startMsg("/server/start", "i");
120 => xmit.addInt;
<<< "test: sent start signal", "" >>>;

xmit.startMsg("/loop/start", "i f"); 
2 => xmit.addInt;
1 => xmit.addFloat;

xmit.startMsg("/loop/start", "i f"); 
0 => xmit.addInt;
1.5 => xmit.addFloat;

50::second => now;

xmit.startMsg("/loop/stop", "i"); 
1 => xmit.addInt;
