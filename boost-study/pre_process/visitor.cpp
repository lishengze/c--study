/** 
  *  Copyright (c) 2018 Archforce Financial Technology.  All rights reserved. 
  *  Redistribution and use in source and binary forms, with or without  modification, are not permitted.   
  *  For more information about Archforce, welcome to archforce.cn.
  **/


#include "visitor.h"

boost::uint32_t Visitor::visitor_index_g = 0;

boost::mutex Visitor::visitor_mutex_g;

Visitor::Visitor()
{
	boost::lock_guard<boost::mutex> lock(visitor_mutex_g);
	visitor_index_self_ = visitor_index_g++;
}

boost::mutex ElementBase::element_base_mutex_g;

bool ElementBase::Accept(Visitor* v)
{
	return true;
}

