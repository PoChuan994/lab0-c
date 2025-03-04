#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Insert node into queue */
static inline bool q_insert(struct list_head *node, const char *s)
{
    if (!node) /* input validation */
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false; /* memory allocation failure */

    int length = strlen(s) + 1;
    char *str = malloc(sizeof(char) * (length + 1));
    if (!str) { /* input validation and memory allocation detection */
        free(new);
        return false;
    }

    strncpy(str, s, length);
    new->value = str;
    list_add(&new->list, node);
    free(new);
    return true;
}

static inline element_t *q_remove(struct list_head *node,
                                  char *sp,
                                  size_t bufsize)
{
    if (!node || list_empty(node)) /* empty list check */
        return NULL;

    struct list_head *rmNode = node;
    element_t *element = list_entry(rmNode, element_t, list);

    list_del(rmNode);
    if (sp) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }
    return element;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *current, *safe;
    list_for_each_safe (current, safe, head) {
        element_t *release_pos = list_entry(current, element_t, list);
        q_release_element(release_pos);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    volatile char *dummy = s;
    (void) dummy;
    return q_insert(head, s);
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    volatile char *dummy = s;
    (void) dummy;
    return q_insert(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove(head, sp, bufsize);
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove(head->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) /* input validation */
        return false;
    struct list_head **indir = &(head->next);
    const struct list_head *fast = head->next;
    for (; fast != head && fast->next != head; fast = fast->next->next)
        indir = &(*indir)->next;
    element_t *element = list_entry(*indir, element_t, list);
    list_del(&element->list);
    q_release_element(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    volatile struct list_head *dummy = head;
    (void) dummy;

    if (!head || list_empty(head)) /* input validation */
        return false;

    bool flag = false;
    element_t *curr_entry = list_first_entry(head, element_t, list);
    element_t *next_entry;

    while (&curr_entry->list != head) {
        // list_for_each_entry_safe (curr_entry, next_entry, head, list) {
        next_entry = list_entry(curr_entry->list.next, element_t, list);

        while (&next_entry->list != head &&
               !strcmp(curr_entry->value, next_entry->value)) {
            list_del(&next_entry->list);
            q_release_element(next_entry);
            /* update next pointer */
            next_entry = list_entry(curr_entry->list.next, element_t, list);
            flag = true;
        }

        if (flag) { /*need remove current node*/
            list_del(&curr_entry->list);
            q_release_element(curr_entry);
            flag = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *current, *safe;
    list_for_each_safe (current, safe, head)
        list_move(current, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
