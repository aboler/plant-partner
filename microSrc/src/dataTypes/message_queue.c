#include "message_queue.h"
#include <string.h>

void queue_init(m_queue_t *q)
{
    memset(q, 0, sizeof(*q));
}

bool queue_enqueue(m_queue_t *q, const char *topic, const char *data)
{
    if (queue_is_full(q))
    {
        return false;
    }

    mqtt_message_t *slot = &q->messages[q->tail];
    strncpy(slot->topic, topic, TOPIC_NAME - 1);
    slot->topic[TOPIC_NAME - 1] = '\0';
    strncpy(slot->data, data, MSG_SIZE - 1);
    slot->data[MSG_SIZE - 1] = '\0';

    q->tail = (q->tail + 1) % QUEUE_CAPACITY;
    q->count++;
    return true;
}

bool queue_dequeue(m_queue_t *q, mqtt_message_t *out)
{
    if (queue_is_empty(q))
    {
        return false;
    }

    *out = q->messages[q->head];
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->count--;
    return true;
}

bool queue_peek(const m_queue_t *q, mqtt_message_t *out)
{
    if (queue_is_empty(q))
    {
        return false;
    }
    *out = q->messages[q->head];
    return true;
}

bool queue_is_empty(const m_queue_t *q)
{
    return q->count == 0;
}

bool queue_is_full(const m_queue_t *q)
{
    return q->count == QUEUE_CAPACITY;
}

bool queue_is_half_full(const m_queue_t *q)
{
    return q->count >= (QUEUE_CAPACITY / 2);
}

int queue_count(const m_queue_t *q)
{
    return q->count;
}
