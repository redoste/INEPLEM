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

syslinux_tar="downloads/syslinux-6.03.tar.gz"
syslinux_uri="https://mirrors.edge.kernel.org/pub/linux/utils/boot/syslinux/syslinux-6.03.tar.gz"
syslinux_sha="250b9bd90945d361596a7a69943d0bdc5fc0c0917aa562609f8d3058a2c36b3a"

download_syslinux(){
	mkdir -p downloads/ 2>/dev/null
	echo "Downloading syslinux 6.03..."
	curl "$syslinux_uri" -o "$syslinux_tar"
	echo "Checking tar sha256..."
	check_sha256 "$syslinux_tar" "$syslinux_sha"
	echo "Extracting tar..."
	tar xf "$syslinux_tar" -C downloads/
	echo "DONE"
}

install_config_efi(){
	[ -f downloads/syslinux-6.03/README ] || fatal_error "Syslinux 6.03 have been not downloaded"
	mkdir -p drive/syslinux
	mkdir -p drive/EFI/BOOT
	echo "Installing syslinux EFI..."
	echo "INCLUDE /syslinux/syslinux.cfg" > drive/EFI/BOOT/syslinux.cfg
	cp downloads/syslinux-6.03/efi64/efi/syslinux.efi drive/EFI/BOOT/BOOTX64.EFI
	cp downloads/syslinux-6.03/efi64/com32/elflink/ldlinux/ldlinux.e64 drive/EFI/BOOT/
	echo "Copying syslinux config..."
	cp config/syslinux.config drive/syslinux/syslinux.cfg
	echo "DONE"
}

install_bootloader_fat(){
	[ -f downloads/syslinux-6.03/README ] || fatal_error "Syslinux 6.03 have been not downloaded"
	[ -f drive.fat ] || fatal_error "drive.fat not found"
	echo "Installing bootloader to drive.fat"
	./downloads/syslinux-6.03/bios/mtools/syslinux -d /syslinux/ drive.fat
	echo "DONE"
}

install_bootloader_mbr(){
	[ -f downloads/syslinux-6.03/README ] || fatal_error "Syslinux 6.03 have been not downloaded"
	echo "Installing bootloader to drive.img mbr"
	printf "\1" | cat downloads/syslinux-6.03/bios/mbr/altmbr.bin - | dd bs=440 count=1 conv=notrunc of=drive.img
	echo "DONE"
}

if [ $# -ne 1 ]; then
	usage
	exit 1
fi
case "$1" in
	"download-syslinux")
		download_syslinux
		;;
	"install-config-efi")
		install_config_efi
		;;
	"install-bootloader-fat")
		install_bootloader_fat
		;;
	"install-bootloader-mbr")
		install_bootloader_mbr
		;;
	*)
		usage
		exit 1
		;;
esac