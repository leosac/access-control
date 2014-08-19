# Maintainer: Thibault 'Ryp' Schueller  <ryp.sqrt@gmail.com>

_gitname=leosac
pkgname=leosac-git
pkgver=0.3.2.r1.g8b62675
pkgrel=1
pkgdesc="Open source access control"
arch=('i686' 'x86_64' 'armv6h')
url="https://github.com/islog/leosac"
license=('AGPL3')
makedepends=('git cmake boost tclap')
conflicts=('leosac')
provides=('leosac')
install='leosac.install'
source=('git+https://github.com/islog/leosac.git'
    leosacd.service
    leosac.install)
sha512sums=('SKIP'
    '76a5c7408deb9ce3a48a2520620f37385e6e32697653f5b7766d3f91e22a9c7386ee8c268618c959fa11cac135c47e9d58edee185d34515456a71b21b93cb94c'
    '53a386f33283f7531d1ba0274d8520c8ed9e6bb04b6be45197e4e940ac33240ca27140450b52d8e8a2db3b04b99ce7b306dd095152e5a797c66b974968c3210a'
)

pkgver() {
    cd "$srcdir/$_gitname"
    git describe --long | sed -r 's/([^-]*-g)/r\1/;s/-/./g'
}

build() {
    mkdir -p build
    pushd build
    cmake "$srcdir/$_gitname"                   \
        -DCMAKE_BUILD_TYPE=Release              \
        -DCMAKE_INSTALL_PREFIX="$pkgdir/usr"    \
        -DLEOSAC_PLATFORM=None
    make
}

package() {
    pushd build
    # Install leosac
    make install
    # Install service file
    install -m 755 -d "$pkgdir/usr/lib/systemd/system"
    install -m 644 -t "$pkgdir/usr/lib/systemd/system"  \
        "$srcdir/leosacd.service"
    # Deamon configuration folder
    install -m 755 -d "$pkgdir/etc/leosac.d"
    install -m 644 -t "$pkgdir/etc/leosac.d"            \
        "$pkgdir/usr/share/leosac/cfg/empty/core.xml"   \
        "$pkgdir/usr/share/leosac/cfg/empty/hw.xml"
}

