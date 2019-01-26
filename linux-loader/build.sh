#!/usr/bin/env bash
SCRIPT_FOLDER=$(realpath "$(dirname "$0")")
cd $SCRIPT_FOLDER

mkdir -p downloads
mkdir -p drive
mkdir -p rootfs
mkdir -p sources
touch drive.img

source config/busybox
source config/chntpw
source config/glibc
source config/ineplem
source config/linux
source config/ntfs-3g
source config/packing
source config/syslinux

function usage(){
	echo "$0 usage: $0 [action 1] [action 2] ... [action n]"
	echo ""
	echo "Actions List:"
	echo ""
	echo "download_[busybox|chntpw|glibc|linux|ntfs-3g] : Download source code of the respective software from the internet"
	echo "build_[busybox|chntpw|glibc|linux|ntfs-3g] : Build respective software"
	echo "install_[busybox|chtnpw|glibc|ntfs-3g] : Install respective software to rootfs/"
	echo "install_ineplem : Install INEPLEM scripts and files to rootfs/"
	echo ""
	echo "install_linux : Install linux kernel to drive/"
	echo "pack_initramfs : Pack rootfs/ to an initramfs and copy it in drive/"
	echo "install_syslinux : Install syslinux config to drive/"
	echo ""
	echo "pack_drive : Pack drive/ in an FAT32 partition to drive.img"
	echo "install_bootloader: Install syslinux bootloader to drive.img"
	echo ""
	echo "You can make a complete build with the following command:"
	echo "$0 download_busybox download_chntpw download_glibc download_linux download_ntfs-3g\\"
	echo " build_busybox build_chntpw build_glibc build_linux build_ntfs-3g\\"
	echo " install_busybox install_chntpw install_glibc install_ntfs-3g\\"
	echo " install_ineplem pack_initramfs install_linux\\"
	echo " install_syslinux pack_drive install_bootloader"
	exit 0
}

download_busybox=0
download_chntpw=0
download_glibc=0
download_linux=0
download_ntfs3g=0

build_busybox=0
build_chntpw=0
build_glibc=0
build_linux=0
build_ntfs3g=0

install_busybox=0
install_chntpw=0
install_glibc=0
install_ntfs3g=0
install_linux=0
install_ineplem=0

pack_initramfs=0
pack_drive=0

install_syslinux=0
install_bootloader=0

if [ $# -lt 1 ]; then
	usage
	exit 1
fi

for a in "$@"; do
	case "$a" in
		"download_busybox")
			download_busybox=1
			;;
		"download_chntpw")
			download_chntpw=1
			;;
		"download_glibc")
			download_glibc=1
			;;
		"download_linux")
			download_linux=1
			;;
		"download_ntfs-3g")
			download_ntfs3g=1
			;;
		"build_busybox")
			build_busybox=1
			;;
		"build_chntpw")
			build_chntpw=1
			;;
		"build_glibc")
			build_glibc=1
			;;
		"build_linux")
			build_linux=1
			;;
		"build_ntfs-3g")
			build_ntfs3g=1
			;;
		"install_busybox")
			install_busybox=1
			;;
		"install_chntpw")
			install_chntpw=1
			;;
		"install_glibc")
			install_glibc=1
			;;
		"install_ntfs-3g")
			install_ntfs3g=1
			;;
		"install_linux")
			install_linux=1
			;;
		"install_ineplem")
			install_ineplem=1
			;;
		"pack_initramfs")
			pack_initramfs=1
			;;
		"pack_drive")
			pack_drive=1
			;;
		"install_syslinux")
			install_syslinux=1
			;;
		"install_bootloader")
			install_bootloader=1
			;;
		*)
			usage
			exit 0
			;;
	esac
done

if [ $download_busybox -eq 1 ]; then busybox_download; fi
if [ $download_chntpw -eq 1 ]; then chntpw_download; fi
if [ $download_glibc -eq 1 ]; then glibc_download; fi
if [ $download_linux -eq 1 ]; then : ; fi
if [ $download_ntfs3g -eq 1 ]; then ntfs3g_download; fi
if [ $build_busybox -eq 1 ]; then busybox_build; fi
if [ $build_chntpw -eq 1 ]; then chntpw_build; fi
if [ $build_glibc -eq 1 ]; then glibc_build; fi
if [ $build_linux -eq 1 ]; then : ; fi
if [ $build_ntfs3g -eq 1 ]; then ntfs3g_build; fi
if [ $install_busybox -eq 1 ]; then busybox_install; fi
if [ $install_chntpw -eq 1 ]; then chntpw_install; fi
if [ $install_glibc -eq 1 ]; then glibc_install; fi
if [ $install_ntfs3g -eq 1 ]; then ntfs3g_intall; fi
if [ $install_ineplem -eq 1 ]; then : ; fi
if [ $install_linux -eq 1 ]; then : ; fi
if [ $pack_initramfs -eq 1 ]; then : ; fi
if [ $install_syslinux -eq 1 ]; then : ; fi
if [ $pack_drive -eq 1 ]; then : ; fi
if [ $install_bootloader -eq 1 ]; then : ; fi
