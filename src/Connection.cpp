/*
    Copyright (C) 2012 Zeg9
    
    This file is part of Project Perabird.
    Project Perabird is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Project Perabird is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Project Perabird.  If not, see <http://www.gnu.org/licenses/>
*/

#include <string>
#include <iostream>
#include <openssl/sha.h>
#include <SDL/SDL_net.h>
#include "Game.h"
#include "Debug.h"
#include "Tools.h"
#include "Connection.h"

Message::Message() : type(255) {}

void Message::addByte(uint8_t b) { bytes.push_back(b); }
void Message::addString(std::string s) { strings.push_back(s); }
void Message::addInt(int i) { ints.push_back(i); }
void Message::addFloat(float f) { floats.push_back(f); }

int Message::getByteCount() { return bytes.size(); }
int Message::getStringCount() { return strings.size(); }
int Message::getIntCount() { return ints.size(); }
int Message::getFloatCount() { return floats.size(); }

uint8_t Message::getByte(int i) { return bytes[i]; }
std::string Message::getString(int i) { return strings[i]; }
int Message::getInt(int i) { return ints[i]; }
float Message::getFloat(int i) { return floats[i]; }

void Message::setType(uint8_t t) { type = t; }
uint8_t Message::getType() { return type; }


Connection::Connection(std::string host, int port) : recvThread(0)
{
	std::cout << "Connecting to " << host << ':' << port << "..." << std::endl;
	SDLNet_ResolveHost(&ip,host.c_str(),port);
	socket = SDLNet_TCP_Open(&ip);
}
Connection::~Connection()
{
	if (isGood())
		SDLNet_TCP_Close(socket);
	socket = 0;
	SDL_KillThread(recvThread);
	std::cout << "Connection closed" << std::endl;
}

bool Connection::isGood() {
	return socket;
}

void Connection::send(void*msg, int size) {
	if (isGood())
		SDLNet_TCP_Send(socket,msg,size);
}
void Connection::sendByte(uint8_t i) {
	send(&i,sizeof(uint8_t));
}
void Connection::sendFloat(float f) {
	send(&f,sizeof(float));
}

uint8_t *Connection::recv(void* buffer, int size) {
	if (isGood())
	{
		if(!SDLNet_TCP_Recv(socket,buffer,size))
		{
			GAME->getConsole()->print("[Lost connection]");
			socket = 0;
			GAME->disconnect();
		}
	}
}

uint8_t Connection::recvByte() {
	uint8_t r(255);
	recv(&r,1);
	return r;
}


Message Connection::parseMessage(std::string format)
{
	Message m;
	uint8_t lastByte(255);
	for (int i = 0; i < format.size(); i++)
	{
		switch(format[i])
		{
			case 'b':
			{
				m.addByte(recvByte());
				lastByte = recvByte();
				break;
			}
			case 's':
			{
				std::string s;
				char r(255);
				while(1) {
					r = recvByte();
					if (r == MSG_SEP || r == MSG_END) break;
					s += r;
				} 
				m.addString(s);
				lastByte = r;
				break;
			}
			case 'i':
			{
				int i;
				recv(&i,sizeof(int));
				m.addInt(i);
				lastByte = recvByte();
				break;
			}
			case 'f':
			{
				float f;
				recv(&f,sizeof(float));
				m.addFloat(f);
				lastByte = recvByte();
				break;
			}
			default:
				break;
		}
	}
	return m;
}

Message Connection::receiveMessage()
{
	char b(255);
	while (b!=MSG_BEGIN)
		b=recvByte();
	char type = recvByte();
	Message m;
	switch (type) {
		case MSG_JOIN:
		case MSG_QUIT:
		case MSG_CHAT:
			m = parseMessage("s");
			break;
		case MSG_PLAYERPOS:
			m = parseMessage("sfff");
	}
	m.setType(type);
	return m;
}


bool Connection::login(std::string user, std::string pswd)
{
	sendByte(MSG_BEGIN);
	sendByte(MSG_LOGIN);
	send((void*)user.c_str(),user.size());
	sendByte(MSG_SEP);
	unsigned char* hash = SHA256((const unsigned char*)pswd.c_str(),pswd.size(),0);
	send(hash, SHA256_DIGEST_LENGTH);
	sendByte(MSG_END);
	Message m = receiveMessage();
	if (m.getType() == MSG_ACCEPT) {
		recvThread = SDL_CreateThread(recvLoop,this);
		return true;
	} else {
		socket = 0;
		return false;
	}
}


int Connection::recvLoop(void *cptr)
{
	Connection *c = (Connection*)cptr;
	Message m;
	while (c->isGood())
	{
		Debug::print(Debug::DEBUG,"receiving...");
		m = c->receiveMessage();
		switch (m.getType())
		{
			case MSG_JOIN:
			{
				Debug::print(Debug::DEBUG,"join: "+m.getString(0));
				Player p;
				p.setName(m.getString(0));
				GAME->addPlayer(p);
			}	break;
			case MSG_CHAT:
				GAME->getConsole()->print(m.getString(0));
				break;
			case MSG_PLAYERPOS:
			{
				Player &p = GAME->getPlayer(m.getString(0));
				p.setPosition(m.getFloat(0),m.getFloat(1),m.getFloat(2));
			}	break;
			default:
				Debug::print(Debug::WARNING, "Unknown message type:"
					+toString(m.getType()));
				break;
		}
	}
}


