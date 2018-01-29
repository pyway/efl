#include "config.h"
#include "Efl.h"

#define MY_CLASS EFL_TEXT_MARKUP_MIXIN_CLASS

#define ERR(...) EINA_LOG_DOM_ERR(EINA_LOG_DOMAIN_DEFAULT, __VA_ARGS__)

#define _REPLACEMENT_CHAR 0xFFFC
#define _PARAGRAPH_SEPARATOR 0x2029
#define _NEWLINE '\n'
#define _TAB '\t'

#define _REPLACEMENT_CHAR_UTF8 "\xEF\xBF\xBC"
#define _PARAGRAPH_SEPARATOR_UTF8 "\xE2\x80\xA9"
#define _NEWLINE_UTF8 "\n"
#define _TAB_UTF8 "\t"
#define EVAS_TEXTBLOCK_IS_VISIBLE_FORMAT_CHAR(ch) \
   (((ch) == _REPLACEMENT_CHAR) || \
    ((ch) ==  _NEWLINE) || \
    ((ch) == _TAB) || \
    ((ch) == _PARAGRAPH_SEPARATOR))

#define _IS_TAB(item)                                             \
   (!strcmp(item, "tab") || !strcmp(item, "\t") || !strcmp(item, "\\t"))

#define _IS_LINE_SEPARATOR(item)                                             \
   (!strcmp(item, "br") || !strcmp(item, "\n") || !strcmp(item, "\\n"))

#define _IS_PARAGRAPH_SEPARATOR_SIMPLE(item)                                 \
   (!strcmp(item, "ps"))

#define _IS_PARAGRAPH_SEPARATOR(o, item)                                     \
   (_IS_PARAGRAPH_SEPARATOR_SIMPLE(item) ||                                  \
    (o->legacy_newline && _IS_LINE_SEPARATOR(item))) /* Paragraph separator */

typedef struct _Efl_Text_Markup_Data Efl_Text_Markup_Data;

struct _Efl_Text_Markup_Data
{
   char *markup;
};

static void
_prepend_text_run(Eo *eo_obj, Efl_Text_Cursor_Cursor *cur,
      const char *s, const char *p)
{
   if ((s) && (p > s))
     {
        char *ts;

        ts = alloca(p - s + 1);
        strncpy(ts, s, p - s);
        ts[p - s] = 0;
        //evas_textblock_cursor_text_prepend(cur, ts);
        efl_text_cursor_text_insert(eo_obj, cur, ts);
     }
}

static EOLIAN void
_efl_text_markup_markup_insert(Eo *eo_obj, Efl_Text_Markup_Data *o EINA_UNUSED,
      Efl_Text_Cursor_Cursor *cur, const char *text)
{
   if (text)
     {
        char *s, *p;
        char *tag_start, *tag_end, *esc_start, *esc_end;

        tag_start = tag_end = esc_start = esc_end = NULL;
        p = (char *)text;
        s = p;
        /* This loop goes through all of the mark up text until it finds format
         * tags, escape sequences or the terminating NULL. When it finds either
         * of those, it appends the text found up until that point to the textblock
         * proccesses whatever found. It repeats itself until the terminating
         * NULL is reached. */
        for (;;)
          {
             size_t text_len;
             /* If we got to the end of string or just finished/started tag
              * or escape sequence handling. */
             if ((*p == 0) ||
                   (tag_end) || (esc_end) ||
                   (tag_start) || (esc_start))
               {
                  if (tag_end)
                    {
                       /* If we reached to a tag ending, analyze the tag */
                       char *ttag;
                       size_t ttag_len = tag_end - tag_start;


                       ttag = malloc(ttag_len + 1);
                       if (ttag)
                         {
                            memcpy(ttag, tag_start, ttag_len);
                            ttag[ttag_len] = 0;
                            //evas_textblock_cursor_format_prepend(cur, ttag);
                            efl_text_format_insert(eo_obj, cur, ttag);
                            free(ttag);
                         }
                       tag_start = tag_end = NULL;
                    }
                  else if (esc_end)
                    {
                       //_prepend_escaped_char(cur, esc_start, esc_end + 1);
                       esc_start = esc_end = NULL;
                    }
                  else if (*p == 0)
                    {
                       _prepend_text_run(eo_obj, cur, s, p);
                       s = NULL;
                    }
                  if (*p == 0)
                    break;
               }
             if (*p == '<')
               {
                  if (!esc_start)
                    {
                       /* Append the text prior to this to the textblock and mark
                        * the start of the tag */
                       tag_start = p;
                       tag_end = NULL;
                       _prepend_text_run(eo_obj, cur, s, p);
                       s = NULL;
                    }
               }
             else if (*p == '>')
               {
                  if (tag_start)
                    {
                       tag_end = p + 1;
                       s = p + 1;
                    }
               }
             else if (*p == '&')
               {
                  if (!tag_start)
                    {
                       /* Append the text prior to this to the textblock and mark
                        * the start of the escape sequence */
                       esc_start = p;
                       esc_end = NULL;
                       _prepend_text_run(eo_obj, cur, s, p);
                       s = NULL;
                    }
               }
             else if (*p == ';')
               {
                  if (esc_start)
                    {
                       esc_end = p;
                       s = p + 1;
                    }
               }
             /* Unicode object replacement char */
             else if (!strncmp(_REPLACEMENT_CHAR_UTF8, p,
                      text_len = strlen(_REPLACEMENT_CHAR_UTF8)) ||
                   !strncmp(_NEWLINE_UTF8, p,
                      text_len = strlen(_NEWLINE_UTF8)) ||
                   !strncmp(_TAB_UTF8, p,
                      text_len = strlen(_TAB_UTF8)) ||
                   !strncmp(_PARAGRAPH_SEPARATOR_UTF8, p,
                      text_len = strlen(_PARAGRAPH_SEPARATOR_UTF8)))
               {
                  /*FIXME: currently just remove them, maybe do something
                   * fancier in the future, atm it breaks if this char
                   * is inside <> */
                  _prepend_text_run(eo_obj, cur, s, p);
                  /* it's also advanced later in this loop need +text_len
                     in total*/
                  p += text_len - 1;
                  s = p + 1; /* One after the end of the replacement char */
               }
             p++;
          }
     }
}

static EOLIAN void
_efl_text_markup_markup_set(Eo *eo_obj, Efl_Text_Markup_Data *o EINA_UNUSED,
      const char *text)
{
   efl_text_set(eo_obj, "");
   if (text)
     {
        efl_text_markup_insert(eo_obj,
              efl_text_cursor_get(eo_obj, EFL_TEXT_CURSOR_GET_MAIN), text);
     }
}

static EOLIAN const char *
_efl_text_markup_markup_get(Eo *eo_obj, Efl_Text_Markup_Data *o EINA_UNUSED)
{
   Eina_Iterator *itr;
   Efl_Text_Format_Format *fmt;
   Eina_Strbuf *buf;
   const char *text, *p;
   int prev_pos;

   buf = eina_strbuf_new();
   p = text = efl_text_get(eo_obj);
   printf("utf8 text: %s\n", text);
   itr = efl_text_formats_get(eo_obj, NULL, NULL);
   prev_pos = 0;
   EINA_ITERATOR_FOREACH(itr, fmt)
     {
        int pos;
        const char *format;
        const char *tmp;
        int off;

        format = efl_text_format_string_get(eo_obj, fmt);
        if (!format) continue;
        if (format[0] == '+')
          {
             tmp = "<%s>";
          }
        else
          {
             tmp = "</%s>";
          }
        pos = efl_text_format_position_get(eo_obj, fmt);
        printf(" -- format pos: %d\n", pos);
        off = pos - prev_pos;
        eina_strbuf_append_length(buf, p, off);
        eina_strbuf_append_printf(buf, tmp, format + 2);
        prev_pos = pos;
        p += off;
        if (efl_text_format_is_visible(eo_obj, fmt))
          {
             p++;
             prev_pos++;
          }
     }
   eina_strbuf_append(buf, p);
   return eina_strbuf_string_steal(buf);
}

#include "interfaces/efl_text_markup.eo.c"
