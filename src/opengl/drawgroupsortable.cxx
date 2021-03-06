#include <OpenGL/Group.h>
#include <OpenGL/DrawGroup.h>
#include <OpenGL/Geometry.h>
#include <OpenGL/Material.h>
#include <OpenGL/Camera.h>

#include <algorithm>

DrawGroupSortable::DrawGroupSortable(Group *group, mat4 matrix)
    : DrawGroup(group, matrix)
{
    Reset();
    SortDefault();
}

void DrawGroupSortable::Reset()
{
    DrawGroup::Reset();
    group->Flatten(group);
}

void DrawGroupSortable::Draw()
{
    mat4 place = matrix * group->matrix;
    for (Group *group : group->flat)
        group->DrawSingle(place);
    DrawBounds();
}

template <typename F>
void DrawGroupSortable::Sort(F &f)
{
    group->Flatten(group);
    auto &flat = group->flat;
    std::sort(flat.begin(), flat.end(), f);
}

void DrawGroupSortable::SortDefault()
{
    Sort([](const Group *a, const Group *b) -> bool {
        if (a->geometry && b->geometry)
        {
            if (a->geometry->material->blending && !b->geometry->material->blending)
                return false;
            if (a->geometry->material->testing && !b->geometry->material->testing)
                return false;
            return true;
        }
        return false;
    });
}