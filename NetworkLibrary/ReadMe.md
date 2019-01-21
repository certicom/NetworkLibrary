
------------------------------------------------------------------------------------------------------------------------
HOW TO USE THE LIBRAIRY :
-----------------------------

#### Infos :
All high level functions are available in the static communication class (create server / client, get servers, spawn object etc..)  
You CAN NOT manually instanciate any object inherited from NetworkObject  
The only public possibility to do that is to use the function SpawnFromServer.  
This function is only callable from server side (if not it will just do nothing).  
When this function is called the new object will be automatically created for all clients.  
At the end of the application, you must use the ShutDownAllCommunications function in order to properly close everything.  

#### Example :
To complete those information an exemple is included. So you look in the main.cpp and playerObject.cpp files to better understand how it works.  
(ExampleCore.cpp is just for displaying the example there is nothing really intersting in it)  

#### Create server :
To create a server you just have to call the StartServer function (See documentation of StartServer)  
For now, it is possible to create only one server per application.  

#### Create client :
To create a client you just have to call the StartClient function (See documentation of StartClient)  
For now, it is possible to create only one client per application.  

#### Close connections :
For now the only way to close one or more connection is to use the functions CloseClient, CloseServer or ShutDownAllCommunications.  

#### Callbacks :
For server side, you can set a connection callback with the function SetNewConnectionCallback that will be call on every new connected client.  
can be very usefull to instanciate a new player or having conditions to accept connections.  
From any inherited class from NetworkObject, you can override the function WasUpdated that append when the server just updated this object.  

#### Spawn objects :
You can not manually instanciate object inherited from NetworkObject, instead you must call the SpawnFromServer function, that only work from server side.  
this feature is here to avoid redundant objects or fake instanciations.  
IMPORTANT, to allow client to create object given by the server, you must call the function AddInstanciableType for each type that you want to create.  

#### Find local servers :
Get the list of all the available local servers is very easy, just call the GetAvailableServers function. This function return a list of Net::InfoServer.  
This structure contains many information about the server, like the ipAdress, the number of clients, etc...  
Note : it is possible to use a max time parameter for this function, that allow you to get only the server with recent communication.
(This parameter will soon be removed to automatically return only alive servers)  

#### Find non local servers :
You can use the function CheckServerExistance to try to contact a non local server. This function do not return anything and this is not a connection attempt.  
If the server exist it will appear in the AvailableServers list.  

#### Connect to a server :
You can only connect to a server you know. The servers that accept connections will broacast their informations on the local network and you can use CheckServerExistance to find a non local server. 
All the servers you know are in the AvailableServers list. To connect to one of them, you need to call Client::Connect with the corresponding InfoServer as parameter. (For now it is recommended to connect with TCP protocol by add true in second parameter)  

#### #### Do something :
All objects inherited from NetworkObjet must override the function ReceiveCommand(Net::Command&).  
Clients can not really do anything, so in order to inform the server that you want to do something, you must send a command, if this command is accepted.  
it will be resend to this exact object on all clients, so you also must be able to understand this command in the ReceiveCommand function. (see read/write command)  

#### Read command :
Undestand a command is simple because you know the content of the sending command since your must write it yourself.  
Read a command must be done with the overrided function from NetworkObject, ReceiveCommand.  
This function give you the Net::Command that contain all the information. You can get the command code with GetCommandCode() and any variable with GetDataFromId.  
GetDataFromId require you to provide the id of the data and his type. This operation is simply the reverse of write command.  
IMPORTANT (since V0.6), your ReceiveCommand overrided function MUST call Accept() or Reject() of the command to indicate if the command is valid (this will matter only from server side)  

#### Write command :
The current way to write command will probably change soon since too much line from end user are required.  
Write command must be done in 3 steps :  
- Create the data package -> Net::NetworkData l_data;  
- Add data in that command (256 data max) -> l_data << Net::Data(id, value);  
- send command -> SendCommand(CODE, l_data);  
Note that you can add a code/id to know what this command is about  

#### Inherit from NetworkObject :
All objects that you want to be duplicated and syncronized on all your clients must be inherited from NetworkObject.  
To make it work correctly you need to do two things:  
- In your constructors you must call the base constructors with as parameters all the addresses of the variables you want to syncronize.  
Note that those constructors will be call from server side only !  
- You need to add a default constructor with also as parameters all the addresses of the variables you want to syncronize.  
Note that this constructor will be call from client side to instanciate objects.  
IMPORTANT with the default constructor, only the syncronized variables will be automatically initialized, you must initialize all the others manually.  

#### More interface features :
The client entity (and server soon) also provide many functions to know their current status.  
(IsConnected, GetServerConnection, GetName, GetStats)  


------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------
FEATURES THAT WILL SOON BE ADDED :
-----------------------------

 - System for file auto sharing from server side
 - System to send more customized command
 - UDP full support
 - No more forced default constructor for network objects (interesting fact in C++ this is impossible because of what are the variadic functions)
 - Fully implemented object update system (with working network priority)
 - Proper disconnection by sending the information
 - End user interface to end a connection
 - Thread for loading the initial data when connected
 - Clock syncronization
 - Nice and simple way to not send data to all clients (In the anti cheat way)
 - Data transmission / reception per seconde
 - Thread for sending files


------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------
INTERESTING FEATURE IDEAS :
-----------------------------

- When loosing the connection with the server, one client becomes server
- data compression
- RSA / DES encryption
- Folder sharing
- Per to Per connection with p2p server



------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------
PROTOCOLS USED FOR COMMUNICATION AND TRANSMITTING OBJECTS :
-----------------------------

##### If the communication protocol is UDP :  
 0.1 uint8: application id of the receiver  
 0.2 uint8: application id of the emitter  

##### All protocols :  
 1 uint16: Protocol code  

##### Protocol for Update data in the packets :
 2 uint8: number of concerned objects  
 for each objects : use Protocole for Object  

##### Protocol for custom command :
 2 String: idUser (only for authentication control, remove it ????)  
 3 uint16: custom command code  
 read command parameters : use Protocole for Object (the id object is useless)  

##### Protocol for Delete object :
 2 uint8: number of concerned objects  
	for each object:  
	3 uint16: idObject  
	4 String: Custom info ????  


##### Protocol for ending connection : 
 2 String: reason(for now not added in the packets)  

##### Protocol for new connection :
 2 String: User name  
 3 uint16: port to use for sending on this connection  

##### Protocol for new object :
 2 uint8: number of concerned objects  
 for each objects  
	3 string: type id  
	use Protocole for Object  

##### Protocol for server broacast :
 2 String: ip address of the origin  
 3 Uint16: the port to communicate with the server  
 4 String: server name  
 5 Uint8: current number of connected clients  
 6 Uint8: max connextions  
 7 String: custom infos  

##### Protocol for file transfert (Not the real FTP of course^^):
 2 bool: start file  
 3 string: file name  
 if not start file  
 4 char: all the file  

##### Protocol for ping
 3 Int32: Current time uncorrected  
 4 bool: ask for a ping back  

##### Protocol for asking clock syncronization
 2 Uint32: Server time (protocol only send by the server)  


##### Protocol for Variable
 x.1 uint8: variable id  
 x.2 uint8: variable type  
 x.3 template: value  


##### Protocol for Object
 x.1 uint32: id object  
 x.2 uint8: number of variables  
	for each variables : use Protocol for Variable  


------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------
CLOCK SYNCRONIZATION SYSTEM :
-----------------------------

For syncronized two clock, the server will send a request for clock syncronization with his current time.  
Then the client send a ping to the server that will send him back. When the client receive the ping from the server, the latency
will be : Latency = (ReceiveTime - SendTime)/2  
The correct reading of the server clock became : RealServerTime = ServerTime + Latency  
Then the offset client time became : OffsetClient = RealServerTime - ClientTime  
So we can now send packet with good aproximation of server time : CurrentServerTime = CurrentClientTime + OffsetClient  
This offset operation may be necessary to repeat several time to get a more accurate average  


------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------
INTERNAL WORK FOR COMMAND COMMUNICATION:
-----------------------------

 - Client : send a command from an object
 - Server : read the command and ask to the game if the command is accepted
 - Game : Accept or reject the request (if it reject it, we stop here and nothing is resend) 
 - Server : if the request is accepted, we resend it to the localhost client (that have the special right to instanciate objects from server)
 - Server : if the previous operation has created objects they will be send to all clients
 - Client : It will received the new objects, read them, then instanciate them
 - Server : Then the command will be resend to all other clients
 - Client : The command will be interpreted by the equivalent emitter object in all clients
 ------------------------------------------------------------------------------------------------------------------------
