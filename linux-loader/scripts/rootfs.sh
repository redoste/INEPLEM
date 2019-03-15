#!/bin/sh

linuxloaderpath="$(realpath "$(dirname "$0")")/.."
cd "$linuxloaderpath" || (echo "Unable to cd to $linuxloaderpath" && exit 1)

. scripts/utils.sh

alpine_tar="downloads/alpine-minirootfs-3.9.2-x86.tar.gz"
alpine_uri="http://dl-cdn.alpinelinux.org/alpine/v3.9/releases/x86/alpine-minirootfs-3.9.2-x86.tar.gz"
alpine_sha="17b623d2e1eca8fc930ab636595b0102186fab40b2335fe9442ad1596330d286"

download_alpine(){
	mkdir -p rootfs/ 2>/dev/null
	mkdir -p downloads/ 2>/dev/null
	rm -rf rootfs/* 2>/dev/null
	echo "Downloading Alpine Linux v3.9.2 minimal rootfs..."
	curl "$alpine_uri" -o "$alpine_tar"
	echo "Checking tar sha256..."
	check_sha256 "$alpine_tar" "$alpine_sha"
	echo "Extracting tar..."
	tar xf "$alpine_tar" -C rootfs/
	echo "DONE"
}

configure_install_pkg(){
	[ -f rootfs/etc/motd ] || fatal_error "Alpine Linux have been not downloaded and extracted to rootfs/ folder"
	echo "Installing chntpw and ntfs-3g packages"
	proot -r rootfs -b /etc/resolv.conf -w / -0 apk add chntpw ntfs-3g

	echo "Update /etc/inittab"
	cat << EOF > rootfs/etc/inittab
::sysinit:/bin/mount -a
tty1::respawn:/ineplem/INEPLEM.init
tty2::askfirst:/bin/login -f root

::ctrlaltdel:/sbin/reboot
EOF

	echo "Update /etc/fstab"
	cat << EOF > rootfs/etc/fstab
proc           /proc        proc     nosuid,noexec,nodev 0     0
sysfs          /sys         sysfs    nosuid,noexec,nodev 0     0
devpts         /dev/pts     devpts   gid=5,mode=620      0     0
tmpfs          /run         tmpfs    defaults            0     0
tmpfs          /tmp         tmpfs    defaults            0     0
devtmpfs       /dev         devtmpfs mode=0755,nosuid    0     0
EOF

	echo "Update /init"
	cat << EOF > rootfs/init
#!/bin/sh
exec /sbin/init
EOF
	chmod +x rootfs/init

	echo "Copy ineplem files"
	mkdir -p rootfs/ineplem
	cp config/ineplem.init rootfs/ineplem/INEPLEM.init || fatal_error "Unable to copy ineplem.init"
	chmod a+x rootfs/ineplem/INEPLEM.init
	cp config/ineplem.reg rootfs/ineplem/INEPLEM1.reg || fatal_error "Unable to copy ineplem.reg"
	cp ../windows-service/INEPLEM1.release.exe rootfs/ineplem/INEPLEM1.exe 2>/dev/null || fatal_error "Unable to copy INEPLEM1.exe. Did you build the windows-service ?"

	echo "Generating file checksum"
	sha1sum rootfs/ineplem/INEPLEM1.exe rootfs/ineplem/INEPLEM1.reg > rootfs/ineplem/INEPLEM.sha

	echo "DONE"
}

pack_initramfs(){
	echo "Packing initramfs"
	[ -f rootfs/etc/motd ] || fatal_error "Alpine Linux have been not downloaded and extracted to rootfs/ folder"
	[ -f rootfs/ineplem/INEPLEM.sha ] || fatal_error "INEPLEM not correctly installed to rootfs/"
	mkdir -p drive/ 2>/dev/null
	(
		cd rootfs || fatal_error "cd rootfs failed"
		find . | cpio -H newc -o | gzip -9 > ../drive/initramfs.img
	)
	echo "DONE"
}

if [ $# -ne 1 ]; then
	usage
	exit 1
fi
case "$1" in
	"download-alpine")
		download_alpine
		;;
	"configure-install-pkg")
		configure_install_pkg
		;;
	"pack-initramfs")
		pack_initramfs
		;;
	*)
		usage
		exit 1
		;;
esac
