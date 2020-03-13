/*
 * Comunication.cpp
 *
 *  Created on: 2.9. 2018
 *      Author: Zahorack
 */

#include "../include/Communication.h"

//static Container::Queue<volatile Packet, 10> s_priorityQueue;

Communication::Communication(SOCKET fd):
    m_socket(fd),
    m_transmitID(0),
    m_receiveID(0),
	m_state(WaitingForNextPacket)
{
	m_timer.start(5000);
}


Container::Result<Packet> Communication::update()
{
	//std::cout<<("rx: %d\n\r", m_socket.bytesAvailable());

	Container::Result<Packet> attempt;

	if((attempt = m_handshaking.update()).isValid) {
		//std::cout<<("Handshaking send attempt\n\r");
		send(attempt.value);
	}

	if(m_timer.run() && m_state == WaitingForNextPacket) {
		sendStatus();
	}

	switch(m_state) {
		case WaitingForNextPacket:
			waitForNextPacket();
			break;

		case ReadingPacketHeader:
			readPacketHeader();
			break;

		case ReadingPacketContents:
			return readPacketContents();
	}

	return Container::Result<Packet>();
}


void Communication::waitForNextPacket()
{
	if(m_socket.available() > sizeof(PacketMark)) {
		if(m_socket.read() == 0x4C && m_socket.read() == 0x4B) {
			m_state = ReadingPacketHeader;
			//std::cout<<("markOK\n\r");
		}
		else {
			std::cout<<("Wrong packet mark\n\r");
		}
	}
}

void Communication::readPacketHeader()
{
	//std::cout<<("readPacketHeader\n\r");
	if(m_socket.available() >= (sizeof(PacketHeader) + sizeof(uint8_t))) {
		m_socket.receive((char*)&m_currentPacket.header, sizeof(m_currentPacket.header));

		if(checkHeaderCrc()) {
			std::cout<<("<- [%d]\n\r", m_currentPacket.header.type);
			m_state = ReadingPacketContents;
		}
		else {
			m_state = WaitingForNextPacket;
			std::cout<<("Header CRC ERROR\n\r");
		}
	}
}

Container::Result<Packet> Communication::readPacketContents()
{
	//std::cout<<("readPacketContents\n\r");
	if(Packet::SizeForType(m_currentPacket.header.type) > 0) {
		if(m_socket.available() >= (Packet::SizeForType(m_currentPacket.header.type) + sizeof(uint8_t))) {
			//m_socket.readStruct(m_currentPacket.contents);
			m_socket.receive(reinterpret_cast<char *>(&m_currentPacket.contents), Packet::SizeForType(m_currentPacket.header.type));

			if(checkDataCrc()) {
				//std::cout<<("dataOK\n\r");
				if(m_currentPacket.header.type != PacketType::ManualControl){
					sendAck();
				}
				m_state = WaitingForNextPacket;
				return Container::Result<Packet>(m_currentPacket);
			}
		}
	}
	else {
		m_state = WaitingForNextPacket;
		if(m_currentPacket.header.type != PacketType::Ack && m_currentPacket.header.type != PacketType::Nack) {
			sendAck();
		}
		else if (m_currentPacket.header.type == PacketType::Ack) {
			m_handshaking.check(m_currentPacket);
		}

		return Container::Result<Packet>(m_currentPacket);
	}

	return Container::Result<Packet>();
}

bool Communication::checkHeaderCrc()
{
	uint8_t crc = m_socket.read();

	if(crc != Packet::CalculateCRC8(m_currentPacket.header)) {
		std::cout<<("HEADER CRC ERROR RX_CRC = %d    ", crc);
		std::cout<<("CRC = %d\n\r", Packet::CalculateCRC8(m_currentPacket.header));
		sendNack();
		m_state = WaitingForNextPacket;
		return false;
	}
	return true;
}

bool Communication::checkDataCrc()
{
	uint8_t crc = m_socket.read();

	if(crc != Packet::CalculateCRC8((uint8_t*)&m_currentPacket.contents, Packet::SizeForType(m_currentPacket.header.type))) {
		std::cout<<("DATA CRC ERROR RX_CRC = %d    ", crc);
		std::cout<<("CRC = %d\n\r", Packet::CalculateCRC8((uint8_t*)&m_currentPacket.contents.dataPacket.joystickData, Packet::SizeForType(m_currentPacket.header.type)));
		//std::cout<<("size: %d\n\r", Packet::SizeForType(m_currentPacket.header.type));
		//std::cout<<("data : %d", m_currentPacket.contents.dataPacket.joystickData.x);
		sendNack();
		m_state = WaitingForNextPacket;

		return false;
	}

	return true;
}

void Communication::sendHeader(PacketHeader header)
{
    char crc = Packet::CalculateCRC8(header);

	m_socket.send((char*)&PacketMark, 2);
	m_socket.send((char*)&header, sizeof(header));
	m_socket.send(&crc, sizeof(uint8_t));
}


 void Communication::sendContents(PacketContents content, uint32_t size)
{
    char crc = Packet::CalculateCRC8((uint8_t*)&content, size);
	m_socket.send((char*)&content, size);
	m_socket.send(&crc, sizeof(char));
}


void Communication::send(Packet packet)
{
	//std::cout<<("-> [%d]\n\r", packet.header.type);
	sendHeader(packet.header);
	sendContents(packet.contents, Packet::SizeForType(packet.header.type));
}

void Communication::send(PacketType::Enum type)
{
	//std::cout<<("-> [%d]\n\r", type);
	PacketHeader header = {
			.id = m_transmitID++,
			.type = type
	};
	sendHeader(header);
}

void Communication::sendAck()
{
	std::cout<<("-> packet ACK[%d]\n\r", PacketType::Ack);
	PacketHeader ack = {
			.id = m_currentPacket.header.id,
			.type = PacketType::Ack
	};
	sendHeader(ack);
}

void Communication::sendNack()
{
	std::cout<<("-> packet NACK[%d]\n\r", PacketType::Nack);
	PacketHeader nack = {
			.id = m_currentPacket.header.id,
			.type = PacketType::Nack
	};
	sendHeader(nack);
}

void Communication::sendStatus()
{
	Packet status;

	m_handshaking.add(status);

	send(status);
}



