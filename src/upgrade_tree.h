#pragma once
#include <vector>

struct upgrade_node
{
	std::vector<unsigned short> unit_base_id;
	std::vector<unsigned short> unit_upgrade_id;
	int prev = -1;
	int exclusive = -1;
};

struct upgrade_graph
{
public:
	std::vector<upgrade_node> nodes;

	void init(int node_count)
	{
		nodes.resize(node_count);
	}

	// adds a unit to upgrade at given upgrade
	bool add(int u, unsigned short base, unsigned short upgrade)
	{
		if (nodes.size() <= u)
			return false;

		nodes[u].unit_base_id.push_back(base);
		nodes[u].unit_upgrade_id.push_back(upgrade);

		return true;
	}

	// gets base unit at given upgrade
	// since one upgrade can upgrade many units, a specific index is required
	bool get_base_unit(int u, int i, unsigned short& base) const
	{
		if (nodes.size() <= u)
			return false;
		if (nodes[u].unit_base_id.size() <= i)
			return false;

		base = nodes[u].unit_base_id[i];
		return true;
	}

	// gets upgraded unit at given upgrade
	// since one upgrade can upgrade many units, a specific index is required
	bool get_upgrade_unit(int u, int i, unsigned short& upgrade) const
	{
		if (nodes.size() <= u)
			return false;
		if (nodes[u].unit_upgrade_id.size() <= i)
			return false;

		upgrade = nodes[u].unit_upgrade_id[i];
		return true;
	}

	// gets base and upgraded unit at given upgrade
	// since one upgrade can upgrade many units, a specific index is required
	bool get_unit(int u, int i, unsigned short& base, unsigned short& upgrade) const
	{
		if (nodes.size() <= u)
			return false;
		if (nodes[u].unit_upgrade_id.size() <= i)
			return false;

		base = nodes[u].unit_base_id[i];
		upgrade = nodes[u].unit_upgrade_id[i];
		return true;
	}

	// checks if a unit is base unit for given upgrade
	// if true, returns specific index
	bool is_unit_upgrade(int u, unsigned short base, int& i)
	{
		unsigned short _b;
		for (int j; get_base_unit(u, j, _b); j++)
		{
			if (_b == base)
			{
				i = j;
				return true;
			}
		}

		return false;
	}

	// links upgrade1 to upgrade2
	// makes it so nested upgrades are possible
	bool chain(int u1, int u2)
	{
		if (nodes.size() <= u2)
			return false;

		if (u1 >= u2)
			return false;

		if (nodes[u2].prev != -1)
			return false;

		nodes[u2].prev = u1;

		return true;
	}

	// excludes upgrade1 from upgrade2
	// makes it so choice upgrades are possible
	bool exclude(int u1, int u2)
	{
		if (nodes.size() <= u1)
			return false;
		if (nodes.size() <= u2)
			return false;

		if (nodes[u1].exclusive != -1)
			return false;
		if (nodes[u2].exclusive != -1)
			return false;

		nodes[u1].exclusive = u2;
		nodes[u2].exclusive = u1;

		return true;
	}

	bool prev_of(int u, int& result) const
	{
		if (nodes.size() <= u)
			return false;
		result = nodes[u].prev;
		return result != -1;
	}

	// returns an upgrade that excludes given upgrade
	// if theres no such upgrade, returns nothing
	bool get_exclusive(int u, int& result) const
	{
		if (nodes.size() <= u)
			return false;

		result = nodes[u].exclusive;
		return result != -1;
	}
};