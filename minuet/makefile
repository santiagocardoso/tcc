configure:
	cd ../../ && CXXFLAGS='-Wall -g -O0' ./waf configure --build-profile=debug --enable-examples --enable-tests

build:
	cd ../../ && ./waf build

run:
	cd ../../ && ./waf --run minuet-scenario

rundb:
	cd ../../ && ./waf --run minuet-scenario --command-template="gdb --args %s <args>"
