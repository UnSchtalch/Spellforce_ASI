#pragma once

unsigned int __thiscall buildingIsSawmillHook(void *param_0, unsigned short param_l)
{
    unsigned char building_id = *(unsigned char *)((unsigned int)param_l * 0x34 + 10 + (unsigned int) param_0);
    if ((building_id != 0x5 ) && (building_id != 0x37) && (building_id != 0x36))
        return 0;
    return 1;
}

unsigned int __thiscall buildingIsHabitableHook(void *param_0, unsigned short param_l)
{
	unsigned char building_id = *(unsigned char *)((unsigned int)param_l * 0x34 + 10 + (unsigned int) param_0);
 	switch(building_id) {
 	case 4:
 	case 5:
 	case 6:
 	case 7:
 	case 8:
 	case 9:
 	case 10:
 	case 0xb:
 	case 0xc:
 	case 0xe:
 	case 0xf:
 	case 0x10:
 	case 0x11:
 	case 0x17:
 	case 0x18:
 	case 0x1a:
 	case 0x1b:
 	case 0x1c:
 	case 0x1d:
 	case 0x1f:
 	case 0x21:
 	case 0x22:
 	case 0x24:
 	case 0x2c:
 	case 0x2e:
 	case 0x2f:
 	case 0x30:
 	case 0x31:
 	case 0x33:
 	case 0x34:
 	case 0x35:
 	//ORC SAWMILL below
	case 0x36:
 	case 0x37:
 	case 0x3d:
 	case 0x3e:
 	case 0x3f:
 	case 0x40:
 	case 0x41:
 	case 0x42:
 	case 0x43:
 	case 0x44:
 	case 0x45:
 	case 0x46:
 	case 0x4b:
 	case 0x4d:
 	case 0x4e:
 	case 0x4f:
 	case 0x50:
 	case 0x52:
 	case 0x53:
 	case 0x58:
 	case 0x59:
 	case 0x5a:
 	case 0x5b:
 	case 0x5c:
 	case 0x5d:
 	case 0x5e:
 	case 0x5f:
 	case 0x60:
 	case 0x63:
 	case 0x64:
 	case 0x89:
 	  return 1;
 	default:
 	  return 0;
  }
}



unsigned int __thiscall buildingIsHabitableSingleHook(void *param_0, unsigned short param_l)
{
	unsigned char building_id = *(unsigned char *)((unsigned int)param_l * 0x34 + 10 + (unsigned int) param_0);
	switch (building_id){
	case 5:
	case 6:
	case 8:
	case 0xf:
	case 0x11:
	case 0x1a:
	case 0x22:
	case 0x24:
	case 0x2e:
	case 0x2f:
	case 0x30:
	case 0x34:
	case 0x35:
	//ORC SAWMILL below
	case 0x36:
	case 0x37:
	case 0x3e:
	case 0x3f:
	case 0x42:
	case 0x44:
	case 0x46:
	case 0x4b:
	case 0x4f:
	case 0x53:
	case 0x5a:
	case 0x5b:
	case 0x5d:
	case 0x5e:
	case 0x60:
	case 0x64:
	case 0x89:
    	return 1;
  	default:
		return 0;
  }
}
