#pragma once


#include "SF_ASI.h"

namespace ASI
{
	inline int GetXData(ASI::Pointer xdata, unsigned int xdata_unk, unsigned char key)
	{
		return ASI::CallClassFunc<0x7A4E00, int, unsigned int, unsigned char>
			(xdata, xdata_unk, key);
	}

	inline void SetXData(ASI::Pointer xdata, unsigned int xdata_unk, unsigned char key, int value)
	{
		ASI::CallClassProc<0x7A4F00, unsigned int, unsigned char, unsigned int>
			(xdata, xdata_unk, key, value);
	}

	inline int SubXData(ASI::Pointer xdata, unsigned int xdata_unk, unsigned char key, int value)
	{
		return ASI::CallClassFunc<0x7A55E0, int, unsigned int, unsigned char, int>
			(xdata, xdata_unk, key, value);
	}

	inline int AddXData(ASI::Pointer xdata, unsigned int xdata_unk, unsigned char key, int value)
	{
		return ASI::CallClassFunc<0x7A5490, int, unsigned int, unsigned char, int>
			(xdata, xdata_unk, key, value);
	}

	inline bool XDataContains(ASI::Pointer xdata, unsigned int xdata_unk, unsigned char key)
	{
		return ASI::CallClassFunc<0x7A57A0, bool, unsigned int, unsigned char>
			(xdata, xdata_unk, key);
	}

	// this creates new entry if it doesnt exist AFAIK, adds value otherwise?
	inline void XDataListAdd(ASI::Pointer xdatalist, unsigned int effect_index, unsigned char key, int value)
	{
		ASI::CallClassProc<0x81A440, unsigned int, unsigned char, int>
			(xdatalist, effect_index, key, value);
	}

	// this creates new entry if it doesnt exist AFAIK, adds value otherwise? seems different from previous one
	inline void XDataListAdd2(ASI::Pointer xdatalist, unsigned int effect_index, unsigned char key, int value)
	{
		ASI::CallClassProc<0x7B8A50, unsigned int, unsigned char, int>
			(xdatalist, effect_index, key, value);
	}
}