#pragma once

namespace ASI
{
    struct UiUnitDescriptionInfo
    {
        unsigned int UnitID;
        unsigned int DescriptionID;    // category 42 in gamedata editor

		UiUnitDescriptionInfo()
		{

		}

		UiUnitDescriptionInfo(unsigned int id, unsigned int desc)
		{
			UnitID = id;
			DescriptionID = desc;
		}

		UiUnitDescriptionInfo(const UiUnitDescriptionInfo& uuii)
		{
			UnitID = uuii.UnitID;
			DescriptionID = uuii.DescriptionID;
		}

		UiUnitDescriptionInfo& operator=(const UiUnitDescriptionInfo& uuii)
		{
			UnitID = uuii.UnitID;
			DescriptionID = uuii.DescriptionID;

			return *this;
		}
    };
}