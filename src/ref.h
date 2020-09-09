#pragma once

#include "dark2.h"

#include "opengl/types"

namespace dark2
{
	struct REFR
	{
		char *EDID = nullptr;
		float *XSCL = nullptr;
		unsigned int *NAME = nullptr;
		unsigned char *DATA = nullptr;
		REFR(Record *);
	};

	class Ref
	{
	public:
		Ref();

		char *EDID = nullptr;
		float *DATA = nullptr;

		mat4 matrix;
		Mesh *mesh = nullptr;
		PointLight *pl = nullptr;

		void SetData(Record *);
	};
} // namespace dark2