#ifndef LND_NETWORK_IO_CHANNEL
#define LND_NETWORK_IO_CHANNEL

#include "emp-tool/io/io_channel.h"

struct Receive_return {
    char* r0; /* msg */
    int r1; /* length */
    char* r2; /* errStr */
};

typedef Receive_return (*cb_receive)(void*);
typedef char* (*cb_send)(void*, int, void*);

namespace emp {

/** @addtogroup IO
  @{
 */
    class LndNetIO: public IOChannel<LndNetIO> {
    public:
        bool is_server;
        void* peer_ptr;
        cb_receive recv;
        cb_send send;
        uint64_t counter = 0;
        char * buffer = nullptr;
        int buffer_ptr = 0;
        int buffer_cap = NETWORK_BUFFER_SIZE;
        bool has_send = false;
        LndNetIO(void* peer_ptr, cb_send send, cb_receive recv, bool is_server, bool quiet = false) {
            this->is_server = is_server;
            this->peer_ptr = peer_ptr;
            this->recv = recv;
            this->send = send;
            buffer = new char[buffer_cap];
            if(!quiet)
                std::cout << "connected\n";
        }
        void sync() {
            int tmp = 0;
            if(is_server) {
                send_data(&tmp, 1);
                recv_data(&tmp, 1);
            } else {
                recv_data(&tmp, 1);
                send_data(&tmp, 1);
                flush();
            }
        }

        ~LndNetIO() {
            flush();
            delete[] buffer;
        }

        void flush() {
            (*send)(buffer, buffer_ptr, peer_ptr);
            buffer_ptr = 0;
        }

        void send_data(const void * data, int len) {
            for (int i = 0; i < len; i++) {
                printf("%u ", ((char*)data)[i]);
            }
            printf("\n");
            counter += len;
            if (len >= buffer_cap) {
                if(has_send) {
                    flush();
                }
                has_send = false;
                (*send)((char *) data, len, peer_ptr);
                return;
            }
            if (buffer_ptr + len > buffer_cap)
                flush();
            memcpy(buffer + buffer_ptr, data, len);
            buffer_ptr += len;
            has_send = true;
        }

        void recv_data(void  * data, int len) {
            int sent = 0;
            if(has_send) {
                flush();
            }
            has_send = false;
            while(sent < len) {
                Receive_return recv_msg = (*recv)(peer_ptr);
                int res = recv_msg.r1;
                memcpy(sent + (char*) data, recv_msg.r0, res);
//                int res = s.read_some(boost::asio::buffer(sent + (char *)data, len - sent));
                if (res >= 0)
                    sent += res;
                else
                    fprintf(stderr,"error: net_send_data %d\n", res);
            }
            for (int i = 0; i < len; i++) {
                printf("%u ", ((char*)data)[i]);
            }
            printf("\n");
        }
    };

}

#endif  //LND_NETWORK_IO_CHANNEL