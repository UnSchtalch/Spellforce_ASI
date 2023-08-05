#pragma once

#include <string>
#include "SF_ASI.h"

namespace ASI
{
	struct UiUnitIconInfo
	{
		unsigned short UnitID;
		char UnitIconName[64];

		UiUnitIconInfo()
		{

		}

		UiUnitIconInfo(int id, const std::string& name)
		{
			UnitID = (unsigned short)id;

			for (int i = 0; i < 64; i++)
				UnitIconName[i] = 0;

			int l = name.length();
			if (l > 63)
				l = 63;

			for (int i = 0; i < l; i++)
				UnitIconName[i] = name[i];
		}

		UiUnitIconInfo(const UiUnitIconInfo& uuii)
		{
			UnitID = uuii.UnitID;
			for (int i = 0; i < 64; i++)
				UnitIconName[i] = uuii.UnitIconName[i];
		}

		UiUnitIconInfo& operator=(const UiUnitIconInfo& uuii)
		{
			UnitID = uuii.UnitID;
			for (int i = 0; i < 64; i++)
				UnitIconName[i] = uuii.UnitIconName[i];

			return *this;
		}
	};
}