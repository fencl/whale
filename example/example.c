#include <stdlib.h>
#include <stdio.h>

// Whale can be included as a header-only library
#include "../whale.c"

// user_data
typedef struct example_context {
    FILE         *file;
    unsigned char buffer;
    unsigned long offset;
} example_context_t;

// simple imlementation of bit stream, not the most effetient one
static unsigned long example_stream_bit(example_context_t *ctx) {
    return (ctx->offset >= 8
        ? (fread(&ctx->buffer, ctx->offset = 1, 1, ctx->file), ctx->buffer)
        : ctx->buffer >> ctx->offset++
    ) & 1;
}

static unsigned long example_stream(unsigned char bits, void *user_data) {
    unsigned long out = 0;
    for (unsigned char b = 0; b < bits; ++b)
        out |= example_stream_bit((example_context_t*)user_data) << b;
    return out;
}

// alloc and free are mapped to standard c allocator
static void *example_alloc(unsigned long size, void *user_data) {
    (void)user_data;
    return malloc(size);
}

static void example_free(void *block, void *user_data) {
    (void)user_data;
    free(block);
}

// convert WebP image to TARGA
static void example_webp_to_targa(const char* in_file, const char *out_file) {
    // open input file
    FILE *in = fopen(in_file, "rb");

    // whale only decodes VP8L bit stream, not the whole webp image
    // however, VP8L bistream starts right after the header which is 21 bytes
    fseek(in, 21, SEEK_CUR);

    // user_data
    example_context_t ctx = { in, 0, 8 };

    // decode webp
    unsigned w, h;
    unsigned char *rgba = (unsigned char*) whale_decode(&ctx,
        example_stream,
        example_alloc,
        example_free,
        &w, &h);

    // close input file
    fclose(in);

    // TARGA uses BGRA, Whale outputs RGBA
    for (unsigned char* px = rgba, *end = px + w * h * 4; px < end; px += 4) {
        unsigned char px0 = px[0];
        px[0] = px[2];
        px[2] = px0;
    }

    // open output file
    FILE *out = fopen(out_file, "wb");

    // TARGA header
    unsigned char tga_hdr[] = {
        0,0,2,0,0,0,0,0,0,0,0,0,
        (unsigned char)(w & 0xFF),
        (unsigned char)((w >> 8) & 0xFF),
        (unsigned char)(h & 0xFF),
        (unsigned char)((h >> 8) & 0xFF),
        32, 32
    };

    // write header
    fwrite(&tga_hdr, 18, 1, out);

    // write BGRA data
    fwrite(rgba, 1, w * h * 4, out);

    // close output file
    fclose(out);

    // free image
    example_free(rgba, &ctx);
}

// entry
int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: webp2tga [input] [output]\n");
        return 1;
    }

    example_webp_to_targa(argv[1], argv[2]);
    return 0;
}
