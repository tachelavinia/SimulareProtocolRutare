/******************************************************************************/
/* Tema 2 Protocoale de Comunicatie (Aprilie 2015)                            */
/******************************************************************************/
#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include <limits>
#include "api.h"
#include "sim.h"
#include "utils.h"
#include "topology.cpp"

int main(int argc, char **argv)
{
	/* Se prezinta etapele cronologice ale simularii unei retele de rutere prin
	implementarea unui program care va fi capabil sa ruleze un algoritm de rutare SPF
	(shortest path first) minimalist si sa se adapteze la schimbarile din retea in cadrul
	aceluiasi algoritm si sa ruteze o serie de mesaje date ca input. Se pot adauga sau
	sterge legaturi in retea */
	
	int noRouters;
	ifstream infile(argv[1]);
	infile >> noRouters;
	

	init_api(argc, argv);
	init_sim(argc, argv);

	loop {
		trigger_events();
		process_messages(noRouters);
		update_routing_table(noRouters);
		api_update_state();
		if (api_simulation_ended())
			break;
	}

	clean_sim();
	clean_api();
	
	return 0;
}
