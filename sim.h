/******************************************************************************/
/* Tema 2 Protocoale de Comunicatie (Aprilie 2015)                            */
/******************************************************************************/

#ifndef _SIM_H_
#define _SIM_H_

void init_sim(int argc, char **argv);
void clean_sim();

void trigger_events();
void process_messages(int noRouters);
void update_routing_table(int noRouters);

struct received_msg{
	int src;
	int dst;
	int time;
	int tag;
	char *msg;
};

struct events{
	int r1;
	int r2;
	int type;
	int cost;
	int time;
};

extern received_msg message_to_add;
extern events event;
void add_message(received_msg &, int src, int dst, int time, int tag, char *message_to_add);
void add_event(events & , int r1, int r2, int type, int cost, int time);

#endif /* _SIM_H_ */
