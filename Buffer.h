#ifndef BUFFER_H
#define BUFFER_H

typedef struct _SegIter SegIter;

SegIter* segiter(void* start, void* end);
char segiter_next(SegIter* s);
char* segiter_string(SegIter* s);

typedef enum _BufType {
    NETBUF, FILEBUF, NETANDFILEBUF,
} BufType;

struct _Buf;
typedef struct _Buf Buf;

Buf* buf(BufType type);
void buf_reserve(Buf* b, int len);
void buf_add(Buf* b, void* data, int len);
void* buf_take(Buf* b, int len);

// uses chunk lengths to finda chunk of data
// indexed in the order that its put into the buffer
SegIter* buf_idx(Buf* b, int idx);

// splits buffer data so its not all combined
// returned pointers depend on the buffer to still be allocated
// do not run this function and then free the buffer
SegIter** buf_split(Buf* b);

#endif /* BUFFER_H / Buffer.h */