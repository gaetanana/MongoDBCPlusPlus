cmake_minimum_required(VERSION 3.17)
project(MongoDBCPlusPlus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_GENERATOR_PLATFORM x64)

#Include des directories C et C++
include_directories("DriversMongoDB/mongo-cxx-driver-r3.7.0/mongo-cxx-driver-r3.7.0/build/install/include/mongocxx/v_noabi")
include_directories("DriversMongoDB/mongo-cxx-driver-r3.7.0/mongo-cxx-driver-r3.7.0/build/install/include/bsoncxx/v_noabi")
include_directories("DriversMongoDB/mongo-cxx-driver-r3.7.0/mongo-cxx-driver-r3.7.0/build/install/include/mongocxx/v_noabi/mongocxx")

include_directories("DriverPugiXML/pugixml2/build/install/include")
include_directories("DriverJSON/jsoncpp2/build/install/include")

include_directories("C:/boost")

link_directories("DriversMongoDB/mongo-cxx-driver-r3.7.0/mongo-cxx-driver-r3.7.0/build/install/lib")
link_directories("DriverPugiXML/pugixml2/build/install/lib")
link_directories("DriverJSON/jsoncpp2/build/install/lib")

add_executable(MongoDBCPlusPlus main.cpp ConnectionMongoDB/connection.cpp ConnectionMongoDB/connection.h CREATE/CREATE.cpp CREATE/CREATE.h DELETE/DELETE.cpp DELETE/DELETE.h READ/READ.cpp READ/READ.h UPDATE/UPDATE.cpp UPDATE/UPDATE.h)

target_link_libraries(MongoDBCPlusPlus mongocxx)
target_link_libraries(MongoDBCPlusPlus bsoncxx)
target_link_libraries(MongoDBCPlusPlus pugixml)
target_link_libraries(MongoDBCPlusPlus jsoncpp)