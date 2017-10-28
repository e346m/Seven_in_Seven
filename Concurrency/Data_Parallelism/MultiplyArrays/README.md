if you compile it in macOS, you have to point framework under the System otherwise compile fails due to missing header
`gcc -o BinaryName -Wall multiply_arrays.c -framework OpenCL`
