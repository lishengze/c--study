#pragma once

#include <stdio.h>


void Before() __attribute__((constructor));

void After() __attribute__((destructor));

void TestAttribute();