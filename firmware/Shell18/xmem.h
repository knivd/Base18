#ifndef XMEM_H
#define XMEM_H

// Exoteric Memory Manager
// (C) 2019, knivd@me.com
// Konstantin Dimitrov

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// IMPORTANT: this function must be called before any other XMEM function
// ===========================================================================

// initialise or reinitialise the entire memory
// will return the size of the managed memory after initialisation
// (mem_start) start address of a pre-allocated memory block or physical memory address
// (mem_size) is the size in bytes of the entire managed memory area
// (*xfail) is pointer to an optional callback function that is executed in case of
//			memory allocation failure where (sz) is the size of the failed block
uint32_t xmeminit(uint32_t mem_start, uint32_t mem_size, void (*xfail)(uint32_t sz));

// ===========================================================================

// allocate block or change size of already allocated one
// When calling for initial allocation the variable must be previously initialised with NULL
// A successfully allocated memory block is cleared
// If the block is being extended, the extension area is cleared
// The parameter (sz) specifies the size in number of bytes
// (sz) can be either positive or negative number:
//	1. If (sz) is a positive number, then a failure in the allocation will only
//		return a NULL pointer but will not cause execution of the failure callback function
//	2. If (sz) is a negative number, the failure callback function will be executed
//		in case of a failed allocation
// EXAMPLE:
// x = NULL;			// must be here for new allocation, or xfree(&x) if it was previously allocated
// xalloc ( (uint8_t **) &x, -1000 );	// allocate 1000 bytes to x (critical not to fail)
void xalloc(uint8_t **v, int32_t sz);

// free allocated block
// will do nothing if the block is not allocated
// will update the variable with NULL
void xfree(uint8_t **v);

// return the actual size of an allocated block
uint32_t xblksize(uint8_t *v);

// perform memory optimisation by combining unused blocks
void xdefrag(void);

// return the size of the largest continuous currently available block
uint32_t xavail(void);

// return the total size of the currently available memory (could be fragmented in many separate blocks)
uint32_t xtotal(void);

#ifdef __cplusplus
}
#endif

#endif // XMEM_H
