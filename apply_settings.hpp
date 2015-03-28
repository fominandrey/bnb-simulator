#ifndef _apply_settings_hpp_
#define _apply_settings_hpp_

#include <fstream>

#include <iterator>

#define NDEBUG

#include <libjson/libjson.h>

#include <communicators/timer.hpp>
#include <serializers/serializer.hpp>
#include <communicators/communicator.hpp>
#include <simulators/resolver.hpp>

void apply_settings();

#endif // _apply_settings_hpp_
