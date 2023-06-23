#pragma once

#include "SF_ASI.h"
#include "SF_Unknown.h"

namespace ASI
{
	struct SF_SpellManager
	{
		unsigned char data[21698];
	};

	inline unsigned int OffsetToSpell(int spell_index)
	{
		return 27 * spell_index + 0x56;
	}

	inline ASI::Pointer GetBuildingManager(SF_SpellManager* man)
	{
		return (ASI::Pointer(((unsigned int*)(man->data))[2]));
	}

	inline ASI::Pointer GetFigureManager(SF_SpellManager* man)
	{
		return (ASI::Pointer(((unsigned int*)(man->data))[7]));
	}

	inline ASI::Pointer GetObjectManager(SF_SpellManager* man)
	{
		return (ASI::Pointer(((unsigned int*)(man->data))[12]));
	}

	inline ASI::Pointer GetXDataPointer(SF_SpellManager* man)
	{
		return (ASI::Pointer(((unsigned int*)(man->data))[20]));
	}

	inline ASI::Pointer GetUnknownPointer(SF_SpellManager* man, unsigned int offset)
	{
		return (ASI::Pointer(((unsigned int*)(man->data))[offset / 4]));
	}

	inline unsigned short& SpellCount(SF_SpellManager* man)
	{
		return *((unsigned short*)(man->data + 84));
	}

	inline unsigned short& SpellSource(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned short*)(man->data + OffsetToSpell(spell_index) + 9));
	}

	inline unsigned short& SpellTarget(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned short*)(man->data + OffsetToSpell(spell_index) + 16));
	}

	inline SF_Unk1 GetUnk1(SF_SpellManager* man, int spell_index)
	{
		return SF_Unk1(
			(*((unsigned char*)(man->data + OffsetToSpell(spell_index) + 8))),
			(*((unsigned short*)(man->data + OffsetToSpell(spell_index) + 9))),
			(*((unsigned short*)(man->data + OffsetToSpell(spell_index) + 11))),
			(*((unsigned short*)(man->data + OffsetToSpell(spell_index) + 13)))
		);
	}

	inline SF_Unk1 GetUnk2(SF_SpellManager* man, int spell_index)
	{
		return SF_Unk1(
			(*((unsigned char*)(man->data + OffsetToSpell(spell_index) + 15))),
			(*((unsigned short*)(man->data + OffsetToSpell(spell_index) + 16))),
			(*((unsigned short*)(man->data + OffsetToSpell(spell_index) + 18))),
			(*((unsigned short*)(man->data + OffsetToSpell(spell_index) + 20)))
		);
	}

	inline short& SpellToDoCount(SF_SpellManager* man, int spell_index)
	{
		return *((short*)(man->data + OffsetToSpell(spell_index) + 0));
	}

	inline short& SpellID(SF_SpellManager* man, int spell_index)
	{
		return *((short*)(man->data + OffsetToSpell(spell_index) + 2));
	}

	inline short& SpellType(SF_SpellManager* man, int spell_index)
	{
		return *((short*)(man->data + OffsetToSpell(spell_index) + 4));
	}

	inline unsigned short& SpellLine(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned short*)(man->data + OffsetToSpell(spell_index) + 6));
	}

	inline unsigned char& SpellParam3(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned char*)(man->data + OffsetToSpell(spell_index) + 8));
	}

	inline int* SpellSourcePointer(SF_SpellManager* man, int spell_index)
	{
		return (int*)((unsigned int)(man->data) + OffsetToSpell(spell_index) + 8);
	}

	// spell target type
	inline unsigned char& SpellParam4(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned char*)(man->data + OffsetToSpell(spell_index) + 15));
	}

	inline int* SpellTargetPointer(SF_SpellManager* man, int spell_index)
	{
		return (int*)((unsigned int)(man->data) + OffsetToSpell(spell_index) + 15);
	}

	inline unsigned short& SpellXData(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned short*)(man->data + OffsetToSpell(spell_index) + 22));
	}

	inline unsigned char& SpellUnk_25(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned char*)(man->data + OffsetToSpell(spell_index) + 25));
	}

	inline unsigned char& SpellFlags(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned char*)(man->data + OffsetToSpell(spell_index) + 26));
	}

	inline short& SpellParamX(SF_SpellManager* man, int spell_index)
	{
		return *((short*)(man->data + OffsetToSpell(spell_index) + 18));
	}

	inline short& SpellParamY(SF_SpellManager* man, int spell_index)
	{
		return *((short*)(man->data + OffsetToSpell(spell_index) + 20));
	}

	inline unsigned int& SpellParamPos(SF_SpellManager* man, int spell_index)
	{
		return *((unsigned int*)(man->data + OffsetToSpell(spell_index) + 18));
	}
}