#include "stdafx.h"
#include "TheMatrixPhysics.h"

const float PI_F = 3.14159265358979f;

void TheMatrixPhysics::tick(CellWorld * world)
{
	++t;
	daytime = t % ticksInDay;
	dayOfTheYear = (t % (ticksInDay * daysInYear)) / ticksInDay;
}

//depth
float TheMatrixPhysics::getSunPower(int depth)
{	
	float daytimeSin = sinf(PI_F * 2.0f * (daytime + ticksInDay / 4) / ticksInDay);
	float dayOfheYearSin = sinf(PI_F * 2.0f * ((dayOfTheYear % (ticksInDay * daysInYear)) / ((float)ticksInDay * daysInYear) - 0.25f));

	return ((daytimeSin + 1.0f) * dayToNightDelta / 2.0f) + ((dayOfheYearSin + 1.0f) * winterToSummerDelta / 2.0f);
}

float TheMatrixPhysics::getHeatingCost(int depth)
{
	
}