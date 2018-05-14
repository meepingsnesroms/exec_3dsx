#ifndef EXEC_3DSX_H
#define EXEC_3DSX_H

//since 3dsx programs are not guaranteed access to the OS, the 3dsx bootloader run by the exploit must run the next program
//your program must have no memory allocated and no extra threads running when this is called, these limits do not apply to exec_cia
int exec_3dsx(const char* path, const char* args);

#endif