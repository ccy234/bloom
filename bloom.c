#include <limits.h>
#include <stdarg.h>
#include <string.h>

#include "bloom.h"

#define SETBIT(a, n) (a[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(a, n) (a[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))
#define p_malloc malloc
#define p_free free
BLOOM *bloom_create(size_t size, size_t nfuncs, ...)
{
	BLOOM *bloom;
	va_list l;
	int n;
    size_t bloom_a_size = sizeof(char) * (size + CHAR_BIT - 1) / CHAR_BIT;
    size_t bloom_funcs_size = nfuncs * sizeof(hashfunc_t);

    if((bloom = p_malloc(sizeof(BLOOM))) == NULL) {
        return NULL;
    }

    memset(bloom, 0, sizeof(BLOOM));
    bloom->a = p_malloc(bloom_a_size);
	if(bloom->a == NULL) {
		goto FAIL;
	}

    bloom->funcs = (hashfunc_t *)p_malloc(bloom_funcs_size);
	if(bloom->funcs == NULL) {
		goto FAIL;
	}

    bzero(bloom->a, bloom_a_size);
    bzero(bloom->funcs, bloom_funcs_size);

	va_start(l, nfuncs);
	for(n = 0; n < nfuncs; ++n) {
		bloom->funcs[n] = va_arg(l, hashfunc_t);
	}
	va_end(l);

	bloom->nfuncs=nfuncs;
	bloom->asize=size;

	return bloom;
FAIL:
    if (bloom != NULL) {
        if (bloom->a != NULL) {
            p_free(bloom->a);
        }

        if (bloom->funcs != NULL) {
            p_free(bloom->funcs);
        }

        p_free(bloom);
    }

    return NULL;
}

int bloom_destroy(BLOOM *bloom)
{
    if (bloom == NULL) {
        return 0;
    }

	p_free(bloom->a);
	p_free(bloom->funcs);
	p_free(bloom);

	return 0;
}

int bloom_add(BLOOM *bloom, const char *s)
{
	size_t n;

	for(n = 0; n < bloom->nfuncs; ++n) {
		SETBIT(bloom->a, bloom->funcs[n](s) % bloom->asize);
	}

	return 0;
}

int bloom_check(BLOOM *bloom, const char *s)
{
	size_t n;

	for(n = 0; n < bloom->nfuncs; ++n) {
        if(!(GETBIT(bloom->a, bloom->funcs[n](s) % bloom->asize))) {
            return 0;
        }
	}

	return 1;
}
