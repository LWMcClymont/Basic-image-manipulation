#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <fstream>

// OpenCL header
#include <CL/cl.hpp>

// Helper files from tutorials
#include "common.h"
#include "bmpfuncs.h"

int main()
{
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device

	// Input and output objects
	unsigned char* inputImage;
	unsigned char* outputImage1;
	unsigned char* outputImage2;
	unsigned char* outputImage3;

	// Image info
	int imageWidth, imageHeight, imageSize;

	cl::ImageFormat imageFormat;
	cl::Image2D inputBuffer, outputBuffer1, outputBuffer2, outputBuffer3;

	try
	{
		// Select the Device
		if (!select_one_device(&platform, &device))
		{
			quit_program("Device not selected.");
		}

		// Create the context using selected device
		context = cl::Context(device);

		// Build the program
		if (!build_program(&program, &context, "task.cl"))
		{
			quit_program("OpenCL program build error.");
		}

		// Create the kernel 
		kernel = cl::Kernel(program, "task");

		// Create the commandqueue
		queue = cl::CommandQueue(context, device);

		// Read the input file
		inputImage = read_BMP_RGB_to_RGBA("peppers.bmp", &imageWidth, &imageHeight);

		// Allocate memory for the outputs
		imageSize = imageWidth * imageHeight * 4;
		outputImage1 = new unsigned char[imageSize];
		outputImage2 = new unsigned char[imageSize];
		outputImage3 = new unsigned char[imageSize];

		// Set the image format
		imageFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8); // CL_UNORM_INT8 = 0.0-1.0

		// Create the input image buffer
		inputBuffer = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imageFormat, imageWidth, imageHeight, 0, (void*)inputImage);

		// Create the output image buffers
		outputBuffer1 = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imageFormat, imageWidth, imageHeight, 0, (void*)outputImage1);
		outputBuffer2 = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imageFormat, imageWidth, imageHeight, 0, (void*)outputImage2);
		outputBuffer3 = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imageFormat, imageWidth, imageHeight, 0, (void*)outputImage3);

		// Set kernal arguments
		kernel.setArg(0, inputBuffer);
		kernel.setArg(1, outputBuffer1);
		kernel.setArg(2, outputBuffer2);
		kernel.setArg(3, outputBuffer3);

		// Enqueue the kernel
		cl::NDRange offset(0, 0);
		cl::NDRange globalSize(imageWidth, imageHeight);

		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		std::cout << "Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// Read images from device to host
		cl::size_t<3> origin, region;
		origin[0] = origin[1] = origin[2] = 0;
		region[0] = imageWidth;
		region[1] = imageHeight;
		region[2] = 1;

		queue.enqueueReadImage(outputBuffer1, CL_TRUE, origin, region, 0, 0, outputImage1);
		queue.enqueueReadImage(outputBuffer2, CL_TRUE, origin, region, 0, 0, outputImage2);
		queue.enqueueReadImage(outputBuffer3, CL_TRUE, origin, region, 0, 0, outputImage3);

		// Write output to file
		write_BMP_RGBA_to_RGB("Taska.bmp", outputImage1, imageWidth, imageHeight);
		write_BMP_RGBA_to_RGB("Taskb.bmp", outputImage2, imageWidth, imageHeight);
		write_BMP_RGBA_to_RGB("Taskc.bmp", outputImage3, imageWidth, imageHeight);

		std::cout << "Completed!" << std::endl;

		// Deallocate memory
		free(inputImage);
		free(outputImage1);
		free(outputImage2);
		free(outputImage3);

	}
	catch (cl::Error e) {
		// call function to handle errors
		handle_error(e);
	}

	std::cout << "\npress a key to quit...";
	std::cin.ignore();

	return 0;
}