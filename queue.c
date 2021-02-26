#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Internal functions
 */

/*
 * Element creation utils.
 */
list_ele_t *new_element(char *s);

/*
 * Merge sort utils.
 */
list_ele_t *merge(list_ele_t *left, list_ele_t *right);
list_ele_t *merge_sort(list_ele_t *head);


/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));

    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }


    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;

    list_ele_t *new_head = q->head;

    while (new_head) {
        new_head = q->head->next;
        free(q->head->value);
        free(q->head);
        q->head = new_head;
    }

    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh = new_element(s);
    if (!newh)
        return false;

    newh->next = q->head;
    q->head = newh;
    q->size++;

    if (!q->tail)
        q->tail = newh;

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newt = new_element(s);
    if (!newt)
        return false;

    if (!q->tail)
        q->head = newt;
    else
        q->tail->next = newt;

    q->tail = newt;
    q->size++;

    return true;
}

/*
 * Create element.
 * Return new element if creation success.
 * Return NULL if error occur during creation.
 */
list_ele_t *new_element(char *s)
{
    /* construct new element */
    list_ele_t *element;
    element = malloc(sizeof(list_ele_t));

    if (!element)
        return NULL;

    element->next = NULL;
    element->value = NULL;

    /* copy string if source string exist */
    if (s) {
        size_t boundary_size = strlen(s) + 1;
        element->value = malloc(boundary_size * sizeof(char));
        if (element->value == NULL) {
            free(element);
            return NULL;
        }
        /* length + 1 for NULL charactor(stirng end) */
        strncpy(element->value, s, boundary_size);
    }

    return element;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q)
        return false;

    if (!q->head)
        return false;

    if (sp) {
        size_t boundary = bufsize - 1;
        strncpy(sp, q->head->value, boundary);
        sp[boundary] = '\0';
    }

    list_ele_t *head_to_remove = q->head;
    if (q->tail == q->head)
        q->tail = NULL;
    q->head = q->head->next;
    q->size--;

    free(head_to_remove->value);
    free(head_to_remove);

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q)
        return q->size;
    else
        return 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q && q->size > 1) {
        list_ele_t *new_head = q->head;
        list_ele_t *old_tail = q->head->next;
        list_ele_t *old_head = q->head->next->next;

        while (old_head) {
            old_tail->next = new_head;
            new_head = old_tail;
            old_tail = old_head;
            old_head = old_head->next;
        }

        /* update next element of head/tail */
        q->tail->next = new_head;
        q->head->next = NULL;

        /* change head and tail */
        list_ele_t *tmp = q->head;
        q->head = q->tail;
        q->tail = tmp;
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || q->size == 0)
        return;

    q->head = merge_sort(q->head);
    q->tail = q->head;

    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}

list_ele_t *merge_sort(list_ele_t *head)
{
    if (!head || !head->next)
        return head;

    list_ele_t *fast = head->next;
    list_ele_t *slow = head;

    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    fast = slow->next;
    slow->next = NULL;

    list_ele_t *left = merge_sort(head);
    list_ele_t *right = merge_sort(fast);

    return merge(left, right);
}

list_ele_t *merge(list_ele_t *left, list_ele_t *right)
{
    list_ele_t tmp;
    list_ele_t *tmp_ref = &tmp;

    tmp.next = NULL;

    while (left && right) {
        if (strcmp(left->value, right->value) <= 0) {
            tmp_ref->next = left;
            tmp_ref = tmp_ref->next;
            left = left->next;
        } else {
            tmp_ref->next = right;
            tmp_ref = tmp_ref->next;
            right = right->next;
        }
    }

    if (left) {
        tmp_ref->next = left;
    } else if (right) {
        tmp_ref->next = right;
    }

    return tmp.next;
}
