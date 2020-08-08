#ifndef NIF_NIF_H
#define NIF_NIF_H

// https://gist.github.com/EmilHernvall/953968

#define api

typedef struct nif_hedr_t nif_hedr_t;
typedef struct nif_t nif_t;
typedef struct nmap_t nmap_t;

struct nif_hedr_t
{
	char *header_string;
	char *version;
	unsigned char endian_type;
	uint32_t user_value;
	uint32_t num_blocks;
	uint32_t user_value_2;
	char *author;
	char *process_script;
	char *export_script;
};

struct nif_t
{
	nif_hedr_t hdr;
	int n;
	const char *path;
	const unsigned char *buf;
};

struct nmap_t {
	void *key;
	nif_t *value;
};
extern nmap_t nmap[1000];
extern int nifs;

void nif_gui();
void nif_test(void *);

char *nif_read_short_string(nif_t *, int *);

api nif_t *nif_alloc();
api void nif_read_header(nif_t *);
api void nif_read_header_string(nif_t *);
api void nif_make(void *, nif_t *);
api void nif_add(void *, nif_t *);
api nif_t *nif_get_stored(void *);

api char *nif_print_hedr(nif_t *);

#endif