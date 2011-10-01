README - To COMPILE AND RUN CHORD LITE 1.0

1.) Run the make file. Type make in the directory that contains ChordLib.c and the rest of the code.
2.) Start the zeroeth peer. Type ./peerZero in the directory containing ChorLib.c and the rest of the code.
3.) Run startChord.sh. This will copy the peer code into new directories by the name peer1, peer2, peer3 etc.
4.) Run a peer by going into the appropriate directory. Any directory you like! The number after peer will help
    distinguish peers.
5.) The rfc's downloaded will be downloaded into directory by the name rfcBase under the peer[0-9] directories corresponding
    to the peers.

FOR PEERZERO CHANGE THE FOLLOWING ATTRIBUTES in ChordLib.h

#define PEER_ZERO_IP "127.0.0.1" -----> IP address of your machine in quotes
#define PEER_ZERO_PORT 5000      -----> PORT on which peerZero needs to be started

FOR ANY OTHER PEER RUN AS FOLLOWS

./peer <ipAddressOfYourMachine> <portOnWhichPeerWillListen>

TO MAKE A PEER LEAVE
Type "leave" on the command line window on which the peer is running.(Don't type in quotes, they are just for emphasis)

Note: Please ensure that you run the peer as ./peer <IP> <PORT> > <fileName> 
      This will redirect debugging output to the fileName file and help type in commands without confusion.

Also you can use sendHome script from anywhere to make a peer exit
./sendHome <peerID> <IP> <port>

TO TRIGGER AN RFC REQUEST FROM A PEER
Type following in the command line window where the peer is running
GET <fourDigitRFCnumber>
If the rfcNo is only 3 digits, make most significant digit as zero. For eg. 345 should be entered as GET 0345.
