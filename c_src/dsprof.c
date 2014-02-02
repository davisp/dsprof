#include "dsprof.h"


ERL_NIF_TERM atom_ok;


static int
load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info)
{
    atom_ok = enif_make_atom(env, "ok");
    start_threaded(env);
    return 0;
}


static ErlNifFunc nif_funcs[] = {
    {"direct", 0, direct},
    {"ds_cpu", 0, ds_cpu},
    {"ds_io", 0, ds_io},
    {"via_thread", 0, via_thread}
};


ERL_NIF_INIT(dsprof, nif_funcs, &load, NULL, NULL, NULL);
