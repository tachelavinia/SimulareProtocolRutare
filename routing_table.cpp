#include<iostream>
#include<vector>
#include<string>
#include<set>
#include "api.h"
#include "utils.h"

class Routing_table{

	/* Tabela de rutare contine costul drumului minim de la ruterul curent la ruterul k si un next-hop pe care ruterul sa il aleaga 
	pentru cazul drumului minim. Next-hop este unul dintre ruterele direct conectate la el.*/

public:
	int **tab_rutare;
	int routers;
	int *D,*S, *V;
	
	Routing_table(const int & nodes){
		routers = nodes;
		tab_rutare = new int*[routers];
		for(int i = 0; i < routers; i++){
			tab_rutare[i] = new int[2];
		}
		D = new int[routers];
		S = new int[routers];
		V = new int[routers];
	}

	~Routing_table(){
		delete tab_rutare;
	}

	void initialize_routing_table(){
		for(int i = 0; i < routers; i++){
			tab_rutare[i][0] = 10000; /* Valoare maxima pentru cazul in care 
	ruterul nu este in retea sau informatii despre el nu au ajuns la ruterul curent*/
			tab_rutare[i][1] = -1; /* Inseamna ca ruterul k nu este inca
	cunoscut de ruterul curent*/
		}
	}

	void show_table(){
		for(int i = 0; i < routers; i++){
			for(int j = 0; j < 2; j++){
				std::cout << tab_rutare[i][j] << " ";
			}
			std::cout << "\n";
		}
	}
/* Returneaza next-hop pentru a ajunge pe calea cea mai scurta la node.*/
	int get_next_hop(int node){
		return tab_rutare[node][1];
	}

	int **get_table(){
		return tab_rutare;
	}

/* Este un algoritm care calculeaza drumurile minime de la un nod al unui graf
la toate celelalte noduri din graf. La fiecare pas in algoritm lui Dijkstra selecteaza
unul dintre varfurile care nu au fost deja selectate. Prin urmare, se vor executa n -1 pasi.
Pentru selectarea unui varf trebuie sa determinam varful neselectat situat la distanta 
minima din multimea varfurile deja selectate. */
	void Dijkstra(int **topology, int node){
		int k = -1;
		for(int i = 0; i < routers; i++){
			S[i] = 0;
			D[i] = topology[node][i];
			if(D[i] < 10000){
				V[i] = i;
			}else{
				V[i] = -1;
			}
		}
		for(int i = 0; i < routers; i++){
			int minim = 10000;
			for(int j = 0; j < routers; j++){
				if(S[j] == 0){
					if(D[j] < minim){
						minim = D[j];
						k = j;
					}
				}
			}
			S[k] = 1;
			for(int j = 0; j < routers; j++){
				if( (S[j] == 0) && (D[k] + topology[k][j] < D[j])){
					D[j] = D[k] + topology[k][j];
					V[j] = V[k];
				}else{
					if( (S[j] == 0) && (D[k] + topology[k][j] == D[j]) && V[k] <= V[j]){
						D[j] = D[k] + topology[k][j];
						V[j] = V[k];
					}	
				}
			}
		}
		/* se actualizeaza tabela de rutare*/
		for(int i = 0; i < routers; i++){
			if(V[i] != -1 && i != node){
				tab_rutare[i][0] = i;
				tab_rutare[i][1] = V[V[i]];
			}else{
				tab_rutare[i][0] = node;
				tab_rutare[i][1] = 0;
			}		
		}
	}
};
