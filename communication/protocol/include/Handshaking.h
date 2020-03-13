/*
 * Handshaking.h
 *
 *  Created on: 30.8.2019
 *      Author: Zahorack
 */

#ifndef HANDSHAKING_H_
#define HANDSHAKING_H_

#include "stdint.h"
//#include "Queue.h"
//#include "Stack.h"

#include "Timer.h"
#include "Packet.h"
#include "Result.h"
#include <queue>

namespace HandshakingStates {
enum Enum {
	Busy = 0,
	Free
};
}

class Handshaking {

//    std::queue<Packet> m_packetQueue;

	Container::Queue<Packet, 10> m_packetQueue;
	Timer m_timer;
	uint8_t m_state;

public:
	Handshaking();

	bool add(Packet);

	Container::Result<Packet> update();

	bool check(Packet);

};



#endif
