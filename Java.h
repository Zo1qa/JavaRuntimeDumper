#pragma once

#include "jni/jni.h"
#include <mutex>
#include <string>
#include <unordered_map>

#include <iostream>
#include "jni/jvmti.h"

class Rustex {
public:
	JNIEnv* env;
	JavaVM* vm;
}rx;

