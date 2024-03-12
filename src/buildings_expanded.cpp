#include <windows.h>
#include "asi/sf_asi.h"
#include <vector>

#include "building_info.h"
#include "building_aux.h"

std::vector<ASI::BuildingHookData> buildingHoodDataList;

unsigned int JOB_LINK_FAIL;
unsigned int JOB_LINK_EXEC;
unsigned int CARPENTER_JOB_FAIL;
unsigned int CARPENTER_JOB_EXEC;
unsigned int JOB_SAWMILL_SELECT_FAIL;
unsigned int JOB_SAWMILL_SELECT_EXEC;
unsigned int SPELL_TYPE_LINK_FAIL;
unsigned int SPELL_TYPE_LINK_EXEC;
unsigned int BUILDING_DONE_EXEC_T; //tower
unsigned int BUILDING_DONE_EXEC_S; //sawmill
unsigned int BUILDING_DONE_FAIL;

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
		"cmpb $0x5, %%al 			      \n\t"
		"je 1f 							  \n\t"
		"jmp *%0  						  \n\t"
		"1: jmp *%1 				 	  \n\t":
		:"o"(CARPENTER_JOB_FAIL), "o"(CARPENTER_JOB_EXEC));
}

void __declspec(naked) woodworker_select_delivery_hook_beta ()
{
    asm(
    "movzbl 0x1a(%%ecx,%%ebx), %%eax \n\t"
    "addl %%ebx, %%ecx 				 \n\t" //overwritten code from hook
    "cmpb $0x5, %%al 				 \n\t" //are we orcs?
    "jne 1f 						 \n\t" //if not -- jump away
    "mov $0x36, %%ebx 				 \n\t"//we are! looking for our building
    "jmp *%0					 	 \n\t"  //jump away to look for
    "1: jmp *%1 					 \n\t":
    : "o"(JOB_SAWMILL_SELECT_EXEC),"o"(JOB_SAWMILL_SELECT_FAIL));
}

//Hammer Throw mappint to arrow tower
void __declspec(naked) spell_type_link_hook_beta()
{
    asm(
        "cmpl $0xf1, %%eax 			\n\t"
        "jne 1f 					\n\t"
        "movw $0x60, 0x5c(%%edi) 	\n\t"
        "jmp *%1 					\n\t"
        "1: cmpb $0xf0, %%al 		\n\t"
        "jmp *%0 					\n\t":
        :"o" (SPELL_TYPE_LINK_FAIL), "o" (SPELL_TYPE_LINK_EXEC));
}


void __declspec(naked) building_done_hook_beta()
{
	asm(
	 	"movzbl 0xa(%%ecx,%%ebx), %%eax \n\t"
	 	"cmpb $0x32, %%al 				\n\t"
	 	"jne 1f 						\n\t"
	 	"push $0xBB5 					\n\t"
	 	"jmp *%0 						\n\t"
	 	"1: cmpb $0x36, %%al 			\n\t"
	 	"jne 2f 						\n\t"
	 	"jmp *%1 						\n\t"
	 	"2: jmp *%2 					\n\t":
		:"o"(BUILDING_DONE_EXEC_T), "o" (BUILDING_DONE_EXEC_S), "o"(BUILDING_DONE_FAIL));
}

void hookBetaVersion()
{
	ASI::MemoryRegion mreg_building_is_habitable (ASI::AddrOf(0x2C7600), 7); // BuildingIsHabitable
	ASI::MemoryRegion mreg_building_is_habitable_single (ASI::AddrOf(0x2C7530), 7); //BuildingIsHabitableSingle
	ASI::MemoryRegion mreg_building_is_sawmill (ASI::AddrOf(0x2EF82D), 5); //BuildingIsSawmil
	ASI::MemoryRegion mreg_building_is_tower (ASI::AddrOf(0x2C71B0), 7);
	
	ASI::MemoryRegion mreg_start_working_at_building (ASI::AddrOf(0x2EDAAF), 5); //Worker start Working at building hook
	ASI::MemoryRegion mreg_carpenter_do_work (ASI::AddrOf(0x2E6AD4), 5); //CarpenterDoWork Hook
    ASI::MemoryRegion mreg_woodworker_select_delivery (ASI::AddrOf(0x2F1F3B), 5); //Woodworker Delivery Hook
    ASI::MemoryRegion mreg_register_building_string(ASI::AddrOf(0x293ACF), 8);
    ASI::MemoryRegion mreg_building_done (ASI::AddrOf(0x2D8ED4), 5);
    ASI::MemoryRegion mreg_spell_manager_hook (ASI::AddrOf(0x328E43), 5);

	JOB_LINK_FAIL = ASI::AddrOf(0x2EDAB4);
	JOB_LINK_EXEC = ASI::AddrOf(0X2EDC69);
	CARPENTER_JOB_FAIL = ASI::AddrOf(0x2E6AD9);
	CARPENTER_JOB_EXEC = ASI::AddrOf(0x2E6AE6);
	JOB_SAWMILL_SELECT_FAIL = ASI::AddrOf(0x2F1F42);
	JOB_SAWMILL_SELECT_EXEC = ASI::AddrOf(0x2F1F56);
	SPELL_TYPE_LINK_FAIL = ASI::AddrOf(0x328E48);
	SPELL_TYPE_LINK_EXEC = ASI::AddrOf(0x329778);
	BUILDING_DONE_EXEC_T = ASI::AddrOf(0x2D9102);
	BUILDING_DONE_EXEC_S = ASI::AddrOf(0x2D8F58);
	BUILDING_DONE_FAIL = ASI::AddrOf(0x2D8ED9);


	//Overwrite function prologue with nop-trail and jump
	ASI::BeginRewrite(mreg_building_is_habitable);
		*(unsigned char*)(ASI::AddrOf(0x2C7600)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x2C7601)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x2C7602)) = 0xE9;
        *(unsigned int*)(ASI::AddrOf(0x2C7603)) = (unsigned int)(&buildingIsHabitableHook) - ASI::AddrOf(0x2C7607);
	ASI::EndRewrite(mreg_building_is_habitable);


	ASI::BeginRewrite(mreg_building_is_habitable_single);
		*(unsigned char*)(ASI::AddrOf(0x2C7530)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x2C7531)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x2C7532)) = 0xE9;
        *(unsigned int*)(ASI::AddrOf(0x2C7533)) = (unsigned int)(&buildingIsHabitableSingleHook) - ASI::AddrOf(0x2C7537);
	ASI::EndRewrite(mreg_building_is_habitable_single);

    //Outright replace function call
	ASI::BeginRewrite(mreg_building_is_sawmill);
        *(unsigned char*)(ASI::AddrOf(0x2EF82D)) = 0xE8;   // Near CALL
        *(unsigned int*)(ASI::AddrOf(0x2EF82E)) = (unsigned int)(&buildingIsSawmillHook) - ASI::AddrOf(0x2EF832);//jump distance
	ASI::EndRewrite(mreg_building_is_sawmill);


	ASI::BeginRewrite(mreg_building_is_tower);
		*(unsigned char*)(ASI::AddrOf(0x2C71B0)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x2C71B1)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x2C71B2)) = 0xE9;
        *(unsigned int*)(ASI::AddrOf(0x2C71B3)) = (unsigned int)(&buildingIsTowerHook) - ASI::AddrOf(0x2C71B7);
	ASI::EndRewrite(mreg_building_is_tower);


	ASI::BeginRewrite(mreg_start_working_at_building);
		*(unsigned char*)(ASI::AddrOf(0x2EDAAF)) = 0xE9;
		*(unsigned int*)(ASI::AddrOf(0x2EDAB0)) = (unsigned int)(&start_working_at_building_hook_beta) - ASI::AddrOf(0x2EDAB4);
	ASI::EndRewrite(mreg_start_working_at_building);


	ASI::BeginRewrite(mreg_carpenter_do_work);
		*(unsigned char*)(ASI::AddrOf(0x2E6AD4)) = 0xE9;
        *(unsigned int*)(ASI::AddrOf(0x2E6AD5)) = (unsigned int)(&carpenter_do_work_hook_beta) - ASI::AddrOf(0x2E6AD9);
	ASI::EndRewrite(mreg_carpenter_do_work);


	ASI::BeginRewrite(mreg_woodworker_select_delivery);
        *(unsigned char*)(ASI::AddrOf(0x2F1F3B)) = 0xE9;   // jmp instruction
        *(unsigned int*)(ASI::AddrOf(0x2F1F3C)) = (unsigned int)(&woodworker_select_delivery_hook_beta) - ASI::AddrOf(0x2F1F40);
    ASI::EndRewrite(mreg_woodworker_select_delivery);


    ASI::BeginRewrite(mreg_building_done);
    	*(unsigned char*)(ASI::AddrOf(0x2D8ED4)) = 0xE9;
        *(unsigned int*)(ASI::AddrOf(0x2D8ED5)) = (unsigned int)(&building_done_hook_beta) - ASI::AddrOf(0x2D8ED9);
    ASI::EndRewrite(mreg_building_done);

    ASI::BeginRewrite(mreg_spell_manager_hook);
        *(unsigned char*)(ASI::AddrOf(0x328E43)) = 0xE9;   // jmp instruction
        *(unsigned int*)(ASI::AddrOf(0x328E44)) = (unsigned int)(&spell_type_link_hook_beta) - ASI::AddrOf(0x328E48);
    ASI::EndRewrite(mreg_spell_manager_hook);

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