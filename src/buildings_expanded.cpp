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
unsigned int BUILD_DATA_RET;



void __stdcall processBuildingData(int** param_l)
{
	typedef void (__stdcall *lua_link_string_ptr)(int**, int**, char*, int, int);
	lua_link_string_ptr lua_link_string = (lua_link_string_ptr) ASI::AddrOf(0xDAB0);

	for (std::vector<ASI::BuildingHookData>::iterator it = buildingHoodDataList.begin();
        it != buildingHoodDataList.end(); ++it)
	{
		lua_link_string(param_l, (int**)0, (it->name), 0, (ASI::convertIntForLua(it->buildingID)) >> 32); //it works, do not touch	
	}
}

void __declspec(naked) building_register_hook(){
    unsigned int param_l = 0;
    asm("push 8(%%ebp)\n\t" //DO NOT CLEAN STACK HERE, it happens later in main code
        "pop %0\n\t": "=c"(param_l)
        :"c"(param_l) );

    processBuildingData((int **)param_l);

    asm("jmp *%0\n\t":
        :"m"(BUILD_DATA_RET));
    
}

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

void __declspec(naked) woodworker_select_delivery_hook_beta (){
    asm(
    "movzbl 0x1a(%%ecx,%%ebx), %%eax \n\t"
    "addl %%ebx, %%ecx 				 \n\t" //overwritten code from hook
    "cmpb $0x5, %%al 				 \n\t" //are we orcs?
    "jne 1f 						 \n\t" //if not -- jump away
    "mov $0x36, %%ebx 				 \n\t"//we are! looking for our building
    "jmp *%0					 	 \n\t"  //jump away to look for
    "1: jmp *%1 					 \n\t":
    : "o"(JOB_SAWMILL_SELECT_EXEC),"o"(JOB_SAWMILL_SELECT_FAIL)
    );
}

void hookBetaVersion()
{
	ASI::MemoryRegion mreg_building_is_habitable (ASI::AddrOf(0x2C7600), 7); // BuildingIsHabitable
	ASI::MemoryRegion mreg_building_is_habitable_single (ASI::AddrOf(0x2C7530), 7); //BuildingIsHabitableSingle
	ASI::MemoryRegion mreg_building_is_sawmill (ASI::AddrOf(0x2EF82D), 5); //BuildingIsSawmil
	ASI::MemoryRegion mreg_start_working_at_building (ASI::AddrOf(0x2EDAAF), 5); //Worker start Working at building hook
	ASI::MemoryRegion mreg_carpenter_do_work (ASI::AddrOf(0x2E6AD4), 5); //CarpenterDoWork Hook
    ASI::MemoryRegion mreg_woodworker_select_delivery (ASI::AddrOf(0x2F1F3B), 5); //Woodworker Delivery Hook
    ASI::MemoryRegion mreg_register_building_string(ASI::AddrOf(0x293ACF), 8);

	JOB_LINK_FAIL = ASI::AddrOf(0x2EDAB4);
	JOB_LINK_EXEC = ASI::AddrOf(0X2EDC69);
	CARPENTER_JOB_FAIL = ASI::AddrOf(0x2E6AD9);
	CARPENTER_JOB_EXEC = ASI::AddrOf(0x2E6AE6);
	JOB_SAWMILL_SELECT_FAIL = ASI::AddrOf(0x2F1F42);
	JOB_SAWMILL_SELECT_EXEC = ASI::AddrOf(0x2F1F56);
	BUILD_DATA_RET = ASI::AddrOf(0x293AEE);


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
        *(unsigned int*)(ASI::AddrOf(0x2F1F3C)) = (unsigned int)(&woodworker_select_delivery_hook_beta) - ASI::AddrOf(0x2F1F40);//jump distance
    ASI::EndRewrite(mreg_woodworker_select_delivery);


    ASI::BeginRewrite(mreg_register_building_string);
         *(unsigned char*)(ASI::AddrOf(0x293ACF)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x293AD0)) = (int)(&building_register_hook) - ASI::AddrOf(0x293AD4);
        *(unsigned char*)(ASI::AddrOf(0x293AD4)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x293AD5)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x293AD6)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg_register_building_string);

}



void initBuildingData()
{
    ASI::BuildingHookData bData;

    bData.buildingID = 50;
    bData.linkedUnitID = 2997;
    bData.name = "kGdBuildingDwarfHammerTower\0"; //fixme
    buildingHoodDataList.push_back(bData);

    bData.buildingID = 54;
    bData.linkedUnitID = 0;
    bData.name = "kGdBuildingOrcSawmill\0"; //fixme
    buildingHoodDataList.push_back(bData);

    bData.buildingID = 214;
    bData.linkedUnitID = 0;
    bData.name = "kGdMaxBuildingTypes\0";
    buildingHoodDataList.push_back(bData);
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