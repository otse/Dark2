#ifndef GLOOM_SKELETON_H
#define GLOOM_SKELETON_H

#include <gloom/dark2.h>

#include <opengl/types.h>
#include <opengl/group.h>

namespace gloom
{
	class Bone;

	class Skeleton
	{
	public:
		Skeleton();

		std::map<int, Bone *> bones;

		nifp *nif;
		Bone *baseBone, *lastBone;

		Bone *Nested(nifprd *);
		void Load(const char *);
		void Construct();
	};

	class Bone
	{
	public:
		Group *group;
		//std::vector<Bone *> bones;
		Bone()
		{
			group = new Group;
		};
	};

} // namespace gloom

#endif