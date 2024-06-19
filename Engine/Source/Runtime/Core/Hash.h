#pragma once

CORE_API std::any HashStreamStart();
CORE_API void HashStreamUpdate(std::any handle, const void* data, u64 size);
CORE_API u64 HashStreamEnd(std::any handle);

CORE_API u64 Hash(const void* data, u64 size);