#!/bin/sh
# Copyright: 2019 redoste
#
#	This file is part of INEPLEM.
#
#	INEPLEM is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	INEPLEM is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with INEPLEM.  If not, see <https://www.gnu.org/licenses/>.
#

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