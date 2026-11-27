#ifndef SHELL_UTILITY_DRIVE_H
#define SHELL_UTILITY_DRIVE_H

extern "C" {
#ifndef XMOUNTUTILITYDRIVE_FORMAT_ALL
#define XMOUNTUTILITYDRIVE_FORMAT_ALL 0x0000000f
unsigned long __stdcall XMountUtilityDriveEx(unsigned long dwFlags, unsigned long dwBytesPerCluster, unsigned long dwFileCacheSize);
unsigned long __stdcall XUnmountUtilityDrive(void);
#endif
unsigned long XMountUtilityDriveUseStfc(unsigned long dwFlags);

extern bool g_utility_drive_mounted;
extern bool g_utility_drive_was_formatted;
}

#endif // SHELL_UTILITY_DRIVE_H
