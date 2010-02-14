#ifndef OSC_LISTENER_H
#define OSC_LISTENER_H

#include <iostream>

#include <liboscpack/osc/OscReceivedElements.h>
#include <liboscpack/osc/OscPacketListener.h>
#include <liboscpack/ip/UdpSocket.h>
//#include <pthread.h>
/* I'm so special! I'm using threads! */

/* this is a super simple wrapper to abstract oscpack's
 * listener class as a pthread
 */
               
typedef void (*BeatEventCallback)(int);
typedef struct {
    int port;
    BeatEventCallback callback;
} listen_info_t;

class OscBeatListener : public osc::OscPacketListener {

private:

    BeatEventCallback callback;

public:

    OscBeatListener(BeatEventCallback e_call) {
        callback = e_call;
    }

    virtual ~OscBeatListener() {
    }

protected:
    virtual void ProcessMessage( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint )
    {
        osc::int32 beat;
        try {
            osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
            args >> beat >> osc::EndMessage;
            callback(beat);
        } catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
	}
};



class OscListener {
private:
    int port;

public:
    OscListener(int p) {
        port = p;
    }

	void StartBeatLoop(BeatEventCallback callback) {
		// get input data

		OscBeatListener listener(callback);
		UdpListeningReceiveSocket s(
				IpEndpointName( IpEndpointName::ANY_ADDRESS, port ),
				(osc::OscPacketListener*) &listener );

		std::cout << "listening for input on port " << port << "...\n";
		std::cout << "press ctrl-c to end\n";

		s.RunUntilSigInt();

		std::cout << "finishing.\n";
		exit(0);
	}
};

#endif //OSC_LISTENER_H
