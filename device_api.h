/*
 * ds18b20_api.h
 *
 *  Created on: Mar 7, 2016
 *      Author: goepfert
 */

#ifndef DEVICE_API_H_
#define DEVICE_API_H_

#include <iostream>
#include <cstring>

#define IDSIZE 16
#define PACKETSIZE sizeof(device_t)

using namespace std;

struct device_t {
		char id[IDSIZE];
		float value;
};

void serialize(device_t* msgPacket, char *data);
void deserialize(char *data, device_t* msgPacket);
void printMsg(device_t* msgPacket);

void serialize(device_t* msgPacket, char *data) {
	memset(data, 0, PACKETSIZE);
	memcpy(data, msgPacket, PACKETSIZE);
}

void deserialize(char *data, device_t* msgPacket) {
	memcpy(msgPacket, data, PACKETSIZE);
}

void printMsg(device_t* msgPacket) {

	cout << msgPacket->id << endl;
	cout << msgPacket->value << endl;
}

#endif /* DEVICE_API_H_ */
