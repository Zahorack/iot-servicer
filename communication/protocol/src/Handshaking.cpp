/*
 * Handshaking.cpp
 *
 *  Created on: 30.8.2019
 *      Author: Zahorack
 */

#include "Handshaking.h"

Handshaking::Handshaking():
	m_state(HandshakingStates::Free)
{
	m_timer.start(200);
}

bool Handshaking::add(Packet packet)
{
	if(packet.header.type == m_packetQueue.value().value.header.type) {
		return false;
	}
	return m_packetQueue.enqueue(packet);
}


Container::Result<Packet> Handshaking::update()
{
	if(m_timer.run()) {
		return m_packetQueue.value();
	}

	return Container::Result<Packet>();
}

bool Handshaking::check(Packet ack)
{
	Container::Result<Packet> current = m_packetQueue.value();

	if(ack.header.id == current.value.header.id) {
		m_packetQueue.dequeue();

		return true;
	}
	return false;
}



