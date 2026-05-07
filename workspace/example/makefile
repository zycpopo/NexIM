all:discovery registry
discovery : discovery.cc main.pb.cc
	g++ -std=c++17 $^ -o $@ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest -lpthread -lbrpc -lleveldb  -lssl -lcrypto -lprotobuf
registry : registry.cc main.pb.cc
	g++ -std=c++17 $^ -o $@ -lspdlog -lfmt -lgflags -letcd-cpp-api -lcpprest -lpthread -lbrpc -lleveldb  -lssl -lcrypto -lprotobuf