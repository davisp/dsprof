#include <assert.h>

#include "dsprof.h"


typedef struct _qitem_t
{
    struct _qitem_t*    next;
    ErlNifPid*          pid;
    ErlNifEnv*          env;
    int                 argc;
    ERL_NIF_TERM*       argv;
} qitem_t;


typedef struct
{
    ErlNifMutex*        lock;
    ErlNifCond*         cond;
    qitem_t*            head;
    qitem_t*            tail;
} queue_t;


typedef struct
{
    ErlNifThreadOpts*   opts;
    ErlNifTid           qthread;
    queue_t*            queue;
} state_t;


state_t* state = NULL;


qitem_t*
queue_item_create()
{
    qitem_t* item = (qitem_t*) enif_alloc(sizeof(qitem_t));
    item->pid = (ErlNifPid*) enif_alloc(sizeof(ErlNifPid));
    item->next = NULL;
    item->env = enif_alloc_env();
    item->argc = 0;
    item->argv = NULL;
    return item;
}


void
queue_item_destroy(qitem_t* item)
{
    enif_free_env(item->env);
    enif_free(item->pid);

    if(item->argv != NULL) {
        enif_free(item->argv);
    }
    
    enif_free(item);
}


queue_t*
queue_create()
{
    queue_t* ret;

    ret = (queue_t*) enif_alloc(sizeof(queue_t));
    if(ret == NULL) return NULL;

    ret->lock = NULL;
    ret->cond = NULL;
    ret->head = NULL;
    ret->tail = NULL;

    ret->lock = enif_mutex_create("queue_lock");
    if(ret->lock == NULL) goto error;

    ret->cond = enif_cond_create("queue_cond");
    if(ret->cond == NULL) goto error;

    return ret;

error:
    if(ret->lock != NULL) enif_mutex_destroy(ret->lock);
    if(ret->cond != NULL) enif_cond_destroy(ret->cond);
    if(ret != NULL) enif_free(ret);
    return NULL;
}


int
queue_push(queue_t* queue, qitem_t* item)
{
    item->next = NULL;

    enif_mutex_lock(queue->lock);

    if(queue->tail != NULL)
    {
        queue->tail->next = item;
    }

    queue->tail = item;

    if(queue->head == NULL)
    {
        queue->head = queue->tail;
    }

    enif_cond_signal(queue->cond);
    enif_mutex_unlock(queue->lock);

    return 1;
}


qitem_t*
queue_pop(queue_t* queue)
{
    qitem_t* item;

    enif_mutex_lock(queue->lock);

    while(queue->head == NULL)
    {
        enif_cond_wait(queue->cond, queue->lock);
    }

    item = queue->head;
    queue->head = item->next;
    item->next = NULL;

    if(queue->head == NULL)
    {
        queue->tail = NULL;
    }

    enif_mutex_unlock(queue->lock);

    return item;
}


static void*
thr_main(void* ignored)
{
    qitem_t* item;
    ERL_NIF_TERM msg;

    while(1) {
        item = queue_pop(state->queue);
        msg = atom_ok;
        enif_send(NULL, item->pid, item->env, msg);
        queue_item_destroy(item);
    }

    return NULL;
}


void
start_threaded(ErlNifEnv* env)
{
    state = (state_t*) enif_alloc(sizeof(state_t));
    state->queue = queue_create();
    state->opts = enif_thread_opts_create("thread_opts");

    enif_thread_create("", &(state->qthread), thr_main, state, state->opts);

    return;
}


ERL_NIF_TERM
via_thread(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    qitem_t* item = queue_item_create();
    int i;
    
    enif_self(env, item->pid);
    item->argc = argc;
    
    if(argc > 0) {
        item->argv = (ERL_NIF_TERM*) enif_alloc(argc * sizeof(ERL_NIF_TERM));
        for(i = 0; i < argc; i++) {
            item->argv[i] = enif_make_copy(item->env, argv[i]);
        }
    }
    
    queue_push(state->queue, item);
    
    return atom_ok;
}
