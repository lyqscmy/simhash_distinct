CXX=g++
# CXX=clang
PROG=simhash_distinct
CXXFLAGS=-std=c++11
LIBS+= -lrt

all: release

release: main.cpp
	$(CXX) -O2 -DNDEBUG -D_CONSOLE $(CXXFLAGS) -o $(PROG) $^ -ltbb $(LIBS)

debug: src/Evolution.cpp src/Update_state.cpp src/Game_of_life.cpp
	$(CXX) -O0 -D_CONSOLE -g -DTBB_USE_DEBUG $(CXXFLAGS) -o $(PROG) $^ -ltbb_debug $(LIBS)

clean:
	rm -f $(PROG) *.o *.d