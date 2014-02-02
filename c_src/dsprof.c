#include "erl_nif.h"


ERL_NIF_TERM atom_ok;


static int
load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info)
{
    atom_ok = enif_make_atom(env, "ok");
    return 0;
}


static ERL_NIF_TERM
direct(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return atom_ok;
}


static ERL_NIF_TERM
defered_call(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return enif_schedule_dirty_nif_finalizer(
            env,
            atom_ok,
            enif_dirty_nif_finalizer
        );
}


static ERL_NIF_TERM
ds_cpu(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int flags = ERL_NIF_DIRTY_JOB_CPU_BOUND;
    return enif_schedule_dirty_nif(env, flags, defered_call, argc, argv);
}


static ERL_NIF_TERM
ds_io(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int flags = ERL_NIF_DIRTY_JOB_IO_BOUND;
    return enif_schedule_dirty_nif(env, flags, defered_call, argc, argv);
}


static ErlNifFunc nif_funcs[] = {
    {"direct", 0, direct},
    {"ds_cpu", 0, ds_cpu},
    {"ds_io", 0, ds_io}
};


ERL_NIF_INIT(dsprof, nif_funcs, &load, NULL, NULL, NULL);
