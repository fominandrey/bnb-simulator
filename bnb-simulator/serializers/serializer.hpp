#ifndef _serializer_hpp_
#define _serializer_hpp_

#define NDEBUG

#include <libjson/libjson.h>

#include <util/mplib/binary_serializer.hpp>

namespace simulator
{
    class serializer : public BinarySerializer
    {
        // actual size
        int s = 0;

    public:

        class parcel_size
        {
            static int command;
            static int record;
            static int subproblem;

        public:

            static void apply_settings(const JSONNode& node);

            static int command_size() { return command; }
            static int record_size() { return record; }
            static int sub_size() { return subproblem; }
        };

        static void apply_settings(const JSONNode& node);

        int actual_size() const { return s; }
        void assign_size(int size) { s = size; }
        void reset() {  BinarySerializer::reset(); s = 0; }
    };
}

#endif // _serializer_hpp_
