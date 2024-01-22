#pragma once

typedef int (__thiscall *upgrade_activated_ptr)(unsigned int, void *, int);
typedef int (__thiscall *cancel_ugrade_ptr)(unsigned int, unsigned int, short);
typedef int (__thiscall *ugrade_started_learning_ptr)(unsigned int, unsigned int, short);

typedef unsigned int (__thiscall *unit_get_health_ptr)(unsigned int, unsigned short);
typedef unsigned int (__thiscall *unit_get_mana_ptr)(unsigned int, unsigned short);
typedef unsigned int (__thiscall *unit_get_agility_ptr)(unsigned int, unsigned short);
typedef unsigned int (__thiscall *unit_get_charisma_ptr)(unsigned int, unsigned short);
typedef unsigned int (__thiscall *unit_get_dexterity_ptr)(unsigned int, unsigned short);
typedef unsigned int (__thiscall *unit_get_intelligence_ptr)(unsigned int, unsigned short);
typedef unsigned int (__thiscall *unit_get_stamina_ptr)(unsigned int, unsigned short);
typedef unsigned int (__thiscall *unit_get_strength_ptr)(unsigned int, unsigned short);
typedef unsigned int (__thiscall *unit_get_wisdom_ptr)(unsigned int, unsigned short);
typedef unsigned int (__thiscall *unit_get_unknown_ptr)(unsigned int, unsigned short);

typedef unsigned int (__thiscall *unit_set_health_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_mana_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_agility_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_charisma_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_dexterity_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_intelligence_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_stamina_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_strength_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_wisdom_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_unknown_ptr)(unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_level_ptr)(unsigned int, unsigned short, unsigned int);

typedef struct unit_fun_ptr
{
		unit_get_health_ptr unit_get_health;
		unit_get_mana_ptr unit_get_mana;
		unit_get_agility_ptr unit_get_agility;
		unit_get_charisma_ptr unit_get_charisma;
		unit_get_dexterity_ptr unit_get_dexterity;
		unit_get_intelligence_ptr unit_get_intelligence;
		unit_get_stamina_ptr unit_get_stamina;
		unit_get_strength_ptr unit_get_strength;
		unit_get_wisdom_ptr unit_get_wisdom;
		unit_get_unknown_ptr unit_get_unknown;

		unit_set_health_ptr unit_set_health;
		unit_set_mana_ptr unit_set_mana;
		unit_set_agility_ptr unit_set_agility;
		unit_set_charisma_ptr unit_set_charisma;
		unit_set_dexterity_ptr unit_set_dexterity;
		unit_set_intelligence_ptr unit_set_intelligence;
		unit_set_stamina_ptr unit_set_stamina;
		unit_set_strength_ptr unit_set_strength;
		unit_set_wisdom_ptr unit_set_wisdom;
		unit_set_unknown_ptr unit_set_unknown;
		unit_set_level_ptr unit_set_level;
} unit_functions_t;


typedef struct upg_fun_ptr
{
	upgrade_activated_ptr upgrade_activated;
	cancel_ugrade_ptr cancel_ugrade;
	ugrade_started_learning_ptr ugrade_started_learning;
} upgrade_functions_t;