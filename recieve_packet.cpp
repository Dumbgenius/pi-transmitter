/*
  recieve_packet

  Usage: ./recieve_packet packet_type
  packet_type - Type of packet: 'string' or 'string2'

  Hacked from RFSniffer by @Dumbgenius

  RFSniffer was hacked from http://code.google.com/p/rc-switch/
  by @justy to provide a handy RF code sniffer
*/

#include "433Utils/rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

RCSwitch mySwitch;



int main(int argc, char *argv[]) {

	// This pin is not the first pin on the RPi GPIO header!
	// Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
	// for more information.
	//This pin is pin 27 on the Raspberry Pi Model B+
	int PIN = 2;

	if (argc == 1) {
		printf("Usage: %s packettype\n", argv[0]);
		printf("packet_type   - Type of packet: 'string' or 'string2'\n");
		return -1;
	}

	//Check if packet type is valid
	enum PacketType {string, string2};
	PacketType packet_type;
	if (std::string(argv[1]) == "string") {
		packet_type = string;
	} else if (std::string(argv[1]) == "string2") {
		packet_type = string2;
	} else {
		printf("Invalid packet type. Valid types are: 'string', 'string2'\n");
		return -1;
	}


	if(wiringPiSetup() == -1) {
		printf("wiringPiSetup failed, exiting...");
		return 1;
	}

	setvbuf(stdout, NULL, _IONBF, 0);
	int pulseLength = 0;
	if (argv[1] != NULL) pulseLength = atoi(argv[1]);

	mySwitch = RCSwitch();
	if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
	mySwitch.enableReceive(PIN);  // Receiver on interrupt 0 => that is pin #2

	bool listening, first;
	char seq, lastSeq, maxSeq, c, c1, c2, c3;


	switch (packet_type) {
	case string:
		while(1) {
			listening = 1;
			first = 1;
			lastSeq = 0;
			maxSeq = 0;
			while(listening) {
				if (mySwitch.available()) {
					unsigned int rawPacket = mySwitch.getReceivedValue();
					if (rawPacket == 0) {
						printf("Unknown encoding, aborting.\n");
						listening = 0;
					} else {
						c = (~255 & rawPacket) >> 8;
						seq = 255 & rawPacket;

						if (first) {
							if (seq != 0) {
								continue;
							} else { //seq==0
								maxSeq = c;
							}
							first = 0;
							printf("Beginning transmission of length %i...\n", maxSeq);
						}

						if (seq > lastSeq) {
							printf("%c", c);
						}

						lastSeq = seq;
						if (seq == maxSeq) {
							listening = 0;
						}
					}
					mySwitch.resetAvailable();
				}
			}
			printf("\nDone.\n\n");
		}
	return 0;

	case string2:
		while(1) {
			listening = 1;
			first = 1;
			lastSeq = 0;
			maxSeq = 0;
			while(listening) {
				if (mySwitch.available()) {
					unsigned int rawPacket = mySwitch.getReceivedValue();
					if (rawPacket == 0) {
						printf("Unknown encoding, aborting.\n");
						listening = 0;
					} else {
						c1 = (~255 & ~(255<<8) & ~(255<<16) & rawPacket) >> 8; 	//FF000000 & rawPacket
						c2 = (~255 & ~(255<<8) & ~(255<<24) & rawPacket) >> 8; 	//00FF0000 & rawPacket
						c3 = (~255 & ~(255<<16) & ~(255<<24) & rawPacket) >> 8;	//0000FF00 & rawPacket
						seq = 255 & rawPacket;									//000000FF & rawPacket

						if (first) {
							if (seq != 0) {
								continue;
							} else { //seq==0
								maxSeq = c1;
							}
							first = 0;
							printf("Beginning transmission of length %i...\n", maxSeq);
						}

						if (seq > lastSeq) {
							printf("%c%c%c", c1, c2, c3);
						}

						lastSeq = seq;
						if (seq == maxSeq) {
							listening = 0;
						}
					}
					mySwitch.resetAvailable();
				}
			}
			printf("\nDone.\n\n");
		}
	return 0;

	default: return -1;
	}
}

