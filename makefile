default:main.o
	g++ main.o -o twertlefetchcpp
main.o:
	g++ -c main.cpp -o main.o
