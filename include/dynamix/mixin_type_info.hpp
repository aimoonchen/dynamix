// DynaMix
// Copyright (c) 2013-2016 Borislav Stanimirov, Zahary Karadjov
//
// Distributed under the MIT Software License
// See accompanying file LICENSE.txt or copy at
// https://opensource.org/licenses/MIT
//
#pragma once


/**
 * \file
 * Defines the internal classes that describe the information about a mixin -
 * features and message data, type name, size, alignment, etc.
 */

#include "global.hpp"
#include "message.hpp"

namespace dynamix
{

class global_allocator;

static const mixin_id INVALID_MIXIN_ID = ~mixin_id(0);

namespace internal
{

typedef void (*mixin_constructor_proc)(void* memory);
typedef void (*mixin_destructor_proc)(void* memory);

// this struct contains information for a given mixin
class DYNAMIX_API mixin_type_info : public noncopyable
{
public:
    mixin_id id; // the mixin's id

    const char* name; // mixin name = name of the actual class

    size_t size; // size of the mixin object
    size_t alignment; // alignment of the mixin type

    // procedures, obtained from the mixin definition that makes the actual
    // construction and destruction
    mixin_constructor_proc constructor;
    mixin_destructor_proc destructor;

    // shows whether this is as initialized mixin
    bool is_valid() const { return id != INVALID_MIXIN_ID; }

    // list of all the message infos for the messages this mixin supports
    std::vector<message_for_mixin> message_infos;

    // used to allocate memory for instances of this mixin
    // usually equal to the allocator of the domain
    global_allocator* allocator;

    mixin_type_info()
        : id(INVALID_MIXIN_ID)
        // since this is always static, other members will be initialized with 0
    {
    }
};

// this metafunction binds the type info of a mixin to its type
template <typename Mixin>
struct mixin_type_info_instance : public noncopyable
{
    // have this static function instead of a simple member to guarantee
    // that mixin_type_info's constructor is called the first time
    static mixin_type_info& info()
    {
        static mixin_type_info d;
        return d;
    }

    // this static member registers the mixin in the domain
    // we need to reference it somewhere so as to call its constructor
    static mixin_type_info_instance registrator;

    // the constructor is defined in mixin.h because it references the domain object
    mixin_type_info_instance();

    // to prevent warnings and optimizations that will say that we're not using
    // mixin_type_info_instance by simply referencing it
    int unused;
};
template <typename Mixin>
mixin_type_info_instance<Mixin> mixin_type_info_instance<Mixin>::registrator;

// this procedure is used for the mixin construction
template <typename Mixin>
void call_mixin_constructor(void* memory)
{
    new (memory) Mixin;
}

template <typename Mixin>
void call_mixin_destructor(void* memory)
{
    reinterpret_cast<Mixin*>(memory)->~Mixin();
}

} // namespace internal
} // namespace dynamix
