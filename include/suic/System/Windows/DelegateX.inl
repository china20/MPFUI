// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����delegate.h
// ��  �ܣ�ʵ��ί�е���
// 
// ��  �ߣ�Sharpui������
// ʱ  �䣺2008-08-02
// 
// ============================================================================

#include <System/Windows/Object.h>
#include <System/Tools/Array.h>

// ֧��û�в����ĺ������
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
    // ����һ�㺯��
    template<typename T>
    class  invoker_func_obj_ptr
    {
    public:

        explicit invoker_func_obj_ptr(T t) : obj_ptr(t) {}
    private:

        FUNC_RET Invoke(FUNC_ARGS_DECLARE) { return obj_ptr(FUNC_ARGS); }
        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_obj_ptr<T>* pSelf = dynamic_cast<invoker_func_obj_ptr<T>*>(Other);
            return (pSelf && pSelf->obj_ptr == obj_ptr);
        }

    private:

        T obj_ptr;
    };

    // �������Ա����
    template<typename T>
    class  invoker_func_mem_ptr
    {
    public:

        typedef FUNC_RET(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_ptr(T * t, func_type f) : obj_ptr(t), func_ptr(f) {}

    private:

        FUNC_RET Invoke(FUNC_ARGS_DECLARE) { return (obj_ptr->*func_ptr)(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_ptr<T>*>(Other);
            return (pSelf && pSelf->obj_ptr == obj_ptr);
        }

    private:

        T * obj_ptr;
        func_type func_ptr;
    };

    // �����������Ա����
    template<typename T>
    class invoker_func_mem_ref_ptr
    {
    public:

        typedef FUNC_RET(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_ref_ptr(T & t, func_type f) : obj_ref(t), func_ptr(f) {}

    private:

        FUNC_RET Invoke(FUNC_ARGS_DECLARE){ return (obj_ref.*func_ptr)(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_ref_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_ref_ptr<T>*>(Other);
            return (pSelf && pSelf->func_ptr == func_ptr);
        }

    private:

        T& obj_ref;
        func_type func_ptr;
    };

    // ���ھ�̬���Ա����
    template<typename T>
    class invoker_func_mem_static_ptr
    {
    public:

        typedef FUNC_RET(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_static_ptr(func_type f) : func_ptr(f) {}

    private:

        FUNC_RET Invoke(FUNC_ARGS_DECLARE){ return (T::func_ptr)(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_static_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_static_ptr<T>*>(Other);
            return (pSelf && pSelf->func_ptr == func_ptr);
        }

    private:

        func_type func_ptr;
    };

    // ��������ָ�����Ա����
    template<typename T>
    class invoker_func_mem_ptr_ptr
    {
    public:

        typedef FUNC_RET(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_ptr_ptr(T t, func_type f) : obj_ref(t), func_ptr(f) {}

    private:

        FUNC_RET Invoke(FUNC_ARGS_DECLARE){ return (obj_ref->*func_ptr)(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_ptr_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_ptr_ptr<T>*>(Other);
            return (pSelf && pSelf->func_ptr == func_ptr);
        }

    private:

        T obj_ref;
        func_type func_ptr;
    };
};

template<TEMPLATE_ARGS_DECLARE>
struct function_call<UNION_ARG(void,TEMPLATE_ARGS)>
{
    // ����һ�㺯��
    template<typename T>
    class invoker_func_obj_ptr
    {
    public:

        explicit invoker_func_obj_ptr(T t) : obj_ptr(t) {}

    private:

        void Invoke(FUNC_ARGS_DECLARE) { obj_ptr(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_obj_ptr<T>* pSelf = dynamic_cast<invoker_func_obj_ptr<T>*>(Other);
            return (pSelf && pSelf->obj_ptr == obj_ptr);
        }

    private:

        T obj_ptr;
    };

    // �������Ա����
    template<typename T>
    class invoker_func_mem_ptr
    {
    public:

        typedef void(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_ptr(T * t, func_type f) : obj_ptr(t), func_ptr(f) {}

    private:

        void Invoke(FUNC_ARGS_DECLARE) { (obj_ptr->*func_ptr)(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_ptr<T>*>(Other);
            return (pSelf && pSelf->obj_ptr == obj_ptr);
        }

    private:

        T * obj_ptr;
        func_type func_ptr;
    };

    // �����������Ա����
    template<typename T>
    class invoker_func_mem_ref_ptr
    {
    public:

        typedef void(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_ref_ptr(T & t, func_type f) : obj_ref(t), func_ptr(f) {}

    private:

        void Invoke(FUNC_ARGS_DECLARE) { (obj_ref.*func_ptr)(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_ref_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_ref_ptr<T>*>(Other);
            return (pSelf && pSelf->func_ptr == func_ptr);
        }

    private:

        T & obj_ptr;
        func_type func_ptr;
    };

    // �����ྲ̬��Ա����
    template<typename T>
    class invoker_func_mem_static_ptr
    {
    public:

        typedef void(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_static_ptr(func_type f) : func_ptr(f) {}

    private:

        void Invoke(FUNC_ARGS_DECLARE) { (T::func_ptr)(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_static_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_static_ptr<T>*>(Other);
            return (pSelf && pSelf->func_ptr == func_ptr);
        }

    private:

        func_type func_ptr;
    };

    // ��������ָ�����Ա����
    template<typename T>
    class  invoker_func_mem_ptr_ptr
    {
    public:

        typedef void(T::*func_type)(TEMPLATE_ARGS);
        explicit invoker_func_mem_ptr_ptr(T t, func_type f) : obj_ref(t), func_ptr(f) {}

    private:

        void Invoke(FUNC_ARGS_DECLARE){ (obj_ref->*func_ptr)(FUNC_ARGS); }

        bool Safe_Equal(invoker_base* Other)
        {
            invoker_func_mem_ptr_ptr<T>* pSelf = dynamic_cast<invoker_func_mem_ptr_ptr<T>*>(Other);
            return (pSelf && pSelf->func_ptr == func_ptr);
        }

    private:

        T obj_ref;
        func_type func_ptr;
    };
};

////////////////////////// ί�е�ʵ��
template<UNION_TARG(TEMPLATE_ARGS_DECLARE),typename Base>
class delegate<UNION_ARG(FUNC_RET,TEMPLATE_ARGS), Base> : public Base
{
public:

    typedef FUNC_RET(*func_type)(TEMPLATE_ARGS);
    typedef typename  function_call<UNION_ARG(FUNC_RET,TEMPLATE_ARGS)> call_type;
    typedef typename call_type::invoker_basePtr invoker_typePtr;
    typedef typename call_type::invoker_base invoker_type;

    delegate(invoker_type* val) 
    {
        if (val)
        {
            _invokers.push_back(val);
        }
    }

    delegate() 
    {
    }

    delegate(const delegate& Other) 
    {
        if (&Other != this)
        {
            for (int i = 0; i < (int)Other._invokers.Length(); ++i)
            {
                _invokers.Add(Other._invokers[i]);
            }
        }
    }

    template<typename T>
    delegate(T t)
    {
        _invokers.Add(invoker_typePtr(new call_type::invoker_func_obj_ptr<T>(t)));
    }

    // �ྲ̬��Ա����
    template<typename T> 
    delegate(FUNC_RET (T::*_Pm)(TEMPLATE_ARGS))
    {
        _invokers.Add(new call_type::invoker_func_mem_static_ptr<T>(_Pm));
    }

    // ���Ա����
    template<typename T>
    delegate(T * t, FUNC_RET (T::*_Pm)(TEMPLATE_ARGS))
    {
        _invokers.Add(new call_type::invoker_func_mem_ptr<T>(t, _Pm));
    }

    template<typename T>
    delegate(T & t, FUNC_RET (T::*_Pm)(TEMPLATE_ARGS))
    {
        _invokers.Add(new call_type::invoker_func_mem_ref_ptr<T>(t, _Pm));
    }

    // �󶨳�Ա����ָ�룬�������ܷ�ʽ
    template<typename T>
    delegate(T t, FUNC_RET (T::*_Pm)(TEMPLATE_ARGS))
    {
        _invokers.Add(new call_type::invoker_func_mem_ptr_ptr<T>(t, _Pm));
    }

    ~delegate() 
    { 
        clear(); 
    }

    delegate operator=(const delegate& Other)
    {
        if (&Other != this)
        {
            clear();

            for (int i = 0; i < (int)Other._invokers.Length(); ++i)
            {
                _invokers.Add(Other._invokers[i]);
            }
        }

        return (*this);
    }

    void operator+=(const delegate& Other)
    {
        for (int i = 0; i < (int)Other._invokers.Length(); ++i)
        {
            _invokers.Add(Other._invokers[i]);
        }
    }

    void operator-=(const delegate& Other)
    {
        if (Other._invokers.Length() == 1)
        {
            for (int i = 0; i < (int)_invokers.Length(); ++i)
            {
                if (_invokers[i]->Safe_Equal(Other._invokers[0].get()))
                {
                    _invokers.RemoveAt(i);
                    break;
                }
            }
        }
    }

    bool Equals(suic::Object* Other) const
    {
        const delegate* pOther(dynamic_cast<const delegate*>(Other));
        return IsEqual(pOther);
    }

    bool IsEqual(const delegate* Other) const
    {
        if (!Other || _invokers.Length() != Other->_invokers.Length())
        {
            return false;
        }
        else if (_invokers.Length() == 0 ||
            (_invokers.Length() == 1 && _invokers[0]->Safe_Equal(Other->_invokers[0].get())))
        {
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
        if (_invokers.Length() == 0) 
        {
            throwBadInvoke();
        }

        int iSize = (int)_invokers.Length() - 1;

        for (int i = 0; i < iSize; ++i)
        {
            _invokers[i]->Invoke(FUNC_ARGS);
        }

        return _invokers[iSize]->Invoke(FUNC_ARGS);
    }

    bool safe_bool() const
    {
        return (_invokers.Length() > 0);
    }

    operator bool() const
    {
        return (_invokers.Length() > 0);
    }

    void clear()
    {
        _invokers.Clear();
    }

private:

    suic::Array<invoker_typePtr> _invokers;
};

// ȡ����Ķ��壬��һ���ػ�����
#undef   UNION_TARG
#undef   ARGS_COUNT
#undef   FUNC_ARGS_DECLARE
#undef   FUNC_ARGS
#undef   TEMPLATE_ARGS_DECLARE
#undef   TEMPLATE_ARGS