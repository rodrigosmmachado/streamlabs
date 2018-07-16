NOTICE: I wasn't able to complete all requirements in a 8h schedule. 

Due to the time constrainst set for this project, only 8 work hours, and the fact that there were no explicit instruction to avoid open source libraries, two free libraries are used:

1) Cereal

A C++11 serialization library

https://uscilab.github.io/cereal/

2) RTTR

A C++ reflection library.

http://www.rttr.org

Create two apps: server.exe and client.exe


CLIENT BRIEF  

Class ClientPipe : for client requests

Class RemoteCacheClient is responsible for requests for the server. This class contains the below methods:
		- invoke: call a method
		- call: call a function
		- callAsync: call a function asynch (not implemented)
		- create: instantiate a class
		
The Client code run some calls to the remote cache: instantiate 3 instances for DemoClass, calls ListObject funciton, and call some methods (not working).
		
SERVER BRIEF

Class ServerPipe: for server

Class RemoteControlledCache: Serve requests


Requests and responses are transmitted in json format, based on jsonrpc, with minor modifications. 


