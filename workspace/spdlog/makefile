all:
	g++ -std=c++17 async.cc -o async -lspdlog -lfmt
	g++ -std=c++17 sync.cc -o sync -lspdlog -lfmt
	g++ -std=c++17 main.cc -o main -lspdlog -lfmt -lgflags
clean:
	rm -f async sync main