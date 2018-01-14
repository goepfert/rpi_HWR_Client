#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>

#include "device_api.h"
#include "mapping.h"

std::string path("/home/pi/Projects/HWR/log/");

void readXBytes(int socketFD, int x, char* buffer);
void write_to_file(const std::string &text);
void error(char const *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char *argv[]) {

	int sockfd, portno, opt;

	struct sockaddr_in serv_addr;
	struct hostent *server;

	if (argc < 4) {
		fprintf(stderr,"usage %s hostname port opt\n", argv[0]);
		exit(1);
	}
	portno = atoi(argv[2]);
	opt = atoi(argv[3]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(1);
	}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
		error("ERROR connecting");
	}

	char buffer[PACKETSIZE];
	memset(buffer, 0, PACKETSIZE);

	if(opt==0) { //why the heck do I have to sent 3 chars
		strcpy(buffer, "GET"); //cacti
	} else {
		strcpy(buffer, "SET"); //fhem
	}

	if(write(sockfd,buffer,strlen(buffer)) < 0) {
		error("ERROR writing to socket");
	}
	memset(buffer, 0, PACKETSIZE);

	/*
	std::ostringstream ss;
	ss << "temp1:1 temp2:2.2 temp3:3.3 temp4:4.4 temp5:5.5 temp6:6.6 temp7:7.7 temp8:8.8 gas:9.9 elt:10.10";
	std::cout << ss.str() << std::endl;
	return 0;
	 */

	int size_read=0;
	//get number of packages
	size_read = read(sockfd, buffer, 3); //magic number
	//printf("bytes read: %i, PACKETSIZE:%i, buffer:%s\n", size_read, PACKETSIZE, buffer);

	if(size_read < 0) {
		error("ERROR size read");
	}

	int nPackages = atoi(buffer);

	std::vector<device_t> devices;

	//printf("number of packages to read: %i\n", nPackages);

	for(uint16_t devIdx=0; devIdx<nPackages; ++devIdx) {
		//printf("reading package: %i\n", devIdx);
		readXBytes(sockfd, PACKETSIZE, buffer);
		device_t dev;
		deserialize(buffer, &dev);
		devices.push_back(dev);
		//printMsg(&dev);
	}

	create_map();

	std::ostringstream ss;
	ss.precision(2);

	if(opt==1) {

		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer,80,"%F_%T HWR",timeinfo);
		std::string date(buffer);

		for(uint16_t idx=0; idx<devices.size(); ++idx){
			std::map<std::string, std::string>::iterator it = devicemap.find(devices.at(idx).id);
			if(it != devicemap.end())	{
				if( (it->second).compare("elt_counter")==0 || (it->second).compare("gas_counter")==0 ) {
					ss << date << " " << (it->second) << ":" << (*reinterpret_cast<int *>(&(devices.at(idx).value))) << std::endl;
				} else {
					ss << date << " " << (it->second) << ":" << devices.at(idx).value << std::endl;
				}
			}
		}
		write_to_file(ss.str());

	} else {
		for(uint16_t idx=0; idx<devices.size(); ++idx){
			std::map<std::string, std::string>::iterator it = devicemap.find(devices.at(idx).id);
			if(it != devicemap.end())	{
				if(idx>0) { ss << " "; }

				if( (it->second).compare("elt_counter")==0 || (it->second).compare("gas_counter")==0 ) {
					ss << (it->second) << ":" << (*reinterpret_cast<int *>(&(devices.at(idx).value)));
				} else {
					ss << (it->second) << ":" << fixed << devices.at(idx).value; //std::setprecision(2) <<
				}
			}
		}
		std::cout << ss.str() << std::endl;

	}

	return 0;
}

// This assumes buffer is at least x bytes long and that the socket is blocking
void readXBytes(int socketFD, int x, char* buffer) {

	int bytesRead = 0;
	int result;
	memset(buffer, 0, x);

	while(bytesRead < x) {
		result = read(socketFD, buffer+bytesRead, x-bytesRead);
		//printf("bytes read: %i, PACKETSIZE:%i, buffer:%s\n", result, PACKETSIZE, buffer);
		if (result < 1 ) {
			error("error reading data");
		}
		bytesRead += result;
	}

}


void write_to_file(const std::string &text) {
	FILE* pFile = fopen(path.append("rpi_HWR_Client.txt").data(), "a");
	fprintf(pFile, text.data());
	fclose(pFile);
}
