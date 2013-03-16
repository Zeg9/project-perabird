#    Copyright (C) 2012 Zeg9
#    
#    This file is part of Project Perabird.
#    Project Perabird is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Project Perabird is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Project Perabird.  If not, see <http://www.gnu.org/licenses/>


MSG_SEP = 0 # used as a separator in messages
MSG_BEGIN = 1 # used as first byte of a message
MSG_END = 2 # used as last byte of a message

MSG_ACCEPT = 3 # player accepted after login
MSG_KICK = 4 # the name says it all :D

MSG_LOGIN = 5 # login message: user and (hashed) password
MSG_JOIN = 6 # someone joined
MSG_QUIT = 7 # someone quit
MSG_PLAYERPOS = 8 # player position update
MSG_CHAT = 9 # a chat message

import struct

def recv(sock):
	data = sock.recv(1)
	if not data: return None, None
	print(chr(ord(data)), ord(data))
	return chr(ord(data)), ord(data)

def send(sock,data):
	msg = data
	if type(msg) == int: msg = chr(msg)
	if type(msg) == str: msg = msg.encode()
	sock.send(msg)

def sendChat(user,msg):
	if user == None:
		print(msg)
	else:
		user.lock()
		send(user.sock,MSG_BEGIN)
		send(user.sock,MSG_CHAT)
		send(user.sock,msg)
		send(user.sock,MSG_END)
		user.unlock()

def sendPosition(source,target):
	x,y,z = source.pos
	target.lock()
	send(target.sock,MSG_BEGIN)
	send(target.sock,MSG_PLAYERPOS)
	send(target.sock,source.name)
	# TODO Identify users by id, an int, so the name isn't set everytime.
	send(target.sock,MSG_SEP)
	send(target.sock,struct.pack('f',x))
	send(target.sock,MSG_SEP)
	send(target.sock,struct.pack('f',y))
	send(target.sock,MSG_SEP)
	send(target.sock,struct.pack('f',z))
	send(target.sock,MSG_END)
	target.unlock()


