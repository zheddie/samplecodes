OBJECTS= p38.1 stringtest test_genlongstring
stringtest: stringtest.cpp string.cpp
	g++ -o stringtest stringtest.cpp string.cpp
p38.1:
	g++ -o p38.1 p38.1.cpp
test_genlongstring: test_genlongstring.cpp
	g++ -o test_genlongstring test_genlongstring.cpp
clean:
	rm ${OBJECTS}