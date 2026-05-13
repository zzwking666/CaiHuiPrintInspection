#include <cstdlib>
#include <cuda_runtime_api.h>
#include <iostream>
#include <stdio.h>

__device__ float addFromGPU(float A, float B)
{
	return A + B;
}

__global__ void addFromGPU(float * A,float * B,float * C,const int N)
{
	const int bid = blockIdx.x;
	const int tid = threadIdx.x;
	const int i = bid * blockDim.x + tid;

	C[i] = addFromGPU(A[i],B[i]);
}

void iniData(float * addr,int elemCount)
{
	for (int i=0;i<elemCount;i++)
	{
		addr[i] = static_cast<float>(rand()&0xff)/10.f;
	}
	return;
}

int main()
{

	int iDeviceCount = 0;
	auto error = cudaGetDeviceCount(&iDeviceCount);
	if (error!=cudaSuccess)
	{
		std::cout << "Error: " << cudaGetErrorString(error) << std::endl;
		exit(-1);
	}

	if (iDeviceCount == 0)
	{
		std::cout << "No CUDA devices found." << std::endl;
		exit(-1);
	}
	std::cout << "Number of CUDA devices: " << iDeviceCount << std::endl;

	auto errorSetDevice = cudaSetDevice(iDeviceCount-1);
	if (errorSetDevice != cudaSuccess)
	{
		std::cout << "Error setting device: " << cudaGetErrorString(errorSetDevice) << std::endl;
		exit(-1);
	}
	int iDevice;
	error = cudaGetDevice(&iDevice);
	if (error != cudaSuccess)
	{
		std::cout << "Error getting device: " << cudaGetErrorString(error) << std::endl;
		exit(-1);
	}


	int iElemCount = 512;
	size_t stBytesCount = iElemCount * sizeof(float);

	auto fpHost_A = static_cast<float*>(malloc(stBytesCount));
	auto fpHost_B = static_cast<float*>(malloc(stBytesCount));
	auto fpHost_C = static_cast<float*>(malloc(stBytesCount));
	if (!fpHost_A || !fpHost_B || !fpHost_C)
	{
		std::cout << "Error allocating host memory." << std::endl;
		exit(-1);
	}

	memset(fpHost_A, 0, stBytesCount);
	memset(fpHost_B, 0, stBytesCount);
	memset(fpHost_C, 0, stBytesCount);

	float* fpDevice_A, * fpDevice_B, * fpDevice_C;
	cudaMalloc(reinterpret_cast<void**>(&fpDevice_A), stBytesCount);
	cudaMalloc(reinterpret_cast<void**>(&fpDevice_B), stBytesCount);
	cudaMalloc(reinterpret_cast<void**>(&fpDevice_C), stBytesCount);

	if (!fpDevice_A||!fpDevice_B||!fpDevice_C)
	{
		std::cout << "Error allocating device memory." << std::endl;
		free(fpHost_A);
		free(fpHost_B);
		free(fpHost_C);
		exit(-1);
	}
	else
	{
		cudaMemset(fpDevice_A, 0, stBytesCount);
		cudaMemset(fpDevice_B, 0, stBytesCount);
		cudaMemset(fpDevice_C, 0, stBytesCount);
	}
	srand(666);

	iniData(fpHost_A, iElemCount);
	iniData(fpHost_B, iElemCount);

	cudaMemcpy(fpDevice_A, fpHost_A, stBytesCount,cudaMemcpyKind::cudaMemcpyHostToDevice);
	cudaMemcpy(fpDevice_B, fpHost_B, stBytesCount, cudaMemcpyKind::cudaMemcpyHostToDevice);
	cudaMemcpy(fpDevice_C, fpHost_C, stBytesCount, cudaMemcpyKind::cudaMemcpyHostToDevice);

	dim3 blockSize(32);
	dim3 gridSize(iElemCount/32);
	addFromGPU << <gridSize, blockSize >> > (fpDevice_A, fpDevice_B, fpDevice_C, iElemCount);

	cudaDeviceSynchronize();

	cudaMemcpy(fpHost_C, fpDevice_C, stBytesCount, cudaMemcpyKind::cudaMemcpyDeviceToHost);

	for (int i=0;i<10;i++)
	{
		printf("idx=%2d\tmatrix_A:%.2f,matrix_B:%.2f\tresult=%.2f\n", i+1,fpHost_A[i], fpHost_B[i], fpHost_C[i]);
	}

	free(fpHost_A);
	free(fpHost_B);
	free(fpHost_C);
	cudaFree(fpDevice_A);
	cudaFree(fpDevice_B);
	cudaFree(fpDevice_C);

	return 0;
}