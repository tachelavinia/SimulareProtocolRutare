#include <limits>
using namespace std;

class Network_topology{
	public:
		int routers;
		int **topology;
		int **versions;

		/* Fiecare ruter are propria lui topologie, tinuta sub forma unui graf*/
		Network_topology(const int & nodes){
			routers = nodes;
			versions = new int *[routers];
			for(int i = 0; i < routers; i++){
				versions[i] = new int[routers];
			}
			topology = new int *[routers];
			for(int i = 0; i < routers; i++){
				topology[i] = new int[routers];
			}
		}
		int size(){
			return routers;
		}
		~Network_topology(){
			for(int i = 0; i < routers; i++){
				delete[] topology[i];
			}
			delete [] topology;
			for(int i = 0; i < routers; i++){
				delete[] versions[i];
			}
			delete [] versions;
		}

		void initialize_topology(){
			for(int i = 0; i < routers; i++){
				for(int j = 0; j < routers; j++){
					topology[i][j] = 10000;
				}
			}
		}

		void initialize_versions(){
			for(int i = 0; i < routers; i++){
				for(int j = 0; j < routers; j++){
					versions[i][j] = -1;
				}
			}
		}

		void update_versionsforRouter(int i,int j, int new_version){
			versions[i][j] = new_version;
		}

		void print_versions(){
			for(int i = 0; i < routers; i++){
				for(int j = 0; j < routers; j++){
					std::cout << versions[i][j] << " ";
				}
				std::cout << endl;
			}
		}

		int get_version(int i, int j){
			return versions[i][j];
		}

		void create_link(int router1, int router2, int cost){
			topology[router1][router2] = cost;
			topology[router2][router1] = cost;
		}

		void delete_link(int router1, int router2){
			topology[router1][router2] = 10000;
			topology[router2][router1] = 10000;
		}

		void show_topology(){
			for(int i = 0; i < routers; i++){
				for(int j = 0; j < routers; j++){
					std::cout << topology[i][j] << " ";
				}
				std::cout << endl;
			}
		}
		/* returneaza vecinii ruterului node */
		std::vector< pair<int,int> > get_neighbor(int ** topology, int node){
			std::vector< pair<int,int> > v;
			for(int i = 0; i < routers; i++){
				if( (topology[i][node] != 10000 || topology[node][i] != 10000) && i != node){
					v.push_back(make_pair(i, topology[i][node]));
				}
			}
			return v;
		}

		int **get_topology(){
			return topology;
		}
};
