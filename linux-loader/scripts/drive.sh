#!/bin/sh

linuxloaderpath="$(realpath "$(dirname "$0")")/.."
cd "$linuxloaderpath" || (echo "Unable to cd to $linuxloaderpath" && exit 1)

. scripts/utils.sh

pack_fat(){
	echo "Packing drive/ to drive.fat"
	[ -f drive/initramfs.img ] || fatal_error "No initramfs.img in drive/"
	dd if=/dev/zero bs=512 count=129024 of=drive.fat
	mkfs.vfat -F 32 -n "INEPLEM" drive.fat
	mcopy -si drive.fat drive/* ::.
	echo "DONE"
}

pack_img(){
	echo "Adding partition table to drive.fat to make drive.img"
	[ -f drive.fat ] || fatal_error "No drive.fat file"
	echo "Adding 2048*512 bytes before drive.fat"
	(dd if=/dev/zero bs=512 count=2048 ; cat drive.fat) > drive.img 2>/dev/null || fatal_error "dd failed"
	echo "Creating dos partition table"
	# label: dos is not always supported
	#echo "label: dos" | sfdisk drive.img > /dev/null 2>&1 || fatal_error "\"label: dos\" via sfdisk failed"
	echo "Adding fat32 partition"
	echo ",,c;" | sfdisk drive.img > /dev/null 2>&1 || fatal_error "\",,c;\" via sfdisk failed"
	echo "DONE"
}

if [ $# -ne 1 ]; then
	usage
	exit 1
fi
case "$1" in
	"pack-fat")
		pack_fat
		;;
	"pack-img")
		pack_img
		;;
	*)
		usage
		exit 1
		;;
esac