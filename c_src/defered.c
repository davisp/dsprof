
#include "dsprof.h"


static ERL_NIF_TERM
defered_call(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return enif_schedule_dirty_nif_finalizer(
            env,
            atom_ok,
            enif_dirty_nif_finalizer
        );
}


ERL_NIF_TERM
ds_cpu(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int flags = ERL_NIF_DIRTY_JOB_CPU_BOUND;
    return enif_schedule_dirty_nif(env, flags, defered_call, argc, argv);
}


ERL_NIF_TERM
ds_io(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int flags = ERL_NIF_DIRTY_JOB_IO_BOUND;
    return enif_schedule_dirty_nif(env, flags, defered_call, argc, argv);
}
