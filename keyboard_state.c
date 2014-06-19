#include "keyboard_state.h"
#include "inih/ini.h"
#include <stdio.h>

static void add_head(struct node *root, int code, int opt, int prop)
{
	struct key_desc *key = malloc(sizeof(*key));
	key->key_code = code;
	key->key_opt = opt;
	key->properties = prop;

	struct node *node = malloc(sizeof(*node));
	node->key = key;
	node->next = root->next;
	root->next = node;
}

static int ini_handler(void* user, const char* section,
			const char* name, const char* value)
{
	struct keyboard_state *ks = user;
	int code = 0, opt = 0, prop = 0, ext = 0;

	(void)name;

	if (strcmp(section, "left_hand_modifiers") == 0)
		prop = KS_PROP_LEFT | KS_PROP_MODIFIER;
	else if (strcmp(section, "right_hand_modifiers") == 0)
		prop = KS_PROP_RIGHT | KS_PROP_MODIFIER;
	else if (strcmp(section, "left_hand_keys") == 0)
		prop = KS_PROP_LEFT;
	else if (strcmp(section, "right_hand_keys") == 0)
		prop = KS_PROP_RIGHT;
	else
		return 0;

	sscanf(value, "%x%d", &code, &ext);
	if (ext)
		opt |= KS_OPT_EXTENDED;

	add_head(&ks->released, code, opt, prop);
	return 0;
}

void ks_init(struct keyboard_state *ks, const char *ini_file)
{
        // cycle lists
	ks->pressed.next = &ks->pressed;
	ks->released.next = &ks->released;

	// read keys descriptions
	ini_parse(ini_file, ini_handler, ks);
}

static void clear_list(struct node *root)
{
	while (root->next != root) {
		struct node *node = root->next;
		root->next = root->next->next;
		if (node->key)
			free(node->key);
		free(node);
	}
}

void ks_release(struct keyboard_state *ks)
{
	clear_list(&ks->pressed);
	clear_list(&ks->released);
}

// returns NULL or previous item
static struct node *find(struct node *root, int code, int opt)
{
	struct node *p = root;
	while (p-> next != root && (p->next->key->key_code != code ||
				    p->next->key->key_opt != opt ))
		p = p->next;
	return p-> next == root ? NULL : p;
}

// removes a->next from list and insert after b
static void move_beetween_lists(struct node *a, struct node *b)
{
	struct node *node = a->next;
	a->next = node->next;
	node->next = b->next;
	b->next = node;
}


void ks_handle_event(struct keyboard_state *ks, KBDLLHOOKSTRUCT *event)
{
	struct node *pr, *re;
	int code = event->scanCode;
	int opt = 0;
        bool up = LLKHF_UP & event->flags;

        if (LLKHF_EXTENDED & event->flags)
		opt |= KS_OPT_EXTENDED;

	pr = find(&ks->pressed, code, opt);
	re = find(&ks->released, code, opt);
	if (!pr && !re) {
		add_head(&ks->released, code, opt, 0);
		re = &ks->released;
	}

	if (up && pr)
		move_beetween_lists(pr, &ks->released);
	else if (!up && re)
		move_beetween_lists(re, &ks->pressed);
}

bool ks_is_alarm_needed(struct keyboard_state *ks)
{
	struct node *p;
	int prop = KS_PROP_LEFT | KS_PROP_RIGHT;
	int mod = 0;
	int n = 0;
	for (p = ks->pressed.next; p != &ks->pressed; p = p->next) {
		prop &= p->key->properties;
		mod |= p->key->properties & KS_PROP_MODIFIER;
		n++;
	}
	return (n == 2 && mod && prop);
}
