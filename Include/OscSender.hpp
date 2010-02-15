#ifndef OSCSENDER_H
#define OSCSENDER_H

/* a wrapper around the oscpack library for super-easy
 * local packet transmission
 */

#include <liboscpack/osc/OscOutboundPacketStream.h>
#include <liboscpack/ip/UdpSocket.h>
#include <liboscpack/ip/IpEndpointName.h>
#include <iostream>

#define IP_MTU_SIZE 1536

class OscSender {
private:
    
    osc::OutboundPacketStream* op_stream;
    UdpTransmitSocket* socket;
    char* buffer;
    
public:
    OscSender(int port) {
        IpEndpointName host("localhost", port);
        char hostIpAddress[ IpEndpointName::ADDRESS_STRING_LENGTH ];
        host.AddressAsString( hostIpAddress );

        buffer = new char[IP_MTU_SIZE];
        op_stream = new osc::OutboundPacketStream( buffer, IP_MTU_SIZE );
        socket = new UdpTransmitSocket(host);
    }

    void beginMsg(const char* location) {
        op_stream->Clear();
        *op_stream << osc::BeginMessage(location);
    }

    void addString(const char* str) {
        *op_stream << str;
    }

    void sendMsg() {
        *op_stream << osc::EndMessage;
        socket->Send(op_stream->Data(), op_stream->Size());
    }

    /*
    void SendFloat(char* location, float data) {
        std::cout << "sending " << data << "to " << location << std::endl;
        op_stream->Clear();
        *op_stream << osc::BeginMessage(location) << data << osc::EndMessage;
        socket->Send(op_stream->Data(), op_stream->Size());
    }           

    void SendInt(char* location, int data) {
        std::cout << "sending " << data << "to " << location << std::endl;
        op_stream->Clear();
        *op_stream << osc::BeginMessage(location) << data << osc::EndMessage;
        socket->Send(op_stream->Data(), op_stream->Size());
    }
    */

    osc::OutboundPacketStream & getPacketStream() {
        return *op_stream;
    }

    UdpTransmitSocket & getSocket() {
        return *socket;
    }
};

#endif
