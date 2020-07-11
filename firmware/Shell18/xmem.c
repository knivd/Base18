#include <string.h>
#include <stdint.h>
#include "xmem.h"

// memory block header structure
typedef struct {
    uint8_t **cv;		// C variable (NULL if the block is free)
	uint32_t len;		// length of the allocated block
	uint32_t need;		// actually needed size for the block (smaller or equal to .len)
	uint8_t *data;	// pointer to the allocated data block
} xhdr_t;

// block headers
xhdr_t *hcur = NULL;
uint8_t *dcur = NULL;

// start address of the managed memory array
volatile uint8_t *xstart = NULL;

// size in bytes of the managed memory array
uint32_t xlength = 0;

// pointer to callback function in case of memory allocation failure
static void (*xfailure)(uint32_t sz) = NULL;


uint32_t xmeminit(uint32_t mem_start, uint32_t mem_size, void (*xfail)(uint32_t sz)) {
	xstart = (uint8_t *) mem_start;
	xlength = mem_size;
    memset((uint8_t *) xstart, 0, xlength);
	dcur = (uint8_t *) xstart;
	hcur = (xhdr_t *) (xstart + xlength);
	xfailure = xfail;
    return xtotal();
}


// internal function
// find header based on data pointer; return NULL if the header can not be found
xhdr_t *findxhdr(uint8_t *da) {
	xhdr_t *h = (xhdr_t *) (xstart + xlength);
	while(--h >= hcur) {
		if(da == h->data) return h;
	}
	return NULL;	// unknown header (memory leak?)
}


// internal function
// try to find a free data block with suitable size among the already allocated headers
// return NULL if no such block exist
xhdr_t *findxavl(uint32_t sz) {
	xhdr_t *hbest = NULL;
	xhdr_t *h = (xhdr_t *) (xstart + xlength);
	while(--h >= hcur) {	// search among the currently allocated but unused blocks
		if( (h->cv == NULL) && (h->len >= sz) ) {
			if(hbest) {
				if( (sz-(h->len)) < (sz-(hbest->len)) ) hbest = h;	// try to find the smallest free block
																	// with size bigger than (sz)
				if(sz == (hbest->len) ) break;	// an exact match has been found
			}
			else hbest = h;
		}
	}
	return hbest;
}


uint32_t xblksize(uint8_t *v) {
	xhdr_t *h = findxhdr((uint8_t *) v);
	return ( (h)? (h->len) : 0 );
}


uint32_t xavail(void) {
	xdefrag();
	uint32_t t = ((uint8_t *) hcur) - dcur;
	xhdr_t *h = (xhdr_t *) (xstart + xlength);
	while(--h >= hcur) {
		if( (h->cv == NULL) && (h->len > t) ) t = h->len;
	}
	return t;
}


uint32_t xtotal(void) {
	xdefrag();
	uint32_t t = 0;
	xhdr_t *h = (xhdr_t *) (xstart + xlength);
	while(--h >= hcur) {
		if(h->cv == NULL) t += h->len;
		else t += (h->len - h->need);
	}
	return (t + (((uint8_t *) hcur) - dcur) + 1);
}


void xdefrag(void) {
    xhdr_t *h = (xhdr_t *) (xstart + xlength);
	while(--h >= hcur) {	// combine neighbouring unused blocks
		if(h->cv == NULL) {
			uint8_t flag = 0;
			uint8_t *p = h->data + h->len;
			xhdr_t *ht = (xhdr_t *) (xstart + xlength);
			while(--ht >= hcur) {
				if( (ht->data == p) && (ht->cv == NULL) ) {	// this one is suitable for combining with (h)
					h->len = h->len + ht->len;
					memmove( (uint8_t *) ((xhdr_t *) (hcur+1)), ((uint8_t *) hcur), ((ht-hcur) * sizeof(xhdr_t)) );
					hcur++;
					ht = (xhdr_t *) (xstart + xlength);	// restart the inner loop
					flag = 1;	// a change has been made
				}
			}
			if(flag) h = (xhdr_t *) (xstart + xlength);	// restart the outer loop
		}
	}
}


void xfree(uint8_t **v) {
    if( !v || (*v == NULL) || (((uint8_t *) *v) < xstart) || (((uint8_t *) *v) > (xstart + xlength)) ) return;
	xhdr_t *h = findxhdr(*v);
	if(h) {
		h->cv = NULL;
		h->need = 0;
	}
	*v = NULL;
}


// allocate new block
void xalloc_new(uint8_t **v, int32_t sz) {
	uint8_t retryf = 0;
	xhdr_t *h;
    retry:
	h = findxavl(sz);

	// new block will have to be allocated
	if(!h) {
		uint32_t htf;
		uint32_t hf = -1;
		xhdr_t *ht = (xhdr_t *) (xstart + xlength);

		// check to see if some of the currently allocated and used blocks have enough extra space
		while(--ht >= hcur) {
			htf = ht->len - ht->need;
			if( (htf >= sz) && (htf < hf) ) {
				h = ht;
				if(hf == sz) break; else hf = htf;	// immediately break on the first exact fit
			}
		}

		// if possible, reuse free part of an existing block and create a new header for it
		if( h && ( ((uint8_t *) hcur - dcur) >= sizeof(xhdr_t) ) ) {
			hcur--;
			hcur->len = sz;
			hcur->need = sz;
			hcur->cv = v;
			uint8_t *p = h->data; p += ((h->len) - sz); hcur->data = p;
			h->len -= sz;	// trim the existing old block down by (sz) bytes
			memset(hcur->data, 0, sz);	// clear the data area
			*v = hcur->data;
		}

		// allocating a completely new block
		else if( ((uint8_t *) hcur - dcur) >= (sz + sizeof(xhdr_t)) ) {
			hcur--;
			hcur->len = sz;
			hcur->need = sz;
			hcur->cv = v;
			hcur->data = dcur;
			dcur += sz;
			memset(hcur->data, 0, sz);	// clear the newly allocated block
			*v = hcur->data;
		}

		// can't find enough memory - try defragmentation
		else {
			xdefrag();
			retryf = !retryf;
			if(retryf) goto retry;
			*v = NULL;	// unable to allocate block with this size
			if(xfailure && (sz < 0)) xfailure(sz);	// execute the failure callback function
		}
	}

	// reusing already allocated block
	else {
		memset(h->data, 0, h->len);	// clear the block
		h->cv = v;
		h->need = sz;
		uint8_t *p = h->data; *v = p;
	}

}


// converted to non-recursive function
void xalloc(uint8_t **v, int32_t sz) {
	if(!v || !sz) {	// size zero is equivalent to free the memory block
		xfree(v);
		return;
	}
	int8_t szs = ((sz < 0) ? -1 : 1);	// will be -1 if (sz) is negative, or 1 otherwise
	if(szs < 0) sz = -sz;
	if( *v && ( (((uint8_t *) *v) < xstart) || (((uint8_t *) *v) > (xstart + xlength)) ) ) return;	// outside the array
	if(sz & 3) sz = ( (sz >> 2) +1 ) << 2;	// always align data length to 4-byte boundaries

	// re-allocation of an existing block
    if(*v) {
        xhdr_t *z = findxhdr(*v);	// find the current header
		if(z) {

			// data relocation will be needed
			if(z->len < sz) {
				xhdr_t *h = findxavl(sz);

				// new block will be needed for this size
				if(!h) {
					uint8_t *vt = *v;
					*v = NULL;
					xalloc_new(v, (sz * szs));	// allocate a new block with the needed size
					if(*v) {
						memcpy( *v, z->data, z->len );	// move the existing data to new location
						memset( (*v + (z->len)), 0, (sz - (z->len)) );	// clear the expansion area
						z->cv = NULL;	// free up the old block
					}
					else {	// unable to allocate the needed size
						*v = vt;
						xfree(v);
					}
				}

				// reusing already allocated block and relocating the data
				else {
					memcpy( h->data, z->data, z->len );
					memset( (h->data + z->len), 0, (sz - (z->len)) );	// clear the expansion area
					uint8_t *p = h->data; *v = p;
					h->need = sz;
					h->cv = z->cv;
					z->cv = NULL;	// free up the old block
				}
			}

			// nothing is needed since the current block can be expanded to the needed size
			else {
				memset( ((z->data) + sz), 0, ((z->len) - sz) );	// clear the expansion area
				z->need = sz;
			}
		}

		// the supplied (*v) was not currently allocated known block
		else {
			*v = NULL;
			if(xfailure && (szs < 0)) xfailure(sz);	// execute the failure callback function
		}
    }

	// new allocation
    else xalloc_new(v, sz);

}
