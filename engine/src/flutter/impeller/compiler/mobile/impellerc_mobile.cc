#include <filesystem>
#include <system_error>

#include "flutter/fml/backtrace.h"
#include "flutter/fml/command_line.h"
#include "flutter/fml/file.h"
#include "flutter/fml/mapping.h"
#include "impeller/compiler/compiler.h"
#include "impeller/compiler/mobile/impellerc_mobile.h"
#include "impeller/compiler/mobile/shader_bundle_mobile.h"
#include "impeller/compiler/mobile/switches_mobile.h"
#include "impeller/compiler/runtime_stage_data.h"
#include "impeller/compiler/shader_bundle.h"
#include "impeller/compiler/source_options.h"
#include "impeller/compiler/switches.h"
#include "impeller/compiler/types.h"
#include "impeller/compiler/utilities.h"

namespace impeller {
namespace compiler {

bool Main(const fml::CommandLine& command_line) {
  fml::InstallCrashHandler();
  if (command_line.HasOption("help")) {
    Switches::PrintHelp(std::cout);
    return true;
  }

  Switches switches(command_line);
  if (!switches.AreValid(std::cerr)) {
    std::cerr << "Invalid flags specified." << std::endl;
    Switches::PrintHelp(std::cerr);
    return false;
  }
  std::cout << switches.shader_bundle << std::endl;
  if (!switches.shader_bundle.empty()) {
    // Invoke the compiler multiple times to build a shader bundle with the
    // given shader_bundle spec.
    return GenerateShaderBundle(switches);
  }
  std::cerr << "NOT FINISHED YET " << std::endl;
  return false;
}

std::shared_ptr<fml::Mapping> MobileMain(
    std::string& json,
    std::map<std::string, std::string>& filesMap) {
  MobileSwitches switches = MobileSwitches(json, filesMap);

  if (!switches.shader_bundle.empty()) {
    return MobileGenerateShaderBundle(switches);
  }
  std::cerr << "NOT FINISHED YET " << std::endl;
  return nullptr;
}

}  // namespace compiler
}  // namespace impeller

const uint8_t* ulanLaunch(std::string& json,
                          std::map<std::string, std::string>& filesMap, int* buffer_size) {
  std::shared_ptr<fml::Mapping> mapping =
      impeller::compiler::MobileMain(json, filesMap);
  // mapping->GetMapping()
  *buffer_size = mapping->GetSize();
  uint8_t* buffer = (uint8_t*)malloc(mapping->GetSize());
  memcpy(buffer, mapping->GetMapping(), mapping->GetSize());
  return buffer;
}

const uint8_t* ulanLaunchC(const char* json_cstr,
                           const char** keys,
                           const char** values,
                           int count,
                           int* buffer_size
                          ) {
  std::string json(json_cstr);
  std::map<std::string, std::string> filesMap;
  for (int i = 0; i < count; ++i) {
    filesMap[keys[i]] = values[i];
  }
  return ulanLaunch(json, filesMap, buffer_size);  // вызов твоей C++ функции
}
