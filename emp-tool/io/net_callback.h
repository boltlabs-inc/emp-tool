#ifndef NET_CALLBACK_H
#define NET_CALLBACK_H

struct Receive_return {
    char* r0; /* msg */
    int r1; /* length */
    char* r2; /* errStr */
};

typedef Receive_return (*cb_receive)(void*, int);
typedef char* (*cb_send)(void*, int, void*, int);
typedef void* (*cb_duplicate)(void*);

#endif // NET_CALLBACK_H
