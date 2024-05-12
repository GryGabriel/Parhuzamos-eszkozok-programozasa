#include <stdio.h>
#include <OpenCL/cl.h>

int main(int argc, char** argv){
	char* file_name = argv[1];
	FILE* fp = open_file(file_name);

	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;

	cl_int result;

	//Initialize the platform
	result = clGetPlatformIDs(
		1, &platform, NULL
	);
	if(result != CL_SUCCESS){
		printf("Couldn't get platform!\n");
		return 1;
	}

	//Initialize device
	result = clGetDeviceIDs(
		platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL
	);
	if(result != CL_SUCCESS){
		printf("Couldn't get device!\n");
		return 2;
	}

	//Create context
	context = clCreateContext(
		NULL, 1, &device, NULL, NULL, &result
	);
	if(result != CL_SUCCESS){
		printf("Couldn't create context!\n");
		return 3;
	}

	queue = clCreateCommandQueue(
		context, device, 0, &result
	);
	if(result != CL_SUCCESS){
		printf("Couldn't create queue!\n");
		return 4;
	}


	return 0;
	/*
	cl_device_id gpu;
	cl_context context;
	cl_int result;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;
	
	size_t count = 3;

	float in[3] = {9, 25, 64};
	float out[3] = {0, 0, 0};
	cl_mem buffer_in, buffer_out;

	FILE* fp;
	char kernel_code[2024];

	fp = fopen("./scripts/kernel.cl", "r");
	if(fp == NULL){
		printf("Couldn't open kernel.cl!\n");
		return 3;
	}
	fread(kernel_code, 1, 2024, fp);
	fclose(fp);

	program = clCreateProgramWithSource(
		context, 1, (const char*[]) {kernel_code}, NULL, &result
	);

	if(result != CL_SUCCESS){
		printf("Couldn't create program!\n");
		return 4;
	}

	result = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if(result != CL_SUCCESS){
		printf("Couldn't build program!\n");
		return 5;
	}

	
	buffer_in = clCreateBuffer(
		context, CL_MEM_READ_ONLY, sizeof(float) * 3, NULL, &result	
	);
	if(result != CL_SUCCESS){
		printf("Couldn't initialize input buffer!\n");
		return 6;
	}

	buffer_out = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, sizeof(float) * 3, NULL, &result	
	);
	if(result != CL_SUCCESS){
		printf("Couldn't initialize output buffer!\n");
		return 7;
	}

	result = clEnqueueWriteBuffer(
		queue, buffer_in, CL_TRUE, 0, sizeof(float) * 3, in, 0, NULL, NULL
	);
	if(result != CL_SUCCESS){
		printf("Couldn't enque write buffer!\n");
		return 8;
	}

	kernel = clCreateKernel(
		program, "square_root", NULL
	);
	clSetKernelArg(
		kernel, 0, sizeof(cl_mem), &buffer_in
	);
	clSetKernelArg(
		kernel, 1, sizeof(cl_mem), &buffer_out
	);

	clEnqueueNDRangeKernel(
		queue, kernel, 1, NULL, &count, NULL, 0, NULL, NULL
	);

	clFinish(queue);

	clEnqueueReadBuffer(
		queue, buffer_out, CL_TRUE, 0, sizeof(float) * 3, out, 0, NULL, NULL
	);

	for(int i=0; i<3; i++){
		printf("%f\n", out[i]);
	}*/

}

FILE* open_file(char* file_name){
	FILE* fp = fopen(file_name, "r");
	if(fp == NULL){
		
	}
}
