CXX=g++
CXXFLAGS=`pkg-config libgvc --cflags` -g -Wall $(EXTRA)
LDFLAGS=`pkg-config libgvc --libs`

build: simulation

simulation: main.cpp sim.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ -lapi -L . $(LDFLAGS)

clean:
	rm -f -R simulation *.o
