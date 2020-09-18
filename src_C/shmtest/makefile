mytest: mytest.c
	xlc -q64 -o mytest mytest.c
mymalloc: mymalloc.c
	xlc -q32 -o mymalloc mymalloc.c
	#xlc -q32 -Wl,-bmaxdata:0x90000000/dsa -o mymalloc mymalloc.c
mymalloc64: mymalloc.c
	xlc -q64 -DDATA64 -o mymalloc64 mymalloc.c
clean:
	rm mytest mymalloc mymalloc64