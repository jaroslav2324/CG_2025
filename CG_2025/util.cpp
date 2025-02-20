#include "util.h"

bool apprEqual(float f1, float f2, float epsilon /*= 1e-6*/) {
	return (std::abs(f1 - f2) <= epsilon);
}