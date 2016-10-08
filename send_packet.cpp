/*
Usage: ./codesend packettype packet [packet2, packet3...]
packettype    - Type of packet: 'string' or 'string2'
packet        - Data to include in packet
packet2, etc  - Additional packets to be sent after the first

 'send_packet' hacked from 'codesend' by @ninjablocks
 'codesend' hacked from 'send' by @justy

 - The provided rc_switch 'send' command uses the form systemCode, unitCode, command
   which is not suitable for our purposes.  Instead, we call
   send(code, length); // where length is always 24 and code is simply the code
   we find using the RF_sniffer.ino Arduino sketch.

*/
#include "433Utils/rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

int main(int argc, char *argv[]) {

    // This pin is not the first pin on the RPi GPIO header!
    // Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
    // for more information.
    int PIN = 0;


    // If no command line argument is given, print the help text
    if (argc == 1) {
        printf("Usage: %s packettype packet [packet2, packet3...]\n", argv[0]);
        printf("packettype    - Type of packet: 'string' or 'string2'\n");
        printf("packet        - Data to include in packet\n");
        printf("packet2, etc  - Additional packets to be sent after the first\n");
        return -1;
    }

	//Check if packet type is valid
	enum PacketType {string, string2};
	PacketType packet_type;
	if (std::string(argv[1]) == "string") {
		packet_type = string;
	}
	if (std::string(argv[1]) == "string2") {
		packet_type = string2;
	} else {
		printf("Invalid packet type. Valid types are: 'string', 'string2'\n");
		return -1;
	}

	//Check if we actually have any packet data
	if (argc == 2) {
		printf("Need packet data.\n");
        printf("Usage: %s packettype packet [packet2, packet3...]\n", argv[0]);
		return -1;
	}

	//Setup switch for transmission
	if (wiringPiSetup () == -1) {return 1;}
	RCSwitch mySwitch = RCSwitch();
	mySwitch.enableTransmit(PIN);


	//For each set of inputted packet data:
	switch (packet_type) {
	case string:
		for (int i=2; i<argc; i++) {
			//Turn the input into a sequence of packets
			std::string input = argv[i];
			printf("Sending packet #0: []\n");
			mySwitch.send(input.length()<<8, 16);
			for (char j=0; j<input.length(); j++) {
				//Make packet
				unsigned short packet = ((unsigned short)input[j])<<8 | ((unsigned short)j+1); //packet is 2 bytes: [data_char][sequence]

				//Send packet
				printf("Sending packet #%i: [%c]\n", j+1, input[j]);
				mySwitch.send(packet, 16);
			}
		}
		return 0;

	case string2:
		for (int i=2; i<argc; i++) {
			//Turn the input into a sequence of packets
			std::string input = argv[i];
			if (input.length() % 3 >= 1) {input += "\0";}
			if (input.length() % 3 == 1) {input += "\0";} //pad out the string to a multiple of 3
			printf("Sending initial packet\n");
			mySwitch.send((unsigned int)(input.length()/3)<<24 , 32);

			for (char j=2; j<input.length(); j+=3) {
				//Make packet
				unsigned int packet = ((unsigned int)input[j-2]<<24) | ((unsigned int)input[j-1]<<16) | ((unsigned int)input[j]<<8) | ((j+1)/3);
				//Packet is 4 bytes: [data1][data2][data3][sequence]

				//Send packet
				printf("Sending packet #%i: [%c%c%c]\n", (j+1)/3, input[j-2], input[j-1], input[j]);
				mySwitch.send(packet, 32);
			}
		}
		return 0;

	default: return -1;
	}
}
