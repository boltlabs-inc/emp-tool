#ifndef LND_NETWORK_IO_CHANNEL
#define LND_NETWORK_IO_CHANNEL

#include "emp-tool/io/io_channel.h"
#include "emp-tool/io/libgo_io_channel.h"

using std::string;

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
	LndNetIO(void* peer_ptr, cb_receive recv, cb_send send, bool is_server, bool quiet = false) {
	    this->is_server = is_server;
	    this->peer_ptr = peer_ptr;
	    this->recv = recv;
	    this->send = send;
	}
	void sync() {
		int tmp = 0;
		if(is_server) {
			send_data(&tmp, 1);
			recv_data(&tmp, 1);
		} else {
			recv_data(&tmp, 1);
			send_data(&tmp, 1);
		}
	}

	~LndNetIO() {

	}

	void flush() { }

	void send_data(const void * data, int len) {
		send((char *) data, len, peer_ptr);
	}

	void recv_data(void  * data, int len) {
		Receive_return recv_msg = recv(peer_ptr);
		data = recv_msg.r0;
		len = recv_msg.r1;
	}
};

}

#endif  //LND_NETWORK_IO_CHANNEL
