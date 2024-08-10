#pragma once

// User defined optional macros
// ARGLESS_DO_NOT_UNDEF - If defined, all macros will not be undefined
// ARGLESS_OUT - If defined, the output stream will be set to the specified value

#include "Argless/Core.hpp"

#include "Argless/Argless.hpp"
#include "Argless/Result.hpp"

#include "Argless/RuleVector.hpp"
#include "Argless/RuleBasicString.hpp"

#if !ARGLESS_DO_NOT_UNDEF
#include "Argless/Core_undef.hpp"
#endif
