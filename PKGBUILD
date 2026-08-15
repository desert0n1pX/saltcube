# Maintainer: @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
pkgname='saltcube-git'
pkgdesc='A CLI tool to help you remember passwords'
provides=('saltcube')
conflicts=('saltcube')
pkgver='1.2.0'
pkgrel=1
arch=('x86_64')

url='https://github.com/desert0n1pX/saltcube'
license=('GPL-3.0-only')

source=(git+https://github.com/desert0n1pX/saltcube.git)
sha256sums=('SKIP')

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
    cd saltcube
    git_ver=$(git describe --long --tags | awk 'BEGIN {FS="-"} ; { sub(/v/,"") sub(/-/,".r") sub(/-/,".")}1')
    
    if [ -z "${git_ver}" ]
    then
        commit=$(git log --oneline | awk '{if ( NR == 1) print $1}')
        salt_ver=$(cat include/TEXT_BLOCKS.hpp | head -n 100 | awk '{ if ($1 == "Version:") print $2 }')
        echo "${salt_ver}.g${commit}"
    else
        echo "${git_ver}"
    fi
}

build() {
    cd saltcube
    make saltcube
}

check() {
    cd saltcube
    ./saltcube --help > /dev/null
}

package() {
    cd saltcube
    make DESTDIR="$pkgdir" install
}   
