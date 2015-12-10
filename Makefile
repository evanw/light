tests:
	c++ src/*.cpp test/*.cpp -std=c++11 -I src -I test
	./a.out
	rm a.out
