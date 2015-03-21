#ifndef _request_hpp_
#define _request_hpp_

#include <util/mplib/binary_serializer.hpp>

namespace simulator
{
    struct send_request
	{
        BinarySerializer content;
        long long timestamp;
    };

    struct receive_request
	{
        Buffer& storage;
        int sender;
    };

    struct receive_response
	{
        int sender;
        long long time;
    };
}

#endif // _request_hpp_
