-module(dsprof).
-on_load(init/0).

-export([
    direct/0,
    ds_cpu/0,
    ds_io/0,
    via_thread/0
]).


direct() ->
    not_loaded(?LINE).


ds_cpu() ->
    not_loaded(?LINE).


ds_io() ->
    not_loaded(?LINE).


via_thread() ->
    not_loaded(?LINE),
    receive
        Msg -> Msg
    end.


init() ->
    SoName = case code:priv_dir(?MODULE) of
        {error, bad_name} ->
            case filelib:is_dir(filename:join(["..", priv])) of
                true ->
                    filename:join(["..", priv, ?MODULE]);
                _ ->
                    filename:join([priv, ?MODULE])
            end;
        Dir ->
            filename:join(Dir, ?MODULE)
    end,
    erlang:load_nif(SoName, 0).


not_loaded(Line) ->
    exit({not_loaded, [{module, ?MODULE}, {line, Line}]}).
