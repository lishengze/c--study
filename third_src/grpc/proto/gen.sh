# !/bin/bash

rm -fr cpp/*

protoc --proto_path=./ --cpp_out=cpp/ test.proto
# generate grpc 
protoc --proto_path=./ --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin --grpc_out=cpp/ test.proto