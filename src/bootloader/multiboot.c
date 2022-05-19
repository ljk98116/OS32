#include "../../libs/multiboot.h"

//initilize global_multiboot_header
struct MultiBootHeader __attribute__((section(".multiboot"))) glb_mboot_hdr={
    MBOOT_MAGIC,
    MBOOT_FLAG_CONFIG(1,1,0,0),
    -(MBOOT_MAGIC + MBOOT_FLAG_CONFIG(1,1,0,0)),
    0,0,0,0,0,
    0,0,0,0
};

