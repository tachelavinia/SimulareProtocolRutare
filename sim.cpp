/******************************************************************************/
/* Tema 2 Protocoale de Comunicatie (Aprilie 2015)  
			Simularea unui protocol de rutare
			    Realizata de : Lavinia Tache - 322CA
							2015
*/
/******************************************************************************/

#include<sstream>
#include<fstream>
#include <iostream> 
#include <string>
#include <cstring>
#include "api.h"
#include "utils.h"
#include "topology.cpp"
#include "routing_table.cpp"
#include "sim.h"

int version = 0;
int routers;
int unit_time;
int no_events;
int no_msg;
Network_topology **topologys;
Routing_table **routing_tab;
received_msg message_to_add;
events event;
std::vector<events> events_list;
std::vector<received_msg> mesg_list;

void add_message(received_msg & message_to_add, int src, int dst, int time, int tag, char *msg){
	message_to_add.src = src;
	message_to_add.dst = dst;
	message_to_add.time = time;
	message_to_add.tag = tag;
	message_to_add.msg = new char[255];
	strcpy(message_to_add.msg, msg);
}

void add_event(events &event, int r1, int r2, int type, int cost, int time){
	event.r1 = r1;
	event.r2 = r2;
	event.type = type;
	event.cost = cost;
	event.time = time;
}

/* init_sim presupune implementarea actiunilor care tin de initializarea simulatorului.
In cadrul acestei functii se va face si floodarea cu mesaje (initializarea protocolului
de rutare. Astfel, fiecare ruter la bootare contine doar vecinii lui direct conectati 
si un graf care contine propria lui topologie. In urma inundarii cu mesaje , graful
se actualizeaza si ruterul va avea informatii despre toata reteaua.
	De asemenea, fiecare ruter contine o tabela de rutare care este reprezentata de 
	nodul destinatie si next-hop ul pentru a ajunge acolo.*/
void init_sim(int argc, char **argv) {
	int noRouters;
	int router1, router2, cost;
	ifstream infile(argv[1]);
	infile >> noRouters;
	routers = noRouters;

	/* initializarea vectorului de pointeri la topologii si tabele de rutare */
	topologys = new Network_topology *[noRouters];
	for(int i =  0; i < noRouters; i++){
		topologys[i] = new Network_topology(noRouters);
	}

	routing_tab = new Routing_table *[noRouters];
	for(int i =  0; i < noRouters; i++){
		routing_tab[i] = new Routing_table(noRouters);
	}
	for(int i = 0; i < noRouters; i++){
		routing_tab[i]->initialize_routing_table();
	}


	for(int i =  0; i < noRouters; i++){
		topologys[i]->initialize_topology();
		topologys[i]->initialize_versions();
	}
	while(!infile.eof()){
		infile >> router1 >> cost >> router2;
		topologys[router1]->create_link(router1,router2,cost);
		topologys[router2]->create_link(router1,router2,cost);
	}
	/* Mesajul trimis de fiecare ruter contine versiunea mesajului, ruterul sursa ( cel 
	care a trimis mesajul si lista de vecini impreuna cu costul pe a ajunge la acel nod 
	codificat sub forma unui string. Versiunea unui mesaj este necesara pentru a evita floodarea
	la infinit cu mesaje. In cazul in care ruterul contine deja modificarile facute in urma unuei versiuni
	a unui mesaj il va ignora */
	for(int i = 0; i < noRouters; i++){
		int maxDim = 2 * noRouters;
		char *mesg = new char[maxDim * sizeof(int)];
		/* adaug versiunea pentru fiecare ruter */
		sprintf(mesg, "%d", version);
		strcat(mesg, "#");
		version++;
		char *dest = (char*) malloc(sizeof(int));
		sprintf(dest, "%d", i);
		strcat(mesg, dest);
		strcat(mesg, "#");

		int ** current_topology = topologys[i]->get_topology();
		std::vector< pair<int,int> > current_neighbors = topologys[i]->get_neighbor(current_topology, i);
		for(unsigned int j = 0; j < current_neighbors.size(); j++){
			char *neighbor = (char *)malloc(sizeof (int));
			char *cost = (char *)malloc(sizeof(int));
			sprintf(neighbor, "%d", current_neighbors[j].first);
			sprintf(cost, "%d", current_neighbors[j].second);

			
			strcat(mesg, neighbor);
			strcat(mesg, "#");
			strcat(mesg, cost);
			strcat(mesg, "#");
			strcat(mesg, "\0");
		}

		for(unsigned int j = 0; j < current_neighbors.size() ; j++){
			endpoint[i].send_msg (&endpoint[current_neighbors[j].first], mesg, strlen(mesg) + 1, NULL);
		}
	}
	/* Fisierul dat ca al treilea argument reprezinta lista de evenimente: adaugarea sau eliminarea unui
	legaturi in retea. Pentru procesarea evenimentelor consider un vector de astfel de evenimente , unde 
	fiecare eveniment este reprezentat de o structura. */
	infile.close();
	ifstream file(argv[3]);
	file >> unit_time;
	file >> no_events;
	int r1, r2, type, cost_event, event_time;
	for(int i = 0; i < no_events; i++){
		file >> r1 >> r2 >> type >> cost_event >> event_time;
		events_list.push_back(events());
		add_event(events_list[i], r1, r2, type, cost_event, event_time);
	}

	file.close();
	/* Fisierul dat ca al doilea argument reprezinta o lista de mesaje ce ar trebui rutate in retea. 
	Lista de mesaje este stocata in memorie intr-un vector de structuri ,unde structura contine destinatarul,
	sursa, timpul simulatorului la care se trimite mesajul si insusi mesajul.*/
	ifstream mfile(argv[2]);
	mfile >> no_msg;
	int src, dest, msg_time, tag;
	std::string new_msg;
	for(int i = 0; i < no_msg; i++){
		mfile >> src >> dest >> msg_time >> tag;
		getline(mfile, new_msg);
		char *char_msg = new char[255];
		memset(char_msg,0,255);
		copy(new_msg.begin(), new_msg.end(), char_msg);
		strcat(char_msg, "\n");
		strcat(char_msg, "\0");
		mesg_list.push_back(received_msg());
		add_message(mesg_list[i], src, dest, msg_time, tag, char_msg);
	}
	mfile.close();
}

void clean_sim() {
  //TODO
}

/* trigger_events reprezinta evenimentele ce urmeaza sa aiba loc in retea la timpul simulatorului.*/
void trigger_events() {
  for(int i = 0; i < no_events; i++){
  	if( events_list.at(i).time == get_time()){
  		if(events_list[i].type == 0){
  			/* In cazul in care type = 0 se adauga o noua legatura in retea, iar vecinii nodurilor tocmai
  			conectate vor primit un mesaj despre vecinii direct conectatii prin mesaje de floodare.*/
  			topologys[events_list.at(i).r1]->create_link(events_list.at(i).r1,events_list.at(i).r2, events_list.at(i).cost);
  			topologys[events_list.at(i).r2]->create_link(events_list.at(i).r1,events_list.at(i).r2, events_list.at(i).cost);

  			int maxDim = 2 * routers;
  			char *mesg = new char[maxDim * sizeof(int)];
	  		sprintf(mesg, "%d", version);
			strcat(mesg, "#");
			version++;
			char *dest = (char*) malloc(sizeof(int));
			sprintf(dest, "%d", i);
			strcat(mesg, dest);
			strcat(mesg, "#");

			int ** current_topology = topologys[i]->get_topology();
			std::vector< pair<int,int> > current_neighbors = topologys[i]->get_neighbor(current_topology, i);
			for(unsigned int j = 0; j < current_neighbors.size(); j++){
				char *neighbor = (char *)malloc(sizeof (int));
				char *cost = (char *)malloc(sizeof(int));
				sprintf(neighbor, "%d", current_neighbors[j].first);
				sprintf(cost, "%d", current_neighbors[j].second);

				
				strcat(mesg, neighbor);
				strcat(mesg, "#");
				strcat(mesg, cost);
				strcat(mesg, "#");
				strcat(mesg, "\0");
			}

			for(unsigned int j = 0; j < current_neighbors.size() ; j++){
				endpoint[i].send_msg (&endpoint[current_neighbors[j].first], mesg, strlen(mesg) + 1, NULL);
			}
  		}else{
  			/* Altfel, daca se elimina o legatura din retea se vor trimite din nou mesaje de floodare pentru ca
  			intreaga retea sa cunoasca noua modificare.*/
  			topologys[events_list.at(i).r1]->delete_link(events_list.at(i).r1,events_list.at(i).r2);
  			topologys[events_list.at(i).r2]->delete_link(events_list.at(i).r1,events_list.at(i).r2);

  			int maxDim = 2 * routers;
  			char *mesg = new char[maxDim * sizeof(int)];
	  		sprintf(mesg, "%d", version);
			strcat(mesg, "#");
			version++;
			char *dest = (char*) malloc(sizeof(int));
			sprintf(dest, "%d", i);
			strcat(mesg, dest);
			strcat(mesg, "#");

			int ** current_topology = topologys[i]->get_topology();
			std::vector< pair<int,int> > current_neighbors = topologys[i]->get_neighbor(current_topology, i);
			for(unsigned int j = 0; j < current_neighbors.size(); j++){
				char *neighbor = (char *)malloc(sizeof (int));
				char *cost = (char *)malloc(sizeof(int));
				sprintf(neighbor, "%d", current_neighbors[j].first);
				sprintf(cost, "%d", current_neighbors[j].second);

				
				strcat(mesg, neighbor);
				strcat(mesg, "#");
				strcat(mesg, cost);
				strcat(mesg, "#");
				strcat(mesg, "\0");
			}

			for(unsigned int j = 0; j < current_neighbors.size() ; j++){
				endpoint[i].send_msg (&endpoint[current_neighbors[j].first], mesg, strlen(mesg) + 1, NULL);
			}
  		}
  	}
  }
}
/* Mesajul de floodare este codat sub forma unui string, iar functia split proceseaza acest mesaj*/
vector<string> split(string str, char delimiter) {
 	vector<string> internal;
 	stringstream ss(str);
 	string tok;
 	while(getline(ss, tok, delimiter)) {
    	internal.push_back(tok);
  	}
	return internal;
}

/* In process_messages se verifica mesajele ce urmeaza a fi procesate. In primul rand se verifica mesajele de floodare si se 
completeaza topologia cu informatiile primite de la celelalte rutere prin inundare. 
De asemenea, dupa completarea topologiei se proceseaza mesaje ce reprezinta evenimentele, iar dupa
actualizarea topologiei se trimit din nou mesaje de floodare. 
Se vor citi si procesa si mesajele ce urmeaza sa fie rutate.*/
void process_messages(int noRouters) {
	for(int i = 0; i < noRouters; i++){
		int maxDim = 2 * noRouters;
		char *recv = new char[maxDim * sizeof(int)];
		int msg_size = endpoint[i].recv_protocol_message(recv);
		while( msg_size > 0){
			std::vector<string> tokens = split(recv, '#');
			int version = atoi(tokens[0].c_str());
			int source = atoi(tokens[1].c_str());
			if(i == source){
				/* Mesajul a ajuns la destinatie!*/				
			}else{
				if(version > topologys[i]->get_version(i,source)){
					for(int k = 0; k < routers; k++){
						topologys[i]->delete_link(k, source);
					}

					for(unsigned int k = 2; k < tokens.size(); k += 2){
						int next_node = atoi(tokens[k].c_str());
						int next_cost = atoi(tokens[k+1].c_str());
						topologys[i]->create_link(source,next_node, next_cost);
					}
					int ** current_topology = topologys[i]->get_topology();
					std::vector< pair<int,int> > current_neighbors = topologys[i]->get_neighbor(current_topology, i);
					for(unsigned int j = 0; j < current_neighbors.size() ; j++){
						endpoint[i].send_msg (&endpoint[current_neighbors[j].first], recv, strlen(recv) + 1, NULL);
					}
					topologys[i]->update_versionsforRouter(i, source, version);
				}else{
				}
			}
			msg_size = endpoint[i].recv_protocol_message(recv);
		}
	}
	
	for(int i = 0; i < no_msg; i++){
		if(mesg_list[i].time == get_time()){
			int next_hop = routing_tab[mesg_list[i].src]->get_next_hop(mesg_list[i].dst);
			endpoint[mesg_list[i].src].route_message(&endpoint[next_hop], mesg_list[i].dst, mesg_list[i].tag, mesg_list[i].msg, NULL );

		}
	}

	for(int i = 0; i < routers; i++){
		int src, dest, tag;
		char *mesg = new char[255];

		int valid = endpoint[i].recv_message(&src, &dest, &tag, mesg);
		while(valid > 0){
			if( dest != i){
				int next_hop = routing_tab[i]->get_next_hop(dest);
				endpoint[i].route_message(&endpoint[next_hop],dest, tag, mesg, NULL);
			}
			valid = endpoint[i].recv_message(&src, &dest, &tag, mesg);
		}
	}
}
/* Fiecare ruter contine o tabela de rutare , unde, pentru fiecare ruter, stochez 
next-hop ul pentru  ajunge pe calea cu costul cel mai mic la ruterele din topologie.*/
void update_routing_table(int noRouters) {
	for(int i = 0; i < noRouters; i++){
		int ** current_topology = topologys[i]->get_topology();
		routing_tab[i]->Dijkstra(current_topology, i);
	}
}



