#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include <stdio.h>

#define NUM_ELEMENTS 102400
#define CHECK_STATUS(s) do { \
  cl_int ss = (s); \
  if (ss != CL_SUCCESS) { \
    fprintf(stderr, "Error %d at line %d\n", ss, __LINE__); \
    exit(1); \
  } \
} while (0)

static char* read_source(const char* src);
static void random_fill(cl_float array[], size_t size);
int main() {
  cl_platform_id platform;
  clGetPlatformIDs(1, &platform, NULL);

  cl_device_id device;
  clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

  cl_int status;
  cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
  CHECK_STATUS(status);

  cl_command_queue queue = clCreateCommandQueue(context, device, 0, &status);
  CHECK_STATUS(status);

  char* source = read_source("multiply_arrays.cl");
  cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, NULL);
  CHECK_STATUS(status);
  free(source);

  clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  cl_kernel kernel = clCreateKernel(program, "multiply_arrays", &status);
  CHECK_STATUS(status);

  cl_float a[NUM_ELEMENTS], b[NUM_ELEMENTS];
  random_fill(a, NUM_ELEMENTS);
  random_fill(b, NUM_ELEMENTS);

  cl_mem inputA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(cl_float) * NUM_ELEMENTS, a, NULL);
  cl_mem inputB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(cl_float) * NUM_ELEMENTS, b, NULL);
  cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
      sizeof(cl_float) * NUM_ELEMENTS, NULL, NULL);
  clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputA);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), &inputB);
  clSetKernelArg(kernel, 2, sizeof(cl_mem), &output);

  cl_event timing_event;
  size_t work_units = NUM_ELEMENTS;
  CHECK_STATUS(clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &work_units, NULL, 0, NULL, &timing_event));
  cl_float results[NUM_ELEMENTS];
  CHECK_STATUS(clEnqueueReadBuffer(queue, output, CL_TRUE, 0, sizeof(cl_float) * NUM_ELEMENTS,
      results, 0, NULL, NULL));

  // Profile
  cl_ulong starttime;
  clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_START,
      sizeof(cl_ulong), &starttime, NULL);
  cl_ulong endtime;
  clGetEventProfilingInfo(timing_event, CL_PROFILING_COMMAND_END,
      sizeof(cl_ulong), &endtime, NULL);

  clReleaseEvent(timing_event);
  clReleaseMemObject(inputA);
  clReleaseMemObject(inputB);
  clReleaseMemObject(output);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);

  for( int i = 0; i < NUM_ELEMENTS; ++i) {
    printf("%f * %f = %f\n", a[i], b[i], results[i]);
  }
  printf("Elapsed (GPU): %lu ns\n\n", (unsigned long)(endtime - starttime));
}

static char* read_source(const char* src) {
  FILE *f = fopen(src, "r");
  fseek(f, 0, SEEK_END);
  size_t s = ftell(f);
  rewind(f);
  char* program = (char*)malloc(s+1);
  fread(program, sizeof(char), s, f);
  program[s] = '\0';
  fclose(f);
  return program;
}

static void random_fill(cl_float array[], size_t size) {
  for(size_t i = 0; i < size; ++i) array[i] = (cl_float)rand()/ RAND_MAX;
}
