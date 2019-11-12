#include "lab5.h"

void compute(double radius, double *area, double *cir) {
	*area = M_PI * pow(radius,2);
	*cir = 2 * M_PI * radius;
	return;
}
