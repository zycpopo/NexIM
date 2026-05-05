all:discovery registry
discovery : discovery.cc
	g++ -std=c++17 $^ -o $@ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest -lpthread
registry : registry.cc
	g++ -std=c++17 $^ -o $@ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest -lpthread