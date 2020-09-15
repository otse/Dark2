#include "putc.h"

#include "nifp.h"

#define Hedr   nif->hdr
#define Blocks nif->blocks

api void nifp_print_hedr(struct nifp *nif, char *s)
{
	snprintf(
		s, 600,
		"\
header string: %s\
\nversion: %s\
\nendian type: %u\
\nuser value: %u\
\nnum blocks: %u\
\nuser value 2: %u\
\nauthor: %s\
\nprocess script: %s\
\nexport script: %s\
\nnum block types: %hu\
\nblock types\
\nblock type index\
\nblock sizes\
\nnum strings: %u\
\nmax string length: %u\
\nstrings\
\nnum groups: %u\
\ngroups\
",
Hedr->header_string,
Hedr->version,
Hedr->endian_type,
Hedr->user_value,
Hedr->num_blocks,
Hedr->user_value_2,
Hedr->author,
Hedr->process_script,
Hedr->export_script,
Hedr->num_block_types,
Hedr->num_strings,
Hedr->max_string_length,
Hedr->num_groups
);
}

static char *print_vec_2p(char *s, struct vec_2p v) {
	snprintf(s, 200, "[%f, %f]", v.x, v.y);
	return s;
}

static char *print_vec_3p(char *s, struct vec_3p v) {
	snprintf(s, 200, "[%f, %f, %f]", v.x, v.y, v.z);
	return s;
}

static char *print_vec_4p(char *s, struct vec_4p v) {
	snprintf(s, 200, "[%f, %f, %f, %f]", v.x, v.y, v.z, v.w);
	return s;
}

static char *print_ushort_3p(char *s, struct ushort_3p v) {
	snprintf(s, 200, "[%hu, %hu, %hu]", v.x, v.y, v.z);
	return s;
}

static char *print_mat_3p(char *s, struct mat_3p v) {
#define V ((float*)&v)
	snprintf(
		s, 200,
		"\
\n   [%f, %f, %f] \
\n   [%f, %f, %f] \
\n   [%f, %f, %f]",
V[0], V[1], V[2],
V[3], V[4], V[5],
V[6], V[7], V[8]);
	return s;
}

static char *print_mat_4p(char *s, float *v) {
#define V ((float*)&v)
	snprintf(
		s, 200,
		"\
\n   [%f, %f, %f, %f] \
\n   [%f, %f, %f, %f] \
\n   [%f, %f, %f, %f] \
\n   [%f, %f, %f, %f]",
V[0], V[1], V[2], V[3],
V[4], V[5], V[6], V[7],
V[8], V[9], V[10], V[11],
V[12], V[13], V[14], V[15]);
	return s;
}

static char *print_ni_common_layout_pointer(struct nifp *nif, char s[600], struct ni_common_layout_pointer *block_pointer)
{
	char x[200], y[200];
	snprintf(
		s, 600,
		"\
name: %s [%i]\
\nnum_extra_data_list: %u\
\nextra_data_list\
\ncontroller: %i\
\nflags: %u\
\ntranslation: %s\
\nrotation: %s\
\nscale: %f\
\ncollision_object: %i\
",
nifp_get_string(nif, block_pointer->A->name),
block_pointer->A->name,
block_pointer->A->num_extra_data_list,
block_pointer->C->controller,
block_pointer->C->flags,
print_vec_3(x, block_pointer->C->translation),
print_mat_3(y, block_pointer->C->rotation),
block_pointer->C->scale,
block_pointer->C->collision_object);
	return s;
}

static void print_ni_node_pointer(struct nifp *nif, int n, char s[1000])
{
	char x[600];
	struct ni_node_pointer *block_pointer = Blocks[n];
	snprintf(
		s, 1000,
		"\
%s\
\nni node: \
\nnum_children: %u\
\nchildren\
\nnum_effects: %u\
",
print_ni_common_layout_pointer(nif, x, block_pointer->common),
block_pointer->A->num_children,
block_pointer->C->num_effects);
}

static void print_ni_tri_shape_pointer(struct nifp *nif, int n, char s[1000])
{
	char x[600];
	struct ni_tri_shape_pointer *block_pointer = Blocks[n];
	snprintf(
		s, 1000,
		"\
%s\
\nni tri shape: \
\ndata: %i\
\nskin_instance: %i\
\nmaterial data\
\nshader_property: %i\
\nalpha_property: %i\
",
print_ni_common_layout_pointer(nif, x, block_pointer->common),
block_pointer->A->data,
block_pointer->A->skin_instance,
block_pointer->B->shader_property,
block_pointer->B->alpha_property);
}

static void print_ni_tri_shape_data_pointer(struct nifp *nif, int n, char s[1000])
{
	char a[200], b[200], c[200], d[200], e[200], f[200], g[200], h[200], i[200], j[200], k[200], l[200], m[200], o[200], p[200];
	struct ni_tri_shape_data_pointer *block_pointer = Blocks[n];
	snprintf(
		s, 1000,
		"\
group_id: %i\
\nnum_vertices: %hu\
\nkeep_flags: %u\
\ncompress_flags: %u\
\nhas_vertices: %i\
\nvertices\
\nvertices 1: %s \
\nvertices %i: %s \
\nbs_vector_flags\
\nmaterial crc: %i\
\nhas_normals: %i\
\nnormals\
\nnormals 1: %s \
\nnormals %i: %s \
\ntangents\
\ntangents 1: %s \
\ntangents %i: %s \
\nbitangents\
\nbitangents 1: %s\
\nbitangents %i: %s\
\ncenter: %s\
\nradius: %f\
\nhas_vertex_colors: %i\
\nvertex_colors\
\nvertex_colors 1: %s\
\nvertex_colors: %i: %s\
\nuv_sets\
\nuv_sets 1: %s\
\nuv_sets %i: %s\
\nconsistency_flags: %hu\
\nadditional_data: %i\
\nnum_triangles: %hu\
\nnum_triangle_points: %hu\
\nhas_triangles: %i\
\ntriangles\
\ntriangles 1: %s\
\ntriangles %i: %s\
\nnum_match_groups: %hu\
\nmatch_groups\
\nmatch_group: %i\
",
block_pointer->A->group_id,
block_pointer->A->num_vertices,
block_pointer->A->keep_flags,
block_pointer->A->compress_flags,
block_pointer->A->has_vertices,
print_vec_3(a, block_pointer->vertices[0]),
block_pointer->A->num_vertices,
print_vec_3(b, block_pointer->vertices[block_pointer->A->num_vertices-1]),
block_pointer->C->material_crc,
block_pointer->C->has_normals,
block_pointer->C->has_normals ? print_vec_3(c, block_pointer->normals[0]) : "",
block_pointer->A->num_vertices,
block_pointer->C->has_normals ? print_vec_3(d, block_pointer->normals[block_pointer->A->num_vertices-1]) : "",
print_vec_3(e, block_pointer->tangents[0]),
block_pointer->A->num_vertices,
print_vec_3(f, block_pointer->tangents[block_pointer->A->num_vertices-1]),
print_vec_3(g, block_pointer->bitangents[0]),
block_pointer->A->num_vertices,
print_vec_3(h, block_pointer->bitangents[block_pointer->A->num_vertices-1]),
print_vec_3(i, block_pointer->G->center),
block_pointer->G->radius,
block_pointer->G->has_vertex_colors,
block_pointer->G->has_vertex_colors ? print_vec_4(j, block_pointer->vertex_colors[0]) : "",
block_pointer->A->num_vertices,
block_pointer->G->has_vertex_colors ? print_vec_4(k, block_pointer->vertex_colors[block_pointer->A->num_vertices-1]) : "",
print_vec_2(l, block_pointer->uv_sets[0]),
block_pointer->A->num_vertices,
print_vec_2(m, block_pointer->uv_sets[block_pointer->A->num_vertices-1]),
block_pointer->J->consistency_flags,
block_pointer->J->additional_data,
block_pointer->J->num_triangles,
block_pointer->J->num_triangle_points,
block_pointer->J->has_triangles,
print_ushort_3(o, block_pointer->triangles[0]),
block_pointer->J->num_triangles,
block_pointer->J->has_triangles ? print_ushort_3(p, block_pointer->triangles[block_pointer->J->num_triangles-1]) : "",
block_pointer->L->num_match_groups,
block_pointer->match_groups
);
}

static void print_bs_lighting_shader_property_pointer(struct nifp *nif, int n, char s[1000])
{
	char a[200], b[200], c[200], d[200];
	struct bs_lighting_shader_property_pointer *block_pointer = Blocks[n];
	snprintf(
		s, 1000,
		"\
skyrim_shader_type: %u\
\nname: %s [%i]\
\nnum_extra_data_list: %u\
\nextra_data_list\
\ncontroller: %i\
\nshader_flags_1: %u\
\nshader_flags_2: %u\
\nuv_offset: %s\
\nuv_scale: %s\
\ntexture_set: %i\
\nemissive_color: %s\
\nemissive_multiple: %f\
\ntexture_clamp_mode: %u\
\nalpha: %f\
\nrefraction_strength: %f\
\nglossiness: %f\
\nspecular_color: %s\
\nspecular_strength: %f\
\nlighting_effect_1: %f\
\nlighting_effect_2: %f\
",
block_pointer->A->skyrim_shader_type,
nifp_get_string(nif, block_pointer->A->name),
block_pointer->A->name,
block_pointer->A->num_extra_data_list,
block_pointer->B->controller,
block_pointer->B->shader_flags_1,
block_pointer->B->shader_flags_2,
print_vec_2(a, block_pointer->B->uv_offset),
print_vec_2(b, block_pointer->B->uv_scale),
block_pointer->B->texture_set,
print_vec_3(c, block_pointer->B->emissive_color),
block_pointer->B->emissive_multiple,
block_pointer->B->texture_clamp_mode,
block_pointer->B->alpha,
block_pointer->B->refraction_strength,
block_pointer->B->glossiness,
print_vec_3(d, block_pointer->B->specular_color),
block_pointer->B->specular_strength,
block_pointer->B->lighting_effect_1,
block_pointer->B->lighting_effect_2
);
}

static void print_bs_shader_texture_set_pointer(struct nifp *nif, int n, char s[1000])
{
	struct bs_shader_texture_set_pointer *block_pointer = Blocks[n];
	snprintf(
		s, 1000,
		"\
num_textures: %i\
\ntextures 0: %s\
\ntextures 1: %s\
\ntextures 2: %s\
\ntextures 3: %s\
\ntextures 4: %s\
\ntextures 5: %s\
\ntextures 6: %s\
\ntextures 7: %s\
\ntextures 8: %s\
",
block_pointer->A->num_textures,
block_pointer->textures[0],
block_pointer->textures[1],
block_pointer->textures[2],
block_pointer->textures[3],
block_pointer->textures[4],
block_pointer->textures[5],
block_pointer->textures[6],
block_pointer->textures[7],
block_pointer->textures[8]
);
}

api void nifp_print_block(struct nifp *nif, int n, char s[1000])
{
#define type(x) 0 == strcmp(block_type, x)
	s[0] = '\0';
	const char *block_type = Hedr->block_types[Hedr->block_type_index[n]];
	if (0);
	else if (type(NI_NODE)) print_ni_node_pointer(nif, n, s);
	else if (type(BS_LEAF_ANIM_NODE)) print_ni_node_pointer(nif, n, s);
	else if (type(BS_FADE_NODE)) print_ni_node_pointer(nif, n, s);
	else if (type(NI_TRI_SHAPE)) print_ni_tri_shape_pointer(nif, n, s);
	else if (type(NI_TRI_SHAPE_DATA)) print_ni_tri_shape_data_pointer(nif, n, s);
	else if (type(BS_LIGHTING_SHADER_PROPERTY)) print_bs_lighting_shader_property_pointer(nif, n, s);
	else if (type(BS_SHADER_TEXTURE_SET)) print_bs_shader_texture_set_pointer(nif, n, s);
}