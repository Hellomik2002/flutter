#include <filesystem>
#include <system_error>

#include "flutter/fml/backtrace.h"
#include "flutter/fml/command_line.h"
#include "flutter/fml/file.h"
#include "flutter/fml/mapping.h"
#include "impeller/compiler/compiler.h"
#include "impeller/compiler/mobile/shader_bundle_mobile.h"
#include "impeller/compiler/mobile/switches_mobile.h"
#include "impeller/compiler/runtime_stage_data.h"
#include "impeller/compiler/shader_bundle.h"
#include "impeller/compiler/source_options.h"
#include "impeller/compiler/switches.h"
#include "impeller/compiler/types.h"
#include "impeller/compiler/utilities.h"

const std::string simpleVertShader = R"(
// Copy into: shaders/simple.vert
in vec2 position;
void main() {
  gl_Position = vec4(position, 0.0, 1.0);
}
)";

const std::string simpleFragShader = R"(
// Copy into: shaders/simple.frag
out vec4 frag_color;
void main() {
  frag_color = vec4(0, 1, 0, 1);
}
)";

const std::string jsonBundlk = R"({
  "SimpleVertex": {
    "type": "vertex",
    "file": "shaders1/simple.vert"
  },
  "SimpleFragment": {
    "type": "fragment",

    "file": "shaders1/simple.frag"
  }
})";

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
    const std::string& json,
    const std::map<std::string, std::string>& filesMap) {
  MobileSwitches switches = MobileSwitches(json, filesMap);

  if (!switches.shader_bundle.empty()) {
    return MobileGenerateShaderBundle(switches);
  }
  std::cerr << "NOT FINISHED YET " << std::endl;
  return nullptr;
}

}  // namespace compiler
}  // namespace impeller

// int check(int argc, char const* argv[]) {
//   return impeller::compiler::Main(
//       fml::CommandLineFromPlatformOrArgcArgv(argc, argv));
//   // return impeller::compiler::MobileMain("213", {{"dwa", "wa"}});
// }

int main(int argc, char const* argv[]) {
  impeller::compiler::MobileMain(jsonBundlk,
                                 {{"shaders1/simple.vert", simpleVertShader},
                                  {"shaders1/simple.frag", simpleFragShader}});
  return 0;
  // return impeller::compiler::Main(
  //     fml::CommandLineFromPlatformOrArgcArgv(argc, argv));
}
