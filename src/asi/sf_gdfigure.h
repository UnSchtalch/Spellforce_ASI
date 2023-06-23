#pragma once

#include "SF_ASI.h"

namespace ASI
{
	inline unsigned int IsAlive(ASI::Pointer figuredata, unsigned short figure_index)
	{
        return (
			(((short)figuredata[figure_index*691 + 0x1C]) != -1) && 
			(((unsigned int)figuredata[figure_index * 691 + 0x16] & 0xA) == 0));
	}

	inline unsigned short GetCurrentHealthPercentLeft(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x7BCFC0, unsigned short>
			(figuredata[figure_index * 691 + 0x42]);
	}

	inline short GetCurrentHealth(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x5C79D0, short, unsigned short>
			(figuredata, figure_index);
	}

	inline short GetCurrentHealthMax(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x669090, short, unsigned short>
			(figuredata, figure_index);
	}

	inline short GetCurrentHealthMissing(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return GetCurrentHealthMax(figuredata, figure_index) - GetCurrentHealth(figuredata, figure_index);
	}

	inline unsigned short GetLifeAction(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned short)(figuredata[figure_index * 691 + 0x48]);
	}

	inline unsigned int GetPosition(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned int)(figuredata[figure_index * 691 + 0x2]);
	}

	inline unsigned short GetX(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned int)(figuredata[figure_index * 691 + 0x2]);
	}

	inline unsigned short GetY(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned int)(figuredata[figure_index * 691 + 0x4]);
	}

	inline unsigned int& GetFlags(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return *((unsigned int*)((unsigned int)figuredata.ptr + figure_index * 691 + 0x16));
	}

	inline unsigned char GetRace(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned char)(figuredata[figure_index * 691 + 0x1A]);
	}

	inline unsigned char GetLevel(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned char)(figuredata[figure_index * 691 + 0x1B]);
	}

	inline unsigned short GetFigureID(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned short)(figuredata[figure_index * 691 + 0x28]);
	}

	inline unsigned short GetBuilding(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (unsigned short)(figuredata[figure_index * 691 + 0x14]);
	}

	inline short GetInUse(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return (short)(figuredata[figure_index * 691 + 0x1C]);
	}

	inline unsigned short GetCurrentMana(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x6690F0, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	inline unsigned short GetCurrentCharisma(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x667C70, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	inline unsigned short GetCurrentManaMax(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x669160, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	inline unsigned short GetCurrentIntelligence(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x668F90, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	inline void ReduceCurrentMana(ASI::Pointer figuredata, unsigned short figure_index, short mana)
	{
		ASI::CallClassProc<0x66B260, unsigned short, short>
			(figuredata, figure_index, mana);
	}

	inline unsigned short GetSpellJobStartNode(ASI::Pointer figuredata, unsigned short figure_index)
	{
		return ASI::CallClassFunc<0x6695D0, unsigned short, unsigned short>
			(figuredata, figure_index);
	}

	inline void DealDamageUnconditional(ASI::Pointer figuredata, unsigned short figure_index, short damage)
	{
		ASI::CallClassProc<0x66B230, unsigned short, short>(figuredata, figure_index, damage);
	}

	inline unsigned short GetSpellJobNextNode(ASI::Pointer spelljobdata, unsigned short spelljobnode)   // [ebp+0x10]
	{
		return (unsigned short)ASI::Pointer((unsigned int)(spelljobdata.ptr) + spelljobnode * 6);
	}

	inline unsigned short GetSpellJobNodeSpellIndex(ASI::Pointer spelljobdata, unsigned short spelljobnode)
	{
		return (unsigned short)ASI::Pointer(spelljobdata[spelljobnode * 6 + 4]);
	}

	// returns effect index in table if it exists, -1 otherwise
	int HasEffect(ASI::SF_SpellManager* spell_manager, unsigned short figure_index, unsigned short spellline_id)
	{
		auto spelljobnode = GetSpellJobStartNode(GetFigureManager(spell_manager), figure_index);
		while (spelljobnode != 0)
		{
			ASI::Pointer some_pointer3(GetUnknownPointer(spell_manager, 0x10));
			ASI::Pointer some_pointer31(*some_pointer3[0]);
			auto spell_index = GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode);
			if(SpellType(spell_manager, spell_index) == spellline_id)
				return spell_index;
			spelljobnode = GetSpellJobNextNode(some_pointer31, spelljobnode);
		}
		return -1;
	}

	int HasEffectExcept(ASI::SF_SpellManager* spell_manager, unsigned short figure_index, unsigned short spellline_id, unsigned short except_effect_id)
	{
		auto spelljobnode = GetSpellJobStartNode(GetFigureManager(spell_manager), figure_index);
		while (spelljobnode != 0)
		{
			ASI::Pointer some_pointer3(GetUnknownPointer(spell_manager, 0x10));
			ASI::Pointer some_pointer31(*some_pointer3[0]);
			auto spell_index = GetSpellJobNodeSpellIndex(some_pointer31, spelljobnode);
			if ((spell_index != except_effect_id) && (SpellType(spell_manager, spell_index) == spellline_id))
				return spell_index;
			spelljobnode = GetSpellJobNextNode(some_pointer31, spelljobnode);
		}
		return -1;
	}

	inline int CheckCompatibleTargets(ASI::SF_SpellManager* spell_manager, ASI::Pointer figuredata, unsigned short figure_index1, unsigned short figure_index2, int target_type)
	{
		if (target_type == 1)
		{
			if (ASI::CallClassFunc<0x7FA9DD, int, unsigned short, unsigned short>
				(ASI::GetUnknownPointer(spell_manager, 0x24), figure_index1, figure_index2) != 0)
				return 0;

			if (ASI::GetInUse(figuredata, figure_index1) != ASI::GetInUse(figuredata, figure_index2))
				return 0;
		}
		else if (target_type == 2)
		{
			if (ASI::CallClassFunc<0x7FA9DD, int, unsigned short, unsigned short>
				(ASI::GetUnknownPointer(spell_manager, 0x24), figure_index1, figure_index2) == 0)
				return 0;

			if (ASI::CallClassFunc<0x7FA8F9, int, unsigned short>
				(ASI::GetUnknownPointer(spell_manager, 0x24), figure_index2) == 0)
				return 0;
		}
		else if (target_type == 3)
			return 1;
		else
			return 0;

		return 1;
	}

	inline void RemoveFlags(ASI::Pointer figuredata, unsigned short figure_index, unsigned int flags)
	{
		ASI::CallClassProc<0x664050, unsigned short>
			(figuredata, figure_index, flags);
	}

	inline unsigned short CreateFigureAsSummon(ASI::Pointer figure_toolbox, unsigned short source_figure_index, unsigned short unit_id)
	{
		return ASI::CallClassFunc<0x7F0942, unsigned short, int, int>
			(figure_toolbox, source_figure_index, unit_id);
	}

	inline void AddSpellEffect(ASI::Pointer figure_toolbox, unsigned short figure_index, unsigned short effect_index)
	{
		ASI::CallClassProc<0x7F0903, int, int>
			(figure_toolbox, figure_index, effect_index);
	}
}