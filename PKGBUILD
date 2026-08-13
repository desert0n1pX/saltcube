# Maintainer: @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
pkgname='saltcube'
pkgdesc='A CLI tool to help you remember passwords'
conflicts=('saltcube-git')

pkgver='1.1.0'
pkgrel=1
arch=('x86_64')

url='https://github.com/desert0n1pX/saltcube'
license=('GPL-3.0-only')

source=(https://github.com/desert0n1pX/saltcube/archive/refs/tags/v1.1.0.tar.gz)
sha256sums=('f0894967fe8816f3cedf3419e5a9ee3bfc349ab28ff4df69e276be47001b4854')

depends=(   'libcrypto.so'
            'libgcc_s.so'
            'libsodium.so'
            'libssl.so'
            'libstdc++.so')

makedepends=(   'binutils'
                'gcc'
                'git'
                'make')

pkgver() {
    cd saltcube-1.1.0
    salt_ver=$(cat include/TEXT_BLOCKS.hpp | head -n 100 | awk '{ if ($1 == "Version:") print $2 }')
    echo "${salt_ver}"
}

build() {
    cd saltcube-1.1.0
    make saltcube
}

check() {
    cd saltcube-1.1.0
    ./saltcube --help > /dev/null
}

package() {
    cd saltcube-1.1.0
    make DESTDIR="$pkgdir" install
}   
