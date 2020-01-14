#include <rocblas.h>
#include <hip/hip_runtime.h>
#include <iostream>

__attribute__((visibility("default"))) void hip_alloc(void **ptr, int size) {
  hipMalloc(ptr, size);
}

__attribute__((visibility("default"))) void hip_free(void *ptr) {
  hipFree(ptr);
}

__global__ void FillKernel(float *ptr, float v, size_t N) {
  size_t offset = hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x;
  size_t stride = hipBlockDim_x * hipGridDim_x;

  for (size_t i = offset; i < N; i += stride) {
    ptr[i] = v;
  }
}

void hip_fill(void *ptr, float v, int size) {
  hipLaunchKernelGGL(FillKernel, dim3(1), dim3(256), 0, 0, static_cast<float*>(ptr), v, static_cast<size_t>(size));
}

__global__ void VecAddKernel(float *A, float *B, float *C, size_t N) {
  size_t offset = hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x;
  size_t stride = hipBlockDim_x * hipGridDim_x;

  for (size_t i = offset; i < N; i += stride) {
    C[i] = A[i] + B[i];
  }
}

void hip_vecadd(void *A, void *B, void *C, int size) {
  hipLaunchKernelGGL(VecAddKernel, dim3(1), dim3(256), 0, 0, static_cast<float*>(A), static_cast<float*>(B), static_cast<float*>(C), static_cast<size_t>(size));
}

void hip_memcpydtoh(void *host, void *device, int bytes) {
  hipMemcpy(host, device, bytes, hipMemcpyDeviceToHost);
}

void rocblas_sgemm(float *da, float *db, float *dc, int m, int n, int k, int lda, int ldb, int ldc) {
  rocblas_operation transa = rocblas_operation_none, transb = rocblas_operation_transpose;
  float alpha = 1.0f, beta = 1.0f;

  rocblas_handle handle;
  rocblas_create_handle(&handle);
  rocblas_sgemm(handle, transa, transb, m, n, k, &alpha, da, lda, db, ldb, &beta, dc, ldc);
  rocblas_destroy_handle(handle);
}