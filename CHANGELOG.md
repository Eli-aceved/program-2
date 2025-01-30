## Version 0.1.5 - Modified server to handle a client termination (partially)
- When server detects a connection closed, it prints the connection was closed
- Fixed issue: message recevied did not print on server terminal

## Version 0.1.4 - Server modified to use poll() to handle multiple clients
- Changes in server.c

## Version 0.1.3 - Server modified to loop until ^c
- Modified server.c (lines 45-48)

## Version 0.1.2 - sendPDU() and recvPDU() complete and passed testing
- Ran into issues, now it should be up to date
- recvPDU() done
- Modifications done on cclient and server (1 line of code change on each)

## Version 0.1.1 - sendPDU() and recvPDU() complete and passed testing
- recvPDU() done
- Modifications done on cclient and server (1 line of code change on each)

## Version 0.1.0 - Ready for testing sendPDU()
- Created pdu_io.c and .h which will hold functions sendPDU and recvPDU
- First few versions will be a part of Lab3 mini program
- Modifications on cclinet for sending
- sendPDU() done in pdu_io.c

*Note: pdu_io.c and .h are not complete.