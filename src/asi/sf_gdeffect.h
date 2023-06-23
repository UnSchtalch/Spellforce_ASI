#pragma once

#include "SF_ASI.h"
#include "SF_Unknown.h"
#include "SF_SpellManager.h"

namespace ASI
{
	inline unsigned int GdEffect_Add(ASI::SF_SpellManager* spell_manager, int spell_index, int effect_type, unsigned char target_type, unsigned short target_index, unsigned short target_x, unsigned short target_y, int time, ASI::SF_Unk1 some_val1, ASI::SF_Unk1 params0, int val17_offset = 0)
	{
        ASI::Pointer some_pointer2(ASI::GetUnknownPointer(spell_manager, 0x4));
        auto val17 = ((unsigned int)some_pointer2[0xC])+val17_offset;
        ASI::SF_Unk1 params1(target_type, target_index, target_x, target_y);
        //GdEffect::Add
        unsigned int result = ASI::CallClassFunc<0x8192D0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
            (ASI::GetUnknownPointer(spell_manager, 0x14), effect_type, &some_val1, &params1, val17, time, &params0);
        return result;
	}

    inline void GdEffect_VisualOnlyAdd(ASI::SF_SpellManager* spell_manager, int spell_index, int effect_type, unsigned char target_type, unsigned short target_index, unsigned short target_x, unsigned short target_y, int time, ASI::SF_Unk1 params0)
    {
        ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
        auto some_value1002 = (unsigned int)some_pointer1000[0xC];
        ASI::SF_Unk1 some_val1 = ASI::GetUnk1(spell_manager, spell_index);
        ASI::SF_Unk1 params1(target_type, target_index, target_x, target_y);
        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
            (spell_manager->data, spell_index, effect_type, &some_val1, &params1, some_value1002, time, &params0);
    }

    inline void GdEffect_VisualOnlyAddEx(ASI::SF_SpellManager* spell_manager, int spell_index, int effect_type, unsigned char target_type, unsigned short target_index, unsigned short target_x, unsigned short target_y, int time, ASI::SF_Unk1 params0, ASI::SF_Unk1 some_val1)
    {
        ASI::Pointer some_pointer1000 = ASI::GetUnknownPointer(spell_manager, 0x4);
        auto some_value1002 = (unsigned int)some_pointer1000[0xC];
        ASI::SF_Unk1 params1(target_type, target_index, target_x, target_y);
        ASI::CallClassProc<0x7B88C0, unsigned int, unsigned int, ASI::SF_Unk1*, ASI::SF_Unk1*, unsigned int, unsigned int, ASI::SF_Unk1*>
            (spell_manager->data, spell_index, effect_type, &some_val1, &params1, some_value1002, time, &params0);
    }

    inline void GdEffect_Remove(ASI::SF_SpellManager* spell_manager, int spell_index)
    {
        ASI::CallClassProc<0x7B9820, unsigned int, unsigned int>(spell_manager->data, spell_index, 0);
    }
}