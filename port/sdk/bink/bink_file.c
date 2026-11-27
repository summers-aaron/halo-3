// port/sdk/bink (both hosts): binkfile.obj's BinkFileIdle (retail 0x718320), the background read of RAD's file reader
// (s_bink_file, include/bink/bink_file.h; the decompiled half is tu/binkxenon/binkfile.cpp). The port's Bink reads its
// frames itself (bink_read.cpp: the kernel's file calls, or the movie in memory), so it never opens an s_bink_file
// and BinkFileBGControl, the only caller, is never reached; with no reader there is nothing to read ahead: 0 bytes.
struct s_bink_file;

int PORT_MS_ABI BinkFileIdle(struct s_bink_file *file)
{
    (void)file;
    return 0;
}
