----------------------------------------------------------------------------------

 Network Library for Games :

 date    : 01/21/2019
 version : V0.6.4

----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
Major Update V0.6
------------------

Added :
- File transfert
- Dedicated server
- Request for cheking server existance
- Accept or reject commands was moved directly in the concerned objects
- Syncronizable sub-structures for NetworkObjects
- Possibility to run multiple clients on the same machine (Very usefull for testing)


Removed :
- Direct localhost connection (use CheckServerExistance instead)
- Callback for handling commands
- (Temporary) Some network statistics


----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
Minor Update V0.6.4
-------------------

Added :
- Multiple servers can now be open on the same machine
- (In example) Game simulation for server only configuration
- User can now call GetNotRespondingConnections that return all the 
  connections that seems down, so the user can handle what to do in this case
- User can now refuse connection in the newConnection callback
- The expected disconnections are now handle correctly from both side

Fixed :
- Multiple servers on the same machines are no more blended
- (in example) New player will no more spawn on other players


----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
 New / discovered issues :
 ------------------------

 - Transfered files can only contains ascii caracters
 - Servers and clients crash when unexpeted disconnection
 - If a connection is refused, the client is still syncro before being disconnected
 - Disconnections crash the client (and the server ?)

 ----------------------------------------------------------------------------------
