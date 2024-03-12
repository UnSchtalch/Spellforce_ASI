#include <windows.h>
#include "asi/sf_asi.h"

unsigned int JOB_SELECT_EXEC;
unsigned int JOB_SELECT_FAIL;


unsigned short __attribute__((no_caller_saved_registers, thiscall)) worker_miner_find_hq(unsigned int *_this, unsigned int param_1)
{
    unsigned int local_c = *(unsigned int *)((unsigned int)_this + 0x20) + param_1;
    unsigned char race_id = *(unsigned char *)(local_c + 0x1a);
    if (race_id  == 0x03)
    {
        //void * (__thiscall *building_get_race)(void *, unsigned int) = ASI::AddrOf(0x2C6820);
        unsigned short (__thiscall *get_closest_building_id)(void *, unsigned int, unsigned int, unsigned int, unsigned int) = ASI::AddrOf(0x2D97D0);
        unsigned short building_id = get_closest_building_id (*(void **)((int)_this + 0x10),
                                   *(unsigned int *)((unsigned int)*(unsigned short *)(local_c + 0x14) * 0x34 + 4 + *(int *)((int)_this + 0xc)), 0x14, 
                                   (unsigned int)*(unsigned short *)(local_c + 0x1c),0x19);
        return building_id;
    }
    return 0;
}


void __declspec(naked) worker_miner_hook_beta()
{
    asm(
        "mov %%esi, %%ecx       \n\t"
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