#pragma once

// Constants
#define PI (3.141592653589793L)

// Math functions

// Linear interpolation
#define LERP(a, b, t)\
					(a + (b - a) * t)

// To radians
#define RAD(x)\
	(x * (PI / 180))

// Sum of 2 squares
#define SQUARE_SUM_2(a, b)\
					(a*a + b*b)

// Sum of 3 squares
#define SQUARE_SUM_3(a, b, c)\
					(a*a + b*b + c*c)

// Sum of 4 squares
#define SQUARE_SUM_4(a, b, c, d)\
					(a*a + b*b + c*c + d*d)

// Sum of 5 squares
#define SQUARE_SUM_5(a, b, c, d, e)	\
					(a*a + b*b + c*c + d*d + e*e)