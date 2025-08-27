#ifndef ROCWMMA_WRAPPER_H
#define ROCWMMA_WRAPPER_H

// This wrapper prevents header conflicts between system HIP and ROCm 6.4.1 installations
#ifdef __HIP_PLATFORM_AMD__

// Exclude problematic system headers during ROCWMMA inclusion
#pragma GCC system_header

// Define guard macros to prevent double inclusion of conflicting headers
#ifndef AMD_HIP_ATOMIC_GUARD
#define AMD_HIP_ATOMIC_GUARD
#ifndef HIP_INCLUDE_HIP_AMD_DETAIL_AMD_HIP_ATOMIC_H
#define HIP_INCLUDE_HIP_AMD_DETAIL_AMD_HIP_ATOMIC_H 0
#endif
#endif

// Include ROCWMMA from the specific ROCm installation
#include "/opt/rocm-6.4.3/include/rocwmma/rocwmma.hpp"

#endif // __HIP_PLATFORM_AMD__

#endif // ROCWMMA_WRAPPER_H
