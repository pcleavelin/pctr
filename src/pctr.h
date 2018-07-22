#ifndef _PCTR_H_
#define _PCTR_H_

#include <libplatform/libplatform.h>
#include <v8.h>

class PCTR {
private:
    v8::Isolate *m_Isolate;
    v8::Isolate::CreateParams m_CreateParams;

    std::string readFileSync(const char* filename);

    v8::Local<v8::Context> setUpExecutionContext(v8::Local<v8::Context> &context, v8::TryCatch &try_catch);
    
    void handleException(v8::TryCatch &try_catch);
public:
    PCTR();

    void initialize(const char* exec_path);
    void dispose();

    int execute(const char* filename, const char* func);
};


#endif