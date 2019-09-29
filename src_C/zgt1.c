/*
 ============================================================================
 Name        : zgt1.c
 Author      : zg
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


int main(void) {
	omp_set_num_threads(5);
	#pragma omp parallel
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;

}
