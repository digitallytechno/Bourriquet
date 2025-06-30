/***************************************************************************
 Bourriquet MorphOS
 	2024-2025 digitally
***************************************************************************/
#include <stdlib.h>

#include "Utilities.h"
#include "Lists.h"
#include "Debug.h"

/******************************************************************
 Initalizes a list
*******************************************************************/
void list_init(struct list *list)
{
  list->first = list->last = NULL;
}

/******************************************************************
 Inserts a node into the list
*******************************************************************/
void list_insert(struct list *list, struct node *newnode, struct node *prednode)
{
  	if(list->last == prednode)
  		{
    		// Insert the node at the list's end
    		list_insert_tail(list, newnode);
    		return;
  		}
  	newnode->list = list;
    if(prednode)
  		{
    		/* Insert the list between two nodes */
    		prednode->next->prev = newnode;
    		newnode->next = prednode->next;
    		newnode->prev = prednode;
    		prednode->next = newnode;
    		return;
  		}
  	list->first->prev = newnode;
  	newnode->next = list->first;
  	newnode->prev = NULL;
  	list->first = newnode;
}

/******************************************************************
 Inserts a node into the list at the last position
*******************************************************************/
void list_insert_tail(struct list *list, struct node *newnode)
{
  	newnode->list = list;
  	if(!list->first)
  		{
    		/* List was empty */
    		newnode->next = newnode->prev = NULL;
    		list->first = list->last = newnode;
    		return;
  		}
  	list->last->next = newnode;
  	newnode->next = NULL;
  	newnode->prev = list->last;
  	list->last = newnode;
}

/******************************************************************
 Removes the last node and returns it
*******************************************************************/
struct node *list_remove_tail(struct list *list)
{
  	struct node *node = list_last(list);
  
	if(node) node_remove(node);
   	return(node);
}

/******************************************************************
 Returns the first entry
*******************************************************************/
struct node *list_first(struct list *list)
{
  	if(!list) D(DBF_NET, "list_first() called with NULL pointer!");
  	return(list ? list->first : NULL);
}

/******************************************************************
 Returns the last entry
*******************************************************************/
struct node *list_last(struct list *list)
{
 	if(!list) D(DBF_NET, "list_last() called with NULL pointer!");
 	return(list ? list->last : NULL);
}

/******************************************************************
 Returns the n'th entry
*******************************************************************/
struct node *list_find(struct list *list, int num)
{
  	struct node *n = list_first(list);
  
	while(n)
  		{
    		if(!num) return n;
    		num--;
    		n = node_next(n);
  		}
  	return(n);
}

/******************************************************************
 Returns the length of the list
*******************************************************************/
int list_length(struct list *list)
{
  	int len = 0;
  	struct node *node = list_first(list);
  
	while(node)
  		{
    		len++;
    		node = node_next(node);
  		}
  	return(len);
}
/******************************************************************
 Returns the node's succersor
*******************************************************************/
struct node *node_next(struct node *node)
{
    if(!node) D(DBF_NET, "node_next() called with NULL pointer!");
  	return(node ? node->next : NULL);
}
/******************************************************************
 Returns the node's pred
*******************************************************************/
struct node *node_prev(struct node *node)
{
 	if(!node) D(DBF_NET, "node_prev() called with NULL pointer!");
  	return(node ? node->prev : NULL);
}
/******************************************************************
 Returns the list where the node belongs to
 (or NULL if not linked to any list)
*******************************************************************/
struct list *node_list(struct node *node)
{
	if(!node) D(DBF_NET, "node_list() called with NULL pointer!");
  	return(node ? node->list : NULL);
}
/******************************************************************
 Returns the index of the node, -1 if no node
*******************************************************************/
int node_index(struct node *node)
{
  	int index = -1;
  	
	while(node)
  		{
    		node = node_prev(node);
    		index++;
  		}
  	return(index);
}

/******************************************************************
 Removes the entry from the list
*******************************************************************/
void node_remove(struct node *node)
{
  	struct  list *liste = node->list;

  	node->list = NULL;
  	if(liste->first == node)
  		{
    		/* The node was the first one */
    		if(liste->last == node)
    			{
      				/* The node was the only one */
      				liste->first = liste->last = NULL;
      				return;
    			}
    		liste->first = node->next;
    		liste->first->prev = NULL;
    		return;
  		}
  	if(liste->last == node)
  		{
    		/* The node was the last one */
    		liste->last = node->prev;
    		liste->last->next = NULL;
    		return;
  		}
  	node->next->prev = node->prev;
  	node->prev->next = node->next;
}