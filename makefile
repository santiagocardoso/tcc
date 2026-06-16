ROOT_DIR:=$(dir $(realpath $(firstword $(MAKEFILE_LIST))))

copy-sociable:
	git clone https://bitbucket.org/lumo-ufpb/sociable.git /tmp/lumo-ufpb-sociable
	cd /tmp/lumo-ufpb-sociable \
		&& git fetch origin develop \
		&& git checkout origin/develop
	mv /tmp/lumo-ufpb-sociable/ns-3.28/src/minuet/ $(ROOT_DIR)/minuet

build-docker-image:
	docker build -f ./Dockerfile -t "ns3:3.29-dev" .

start:
	docker compose up --no-start
	docker compose start

exec:
	docker compose exec ns3 bash

build:
	docker compose exec ns3 bash -c "cd ns-3.29 && CXXFLAGS='-Wall -g -O0' ./waf configure --build-profile=optimized --enable-examples --enable-tests"
	docker compose exec ns3 bash -c "cd ns-3.29 && ./waf build"

run:
	docker compose exec ns3 bash -c "cd ns-3.29 && ./waf --run minuet-scenario"