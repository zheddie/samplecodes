#OBJECTS= p38.1 test_genlongstring virtualclass maintest testvariable
# stringtest: stringtest.cpp string.cpp
# 	g++ -o stringtest stringtest.cpp string.cpp
# p38.1:
# 	g++ -o p38.1 p38.1.cpp
# test_genlongstring: test_genlongstring.cpp
# 	g++ -o test_genlongstring test_genlongstring.cpp
# virtualclass: virtualclass.cpp
# 	g++ -o virtualclass virtualclass.cpp
main: main.cpp string.cpp
#	g++ -o $@ $^
# testvariable: testvariable.cpp
# 	g++ -o testvariable testvariable.cpp
clean:
	#rm ${OBJECTS}
	ls -l *.cpp|awk '{print $$9}'|awk -F'.' '{print $$1}'|xargs rm