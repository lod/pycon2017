#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "gpio_lib.h"

static struct lws *web_socket = NULL;

#define RX_BUFFER_BYTES (20)

char rcolour[8] = "#FF0033";
char lcolour[8] = "#00CC00";
char* send_colour = NULL;

static int callback (struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	switch( reason )
	{
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lws_callback_on_writable( wsi );
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
			/* Handle incomming messages here. */
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			if (send_colour) {
				unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
				unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
				size_t n = sprintf( (char *)p, "{\"colour\":\"%s\"}", send_colour );
				lws_write( wsi, p, n, LWS_WRITE_TEXT );
				send_colour = NULL;
			}
			break;
		}

		case LWS_CALLBACK_CLOSED:
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			web_socket = NULL;
			break;

		default:
			break;
	}

	return 0;
}

enum protocols
{
	PROTOCOL_COLOUR = 0,
	PROTOCOL_COUNT
};

static struct lws_protocols protocols[] =
{
	{
		"colour-protocol",
		callback,
		0,
		RX_BUFFER_BYTES,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};
