#pragma once

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <memory>
#include <thread>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <chrono>
#include <mutex>
#include <list>
#include <vector>

using namespace std;

using SessionType = std::string;
using RpcType = std::string;

#define DECLARE_PTR(X) typedef boost::shared_ptr<X> X##Ptr     /** < define smart ptr > */
#define FORWARD_DECLARE_PTR(X) class X; DECLARE_PTR(X)         /** < forward defile smart ptr > */
