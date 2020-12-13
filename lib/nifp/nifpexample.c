#include "putc.h"

#include "nifp.h"

#include <bsa.h>

void test_callback(struct nifprd *, int, int);

// look at mesh.cc for good example of rd

void nifp_test()
{
	struct rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	// or
	// struct bsa *meshes = get_archives()[0];
	// struct rc *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	cassert(rc, "mh no bucket02a");
	bsa_read(rc);
	struct nifp *bucket = malloc_nifp();
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	nifp_read(bucket);
	nifp_save(rc, bucket);
	struct nifprd *rd = malloc_nifprd();
	rd->nif = bucket;
	rd->data = 0xf; // like a Mesh instance
    rd->other = test_callback;
	nifp_rd(rd);
	free_nifprd(&rd);
	free_nifp(&bucket);
    //char str[600];
    //nifp_print_hedr(bucket, str);
	//printf("nifp hedr end %i\n", bucket->hdr->end);
}

static void test_callback(struct nifprd *rd, int parent, int current) {

}