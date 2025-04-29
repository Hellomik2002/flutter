
#include "impeller/compiler/mobile/switches_mobile.h"

#include "impeller/compiler/compiler.h"
#include "impeller/compiler/reflector.h"
#include "impeller/compiler/shader_bundle.h"
#include "impeller/compiler/utilities.h"
#include "impeller/runtime_stage/runtime_stage.h"
#include "impeller/shader_bundle/shader_bundle_flatbuffers.h"
#include "third_party/json/include/nlohmann/json.hpp"

namespace impeller {
namespace compiler {

std::shared_ptr<fml::Mapping> MobileGenerateShaderBundle(
    MobileSwitches& switches);

}  // namespace compiler
}  // namespace impeller