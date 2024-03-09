#include <windows.h>
#include "asi/sf_asi.h"
#include "asi/SF_SpellManager.h"
#include "asi/SF_SpellData.h"
#include "asi/SF_GdFigure.h"
#include "asi/SF_XData.h"
#include "asi/SF_Unknown.h"
#include "asi/SF_GdEffect.h"
#include "building_info.h"
#include "building_aux.h"

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
    ASI::CallGlobalCDeclFunc<0xc1aa0, void, int**, int**, int**, int, int>(p1, p2, p3, p4, p5);
}

inline void init_memory_obj(int * p1)
{

    if (ASI::CheckSFVersion(ASI::SF_154))
    {
        ASI::CallGlobalCDeclFunc<0xb7870, void, int *>(p1);
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
        ASI::CallGlobalCDeclFunc<0xb7c60, void, int *, int **>(p1, p2);
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
unsigned int BUILD_SAWMILL_CONT;


unsigned int JOB_SAWMILL_SELECT_RET;
unsigned int JOB_SAWMILL_SELECT_CONT;

unsigned int BUILD_IS_TOWER_RET;

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
                ASI::CallGlobalCDeclFunc<0xc1aa0, void, int**, int**, char*, int, int>(param_l, (int**)0, (it->name), 0, (ASI::convertIntForLua(it->buildingID)) >> 32); //it works, do not touch

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

void __declspec(naked) building_register_hook(){
    unsigned int param_l = 0;
    asm("push 8(%%ebp)\n\t" //DO NOT CLEAN STACK HERE, it happens later in main code
        "pop %0\n\t": "=c"(param_l)
        :"c"(param_l) );

    processBuildingData((int **)param_l);

    asm("jmp *%0\n\t":
        :"m"(BUILD_DATA_RET));
    
}


//CGdBuildingToolBox:Done 1.54 Version
//Had to keep'em separated
int link_buiding_data; //Dirty hack
void __declspec(naked) building_link_hook(){
asm(
    "cmpb $0x32, %%al\n\t" //our building ID for tower
    "je 2f\n\t"
    "cmpb $0x36, %%al\n\t" //check for sawmill
    "je 1f\n\t"
    "cmp $0xd1, %%ebp\n\t" //rewritten code
    "jmp *%0\n\t"
    "1: jmp *%2\n\t" //return code flow to case of sawmills
    "2: push $0x0000bb5 \n\t"
    "push %%edi\n\t"
    "mov %%esi, %%ecx\n\t"
    "call *%1\n\t"
    "pop %%edi\n\t"
    "pop %%esi\n\t"
    "pop %%ebp\n\t"
    "pop %%ebx\n\t"
    "add $0x18,%%esp\n\t"
    "ret $0x4\n\t":
    : "o"(BUILD_LINK_RET), "o"(link_buiding_data), "o"(BUILD_LINK_CONT));
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


void __declspec(naked) spell_type_link_hook_modern(){
    asm(
        "cmpl $0xf1, %%eax\n\t"
        "jne 1f\n\t"
        "movw $0x60, 0x5c(%%edi)\n\t"
        "jmp *%1\n\t"
        "1: cmpb $0xf0, %%al\n\t"
        "jmp *%0\n\t\n\t":
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

void __declspec(naked) buildingIsTowerHook()
{
    asm(
        "cmpb $0x32, %%bl\n\t"
        "je 1f\n\t"
        "cmp $0xc8, %%ecx\n\t"
        "jmp *%0\n\t"
        "1: movl $0x1, %%eax\n\t"
        "pop %%ebx\n\t"
        "ret $0x4\n\t":
        :"o" (BUILD_IS_TOWER_RET));
}

void __declspec(naked) buildingIsTowerHook_modern()
{
    asm(
        "cmpb $0x25, %%al\n\t"
        "je 1f\n\t"
        "cmp $0xc8, %%eax\n\t"
        "jmp *%0\n\t"
        "1: movl $0x1, %%edx\n\t"
        "mov %%edx, %%eax\n\t"
        "pop %%ebp\n\t"
        "ret $0x4\n\t":
        :"o" (BUILD_IS_TOWER_RET));
}




unsigned int JOB_LINK_RET;
unsigned int JOB_LINK_CONT;

unsigned int JOB_SAWMILL_WORK_RET;
unsigned int JOB_SAWMILL_WORK_CONT;


void __declspec(naked) job_link_hook(){
asm(
    "cmpb $0x35, %%al\n\t" //our building ID-1 (decreased instruction ago)
    "jne 1f\n\t" //if not our case, just continue
    "jmp *%0\n\t"//jump to procedures for workers
    "1: cmpb $0x88, %%al\n\t" //rewriten comparison
    "jmp *%1\n\t":
    : "o"(JOB_LINK_RET),"m"(JOB_LINK_CONT)
    );
}

void __declspec(naked) job_link_hook_legacy(){
asm(
    "cmpb $0x35, %%cl\n\t" //our building ID-1 (decreased instruction ago)
    "jne 1f\n\t" //if not our case, just continue
    "jmp *%0\n\t"//jump to procedures for workers
    "1: cmpb $0x88, %%cl\n\t" //rewriten comparison
    "jmp *%1\n\t":
    : "o"(JOB_LINK_RET),"m"(JOB_LINK_CONT)
    );
}

//CGdBuildingToolBox:Done Version 1.61
void __declspec(naked) buildingDoneHook(){
asm(
    "cmpb $0x32, %%al\n\t" //our building ID -4 (decreased instruction ago)
    "jne 1f\n\t" //if not our case, just continue
    "jmp *%0\n\t"//jump to XData Filling
    "1:cmpb $0x2E,%%al\n\t"
    "jne 2f\n\t"
    "push $0xbb5\n\t"
    "jmp *%2\n\t"
    "2: cmpb $0xd1, %%al\n\t" //rewriten comparison
    "jmp *%1\n\t":
    : "o"(BUILD_LINK_CONT),"o"(BUILD_LINK_RET), "o"(link_buiding_data));

}

void __declspec(naked) woodworkerDeliveryHook (){
    asm(
    "movzbl 0x1a(%%ecx,%%ebx), %%eax\n\t"
    "addl %%ebx, %%ecx\n\t" //overwritten code from hook
    "cmpb $0x5, %%al\n\t" //are we orcs?
    "jne 1f\n\t" //if not -- jump away
    "mov $0x36, %%ebx\n\t"//we are! looking for our building
    "jmp *%0\n\t"  //jump away to look for
    "1: jmp *%1\n\t":
    : "o"(JOB_SAWMILL_SELECT_RET),"o"(JOB_SAWMILL_SELECT_CONT)
    );
}

void __declspec(naked) woodworkerDeliveryHook_legacy (){
    asm(
    "movzbl 0x1a(%%ebx,%%edx), %%eax\n\t"
    "lea (%%ebx,%%edx), %%ecx\n\t" //overwritten code from hook
    "cmpb $0x5, %%al\n\t" //are we orcs?
    "jne 1f\n\t" //if not -- jump away
    "mov $0x36, %%ebp\n\t"//we are! looking for our building
    "jmp *%0\n\t"  //jump away to look for
    "1: jmp *%1\n\t":
    : "o"(JOB_SAWMILL_SELECT_RET),"o"(JOB_SAWMILL_SELECT_CONT)
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


void __declspec(naked) carpenterDoWorkHook_legacy(){
    asm(
    "movzbl %%dl, %%edx \n\t" //overwritten by our hook
    "cmpb $0x5, %%dl \n\t"
    "jne 1f\n\t"
    "jmp *%0\n\t"
    "1: jmp *%1\n\t":
    : "o" (JOB_SAWMILL_WORK_RET), "o" (JOB_SAWMILL_WORK_CONT));
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

void hookLegacyVersion()
{
    BUILD_DATA_RET = ASI::AddrOf(0x001bb33f);
    BUILD_INIT_RET = ASI::AddrOf(0x001d38a1);

    BUILD_LINK_RET = ASI::AddrOf(0x0041E02A); //just continue as usual
    BUILD_LINK_CONT = ASI::AddrOf (0x41E14D);
    link_buiding_data= ASI::AddrOf(0x41E9B0);

    OLD_MAX_BUILDING_VALUE = 0x406AC00000000000;
    NEW_MAX_BUILDING_VALUE = 0x406AE00000000000;
    HAMMER_THROW_ID = 0x406E400000000000;
    NEW_MAX_SPELL_LINE_VALUE = 0x406e600000000000;


    BUILD_IS_TOWER_RET = ASI::AddrOf(0x342af6);
    SPELL_DATA_RET = ASI::AddrOf(0x001bcfa6);
    SPELL_TYPE_LINK_RET_C = ASI::AddrOf(0x3B7647);
    SPELL_TYPE_LINK_RET = ASI::AddrOf(0x3B84B1);

    JOB_LINK_RET = ASI::AddrOf(0x40C8D5);
    JOB_LINK_CONT = ASI::AddrOf(0x40C5E3);

    JOB_SAWMILL_WORK_RET = ASI::AddrOf(0x40F261);
    JOB_SAWMILL_WORK_CONT = ASI::AddrOf(0x40F254);

    JOB_SAWMILL_SELECT_RET = ASI::AddrOf(0x4187CF);
    JOB_SAWMILL_SELECT_CONT = ASI::AddrOf(0x4187BB);

    ASI::MemoryRegion mreg (ASI::AddrOf(0x1bb322), 9);
    ASI::MemoryRegion mreg2 (ASI::AddrOf(0x342af0), 6); //IsTower cmp before ja
    ASI::MemoryRegion mreg3 (ASI::AddrOf(0x342F30), 9); // BuildingIsHabitable
    ASI::MemoryRegion mreg4 (ASI::AddrOf(0x1d389c), 5);
    ASI::MemoryRegion mreg5 (ASI::AddrOf(0x41E024), 6); //Building <-> unit link "default" statement start
    ASI::MemoryRegion mreg6 (ASI::AddrOf(0x3B7642), 5);
    ASI::MemoryRegion mreg7 (ASI::AddrOf(0x1bcf89), 9); //SpellLine definition for lua
    ASI::MemoryRegion mreg8 (ASI::AddrOf(0x342E60), 9); //BuildingIsHabitableSingle
    ASI::MemoryRegion mreg9 (ASI::AddrOf(0x40E378), 5); //Sawmill hook

    ASI::MemoryRegion mreg10 (ASI::AddrOf(0x40C5DD), 6); //Worker start Working at building hook
    ASI::MemoryRegion mreg11 (ASI::AddrOf(0x40F24E), 6); //CarpenterDoWork Hook
    ASI::MemoryRegion mreg12 (ASI::AddrOf(0x4187B4), 7); //Woodworker Delivery Hook

    ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x001bb322)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x001bb323)) = (int)(&building_register_hook) - ASI::AddrOf(0x001bb327);
        *(unsigned char*)(ASI::AddrOf(0x001bb327)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x001bb328)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x001bb329)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x001bb32a)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg);

    ASI::BeginRewrite(mreg2);
        *(unsigned char*)(ASI::AddrOf(0x342af0)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x342af1)) = (int)(&buildingIsTowerHook) - ASI::AddrOf(0x342af5); //jump distance should be calculated from the end of the instruction
        *(unsigned char*)(ASI::AddrOf(0x342af5)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg2);


        //Hack here: we hook the start of the function to transfer control to our implementation
        //Also NOP trail to avoid some nasty execution flow corruption
     ASI::BeginRewrite(mreg3);
        *(unsigned char*)(ASI::AddrOf(0x342F30)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x342F31)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x342F32)) = 0xE9;
        *(int*)(ASI::AddrOf(0x342F33)) = (unsigned int)(&buildingIsHabitableHook) - ASI::AddrOf(0x342F37);
        *(unsigned char*)(ASI::AddrOf(0x342F37)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x342F38)) = 0x90;   // nop instruction
     ASI::EndRewrite(mreg3);

    ASI::BeginRewrite(mreg8);
        *(unsigned char*)(ASI::AddrOf(0x342E60)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x342E61)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x342E62)) = 0xE9;
        *(int*)(ASI::AddrOf(0x342E63)) = (unsigned int)(&buildingIsHabitableSingleHook) - ASI::AddrOf(0x342E67);
        *(unsigned char*)(ASI::AddrOf(0x342E67)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x342E68)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg8);

    ASI::BeginRewrite(mreg4);
        *(unsigned char*)(ASI::AddrOf(0x1d389c)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x1d389d)) = (int)(&building_init_hook) - ASI::AddrOf(0x1d38a1);
    ASI::EndRewrite(mreg4);

    ASI::BeginRewrite(mreg5);
        *(unsigned char*)(ASI::AddrOf(0x41E024)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x41E025)) = (int)(&building_link_hook) - ASI::AddrOf(0x0041E029); //jump distance should be calculated from the end of the instruction
        *(unsigned char*)(ASI::AddrOf(0x41E029)) = 0x90;   // nop instruction
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

    ASI::BeginRewrite(mreg9);
        *(unsigned char*)(ASI::AddrOf(0x40E378)) = 0xE8;   // Near Call
        *(int*)(ASI::AddrOf(0x40E379)) = (unsigned int)(&buildingIsSawmillHook) - ASI::AddrOf(0x40E37D);//Distance to new function
    ASI::EndRewrite(mreg9);

    ASI::BeginRewrite(mreg10);
        *(unsigned char*)(ASI::AddrOf(0x40C5DD)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x40C5DE)) = (unsigned int)(&job_link_hook_legacy) - ASI::AddrOf(0x40C5E2);//jump distance
        *(unsigned char*)(ASI::AddrOf(0x40C5E2)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg10);


    ASI::BeginRewrite(mreg11);
        *(unsigned char*)(ASI::AddrOf(0x40F24E)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x40F24F)) = (unsigned int)(&carpenterDoWorkHook_legacy) - ASI::AddrOf(0x40F253);//jump distance
        *(unsigned char*)(ASI::AddrOf(0x40F253)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg11);


    ASI::BeginRewrite(mreg12);
        *(unsigned char*)(ASI::AddrOf(0x4187B4)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x4187B5)) = (unsigned int)(&woodworkerDeliveryHook_legacy) - ASI::AddrOf(0x4187B9);//jump distance
        *(unsigned char*)(ASI::AddrOf(0x4187B9)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x4187BA)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg12);


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
    //NOTE: IS NOT link_tower_unit Function, just variable re-use to jump for it's call
    link_buiding_data = ASI::AddrOf(0x331422);

    JOB_SAWMILL_WORK_RET = ASI::AddrOf(0x33F316);
    JOB_SAWMILL_WORK_CONT = ASI::AddrOf(0x33F309);

    SPELL_TYPE_LINK_RET = ASI::AddrOf(0x381ea8);
    SPELL_TYPE_LINK_RET_C = ASI::AddrOf(0x381578);

    JOB_SAWMILL_SELECT_RET = ASI::AddrOf(0x34A786); // If we are orcs and wanna look for sawmill
    JOB_SAWMILL_SELECT_CONT = ASI::AddrOf(0x34A772); //If we not orcs go here

    BUILD_IS_TOWER_RET = ASI::AddrOf(0x31f479);

    ASI::MemoryRegion mreg (ASI::AddrOf(0x2EC2F1), 8);//Register string for the building
    ASI::MemoryRegion mreg2 (ASI::AddrOf(0x381573), 5); //spell_manager_cast_spell hook
    ASI::MemoryRegion mreg3 (ASI::AddrOf(0x31f474), 5); //BuildingIsTower hook;
    ASI::MemoryRegion mreg4 (ASI::AddrOf(0x3462DF), 5); //job hook

    ASI::MemoryRegion mreg5 (ASI::AddrOf(0x2E7004), 5); //Init string for the building

    ASI::MemoryRegion mreg6 (ASI::AddrOf(0x3311FC), 5); // CGdBuildingToolBox::Done, i.e. linkBuildingWithStuff

    ASI::MemoryRegion mreg7 (ASI::AddrOf(0x31F8B0), 7); // BuildingIsHabitable

    ASI::MemoryRegion mreg8 (ASI::AddrOf(0x31F7E0), 7); // BuildingIsHabitableSingle

    ASI::MemoryRegion mreg9 (ASI::AddrOf(0x34805D), 5); //Building IsSawmill hook

    ASI::MemoryRegion mreg10 (ASI::AddrOf(0x34A76B), 5); //Sawmill selection hook

    ASI::MemoryRegion mreg11 (ASI::AddrOf(0x33F304), 5); //Sawmill worker hook

    ASI::BeginRewrite(mreg);
        *(unsigned char*)(ASI::AddrOf(0x2EC2F1)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2EC2F2)) = (int)(&building_register_hook) - ASI::AddrOf(0x2EC2F6);
        *(unsigned char*)(ASI::AddrOf(0x2EC2F6)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x2EC2F7)) = 0x90;   // nop instruction
        *(unsigned char*)(ASI::AddrOf(0x2EC2F8)) = 0x90;   // nop instruction
    ASI::EndRewrite(mreg);

    //Add mapping of hammer throw spell to spell line of arrow tower spell
    ASI::BeginRewrite(mreg2);
        *(unsigned char*)(ASI::AddrOf(0x381573)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x381574)) = (int)(&spell_type_link_hook_modern) - ASI::AddrOf(0x381578);
    ASI::EndRewrite(mreg2);

    ASI::BeginRewrite(mreg3);
        *(unsigned char*)(ASI::AddrOf(0x31f474)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x31f475)) = (int)(&buildingIsTowerHook_modern) - ASI::AddrOf(0x31f479);
    ASI::EndRewrite(mreg3);

    ASI::BeginRewrite(mreg5);
        *(unsigned char*)(ASI::AddrOf(0x2E7004)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x2E7005)) = (int)(&building_init_hook) - ASI::AddrOf(0x2E7009);
    ASI::EndRewrite(mreg5);

    ASI::BeginRewrite(mreg4);
        *(unsigned char*)(ASI::AddrOf(0x3462DF)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x3462E0)) = (unsigned int)(&job_link_hook) - ASI::AddrOf(0x3462E4);//jump distance
    ASI::EndRewrite(mreg4);

    
    ASI::BeginRewrite(mreg6);
        *(unsigned char*)(ASI::AddrOf(0x3311FC)) = 0xE9;   // jmp instruction
        *(int*)(ASI::AddrOf(0x3311FD)) = (unsigned int)(&buildingDoneHook) - ASI::AddrOf(0x331201);//jump distance
    ASI::EndRewrite(mreg6);

    //NOP trail + jump to our replacement function
    ASI::BeginRewrite(mreg7);
        *(unsigned char*)(ASI::AddrOf(0x31F8B0)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x31F8B1)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x31F8B2)) = 0xE9;
        *(int*)(ASI::AddrOf(0x31F8B3)) = (unsigned int)(&buildingIsHabitableHook) - ASI::AddrOf(0x31F8B7);
    ASI::EndRewrite(mreg7);

    //NOP trail + jump to our replacement function
    ASI::BeginRewrite(mreg8);
        *(unsigned char*)(ASI::AddrOf(0x31F7E0)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x31F7E1)) = 0x90;
        *(unsigned char*)(ASI::AddrOf(0x31F7E2)) = 0xE9;
        *(int*)(ASI::AddrOf(0x31F7E3)) = (unsigned int)(&buildingIsHabitableSingleHook) - ASI::AddrOf(0x31F7E7);
    ASI::EndRewrite(mreg8);

    //Outright replace function call
    ASI::BeginRewrite(mreg9);
        *(unsigned char*)(ASI::AddrOf(0x34805D)) = 0xE8;   // Near CALL
        *(int*)(ASI::AddrOf(0x34805E)) = (unsigned int)(&buildingIsSawmillHook) - ASI::AddrOf(0x348062);//jump distance
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
        if (!ASI::CheckSFVersion(ASI::SF_154) && !ASI::CheckSFVersion(ASI::SF_161))
        {
            return FALSE;
        }
        //game_window = *(HWND*)(ASI::AddrOf(ASI::WINDOW_OFFSET));
        initBuildingData();

        if (ASI::CheckSFVersion(ASI::SF_154))
        {
                hookLegacyVersion();
                break;
        }
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