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

#ifndef __CONNECTION_H__
#define __CONNECTION_H__
#include <string>
#include <vector>
#include <SDL/SDL_net.h>
#include <SDL/SDL_endian.h>

#define MSG_SEP 0
#define MSG_BEGIN 1
#define MSG_END 2

#define MSG_ACCEPT 3
#define MSG_KICK 4

#define MSG_LOGIN 5
#define MSG_JOIN 6
#define MSG_QUIT 7
#define MSG_PLAYERPOS 8
#define MSG_CHAT 9

class Message
{
	public:
		Message();
		
		void addByte(uint8_t b);
		void addString(std::string s);
		void addInt(int i);
		void addFloat(float f);
		
		int getByteCount();
		int getStringCount();
		int getIntCount();
		int getFloatCount();
		
		uint8_t getByte(int i);
		std::string getString(int i);
		int getInt(int i);
		float getFloat(int i);
		
		void setType(uint8_t t);
		uint8_t getType();
	private:
		std::vector<uint8_t> bytes;
		std::vector<std::string> strings;
		std::vector<int> ints;
		std::vector<float> floats;
		uint8_t type;
};

class Connection
{
	public:
		Connection(std::string host, int port);
		~Connection();
		bool isGood();
		void send(void*msg, int size);
		void sendByte(uint8_t i);
		void sendFloat(float f);
		uint8_t * recv(void* buffer, int size);
		uint8_t recvByte();
		Message parseMessage(std::string format); // format: b for byte, s for string, i for int, f for float
		Message receiveMessage();
		bool login(std::string user, std::string pswd); // pswd will be hashed
		static int recvLoop(void*cptr);
	private:
		IPaddress ip;
		TCPsocket socket;
		SDL_Thread * recvThread;
};

#endif //__CONNECTION_H__
