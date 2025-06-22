/**
*  Copyright (c) 2018 Archforce Financial Technology.  All rights reserved.
*  Redistribution and use in source and binary forms, with or without  modification, are not permitted.
*  For more information about Archforce, welcome to archforce.cn.
**/
/**
* @brief 访问者设计模式
*/
#ifndef MP_VISITOR_H_
#define MP_VISITOR_H_

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/unordered_map.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <stdint.h>


	/**
	* @brief 所有访问者类的基类
	*/
	class Visitor
	{
		static uint32_t visitor_index_g;
		static boost::mutex visitor_mutex_g;
	public:
		Visitor();
		virtual ~Visitor() {}
		 	uint32_t visitor_index_self_;
		
	};

	/**
	* @brief 所有元素类的基类
	*/
	class ElementBase
	{
	public:
		virtual ~ElementBase() {}

		virtual bool Accept(Visitor* v);
		static boost::mutex element_base_mutex_g;
	};

	class VisitorCallable
	{
	public:
		typedef bool (VisitorCallable::*EntryType)(void*);
	};

	struct CallableInfo
	{
		VisitorCallable::EntryType  entry;
		void*                       ctx;
	};

	/**
	* @brief 派生元素类中使用该宏声明访问者模式
	*
	* @param[in] ParentElement     父类名称
	*/
#define MP_VISITOR_PATTERN_DECLARE(ParentElement)   \
        public: \
		virtual bool Accept(Visitor* v)  \
        {  \
			if (!v) \
			{ \
				return false; \
			} \
            if (v->visitor_index_self_ < visitor_registry_.size()) \
            { \
				CallableInfo* ci = visitor_registry_[v->visitor_index_self_];    \
				if (ci != NULL) \
				{\
					return (((VisitorCallable*)(ci->ctx))-> *(*ci).entry) (this);  \
				}  \
				else  \
					return ParentElement::Accept(v);  \
            } \
            else  \
                return ParentElement::Accept(v);  \
        }  \
		static void RegisterVisitor(Visitor* v,    \
                                    VisitorCallable::EntryType entry,   \
                                    void* ctx) \
        { \
			boost::lock_guard<boost::mutex> lock(element_base_mutex_g); \
			if (visitor_registry_.size() <= v->visitor_index_self_) \
			{ \
				visitor_registry_.resize(v->visitor_index_self_ + 1, 0); \
			} \
			CallableInfo* call_info = new CallableInfo;	\
			call_info->entry = entry;	\
			call_info->ctx = ctx;    \
            visitor_registry_[v->visitor_index_self_] = call_info; \
        } \
		static std::vector<CallableInfo* > visitor_registry_;


	/**
	* @brief 派生类元素的CPP文件中使用该宏声明访问者模式
	*
	* @param[in] Element 派生类名称
	*/
#define MP_VISITOR_PATTERN_DEFINE(Element)    \
        std::vector< CallableInfo* > Element::visitor_registry_;

#define _REGISTER_MESSAGE_PROCESSOR(r, VisitorClass, tuple) \
	BOOST_PP_TUPLE_ELEM(2, 0, tuple)::RegisterVisitor(this,   \
        (VisitorCallable::EntryType)&VisitorClass::BOOST_PP_TUPLE_ELEM(2, 1, tuple), this);

	/**
	* @brief  向元素类注册访问者
	*
	* @param[in] VisitorClass		   访问者类
	* @param[in] msg_proc_tuple_seq    元素类和访问者类的对应关系的 tuple seq 列表，格式为
	*                                （(e0,func0))((e1,func1))((e2,func2))
	*/
#define MP_REGISTER_MESSAGE_PROCESSOR(VisitorClass, msg_proc_tuple_seq) \
	BOOST_PP_SEQ_FOR_EACH(_REGISTER_MESSAGE_PROCESSOR, VisitorClass, msg_proc_tuple_seq)


#endif // MP_VISITOR_H_
