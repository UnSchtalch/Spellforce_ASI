#include <windows.h>
#include "asi/sf_asi.h"
#include "asi/SF_SpellManager.h"
#include "asi/SF_SpellData.h"
#include "asi/SF_GdFigure.h"
#include "asi/SF_XData.h"
#include "asi/SF_Unknown.h"
#include "asi/SF_GdEffect.h"
#include "building_info.h"

#include <vector>
//3D1000 is base for 1.61
//
static const char * new_building_string = "kGdBuildingOrcSawmill\0";
static const char * new_spell_line_string = "kGdSpellLineHammerThrow\0";
/*
    TODO
    rewrite at absolute 5bb325:
    0x9E myhook_address 0x90

*/
HWND game_window;


inline void resourceLoadProb(int ** p1, int** p2, int** p3, int p4, int p5)
{
    ASI::CallGlobalCDeclFunc<0x4c1aa0, void, int**, int**, int**, int, int>(p1, p2, p3, p4, p5);
}

inline void init_memory_obj(int * p1)
{

    if (ASI::CheckSFVersion(ASI::SF_154))
    {
        ASI::CallGlobalCDeclFunc<0x4b7870, void, int *>(p1);
    }
    if (ASI::CheckSFVersion(ASI::SF_161))
    {
        ASI::CallGlobalCDeclFunc<0x3D3870, void, int *>(p1);
    }
}

inline void init_memory_string (int* p1, int ** p2)
{
    if (ASI::CheckSFVersion(ASI::SF_154))
    {
        ASI::CallGlobalCDeclFunc<0x4b7c60, void, int *, int **>(p1, p2);
    }
    if (ASI::CheckSFVersion(ASI::SF_161))
    {
        ASI::CallGlobalCDeclFunc<0x3D3C60, void, int *, int **>(p1, p2);
    }
}

/*
inline void link_buiding_data(ASI::Pointer caller,  int param,  int id)
{
    ASI::CallClassFunc<0x81E9B0, void,  int, int>(caller, param, id);
}
*/

unsigned int BUILD_DATA_RET;
unsigned long long int OLD_MAX_BUILDING_VALUE;
unsigned long long int NEW_MAX_BUILDING_VALUE;
unsigned int BUILD_INIT_RET;
unsigned int BUILD_LINK_RET;
unsigned int BUILD_LINK_CONT;
unsigned int SPELL_TYPE_LINK_RET;
unsigned int SPELL_TYPE_LINK_RET_C;
unsigned long long int HAMMER_THROW_ID;
unsigned long long int NEW_MAX_SPELL_LINE_VALUE;
unsigned int SPELL_DATA_RET;


unsigned int BUILD_HAB_RET;
unsigned int BUILD_HAB_CONT;

unsigned int BUILD_HAB_S_RET;
unsigned int BUILD_HAB_S_CONT;

unsigned int BUILD_SAWMILL_RET;


unsigned int JOB_SAWMILL_SELECT_RET;
unsigned int JOB_SAWMILL_SELECT_CONT;

std::vector<ASI::BuildingHookData> buildingHoodDataList;


void __declspec(naked) building_init_hook(){
    asm(
        "pop %%ecx\n\t"
        "pop %%ecx\n\t"
        "push 8(%%ebp)\n\t"
        "call %0\n\t"
        "pop %%ecx\n\t"
        "push %1 \n\t"
        "push 8(%%ebp)\n\t"
        "call %2\n\t"
        "pop %%ecx\n\t"
        "pop %%ecx\n\t"
        "push 8(%%ebp)\n\t"
        "jmp *%3\n\t" :
        : "o" (init_memory_obj), "m"(new_building_string), "o"(init_memory_string), "m"(BUILD_INIT_RET)
        );
}

void __stdcall processBuildingData(int** param_l)
{
        if (ASI::CheckSFVersion(ASI::SF_154))
        {
            for (std::vector<ASI::BuildingHookData>::iterator it = buildingHoodDataList.begin();
                it != buildingHoodDataList.end(); ++it)
            {
                ASI::CallGlobalCDeclFunc<0x4c1aa0, void, int**, int**, char*, int, int>(param_l, (int**)0, (it->name), 0, (ASI::convertIntForLua(it->buildingID)) >> 32); //it works, do not touch

            }
        }
        if (ASI::CheckSFVersion(ASI::SF_161))
        {
            for (std::vector<ASI::BuildingHookData>::iterator it = buildingHoodDataList.begin();
                it != buildingHoodDataList.end(); ++it)
            {
                ASI::CallGlobalCDeclFunc<0xDAB0, void, int**, int**, char*, int, int>(param_l, (int**)0, (it->name), 0, (ASI::convertIntForLua(it->buildingID)) >> 32); //it works, do not touch
                asm("movl $0x0, (%esp)");
            }
        }
   
}


//Old code for history and debug puposes:
    /*
asm(

    "add $0x14, %%esp\n\t"
    "fldl (%0)\n\t"
    "push %%ecx\n\t"
    "push %%ecx\n\t"
    "fstpl (%%esp)\n\t"
    "push %3 \n\t"
    "push $0x0\n\t"
    "push 8(%%ebp)\n\t"
    "call %2\n\t"
    
    "add  $0x14, %%esp\n\t"
    "fldl (%1)\n\t"
    "push %%ecx\n\t"
    "push %%ecx\n\t"
    "fstpl (%%esp)\n\t"
    "push $0x00d19638\n\t"
    "push $0x0\n\t"
    "push 8(%%ebp)\n\t"
    "call %2\n\t"
    "jmp *%4\n\t" :
    : "m"(OLD_MAX_BUILDING_VALUE), "m"(NEW_MAX_BUILDING_VALUE), "o"(resourceLoadProb), "m"(new_building_string) ,"m"(BUILD_DATA_RET)
    );
    */

void __declspec(naked) building_register_hook(){
    unsigned int param_l = 0;
    asm("push 8(%%ebp)\n\t" //DO NOT CLEAN STACK HERE, it happens later in main code
        "pop %0\n\t": "=c"(param_l)
        :"c"(param_l) );

    processBuildingData(param_l);

    asm("jmp *%0\n\t":
        :"m"(BUILD_DATA_RET));
    
}

int link_buiding_data = 0x81E9B0; //Dirty hack
void __declspec(naked) building_link_hook(){
asm(
    "cmpb $0xd6, %%al\n\t" //our building ID
    "jne 1f\n\t"
    "push $0x0000bb9 \n\t" //341 -- human tower unit, bb9 -- our tower unit
    "push %%edi\n\t"
    "mov %%esi, %%ecx\n\t"
    "call *%0\n\t"
    "1: pop %%edi\n\t"//normal "default"
    "pop %%esi\n\t" 
    "pop %%ebp\n\t"
    "pop %%ebx\n\t"
    "add $0x18, %%esp\n\t"
    "jmp *%1\n\t":
    : "o"(link_buiding_data),"m"(BUILD_LINK_RET)
    );
}


void __declspec(naked) spell_type_link_hook(){
    asm(
        "AND $0xffff, %%eax\n\t"
        "cmpl $0xf2, %%eax\n\t"
        "jne 1f\n\t"
        "movw $0x60, 0x5c(%%ebx)\n\t"
        "jmp *%1\n\t"
        "1: jmp *%0\n\t\n\t":
        :"o" (SPELL_TYPE_LINK_RET_C), "o" (SPELL_TYPE_LINK_RET));
}


void __declspec(naked) spell_type_register_hook(){
asm(
    "add $0x14, %%esp\n\t"
    "fldl (%0)\n\t"
    "push %%ecx\n\t"
    "push %%ecx\n\t"
    "fstpl (%%esp)\n\t"
    "push %1 \n\t"
    "push $0x0\n\t"
    "push 8(%%ebp)\n\t"
    "call %2\n\t"

    "add  $0x14, %%esp\n\t"
    "fldl (%3)\n\t"
    "push %%ecx\n\t"
    "push %%ecx\n\t"
    "fstpl (%%esp)\n\t"
    "push $0x00d17d58\n\t"
    "push $0x0\n\t"
    "push 8(%%ebp)\n\t"
    "call %2\n\t"
    "jmp *%4\n\t" :
    :"m"(HAMMER_THROW_ID), "o"(new_spell_line_string), "o"(resourceLoadProb), "m"(NEW_MAX_SPELL_LINE_VALUE), "o"(SPELL_DATA_RET));
}


void initBuildingData()
{
    ASI::BuildingHookData bData;

    bData.buildingID = 214;
    bData.linkedUnitID = 3001;
    bData.name = "kGdBuildingOrcSawmill\0";
    buildingHoodDataList.push_back(bData);

    bData.buildingID = 215;
    bData.linkedUnitID = 0;
    bData.name = "kGdMaxBuildingTypes\0";
    buildingHoodDataList.push_back(bData);
}

void hookLegacyVersion()
{
        BUILD_DATA_RET = ASI::AddrOf(0x001bb33f);
        BUILD_INIT_RET = ASI::AddrOf(0x001d38a1);

        BUILD_LINK_RET = ASI::AddrOf(0x0041E69D); //Unit Linkage RET statement for "default" case

        OLD_MAX_BUILDING_VALUE = 0x406AC00000000000;
        NEW_MAX_BUILDING_VALUE = 0x406AE00000000000;
        HAMMER_THROW_ID = 0x406E400000000000;
        NEW_MAX_SPELL_LINE_VALUE = 0x406e600000000000;

        SPELL_DATA_RET = ASI::AddrOf(0x001bcfa6);
        SPELL_TYPE_LINK_RET_C = ASI::AddrOf(0x3B7647);
        SPELL_TYPE_LINK_RET = ASI::AddrOf(0x3B84B1);

        ASI::MemoryRegion mreg(ASI::AddrOf(0x001bb322), 9);

        ASI::MemoryRegion mreg2(ASI::AddrOf(0x1f9258),1); //Building amount 1st entry
        ASI::MemoryRegion mreg3(ASI::AddrOf(0x4359cd),1); //Building amount 2nd entry

        ASI::MemoryRegion mreg4(ASI::AddrOf(0x1d389c), 5);

        ASI::MemoryRegion mreg5(ASI::AddrOf(0x41E696), 7); //Building <-> unit link "default" statement start

        ASI::MemoryRegion mreg6(ASI::AddrOf(0x3B7642), 5);


        ASI::MemoryRegion mreg7(ASI::AddrOf(0x001bcf89), 9); //SpellLine definition for lua

        ASI::BeginRewrite(mreg);
                *(unsigned char*)(ASI::AddrOf(0x001bb322)) = 0xE9;   // jmp instruction
                *(int*)(ASI::AddrOf(0x001bb323)) = (int)(&building_register_hook) - ASI::AddrOf(0x001bb327);
                *(unsigned char*)(ASI::AddrOf(0x001bb327)) = 0x90;   // nop instruction
                *(unsigned char*)(ASI::AddrOf(0x001bb328)) = 0x90;   // nop instruction
                *(unsigned char*)(ASI::AddrOf(0x001bb329)) = 0x90;   // nop instruction
                *(unsigned char*)(ASI::AddrOf(0x001bb32a)) = 0x90;   // nop instruction
        ASI::EndRewrite(mreg);

        ASI::BeginRewrite(mreg4);
            *(unsigned char*)(ASI::AddrOf(0x1d389c)) = 0xE9;   // jmp instruction
            *(int*)(ASI::AddrOf(0x1d389d)) = (int)(&building_init_hook) - ASI::AddrOf(0x1d38a1);
        ASI::EndRewrite(mreg4);

        ASI::BeginRewrite(mreg2);
            *(unsigned char*)(ASI::AddrOf(0x1f9258)) = 0xD7; // D6 -> D7
        ASI::EndRewrite(mreg2);

        ASI::BeginRewrite(mreg3);
            *(unsigned char*)(ASI::AddrOf(0x4359cd)) = 0xD7; // D6 -> D7
        ASI::EndRewrite(mreg3);

        ASI::BeginRewrite(mreg5);
            *(unsigned char*)(ASI::AddrOf(0x0041E696)) = 0xE9;   // jmp instruction
            *(int*)(ASI::AddrOf(0x0041E697)) = (int)(&building_link_hook) - ASI::AddrOf(0x0041E69B);
            *(unsigned char*)(ASI::AddrOf(0x0041E69B)) = 0x90;   // nop instruction
            *(unsigned char*)(ASI::AddrOf(0x0041E69C)) = 0x90;   // nop instruction
        ASI::EndRewrite(mreg5);      

        ASI::BeginRewrite(mreg6);
            *(unsigned char*)(ASI::AddrOf(0x3B7642)) = 0xE9;   // jmp instruction
            *(int*)(ASI::AddrOf(0x3B7643)) = (int)(&spell_type_link_hook) - ASI::AddrOf(0x3B7647);
        ASI::EndRewrite(mreg6);    


        ASI::BeginRewrite(mreg7);
                *(unsigned char*)(ASI::AddrOf(0x001bcf89)) = 0xE9;   // jmp instruction
                *(int*)(ASI::AddrOf(0x001bcf8a)) = (int)(&spell_type_register_hook) - ASI::AddrOf(0x001bcf8e);
                *(unsigned char*)(ASI::AddrOf(0x001bcf8e)) = 0x90;   // nop instruction
                *(unsigned char*)(ASI::AddrOf(0x001bcf8f)) = 0x90;   // nop instruction
                *(unsigned char*)(ASI::AddrOf(0x001bcf90)) = 0x90;   // nop instruction
                *(unsigned char*)(ASI::AddrOf(0x001bcf91)) = 0x90;   // nop instruction
        ASI::EndRewrite(mreg7);  
}


unsigned int JOB_LINK_RET;
unsigned int JOB_LINK_CONT;

unsigned int JOB_SAWMILL_WORK_RET;
unsigned int JOB_SAWMILL_WORK_CONT;


void __declspec(naked) job_link_hook(){
asm(
    "cmpb $0xd5, %%al\n\t" //our building ID-1 (IDC WHY)
    "jne 1f\n\t" //if not our case, just continue
    "jmp *%0\n\t"//jump to procedures for workers
    "1: cmpb $0x88, %%al\n\t" //rewriten comparison
    "jmp *%1\n\t":
    : "o"(JOB_LINK_RET),"m"(JOB_LINK_CONT)
    );
}


void __declspec(naked) buildingDoneHook(){
asm(
    "cmpb $0xd2, %%al\n\t" //our building ID -4 (???)
    "jne 1f\n\t" //if not our case, just continue
    "jmp *%0\n\t"//jump to XData Filling
    "1: cmpb $0xd1, %%al\n\t" //rewriten comparison
    "jmp *%1\n\t":
    : "o"(BUILD_LINK_CONT),"m"(BUILD_LINK_RET)

    );

}


void __declspec(naked) buildingHabitableHook(){
asm(
    "cmpb $0xd2, %%al\n\t" //our building ID -4 
    "jne 1f\n\t" //if not our case, just continue
    "jmp *%0\n\t"//jump to procedures for workers
    "1: cmpb $0x84, %%al\n\t" //rewriten comparison
    "jmp *%1\n\t":
    : "o"(BUILD_HAB_RET),"m"(BUILD_HAB_CONT)
    );

}


void __declspec(naked) buildingHabitableSingleHook(){
asm(
    "cmpb $0xd6, %%al\n\t" //our building ID
    "jne 1f\n\t" //if not our case, just continue
    "jmp *%0\n\t"//jump to procedures for workers
    "1: cmpb $0x84, %%al\n\t" //rewriten comparison
    "jmp *%1\n\t":
    : "o"(BUILD_HAB_S_RET),"m"(BUILD_HAB_S_CONT)
    );
}


void __declspec(naked) buildingSawmillHook(){
asm(
    "cmpb $0xd6, %%cl\n\t" //our building ID
    "jne 1f\n\t" //if not our case, just continue
    "movb $0x1, %%al\n\t"
    "pop %%ebp\n\t"
    "ret $0x4\n\t"
    "1: cmpb $0x5, %%cl\n\t" //rewriten comparison
    "jne 2f\n\t"
    "movb $0x1, %%al\n\t"
    "pop %%ebp\n\t"
    "ret $0x4\n\t"
    "2: jmp *%0\n\t":
    : "o"(BUILD_SAWMILL_RET)
    );
}

void __declspec(naked) woodworkerDeliveryHook (){
    asm(
    "movzbl 0x1a(%%ecx,%%ebx), %%eax\n\t"
    "addl %%ebx, %%ecx\n\t" //overwritten code from hook
    "cmpb $0x5, %%al\n\t" //are we orcs?
    "jne 1f\n\t" //if not -- jump away
    "movl $0xd6, %%ebx\n\t"//we are! looking for our building
    "jmp *%0\n\t"  //jump away to look for
    "1: jmp *%1\n\t":
    : "o"(JOB_SAWMILL_SELECT_RET),"m"(JOB_SAWMILL_SELECT_CONT)
    );
}

void __declspec(naked) carpenterDoWorkHook(){
    asm(
    "movzbl 0x1a(%%ecx,%%edx), %%eax \n\t" //overwritten by our hook
    "cmpb $0x5, %%al \n\t"
    "jne 1f\n\t"
    "jmp *%0\n\t"
    "1: jmp *%1\n\t":
    : "o" (JOB_SAWMILL_WORK_RET), "o" (JOB_SAWMILL_WORK_CONT));
}

void hookModernVersion()
{

    //FIXME - Region Size
    JOB_LINK_RET = ASI::AddrOf(0x346499);
    JOB_LINK_CONT = ASI::AddrOf(0x3462E4);
    BUILD_DATA_RET = ASI::AddrOf(0x2EC310);

    BUILD_INIT_RET = ASI::AddrOf(0x2E9310);

    BUILD_LINK_RET = ASI::AddrOf(0x331201);;
    BUILD_LINK_CONT = ASI::AddrOf(0x331278);

    BUILD_HAB_RET = ASI::AddrOf(0x31F8D7);
    BUILD_HAB_CONT = ASI::AddrOf(0x31F8C7);

    BUILD_HAB_S_RET =  ASI::AddrOf(0x31F807);
    BUILD_HAB_S_CONT = ASI::AddrOf(0x31F7F7);

    BUILD_SAWMILL_RET = ASI::AddrOf(0x31F2F3);
    //BUILD_SAWMILL_CONT = ASI::AddrOf(0x31F2F3);

    JOB_SAWMILL_WORK_RET = ASI::AddrOf(0x33F316);
    JOB_SAWMILL_WORK_CONT = ASI::AddrOf(0x33F309);


    JOB_SAWMILL_SELECT_RET = ASI::AddrOf(0x34A786); // If we are orcs and wanna look for sawmill
    JOB_SAWMILL_SELECT_CONT = ASI::AddrOf(0x34A772); //If we not orcs go here


    ASI::MemoryRegion mreg(ASI::AddrOf(0x2EC2F1), 8);//Register string for the building
    ASI::MemoryRegion mreg2(ASI::AddrOf(0x2C2E9C),1); //CGdResource::LoadBuildings
    ASI::MemoryRegion mreg3(ASI::AddrOf(0x333B1C),1); //CGdDataLoader::LoadBuildings
    
    ASI::MemoryRegion mreg4(ASI::AddrOf(0x3462DF),5); //job hook

    ASI::MemoryRegion mreg5(ASI::AddrOf(0x2E7004),5); //Init string for the building

    ASI::MemoryRegion mreg6 (ASI::AddrOf(0x3311FC),5); // CGdBuildingToolBox::Done, i.e. linkBuildingWithStuff

    ASI::MemoryRegion mreg7 (ASI::AddrOf(0x31F8C2),5); // BuildingIsHabitable

    ASI::MemoryRegion mreg8 (ASI::AddrOf(0x31F7F2),5); // BuildingIsHabitableSingle

    ASI::MemoryRegion mreg9 (ASI::AddrOf(0x31f2EE), 5); //Sawmill hook

    ASI::MemoryRegion mreg10 (ASI::AddrOf(0x34A76B), 5); //Sawmill selection hook

    ASI::MemoryRegion mreg11 (ASI::AddrOf(0x33F304), 5); //Sawmill worker hook

    ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x2EC2F1)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2EC2F2)) = (int)(&building_register_hook) - ASI::AddrOf(0x2EC2F6);
        *(unsigned char*)(ASI::AddrOf(0x2EC2F6)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x2EC2F7)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x2EC2F8)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg);

    ASI::BeginRewrite(mreg5);
        *(unsigned char*)(ASI::AddrOf(0x2E7004)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2E7005)) = (int)(&building_init_hook) - ASI::AddrOf(0x2E7009);
    ASI::EndRewrite(mreg5);


    ASI::BeginRewrite(mreg2);
        *(unsigned char*)(ASI::AddrOf(0x2C2E9C)) = 0xD7; // D6 -> D7
    ASI::EndRewrite(mreg2);
    
    ASI::BeginRewrite(mreg3);
        *(unsigned char*)(ASI::AddrOf(0x333B1C)) = 0xD7; // D6 -> D7
    ASI::EndRewrite(mreg3);

    ASI::BeginRewrite(mreg4);
        *(unsigned char*)(ASI::AddrOf(0x3462DF)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x3462E0)) = (unsigned int)(&job_link_hook) - ASI::AddrOf(0x3462E4);//jump distance
    ASI::EndRewrite(mreg4);

    
    ASI::BeginRewrite(mreg6);
        *(unsigned char*)(ASI::AddrOf(0x3311FC)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x3311FD)) = (unsigned int)(&buildingDoneHook) - ASI::AddrOf(0x331201);//jump distance
    ASI::EndRewrite(mreg6);

    ASI::BeginRewrite(mreg7);
        *(unsigned char*)(ASI::AddrOf(0x31F8C2)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x31F8C3)) = (unsigned int)(&buildingHabitableHook) - ASI::AddrOf(0x31F8C7);//jump distance
    ASI::EndRewrite(mreg7);


    ASI::BeginRewrite(mreg8);
        *(unsigned char*)(ASI::AddrOf(0x31F7F2)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x31F7F3)) = (unsigned int)(&buildingHabitableSingleHook) - ASI::AddrOf(0x31F7F7);//jump distance
    ASI::EndRewrite(mreg8);

    ASI::BeginRewrite(mreg9);
        *(unsigned char*)(ASI::AddrOf(0x31F2EE)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x31F2EF)) = (unsigned int)(&buildingSawmillHook) - ASI::AddrOf(0x31F2F3);//jump distance
    ASI::EndRewrite(mreg9);


    ASI::BeginRewrite(mreg10);
        *(unsigned char*)(ASI::AddrOf(0x34A76B)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x34A76C)) = (unsigned int)(&woodworkerDeliveryHook) - ASI::AddrOf(0x34A770);//jump distance
    ASI::EndRewrite(mreg10);


    ASI::BeginRewrite(mreg11);
        *(unsigned char*)(ASI::AddrOf(0x33F304)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x33F305)) = (unsigned int)(&carpenterDoWorkHook) - ASI::AddrOf(0x33F309);//jump distance
    ASI::EndRewrite(mreg11);
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
        if (!ASI::CheckSFVersion(ASI::SF_161))
        {
            return FALSE;
        }
        //game_window = *(HWND*)(ASI::AddrOf(ASI::WINDOW_OFFSET));
        initBuildingData();

        /*if (ASI::CheckSFVersion(ASI::SF_154))
        {
                hookLegacyVersion();
                break;
        }*/
        if (ASI::CheckSFVersion(ASI::SF_161))
        {
                hookModernVersion();
                break;
        }
     break;
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}