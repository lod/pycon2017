int main( int argc, char *argv[] ) {
	lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG | LLL_CLIENT, NULL);

	struct lws_context_creation_info info = {
		.port = CONTEXT_PORT_NO_LISTEN;
		.protocols = protocols;
		.gid = -1;
		.uid = -1;
	};

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

	while( 1 ) {
		struct timeval tv;
		gettimeofday( &tv, NULL );

		if( !web_socket && tv.tv_sec != old ) {
			web_socket = lws_client_connect_via_info(&connect_info);
		}

		old = tv.tv_sec;

		// No need to properly debounce, if it's changing it's changing
		int rstate = sunxi_gpio_input(right);
		int lstate = sunxi_gpio_input(left);

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

		lws_service( context, 250 );
	}

	lws_context_destroy( context );

	return 0;
}
