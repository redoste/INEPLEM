linux-loader building documentation
===================================

Commands
--------

```bash
./scripts/rootfs.sh download-alpine # Download Alpine Linux minimal rootfs
./scripts/rootfs.sh configure-install-pkg # Install required packages and configurations to the rootfs
./scripts/rootfs.sh pack-initramfs # Pack the rootfs to an initramfs to the drive/ folder

./scripts/drive.sh pack-fat # Pack the content of the drive/ folder to drive.fat FAT32 image
./scripts/drive.sh pack-img # Add a partition table to drive.fat to produce drive.img

./scripts/syslinux.sh download-syslinux # Download syslinux files in the downloads/ folder
./scripts/syslinux.sh install-config-efi # Install EFI binaries and configuration of syslinux to the drive/ folder
./scripts/syslinux.sh install-bootloader-fat # Install syslinux (2nd stage) BIOS bootloader to drive.fat
./scripts/syslinux.sh install-bootloader-mbr # Install syslinux (1st stage) BIOS bootloader to drive.img

./scripts/utils.sh # It is used by other scripts

./scripts/linux.sh # Export variables about the Linux kernel (see Kernel section)
```

Kernel
------

[Linux Kernel](https://www.kernel.org/) must be build manually.
The version and download URL of the required kernel can be found in the `scripts/linux.sh` file.
Here is a quick example of how to use it
```bash
. scripts/linux.sh
cd downloads/
curl -O $INEPLEM_LINUX_URI
tar xvf linux-$INEPLEM_LINUX_VERSION.tar.gz
```
Then simply apply the configuration and start the build
```bash
cd linux-$INEPLEM_LINUX_VERSION
cp ../../config/linux.defconfig arch/x86/ineplem_defconfig
make ARCH=x86 ineplem_defconfig
make bzImage -j 4 # Replace 4 with the number of threads you want
```
Now you can simply copy your output binary to the `drive/` folder
```bash
mkdir -p ../../drive
cp arch/x86/boot/bzImage ../../drive
```

Final Image
-----------

Now that you have built the kernel you can build the rootfs and the final image. We recommend you to launch scripts in this order but feel free to edit them.
Just see the `Commands` section if you want to understand what each commands do.
```bash
./scripts/rootfs.sh download-alpine
./scripts/rootfs.sh configure-install-pkg
./scripts/rootfs.sh pack-initramfs
./scripts/syslinux.sh download-syslinux
./scripts/syslinux.sh install-config-efi
# Now copy files you want to add to the drive in the drive/ folder
./scripts/drive.sh pack-fat
./scripts/syslinux.sh install-bootloader-fat
./scripts/drive.sh pack-img
./scripts/syslinux.sh install-bootloader-mbr
```
