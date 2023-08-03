// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once

#ifdef IS_CUDA
#include <npp.h>
#include <math_constants.h>
#else
#include <limits>
#define 	NPP_MIN_32U   ( 0 )
#define 	NPP_MAX_32U   ( 4294967295U )
#define 	NPP_MIN_32S   (-2147483647 - 1 )
#define 	NPP_MAX_32S   ( 2147483647 )
#define 	NPP_MIN_64U   ( 0 )
#define 	NPP_MAX_64U   ( 18446744073709551615ULL )
#define 	NPP_MIN_64S   (-9223372036854775807LL - 1)
#define 	NPP_MAX_64S   ( 9223372036854775807LL )
#define     CUDART_INF_F  std::numeric_limits<float>::infinity()
#define     CUDART_INF    std::numeric_limits<double>::infinity()
#endif


template<class T> __device__ T numeric_min();
template<class T> __device__ T numeric_max();

template<> __device__ __inline__ int numeric_max<int>() { return NPP_MAX_32S; };
template<> __device__ __inline__ int numeric_min<int>() { return NPP_MIN_32S; };

template<> __device__ __inline__ unsigned int numeric_max<unsigned int>() { return NPP_MAX_32U; };
template<> __device__ __inline__ unsigned int numeric_min<unsigned int>() { return NPP_MIN_32U; };

template<> __device__ __inline__ long long numeric_max<long long>() { return NPP_MAX_64S; };
template<> __device__ __inline__ long long numeric_min<long long>() { return NPP_MIN_64S; };

template<> __device__ __inline__ unsigned long long numeric_max<unsigned long long>() { return NPP_MAX_64U; };
template<> __device__ __inline__ unsigned long long numeric_min<unsigned long long>() { return NPP_MIN_64U; };


template<> __device__ __inline__ float numeric_max<float>() { return CUDART_INF_F; };
template<> __device__ __inline__ float numeric_min<float>() { return -CUDART_INF_F; };

template<> __device__ __inline__ double numeric_max<double>() { return CUDART_INF; };
template<> __device__ __inline__ double numeric_min<double>() { return -CUDART_INF; };