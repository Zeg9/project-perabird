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



import struct
from protocol import *

#msgspec:
#	For user, password:
#		str:hexstr

def parseMessage(msgspec, sock):
	parsed = []
	for t in msgspec.split(':'):
		c = ''
		o = -1
		if t == 'str':
			s = ''
			while 1:
				c, o = recv(sock)
				if o in (MSG_SEP,MSG_END): break
				s += c
			parsed.append(s)
		if t == 'hexstr':
			s = ''
			while 1:
				o = recv(sock)[1]
				if o in (MSG_SEP,MSG_END): break
				c = hex(o).replace('0x','')
				if len(c)==1: c='0'+c
				s += c
			parsed.append(s)
		if t == 'float':
			parsed.append(struct.unpack('f',sock.recv(struct.calcsize('f')))[0])
			c,o = recv(sock)
		if o != MSG_SEP: # can be MSG_END
			break
	return parsed
