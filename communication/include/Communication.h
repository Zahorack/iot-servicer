/*
 * Comunication.h
 *
 *  Created on: 2.9. 2018
 *      Author: Zahorack
 */

#ifndef UTIL_COMMUNICATION_H_
#define UTIL_COMMUNICATION_H_

#include <Result.h>
#include "Packet.h"
#include "Queue.h"
#include "Handshaking.h"
#include "Timer.h"

#include "../socket/include/TcpSocket.h"


class Communication {
    TcpSocket m_socket;
    uint32_t m_transmitID;
    uint32_t m_receiveID;
    Timer m_timer;

    enum State {
        WaitingForNextPacket,
        ReadingPacketHeader,
        ReadingPacketContents
    };


    State m_state;
    Packet m_currentPacket;
    Handshaking m_handshaking;

public:
    Communication(SOCKET);

    Container::Result<Packet> update();
    void sendStatus();
    void sendAck();
    void sendNack();
    void send(PacketType::Enum type);

    void send(Packet);



private:
    void waitForNextPacket();
    void readPacketHeader();
    Container::Result<Packet> readPacketContents();
    bool checkHeaderCrc();
    bool checkDataCrc();

    void sendHeader(PacketHeader header);
    void sendContents(PacketContents content, uint32_t);
};

#endif /* UTIL_PACKET_H_ */