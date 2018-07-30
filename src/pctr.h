#ifndef _PCTR_H_
#define _PCTR_H_

#include <libplatform/libplatform.h>
#include <v8.h>

class PCTR {
private:
    v8::Isolate *m_Isolate;
    v8::Isolate::CreateParams m_CreateParams;

    static void OutCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void RequireCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void RecvCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void handleException(v8::TryCatch &try_catch);

    static v8::Local<v8::Context> setUpExecutionContext(v8::Isolate *isolate, v8::TryCatch &try_catch);

    static std::string fixFilename(std::string filename);
public:
    PCTR();

    static std::string readFileSync(const char* filename);

    void initialize(const char* exec_path);
    void dispose();

    int execute(const char* filename, const char* func);
};


#endif