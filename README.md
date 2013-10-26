dynamixel_zmq
================================

Linux usermode service for Dynamixel AX12/AX18 servos with ZeroMQ interface

purpose
-------------------------
The main purpose is to allow fast and easy access to a Dynamixel servo network.
I did this already with Python but cos of performance I decided to write this tool.
CRC and buffer management is done in CPP so that should increase performance quite a bit.

ZeroMQ even allows us to do this in a destributed way, allowing access over the network.

I'm also planning to implement a client which will provide all the NUKE functionality, but thats the second step.


install
-------------------------

... on gentoo
------------
You'll need:
  * dev-libs/boost
  * net-libs/zeromq
  * net-libs/cppzmq
  * dev-libs/msgpack

<pre>
git clone https://github.com/erazor83/dynamixel_zmq
cd dynamixel_zmq
cmake .
make
</pre>

There are some options to enable different additional features:
  * WITH_PYPOSE - enable pypose special packages
  * WITH_TROSSEN - enable Trossen Commander Support (needs libdynamixel with the same feature)

<pre>
cmake -DWITH_SHARED=1 .
</pre>

links
-------------------------

ZeroMQ
------------
  * http://zeromq.org/

dynapi
------------
  * http://www.jrl.cs.uni-frankfurt.de/web/projects/dynapi-robotis-dynamixel-c-api/

Python
------------
  * http://www.python.org/
  * https://code.google.com/p/pydynamixel/

NUKE
------------
  * https://code.google.com/p/arbotix/wiki/GettingSetup

