#pragma once


inline int ABS(int x) {
	return x > 0 ? x : -x;
}

inline int MAX(int x, int y)
{
	return x > y ? x : y;
}

inline int MIN(int x, int y)
{
	return x < y ? x : y;
}

inline int CLAMP(int v1, int v2, int t1, int t2, int t)
{
	return (t > t2 ? v2 : 
		   (t < t1 ? v1 : 
		   (v1 + (((((t - t1) * 100) / (t2 - t1)) * (v2 - v1)) / 100))));
}


namespace ASI
{
	#pragma pack(1)
	struct SF_PositionQuery
	{
		unsigned short x;
		unsigned short y;
		unsigned char other_data[4];

		inline SF_PositionQuery(void* ptr)
		{
			x = ((unsigned short*)(ptr))[0];
			y = ((unsigned short*)(ptr))[1];
			other_data[0] = ((unsigned char*)(ptr))[4];
			other_data[1] = ((unsigned char*)(ptr))[5];
			other_data[2] = ((unsigned char*)(ptr))[6];
			other_data[3] = ((unsigned char*)(ptr))[7];
		}

		inline SF_PositionQuery(unsigned int pos)
		{
			x = pos & 0xFFFF;
			y = pos >> 16;
			other_data[0] = 0;
			other_data[1] = 0;
			other_data[2] = 0;
			other_data[3] = 0;
		}

		inline SF_PositionQuery(unsigned short _x, unsigned short _y)
		{
			x = _x;
			y = _y;
			other_data[0] = 0;
			other_data[1] = 0;
			other_data[2] = 0;
			other_data[3] = 0;
		}
	};

	inline unsigned short FastDistance(SF_PositionQuery& p1, SF_PositionQuery& p2)
	{
		auto val8 = (p2.x) - (p1.x);    // x difference?
		auto val9 = ABS(val8);    // bigger of deltas

		auto val10 = (p2.y) - (p1.y);
		auto val11 = ABS(val10);  // smaller of deltas

		if (val11 > val9)
		{
			auto tmp_swap = val11;
			val11 = val9;
			val9 = tmp_swap;
		}

		return ((val11 * 13) / 32 + val9);
	}

	#pragma pack(1)
	struct SF_Unk1
	{
		unsigned char c1;
		unsigned short s1;
		unsigned short s2;
		unsigned short s3;

		SF_Unk1()
		{
			c1 = 0;
			s1 = 0;
			s2 = 0;
			s3 = 0;
		}

		SF_Unk1(unsigned char _c1, unsigned short _s1, unsigned short _s2, unsigned short _s3)
		{
			c1 = _c1;
			s1 = _s1;
			s2 = _s2;
			s3 = _s3;
		}

		SF_Unk1(void* ptr)
		{
			c1 = ((unsigned char*)ptr)[0];
			s1 = ((unsigned short*)((unsigned int)ptr + 1))[0];
			s2 = ((unsigned short*)((unsigned int)ptr + 1))[1];
			s3 = ((unsigned short*)((unsigned int)ptr + 1))[2];
		}
	};

	struct AreaEntityQuery
	{
		unsigned char data[68];


	};


	inline void SetArea(AreaEntityQuery& q, short x1, short y1, short x2, short y2)
	{
		ASI::CallClassProc<0x7A2120, short, short, short, short>
			(q.data, 0, 0, 1023, 1023);
	}		
	
	inline void BindEntityContainersToArea(AreaEntityQuery& q, ASI::Pointer p1, ASI::Pointer p2, ASI::Pointer p3)
	{
		ASI::CallClassProc<0x7A23E0, ASI::Pointer, ASI::Pointer, ASI::Pointer>
			(q.data, p1, p2, p3);
	}

	inline void FindAllEntitiesInArea(AreaEntityQuery& q, unsigned int& pos, int radius)
	{
		ASI::CallClassProc<0x7A19E0, unsigned int&, int>
			(q.data, pos, radius);  // area radius
	}

	inline unsigned short GetNextEntity(AreaEntityQuery& q)
	{
		return ASI::CallClassFunc<0x7A2250, unsigned short>
			(q.data);
	}
}