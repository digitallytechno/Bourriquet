/*
** lists.h
*/

#ifndef LISTS_H
#define LISTS_H

struct node
{
	struct node *next;
	struct node *prev;
	struct list *list; /* To which list belongs this node? */
};

struct list
{
	struct node *first;
	struct node *last;
};

/* Prototypes */
void list_init(struct list *list);
void list_insert(struct list *list, struct node *newnode, struct node *prednode);
void list_insert_tail(struct list *list, struct node *newnode);
struct node *list_remove_tail(struct list *list);
struct node *list_find(struct list *list, int num);
int list_length(struct list *list);
int node_index(struct node *node);
void node_remove(struct node *node);

struct node *list_first(struct list *list);
struct node *list_last(struct list *last);
struct node *node_next(struct node *node);
struct node *node_prev(struct node *node);
struct list *node_list(struct node *node);

#endif  /* LISTS_H */

