#include <malloc.h>
#include <string.h>

#include "Buffer.h"

#define INIT_BUF_SIZE 16

typedef struct _SegIter {
    void* start;
    void* end;
    void* cur;
} SegIter;

SegIter* segiter(void* start, void* end) {
    SegIter* s = malloc(sizeof(SegIter));

    s->start = start;
    s->end = end;
    s->cur = start;

    return s;
}

char segiter_next(SegIter* s) {
    return ((char*)(s->cur++))[0];
}

// does allocate unfortunately
char* segiter_string(SegIter* s) {
    int len = s->end - s->start;
    char* str = malloc(len);
    memcpy(str, s->start, len);
    return str;
}

// get bytes to allocate from buffer and array length
#define unitsized(b, units) (units * b->unit_byte_size)

typedef struct _Buf {
    int id;
    BufType type;

    void* data;
    int* chunk_lens;
    int chunks_alloced;

    int len;
    int cap;
    int cur;
} Buf;

Buf* buf(BufType type) {
    static unsigned int id = 0;

    Buf* b = malloc(sizeof(Buf));

    b->id = id;
    b->type = type;

    b->len = 0;
    b->cap = INIT_BUF_SIZE;
    b->cur = 0;
    b->chunks_alloced = 0;
    
    b->chunk_lens = malloc(1);
    b->data = malloc(b->cap);

    return b;
}

void buf_reserve(Buf* b, int len) {
    // if we dont have enough space for len bytes
    if (b->len + len > b->cap) {
        // then realloc to b->cap *= 2
        // double to minimize reallocs
        b->cap *= 2;
        b->data = realloc(b->data, b->cap);

        // rerun to make sure doubling is enough
        buf_reserve(b, len);
    }
}

void buf_add(Buf* b, void* data, int len) {
    // check that we have enough space
    buf_reserve(b, len);

    // append
    memcpy(b->data + b->len, data, len);

    // update chunk length information
    b->chunks_alloced += 1;
    b->chunk_lens = realloc(b->chunk_lens, b->chunks_alloced);
    b->chunk_lens[b->chunks_alloced - 1] = len;

    // update length information
    b->len += len;
}

// uses chunk lengths to finda chunk of data
// indexed in the order that its put into the buffer
SegIter* buf_idx(Buf* b, int idx) {
    void* start = b->data;
    int i;
    for (i = 0; i < idx; i++) {
        start += b->chunk_lens[i];
    }
    void* end = start;
    end += b->chunk_lens[i];

    return segiter(start, end);
}

// takes len bytes from the buffer
void* buf_take(Buf* b, int len) {
    void* ptr = b->data + b->cur;
    b->cur += len;
    return ptr;
}

// splits buffer data so its not all combined
// returned pointers depend on the buffer to still be allocated
// do not run this function and then free the buffer
SegIter** buf_split(Buf* b) {
    SegIter** sarr = malloc(b->chunks_alloced * sizeof(SegIter*));

    void* running_ptr = b->data;
    int offset;

    for (int i = 0; i < b->chunks_alloced; i++) {
        offset = b->chunk_lens[i];
        sarr[i] = segiter(running_ptr, running_ptr+offset);
        running_ptr += offset;
    }

    return sarr;
}
