
all:
	./rebar compile

clean:
	./rebar clean
	rm -rf .eunit/
	rm -rf priv/

check:
	./rebar compile eunit
