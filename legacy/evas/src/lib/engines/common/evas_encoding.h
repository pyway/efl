#ifndef EVAS_ENCODING_H
#define EVAS_ENCODING_H
#include <Eina.h>

/* FIXME: An assumption that will probably break in the future */
#define EVAS_ENCODING_UTF8_BYTES_PER_CHAR 4

EAPI int
evas_common_encoding_utf8_get_next(const unsigned char *buf, int *iindex);

EAPI int
evas_common_encoding_utf8_get_prev(const unsigned char *buf, int *iindex);

EAPI int
evas_common_encoding_utf8_get_last(const unsigned char *buf, int buflen);

EAPI int
evas_common_encoding_utf8_get_len(const unsigned char *buf);

EAPI Eina_Unicode *
evas_common_encoding_utf8_to_unicode(const unsigned char *utf, int *_len) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_MALLOC;

EAPI char *
evas_common_encoding_unicode_to_utf8(const Eina_Unicode *uni, int *_len) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_MALLOC;

#endif
