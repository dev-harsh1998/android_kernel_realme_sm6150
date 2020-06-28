#ifndef __INCLUDE_LINUX_OPPO_CHECKS_H
#define __INCLUDE_LINUX_OPPO_CHECKS_H
static bool has_nfc;
static bool on_ambient_screen;
bool device_is_dozing(void);
bool device_has_nfc(void);
#endif