/*
  recieve_packet

  Usage: ./recieve_packet [<pulseLength>]

  Hacked from RFSniffer by @Dumbgenius

  RFSniffer was hacked from http://code.google.com/p/rc-switch/
  by @justy to provide a handy RF code sniffer
*/

#include "433Utils/rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>

RCSwitch mySwitch;



int main(int argc, char *argv[]) {

	// This pin is not the first pin on the RPi GPIO header!
	// Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
	// for more information.
	int PIN = 2;

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
	char seq, lastSeq, maxSeq, c;
	while(1) {
		listening = 1;
		first = 1;
		lastSeq = 0;
		maxSeq = 0;
		while(listening) {
			if (mySwitch.available()) {
				int rawPacket = mySwitch.getReceivedValue();
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
}

