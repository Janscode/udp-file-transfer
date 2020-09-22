Usage:
Type make in either the client or server directory to build the executables. 
The executables are called client and server, and they are used as specified in the assignment pdf.

Implementation:
My program sends the command with one wasted byte to the server, which then fills that byte to tell the client how to process the rest of the servers response. 
In the case that a file is being transfered, the fread funciton is used to fill the buffer which is then sent and the fwrite funciton is used to write to a file. The first byte of the buffer is always wasted so that when the transfer is complete a single byte can be sent as an indication that the file can be closed.
