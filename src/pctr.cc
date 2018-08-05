#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pctr.h"

PCTR::PCTR() {
    
}

std::string PCTR::fixFilename(std::string filename) {
    if(filename.size() > 3) {
        std::string extension = filename.substr(filename.size()-3);
        if(extension == ".ts") {
            std::string new_filename = filename.substr(0, filename.size()-3);
            new_filename += ".js";

            return new_filename;
        }
    }
    return filename;
}

std::string PCTR::readFileSync(const char* filename) {
    if(filename == nullptr) {
        std::cerr << "Error in readFileSync (Null pointer to filename)\n";
        return std::string();
    }

    std::string contents;

    std::ifstream file(filename, std::ios::in);

    if(file.is_open()) {
        std::string line;
        while(std::getline(file, line)) {

            // Check for inline imports
            if(line.find("//#import") != std::string::npos) {
                std::string import_file = line.substr(10);
                contents += PCTR::readFileSync(import_file.c_str());
                contents += "\n";
                continue;
            }
            contents += line;
            contents += "\n";
        }
    } else {
        std::cerr << "could not find file " << filename << "\n";
    }

    return contents;
}

void PCTR::OutCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() < 1) return;
    auto isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    v8::Local<v8::Value> arg = args[0];
    v8::String::Utf8Value val(isolate, arg);

    std::cout << *val << "\n";
}

void PCTR::CompileCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if(args.Length() < 2) return;

    std::cout << "C++ Attempting to compile typescript module\n";
    auto isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    v8::Local<v8::Value> name_arg = args[0];
    v8::Local<v8::Value> contents_arg = args[1];

    //std::string file_contents = PCTR::readFileSync(*val);
    //auto contents = v8::String::NewFromUtf8(isolate, file_contents.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

    v8::TryCatch try_catch(isolate);

    auto module_context = isolate->GetCurrentContext();

    auto script = v8::Script::Compile(v8::Local<v8::String>::Cast(contents_arg), v8::Local<v8::String>::Cast(name_arg));
    if(script.IsEmpty()) {
        PCTR::handleException(try_catch);
    } else {
        auto script_result = script->Run(module_context);
        if(script_result.IsEmpty()) {
            PCTR::handleException(try_catch);
            std::cerr << "Couldn't import module\n";
        } else {
            args.GetReturnValue().Set(module_context->Global());
            /*
            auto exports_name = v8::String::NewFromUtf8(isolate, "exports", v8::NewStringType::kNormal).ToLocalChecked();
            auto exports_val = module_context->Global()->Get(module_context, exports_name);
            if(exports_val.IsEmpty()) {
                PCTR::handleException(try_catch);
            } else {
                auto rval = exports_val.ToLocalChecked();
                if(rval->IsUndefined()) {
                    std::cerr << "Error when importing module (undefined)\n";
                } else {
                    std::cout << "Returning " << *rval << " to JS\n";
                    args.GetReturnValue().Set(module_context->Global());
                }
            }*/
        }
    }
}

void PCTR::RecvCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    //std::cout << "C++ RecvCallback\n";

    if(args.Length() < 2) return;
    auto isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    v8::Local<v8::Value> arg1 = args[0];
    v8::String::Utf8Value msg_type(isolate, arg1);

    v8::Local<v8::Value> arg2 = args[1];
    v8::String::Utf8Value data(isolate, arg2);

    //std::cout << "msg_type: " << *msg_type << ", data: " << *data << "\n";

    if(std::string(*msg_type) == "0") {
        std::string file_contents = PCTR::readFileSync(*data);
        auto contents = v8::String::NewFromUtf8(isolate, file_contents.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
        args.GetReturnValue().Set(contents);

        std::cout << "Got file read\n";
        return;
    }

    args.GetReturnValue().SetUndefined();
}

void PCTR::ExecuteCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    std::cout << "C++ ExecuteCallback\n";

    if(args.Length() < 2) return;
    auto isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    v8::Local<v8::Value> arg1 = args[0];
    v8::String::Utf8Value filename(isolate, arg1);

    v8::Local<v8::Value> arg2 = args[1];
    v8::String::Utf8Value func(isolate, arg2);

    int rval = PCTR::execute(isolate, isolate->GetCurrentContext(), *filename, *func, 0, NULL);

    args.GetReturnValue().Set(rval);
}

v8::Local<v8::Context> PCTR::setUpExecutionContext(v8::Isolate *isolate) {
    v8::TryCatch try_catch(isolate);

    // Create the Global Object Template to add an interface from JS to native
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    auto pctr_obj = v8::ObjectTemplate::New(isolate);
    auto exports_obj = v8::ObjectTemplate::New(isolate);
    pctr_obj->Set(
        v8::String::NewFromUtf8(isolate, "recv"),
        v8::FunctionTemplate::New(isolate, RecvCallback)
    );
    pctr_obj->Set(
        v8::String::NewFromUtf8(isolate, "execute"),
        v8::FunctionTemplate::New(isolate, ExecuteCallback)
    );
    pctr_obj->Set(
        v8::String::NewFromUtf8(isolate, "compile"),
        v8::FunctionTemplate::New(isolate, CompileCallback)
    );
    global->Set(
        v8::String::NewFromUtf8(isolate, "out"),
        v8::FunctionTemplate::New(isolate, OutCallback)
    );
    global->Set(isolate, "pctr", pctr_obj);
    global->Set(isolate, "exports", exports_obj);

    return v8::Context::New(isolate, NULL, global);
}

void PCTR::handleException(v8::TryCatch &try_catch) {
    v8::Local<v8::Value> exception = try_catch.Exception();
    auto trace = try_catch.Message();
    v8::String::Utf8Value exception_str(exception);

    v8::String::Utf8Value thing(trace->GetScriptResourceName());
    v8::String::Utf8Value source_line(trace->GetSourceLine());

    std::cerr << *thing << " '" << *source_line << "' Line " << trace->GetLineNumber() << " Exception occurred: " << *exception_str << "\n" << "\n";
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

int PCTR::start(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "No input file specified!\n";
        return 1;
    }

    v8::Isolate::Scope isolate_scope(this->m_Isolate);
    v8::HandleScope handle_scope(this->m_Isolate);
    this->m_MainContext = PCTR::setUpExecutionContext(this->m_Isolate);
    v8::Context::Scope context_scope(this->m_MainContext);

    return this->execute("pctrlib/runtime.js", "start", argc, argv);
}

int PCTR::execute(const char* filename, const char* func, int argc, char** argv) {
    return PCTR::execute(this->m_Isolate, this->m_MainContext, filename, func, argc, argv);
}

int PCTR::execute(v8::Isolate *isolate, v8::Local<v8::Context> context, 
                    const char* filename,
                    const char* func,
                    int argc,
                    char** argv)
{
    if(filename == nullptr) {
        std::cerr << "Error in PCTR::execute (Null pointer to filename)\n";
        return 1;
    }

    v8::TryCatch try_catch(isolate);

    auto js_source = PCTR::readFileSync(filename);

    auto source =
        v8::String::NewFromUtf8(isolate, js_source.c_str(), v8::NewStringType::kNormal);
    if(source.IsEmpty()) {
        PCTR::handleException(try_catch);
        return 1;
    }

    auto script = v8::Script::Compile(source.ToLocalChecked(), v8::String::NewFromUtf8(isolate, filename, v8::NewStringType::kNormal).ToLocalChecked());
    if(script.IsEmpty()) {
        PCTR::handleException(try_catch);
    } else {
        auto script_result = script->Run(context);
        if(script_result.IsEmpty()) {
            PCTR::handleException(try_catch);
        } else {
            auto main_name = v8::String::NewFromUtf8(isolate, func, v8::NewStringType::kNormal).ToLocalChecked();
            auto main_val = context->Global()->Get(context, main_name);
            if(main_val.IsEmpty()) {
                PCTR::handleException(try_catch);
            } else if(main_val.ToLocalChecked()->IsFunction()) {
                auto main_func = v8::Local<v8::Function>::Cast(main_val.ToLocalChecked());

                v8::Local<v8::Value> args[argc];
                for(int i=0;i<argc;++i) {
                    args[i] = v8::String::NewFromUtf8(isolate, argv[i], v8::NewStringType::kNormal).ToLocalChecked();
                }

                auto result = main_func->Call(context, context->Global(), argc, args); 
                if(result.IsEmpty()) {
                    PCTR::handleException(try_catch);
                } else {
                    auto return_val = v8::Local<v8::Int32>::Cast(result.ToLocalChecked());

                    return return_val->Int32Value();
                }
            } else {
                std::cerr << "No '" << func << "' function found\n";
            }
        }
    }

    return 1;
}