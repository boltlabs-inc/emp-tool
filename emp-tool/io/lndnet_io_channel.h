#ifndef LND_NETWORK_IO_CHANNEL
#define LND_NETWORK_IO_CHANNEL

#include "emp-tool/io/io_channel.h"
#include "emp-tool/io/go_io_channel.h"

using std::string;

namespace emp {

/** @addtogroup IO
  @{
 */
class LndNetIO: public IOChannel<LndNetIO> {
public:
	bool is_server;
    size_t peer_ptr;
	LndNetIO(size_t peer_ptr, bool is_server, bool quiet = false) {
	    this->is_server = is_server;
	    this->peer_ptr = peer_ptr;
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

	void send_data(const void * data, int len) {
		Send((char *) data, len, (GoUintptr) peer_ptr);
	}

	void recv_data(void  * data, int len) {
		Receive_return recv_msg = Receive((GoUintptr) peer_ptr);
		data = recv_msg.r0;
		len = recv_msg.r1;
	}
};

}

#endif  //LND_NETWORK_IO_CHANNEL
