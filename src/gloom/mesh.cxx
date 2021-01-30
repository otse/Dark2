#include <libs>

#include <gloom/mesh.h>

#include <opengl/shader.h>
#include <opengl/texture.h>
#include <opengl/types.h>

namespace gloom
{
	Mesh::Mesh()
	{
		baseGroup = new Group();
		groups[-1] = baseGroup;
		lastGroup = baseGroup;
	}

	static void other(NifpRd *, void *);
	static void ni_node_callback						(NifpRd *, ni_node_pointer *);
	static void ni_node_callback_2						(NifpRd *, ni_node_pointer *);
	static void ni_tri_shape_callback					(NifpRd *, ni_tri_shape_pointer *);
	static void ni_tri_shape_callback_2					(NifpRd *, ni_tri_shape_pointer *);
	static void ni_tri_shape_data_callback				(NifpRd *, ni_tri_shape_data_pointer *);
	static void bs_lighting_shader_property_callback	(NifpRd *, bs_lighting_shader_property_pointer *);
	static void bs_effect_shader_property_callback		(NifpRd *, bs_effect_shader_property_pointer *);
	static void bs_shader_texture_set_callback			(NifpRd *, bs_shader_texture_set_pointer *);
	static void ni_alpha_property_callback				(NifpRd *, ni_alpha_property_pointer *);
	static void ni_skin_instance_callback				(NifpRd *, ni_skin_instance_pointer *);
	static void ni_skin_data_callback					(NifpRd *, ni_skin_data_pointer *);
	static void ni_skin_partition_callback				(NifpRd *, ni_skin_partition_pointer *);

	static std::map<void *, Mesh *> store;

	void Mesh::Store(void *key, Mesh *mesh)
	{
		store.emplace(key, mesh);
	}

	Mesh *Mesh::GetStored(void *key)
	{
		if (store.count(key))
			return store[key];
		return nullptr;
	}

	void Mesh::Construct(Nifp *bucket)
	{
		nif = bucket;
		NifpRd *rd = malloc_nifprd();
		rd->nif = bucket;
		rd->data = this;
		rd->other = other;
		rd->ni_node = ni_node_callback;
		rd->ni_tri_shape = ni_tri_shape_callback;
		rd->ni_tri_shape_data = ni_tri_shape_data_callback;
		rd->bs_lighting_shader_property = bs_lighting_shader_property_callback;
		rd->bs_effect_shader_property = bs_effect_shader_property_callback;
		rd->bs_shader_texture_set = bs_shader_texture_set_callback;
		rd->ni_alpha_property = ni_alpha_property_callback;
		nifp_rd(rd);
		free_nifprd(&rd);
		baseGroup->Update();
	}

	void SkinnedMesh::Construct()
	{
		cassert(skeleton, "smesh needs skeleton");
		// "Second pass rundown"
		NifpRd *rd = malloc_nifprd();
		rd->nif = mesh->nif;
		rd->data = this;
		rd->other = other;
		rd->ni_node = ni_node_callback_2;
		rd->ni_tri_shape = ni_tri_shape_callback_2;
		rd->ni_skin_instance = ni_skin_instance_callback;
		rd->ni_skin_data = ni_skin_data_callback;
		rd->ni_skin_partition = ni_skin_partition_callback;
		nifp_rd(rd);
		free_nifprd(&rd);
		Initial();
	}

	void SkinnedMesh::Initial()
	{
		for (ni_ref index : shapes)
		{
			//Group *group = mesh->groups[index];
			auto shape = 			(ni_tri_shape_pointer *)		nifp_get_block(mesh->nif, index);
			auto skin_instance = 	(ni_skin_instance_pointer *)	nifp_get_block(mesh->nif, shape->A->skin_instance);
			auto skin_partition = 	(ni_skin_partition_pointer *)	nifp_get_block(mesh->nif, skin_instance->A->skin_partition);
			for (unsigned int k = 0; k < *skin_partition->num_skin_partition_blocks; k++)
			{
				struct skin_partition *part = skin_partition->skin_partition_blocks[k];
				Group *group = mesh->groups[index]->groups[k];
				Material *material = group->geometry->material;
				material->boneMatrices.clear();
				material->boneMatrices.reserve(part->A->num_bones);
				material->bindMatrix = group->matrixWorld;
				for (int i = 0; i < part->A->num_bones; i++)
				{
					auto node = (ni_node_pointer *)nifp_get_block(mesh->nif, skin_instance->bones[part->bones[i]]);
					char *name = nifp_get_string(mesh->nif, node->common->A->name);
					auto has = skeleton->bones_named.find(name);
					if (has == skeleton->bones_named.end())
					{
						material->boneMatrices.push_back(mat4(1.0));
						continue;
					}
					Bone *bone = has->second;
					material->boneMatrices.push_back(bone->group->matrixWorld * inverse(bone->rest));
				}
			}
		}
	}

	void SkinnedMesh::Forward()
	{
		if (skeleton)
			skeleton->Step();
		Initial();
	}

	Group *Mesh::Nested(NifpRd *rd)
	{
		Group *group = new Group();
		groups[rd->current] = group;
		groups[rd->parent]->Add(group);
		lastGroup = group;
		return group;
	}

	void other(NifpRd *rd, void *block_pointer)
	{
		Mesh *mesh = (Mesh *)rd->data;
		//printf("NifpRd unhandled other block type %s\n", nifp_get_block_type(rd->nif, rd->current));
	}

	void matrix_from_common(Group *group, ni_common_layout_pointer *common)
	{
		group->matrix = translate(group->matrix, Gloom_Vec_3(common->C->translation));
		group->matrix *= mat4(Gloom_Mat_3(common->C->rotation));
		group->matrix = scale(group->matrix, vec3(common->C->scale));
	}

	void matrix_from_common_2(Group *group, ni_common_layout_pointer *common)
	{
		group->matrix = translate(group->matrix, Gloom_Vec_3(common->C->translation));
		group->matrix *= inverse(mat4(Gloom_Mat_3(common->C->rotation)));
		group->matrix = scale(group->matrix, vec3(common->C->scale));
	}

	void ni_node_callback(NifpRd *rd, ni_node_pointer *block)
	{
		// printf("ni node callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->Nested(rd);
		matrix_from_common(group, block->common);
	}

	void ni_tri_shape_callback(NifpRd *rd, ni_tri_shape_pointer *block)
	{
		// printf("ni tri shape callback %s\n", block->common.name_string);
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->Nested(rd);
		matrix_from_common_2(group, block->common);
		// testing here
		// group->matrix = glm::inverse(group->matrix);
		//if (block->A->skin_instance == -1)
		{
			group->geometry = new Geometry();
			group->geometry->material->src = &simple;
		}
	}

	void ni_node_callback_2(NifpRd *rd, ni_node_pointer *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		if (rd->current == 0)
			return;
		Group *group = smesh->mesh->groups[rd->current];
		//group->visible = false;
		//if (group->geometry)
		//	group->geometry->material->color = vec3(1);
	}

	void ni_tri_shape_callback_2(NifpRd *rd, ni_tri_shape_pointer *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		smesh->lastShape = smesh->mesh->groups[rd->current];
	}

	void ni_tri_shape_data_callback(NifpRd *rd, ni_tri_shape_data_pointer *block)
	{
		// printf("ni tri shape data callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Geometry *geometry = mesh->lastGroup->geometry;
		geometry->Clear(0, 0);
		if (!block->A->num_vertices)
			return;
		if (block->J->has_triangles)
		{
			geometry->Clear(block->A->num_vertices, block->J->num_triangles * 3);
			for (int i = 0; i < block->J->num_triangles; i++)
			{
				unsigned short *triangle = (unsigned short *)&block->triangles[i];
				geometry->elements.insert(geometry->elements.end(), {triangle[0], triangle[1], triangle[2]});
			}
		}
		for (int i = 0; i < block->A->num_vertices; i++)
		{
			geometry->vertices[i].position = Gloom_Vec_3(block->vertices[i]);
			if (block->C->bs_vector_flags & 0x00000001)
				geometry->vertices[i].uv = *cast_vec_2((float *)&block->uv_sets[i]);
			geometry->vertices[i].normal = Gloom_Vec_3(block->normals[i]);
			if (block->C->bs_vector_flags & 0x00001000)
			{
				geometry->material->tangents = true;
				geometry->vertices[i].tangent = Gloom_Vec_3(block->tangents[i]);
				geometry->vertices[i].bitangent = Gloom_Vec_3(block->bitangents[i]);
			}
			if (block->G->has_vertex_colors)
				geometry->vertices[i].color = Gloom_Vec_4(block->vertex_colors[i]);
		}
		geometry->SetupMesh();
	}

	void bs_lighting_shader_property_callback(NifpRd *rd, bs_lighting_shader_property_pointer *block)
	{
		// printf("bs lighting shader property callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Geometry *geometry = mesh->lastGroup->geometry;
		if (geometry)
		{
			geometry->material->color = vec3(1.0);
			geometry->material->emissive = Gloom_Vec_3(block->B->emissive_color);
			geometry->material->specular = Gloom_Vec_3(block->B->specular_color);
			geometry->material->specular *= block->B->specular_strength;
			geometry->material->opacity = block->B->alpha;
			geometry->material->glossiness = block->B->glossiness;
			if (block->B->shader_flags_1 & 0x00000002)
				geometry->material->skinning = true;
			else
				geometry->material->dust = true;
			if (block->B->shader_flags_1 & 0x00001000)
			{
				printf("Model_Space_Normals\n");
				geometry->material->modelSpaceNormals = true;
			}
			if (block->B->shader_flags_2 & 0x00000020)
				geometry->material->vertexColors = true;
		}
	}

	void bs_effect_shader_property_callback(NifpRd *rd, bs_effect_shader_property_pointer *block)
	{
		printf(" mesh bs effect shader cb ");
		Mesh *mesh = (Mesh *)rd->data;
		Geometry *geometry = mesh->lastGroup->geometry;
		if (geometry)
		{
			geometry->material->src = &basic;
			geometry->material->color = vec3(1.0);
			geometry->material->emissive = Gloom_Vec_3(block->C->emissive_color);
			geometry->material->map = GetProduceTexture(block->source_texture);
			printf("source texture is %s\n", block->source_texture);
		}
	}
	
	void bs_shader_texture_set_callback(NifpRd *rd, bs_shader_texture_set_pointer *block)
	{
		// printf("bs shader texture set callback\n");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->lastGroup;
		Geometry *geometry = group->geometry;
		if (geometry)
		{
			for (int i = 0; i < block->A->num_textures; i++)
			{
				std::string path = std::string(block->textures[i]);
				if (path.empty())
					continue;
				if (path.find("skyrimhd\\build\\pc\\data\\") != std::string::npos)
					path = path.substr(23, std::string::npos);
				if (i == 0)
					geometry->material->map = GetProduceTexture(block->textures[i]);
				if (i == 1)
					geometry->material->normalMap = GetProduceTexture(block->textures[i]);
				if (i == 2)
					geometry->material->glowMap = GetProduceTexture(block->textures[i]);
			}
		}
	}

	void ni_alpha_property_callback(NifpRd *rd, ni_alpha_property_pointer *block)
	{
		// printf("ni alpha property");
		Mesh *mesh = (Mesh *)rd->data;
		Group *group = mesh->lastGroup;
		Geometry *geometry = group->geometry;
		if (geometry)
		{
			geometry->material->treshold = block->C->treshold / 255.f;
		}
	}

	void ni_skin_instance_callback(NifpRd *rd, ni_skin_instance_pointer *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		Nifp *nif = smesh->mesh->nif;
		cassert(0 == strcmp(nifp_get_block_type(nif, rd->parent), NI_TRI_SHAPE), "root not shape");
		auto shape = (ni_tri_shape_pointer *)nifp_get_block(nif, rd->parent);
		smesh->shapes.push_back(rd->parent);
	}

	void ni_skin_data_callback(NifpRd *rd, ni_skin_data_pointer *block)
	{
		// used below
	}

	void ni_skin_partition_callback(NifpRd *rd, ni_skin_partition_pointer *block)
	{
		SkinnedMesh *smesh = (SkinnedMesh *)rd->data;
		auto nif = smesh->mesh->nif;
		auto shape = (ni_tri_shape_pointer *)nifp_get_block(nif, smesh->shapes.back());
		auto data = (ni_tri_shape_data_pointer *)nifp_get_block(nif, shape->A->data);
		for (unsigned int k = 0; k < *block->num_skin_partition_blocks; k++)
		{
			struct skin_partition *part = block->skin_partition_blocks[k];
			Group *group = new Group;
			Geometry *geometry = new Geometry();
			geometry->skinning = true;
			geometry->Clear(0, 0);
			if (!data->A->num_vertices)
				continue;
			if (part->A->num_triangles > 0)
			{
				geometry->Clear(part->A->num_vertices, part->A->num_triangles * 3);
				for (int i = 0; i < part->A->num_triangles; i++)
				{
					unsigned short *triangle = (unsigned short *)&part->triangles[i];
					geometry->elements.insert(geometry->elements.end(), {triangle[0], triangle[1], triangle[2]});
				}
			}
			for (int i = 0; i < part->A->num_vertices; i++)
			{
				if (!*part->has_vertex_map)
					break;
				unsigned short j = part->vertex_map[i];
				geometry->vertices[i].position = Gloom_Vec_3(data->vertices[j]);
				if (data->C->bs_vector_flags & 0x00000001)
					geometry->vertices[i].uv = *cast_vec_2((float *)&data->uv_sets[j]);
				if (data->C->has_normals)
					geometry->vertices[i].normal = Gloom_Vec_3(data->normals[j]);
				if (data->C->bs_vector_flags & 0x00001000)
				{
					smesh->lastShape->geometry->material->tangents = true;
					geometry->vertices[i].tangent = Gloom_Vec_3(data->tangents[j]);
					geometry->vertices[i].bitangent = Gloom_Vec_3(data->bitangents[j]);
				}
				if (data->G->has_vertex_colors)
					geometry->vertices[i].color = Gloom_Vec_4(data->vertex_colors[j]);
				if (*part->has_bone_indices)
				{
					auto a = part->bone_indices[i];
					geometry->vertices[i].skin_index = vec4(a.a, a.b, a.c, a.d);
				}
				if (*part->has_vertex_weights)
					geometry->vertices[i].skin_weight = Gloom_Vec_4(part->vertex_weights[i]);
			}
			//if (smesh->lastShape->geometry->material->tangents)
			//	printf("has tangents");
			geometry->material = new Material(*smesh->lastShape->geometry->material);
			geometry->material->bones = part->A->num_bones;
			//geometry->material->skinning = true;
			geometry->material->prepared = false;
			geometry->SetupMesh();
			group->geometry = geometry;
			smesh->lastShape->Add(group);
		}
		smesh->lastShape->Update();
	}

} // namespace gloom