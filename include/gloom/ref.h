#pragma once

#include <Gloom/Dark2.h>
#include <Gloom/Object.h>

#include <OpenGL/Types.h>

#include <OpenGL/Aabb.h>

namespace gloom
{
	class Ref
	{
	public:
		Ref(Record *);
		~Ref();

		const char *editorId = nullptr;

		Object self, baseObject;

		mat4 matrix;

		mat4 translation, rotation, scale;

		Mesh *mesh;
		DrawGroup *drawGroup;
		PointLight *pointLight;
		//SpotLight *spotLight;

		void Go();
		
		void forScale(float *);
		void forLocationalData(float *);
		void forBaseId(unsigned int);

		float GetDistance();
		bool DisplayAsItem();
	};
} // namespace gloom