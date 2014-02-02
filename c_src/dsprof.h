#ifndef DSPROF_H
#define DSPROF_H


#include "erl_nif.h"


extern ERL_NIF_TERM atom_ok;


ERL_NIF_TERM direct(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM ds_cpu(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM ds_io(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM via_thread(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);

void start_threaded(ErlNifEnv* env);


#endif // Included dsprof.h