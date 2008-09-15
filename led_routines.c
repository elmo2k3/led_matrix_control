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

#include <inttypes.h>
#include <string.h>

#include "led_routines.h"
#include "arial_bold_14.h"

static void hilfsarray_to_normal(void);
static uint16_t charGetStart(char c);

/* Diese Arrays werden nur zur Uebertragung ans Modul genutzt */
static uint16_t RED[4][16];
static uint16_t GREEN[4][16];

/* Dies sind die eigentlich genutzten Arrays. Grund: 
 * einfachere Handhabung! Jedes Element entspricht genau einer Spalte
 */
static uint16_t column_red[512];
static uint16_t column_green[512];

static int position = 0;

static uint16_t x,y=1;

void updateDisplay(int client_sock)
{
	int bytes_send;
	bytes_send = send(client_sock, &RED, sizeof(RED),0);
	bytes_send = send(client_sock, &GREEN, sizeof(GREEN),0);

}

/* Achtung, funktioniert derzeit nur fuer Arial_Bold_14 ! */
static uint16_t charGetStart(char c)
{
	uint8_t first_char = Arial_Bold_14[4];
	uint8_t char_count = Arial_Bold_14[5];
	uint8_t char_width = Arial_Bold_14[6+c-first_char];
	uint8_t char_height = Arial_Bold_14[3];

	uint8_t factor = 1;

	if(char_height > 8)
		factor = 2;

	uint8_t counter;
	uint16_t position = 0;

	for(counter=0;counter<(c-first_char);counter++)
	{
		position += Arial_Bold_14[6+counter]*factor;
	}

	return position;
}


static void hilfsarray_to_normal(void)
{
	int i,p,m;
	memset(&RED,0,sizeof(RED));
	memset(&GREEN,0,sizeof(GREEN));

	for(m=0;m<4;m++)
	{
		for(i=0;i<16;i++)
		{
			for(p=0;p<16;p++)
			{
				RED[m][i+y] |= ((column_red[p+m*16] & (1<<i))>>(i)<<p);
				GREEN[m][i+y] |= ((column_green[p+m*16] & (1<<i))>>(i)<<p);
			}
		}
	}
}

void putChar(char c, uint8_t color)
{
	/* Leerzeichen abfangen */
	if(c == 32)
	{
		x += 4;
		return;
	}

	uint8_t first_char = Arial_Bold_14[4];
	uint8_t char_count = Arial_Bold_14[5];
	uint8_t char_width = Arial_Bold_14[6+c-first_char];

	uint16_t start = charGetStart(c);

	uint8_t i,p;

	for(i=0;i<char_width;i++)
	{
		if(color == COLOR_RED)
		{
			column_red[i+x] = Arial_Bold_14[6+char_count+start+i];
		}
		else if(color == COLOR_GREEN)
		{
			column_green[i+x] = Arial_Bold_14[6+char_count+start+i];
		}
		else if(color == COLOR_AMBER)
		{
			column_red[i+x] = Arial_Bold_14[6+char_count+start+i];
			column_green[i+x] = Arial_Bold_14[6+char_count+start+i];
		}
	}
	for(i=0;i<char_width;i++)
	{
		if(color == COLOR_RED)
		{
			/* Man erklaere mir was ich hier geschrieben. Aber funktionieren tuts! :-) */
			column_red[i+x] |= Arial_Bold_14[6+char_count+start+i+char_width]<<6;
		}
		else if(color == COLOR_GREEN)
		{
			column_green[i+x] |= Arial_Bold_14[6+char_count+start+i+char_width]<<6;
		}
		else if(color == COLOR_AMBER)
		{
			column_red[i+x] |= Arial_Bold_14[6+char_count+start+i+char_width]<<6;
			column_green[i+x] |= Arial_Bold_14[6+char_count+start+i+char_width]<<6;
		}
	}

	hilfsarray_to_normal();

	/* Bei Bedarf wieder an den Anfang gehen */
	if(x + char_width +1 < 511)
		x += char_width + 1;
	else
		x = 0;
}

void putString(char *string, uint8_t color)
{
	while(*string)
	{
		putChar(*string++,color);
	}
}

void clearScreen(void)
{
	memset(&column_red,0,sizeof(column_red));
	memset(&column_green,0,sizeof(column_green));
}


void shiftLeft(void)
{
	uint16_t buffer_red,buffer_green;
	int counter;
	
	buffer_red = column_red[0];
	buffer_green = column_green[0];

	for(counter=0;counter<511;counter++)
	{
		column_red[counter] = column_red[counter+1];
		column_green[counter] = column_green[counter+1];
	}

	column_red[511] = buffer_red;
	column_green[511] = buffer_green;

	if(position)
		position -= 1;
	else
		position = 511;
	hilfsarray_to_normal();
}
