#ifndef OSC_LISTENER_H
#define OSC_LISTENER_H

#include <iostream>

#include <liboscpack/osc/OscReceivedElements.h>
#include <liboscpack/osc/OscPacketListener.h>
#include <liboscpack/ip/UdpSocket.h>

#include <memory>

#define DEBUG
#define DEBUG_
#include <boost/thread.hpp>
//#include <boost/bind.hpp>
/* I'm so special! I'm using threads! */

/* this is a super simple wrapper to abstract oscpack's
 * listener class to a thread
 */

class OscBeatListener : public osc::OscPacketListener {

private:

    int port;
    int cur_beat;
    std::auto_ptr<boost::thread> thrd_;

    void beatEvent(int beat) { // we could callback to a method in the main program here...
        cur_beat = beat;
    }

public:
	void startListening() {

		UdpListeningReceiveSocket s(
				IpEndpointName( IpEndpointName::ANY_ADDRESS, port ),
				(osc::OscPacketListener*) this );

		//std::cout << "listening for input on port " << port << "...\n";
		//std::cout << "press ctrl-c to end\n";

		s.RunUntilSigInt();

		//std::cout << "finishing.\n";
		exit(0);
	}

    OscBeatListener(int p)
        : port(p)
    {
		thrd_.reset(new boost::thread(&OscBeatListener::startListening, this));
        //thrd_->join();
    }

    int getCurBeat() const {
        return cur_beat;
    }

protected:
    virtual void ProcessMessage( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint )
    {
        osc::int32 beat;
        try {
            osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
            args >> beat >> osc::EndMessage;
            beatEvent(beat);
        } catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
	}
};

#endif //OSC_LISTENER_H
