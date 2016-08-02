#include "memcpy.h"

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {
  Nan::Set(target, Nan::New("memcpy").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(memcpy)).ToLocalChecked());
}

NODE_MODULE(NativeExtension, InitAll)
