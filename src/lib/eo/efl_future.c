#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eina.h>
#include "Eo.h"

typedef struct _Efl_Future_Chain Efl_Future_Chain;
typedef struct _Efl_Future_Steal Efl_Future_Steal;

struct _Efl_Future_Chain
{
   EINA_INLIST;

   Efl_Promise *next;

   Efl_Future_Success_Cb success;
   Efl_Future_Failure_Cb failure;
   Efl_Future_Progress_Cb progress;

   void *data;
};

struct _Efl_Future_Steal
{
   Efl_Promise *next;

   Efl_Future_Success_Steal_Cb success;
   Efl_Future_Failure_Cb failure;
   Efl_Future_Progress_Cb progress;

   void *data;
};

struct _Efl_Future
{
   Eina_Inlist *chains_start;

   Efl_Future_Steal *stealer;

   Efl_Future ***wrefs;

   Eo *parent;

   struct {
      Efl_Future_Value *value;
      Eina_Error error;
   } hold;

   Eina_Bool loop_iterated : 1;
   Eina_Bool data : 1;
   Eina_Bool error : 1;
};

static Eina_Mempool *mpf = NULL;
static Eina_Mempool *mpfc = NULL;

static Efl_Class *promise = NULL;

static const char EINA_ERROR_FUTURE_CANCEL_STR[] = "Future cancelled";
EAPI Eina_Error EINA_ERROR_FUTURE_CANCEL;

static inline Eina_Bool
efl_future_check(Efl_Future *f)
{
#ifdef DEBUG
   return eina_mempool_from(mpf, f);
#endif
   return EINA_TRUE;
}

EAPI Efl_Future *
efl_future_add(Eo *parent)
{
   Efl_Future *f;

   f = eina_mempool_malloc(mpf, sizeof (Efl_Future));
   if (!f) return NULL;

   f->chains_start = NULL;
   f->stealer = NULL;
   f->wrefs = NULL;
   f->loop_iterated = EINA_FALSE;
   f->data = EINA_FALSE;
   f->error = EINA_FALSE;

   // FIXME: Track lifetime of parent
   f->parent = parent;

   return f;
}

EAPI Efl_Future *
efl_future_then(Efl_Future *f,
                Efl_Future_Success_Cb success,
                Efl_Future_Failure_Cb failure,
                Efl_Future_Progress_Cb progress,
                void *data)
{
   Efl_Future_Chain *fc;

   if (!promise)
     {
        ERR("Initialization of Future hasn't been done properly");
        return NULL;
     }

   if (!efl_future_check(f)) return NULL;

   fc = eina_mempool_malloc(mpfc, sizeof (Efl_Future_Chain));
   if (!fc) return NULL;

   fc->next = efl_add(promise, f->parent);
   if (!fc->next) goto on_error;

   fc->success = success;
   fc->failure = failure;
   fc->progress = progress;
   fc->data = data;

   f->chains_start = eina_inlist_append(f->chains_start, EINA_INLIST_GET(fc));

   return efl_promise_future_get(fc->next);
}

EAPI Efl_Future *
efl_future_steal_then(Efl_Future *f,
                      Efl_Future_Success_Steal_Cb success,
                      Efl_Future_Failure_Cb failure,
                      Efl_Future_Progress_Cb progress,
                      void *data)
{
   Efl_Future_Steal *stealer;

   if (!efl_future_check(f)) return NULL;
   if (f->stealer)
     {
        ERR("There is already a stealing callback pair registered on future %p. There can only be one.", f);
        return NULL;
     }
   // FIXME: Check promise parent to see if there is already a stealer there

   stealer = eina_mempool_malloc(mpfs, sizeof (Efl_Future_Steal));
   if (!stealer) return NULL;

   stealer->next = efl_add(promise, f->parent);
   if (!stealer->next) goto on_error;

   stealer->success = success;
   stealer->failure = failure;
   stealer->progress = progress;
   stealer->data = data;

   f->stealer = stealer;

   return efl_promise_future_get(stealer->next);
}

EAPI void
efl_future_cancel(Efl_Future *f)
{
   efl_future_error_set(f, EINA_ERROR_FUTURE_CANCEL);
}

EAPI void
efl_future_use(Efl_Future **storage, Efl_Future *future)
{
   unsigned int count = 0;
   Efl_Future ***tmp;

   if (!efl_future_check(f)) return NULL;

   if (future->wrefs)
     {
        for (; future->wrefs[count]; count++)
          ;
     }
   count++; /* New wref */

   tmp = realloc(future->wrefs, sizeof (Efl_Future **) * (count + 1));
   if (tmp)
     {
        future->wrefs = tmp;
        future->wrefs[count - 1] = storage;
        future->wrefs[count] = NULL;
        *storage = future;
     }
}

EAPI void
efl_future_propagate(Efl_Future *f)
{
   Efl_Future_Chain *fc;

   EINA_INLIST_FREE(f->chains_start, fc)
     {
        if (f->error) fc->failure(fc->data, fc->next, f->hold.error);
        else fc->success(fc->data, fc->next, f->hold.value->data);
        eina_mempool_free(mpfc, fc);
     }

   if (f->stealer)
     {
        if (EINA_REFCOUNT_GET(f->hold.value->data) > 1)
          {
             ERR("Future %p should have been called to propagate value the last, but hasn't.", f);
             goto on_error;
          }

        if (f->error) f->stealer->failure(f->stealer->data, f->stealer->next, f->hold.error);
        else f->stealer->success(f->stealer->data, f->stealer->next, f->hold.value->data);

        eina_mempool_free(mpfs, f->stealer);
        f->stealer = NULL;

        // No need to free the value has it has been stolen.
     }
   else
     {
     on_error:
        EINA_REFCOUNT_UNREF(f->hold.value)
          {
          }
     }
}

EAPI Efl_Future_Value *
efl_future_value_new(void *data, Eina_Free_Cb cb)
{
   
}

EAPI void
efl_future_value_free(Efl_Future_Value *value)
{
   if (value->cb)
     value->cb(value->data);
   eina_mempool_free(mpv, value);
   value = NULL;
}

EAPI Eina_Bool
efl_future_loop_iteration(Efl_Future *f)
{
   f->loop_iterated = EINA_TRUE;

   if (f->data || f->error) return EINA_TRUE;
   return EINA_FALSE;
}

EAPI Eina_Bool
efl_future_data_set(Efl_Future *f, Efl_Future_Value *value)
{
   if (f->data || f->error)
     {
        ERR("Failed to set data on %f which is already finalized.", f);
        return EINA_FALSE;
     }

   f->data = EINA_TRUE;
   f->hold.value = value;
   EINA_REFCOUNT_REF(f->hold.value);

   if (!f->loop_iterated) return EINA_FALSE;
   return EINA_TRUE;
}

EAPI Eina_Bool
efl_future_error_set(Efl_Future *f, Eina_Error error)
{
   f->error = EINA_TRUE;
   f->hold.error = error;

   if (!f->loop_iterated) return EINA_FALSE;
   return EINA_TRUE;
}

EAPI void
efl_future_prepare(Efl_Future *f)
{
   unsigned int i;

   if (f->wrefs == NULL) return ;

   // Clears up all reference
   for (i = 0; f->wrefs[i]; i++)
     *f->wrefs[i] = NULL;
}

EAPI void
efl_future_promise_class_set(Efl_Class *p)
{
   promise = p;
}

Eina_Bool
efl_future_init(void)
{
   EINA_ERROR_FUTURE_CANCEL = eina_error_msg_static_register(EINA_ERROR_FUTURE_CANCEL_STR);

   mpf = eina_mempool_add("chained_mempool", "test", NULL, sizeof (Efl_Future), 64);
   mpfc = eina_mempool_add("chained_mempool", "test", NULL, sizeof (Efl_Future_Chain), 64);
   mpfs = eina_mempool_add("chained_mempool", "test", NULL, sizeof (Efl_Future_Steal), 64);
   return EINA_TRUE;
}

Eina_Bool
efl_future_shutdown(void)
{
   eina_mempool_del(mpf);
   mpf = NULL;

   eina_mempool_del(mpfc);
   mpfc = NULL;

   eina_mempool_del(mpfs);
   mpfs = NULL;

   return EINA_TRUE;
}
