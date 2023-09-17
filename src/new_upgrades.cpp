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

int UPGRADE_EXEC_ABSOLUTE;

int UPGRADE_UI_CHECK_EXEC_ABSOLUTE;
int UPGRADE_UI_CHECK_FAIL_ABSOLUTE;

int UPGRADE_CHECK_FAIL_ABSOLUTE;
int UPGRADE_CHECK_EXEC_ABSOLUTE;

int POST_INIT_EXEC_ABSOLUTE;

HWND game_window;

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

// returns if upgrade started learning OR if it was learned
inline int upgrade_started_learning(ASI::Pointer caller, void* unk_ptr, int upgrade_id)
{
    return ASI::CallClassFunc<0x641EE0, unsigned int, void*, int>
        (caller, unk_ptr, upgrade_id);
}

// returns if upgrade was learned
inline int is_upgrade_activated(ASI::Pointer caller, void* unk_ptr, int upgrade_id)
{
    return ASI::CallClassFunc<0x641F70, unsigned int, void*, int>
        (caller, unk_ptr, upgrade_id);
}

// returns if predecessor upgrade is learned (if any) and if exclusive upgrade is not learned (if any)
int __stdcall upgrade_can_be_learned(ASI::Pointer caller, void* unk_ptr, int upgrade_id)
{
    int upg2;

    // check if previous upgrade is learned
    if (upg_G.prev_of(upgrade_id, upg2))
        if (!is_upgrade_activated(caller, unk_ptr, upg2))
            return 0;

    // check if exclusive upgrade is not learned
    if (upg_G.get_exclusive(upgrade_id, upg2))
        if (is_upgrade_activated(caller, unk_ptr, upg2))
            return 0;

    return 1;
}

void __stdcall get_upgraded_unit_variant_id(ASI::Pointer unknown, ASI::Pointer unit_data)
{
    ASI::Pointer stack_pointer(0);
    ASI::Pointer __this(0);
    asm ( "mov %%esp, %0\n\t"
          "mov %%ecx, %1\n\t":
          :"m"(stack_pointer.ptr), "m"(__this,ptr));

/*    __asm mov stack_pointer.ptr, esp
    __asm mov __this.ptr, ecx
*/

    unsigned short unit_id = (unsigned short)(unit_data[0x0]);
    unsigned short upgraded_unit_id = unit_id;
    unsigned short iter_unit_id;
    unsigned short iter_upgraded_unit_id;

    for (int i = 0; i <= max_upgrade_index; i++)
    {
        for (int j = 0; upg_G.get_unit(i, j, iter_unit_id, iter_upgraded_unit_id); j++)
        {
            if (iter_unit_id == unit_id)
            {
                if (is_upgrade_activated((*__this[0x48]), unknown.ptr, i))
                {
                    upgraded_unit_id = iter_upgraded_unit_id;
                }
            }
        }
    }

    (unit_data[0x0]).AsRef<unsigned short>() = upgraded_unit_id;

    /*std::unordered_map<unsigned short, std::pair<int, unsigned short>>::const_iterator uud_val = unit_upgrade_data.find(unit_id);
    if (uud_val != unit_upgrade_data.end())
    {
        if(is_upgrade_activated((*__this[0x48]), unknown.ptr, (*uud_val).second.first))
            (unit_data[0x0]).AsRef<unsigned short>() = (*uud_val).second.second;
    }*/

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
        "push %%ecx\n\t"
        "call *%0\n\t"
        "test %%eax, %%eax\n\t"
        "jz 1f\n\t"
        "mov 0xDC(%%esp), %%edx\n\t"
        "jmp *%1\n\t"
        "1: jmp *%2\n\t":
        :"o"(upgrade_can_be_learned), "o"(UPGRADE_CHECK_EXEC_ABSOLUTE), "o"(UPGRADE_CHECK_FAIL_ABSOLUTE));
}

void __declspec(naked) ui_check_can_upgrade_hook()
{
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
    asm("mov 0x24(%%esp), %%edx\n\t"
        "mov 0x20(%%esp), %%ecx\n\t"
        "movw (%%ebx, %%edx), %%ax\n\t"
        "push %%eax\n\t"
        "push %%ecx\n\t"
        "mov 0x37C(%%edi), %%ecx\n\t"
        "push %%ecx\n\t"
        "call *%0\n\t"
        "test %%eax, %%eax"
        "jz 1f\n\t"
        "mov 0x24(%%esp), %%edx\n\t"
        "lea (%%ebx, %%edx), %%eax\n\t"
        "jmp *%1 \n\t"
        "1: jmp *%2\n\t":
       :"o"(upgrade_can_be_learned), "o"(UPGRADE_UI_CHECK_EXEC_ABSOLUTE),"o"(UPGRADE_UI_CHECK_FAIL_ABSOLUTE));
}

void __declspec(naked) ui_check_can_upgrade_hook_beta()
{
    asm("mov -0x90(%%ebp), %%eax \n\t"
        "add %%esi, %%eax        \n\t"
        "push %%eax              \n\t"
        "push -0x94(%%ebp)       \n\t"
        "mov 0x37C(%%edi), %%ecx \n\t"
        "push %%ecx              \n\t"
        "call *%0                \n\t"
        "test %%eax, %%eax       \n\t"
        "jz 1f                   \n\t"
        "mov -0x90(%%ebp), %%eax \n\t"
        "jmp *%1                 \n\t"
        "1: jmp *%2              \n\t":
        :"o"(upgrade_can_be_learned), "o"(UPGRADE_UI_CHECK_EXEC_ABSOLUTE),"o"(UPGRADE_UI_CHECK_FAIL_ABSOLUTE));
}

void __declspec(naked) get_upgraded_unit_variant_id_hook()
{
 /*   __asm
    {
        call get_upgraded_unit_variant_id
        jmp UPGRADE_EXEC_ABSOLUTE
    }*/
    asm ("call *%0 \n\t"
        "jmp *%1":
        :"o"(get_upgraded_unit_variant_id), "o"(UPGRADE_EXEC_ABSOLUTE));
}


// required for things that are loaded before asi plugins are loaded in, to reload these things entirely
void __stdcall post_init_modifications()
{
    ASI::Pointer AppMain(ASI::AddrOf(ASI::APPMAIN_OFFSET));
    ASI::Pointer AppMenu = (*AppMain)[0x4];
    ASI::Pointer appmenu_gamedata_stuff = (*AppMenu)[0x50];
    ASI::Pointer unit_icon_list = (*appmenu_gamedata_stuff)[0x6B0];
    ASI::Pointer unit_description_list = (*appmenu_gamedata_stuff)[0x6BC];

    // reload unit icon cache
    // WARNING: curently loaded cache becomes discarded!!! slight memory leak!

    // initialize cache
    ASI::CallClassFunc<0x921EF0, void*, void*, void*>
        (unit_icon_list, AppMenu[0x44], AppMenu[0x4C]);

    // load entries into cache
    ASI::CallClassFunc<0x91B980, bool>
        (*appmenu_gamedata_stuff);

    // reload unit description cache
    // WARNING: currently loaded cache becomes discarded!!! slight memory leak!

    // initialize cache
    ASI::CallClassFunc<0x5FD210, void*, void*, void*>
        (unit_description_list, AppMenu[0x44], AppMenu[0x4C]);

    // load entries into cache
    ASI::CallClassFunc<0x91BAC0, bool>
        (*appmenu_gamedata_stuff);
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
        "call %0\n\t"
        "mov -0xc(%%ebp)\n\t"
        "mov %%ecx, %%fs:0x0\n\t"
        "jmp *%1\n\t":
        :"m"(post_init_modifications), "o"(POST_INIT_EXEC_ABSOLUTE));

}


struct upgrade_line
{
    int command_type;
    int upgrade_id1;
    unsigned short unit_base_id;
    unsigned short unit_upgrade_id;
    int upgrade_id2;
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
    else
        upgl->command_type = -1;

    return true;
}

/*bool InitializeUnitUpgradeData()
{
    std::fstream myfile("upgrades_config", std::ios_base::in);
    if (!myfile.is_open())
        return false;

    unsigned short unit_base_id;
    int upgrade_id;
    unsigned short unit_upgrade_id;
    while (!myfile.fail())
    {
        myfile >> unit_base_id;
        if (myfile.eof())
            break;

        myfile >> upgrade_id;
        myfile >> unit_upgrade_id;

        unit_upgrade_data.emplace(unit_base_id, std::pair<int, unsigned short>(upgrade_id, unit_upgrade_id));

        if (upgrade_id > max_upgrade_index)
            max_upgrade_index = (char)upgrade_id;
    }

    if (!myfile.eof())
    return false;

    myfile.close();

    return true;
}
*/

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


void HookBetaVersion()
{
    UPGRADE_EXEC_ABSOLUTE = ASI::AddrOf(0x2bb2fd);
    UPGRADE_CHECK_FAIL_ABSOLUTE = ASI::AddrOf(0x326b15);
    UPGRADE_CHECK_EXEC_ABSOLUTE = ASI::AddrOf(0x3269d2);
    UPGRADE_UI_CHECK_FAIL_ABSOLUTE = ASI::AddrOf(0x646574);//Not sure at all
    UPGRADE_UI_CHECK_EXEC_ABSOLUTE = ASI::AddrOf(0x646511); //Do not forget to mov edi to ecx
    POST_INIT_EXEC_ABSOLUTE = ASI::AddrOf(0x178340); //Seems fine, I hope?




    ASI::MemoryRegion mreg (ASI::AddrOf(0x2bb2fd), 5);
    ASI::MemoryRegion mreg2(ASI::AddrOf(0x6468fe), 1);
    ASI::MemoryRegion mreg3(ASI::AddrOf(0x573d90), 9);
    ASI::MemoryRegion mreg6(ASI::AddrOf(0x573e4d), 9);

    
    
    ASI::MemoryRegion mreg_check_upg (ASI::AddrOf(0x3269cc), 5);  //gotta fix asm here, alarm
    ASI::MemoryRegion mreg_ui_check_upg(ASI::AddrOf(0x646507),6);

    ASI::MemoryRegion post_init_mreg(ASI::AddrOf(0x178336), 10);


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
        *(int*)(ASI::AddrOf(0x646508)) = (int)(&ui_check_can_upgrade_hook) - ASI::AddrOf(0x64650D);
        *(unsigned char*)(ASI::AddrOf(0x64650D)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg_ui_check_upg);


            // hook into init routine at the end, to reload cache based on replaced data
    ASI::BeginRewrite(post_init_mreg);
        *(unsigned char*)(ASI::AddrOf(0x178336)) = 0x90;   
        *(unsigned char*)(ASI::AddrOf(0x178337)) = 0x90;   
        *(unsigned char*)(ASI::AddrOf(0x178338)) = 0x90;   // NOP trail

        *(unsigned char*)(ASI::AddrOf(0x178339)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x17833A)) = (int)(&post_init_modifications_hook) - ASI::AddrOf(0x00F55E); 
    ASI::EndRewrite(post_init_mreg);

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
            return FALSE;

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
            HookBetaVersion();
        }
        // THESE DO NOT WORK BECAUSE THEY'RE NOT UPDATED UNTIL AFTER USED
        /*ASI::BeginRewrite(mreg4);
        *(unsigned short*)(ASI::AddrOf(0x4359CD)) = MAX_BUILDING_INDEX;
        ASI::EndRewrite(mreg4);

        ASI::BeginRewrite(mreg5);
        *(unsigned short*)(ASI::AddrOf(0x1F9258)) = MAX_BUILDING_INDEX;
        ASI::EndRewrite(mreg5);*/

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
