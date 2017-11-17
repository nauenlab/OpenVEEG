#pragma once
#ifdef _WIN32
#include <objbase.h>
#else
#include <LinuxCOM.h>
#endif

#include <boost/intrusive_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

template <typename T>
class ComPtr : public boost::intrusive_ptr<T>
{

public:
	ComPtr();
	ComPtr(T* p, bool add_ref = false);
	ComPtr(ComPtr const& r);
	template<typename Y>
	ComPtr(ComPtr<Y> const& r);
};

template <typename T>
ComPtr<T>::ComPtr()
	: boost::intrusive_ptr<T>()
{
}

template <typename T>
ComPtr<T>::ComPtr(T* p, bool add_ref)
	: boost::intrusive_ptr<T>(p, add_ref)
{
}

template <typename T>
ComPtr<T>::ComPtr(ComPtr const& r)
	: boost::intrusive_ptr<T>(r)
{
}

template <typename T>
template <typename Y>
ComPtr<T>::ComPtr(ComPtr<Y> const& r)
	: boost::intrusive_ptr<T>(r)
{
}

inline void intrusive_ptr_add_ref(IUnknown* p)
{
	p->AddRef();
}

inline void intrusive_ptr_release(IUnknown* p)
{
	p->Release();
}

