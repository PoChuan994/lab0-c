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

static void q_restruct(struct list_head *head)
{
    struct list_head *curr = head, *nxt = curr->next;
    while (nxt) {
        nxt->prev = curr;
        curr = nxt;
        nxt = nxt->next;
    }
    curr->next = head;
    head->prev = curr;
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

    if (!head)
        return false;
    /* malloc space for new entry item */
    element_t *new = malloc(sizeof(element_t));

    if (!new)
        return false;

    new->value = malloc(sizeof(char) * strlen(s) + 1);
    if (!new->value) {
        free(new);
        return false;
    }

    new->value = strncpy(new->value, s, strlen(s) + 1);
    list_add(&new->list, head);

    return true;
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
    if (!head)
        return;

    struct list_head *first = head->next;
    struct list_head *second = first->next;
    for (; first != head && second != head;
         first = first->next, second = first->next) {
        list_del(first);
        list_add(first, second);
    }
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
    if (!head || list_empty(head))
        return;

    int cnt = 0;
    struct list_head *sub_head = head->next, *next_head = NULL,
                     *old_tail = head;

    /* cut the list to be singly-linked list */
    head->prev->next = NULL;

    for (struct list_head *sub_tail = head->next; sub_tail;
         sub_tail = sub_tail->next) {
        if (++cnt == k) {
            next_head = sub_tail->next;
            sub_tail->next = old_tail;
            q_reverse(old_tail);
            /* old node connects to the head of new list */
            old_tail->next = sub_tail;
            /* the new list connect to the next node */
            sub_head->next = next_head;
            old_tail = sub_tail = sub_head;
            sub_head = next_head;
            cnt = 0;
        }
    }
    /* restructure_list(head) */
    q_restruct(head);
}

/* meger each elements of queue in ascending/descending order */
void merge(struct list_head *head,
           struct list_head *left,
           struct list_head *right,
           bool descend)
{
    while (!list_empty(left) && !list_empty(right)) {
        const element_t *l = list_entry(left->next, element_t, list);
        const element_t *r = list_entry(right->next, element_t, list);
        if (((descend * 2) - 1) * strcmp(l->value, r->value) > 0)
            list_move_tail(left->next, head);
        else
            list_move_tail(right->next, head);
    }
    if (list_empty(head))
        list_splice_tail(right, head);
    else
        list_splice_tail(left, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *slow = head->next, *fast = head->next->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    struct list_head l, r;
    INIT_LIST_HEAD(&l);
    INIT_LIST_HEAD(&r);

    list_cut_position(&l, head, slow);
    list_splice_init(head, &r);

    q_sort(&l, descend);
    q_sort(&r, descend);

    merge(head, &l, &r, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    element_t *curr = NULL, *prev = NULL;
    const element_t *target;
    struct list_head *pos = NULL;

    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    list_for_each_entry (curr, head, list) {
        /* Release the element in the next round */
        if (prev) {
            q_release_element(prev);
            prev = NULL;
        }

        /* check right side and find if there is greater value */
        if (curr->list.next)
            pos = curr->list.next;
        for (; pos != head; pos = pos->next) {
            target = list_entry(pos, element_t, list);
            if (strcmp(curr->value, target->value) > 0) {
                list_del(&curr->list);
                prev = curr;
                break;
            }
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    element_t *curr = NULL, *prev = NULL;
    const element_t *target;
    struct list_head *pos = NULL;

    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    list_for_each_entry (curr, head, list) {
        /* Release the element in the next round */
        if (prev) {
            q_release_element(prev);
            prev = NULL;
        }


        /* check right side and find if there is greater value */
        if (curr->list.next)
            pos = curr->list.next;
        for (; pos != head; pos = pos->next) {
            target = list_entry(pos, element_t, list);
            if (strcmp(curr->value, target->value) < 0) {
                list_del(&curr->list);
                prev = curr;
                break;
            }
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);

    queue_contex_t *first, *target = NULL;
    first = list_first_entry(head, queue_contex_t, chain);

    /* move each target's queue to first context's queue */
    list_for_each_entry (target, head->next, chain) {
        if (target->id == first->id)
            break;
        list_splice_tail_init(target->q, first->q);
    }
    q_sort(first->q, descend);
    head = first->q;

    return q_size(head);
}
