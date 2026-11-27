// port/sdk/bink (both hosts): the XMP (Xbox Music Player) calls bink_playback.cpp makes around a movie: the Xbox lets
// a player's own music play over a title and the title pauses it for its movies. The port has no system music
// player, so there is nothing to override or restore; both succeed (ERROR_SUCCESS), as with no music playing.
// (This file used to be bink_null.c, which also held a headless Bink; the port's Bink is bink_read.cpp.)

unsigned long PORT_MS_ABI XMPOverrideBackgroundMusic(void) { return 0; }
unsigned long PORT_MS_ABI XMPRestoreBackgroundMusic(void) { return 0; }
