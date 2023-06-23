#pragma once


namespace ASI
{

	union uData
	{
		double f;
		unsigned long long int t;

	};

	struct BuildingHookData
	{
		unsigned long long int buildingID;
		char * name;
		unsigned short int linkedUnitID; //optional
	};

	unsigned long long int convertIntForLua(unsigned int input)
	{
		union uData data;
		data.f = 1.0*input;
		return data.t;
	}


}