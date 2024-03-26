#include <windows.h>
#include "asi/sf_asi.h"

unsigned int JOB_SELECT_EXEC;
unsigned int JOB_SELECT_FAIL;

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
    unsigned int (__thiscall *get_figure_xdata)(void *, unsigned int, unsigned int) = ASI::AddrOf(0x2FE3e8);
    unsigned int (__thiscall *prepare_building_mask)(void *, unsigned short, unsigned short, unsigned short, unsigned short) = ASI::AddrOf(0x318290);
    unsigned int (__thiscall *query_entities_in_radius)(void *,unsigned int *, unsigned short) = ASI::AddrOf(0x3195D0);
    unsigned int (__thiscall *init_building_mask)(void*, unsigned int, unsigned int, unsigned int) = ASI::AddrOf(0x31A640);
    unsigned int (__thiscall *specify_building)(void *, unsigned short) = ASI::AddrOf(0x31A6C0);
    unsigned int (__thiscall *get_next_building_from_mask)(void *) = ASI::AddrOf(0x318CA0);

    unsigned int figure_coords = * (unsigned int *)(*(void **)((int)_this + 0x24) + figure_id * 0x2b3 + 0x2);
    unsigned short figure_x = figure_coords & 0xffff;
    unsigned short figure_y = figure_coords >> 0x10;
    unsigned char mask[128];
    unsigned short building_id = 0;
    unsigned short last_norm = 0xFFFF;
    unsigned short best_id = 0;

    prepare_building_mask (&mask, 0,0, 0x3ff, 0x3ff);
    unsigned int local_c = *(unsigned int *)(*(void **)((int)_this + 0x10));
    init_building_mask(&mask, *(unsigned int *)((int)local_c + 0xc), *(unsigned int *)((int)local_c + 0x3c), *(unsigned int *)((int)local_c + 0x40));
    query_entities_in_radius(&mask, &figure_coords, 0x1f); //hardcode the radius
    specify_building(&mask, building_type);
    building_id = get_next_building_from_mask(&mask);
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
        building_id = get_next_building_from_mask(&mask);
    }
    return best_id;
}

unsigned short __attribute__((no_caller_saved_registers, thiscall)) worker_miner_find_hq(unsigned int *_this, unsigned int param_1, unsigned short figure_id)
{
    unsigned int local_c = *(unsigned int *)((unsigned int)_this + 0x20) + param_1;
    unsigned char race_id = *(unsigned char *)(local_c + 0x1a);
    if (race_id  == 0x03)
    {
        //void * (__thiscall *building_get_race)(void *, unsigned int) = ASI::AddrOf(0x2C6820);
    /*    unsigned short (__thiscall *get_closest_building_id)(void *, unsigned int, unsigned int, unsigned int, unsigned int) = ASI::AddrOf(0x2D97D0);
        unsigned short building_id = get_closest_building_id (*(void **)((int)_this + 0x10),
                                   *(unsigned int *)((unsigned int)*(unsigned short *)(local_c + 0x14) * 0x34 + 4 + *(int *)((int)_this + 0xc)), 0x14, 
                                   (unsigned int)*(unsigned short *)(local_c + 0x1c),0x19);*/
        return find_closest_building(_this, figure_id, 0x14);
    }
    return 0;
}


void __declspec(naked) worker_miner_hook_beta()
{
    asm(
        "mov %%esi, %%ecx       \n\t"
        "push %%edi             \n\t"
        "push -0x4(%%ebp)       \n\t" //iVar7
        "call %P0               \n\t"
        "movzw %%ax, %%ebx      \n\t"
        "test %%bx, %%bx        \n\t"
        "jne 1f                 \n\t"
        "mov 0x24(%%esi), %%ecx \n\t"
        "jmp *%1                \n\t"
        "1: jmp *%2             \n\t":
       :"i"(worker_miner_find_hq), "o"(JOB_SELECT_FAIL), "o"(JOB_SELECT_EXEC));
}

void hookBetaVersion()
{
    //FIXME!!!
	JOB_SELECT_EXEC = ASI::AddrOf(0x2EFF42);
	JOB_SELECT_FAIL = ASI::AddrOf(0x2F0009);
	ASI::MemoryRegion mreg(ASI::AddrOf(0x2F0004), 5); //building selector

	ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x2F0004)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2F0005)) = (int)(&worker_miner_hook_beta) - ASI::AddrOf(0x2F0009);
    ASI::EndRewrite(mreg);
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