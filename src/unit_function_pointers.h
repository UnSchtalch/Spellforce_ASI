#pragma once

typedef int (__thiscall *upgrade_activated_ptr)(void *, void *, int);
typedef int (__thiscall *cancel_ugrade_ptr)(void *, unsigned int, short);
typedef int (__thiscall *ugrade_started_learning_ptr)(void *, unsigned int, short);

typedef unsigned int (__thiscall *unit_get_health_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *unit_get_mana_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *unit_get_agility_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *unit_get_charisma_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *unit_get_dexterity_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *unit_get_intelligence_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *unit_get_stamina_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *unit_get_strength_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *unit_get_wisdom_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *unit_get_unknown_ptr)(void *, unsigned int);

typedef unsigned int (__thiscall *unit_set_health_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_mana_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_agility_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_charisma_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_dexterity_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_intelligence_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_stamina_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_strength_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_wisdom_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_unknown_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_set_level_ptr)(void *, unsigned int, unsigned int);


typedef void (__thiscall *init_unknown_stuff_f120_ptr)(void *);
typedef void (__stdcall *vector_constructor_iterator_ptr)(void *,unsigned int,unsigned int,void *(__thiscall*)(void *));
typedef unsigned int (__thiscall *get_unknown_field_23a0_ptr)(void *, unsigned int);
typedef unsigned int (__thiscall *get_unknown_data_f130_ptr) (void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_find_spawn_position_ptr)(void *, unsigned int, unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_get_data_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *unit_get_another_data_ptr)(void *, unsigned int, unsigned int);
typedef unsigned int (__thiscall *get_unknown_data_93d0_ptr) (void *, unsigned int);
typedef unsigned int (__thiscall *get_unknown_data_92b0_ptr) (void *, unsigned int);
typedef unsigned int (__thiscall *figure_add_ptr) (void *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
typedef unsigned int (__thiscall *figure_transform_ptr) (void *, unsigned int, unsigned int, unsigned int, unsigned int);


typedef struct support_fun_ptr
{
		init_unknown_stuff_f120_ptr	init_unknown_stuff_f120;
		vector_constructor_iterator_ptr	vector_constructor_iterator;
		get_unknown_field_23a0_ptr get_unknown_field_23a0;
		get_unknown_data_f130_ptr get_unknown_data_f130;
		unit_find_spawn_position_ptr unit_find_spawn_position;
		unit_get_data_ptr unit_get_data;
		unit_get_another_data_ptr unit_get_another_data;
		get_unknown_data_93d0_ptr get_unknown_data_93d0;
		get_unknown_data_92b0_ptr get_unknown_data_92b0;
		figure_add_ptr figure_add;
		figure_transform_ptr figure_transform;
} support_functions_t;

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