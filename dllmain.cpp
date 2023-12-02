// dllmain.cpp : Defines the entry point for the DLL application.
#define _CRT_SECURE_NO_WARNINGS
#include "Java.h"
#include "MinHook.h"
#include <iostream>
#include <fstream>

FILE* file = nullptr;
typedef __int64(__fastcall* DefineClassV)(JNIEnv* env, jobject loader, jstring name, jbyteArray data, jint offset, jint length, jobject pd, jstring source);
DefineClassV defineClassV_;

long long i = 0;

std::string jstring2string(JNIEnv* env, jstring jStr) {
    if (!jStr)
        return "";

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray)env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    size_t length = (size_t)env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char*)pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

jclass JNICALL hookedDefineClass(JNIEnv* env, jobject loader, jstring name, jbyteArray data, jint offset, jint length, jobject pd, jstring source) 
{
    return (jclass)defineClassV_(env, loader, name, data, offset, length, pd, source);
}

void init(void* instance) {
        std::string className = "C:/ClassDumper/penis.class";

        std::ofstream outfile(className);
        outfile << "my text here!" << std::endl;

        if (MH_Initialize() != MH_OK) {
            outfile << "Error while initializing hooks!\n";
            return;
        }

        HMODULE module = GetModuleHandleA("java.dll");
        while (!module) {
            module = GetModuleHandleA("java.dll");
            outfile << "java.dll module not found!\n";
            continue;
        }

        void* defineClass = (void*)GetProcAddress(module, "Java_java_lang_ClassLoader_defineClass1");
        while (!defineClass) {
            defineClass = (void*)GetProcAddress(module, "Java_java_lang_ClassLoader_defineClass1");
            outfile << "Java_java_lang_ClassLoader_defineClass1 not founded\n";
        }

        auto chlen2 = MH_CreateHook((void*)defineClass, (void*)hookedDefineClass, (void**)(&defineClassV_));
        if (chlen2 != MH_OK) {
            outfile << MH_StatusToString(chlen2) << std::endl;
            return;
        }
        unsigned long old;
        outfile << VirtualProtect((void*)defineClass, 0x400, PAGE_EXECUTE_READWRITE, &old) << std::endl;

        outfile << old << std::endl;
        auto chlen = MH_EnableHook((void*)defineClass);
        if (chlen != MH_OK) {
            outfile << MH_StatusToString(chlen) << std::endl;
            return;
        }

        outfile.close();

        while (!GetAsyncKeyState(VK_END)) {

        }

        fclose(file);
        FreeConsole();

        ShowWindow(GetConsoleWindow(), SW_HIDE);

        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    DisableThreadLibraryCalls(hModule);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        CloseHandle(CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(init), hModule, 0, nullptr));
    }
    return TRUE;
}

