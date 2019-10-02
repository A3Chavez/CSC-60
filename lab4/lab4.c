/*-------------------------------------------------*/
/* Anthony Chavez                                  */
/* Lab 4                                           */
/* Figure the perimeter and area of a polygon      */
/* surrounded by a circle                          */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FILE_IN "lab4.dat"
//#define FILE_IN "lab4sample.dat"

/*-----Start of Main------------------------------------------------*/
int main(void) {
	double nSides, radius, Perm, Area;
	FILE * input_file;
	FILE * answer_file;

	input_file = fopen(FILE_IN, "r");
	if(input_file == NULL) {
		printf("Error on opening the data file\n");
		exit(EXIT_FAILURE);
	}

	answer_file = fopen("lab4.out", "w");
	if(answer_file == NULL) {
		printf("Error on opening the output file\n");
		exit(EXIT_FAILURE);
	}

	fprintf(answer_file, "\nAnthony Chavez.  Lab 4.\n\n");
	fprintf(answer_file, "            Number      Perimeter      Area Of  \n");
	fprintf(answer_file, " Radius    Of Sides    Of Polygon      Polygon  \n");
	fprintf(answer_file, "--------   --------   ------------   ----------- \n");
	while((fscanf(input_file," %lf%lf", &radius, &nSides)) == 2) {
		Perm = 2 * nSides * radius * sin(M_PI / nSides);
		Area = (1.0 / 2) * nSides * pow(radius, 2) * sin((2 * M_PI) / nSides);
		fprintf(answer_file, "%7.2lf%11.2lf%15.4lf%14.4lf \n", radius, nSides, Perm, Area);
	}

	fclose(input_file);
	fclose(answer_file);

	return EXIT_SUCCESS;
}
/*-----End of Main--------------------------------------------------*/
