#ifndef __EMBED_FILE_H__
#define __EMBED_FILE_H__

#define EXT_EMBED_FILE(NAME) \
    extern const uint8_t _binary_ ## NAME ## _start[]; \
    extern const uint8_t _binary_ ## NAME ## _end[];
#define EXT_EMBED_FILE_CONTENT(NAME) \
    ((uint8_t *) _binary_ ## NAME ## _start)
#define EXT_EMBED_FILE_END(NAME) \
    ((uint8_t *) _binary_ ## NAME ## _end)
#define EXT_EMBED_FILE_SIZE(NAME) \
    ((_binary_ ## NAME ## _end) - (_binary_ ## NAME ## _start))
#define EXT_EMBED_FILE_PRINT(NAME) \
    char *p = ((char*)_binary_## NAME ##_start); \
    while (p != ((char *)_binary_ ## NAME ## _end)) putchar(*p++);
#endif
