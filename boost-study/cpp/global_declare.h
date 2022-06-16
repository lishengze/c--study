#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using ID_TYPE = unsigned long long;

using std::string;
using std::cout;
using std::endl;

#define DECLARE_PTR(X) typedef boost::shared_ptr<X> X##Ptr     /** < define smart ptr > */
#define FORWARD_DECLARE_PTR(X) class X; DECLARE_PTR(X)         /** < forward defile smart ptr > */