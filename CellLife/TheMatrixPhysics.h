#pragma once
#include <memory>
#include "CellWorldPhysics.h"

class TheMatrixPhysics :
	public CellWorldPhysics
{
protected:
	long long t;
	int daytime;
	int ticksInDay;
	int dayOfTheYear;
	int daysInYear;

	float dayToNightDelta;
	float winterToSummerDelta;

public:
	using CellWorldPhysics::CellWorldPhysics;

	TheMatrixPhysics(int TicksInDay = 96, int DaysInYear = 365, float DayToNightDelta = 10.0f, float WinterToSummerDelta = 20.0f) :
		t(0), daytime(0), ticksInDay(TicksInDay), dayOfTheYear(0), daysInYear(DaysInYear), dayToNightDelta(DayToNightDelta), winterToSummerDelta(WinterToSummerDelta) {}
	~TheMatrixPhysics() {}

	long long getGenCount() { return t; }
	int getDaytime() { return daytime; }
	int getTicksInDay() { return ticksInDay; }
	int getDayOfTheYear() { return dayOfTheYear; }
	int getDaysInYear() { return daysInYear; }

	float getDayToNightDelta() { return dayToNightDelta; }
	float getWinterToSummerDelta() { return winterToSummerDelta; }

	float getSunPower(int depth);
	float getHeatingCost(int depth); //Deeper -> colder -> harder to stay alive

	virtual void tick(CellWorld * world);

	virtual void reset() { t = 0; }
};

typedef shared_ptr<class TheMatrixPhysics> sharedTheMatrixPhysics;
typedef weak_ptr<class TheMatrixPhysics> weakTheMatrixPhysics;