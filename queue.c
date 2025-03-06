#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

void q_merge_two(struct list_head *first,
                 struct list_head *second,
                 bool descend);


/* Insert node into queue */
static inline bool q_insert(struct list_head *node, const char *s)
{
    if (!node) /* input validation */
        return false;

    /* allocate space for new item */
    element_t *new = malloc(sizeof(element_t));

    if (!new)
        return false; /* memory allocation failure */

    new->value = malloc(sizeof(char) * (strlen(s) + 1));
    /* input validation and memory allocation detection */
    if (!new->value) {
        free(new);
        return false;
    }

    new->value = strncpy(new->value, s, (strlen(s) + 1));
    list_add(&new->list, node);

    return true;
}

static inline element_t *q_remove(struct list_head *node,
                                  char *sp,
                                  size_t bufsize)
{
    element_t *element = list_entry(node, element_t, list);

    if (!element)
        return NULL;

    if (sp) {
        strncpy(sp, list_entry(node, element_t, list)->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(node);
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
    if (!head || list_empty(head))
        return NULL;

    element_t *element = list_entry(head->next, element_t, list);

    if (!element)
        return NULL;
    if (sp) {
        strncpy(sp, list_entry(head->next, element_t, list)->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del_init(head->next);
    return element;
    // return q_remove(head->next, sp, bufsize);
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

    element_t *curr_entry = list_first_entry(head, element_t, list);
    while (&curr_entry->list != head) {
        struct list_head *next = curr_entry->list.next;
        element_t *next_entry = list_entry(next, element_t, list);

        /* check if there is duplicate element */
        bool check = false;
        while (&next_entry->list != head &&
               !strcmp(curr_entry->value, next_entry->value)) {
            struct list_head *tmp = next_entry->list.next;
            list_del(&next_entry->list);
            q_release_element(next_entry);
            next_entry = list_entry(tmp, element_t, list);
            check = true;
        }

        if (check) {
            struct list_head *tmp = curr_entry->list.next;
            list_del(&curr_entry->list);
            q_release_element(curr_entry);
            curr_entry = list_entry(tmp, element_t, list);
        } else {
            curr_entry = next_entry;
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

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || head->next == head || head->prev == head->next)
        return;
    struct list_head *slow = head;
    const struct list_head *fast = head->next;
    for (; fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;
    struct list_head left;
    list_cut_position(&left, head, slow);
    q_sort(&left, descend);
    q_sort(head, descend);
    q_merge_two(head, &left, descend);
}

void q_merge_two(struct list_head *first,
                 struct list_head *second,
                 bool descend)
{
    if (!first || !second)
        return;

    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);
    while (!list_empty(first) && !list_empty(second)) {
        element_t *first_top = list_first_entry(first, element_t, list);
        element_t *second_top = list_first_entry(second, element_t, list);
        const char *first_str = first_top->value,
                   *second_str = second_top->value;
        bool check;
        if (descend)
            check = strcmp(first_str, second_str) > 0;
        else
            check = strcmp(first_str, second_str) < 0;
        element_t *add_first = check ? first_top : second_top;
        list_move_tail(&add_first->list, &tmp);
    }
    list_splice_tail_init(first, &tmp);
    list_splice_tail_init(second, &tmp);
    list_splice(&tmp, first);
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
