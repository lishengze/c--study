# !/bin/bash

rm -fr cpp/*

protoc --proto_path=./ --cpp_out=cpp/ market_data.proto
# generate grpc 
protoc --proto_path=./ --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin --grpc_out=cpp/ market_data.proto