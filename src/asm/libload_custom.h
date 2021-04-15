#ifndef libload_custom_h
#define libload_custom_h

#include <stdint.h>
#include <stdbool.h>

typedef struct _sha1_ctx {
	uint8_t data[64];
	uint32_t datalen;
	uint8_t bitlen[8];
	uint32_t state[5];
	uint32_t k[4];
} sha1_ctx;

typedef struct _sha256_ctx {
	uint8_t data[64];
	uint32_t datalen;
	uint8_t bitlen[8];
	uint32_t state[8];
} sha256_ctx;

#define SHA1_DIGEST_LEN     20
#define SHA256_DIGEST_LEN   32


bool load_library(const uint8_t *name);
bool load_hashlib(void);
//bool load_bigint(void);


// HASHLIB DEFINES (FROM HASHLIB.H but here because we control things now. mwahaha)

void hashlib_SetMalloc(void*(void(*_alloc)(size_t)));
uint8_t hashlib_Sha1Init(sha1_ctx *ctx);
void hashlib_Sha1Update(sha1_ctx *ctx, const uint8_t* buf, size_t len);
void hashlib_Sha1Final(sha1_ctx *ctx, uint8_t* digest);

static inline void hashlib_SHA1(uint8_t* buf, size_t len, uint8_t* digest) {
	sha1_ctx ctx;
	hashlib_Sha1Init(&ctx);
	hashlib_Sha1Update(&ctx, buf, len);
	hashlib_Sha1Final(&ctx, digest);
}

//void _bigint_init(void); /* might wanna replace this with an actual function */


#endif
