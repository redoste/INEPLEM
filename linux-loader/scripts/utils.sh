#!/bin/sh

usage(){
	echo "Usage: $0 [command]"
	echo "See README.md for more information"
}

fatal_error(){
	printf "\e[31m\e[1mERROR:\e[0m "
	echo "$1"
	exit 1
}

check_dependency(){
	hash "$1" 2>/dev/null || fatal_error "$1 is required ($2)"
}

check_dependecies(){
	check_dependency "mcopy" "It is required for packing the drive/ folder in drive.fat"
	check_dependency "proot" "It is required to chroot in rootfs without root rights"
	check_dependency "mkfs.vfat" "It is required to format drive.fat"
	check_dependency "curl" "It is required to download file from the Internet via HTTP"
	check_dependency "cpio" "It is required to pack the initramfs"
	check_dependency "find" "It is required to pack the initramfs"
	check_dependency "gzip" "It is required to compress the initramfs"
	check_dependency "dd" "It is required to make drive.fat and drive.img"
	check_dependency "sfdisk" "It is required to make drive.img partition table"
	check_dependency "sha256sum" "It is required to check file integrity"
	check_dependency "sha1sum" "It is required to check file integrity"
}

check_sha256(){
	file_sha=$(sha256sum "$1" | cut -d " " -f 1)
	if [ "$file_sha" != "$2" ]; then
		fatal_error "File integrity check failed for $0"
	fi
}

if [ "$_" = "$0" ]; then
	fatal_error "This is script is not supposed to be called"
fi
check_dependecies
