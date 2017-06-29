#ifndef EFL_FUTURE_H_
# define EFL_FUTURE_H_

/**
 * @addtogroup Efl_Future Efl future and promise.
 * @{
 */

/**
 * @typedef Efl_Promise
 * The type of Efl Promise used in asynchronous operation, the write side of a promise.
 */
typedef Eo Efl_Promise;
#define _EFL_PROMISE_EO_CLASS_TYPE

typedef struct _Efl_Future Efl_Future;

EAPI extern Eina_Error EINA_ERROR_FUTURE_CANCEL; /**< @since 1.19 */

typedef void (*Efl_Future_Success_Cb)(void *data, Efl_Promise *next, const void *value);
typedef void (*Efl_Future_Success_Steal_Cb)(void *data, Efl_Promise *next, void *value);
typedef void (*Efl_Future_Failure_Cb)(void *data, Efl_Promise *next, Eina_Error error);
typedef void (*Efl_Future_Progress_Cb)(void *data, Efl_Promise *next, const void *progress);

/**
 * @brief Add sets of callbacks to handle the progress and result of a future.
 *
 * callbacks are called depending on the outcome of the promise related to the future.
 *
 * @param[in] f the future to track
 * @param[in] success the callback to call in case of a succesful computation from the promise
 * @param[in] failure the callback to call in case of a failure to deliver from the promise
 * @param[in] progress the callback to call during the progression of the the promise, this is optional
 * @param[in] data additional data to pass to the callback
 *
 * @return Return a new future when the callback has been successfully added pointing to the next request
 * being processed during the future success, failure or progress callback (You can reference count the next
 * promise to defer the result and make it asynchronous too). This future can be ignored.
 *
 * It is assumed that you are going to steal the value of this future and will
 * deal by yourself of its life. There can only be one steal and it will be
 * called last, once all the other then have been processed (It will technically
 * wait for the next loop iteration to be propagated).
 *
 * @note except if you do reference count the Efl.Future object, you can only call once this function.
 *
 * @ingroup Efl_Future
 */
EAPI Efl_Future *efl_future_steal_then(Efl_Future *f,
                                       Efl_Future_Success_Steal_Cb success,
                                       Efl_Future_Failure_Cb failure,
                                       Efl_Future_Progress_Cb progress,
                                       void *data);

/**
 * @brief Add sets of callbacks to handle the progress and result of a future.
 *
 * callbacks are called depending on the outcome of the promise related to the future.
 *
 * @param[in] f the future to track
 * @param[in] success the callback to call in case of a succesful computation from the promise
 * @param[in] failure the callback to call in case of a failure to deliver from the promise
 * @param[in] progress the callback to call during the progression of the the promise, this is optional
 * @param[in] data additional data to pass to the callback
 *
 * @return Return a new future when the callback has been successfully added pointing to the next request
 * being processed during the future success, failure or progress callback (You can reference count the next
 * promise to defer the result and make it asynchronous too. This future can be ignored.
 *
 * @note except if you do reference count the Efl.Future object, you can only call once this function.
 *
 * @since 1.20
 *
 * @ingroup Efl_Future
 */
EAPI Efl_Future *efl_future_then(Efl_Future *f,
                                 Efl_Future_Success_Cb success,
                                 Efl_Future_Failure_Cb failure,
                                 Efl_Future_Progress_Cb progress,
                                 void *data);

/**
 * @brief Cancel the need for that specific future.
 *
 * This will trigger the failure of the future and may result in the promise stopping its computation as
 * it will be notified when there is no more need for computing the request.
 *
 * @see efl_future_use
 *
 * @ingroup Efl_Future
 *
 * @since 1.20
 */
EAPI void efl_future_cancel(Efl_Future *f);

/**
 * @brief To be used in conjunction with when you plan to use efl_future_cancel
 *
 * This function will store in *wref, obj and make sure that on failure or success of the future, it
 * will be reset to NULL. This guarantee that the pointer you are using will always be correct and
 * that you do not have to worry about passing a dead pointer to efl_future_cancel.
 *
 * @param[out] storage Will be set to obj and tracked during all the lifetime of the future.
 * @param[in] future The future to remember about.
 *
 * @see efl_future_cancel
 *
 * @ingroup Efl_Future
 */
EAPI void efl_future_use(Efl_Future **storage, Efl_Future *future);

/**
 * @}
 */

/**
 * @}
 */

#endif
