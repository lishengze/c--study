# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(testg)

SET("BIN_NAME" testg)
set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread  -flto -O3 -ggdb -g")
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps")
SET(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS} -fno-access-control")

# SET(CMAKE_CXX_FLAGS "-std=c++14  -Wall ${CMAKE_CXX_FLAGS}")

# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -O3")

# set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC -Wall -m64 ${CMAKE_CXX_FLAGS}")
# set(CMAKE_CXX_FLAGS_DEBUG "-g ${CMAKE_CXX_FLAGS}")
# set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 ${CMAKE_CXX_FLAGS}")

# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -O3")

# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -O3")

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(BOOST_ROOT /usr/local/boost_1_68)

# find_package(Threads REQUIRED)

AUX_SOURCE_DIRECTORY(. MAIN_SRC)

# FIND_PACKAGE(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 
#                                      chrono regex program_options unit_test_framework locale REQUIRED)

# INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})

# INCLUDE_DIRECTORIES("../include")

# INCLUDE_DIRECTORIES("../../component/pandora")
# INCLUDE_DIRECTORIES("../../component")
# INCLUDE_DIRECTORIES("../server")
# INCLUDE_DIRECTORIES("../include")
# INCLUDE_DIRECTORIES("../../proto")



# LINK_DIRECTORIES("/usr/local/boost_1_68/lib")
# LINK_DIRECTORIES("/opt/utrade/console/lib/pandora/")


SET(ALL_SRC ${MAIN_SRC})

# generate bin
ADD_EXECUTABLE(${BIN_NAME} ${ALL_SRC})

TARGET_LINK_LIBRARIES(${BIN_NAME} gtest gmock)



