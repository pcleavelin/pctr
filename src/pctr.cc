#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pctr.h"

PCTR::PCTR() {
    
}

std::string PCTR::readFileSync(const char* filename) {
    if(filename == nullptr) {
        std::cerr << "Error in readFileSync (Null pointer to filename)\n";
        return std::string();
    }

    std::string contents;

    std::ifstream file(filename);

    if(file.is_open()) {
        std::string line;
        while(std::getline(file, line)) {
            contents += line;
        }
    }

    return contents;
}

static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() < 1) return;
    auto isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    v8::Local<v8::Value> arg = args[0];
    v8::String::Utf8Value val(isolate, arg);

    std::cout << *val << "\n";
}

v8::Local<v8::Context> PCTR::setUpExecutionContext(v8::Local<v8::Context> &context, v8::TryCatch &try_catch) {
    // Create the Global Object Template to add an interface from JS to native
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(this->m_Isolate);
    global->Set(
        v8::String::NewFromUtf8(this->m_Isolate, "log"),
        v8::FunctionTemplate::New(this->m_Isolate, LogCallback)
    );

    return v8::Context::New(this->m_Isolate, NULL, global);
}

void PCTR::handleException(v8::TryCatch &try_catch) {
    v8::Local<v8::Value> exception = try_catch.Exception();
    v8::String::Utf8Value exception_str(exception);
    std::cerr << "Exception occurred: " << *exception_str << "\n";
}

void PCTR::initialize(const char* exec_path) {
    v8::V8::InitializeICUDefaultLocation(exec_path);
    v8::V8::InitializeExternalStartupData(exec_path);
    auto* platform = v8::platform::CreateDefaultPlatform();

    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();

    this->m_CreateParams = v8::Isolate::CreateParams();

    this->m_CreateParams.array_buffer_allocator = 
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    this->m_Isolate = v8::Isolate::New(this->m_CreateParams);
}

void PCTR::dispose() {
    this->m_Isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();

    delete this->m_CreateParams.array_buffer_allocator;
}

int PCTR::execute(const char* filename, const char* func) {
    if(filename == nullptr) {
        std::cerr << "Error in PCTR::execute (Null pointer to filename)\n";
        return 1;
    }

    v8::TryCatch try_catch(this->m_Isolate);

    v8::Isolate::Scope isolate_scope(this->m_Isolate);
    v8::HandleScope handle_scope(this->m_Isolate);
    v8::Local<v8::Context> context = this->setUpExecutionContext(context, try_catch);
    v8::Context::Scope context_scope(context);

    auto js_source = this->readFileSync(filename);

    auto source =
        v8::String::NewFromUtf8(this->m_Isolate, js_source.c_str(), v8::NewStringType::kNormal);
    if(source.IsEmpty()) {
        this->handleException(try_catch);
        return 1;
    }

    auto script = v8::Script::Compile(context, source.ToLocalChecked());
    if(script.IsEmpty()) {
        this->handleException(try_catch);
    } else {
        auto script_result = script.ToLocalChecked()->Run(context);
        if(script_result.IsEmpty()) {
            this->handleException(try_catch);
        } else {
            auto main_name = v8::String::NewFromUtf8(this->m_Isolate, func, v8::NewStringType::kNormal).ToLocalChecked();
            auto main_val = context->Global()->Get(context, main_name);
            if(main_val.IsEmpty()) {
                this->handleException(try_catch);
            } else if(main_val.ToLocalChecked()->IsFunction()) {
                auto main_func = v8::Local<v8::Function>::Cast(main_val.ToLocalChecked());

                auto source_name =
                    v8::String::NewFromUtf8(this->m_Isolate, filename, v8::NewStringType::kNormal).ToLocalChecked();

                auto result = main_func->Call(context, context->Global(), 0, NULL); 
                if(result.IsEmpty()) {
                    this->handleException(try_catch);
                } else {
                    auto return_val = v8::Local<v8::Int32>::Cast(result.ToLocalChecked());

                    return return_val->Int32Value();
                }
            } else {
                std::cerr << "No main function found\n";
            }
        }
    }
}