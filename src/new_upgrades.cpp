/*
Original version written by leszekd25 aka Shovel Knight
Ported to 1.61-beta by UnSchtalch
*/


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <tuple>
#include "asi/sf_asi.h"
#include "asi/sf_util_string.h"
#include "ui_unit_icons.h"
#include "ui_unit_descriptions.h"
#include "upgrade_tree.h"
#include "unit_function_pointers.h"

int UPGRADE_EXEC_ABSOLUTE;

int UPGRADE_UI_CHECK_EXEC_ABSOLUTE;
int UPGRADE_UI_CHECK_FAIL_ABSOLUTE;

int UPGRADE_SPECIAL_EXEC_ABSOLUTE; //Pass our code and then exit
int UPGRADE_SPECIAL_FAIL_ABSOLUTE; //not our client, run as normal

int UPGRADE_CHECK_FAIL_ABSOLUTE;
int UPGRADE_CHECK_EXEC_ABSOLUTE;

int UPGRADE_UI_CHECK_SUPPLY_EXEC;
int UPGRADE_UI_CHECK_SUPPLY_FAIL;

int SPELL_TYPE_FAIL_ABSOLUTE;
int SPELL_TYPE_EXEC_ABSOLUTE;

int POST_INIT_EXEC_ABSOLUTE;

HWND game_window;

unit_functions_t unit_functions;
upgrade_functions_t upgrade_functions;
support_functions_t support_functions;
// unit upgrade data (unit id, (upgrade id, upgraded unit id))
//std::unordered_map<unsigned short, std::pair<int, unsigned short>> unit_upgrade_data; 
char max_upgrade_index = 54;          // base in game
upgrade_graph upg_G;

// unit icon data
ASI::UiUnitIconInfo* unit_icons = 0;
unsigned short max_ui_icon_index = 266;

// unit description data
ASI::UiUnitDescriptionInfo* unit_descriptions = 0;
unsigned short max_ui_description_index = 48;


int MAX_BUILDING_INDEX = 64000;


unsigned short IsDarkRace(unsigned char race_id)
{
    if ((race_id != 0x1) && (race_id != 0x2) && (race_id != 0x3))
        {
            return 1;
        }
    return 0;
}

// returns if upgrade started learning OR if it was learned
inline int upgrade_started_learning(unsigned int caller, void* unk_ptr, int upgrade_id)
{
    return ASI::CallClassFunc<0x241EE0, unsigned int, void*, int>
        (caller, unk_ptr, upgrade_id);
}

// returns if upgrade was learned
inline int is_upgrade_activated(unsigned int caller, void* unk_ptr, int upgrade_id)
{
    return ASI::CallClassFunc<0x241F70, unsigned int, void*, int>
        (caller, unk_ptr, upgrade_id);
}


// returns if upgrade started learning OR if it was learned
inline int upgrade_started_learning_beta(unsigned int caller, void* unk_ptr, int upgrade_id)
{
    return ASI::CallClassFunc<0x2A5120, unsigned int, void*, int>
        (caller, unk_ptr, upgrade_id);
}

// returns if upgrade was learned
inline int is_upgrade_activated_beta(unsigned int caller, void* unk_ptr, int upgrade_id)
{
    return ASI::CallClassFunc<0x2A51A0, unsigned int, void*, int>
        (caller, unk_ptr, upgrade_id);
}

// returns if predecessor upgrade is learned (if any) and if exclusive upgrade is not learned (if any)
int __attribute__((no_caller_saved_registers, thiscall)) upgrade_can_be_learned(unsigned int caller, void* unk_ptr, int upgrade_id)
{
    int upg2;
    if (ASI::CheckSFVersion(ASI::SF_154))
    {
        // check if previous upgrade is learned
        if (upg_G.prev_of(upgrade_id, upg2))
            if (!is_upgrade_activated(caller, unk_ptr, upg2))
                return 0;

        // check if exclusive upgrade is not learned
        if (upg_G.get_exclusive(upgrade_id, upg2))
            if (is_upgrade_activated(caller, unk_ptr, upg2))
                return 0;
    }

    if (ASI::CheckSFVersion(ASI::SF_BETA))
    {
        // check if previous upgrade is learned
        if (upg_G.prev_of(upgrade_id, upg2))
            if (!upgrade_functions.upgrade_activated(caller, unk_ptr, upg2))
                return 0;

        // check if exclusive upgrade is not learned
        if (upg_G.get_exclusive(upgrade_id, upg2))
            if (upgrade_functions.upgrade_activated(caller, unk_ptr, upg2))
                return 0;
    } 



    return 1;
}

void __attribute__((thiscall)) get_upgraded_unit_variant_id(unsigned int ** __this,unsigned int unknown,unsigned short * unit_data)
{
    unsigned int stack_ptr;

/*    __asm mov stack_pointer.ptr, esp
    __asm mov __this.ptr, ecx
*/

    unsigned short unit_id = *unit_data;
    unsigned short upgraded_unit_id = unit_id;
    unsigned short iter_unit_id;
    unsigned short iter_upgraded_unit_id;

    for (int i = 0; i <= max_upgrade_index; i++)
    {
        for (int j = 0; upg_G.get_unit(i, j, iter_unit_id, iter_upgraded_unit_id); j++)
        {
            if (iter_unit_id == unit_id)
            {
                if (ASI::CheckSFVersion(ASI::SF_BETA))
                {
                    if (upgrade_functions.upgrade_activated (*(void **)((int)__this+0x48), (unsigned short) unknown, i))
                    //if (is_upgrade_activated_beta(*(__this[0x48]), (unsigned short) unknown, i))
                    {
                        upgraded_unit_id = iter_upgraded_unit_id;
                    }
                }
                else
                {
                    if (is_upgrade_activated(*(void **)((int)__this+0x48), (unsigned short) unknown, i))
                    {
                        upgraded_unit_id = iter_upgraded_unit_id;
                    }
                }
            }
        }
    }

    *unit_data = upgraded_unit_id;

    return;
}

void __declspec(naked) check_can_upgrade_hook()
{
    /*__asm
    {
        push esi
        push ebp
        push ecx
        call upgrade_can_be_learned
        test eax, eax
        jz fail
        mov edx, [esp + 0xDC]
        jmp UPGRADE_CHECK_EXEC_ABSOLUTE
        fail :
        jmp UPGRADE_CHECK_FAIL_ABSOLUTE
    }*/
    asm("push %%esi\n\t"
        "push %%ebp\n\t"
        "call *%0\n\t"
        "test %%eax, %%eax\n\t"
        "jz 1f\n\t"
        "mov 0xDC(%%esp), %%edx\n\t"
        "jmp *%1\n\t"
        "1: jmp *%2\n\t":
        :"o"(upgrade_can_be_learned), "o"(UPGRADE_CHECK_EXEC_ABSOLUTE), "o"(UPGRADE_CHECK_FAIL_ABSOLUTE));
}


 /*   __asm
    {
        mov edx, [esp+0x24]
        mov ecx, [esp+0x20]
        mov ax, [ebx+edx]
        push eax
        push ecx
        mov ecx, [edi+0x37C]
        push ecx
        call upgrade_can_be_learned
        test eax, eax
        jz fail
        mov edx, [esp+0x24]
        lea eax, [ebx+edx]
        jmp UPGRADE_UI_CHECK_EXEC_ABSOLUTE
        fail:
        jmp UPGRADE_UI_CHECK_FAIL_ABSOLUTE
    }*/
void __declspec(naked) ui_check_can_upgrade_hook()
{
    asm("mov 0x24(%%esp), %%edx\n\t"
        "mov 0x20(%%esp), %%ecx\n\t"
        "movw (%%ebx, %%edx), %%ax\n\t"
        "push %%eax\n\t"
        "push %%ecx\n\t"
        "mov 0x37C(%%edi), %%ecx\n\t"
        "call *%0\n\t"
        "test %%eax, %%eax\n\t"
        "jz 1f\n\t"
        "mov 0x24(%%esp), %%edx\n\t"
        "lea (%%ebx, %%edx), %%eax\n\t"
        "jmp *%1 \n\t"
        "1: jmp *%2\n\t":
       :"o"(upgrade_can_be_learned), "o"(UPGRADE_UI_CHECK_EXEC_ABSOLUTE),"o"(UPGRADE_UI_CHECK_FAIL_ABSOLUTE));
}

void __declspec(naked) ui_check_can_upgrade_hook_beta()
{
    asm("movzw (%%ebx, %%esi), %%eax \n\t"
        "push %%eax                  \n\t"
        "push -0x94(%%ebp)           \n\t"
        "mov 0x37C(%%edi), %%ecx     \n\t"
        "call %P0                    \n\t"
        "test %%eax, %%eax           \n\t"
        "jz 1f                       \n\t"
        "mov -0x90(%%ebp), %%eax     \n\t"
        "jmp *%1                     \n\t"
        "1: jmp *%2                  \n\t":
        :"i"(upgrade_can_be_learned), "o"(UPGRADE_UI_CHECK_EXEC_ABSOLUTE),"o"(UPGRADE_UI_CHECK_FAIL_ABSOLUTE));
}


bool __attribute__((no_caller_saved_registers, thiscall)) check_available_supply(void * _this, unsigned int upg_id, unsigned int unk_data, unsigned int building_id)
{

    unsigned short unit_id, spawn_count, supply_cost;
    if (upg_G.get_spawn_data(upg_id, unit_id, spawn_count, supply_cost))
    {
        void * (__thiscall *building_get_race)(void *, unsigned int) = ASI::AddrOf(0x2C6820);
        unsigned short (__thiscall *player_get_warrior_count) (void *, unsigned int, unsigned short) = ASI::AddrOf(0x2A2900);
        unsigned int (__thiscall *limit_support_function)(void *) = ASI::AddrOf(0x27741F);
        unsigned short (__thiscall *get_unit_limit) (void *, unsigned int, unsigned short) = ASI::AddrOf(0x327C50);
        unsigned short race = building_get_race(*(void **)((int)_this + 0x380),building_id);
        unsigned short side = IsDarkRace(race);
        unsigned short warrior_count = player_get_warrior_count(*(void **)((int)_this + 0x37c), unk_data, side);
        unsigned int _t = limit_support_function(*(void **)((int)_this + 0x368));
        unsigned short warrior_limit = get_unit_limit((void *)_t, unk_data, side);
        return warrior_limit >= warrior_count + spawn_count*supply_cost;

    }
    return true;
}

void __declspec(naked) ui_check_supply_available_beta_hook()
{
    asm("mov %%edi, %%ecx            \n\t"
        "push -0x74(%%ebp)           \n\t"
        "push -0x94(%%ebp)           \n\t"
        "mov -0x90(%%ebp), %%ebx     \n\t"
        "movzw (%%ebx, %%esi), %%eax \n\t"
        "push %%eax                  \n\t"
        "call %P0                    \n\t"
        "test %%eax, %%eax           \n\t"
        "jz 1f                       \n\t"
        "mov -0x90(%%ebp), %%ebx     \n\t"
        "jmp *%2                     \n\t"
        "1: jmp *%1                  \n\t":
        :"i"(check_available_supply), "o"(UPGRADE_UI_CHECK_FAIL_ABSOLUTE), "o"(UPGRADE_UI_CHECK_SUPPLY_FAIL));
}

void __declspec(naked) get_upgraded_unit_variant_id_hook()
{
    asm ("call %P0 \n\t"
        "jmp *%1":
        :"i"(get_upgraded_unit_variant_id), "o"(UPGRADE_EXEC_ABSOLUTE));
}


//REWRITE FOR BETA!!
// required for things that are loaded before asi plugins are loaded in, to reload these things entirely
void __stdcall post_init_modifications()
{
    ASI::Pointer AppMain(ASI::AddrOf(ASI::APPMAIN_OFFSET));
    ASI::Pointer AppMenu = (*AppMain)[0x4];

    // reload unit icon cache
    // WARNING: curently loaded cache becomes discarded!!! slight memory leak!


    //Old and working for 1.54 code
    if (ASI::CheckSFVersion(ASI::SF_154))
    {
        ASI::Pointer appmenu_gamedata_stuff = (*AppMenu)[0x50];
        ASI::Pointer unit_icon_list = (*appmenu_gamedata_stuff)[0x6B0];
        ASI::Pointer unit_description_list = (*appmenu_gamedata_stuff)[0x6BC];
    // initialize cache
        ASI::CallClassFunc<0x521EF0, void*, void*, void*>
            (unit_icon_list, AppMenu[0x44], AppMenu[0x4C]);

    // load entries into cache
        ASI::CallClassFunc<0x51B980, bool>
            (*appmenu_gamedata_stuff);

    // reload unit description cache
    // WARNING: currently loaded cache becomes discarded!!! slight memory leak!

    // initialize cache
        ASI::CallClassFunc<0x1FD210, void*, void*, void*>
            (unit_description_list, AppMenu[0x44], AppMenu[0x4C]);

    // load entries into cache
        ASI::CallClassFunc<0x51BAC0, bool>
            (*appmenu_gamedata_stuff);
        return;
    }

    //There will be dragons --functions differ a fucking lot
    if (ASI::CheckSFVersion(ASI::SF_BETA))
    {
        ASI::Pointer appmenu_gamedata_stuff = (*AppMenu)[0x4C];
        ASI::Pointer unit_icon_list = (*appmenu_gamedata_stuff)[0x698];
        ASI::Pointer unit_description_list = (*appmenu_gamedata_stuff)[0x69C];
        //icon cache init
        ASI::CallClassFunc<0x579CE0, void*, void*, void*>
            (unit_icon_list, AppMenu[0x40], AppMenu[0x48]);

        ASI::CallClassFunc<0x573D40, bool>
            (*appmenu_gamedata_stuff);


        //unit description init
        ASI::CallClassFunc<0x2542E0, void*, void*, void*>
            (unit_description_list, AppMenu[0x40], AppMenu[0x48]);


        ASI::CallClassFunc<0x573E00, bool>
            (*appmenu_gamedata_stuff);

        return;
    }
}

void __declspec(naked) post_init_modifications_hook()
{
    asm(
        "call %0\n\t"
        "mov 0x94(%%esp), %%ecx\n\t"
        "jmp *%1":
         :"m"(post_init_modifications), "o"(POST_INIT_EXEC_ABSOLUTE));
}

void __declspec(naked) post_init_modifications_hook_beta()
{
    //function tail differs from 1.54, so I've taken one more instruction for safety
    asm(
        "call %P0\n\t"
        "mov -0xc(%%ebp), %%ecx\n\t"
        "mov %%ecx, %%fs:0x0\n\t"
        "jmp *%1\n\t":
        :"i"(post_init_modifications), "o"(POST_INIT_EXEC_ABSOLUTE));

}


struct upgrade_line
{
    int command_type;
    int upgrade_id1;
    unsigned short unit_base_id;
    unsigned short unit_upgrade_id;
    int upgrade_id2;
    int spawn_count;
};

// returns if successfully read
template<class T>
bool readint(char* str, int& index, T& result)
{
    result = 0;

    int start_index = index;
    for (; index < 64; index++)
    {
        if ((str[index] == ' ') || (str[index] == '\0'))
            break;
    }

    int mul = 1;
    for (int i = index-1; i >= start_index; i--)
    {
        result += (str[i]-'0') * mul;
        mul *= 10;
    }

    index += 1;

    return true;
}

// returns if command was successfully parsed
bool parse_upgrade_command(char* str, upgrade_line* upgl)
{
    int index = 0;
    for (; index < 64; index++)
    {
        if ((str[index] == ' ')||(str[index] == '\0'))
            break;
    }

    char command[16];
    for (int i = 0; i < 15; i++)
        command[i] = str[i];
    command[index] = '\0';
    command[15] = '0';

    index += 1;
    if (strcmp(command, "upgrade") == 0)
    {
        upgl->command_type = 0;
        if (!readint<int>(str, index, upgl->upgrade_id1)) return false;
        if (!readint<unsigned short>(str, index, upgl->unit_base_id)) return false;
        if (!readint<unsigned short>(str, index, upgl->unit_upgrade_id)) return false;
    }
    else if (strcmp(command, "chain") == 0)
    {
        upgl->command_type = 1;
        if (!readint<int>(str, index, upgl->upgrade_id1)) return false;
        if (!readint<int>(str, index, upgl->upgrade_id2)) return false;
    }
    else if (strcmp(command, "exclusive") == 0)
    {
        upgl->command_type = 2;
        if (!readint<int>(str, index, upgl->upgrade_id1)) return false;
        if (!readint<int>(str, index, upgl->upgrade_id2)) return false;
    }
    else if (strcmp(command, "spawn") == 0)
    {
        upgl->command_type = 3;
        if (!readint<int>(str, index, upgl->upgrade_id1)) return false; //ID
        if (!readint<unsigned short>(str, index, upgl->unit_upgrade_id)) return false; //spawn unit id
        if (!readint<int>(str, index, upgl->spawn_count)) return false; // spawn count
        if (!readint<unsigned short>(str, index, upgl->unit_base_id)) return false; //supply cost per unit


    }
    else if (strcmp(command, "repeatable") == 0)
    {
        upgl->command_type = 4;
        if (!readint<int>(str, index, upgl->upgrade_id1)) return false;
    }
    else 
    {
        upgl->command_type = -1;
    }

    return true;
}


bool InitializeUnitUpgradeData()
{
    char message[100];

    std::fstream myfile("upgrades_config", std::ios_base::in);
    if (!myfile.is_open())
    {
        sprintf_s(message, 80, "Could not open config file");
        MessageBoxA(game_window, message, "New upgrades message", MB_OK);
        return false;
    }

    upgrade_line upg_l;

    char line[64];

    int MAX_G_SIZE = 256;
    upg_G.init(MAX_G_SIZE);
    // parse upgrade file

    int line_c = 1;
    while (!myfile.fail())
    {
        myfile.getline(line, 64);
        if (!parse_upgrade_command(line, &upg_l))
        {
            sprintf_s(message, 100, "Could not read from file (line %i)", line_c);
            MessageBoxA(game_window, message, "New upgrades message", MB_OK);
            myfile.close();
            return false;
        }

        switch (upg_l.command_type)
        {
        case -1:  // no command
        {
            break;
        }
        case 0:   // "upgrade"
        {
            if (!upg_G.add(upg_l.upgrade_id1, upg_l.unit_base_id, upg_l.unit_upgrade_id))
            {
                sprintf_s(message, 100, "Could not add upgrade %i (units %hi, %hi) (line %i)", upg_l.upgrade_id1, upg_l.unit_base_id, upg_l.unit_upgrade_id, line_c);
                MessageBoxA(game_window, message, "New upgrades message", MB_OK);
                myfile.close();
                return false;
            }

            if (upg_l.upgrade_id1 > max_upgrade_index)
                max_upgrade_index = upg_l.upgrade_id1;
            if (MAX_G_SIZE <= upg_l.upgrade_id1)
            {
                myfile.close();
                return false;
            }

            break;
        }
        case 1:   // "chain"
        {
            if (!upg_G.chain(upg_l.upgrade_id1, upg_l.upgrade_id2))
            {
                sprintf_s(message, 100, "Could not chain upgrade %i with %i (line %i)", upg_l.upgrade_id1, upg_l.upgrade_id2, line_c);
                MessageBoxA(game_window, message, "New upgrades message", MB_OK);
                myfile.close();
                return false;
            }
            break;
        }
        case 2:   // "exclusive"
        {
            if (!upg_G.exclude(upg_l.upgrade_id1, upg_l.upgrade_id2))
            {
                sprintf_s(message, 100, "Could not exclude upgrade %i from %i (line %i)", upg_l.upgrade_id1, upg_l.upgrade_id2, line_c);
                MessageBoxA(game_window, message, "New upgrades message", MB_OK);
                myfile.close();
                return false;
            }
            break;
        }
        case 3: //spawn
        {
            if (!upg_G.add_spawn(upg_l.upgrade_id1, upg_l.unit_upgrade_id, upg_l.spawn_count, upg_l.unit_base_id))
            {
                sprintf_s(message, 100, "Could not add upgrade %i (units %hi, %hi) (line %i)", upg_l.upgrade_id1, upg_l.unit_upgrade_id, upg_l.spawn_count, line_c);
                MessageBoxA(game_window, message, "New upgrades message", MB_OK);
                myfile.close();
                return false;
            }

            if (upg_l.upgrade_id1 > max_upgrade_index)
                max_upgrade_index = upg_l.upgrade_id1;
            if (MAX_G_SIZE <= upg_l.upgrade_id1)
            {
                myfile.close();
                return false;
            }
            break;
        }
        case 4: //repeatable
        {
            if (!upg_G.set_repeatable(upg_l.upgrade_id1))
            {
                myfile.close();
                return false;
            } 
            break;
        }
        default:
        {
            myfile.close();
            return false;
        }
        }
        line_c += 1;
    }

    if (!myfile.eof())
    {
        sprintf_s(message, 100, "Error reading file");
        MessageBoxA(game_window, message, "New upgrades message", MB_OK);
        myfile.close();
        return false;
    }

    myfile.close();

    return true;
}

bool InitializeUnitIconsData()
{
    std::fstream myfile("unit_icons_config", std::ios_base::in);
    if (!myfile.is_open())
        return false;

    std::vector<ASI::UiUnitIconInfo> unit_icon_preload;

    int unit_id;
    std::string unit_name;


    while (!myfile.fail())
    {
        myfile >> unit_id;
        if (myfile.eof())
            break;

        myfile >> unit_name;

        unit_icon_preload.push_back(ASI::UiUnitIconInfo(unit_id, unit_name));
    }

    if (!myfile.eof())
        return false;

    myfile.close();

    unit_icons = new ASI::UiUnitIconInfo[unit_icon_preload.size()];
    for (int i = 0; i < unit_icon_preload.size(); i++)
        unit_icons[i] = ASI::UiUnitIconInfo(unit_icon_preload[i]);

    max_ui_icon_index = unit_icon_preload.size();

    return true;
}

bool InitializeUnitDescriptionsData()
{
    std::fstream myfile("unit_descriptions_config", std::ios_base::in);
    if (!myfile.is_open())
        return false;

    std::vector<ASI::UiUnitDescriptionInfo> unit_descriptions_preload;

    unsigned int unit_id;
    unsigned int desc_id;

    while (!myfile.fail())
    {
        myfile >> unit_id;
        if (myfile.eof())
            break;

        myfile >> desc_id;

        unit_descriptions_preload.push_back(ASI::UiUnitDescriptionInfo(unit_id, desc_id));
    }

    if (!myfile.eof())
        return false;

    myfile.close();

    unit_descriptions = new ASI::UiUnitDescriptionInfo[unit_descriptions_preload.size()];
    for (int i = 0; i < unit_descriptions_preload.size(); i++)
        unit_descriptions[i] = ASI::UiUnitDescriptionInfo(unit_descriptions_preload[i]);

    max_ui_description_index = unit_descriptions_preload.size();

    return true;
}


void __thiscall on_unit_death(void * _this, unsigned short figure_id)
{
    unsigned int unit_stat_offset = figure_id*0x2b3;
    unsigned short *data = *(unsigned short *)(*(int *)((int)_this + 0x1c) + 0x1c + unit_stat_offset);
    unsigned char is_dark_race = IsDarkRace(*(char *)(*(int *)((int)_this + 0x1c) + 0x1a + unit_stat_offset));
    unsigned short upg_unit_id;
    unsigned short spawn_count;
    unsigned short supply_cost;
    unsigned short unit_id = *(unsigned short *)((int)(*(void **)((int)_this + 0x1c)) + unit_stat_offset + 0x28);
    for (int i = 0; i <= max_upgrade_index; i++)
    {
        if (upg_G.get_spawn_data(i, upg_unit_id, spawn_count, supply_cost))
        {
            if (upg_unit_id == unit_id)
            {
                if (supply_cost > 1)
                {
                    for (int j = 0; j < supply_cost-1; j++)
                    {
                        support_functions.free_army_slot(*(void**)((unsigned int)_this+0x34), (unsigned int)data, is_dark_race);
                   }
                }
                if (supply_cost == 0)
                {

                }
            }
        }
    } 

}


unsigned int UNIT_DEATH_RET;
void __declspec(naked) on_unit_death_beta_hook()
{
    asm("mov %%edi, %%ecx         \n\t"
        "push 0x8(%%ebp)          \n\t"
        "call %P1                 \n\t"
        "pop %%edi                \n\t"
        "pop %%esi                \n\t"
        "xor %%eax, %%eax         \n\t"
        "pop %%ebx                \n\t"
        "jmp *%0                  \n\t":
        :"o"(UNIT_DEATH_RET), "i"(on_unit_death));
}

void __thiscall unit_assign_army_slots(void* _this, unsigned short unit_id, unsigned char slot_count)
{
    unsigned int unit_stat_offset = unit_id*0x2b3;
    unsigned short *data = *(unsigned short *)(*(int *)((int)_this + 0x1c) + 0x1c + unit_stat_offset);
    unsigned char is_dark_race = IsDarkRace(*(char *)(*(int *)((int)_this + 0x1c) + 0x1a + unit_stat_offset));
    for (int i = 0; i < slot_count; i++)
    {
        support_functions.allocate_army_slot(*(void**)((unsigned int)_this+0x34), (unsigned int)data, is_dark_race);
    }
}

void __thiscall spawn_custom_unit(void *_this, unsigned int param1, unsigned int param3, unsigned short upgrade_id)
{
    unsigned char vector_base[33];
    unsigned int local_74;
    unsigned int local_78;
    unsigned int local_80;
    unsigned int local_64 = 0;
    unsigned char local_54[0x50];
    unsigned char local_5a = 0xA6;
    unsigned short unit_id = 0;
    unsigned short spawn_count = 0;
    unsigned short unit_cost = 0;
    support_functions.init_unknown_stuff_f120(&local_74); //IDK
    void * (__thiscall *unknown_function_ptr)(void *) = ASI::AddrOf(0x17DA10);
    support_functions.vector_constructor_iterator((void *)vector_base, 3, 10, unknown_function_ptr); //we need 10 elements of FFFF FF, so 30 bytes
    //here we choose what unit we are spawning depending on race. 
    if (!upg_G.get_spawn_data(upgrade_id+1, unit_id, spawn_count, unit_cost)) //yes, game counts them from 0 internally here, gotta +1 id
        return false;

    //for test purposes -- we spawn our test unit (bare-handed warder)
    //unit_id = 2997;
    
    for (int i = 0; i<spawn_count;i++)
    {
        unsigned short t = support_functions.get_unknown_field_23a0(*(void**)((unsigned int)_this+0x60), param3);
        unsigned int t1 = support_functions.get_unknown_data_f130(&local_80, 0, 0x14);//WTF? Gotta check with debugger, wtf it's a pointer to WHERE?!
        bool pos_found = support_functions.unit_find_spawn_position(*(void**)((unsigned int)_this+0x64), param3, t1, t, &local_74);
        bool data_found = support_functions.unit_get_data(*(void**)((unsigned int)_this+0x50),unit_id,&local_78);
        bool unknown_data_found = support_functions.unit_copy_data(*(void**)((unsigned int)_this+0x50),local_78,&local_54);
        if (pos_found && unknown_data_found && data_found)
        {
            unsigned short t2 = support_functions.get_player_figure_id(*(void**)((unsigned int)_this+0x48), param1); //get player figure id
            t2 = support_functions.unit_get_level(*(void**)((unsigned int)_this+0x24),t2); //get avatar level
            local_54[2] = t2 & 0xFF; //Probably right..?
            local_54[3] = 0x00; //here we apply level of hero to unit
            t1 = support_functions.figure_add(_this, local_74,(local_74>>0x10), param1, &local_54, 0xb, 0); //local_74 is XY coordinates, where X is lower 16 bits, Y is upper
            local_64 = t1 & 0xffff;
            if (local_64 != 0)
            {
                support_functions.figure_transform(_this, local_64, unit_id, 0, 0);
                t1 = support_functions.unit_get_level(*(void**)((unsigned int)_this+0x24), local_64); //get unit level
                t1 = t1 & 0xff;
                t2 = unit_functions.unit_get_unknown(*(void**)((unsigned int)_this+0x24), local_64);
                unit_functions.unit_set_unknown(*(void**)((unsigned int)_this+0x24),local_64, 
                    (int)(t2 * (t1 * local_5a + 5000))/10000); //Some pointer arithmetics?

                t2 = unit_functions.unit_get_health(*(void**)((unsigned int)_this+0x24), local_64);
                unit_functions.unit_set_health(*(void**)((unsigned int)_this+0x24),local_64, 
                    (int)(t2 * (t1 * local_5a + 5000))/10000); 

                t2 = unit_functions.unit_get_mana(*(void**)((unsigned int)_this+0x24), local_64);
                unit_functions.unit_set_mana(*(void**)((unsigned int)_this+0x24),local_64, 
                    (int)(t2 * (t1 * local_5a + 5000))/10000);
                if (t1 < 0x1f)
                {
                    t2 = unit_functions.unit_get_agility(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_agility(*(void**)((unsigned int)_this+0x24),local_64, 
                        (int)(t2 * (t1 * local_5a + 5000))/10000);
                    t2 = unit_functions.unit_get_charisma(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_charisma(*(void**)((unsigned int)_this+0x24),local_64, 
                        (int)(t2 * (t1 * local_5a + 5000))/10000);
                    t2 = unit_functions.unit_get_dexterity(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_dexterity(*(void**)((unsigned int)_this+0x24),local_64, 
                        (int)(t2 * (t1 * local_5a + 5000))/10000);
                    t2 = unit_functions.unit_get_intelligence(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_intelligence(*(void**)((unsigned int)_this+0x24),local_64, 
                        (int)(t2 * (t1 * local_5a + 5000))/10000);
                    t2 = unit_functions.unit_get_stamina(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_stamina(*(void**)((unsigned int)_this+0x24),local_64, 
                        (int)(t2 * (t1 * local_5a + 5000))/10000);
                    t2 = unit_functions.unit_get_strength(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_strength(*(void**)((unsigned int)_this+0x24),local_64, 
                        (int)(t2 * (t1 * local_5a + 5000))/10000);
                    t2 = unit_functions.unit_get_wisdom(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_wisdom(*(void**)((unsigned int)_this+0x24),local_64, 
                        (int)(t2 * (t1 * local_5a + 5000))/10000);
                }
                else
                {
                    t2 = unit_functions.unit_get_agility(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_agility(*(void**)((unsigned int)_this+0x24),local_64, 
                        ((int)(t2 * (t1 * local_5a + 5000))/10000) - 0x3c + t1 * 2);
                    
                    t2 = unit_functions.unit_get_charisma(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_charisma(*(void**)((unsigned int)_this+0x24),local_64, 
                        ((int)(t2 * (t1 * local_5a + 5000))/10000) - 0x3c + t1 * 2);
                    
                    t2 = unit_functions.unit_get_dexterity(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_dexterity(*(void**)((unsigned int)_this+0x24),local_64, 
                        ((int)(t2 * (t1 * local_5a + 5000))/10000) - 0x3c + t1 * 2);
                    
                    t2 = unit_functions.unit_get_intelligence(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_intelligence(*(void**)((unsigned int)_this+0x24),local_64, 
                        ((int)(t2 * (t1 * local_5a + 5000))/10000) - 0x3c + t1 * 2);
                    
                    t2 = unit_functions.unit_get_stamina(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_stamina(*(void**)((unsigned int)_this+0x24),local_64, 
                        ((int)(t2 * (t1 * local_5a + 5000))/10000) - 0x3c + t1 * 2);
                    
                    t2 = unit_functions.unit_get_strength(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_strength(*(void**)((unsigned int)_this+0x24),local_64, 
                        ((int)(t2 * (t1 * local_5a + 5000))/10000) - 0x3c + t1 * 2);

                    t2 = unit_functions.unit_get_wisdom(*(void**)((unsigned int)_this+0x24), local_64);
                    unit_functions.unit_set_wisdom(*(void**)((unsigned int)_this+0x24),local_64, 
                        ((int)(t2 * (t1 * local_5a + 5000))/10000) - 0x3c + t1 * 2);

                    unit_functions.unit_set_level(*(void**)((unsigned int)_this+0x24), local_64, 
                        (unsigned char)(((((unsigned int) t1 * 1000)/100)*0x3c)/1000));

                }
                unit_assign_army_slots (*(void**)((unsigned int)_this+0x4C), local_64, unit_cost);
            }
        }

    }
}

void HookLegacyVersion()
{
        UPGRADE_EXEC_ABSOLUTE = ASI::AddrOf(0x25B1A2);
        UPGRADE_CHECK_FAIL_ABSOLUTE = ASI::AddrOf(0x3E16FA);
        UPGRADE_CHECK_EXEC_ABSOLUTE = ASI::AddrOf(0x3E15A9);
        UPGRADE_UI_CHECK_FAIL_ABSOLUTE = ASI::AddrOf(0x639DE7);
        UPGRADE_UI_CHECK_EXEC_ABSOLUTE = ASI::AddrOf(0x639D95);
        POST_INIT_EXEC_ABSOLUTE = ASI::AddrOf(0x00F558);

        ASI::MemoryRegion mreg(ASI::AddrOf(0x25B19D), 5);
        ASI::MemoryRegion mreg2(ASI::AddrOf(0x63A1D3), 1);
        ASI::MemoryRegion mreg3(ASI::AddrOf(0x51B9CD), 9);
        ASI::MemoryRegion mreg6(ASI::AddrOf(0x51BB09), 9);

        ASI::MemoryRegion mreg_check_upg(ASI::AddrOf(0x3E15A2), 7);
        ASI::MemoryRegion mreg_ui_check_upg(ASI::AddrOf(0x639D8E), 7);

        ASI::MemoryRegion post_init_mreg(ASI::AddrOf(0x00F551), 7);

        // A3A1D0

        // replace original get_upgraded_unit_variant_id with forged variant
        ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x25B19D)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x25B19E)) = (int)(&get_upgraded_unit_variant_id_hook) - ASI::AddrOf(0x25B1A2);
        ASI::EndRewrite(mreg);

        // replace max upgrade index constant
        ASI::BeginRewrite(mreg2);
        *(unsigned char*)(ASI::AddrOf(0x63A1D3)) = (char)(max_upgrade_index+1);
        ASI::EndRewrite(mreg2);

        // inject special upgrade check
        ASI::BeginRewrite(mreg_check_upg);
        *(unsigned char*)(ASI::AddrOf(0x3E15A2)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x3E15A3)) = (int)(&check_can_upgrade_hook) - ASI::AddrOf(0x3E15A7);
        *(unsigned char*)(ASI::AddrOf(0x3E15A7)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x3E15A8)) = 0x90;   // nop instruction
        ASI::EndRewrite(mreg_check_upg);

        // inject special ui upgrade check
        ASI::BeginRewrite(mreg_ui_check_upg);
        *(unsigned char*)(ASI::AddrOf(0x639D8E)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x639D8F)) = (int)(&ui_check_can_upgrade_hook) - ASI::AddrOf(0x639D93);
        *(unsigned char*)(ASI::AddrOf(0x639D93)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x639D94)) = 0x90;   // nop instruction
        ASI::EndRewrite(mreg_ui_check_upg);

        // replace ui unit icon data
        ASI::BeginRewrite(mreg3);
        *(int*)(ASI::AddrOf(0x51B9CD)) = (int)(unit_icons);
        *(int*)(ASI::AddrOf(0x51B9D2)) = max_ui_icon_index;
        ASI::EndRewrite(mreg3);

        // replace ui unit description data
        ASI::BeginRewrite(mreg6);
        *(int*)(ASI::AddrOf(0x51BB09)) = (int)(unit_descriptions) + 4;               // offset is necessary here
        *(int*)(ASI::AddrOf(0x51BB0E)) = max_ui_description_index;
        ASI::EndRewrite(mreg6);

        // hook into init routine at the end, to reload cache based on replaced data
        ASI::BeginRewrite(post_init_mreg);
        *(unsigned char*)(ASI::AddrOf(0x00F551)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x00F552)) = (int)(&post_init_modifications_hook) - ASI::AddrOf(0x00F556); 
        ASI::EndRewrite(post_init_mreg);

}

/*
void __declspec(naked) spell_type_hook_beta()
{
    asm(
        "cmpl $0xf1, %%eax\n\t"
        "jne 1f\n\t"
        "jmp *%1\n\t"
        "1: cmpb $0xf0, %%al\n\t"
        "jmp *%0\n\t\n\t":
       : "o"(SPELL_TYPE_FAIL_ABSOLUTE),"o"(SPELL_TYPE_EXEC_ABSOLUTE));
}
*/

unsigned int BHANDLE_EXEC;
unsigned int BHANDLE_FAIL;
void __declspec(naked) on_finish_upgrade_special_beta_hook()
{
    asm("cmpb $0x36, %%al              \n\t" //it's -1 smh
        "jl 1f                         \n\t"
        "push %%eax                    \n\t"
        "mov 0xc(%%edi), %%ecx         \n\t"
        "mov 0x8(%%ebp), %%eax         \n\t" //mov eax, dword ptr [ebp+param1=0x8]
        "mov 0x4(%%ecx, %%eax), %%eax  \n\t" //mov eax, dword ptr [ecx + eax*1 + 4]
        "mov %%eax, 0x8(%%ebp)         \n\t"//mov dword ptr [ebp+param1=0x8], eax
        "lea 0x8(%%ebp), %%eax         \n\t" //lea eax, [ebp+8]
        "push %%eax                    \n\t"
        "push -0x4(%%ebp)              \n\t"
        "mov 0x24(%%edi), %%ecx        \n\t"
        "call %P2                      \n\t"
        "jmp 2f                        \n\t"
        "1: cmpb $0x35, %%al           \n\t"
        "ja 2f                         \n\t"
        "jmp *%1                       \n\t"
        "2: jmp *%0                    \n\t":
        :"o"(UPGRADE_SPECIAL_EXEC_ABSOLUTE), "o"(UPGRADE_SPECIAL_FAIL_ABSOLUTE), "i"(spawn_custom_unit));
}

int __attribute__((no_caller_saved_registers, thiscall)) is_upgrade_repeatable(unsigned char uprgade_id)
{
    if (upg_G.get_repeatable(uprgade_id))
    {
        return 1;
    }
    return 0;
}

void __declspec(naked) building_manager_handle_building_hook_beta() //gotta figure out, how to cancel upgrade
{
    asm("movzw 0x10(%%esi), %%ecx   \n\t" //and here it's raw
        "call %P2                   \n\t"
        "test %%eax, %%eax          \n\t"
        "je 1f                      \n\t"
        "jmp *%0                    \n\t"
        "1:lea -0x25(%%ecx), %%eax  \n\t"
        "jmp *%1                    \n\t":
        :"o"(BHANDLE_EXEC), "o"(BHANDLE_FAIL), "i"(is_upgrade_repeatable):"ecx");
}


void HookBetaVersion()
{
    UPGRADE_EXEC_ABSOLUTE = ASI::AddrOf(0x2bb2fd);
    UPGRADE_CHECK_FAIL_ABSOLUTE = ASI::AddrOf(0x326b15);
    UPGRADE_CHECK_EXEC_ABSOLUTE = ASI::AddrOf(0x3269d2);
    UPGRADE_UI_CHECK_FAIL_ABSOLUTE = ASI::AddrOf(0x646574);//Not sure at all
    UPGRADE_UI_CHECK_EXEC_ABSOLUTE = ASI::AddrOf(0x64650D); //Do not forget to mov edi to ecx
    POST_INIT_EXEC_ABSOLUTE = ASI::AddrOf(0x178340); //Seems fine, I hope?

    UPGRADE_SPECIAL_FAIL_ABSOLUTE = ASI::AddrOf(0x2D8C8E);
    UPGRADE_SPECIAL_EXEC_ABSOLUTE = ASI::AddrOf(0x2D8D88);

    UPGRADE_UI_CHECK_SUPPLY_FAIL = ASI::AddrOf(0x646528);
    UPGRADE_UI_CHECK_SUPPLY_EXEC = ASI::AddrOf(0x646771);
    BHANDLE_EXEC = ASI::AddrOf(0x2DB08E);
    BHANDLE_FAIL = ASI::AddrOf(0x2DB077);

    UNIT_DEATH_RET = ASI::AddrOf(0x326F32);

    ASI::MemoryRegion mreg (ASI::AddrOf(0x2bb2f8), 5);
    ASI::MemoryRegion mreg2(ASI::AddrOf(0x6468fe), 1);
    ASI::MemoryRegion mreg3(ASI::AddrOf(0x573d90), 9);
    ASI::MemoryRegion mreg6(ASI::AddrOf(0x573e4d), 9); 
    
    ASI::MemoryRegion mreg_check_upg (ASI::AddrOf(0x3269cc), 5);  //gotta fix asm here, alarm

    ASI::MemoryRegion mreg_ui_check_upg(ASI::AddrOf(0x646507),6);
    ASI::MemoryRegion post_init_mreg(ASI::AddrOf(0x178336), 10);

    ASI::MemoryRegion upgrade_finish_special_mreg(ASI::AddrOf(0x2D8C88), 6);
    ASI::MemoryRegion building_manager_handle_mreg(ASI::AddrOf(0x2DB070), 7);

    ASI::MemoryRegion unit_death_mreg(ASI::AddrOf(0x326F2D), 5);
    ASI::MemoryRegion ui_supply_check_mreg(ASI::AddrOf(0X646522), 6);

  ASI::BeginRewrite(ui_supply_check_mreg);
        *(unsigned char*)(ASI::AddrOf(0X646522)) = 0x90;   
        *(unsigned char*)(ASI::AddrOf(0X646523)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0X646524)) = (int)(&ui_check_supply_available_beta_hook) - ASI::AddrOf(0x646528);
    ASI::EndRewrite(ui_supply_check_mreg);

    ASI::BeginRewrite(unit_death_mreg);
       *(unsigned char*)(ASI::AddrOf(0x326F2D)) = 0xE9;   // jmp instruction
       *(int*)(ASI::AddrOf(0x326F2E)) = (int)(&on_unit_death_beta_hook) - ASI::AddrOf(0x326F32);
    ASI::EndRewrite(unit_death_mreg);

    ASI::BeginRewrite(building_manager_handle_mreg);
        *(unsigned char*)(ASI::AddrOf(0x2DB070)) = 0x90;   
        *(unsigned char*)(ASI::AddrOf(0x2DB071)) = 0x90;   
        *(unsigned char*)(ASI::AddrOf(0x2DB072)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2DB073)) = (int)(&building_manager_handle_building_hook_beta) - ASI::AddrOf(0x2DB077);
    ASI::EndRewrite(building_manager_handle_mreg);


    SPELL_TYPE_FAIL_ABSOLUTE = ASI::AddrOf(0x328e48);
    SPELL_TYPE_EXEC_ABSOLUTE = ASI::AddrOf(0x32924e);

    ASI::BeginRewrite(upgrade_finish_special_mreg);
        *(unsigned char*)(ASI::AddrOf(0x2D8C88)) = 0x90;   
        *(unsigned char*)(ASI::AddrOf(0x2D8C89)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2D8C8A)) = (int)(&on_finish_upgrade_special_beta_hook) - ASI::AddrOf(0x2D8C8E);
    ASI::EndRewrite(upgrade_finish_special_mreg);

    ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x2bb2f8)) = 0xE8;   // call. Just for lulz
        *(int*)(ASI::AddrOf(0x2bb2f9)) = (int)(&get_upgraded_unit_variant_id) - ASI::AddrOf(0x2bb2fd);
    ASI::EndRewrite(mreg);

    // replace max upgrade index constant
    ASI::BeginRewrite(mreg2);
        *(unsigned char*)(ASI::AddrOf(0x6468fe)) = (char)(max_upgrade_index+1);
    ASI::EndRewrite(mreg2);
    
    // replace ui unit icon data
    ASI::BeginRewrite(mreg3);
        *(int*)(ASI::AddrOf(0x573d90)) = (int)(unit_icons);
        *(int*)(ASI::AddrOf(0x573d95)) = max_ui_icon_index;
    ASI::EndRewrite(mreg3);
    
    // replace ui unit description data
    ASI::BeginRewrite(mreg6);
        *(int*)(ASI::AddrOf(0x573e4d)) = (int)(unit_descriptions) + 4;               // offset is necessary here
        *(int*)(ASI::AddrOf(0x573e52)) = max_ui_description_index;
    ASI::EndRewrite(mreg6);

    ASI::BeginRewrite(mreg_ui_check_upg);
        *(unsigned char*)(ASI::AddrOf(0x646507)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x646508)) = (int)(&ui_check_can_upgrade_hook_beta) - ASI::AddrOf(0x64650C);
        *(unsigned char*)(ASI::AddrOf(0x64650C)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg_ui_check_upg);

    // hook into init routine at the end, to reload cache based on replaced data
    ASI::BeginRewrite(post_init_mreg);
        *(unsigned char*)(ASI::AddrOf(0x178336)) = 0x90;   
        *(unsigned char*)(ASI::AddrOf(0x178337)) = 0x90;   
        *(unsigned char*)(ASI::AddrOf(0x178338)) = 0x90;   // NOP trail
        *(unsigned char*)(ASI::AddrOf(0x178339)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x17833A)) = (int)(&post_init_modifications_hook_beta) - ASI::AddrOf(0x17833E); 
    ASI::EndRewrite(post_init_mreg);

}

void init_upgrade_functions_beta()
{
    upgrade_functions.upgrade_activated = ASI::AddrOf(0x2A51A0);
    upgrade_functions.cancel_ugrade = ASI::AddrOf(0x2A5080);
    upgrade_functions.ugrade_started_learning = ASI::AddrOf(0x2A5120);
}

void init_support_functions_beta()
{
    support_functions.init_unknown_stuff_f120 = ASI::AddrOf(0x2CF120); //IDK WTF is it
    support_functions.vector_constructor_iterator = ASI::AddrOf(0x174D40); //IDK why it is here
    support_functions.get_unknown_field_23a0 = ASI::AddrOf(0x2D23A0);
    support_functions.get_unknown_data_f130 = ASI::AddrOf(0x2CF130);
    support_functions.unit_get_data = (unit_get_data_ptr)ASI::AddrOf(0x26E7C0);
    support_functions.unit_find_spawn_position = ASI::AddrOf(0x34E9A0);
    support_functions.unit_copy_data = ASI::AddrOf(0x2686F0);
    support_functions.get_player_figure_id = ASI::AddrOf(0x2793D0);
    support_functions.unit_get_level = (unit_get_level_ptr)ASI::AddrOf(0x2792B0);
    support_functions.figure_add = (figure_add_ptr)ASI::AddrOf(0x2F6082);
    support_functions.figure_transform = (figure_transform_ptr)ASI::AddrOf(0x300C45);
    support_functions.allocate_army_slot = (allocate_army_slot_ptr)ASI::AddrOf(0x2A2BF0);
    support_functions.free_army_slot = (free_army_slot_ptr)ASI::AddrOf(0x2A1D80);
}

void init_unit_functions_beta()
{
    unit_functions.unit_get_health = ASI::AddrOf(0x2792F0);
    unit_functions.unit_get_mana = ASI::AddrOf(0x279410);
    unit_functions.unit_get_agility = ASI::AddrOf(0x278CB0);
    unit_functions.unit_get_charisma = ASI::AddrOf(0x278EA0);
    unit_functions.unit_get_dexterity = ASI::AddrOf(0x278F20);
    unit_functions.unit_get_intelligence = ASI::AddrOf(0x279230);
    unit_functions.unit_get_stamina = ASI::AddrOf(0x279790);
    unit_functions.unit_get_strength = ASI::AddrOf(0x2797F0);
    unit_functions.unit_get_wisdom = ASI::AddrOf(0x279AC0);
    unit_functions.unit_get_unknown = ASI::AddrOf(0x278C30);
    unit_functions.unit_set_health = ASI::AddrOf(0x2B6C70);
    unit_functions.unit_set_mana = ASI::AddrOf(0x2B6CD0);
    unit_functions.unit_set_agility = ASI::AddrOf(0x2B66E0);
    unit_functions.unit_set_charisma = ASI::AddrOf(0x2B6870);
    unit_functions.unit_set_dexterity = ASI::AddrOf(0x2B6970);
    unit_functions.unit_set_intelligence = ASI::AddrOf(0x2B6B50);
    unit_functions.unit_set_stamina = ASI::AddrOf(0x2B7030);
    unit_functions.unit_set_strength = ASI::AddrOf(0x2B7070);
    unit_functions.unit_set_wisdom = ASI::AddrOf(0x2B72A0);
    unit_functions.unit_set_unknown = ASI::AddrOf(0x2B6590);
    unit_functions.unit_set_level = ASI::AddrOf(0x2B6C10);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        if (!ASI::Init(hModule))
            return FALSE;
        if (!ASI::CheckSFVersion(ASI::SF_154) && !ASI::CheckSFVersion(ASI::SF_BETA))
        {
            return FALSE;
        }

        game_window = *(HWND*)(ASI::AddrOf(ASI::WINDOW_OFFSET));
        if (!InitializeUnitUpgradeData())
            return FALSE;
        if (!InitializeUnitIconsData())
            return FALSE;
        if (!InitializeUnitDescriptionsData())
            return FALSE;

       
        if (ASI::CheckSFVersion(ASI::SF_154))
        {
            HookLegacyVersion();
        }

        if (ASI::CheckSFVersion(ASI::SF_BETA))
        {
            init_upgrade_functions_beta();
            init_unit_functions_beta();
            init_support_functions_beta();
            HookBetaVersion();
        }
        break;
    }
    case DLL_PROCESS_DETACH:
        if (unit_icons != 0)
            delete[] unit_icons;
        if (unit_descriptions != 0)
            delete[] unit_descriptions;
        break;
    }
    return TRUE;
}
