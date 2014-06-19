#ifndef _KEYBOARD_STATE_H_
#define _KEYBOARD_STATE_H_

#include <stdbool.h>
#include <windows.h>

#define KS_OPT_EXTENDED 1

#define KS_PROP_LEFT 1
#define KS_PROP_RIGHT 2
#define KS_PROP_MODIFIER 4

struct key_desc {
	int key_code;
	int key_opt;
	int properties;
};

struct node {
	struct key_desc *key;
	struct node *next;
};

struct keyboard_state {
	struct node pressed;
	struct node released;
};

void ks_init(struct keyboard_state *ks, const char *ini_file);
void ks_release(struct keyboard_state *ks);

void ks_handle_event(struct keyboard_state *ks, KBDLLHOOKSTRUCT *event);
bool ks_is_alarm_needed(struct keyboard_state *ks);

#endif /* _KEYBOARD_STATE_H_ */
