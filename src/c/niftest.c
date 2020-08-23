#include "c.h"

#include "nif.h"
#include "bsa.h"

void test_rundown(int, int, nif_rundown_t *);

void nif_test(void *bsa)
{
	bsa_t *meshes = (bsa_t *)bsa;
	rc_t *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	cassert_(rc, "mh no bucket02a");
	bsa_read(meshes, rc);
	nif_t *bucket = nif_alloc();
	bucket->path = rc->path;
	bucket->buf = rc->inf;
	nif_read(bucket);
	nif_save(rc, bucket);
	nif_rundown_t *rd = nif_alloc_rundown();
	rd->nif = bucket;
    rd->generic = test_rundown;
	nif_accept(bucket, rd, NULL);
}

void test_rundown(int parent, int block, nif_rundown_t *rd) {
	
}