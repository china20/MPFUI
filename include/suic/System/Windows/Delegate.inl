// 华勤科技版权所有 2010-2011
// 
// 文件名：delegate.h
// 功  能：实现委托调用
// 
// 作  者：MPF开发组
// 时  间：2008-08-02
// 
// ============================================================================

#include <System/Windows/Object.h>
#include <System/Tools/Array.h>

// 支持没有参数的函数情况
#define UNION_TARG(u) typename FUNC_RET,u

#if ARGS_COUNT == 0

#undef UNION_TARG
#define UNION_TARG(u) typename FUNC_RET

#define	FUNC_ARGS_DECLARE
#define	FUNC_ARGS
#define	TEMPLATE_ARGS_DECLARE
#define	TEMPLATE_ARGS

#elif ARGS_COUNT < 0

# error Cannot handle delegate objects that accept more than 9 arguments!

#else

#define	FUNC_ARGS_DECLARE REPEAT_2_N(ARGS_COUNT,T,a)
#define	FUNC_ARGS REPEAT_1_N(ARGS_COUNT,a)
#define	TEMPLATE_ARGS_DECLARE  REPEAT_1_N(ARGS_COUNT,typename T)
#define	TEMPLATE_ARGS REPEAT_1_N(ARGS_COUNT,T)

#endif

////////////////////////////////////////////////////////////////////////////////////////
// delegate.ini
template<UNION_TARG(TEMPLATE_ARGS_DECLARE)>
struct function_call<UNION_ARG(FUNC_RET,TEMPLATE_ARGS)>
{
    class invoker_base : public suic::Object
    {
    public:

        virtual FUNC_RET Invoke(FUNC_ARGS_DECLARE) = 0;
        virtual bool Safe_Equal(invoker_base* Other) = 0;
    };

    // 对于一般函数
    template<typename T>
    class  invoker_func_obj_ptr : public invoker_base
    {
    public:

        explicit invoker_func_obj_ptr(T t) : _func(t) {}

    private:

        FUNC_RET Invoke(FUNC_ARGS_DECLARE) { return _func(FUNC_ARGS); }
        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_obj_ptr<T>* pSelf = dynamic_cast<invoker_func_obj_ptr<T>*>(Other);
            return (pSelf && pSelf->_func == _func);
        }

    private:

        T _func;
    };

    // 对于类成员函数
    template<typename T>
    class  invoker_func_mem_ptr : public invoker_base
    {
    public:

        typedef FUNC_RET(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_ptr(T* t, func_type f) : _target(t), _func(f) {}

    private:

        FUNC_RET Invoke(FUNC_ARGS_DECLARE) 
        {
            if (_target.IsAlive())
            {
                return ((_target.GetElement<T>())->*_func)(FUNC_ARGS); 
            }
            else
            {
                return FUNC_RET();
            }
        }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_ptr<T>*>(Other);
            return (pSelf && _func == pSelf->_func);
        }

    private:

        func_type _func;
        suic::WeakRef _target;
    };
};

template<TEMPLATE_ARGS_DECLARE>
struct function_call<UNION_ARG(void,TEMPLATE_ARGS)>
{
    class invoker_base : public suic::Object
    {
    public:

        virtual void Invoke(FUNC_ARGS_DECLARE) = 0;
        virtual bool Safe_Equal(invoker_base* Other) = 0;
    };

    // 对于一般函数
    template<typename T>
    class invoker_func_obj_ptr : public invoker_base
    {
    public:

        explicit invoker_func_obj_ptr(T t) : _func(t) {}

    private:

        void Invoke(FUNC_ARGS_DECLARE) { _func(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_obj_ptr<T>* pSelf = dynamic_cast<invoker_func_obj_ptr<T>*>(Other);
            return (pSelf && pSelf->_func == _func);
        }

    private:

        T _func;
    };

    // 对于类成员函数
    template<typename T>
    class invoker_func_mem_ptr : public invoker_base
    {
    public:

        typedef void(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_ptr(T* t, func_type f) : _target(t), _func(f) {}

    private:

        void Invoke(FUNC_ARGS_DECLARE) 
        { 
            if (_target.IsAlive())
            {
                ((T*)(_target.GetTarget())->*_func)(FUNC_ARGS); 
            }
        }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_ptr<T>*>(Other);
            return (pSelf && pSelf->_func == _func);
        }

    private:

        func_type _func;
        suic::WeakRef _target;
    };
};

////////////////////////// 委托的实现

template<UNION_TARG(TEMPLATE_ARGS_DECLARE),typename Base>
class delegate<UNION_ARG(FUNC_RET,TEMPLATE_ARGS),Base> : public Base
{
public:

    typedef FUNC_RET(*func_type)(TEMPLATE_ARGS);
    typedef typename  function_call<UNION_ARG(FUNC_RET,TEMPLATE_ARGS)> call_type;
    typedef typename call_type::invoker_base invoker_type;

    delegate() 
    {
    }

    delegate(invoker_type* invoker) 
    {
        AddInvoker(invoker);
    }

    static suic::RTTIOfInfo* RTTIType()
    {
        static suic::RTTIOfInfo _typeIns = {0,_U("delegate"),&suic::Handler::RTTIType,&delegate::Create};
        return &_typeIns;
    }

    static suic::Object* __stdcall Create()
    {
        return new delegate();
    }

    suic::RTTIOfInfo* GetRTTIType() const
    {
        return RTTIType();
    }

    delegate(const delegate& Other) 
    {
        if (&Other != this)
        {
            for (int i = 0; i < (int)Other._invokers.Length(); ++i)
            {
                AddInvoker(Other._invokers[i]);
            }
        }
    }

    template<typename T>
    delegate(T t)
    {
        AddInvoker(new call_type::invoker_func_obj_ptr<T>(t));
    }

    // 类成员函数
    template<typename T>
    delegate(T* func, FUNC_RET (T::*_Pm)(TEMPLATE_ARGS))
    {
        AddInvoker(new call_type::invoker_func_mem_ptr<T>(func, _Pm));
    }

    ~delegate() 
    { 
        Clear(); 
    }

    void AddInvoker(invoker_type* invoker)
    {
        if (invoker)
        {
            invoker->ref();
            _invokers.Add(invoker);
        }
    }

    delegate operator=(const delegate& Other)
    {
        if (&Other != this)
        {
            Clear();
            for (int i = 0; i < (int)Other._invokers.Length(); ++i)
            {
                AddInvoker(Other._invokers[i]);
            }
        }
        return (*this);
    }

    void operator+=(const delegate& Other)
    {
        for (int i = 0; i < (int)Other._invokers.Length(); ++i)
        {
            AddInvoker(Other._invokers[i]);
        }
    }

    void operator-=(const delegate& Other)
    {
        for (int i = 0; i < (int)Other._invokers.Length(); ++i)
        {
            RemoveInvoker(Other._invokers[i]);
        }
    }

    bool Equals(suic::Object* Other) const
    {
        const delegate* pOther(dynamic_cast<const delegate*>(Other));
        return IsEqual(pOther);
    }

    bool IsEqual(const delegate* Other) const
    {
        if (NULL != Other && _invokers.Length() > 0 && 
            _invokers.Length() == Other->_invokers.Length())
        {
            for (int i = 0; i < _invokers.Length(); ++i)
            {
                if (!_invokers[i]->Safe_Equal(Other->_invokers[i]))
                {
                    return false;
                }
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator==(const delegate& Other) const
    {
        return IsEqual(&Other);
    }

    FUNC_RET operator()(FUNC_ARGS_DECLARE)
    {
        int iSize = (int)_invokers.Length() - 1;
        for (int i = 0; i < iSize; ++i)
        {
            _invokers[i]->Invoke(FUNC_ARGS);
        }
        return _invokers[iSize]->Invoke(FUNC_ARGS);
    }

    void Dispose()
    {
        Clear();
    }

    void Clear()
    {
        for (int i = 0; i < _invokers.Length(); ++i)
        {
            _invokers[i]->unref();
        }
        _invokers.Clear();
    }

    int size() const
    {
        return _invokers.Length();
    }

    bool safe_bool() const
    {
        return (size() > 0);
    }

    operator bool() const
    {
        return (size() > 0);
    }

private:

    void AddEvent(suic::Object* target, suic::FnEventCall fcall)
    {
        AddInvoker(CreateInvoker(target, fcall));
    }

    void RemoveInvoker(invoker_type* invoker)
    {
        for (int i = 0; i < (int)_invokers.Length(); ++i)
        {
            if (_invokers[i]->Safe_Equal(invoker))
            {
                _invokers[i]->unref();
                _invokers.RemoveAt(i);
                break;
            }
        }
    }

    invoker_type* CreateInvoker(suic::Object* target, suic::FnEventCall fcall)
    {
        union mmpf
        {
            suic::FnEventCall pf;
            FUNC_RET (suic::Object::*_Pm)(TEMPLATE_ARGS);
        };
        mmpf _mmf;
        _mmf.pf = fcall;
        return new call_type::invoker_func_mem_ptr<suic::Object>(target, _mmf._Pm);
    }

private:

    suic::Array<invoker_type*> _invokers;
};

// 取消宏的定义，下一次特化定义
#undef UNION_TARG
#undef ARGS_COUNT
#undef FUNC_ARGS_DECLARE
#undef FUNC_ARGS
#undef TEMPLATE_ARGS_DECLARE
#undef TEMPLATE_ARGS
