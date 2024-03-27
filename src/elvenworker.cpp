#include <windows.h>
#include "asi/sf_asi.h"

unsigned int JOB_IRON_SELECT_EXEC;
unsigned int JOB_IRON_SELECT_FAIL;
unsigned int JOB_IRON_SELECT_FAIL_2;

unsigned int JOB_STONE_SELECT_EXEC;
unsigned int JOB_STONE_SELECT_FAIL;
unsigned int JOB_STONE_SELECT_FAIL_2;

unsigned int JOB_WOOD_SELECT_EXEC;
unsigned int JOB_WOOD_SELECT_FAIL;
unsigned int JOB_WOOD_SELECT_FAIL_2;

unsigned int (__thiscall *prepare_building_mask)(void *, unsigned short, unsigned short, unsigned short, unsigned short);
unsigned int (__thiscall *query_entities_in_radius)(void *,unsigned int *, unsigned short);
unsigned int (__thiscall *init_building_mask)(void *, unsigned int, unsigned int, unsigned int);
unsigned int (__thiscall *specify_building)(void *, unsigned short);
unsigned int (__thiscall *get_next_building_from_mask)(void *);

unsigned short calculate_norm(short x, short y)
{
    unsigned short m1 = x;
    unsigned short m2 = x;
    if (m1 < y) {m1 = y;}
    if (m2 > y) {m2 = y;}
    return (m1 - m2) + ((m2 * 0xd ) >> 0x5);
}

unsigned int __thiscall find_closest_building(unsigned int *_this, unsigned int figure_id, unsigned short building_type)
{
    unsigned int local_c = (unsigned int)(*(void **)((int)_this + 0x10));
    unsigned int figure_coords = * (unsigned int *)(*(void **)((int)_this + 0x20) + figure_id * 0x2b3 + 0x2);
    unsigned short figure_x = figure_coords & 0xffff;
    unsigned short figure_y = figure_coords >> 0x10;
    unsigned char mask[0x52];
    unsigned short building_id = 0;
    unsigned short last_norm = 0xFFFF;
    unsigned short best_id = 0;
    prepare_building_mask (mask, 0,0, 0x3ff, 0x3ff);
    init_building_mask(mask, *(unsigned int *)((int)local_c + 0xc), *(unsigned int *)((int)local_c + 0x3c), *(unsigned int *)((int)local_c + 0x40));
    query_entities_in_radius(mask, &figure_coords, 0x1f); //hardcode the radius
    specify_building(mask, building_type);
    building_id = get_next_building_from_mask(mask);
    while (building_id != 0)
    {

        unsigned int building_coordinates = *(unsigned int *)(((building_id & 0xffff) * 0x34) + *(unsigned int *)((int)_this + 0xc) + 0x4);
        short dx = figure_x - (building_coordinates & 0xffff);
        short dy = figure_y - (building_coordinates >> 0x10);
        if (dx < 0) { dx = 0 - dx;}
        if (dy < 0) { dy = 0 - dy;}
        if (calculate_norm(dx, dy) < last_norm)
        {
            last_norm = calculate_norm(dx, dy);
            best_id = building_id;
        }
        building_id = get_next_building_from_mask(&mask[0]);
    }
    return best_id;
}

unsigned short __attribute__((no_caller_saved_registers, thiscall)) worker_miner_find_hq(unsigned int *_this, unsigned int param_1, unsigned short figure_id)
{
    unsigned int local_c = *(unsigned int *)((unsigned int)_this + 0x20) + param_1;
    unsigned char race_id = *(unsigned char *)(local_c + 0x1a);
    if (race_id == 0x01)
    {
        return find_closest_building(_this, figure_id, 0x02);
    }
    if (race_id == 0x02)
    {
        return find_closest_building(_this, figure_id, 0x29);
    }
    if (race_id  == 0x03)
    {
        return find_closest_building(_this, figure_id, 0x14);
    }
    if (race_id == 0x04)
    {
        return find_closest_building(_this, figure_id, 0x49);
    }
    if (race_id == 0x05)
    {
        return find_closest_building(_this, figure_id, 0x3A);
    }
    if (race_id == 0x06)
    {
        return find_closest_building(_this, figure_id, 0x55);
    }

    return 0;
}


void __declspec(naked) worker_miner_hook_beta()
{
    asm(
        "cmpb $0x2, %%al        \n\t"
        "jne 2f                 \n\t"
        "mov %%esi, %%ecx       \n\t"
        "push %%edi             \n\t"
        "push -0x4(%%ebp)       \n\t" //iVar7
        "call %P0               \n\t"
        "movzw %%ax, %%ebx      \n\t"
        "test %%bx, %%bx        \n\t"
        "jne 1f                 \n\t"
        "jmp *%1                \n\t"
        "1: jmp *%2             \n\t"
        "2: jmp *%3             \n\t":
       :"i"(worker_miner_find_hq), "o"(JOB_IRON_SELECT_FAIL), "o"(JOB_IRON_SELECT_EXEC), "o"(JOB_IRON_SELECT_FAIL_2));
}

void __declspec(naked) worker_stoneworker_hook_beta()
{
    asm(
        "cmpb $0x2, %%al        \n\t"
        "jne 2f                 \n\t"
        "mov %%esi, %%ecx       \n\t"
        "push %%edi             \n\t"
        "push -0x4(%%ebp)       \n\t" //iVar7
        "call %P0               \n\t"
        "movzw %%ax, %%ebx      \n\t"
        "test %%bx, %%bx        \n\t"
        "jne 1f                 \n\t"
        "jmp *%1                \n\t"
        "1: jmp *%2             \n\t"
        "2: jmp *%3             \n\t":
       :"i"(worker_miner_find_hq), "o"(JOB_STONE_SELECT_FAIL), "o"(JOB_STONE_SELECT_EXEC), "o"(JOB_STONE_SELECT_FAIL_2));
}

void __declspec(naked) worker_woodcutter_hook_beta()
{
    asm(
        "cmpb $0x2, %%al        \n\t"
        "jne 2f                 \n\t"
        "mov %%esi, %%ecx       \n\t"
        "push %%edi             \n\t"
        "push -0x4(%%ebp)       \n\t" //iVar7
        "call %P0               \n\t"
        "test %%ax, %%ax        \n\t"
        "jne 1f                 \n\t"
        "mov 0x20(%%esi), %%ecx \n\t"
        "jmp *%1                \n\t"
        "1: movzw %%ax, %%ebx   \n\t"
        "jmp *%2                \n\t"
        "2: jmp *%3             \n\t":
       :"i"(worker_miner_find_hq), "o"(JOB_WOOD_SELECT_FAIL), "o"(JOB_WOOD_SELECT_EXEC), "o"(JOB_WOOD_SELECT_FAIL_2));
}

void hookBetaVersion()
{
    prepare_building_mask = ASI::AddrOf(0x318290);
    query_entities_in_radius = ASI::AddrOf(0x3195D0);
    init_building_mask = ASI::AddrOf(0x31A640);
    specify_building = ASI::AddrOf(0x31A6C0);
    get_next_building_from_mask = ASI::AddrOf(0x318CA0);

    //FIXME!!!
	JOB_IRON_SELECT_EXEC = ASI::AddrOf(0x2EFF42);
	JOB_IRON_SELECT_FAIL = ASI::AddrOf(0x2F0004);
    JOB_IRON_SELECT_FAIL_2 = ASI::AddrOf(0x2EFEB1);
	ASI::MemoryRegion iron_mreg(ASI::AddrOf(0x2EFEA9), 8); //building selector

    JOB_WOOD_SELECT_EXEC = ASI::AddrOf(0x2F1F74);
    JOB_WOOD_SELECT_FAIL = ASI::AddrOf(0x2F2041);
    JOB_WOOD_SELECT_FAIL_2 = ASI::AddrOf(0x2F1F3B);
    ASI::MemoryRegion wood_mreg(ASI::AddrOf(0x2F1F33), 8); //building selector

    JOB_STONE_SELECT_EXEC = ASI::AddrOf(0x2F0D8A);
    JOB_STONE_SELECT_FAIL = ASI::AddrOf(0x2F0DE6);
    JOB_STONE_SELECT_FAIL_2 = ASI::AddrOf(0x2F0CE1);
    ASI::MemoryRegion stone_mreg(ASI::AddrOf(0x2F0CD9), 8); //building selector

	ASI::BeginRewrite(iron_mreg);
        *(unsigned char*)(ASI::AddrOf(0x2EFEA9)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2EFEAA)) = (int)(&worker_miner_hook_beta) - ASI::AddrOf(0x2EFEAE);
        *(unsigned char*)(ASI::AddrOf(0x2EFEAE)) = 0x90;   // nop trail
        *(unsigned char*)(ASI::AddrOf(0x2EFEAF)) = 0x90;   // nop trail
        *(unsigned char*)(ASI::AddrOf(0x2EFEB0)) = 0x90;   // nop trail
    ASI::EndRewrite(iron_mreg);

    ASI::BeginRewrite(wood_mreg);
         *(unsigned char*)(ASI::AddrOf(0x2F1F33)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2F1F34)) = (int)(&worker_woodcutter_hook_beta) - ASI::AddrOf(0x2F1F38);
        *(unsigned char*)(ASI::AddrOf(0x2F1F38)) = 0x90;   // nop trail
        *(unsigned char*)(ASI::AddrOf(0x2F1F39)) = 0x90;   // nop trail
        *(unsigned char*)(ASI::AddrOf(0x2F1F3A)) = 0x90;   // nop trail
    ASI::EndRewrite(wood_mreg);

    ASI::BeginRewrite(stone_mreg);
         *(unsigned char*)(ASI::AddrOf(0x2F0CD9)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2F0CDA)) = (int)(&worker_stoneworker_hook_beta) - ASI::AddrOf(0x2F0CDE);
        *(unsigned char*)(ASI::AddrOf(0x2F0CDE)) = 0x90;   // nop trail
        *(unsigned char*)(ASI::AddrOf(0x2F0CDF)) = 0x90;   // nop trail
        *(unsigned char*)(ASI::AddrOf(0x2F0CE0)) = 0x90;   // nop trail
    ASI::EndRewrite(stone_mreg);
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
        //!ASI::CheckSFVersion(ASI::SF_154) &&
        if (!ASI::CheckSFVersion(ASI::SF_BETA))
        {
            return FALSE;
        }
        else 
        {
            hookBetaVersion();
            break;
        }
     break;
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}