#include <windows.h>
#include "asi/sf_asi.h"

#include "building_info.h"
#include "building_aux.h"

unsigned int JOB_LINK_FAIL;
unsigned int JOB_LINK_EXEC;
unsigned int CARPENTER_JOB_FAIL;
unsigned int CARPENTER_JOB_EXEC;

void __declspec(naked) start_working_at_building_hook_beta()
{
	asm("cmpb $0x35, %%al \n\t"
		"je 1f            \n\t"
		"cmpb $0x88, %%al \n\t"
		"jmp *%0          \n\t"
		"1: jmp *%1       \n\t":
		:"o"(JOB_LINK_FAIL), "o" (JOB_LINK_EXEC));
}

void __declspec(naked) carpenter_do_work_hook_beta()
{
	asm("movzbl 0x1a(%%ecx, %%edx), %%eax \n\t"
		"cmpb $0x4, %%al 			      \n\t"
		"je 1f 							  \n\t"
		"jmp *%0  						  \n\t"
		"1: jmp *%1 				 	  \n\t":
		:"o"(CARPENTER_JOB_FAIL), "o"(CARPENTER_JOB_EXEC));
}

void hookBetaVersion()
{
	ASI::MemoryRegion mreg_building_is_habitable (ASI::AddrOf(0x2C7600), 7); // BuildingIsHabitable
	ASI::MemoryRegion mreg_building_is_habitable_single (ASI::AddrOf(0x2C7530), 7); //BuildingIsHabitableSingle
	ASI::MemoryRegion mreg_building_is_sawmill (ASI::AddrOf(0x2EF82D), 5); //BuildingIsSawmil
	ASI::MemoryRegion mreg_start_working_at_building (ASI::AddrOf(0x2EDAAF), 5); //Worker start Working at building hook

	JOB_LINK_FAIL = ASI::AddrOf(0x2EDAB4);
	JOB_LINK_EXEC = ASI::AddrOf(0X2EDC69);
	CARPENTER_JOB_FAIL = ASI::AddrOf(0x2E6AD9);
	CARPENTER_JOB_EXEC = ASI::AddrOf(0x2E6AE6);


    ASI::MemoryRegion mreg11 (ASI::AddrOf(0x40F24E), 6); //CarpenterDoWork Hook
    ASI::MemoryRegion mreg12 (ASI::AddrOf(0x4187B4), 7); //Woodworker Delivery Hook



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
        //game_window = *(HWND*)(ASI::AddrOf(ASI::WINDOW_OFFSET));
        initBuildingData();
        if (ASI::CheckSFVersion(ASI::SF_BETA))
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