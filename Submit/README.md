Use "make client" to build the client executable adn "make server" to build server executable.

I wrote a program that repeatedly sends chunks of data using sendto when reading large files, and I used the first byte of each packet as a sort of header to indicate special occasions such as when the entire file had been read. 
