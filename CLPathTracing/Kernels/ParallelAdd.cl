__kernel void main(__global float* x, __global float* y, __global float* z) {
	const int i = get_global_id(0); //Return a unique number to indentify the work item in the global pool
	z[i] = x[i] + y[i];
}