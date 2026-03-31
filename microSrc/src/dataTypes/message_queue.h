#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

// Match sizes defined in mqtt.h; guards prevent redefinition if both are included
#ifndef MSG_SIZE
#define MSG_SIZE    256
#endif
#ifndef TOPIC_NAME
#define TOPIC_NAME  64
#endif

#define QUEUE_CAPACITY  8   // Must be >= 2; increase if messages arrive faster than they're consumed

typedef struct {
    char topic[TOPIC_NAME];
    char data[MSG_SIZE];
} mqtt_message_t;

typedef struct {
    mqtt_message_t messages[QUEUE_CAPACITY];
    int head;   // index of next dequeue
    int tail;   // index of next enqueue
    int count;  // number of messages currently stored
} m_queue_t;

// Initialize (zero out) the queue — call once before use
void queue_init(m_queue_t *q);

// Add a message. Returns false and drops the message if the queue is full.
bool queue_enqueue(m_queue_t *q, const char *topic, const char *data);

// Remove the oldest message into *out. Returns false if the queue is empty.
bool queue_dequeue(m_queue_t *q, mqtt_message_t *out);

// Read the oldest message without removing it. Returns false if the queue is empty.
bool queue_peek(const m_queue_t *q, mqtt_message_t *out);

bool queue_is_empty(const m_queue_t *q);
bool queue_is_full(const m_queue_t *q);
bool queue_is_half_full(const m_queue_t *q);  // true when count >= QUEUE_CAPACITY / 2

// Number of messages currently in the queue
int  queue_count(const m_queue_t *q);

#endif
