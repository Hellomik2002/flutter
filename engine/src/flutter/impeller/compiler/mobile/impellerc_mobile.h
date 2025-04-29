
#include <filesystem>
#include <map>
#include <system_error>

extern "C" {
__attribute__((visibility("default"))) extern const uint8_t* ulanLaunchC(
    const char* json_cstr,
    const char** keys,
    const char** values,
    int count,
    int* buffer_size);
}