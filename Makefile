#Makefile for pi-transmitter

CXXFLAGS=-DRPI #Defines CXXFLAGS for rc-switch makefile

all: send_packet recieve_packet

send_packet: send_packet.o 433Utils/rc-switch/RCSwitch.o
	g++ -DRPI -o send_packet send_packet.o 433Utils/rc-switch/RCSwitch.o -lwiringPi

send_packet.o:
	g++ -DRPI -Wall -c send_packet.cpp

recieve_packet: recieve_packet.o 433Utils/rc-switch/RCSwitch.o
	g++ -DRPI -o recieve_packet recieve_packet.o 433Utils/rc-switch/RCSwitch.o -lwiringPi

recieve_packet.o:
	g++ -DRPI -Wall -c recieve_packet.cpp


clean:
	$(RM) 433Utils/rc-switch/*.o *.o send_packet recieve_packet
