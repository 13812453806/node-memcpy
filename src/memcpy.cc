/*
 Copyright 2013 Daniel Wirtz <dcode@dcode.io>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */
#include <string.h>
#include <node.h>
#include <nan.h>
#include <ArrayBuffer.h>
#include "memcpy.h"

using namespace v8;

// Copies data between kExternalUnsignedByteArrays like node Buffers and ArrayBuffers.
// memcpy(target[, targetStart], source[, sourceStart[, sourceEnd]]):bytesCopied
NAN_METHOD(memcpy){

    if (sizeof(unsigned char) != 1) {
        Nan::ThrowError("sizeof(unsigned char) != 1");
        info.GetReturnValue().SetUndefined();
    }

    char* targetData;
    size_t targetStart = 0,
           targetLength;

    char* sourceData;
    size_t sourceStart = 0,
           sourceLength,
           sourceEnd;

    // Requires at least two arguments: target and source
    if (info.Length() < 2) {
        Nan::ThrowTypeError("illegal number of arguments");
        info.GetReturnValue().SetUndefined();
    }

    int i = 0;

    // Target must be an object
    if (!info[i] -> IsObject()) {
        Nan::ThrowTypeError("illegal target: not an object");
        info.GetReturnValue().SetUndefined();
    }

    Local<Object> target = info[i] -> ToObject();

	// Target must be a buffer or ArrayBuffer
    if(node::Buffer::HasInstance(target)) {
		targetData = node::Buffer::Data(target);
		targetLength = node::Buffer::Length(target);
    }

	else if (info[i] -> IsArrayBuffer()) {
		node::ArrayBuffer* ab = node::ArrayBuffer::New(info[i]);
		targetData = static_cast<char*>(ab -> Data());
        targetLength = ab -> ByteLength();
	}

	else {
		Nan::ThrowTypeError("illegal target: not a buffer or ArrayBuffer.");
		info.GetReturnValue().SetUndefined();
	}

    // If specified, targetStart must be an unsigned integer in [0,targetLength]
    if (info[++i] -> IsUint32()) {
        targetStart = info[i++]->ToUint32()->Value();
        if (targetStart < 0 || targetStart > targetLength) {
            Nan::ThrowRangeError("illegal targetStart: out of bounds");
            info.GetReturnValue().SetUndefined();
        }
    }

    // Requires at least one additional argument: source
    if (i >= info.Length()) {
        Nan::ThrowTypeError("illegal number of arguments");
        info.GetReturnValue().SetUndefined();
    }

    // Source must be an object
    if (!info[i] -> IsObject()) {
        Nan::ThrowTypeError("illegal source: not an object");
        info.GetReturnValue().SetUndefined();
    }

    Local<Object> source = info[i] -> ToObject();

	// Source must be a buffer or ArrayBuffer
	if(node::Buffer::HasInstance(source)) {
		sourceData = node::Buffer::Data(source);
		sourceEnd = sourceLength = node::Buffer::Length(source);
	}

	else if (info[i] -> IsArrayBuffer()) {
		node::ArrayBuffer* ab = node::ArrayBuffer::New(info[i]);
        sourceData = static_cast<char*>(ab -> Data());
        sourceEnd = sourceLength = ab -> ByteLength();
	}

	else {
		Nan::ThrowTypeError("illegal source: not a buffer or ArrayBuffer.");
		info.GetReturnValue().SetUndefined();
	}

    // If specified, sourceStart must be an unsigned integer in [0,sourceLength]
    if (++i < info.Length()) {
        if (!info[i] -> IsUint32()) {
            Nan::ThrowTypeError("illegal sourceStart: not an uint32");
            info.GetReturnValue().SetUndefined();
        }

        sourceStart = info[i++] -> ToUint32() -> Value();

		if (sourceStart < 0 || sourceStart > sourceLength) {
            Nan::ThrowTypeError("illegal sourceStart: out of bounds");
            info.GetReturnValue().SetUndefined();
        }
    }

    // If specified, sourceEnd must be an unsigned integer in [sourceStart,sourceLength]
    if (i < info.Length()) {
        if (!info[i] -> IsUint32()) {
            Nan::ThrowTypeError("illegal sourceEnd: not an uint32");
            info.GetReturnValue().SetUndefined();
        }

		sourceEnd = info[i++] -> ToUint32() -> Value();

		if (sourceEnd < sourceStart || sourceEnd > sourceLength) {
            Nan::ThrowTypeError("illegal sourceEnd: out of bounds");
            info.GetReturnValue().SetUndefined();
        }
    }

    // Additional arguments are invalid
    if (i < info.Length()) {
        Nan::ThrowTypeError("illegal number of arguments");
        info.GetReturnValue().SetUndefined();
    }

    // Determine number of bytes to copy
    int length = sourceEnd - sourceStart;
    if (length == 0) {
        info.GetReturnValue().Set(Nan::New<Number>(length));
    }

    // Perform sanity checks
    if (targetStart + length > targetLength) {
        Nan::ThrowTypeError("illegal source range: target capacity overrun");
        info.GetReturnValue().SetUndefined();
    }

    // Do the thing (memmove to be compatible with native Buffer#copy)
	memmove(
		targetData + targetStart,
		sourceData + sourceStart,
		length
	);

    info.GetReturnValue().Set(Nan::New<Number>(length));
}
