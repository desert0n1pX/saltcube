BIN_NAME=saltcube
CXX=g++
CXX_FLAGS=-Wall -Wpedantic -Wextra -Werror
CXX_LIBS=-lsodium -lssl -lcrypto
CXX_INCLUDES=-Iinclude
CXX_VERS=c++23
CXX_SRC=src/*.cpp

CONTAINERFILE_REV=1.0
CONTAINERFILE=Containerfile
CONTAINER_SRC=localhost
CONTAINER_NAME=buildenv
CONTAINERFILE_IGNORE=.containerignore

# Source targets, default saltcube

$(BIN_NAME):
	$(CXX) -std=$(CXX_VERS) $(CXX_EXTRA_FLAGS) $(CXX_FLAGS) $(CXX_INCLUDES) $(CXX_SRC) $(CXX_LIBS) -o $(BIN_NAME)

# Container images for env

$(CONTAINER_NAME)-docker_$(CONTAINERFILE_REV).tar:
	ln -sf $(CONTAINERFILE_IGNORE) .dockerignore
	docker image build --file Containerfile -t $(CONTAINER_NAME):$(CONTAINERFILE_REV) .
	docker image save $(CONTAINER_NAME):$(CONTAINERFILE_REV) -o $(CONTAINER_NAME)-docker_$(CONTAINERFILE_REV).tar
	rm .dockerignore

$(CONTAINER_NAME)-podman_$(CONTAINERFILE_REV).tar:
	podman build -f Containerfile --ignorefile $(CONTAINERFILE_IGNORE) -t $(CONTAINER_SRC)/$(CONTAINER_NAME):$(CONTAINERFILE_REV) .
	podman image save $(CONTAINER_SRC)/$(CONTAINER_NAME):$(CONTAINERFILE_REV) -o $(CONTAINER_NAME)-podman_$(CONTAINERFILE_REV).tar

# Container Targets

in-docker: $(CONTAINER_NAME)-docker_$(CONTAINERFILE_REV).tar
	docker run --rm -v .:/build --env CXX_EXTRA_FLAGS=$(CXX_EXTRA_FLAGS) --env EXTRA_MAKE_FLAGS=--always-make --env MAKE_TARGET=$(BIN_NAME) $(CONTAINER_NAME):$(CONTAINERFILE_REV)

in-podman: $(CONTAINER_NAME)-podman_$(CONTAINERFILE_REV).tar
	podman run --rm -v .:/build --env CXX_EXTRA_FLAGS=$(CXX_EXTRA_FLAGS) --env EXTRA_MAKE_FLAGS=--always-make $(CONTAINER_SRC)/$(CONTAINER_NAME):$(CONTAINERFILE_REV)

# Install / Remove

install: $(BIN_NAME)
# Dont strip if we aren't installing on a real system for example if we are being run by makepkg
	test -z "$(DESTDIR)" && strip -s $(BIN_NAME)  || echo "Not stripping binary, we aren't installing on a live system"
	install -Dm755 $(BIN_NAME) $(DESTDIR)/usr/bin/$(BIN_NAME)

uninstall:
	rm -f $(DESTDIR)/usr/bin/$(BIN_NAME)

# Environment 

clean:
	rm -rf $(BIN_NAME)
	rm -rf vgcore.*

clean-all: clean
	rm -rf $(CONTAINER_NAME)-docker_$(CONTAINERFILE_REV).tar
	rm -rf $(CONTAINER_NAME)-podman_$(CONTAINERFILE_REV).tar
	docker image rm $(CONTAINER_NAME):$(CONTAINERFILE_REV) || true
	podman image rm $(CONTAINER_SRC)/$(CONTAINER_NAME):$(CONTAINERFILE_REV) || true

configure:
	bear -- env CXX_EXTRA_FLAGS="-g" make --always-make $(BIN_NAME)
	mkdir -p .vscode
	cp -i ./development/*.json ./.vscode/

.PHONY: clean configure container debug in-docker in-podman