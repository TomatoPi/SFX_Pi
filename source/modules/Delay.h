#ifndef DEF_DELAY_H
#define DEF_DELAY_H

#include "../core/Modules.h"
#include "../core/Utility.h"
#include "../core/Ringbuffer.h"

#define D_DMAX 5000
#define D_DELAY 250
#define D_FEED 0.5

int delay_Process_Callback(jack_nframes_t nframes, void *u);

class Delay : public Module{

	public:
		Delay(const char *server);
		Delay(const char *server, MODULE_TYPE type, int params_count);
		~Delay();
		
		int process(jack_nframes_t nframes, void *arg);
		int process_2(jack_nframes_t nframes, void *arg);
		int bypass(jack_nframes_t nframes, void *arg);
	
	protected:
	
		jack_client_t *client_2;
		char *name_2;
	
		jack_port_t **private_port;
		int private_port_count;
		
		Ringbuffer *buffer_L, *buffer_R;
		rng_reader reader_L, reader_R;
	
		/*
		* Params are :
		*
		* max_delay
		*
		* delay_L
		* delay_R
		* feedback_L
		* feedback_R
		*
		* dry _ wet
		*/
};

#endif
