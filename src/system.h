/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __system_h__
#define __system_h__

#include <stddef.h>

/** @return 0 on success, -1 on failure
 */
int load_file_data(const char* filename, void** data, size_t* data_size);
void free_file_data(void* data);
/** Prints a message to the systems log
 */
void system_log(const char* format, ...);

#endif /* include guard */
