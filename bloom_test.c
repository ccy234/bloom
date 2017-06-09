#include <stdio.h>
#include "bloom.h"
#include <string.h>
#define BLOOM_SIZE 1024000
//#include<stdlib.h>
//
//typedef unsigned int (*hashfunc_t)(const char *);
//typedef struct {
//    size_t asize;
//    unsigned char *a;
//    size_t nfuncs;
//    hashfunc_t *funcs;
//} BLOOM;
//
//BLOOM *bloom_create(size_t size, size_t nfuncs, ...);
//int bloom_destroy(BLOOM *bloom);
//int bloom_add(BLOOM *bloom, const char *s);
//int bloom_check(BLOOM *bloom, const char *s);

unsigned int sax_hash(const char *key)
{
    unsigned int h = 0;

    while (*key) {
        h ^= (h << 5)+(h >> 2) + (unsigned char)*key++;
    }

    return h;
}

unsigned int sdbm_hash(const char *key)
{
    unsigned int h = 0;

    while (*key) {
        h = (unsigned char)*key++ + (h << 6) + (h << 16) - h;
    }

    return h;
}


int main()
{
    int data=0;
    BLOOM *bloom=NULL;
    char str[16]={0};
    unsigned int i=0;
    if ((bloom = bloom_create(BLOOM_SIZE, 2, sax_hash, sdbm_hash)) == NULL) {
        printf("create error\n");
    } 

    for (i=0; i<10000; i++) {
        memset(str, 0x0, sizeof(str));
        snprintf(str, sizeof(str), "%u", i);
        bloom_add(bloom, str);

    }

    if (bloom_check(bloom, "1234")) {
        printf("find it\n");
    } else {
        printf("not find\n");
    }

    if (bloom_check(bloom, "12345")) {
        printf("find it");
    } else {
        printf("not find\n");
    }
    
    bloom_destroy(bloom);
    return 0;
}
