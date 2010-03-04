// run this file concurrently with play.ck
// to simulate osc messages and test the main program.

OscSend xmit;
xmit.setHost("localhost", 6449);

xmit.startMsg("/server/start", "i");
120 => xmit.addInt;
<<< "test: sent start signal", "" >>>;

1::second => now;

xmit.startMsg("/server/stop", "i");
1 => xmit.addInt;
<<< "test: sent stop signal", "" >>>;
