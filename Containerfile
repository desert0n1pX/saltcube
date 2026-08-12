# Continerfile Revision 1

# If this project ever targets non arch systems for packaged versions you should use debian:forky-slim
FROM archlinux:base-devel-20260809.0.570793

WORKDIR /root

# Debian deps: build-essential bear libsodium-dev libssl-dev
RUN pacman-key --init && pacman -Syy --noconfirm base-devel bear libsodium openssl && rm -rf /etc/pacman.d/gnupg

VOLUME /build

ENTRYPOINT ["sh"]
CMD ["-c","make -C /build -f makefile ${EXTRA_MAKE_FLAGS} ${MAKE_TARGET}"]