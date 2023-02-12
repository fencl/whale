#pragma once
#ifndef _whale_h_
#define _whale_h_
#ifdef __cplusplus
extern "C" {
#endif

/* /===========================================================================\
 * | whale_decode                                                              |
 * |===========================================================================|
 * | decodes a VP8L (Lossless WebP) bit stream into RGBA array                 |
 * |===========================================================================|
 * | user_data | user-defined pointer passed into stream, alloc and free       |
 * | stream    | a function which reads n bits from stream                     |
 * | alloc     | a function which allocates n bytes of memory                  |
 * | free      | a function which frees memory allocated by alloc              |
 * | width     | a pointer to unsigned long where width will be stored         |
 * | height    | a pointer to unsigned long where height will be stored        |
 * |===========================================================================|
 * |           | a pointer to an array of decoded bytes                        |
 * | returns   | (one byte per component, RGBA order, no padding/alignment)    |
 * |           | allocated using alloc, caller is responsible for freeing      |
 * \===========================================================================/
 */

unsigned char *whale_decode(
    void *user_data,

    // interface
    unsigned long (*stream) (unsigned char n, void *user_data),
    void*         (*alloc)  (unsigned long n, void *user_data),
    void          (*free)   (void     *block, void *user_data),

    // output
    unsigned *width,
    unsigned *height
);

#ifdef __cplusplus
}
#endif
#endif
