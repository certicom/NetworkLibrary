----------------------------------------------------------------------------------

 Network Library for Games :

 date    : 01/24/2019
 version : V0.6.7

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
Minor Update V0.6.7
-------------------

Added :
- User interface for simple file syncronization
- For internal uses, file transferts can now be specific for one client

Fixed :
- File transfert is now correctly handle and can transfert large files



----------------------------------------------------------------------------------
----------------------------------------------------------------------------------
 New / discovered issues :
 ------------------------

 - Servers and clients crash when unexpeted disconnection
 - If a connection is refused, the client is still syncro before being disconnected
 - Disconnections crash the client (and the server ?)

 ----------------------------------------------------------------------------------
