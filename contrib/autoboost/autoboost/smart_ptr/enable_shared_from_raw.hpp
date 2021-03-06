#ifndef AUTOBOOST_ENABLE_SHARED_FROM_RAW_HPP_INCLUDED
#define AUTOBOOST_ENABLE_SHARED_FROM_RAW_HPP_INCLUDED

//
//  enable_shared_from_raw.hpp
//
//  Copyright 2002, 2009, 2014 Peter Dimov
//  Copyright 2008-2009 Frank Mori Hess
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

#include <autoboost/config.hpp>
#include <autoboost/shared_ptr.hpp>
#include <autoboost/weak_ptr.hpp>
#include <autoboost/assert.hpp>
#include <autoboost/detail/workaround.hpp>

namespace autoboost
{
template<typename T> autoboost::shared_ptr<T> shared_from_raw(T *);
template<typename T> autoboost::weak_ptr<T> weak_from_raw(T *);

namespace detail
{
template< class X, class Y > inline void sp_enable_shared_from_this( autoboost::shared_ptr<X> * ppx, Y const * py, autoboost::enable_shared_from_raw const * pe );

} // namespace detail

class enable_shared_from_raw
{
protected:

    enable_shared_from_raw()
    {
    }

    enable_shared_from_raw( enable_shared_from_raw const & )
    {
    }

    enable_shared_from_raw & operator=( enable_shared_from_raw const & )
    {
        return *this;
    }

    ~enable_shared_from_raw()
    {
        AUTOBOOST_ASSERT( shared_this_.use_count() <= 1 ); // make sure no dangling shared_ptr objects exist
    }

private:

    void init_weak_once() const
    {
        if( weak_this_.expired() )
        {
            shared_this_.reset( static_cast<void*>(0), detail::esft2_deleter_wrapper() );
            weak_this_ = shared_this_;
        }
    }

#ifdef AUTOBOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
private:
    template<class Y> friend class shared_ptr;
    template<typename T> friend autoboost::shared_ptr<T> shared_from_raw(T *);
    template<typename T> friend autoboost::weak_ptr<T> weak_from_raw(T *);
    template< class X, class Y > friend inline void detail::sp_enable_shared_from_this( autoboost::shared_ptr<X> * ppx, Y const * py, autoboost::enable_shared_from_raw const * pe );
#endif

    shared_ptr<void const volatile> shared_from_this() const
    {
        init_weak_once();
        return shared_ptr<void const volatile>( weak_this_ );
    }

    shared_ptr<void const volatile> shared_from_this() const volatile
    {
        return const_cast< enable_shared_from_raw const * >( this )->shared_from_this();
    }

    // Note: invoked automatically by shared_ptr; do not call
    template<class X, class Y> void _internal_accept_owner( shared_ptr<X> * ppx, Y * py ) const
    {
        AUTOBOOST_ASSERT( ppx != 0 );

        if( weak_this_.expired() )
        {
            weak_this_ = *ppx;
        }
        else if( shared_this_.use_count() != 0 )
        {
            AUTOBOOST_ASSERT( ppx->unique() ); // no weak_ptrs should exist either, but there's no way to check that

            detail::esft2_deleter_wrapper * pd = autoboost::get_deleter<detail::esft2_deleter_wrapper>( shared_this_ );
            AUTOBOOST_ASSERT( pd != 0 );

            pd->set_deleter( *ppx );

            ppx->reset( shared_this_, ppx->get() );
            shared_this_.reset();
        }
    }

    mutable weak_ptr<void const volatile> weak_this_;

private:

    mutable shared_ptr<void const volatile> shared_this_;
};

template<typename T>
autoboost::shared_ptr<T> shared_from_raw(T *p)
{
    AUTOBOOST_ASSERT(p != 0);
    return autoboost::shared_ptr<T>(p->enable_shared_from_raw::shared_from_this(), p);
}

template<typename T>
autoboost::weak_ptr<T> weak_from_raw(T *p)
{
    AUTOBOOST_ASSERT(p != 0);
    autoboost::weak_ptr<T> result;
    result._internal_aliasing_assign(p->enable_shared_from_raw::weak_this_, p);
    return result;
}

namespace detail
{
    template< class X, class Y > inline void sp_enable_shared_from_this( autoboost::shared_ptr<X> * ppx, Y const * py, autoboost::enable_shared_from_raw const * pe )
    {
        if( pe != 0 )
        {
            pe->_internal_accept_owner( ppx, const_cast< Y* >( py ) );
        }
    }
} // namepsace detail

} // namespace autoboost

#endif  // #ifndef AUTOBOOST_ENABLE_SHARED_FROM_RAW_HPP_INCLUDED
