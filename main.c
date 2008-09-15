/*
 * Copyright (C) 2008 Bjoern Biesenbach <bjoern@bjoern-b.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "led_routines.h"

#define LMS_IP "192.168.0.93"
#define LMS_PORT 9328

static int client_sock;


int initNetwork(void)
{
	struct sockaddr_in server,client;
	int recv_size;
	int send_size;
	unsigned char command;

	client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(client_sock < 0)
		printf("Client_sock konnte nicht erstellt werden\n");
	server.sin_family = AF_INET;
	server.sin_port = htons(LMS_PORT);
	inet_aton(LMS_IP, &server.sin_addr);
	

	if(connect(client_sock, (struct sockaddr*)&server, sizeof(server)) != 0)
	{
		printf("Konnte nicht verbinden\n");
		return -1;
	}	

	return 0;
}


int main(int argc, char *argv[])
{
	char stringToPrint[64];

	clearScreen();

	initNetwork();

	sprintf(stringToPrint,"Hallo, dies ist ein Test! !\"$%&/()=?),.-;:_#'`*+~");
	putString(stringToPrint,COLOR_RED);
	
	putString(" :-) :-D ",COLOR_GREEN);
	putString("Einiges noch zu tun!",COLOR_AMBER);

	updateDisplay(client_sock);

	sleep(1);

	while(1)
	{
		shiftLeft();
		updateDisplay(client_sock);
		usleep(20000);
	}


	close(client_sock);

	return 0;
}
	
