#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include <stdio.h>
#include <float.h>

#define WIDTH_A 20
#define HEIGHT_A 40
#define ELEMENTS_A (WIDTH_A * HEIGHT_A)

#define WIDTH_B 30
#define HEIGHT_B 20
#define ELEMENTS_B (WIDTH_B * HEIGHT_B)

#define WIDTH_OUTPUT WIDTH_B
#define HEIGHT_OUTPUT HEIGHT_A
#define ELEMENTS_OUTPUT (WIDTH_OUTPUT * HEIGHT_OUTPUT)

#define CHECK_STATUS(s) do { \
  cl_int ss = (s); \
  if (ss != CL_SUCCESS) { \
    fprintf(stderr, "Error %d at line %d\n", ss, __LINE__); \
    exit(1); \
  } \
} while (0)

char* read_source(const char* src);
static void random_fill(cl_float array[], size_t size);
int main() {
  cl_int status;

  cl_platform_id platform;
  CHECK_STATUS(clGetPlatformIDs(1, &platform, NULL));

  cl_device_id device;
  CHECK_STATUS(clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL));

  cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
  CHECK_STATUS(status);

  cl_command_queue queue = clCreateCommandQueue(context, device, 0, &status);
  CHECK_STATUS(status);

  char* source = read_source("matrix_multiplication.cl");
  cl_program program = clCreateProgramWithSource(context, 1,
      (const char**)&source, NULL, NULL);
  CHECK_STATUS(status);
  free(source);

  CHECK_STATUS(clBuildProgram(program, 0, NULL, NULL, NULL, NULL));

  cl_kernel kernel = clCreateKernel(program, "matrix_multiplication", &status);
  CHECK_STATUS(status);

  cl_float a[ELEMENTS_A], b[ELEMENTS_B];
  random_fill(a, ELEMENTS_A);
  random_fill(b, ELEMENTS_B);

  cl_mem inputA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(cl_float) * ELEMENTS_A, a, NULL);
  cl_mem inputB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(cl_float) * ELEMENTS_B, b, NULL);
  cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
      sizeof(cl_float) * ELEMENTS_OUTPUT, NULL, NULL);

  cl_int widthA = WIDTH_A;
  clSetKernelArg(kernel, 0, sizeof(cl_mem), &widthA);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), &inputA);
  clSetKernelArg(kernel, 2, sizeof(cl_mem), &inputB);
  clSetKernelArg(kernel, 3, sizeof(cl_mem), &output);

  cl_event timing_event;
  size_t work_units[] = {WIDTH_OUTPUT, HEIGHT_OUTPUT};
  CHECK_STATUS(clEnqueueNDRangeKernel(queue, kernel, 2, NULL, work_units,
        NULL, 0, NULL, &timing_event));
  cl_float results[ELEMENTS_OUTPUT];
  CHECK_STATUS(clEnqueueReadBuffer(queue, output, CL_TRUE, 0, sizeof(cl_float) * ELEMENTS_OUTPUT,
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

  printf("Elapsed (GPU): %lu ns\n\n", (unsigned long)(endtime - starttime));
  return 0;
}

char* read_source(const char* src) {
  FILE *f = fopen(src, "r");
  if(!f) {
    fprintf(stderr, "Unable to open file %s\n", src);
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  size_t s = ftell(f);
  rewind(f);
  char* program = (char*)malloc(s + 1);
  fread(program, sizeof(char), s, f);
  program[s] = '\0';
  fclose(f);
  return program;
}

static void random_fill(cl_float array[], size_t size) {
  for(size_t i = 0; i < size; ++i) array[i] = (cl_float)rand() / RAND_MAX;
}
