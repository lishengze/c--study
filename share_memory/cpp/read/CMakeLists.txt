# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(read)

SET("BIN_NAME" read)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O3")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

AUX_SOURCE_DIRECTORY(. MAIN_SRC)


INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
INCLUDE_DIRECTORIES("../include")

INCLUDE_DIRECTORIES("../../../component/pandora")
INCLUDE_DIRECTORIES("../../../component")


LINK_DIRECTORIES("/opt/utrade/console/lib/pandora/")


SET(ALL_SRC ${MAIN_SRC})

# generate bin
ADD_EXECUTABLE(${BIN_NAME} ${ALL_SRC})

# generate the library
# ADD_LIBRARY(${PROJECT_NAME} SHARED ${ALL_SRC})


# TARGET_LINK_LIBRARIES(${BIN_NAME} ${Boost_LIBRARIES})

# TARGET_LINK_LIBRARIES(${BIN_NAME} fmt)

# TARGET_LINK_LIBRARIES(${BIN_NAME} Threads::Threads)

# TARGET_LINK_LIBRARIES(${BIN_NAME} folly)
# TARGET_LINK_LIBRARIES(${BIN_NAME} dl)
# TARGET_LINK_LIBRARIES(${BIN_NAME} fmt)

# TARGET_LINK_LIBRARIES(${BIN_NAME} c)
# TARGET_LINK_LIBRARIES(${BIN_NAME} static_hook)
# TARGET_LINK_LIBRARIES(${BIN_NAME} libgo)


