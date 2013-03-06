#!/usr/bin/python
from sys import argv
from hashlib import sha256
if len(argv) != 3:
	print("Usage:",argv[0],"<name> <password>")
	print("This script outputs a line that can be added to server's auth.db")
	exit(1)
o = ""
o += argv[1]
o += ':'
o += sha256(sha256(argv[2].encode()).hexdigest().encode()).hexdigest()

print(o)
exit(0)

