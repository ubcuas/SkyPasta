## Util ##
list:
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$'

## Build ##
build-cpp:
	mkdir -p build/
	cd build/; cmake ../src/; $(MAKE)

## Test ##
test-cpp: build-cpp
	cd build; make test

## Install ##
install: build-cpp
	cd build; make install

## Cleanup ##
clean :
	-rm -rf build/

## Docker ##
docker-multiarch-deps:
	DOCKER_CLI_EXPERIMENTAL=enabled DOCKER_BUILDKIT=enabled docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
	DOCKER_CLI_EXPERIMENTAL=enabled DOCKER_BUILDKIT=enabled docker buildx create --name mubuilder | echo "ok"
	DOCKER_CLI_EXPERIMENTAL=enabled DOCKER_BUILDKIT=enabled docker buildx use mubuilder
	DOCKER_CLI_EXPERIMENTAL=enabled DOCKER_BUILDKIT=enabled docker buildx inspect --bootstrap

docker:
	docker build . --pull=true --tag ubcuas/skypasta:latest

docker-publish: docker
	docker push ubcuas/skypasta:latest

docker-multiarch: docker-multiarch-deps
	DOCKER_CLI_EXPERIMENTAL=enabled \
	DOCKER_BUILDKIT=enabled \
	docker buildx build . --pull=true -t ubcuas/skypasta:latest --platform "linux/amd64"

docker-multiarch-publish: docker-multiarch-deps
	DOCKER_CLI_EXPERIMENTAL=enabled \
	DOCKER_BUILDKIT=enabled \
	docker buildx build . --pull=true -t ubcuas/skypasta:latest --push --platform "linux/amd64"

## CI ##
ci-test:
	echo "Test"
