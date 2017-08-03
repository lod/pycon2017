// Licence: GPLv2.0+

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

int main( int argc, char *argv[] ) {
	lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG | LLL_CLIENT, NULL);

	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	struct lws_context *context = lws_create_context( &info );

	struct lws_client_connect_info connect_info = {
		.port = 8042,
		.path = "/control",
		.address = "localhost",
		.context = context,
		.ssl_connection = 0,
		.host = lws_canonical_hostname(context),
		.origin = "origin",
		.ietf_version_or_minus_one = -1,
		.protocol = protocols[PROTOCOL_COLOUR].name,
	};


	unsigned int right = SUNXI_GPI(16);
	unsigned int left = SUNXI_GPH(21);

	sunxi_gpio_init();

	sunxi_gpio_set_cfgpin(right, SUNXI_GPIO_INPUT);
	sunxi_gpio_pullup(right, SUNXI_PULL_UP);

	sunxi_gpio_set_cfgpin(left, SUNXI_GPIO_INPUT);
	sunxi_gpio_pullup(left, SUNXI_PULL_UP);

	time_t old = 0;

	int old_rstate = sunxi_gpio_input(right);
	int old_lstate = sunxi_gpio_input(left);

	while( 1 )
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );

		if( !web_socket && tv.tv_sec != old ) {
			web_socket = lws_client_connect_via_info(&connect_info);
		}

		int rstate = sunxi_gpio_input(right);
		int lstate = sunxi_gpio_input(left);

		// No need to properly debounce, if it's changing it's changing

		if (rstate != old_rstate) {
			old_rstate = rstate;
			send_colour = rcolour;
			lws_callback_on_writable( web_socket );
		}

		if (lstate != old_lstate) {
			old_lstate = lstate;
			send_colour = lcolour;
			lws_callback_on_writable( web_socket );
		}

		old = tv.tv_sec;

		lws_service( context, 250 );
	}

	lws_context_destroy( context );

	return 0;
}
