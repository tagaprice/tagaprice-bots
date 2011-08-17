CXXFLAGS += -Iosmium/include -lprotobuf -lz -lexpat

all: osm_import

osm_import: main.o
	$(CXX) $(CXXFLAGS) -o $@ *.o -losmpbf

%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o osm_import
