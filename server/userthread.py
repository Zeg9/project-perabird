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


from user import *
from protocol import *
from auth import *
from msgparser import *
from commands import *

def userthread(user,users):
	sock = user.sock
	addr = user.addr
	print(addr[0],'connected')
	while 1:
		o = recv(sock)[1]
		if not o: break
		if o != MSG_BEGIN: continue
		msgType = recv(sock)[1]
		if msgType == MSG_LOGIN and user.name == "":
			parsed = parseMessage('str:hexstr',sock)
			if len(parsed) != 2: continue
			name, pswd = parsed
			if auth(name,pswd):
				user.name = name
				print(user.name,"logged in from",addr[0])
				user.lock()
				send(user.sock,MSG_BEGIN)
				send(user.sock,MSG_ACCEPT)
				send(user.sock,MSG_END)
				user.unlock()
				for u in users:
					if u != user:
						u.lock()
						send(u.sock,MSG_BEGIN)
						send(u.sock,MSG_JOIN)
						send(u.sock,user.name)
						send(u.sock,MSG_END)
						u.unlock()
					user.lock()
					send(user.sock,MSG_BEGIN)
					send(user.sock,MSG_JOIN)
					send(user.sock,u.name)
					send(user.sock,MSG_END)
					user.unlock()
				for u in users:
					sendChat(u,'*** '+user.name+' joined the game')
			else:
				print(addr[0],"tried to login as",name)
				user.lock()
				send(user.sock,MSG_BEGIN)
				send(user.sock,MSG_KICK)
				send(user.sock,MSG_END)
				user.unlock()
				break
		if msgType == MSG_CHAT:
			parsed = parseMessage('str',sock)
			if len(parsed) != 1: continue
			msg = parsed[0]
			if len(msg)>1 and msg[0] == '/':
				handleCommand(user,msg[1:],users)
			else:
				for u in users:
					sendChat(u,'<'+user.name+'> '+msg)
				sendChat(None,'[Chat] '+user.name+': '+msg)
		if msgType == MSG_PLAYERPOS:
			parsed = parseMessage('float:float:float',sock)
			user.pos = tuple(parsed)
			for u in users:
				sendPosition(user,u)
	print(addr[0],"lost connection")
	user.sock.close()
	users.remove(user)
	for u in users:
		sendChat(u,'*** '+user.name+' left the game')

