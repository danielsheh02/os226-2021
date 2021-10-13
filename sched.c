#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sched.h"

static int time = 0;

struct sched_task {
    void (*entrypoint)(void *aspace);

    void *aspace;
    int priority;
    int deadline;
    int start;
};

struct sched_node {
    struct sched_task task;
    struct sched_node *next;
};

static struct sched_node *sched_head = NULL;

static struct sched_task *cur_task = NULL;

typedef int (*policy_fun)(struct sched_task *, struct sched_task *);

void add_new_task(struct sched_task task) {
    struct sched_node *node = malloc(sizeof(struct sched_node));
    if (node == NULL) {
        printf("Failed to allocate memory");
        return;
    }
    node->task = task;
    node->next = sched_head;
    sched_head = node;
}

void sched_new(void (*entrypoint)(void *aspace),
               void *aspace,
               int priority,
               int deadline) {
    struct sched_task task = {entrypoint, aspace, priority, deadline, 0};
    add_new_task(task);
}


void sched_cont(void (*entrypoint)(void *aspace),
                void *aspace,
                int timeout) {
    if (cur_task == NULL) {
        return;
    }
    struct sched_task task = {entrypoint, aspace, cur_task->priority, cur_task->deadline, time + timeout};
    add_new_task(task);
}

void sched_time_elapsed(unsigned amount) {
    time += amount;

}

void sched_run_task(policy_fun policy) {
    while (sched_head != NULL) {
        struct sched_node *previous = NULL, *node = NULL;
        for (struct sched_node *i = NULL, *cur_node = sched_head;
             cur_node != NULL;
             cur_node = cur_node->next) {
            if ((node == NULL && cur_node->task.start <= time) ||
                (node != NULL && policy(&(node->task), &(cur_node->task)))) {
                previous = i;
                node = cur_node;
            }
            i = cur_node;
        }
        if (node != NULL) {
            if (previous != NULL) {
                previous->next = node->next;
            } else {
                sched_head = node->next;
            }
            cur_task = &(node->task);
            cur_task->entrypoint(cur_task->aspace);
            cur_task = NULL;
            free(node);
        }
    }
}

static int policy_fifo(struct sched_task *task1, struct sched_task *task2) {
    return task2->start <= time;
}

static int policy_prio(struct sched_task *task1, struct sched_task *task2) {
    return task2->start <= time && task2->priority >= task1->priority;
}

static int policy_deadline(struct sched_task *task1, struct sched_task *task2) {
    return task2->start <= time &&
           ((0 < task2->deadline && (task2->deadline < task1->deadline || task1->deadline <= 0)) ||
            (task2->deadline == task1->deadline));
}

void sched_run(enum policy policy) {
    switch (policy) {
        case POLICY_FIFO:
            sched_run_task(policy_fifo);
            break;
        case POLICY_PRIO:
            sched_run_task(policy_prio);
            break;
        case POLICY_DEADLINE:
            sched_run_task(policy_deadline);
            break;
        default:
            fprintf(stderr, "Unknown policy\n");
            break;
    }
}
