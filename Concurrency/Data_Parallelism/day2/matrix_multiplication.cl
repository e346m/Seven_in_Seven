__kernel void matrix_multiplication(uint widthA,
                                    __global const float* inputA,
                                    __global const float* inputB,
                                    __global float* output) {

  int i = get_global_id(0);
  int j = get_global_id(1);

  int outputWidth = get_global_size(0);
  int outputHeight = get_global_size(1);
  int widthB = outputWidth;

  float total = 0.0;
  for (int k = 0; k < widthA; ++k) {
      total += inputA[j * widthA + k] * inputB[k * widthB +i];
  }
  output[j * outputWidth + i] = total;
}
