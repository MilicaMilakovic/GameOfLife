#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.hpp>
#include <string>
#include <fstream>

struct Pixel
{
	unsigned char r, g, b;
};

char* readKernelSource(const char* filename) 
{
	char* kernelSource = nullptr;
	long length;
	FILE* f = fopen(filename, "r");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		kernelSource = (char*)calloc(length, sizeof(char));
		if (kernelSource)
			fread(kernelSource, 1, length, f);
		fclose(f);
	}
		
	return kernelSource;
} 

void writeImage(const char* filename, const Pixel* array, const int width, const int height) //funkcija za ispis slika kao iteracija igre
{
	FILE* fp = fopen(filename, "wb"); /* b - binary mode */
	fprintf(fp, "P6\n%d %d\n255\n", width, height);
	fwrite(array, sizeof(Pixel), (size_t)width * (size_t)height, fp);
	fclose(fp);
}

void writeMatrix(int width, int height)  
{
	std::ofstream outfile("mat.txt");
	const char* values[] = { "0 ", "1 " };
	for (int i = 0; i < height; i++)
	{	
		for (int j = 0; j < width; j++)
			outfile << rand() % 2 << " ";
		outfile << std::endl;
	}
	outfile.close();
}


void loadImage(const char* filename, int*& array, int width, int height)
{
	FILE* fp = fopen(filename, "rb"); /* b - binary mode */
	if (!fscanf(fp, "P6\n%d %d\n255\n", &width, &height)) {
		throw "error";
	}

	size_t imageSize = width * height*sizeof(Pixel);
	Pixel* image = new Pixel[(size_t)width * height];
	fread(image, sizeof(Pixel), (size_t)width * (size_t)height, fp);
	fclose(fp);

	array = (int*)malloc(width * height);
	std::cout << "\n" << width << "x" << height <<"\n";

	for (int i = 0; i < imageSize; i++)
	{
		if (image[i].r == 0 && image[i].b == 0 && image[i].g == 0)
			array[i] = 0;
		else
			array[i] = 1;
	}

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			std::cout << array[j*width+i] << " ";
		}
		std::cout << "\n";
	}
	
}

int *subSegment(int width, int height, int* array); //funkcija za dobavljanje podsegmenta

int main()
{	
	int in = 0;
	std::cout << " Podesavanje inicijalnih parametara simulacije: \n" << "  1 Prosljedjivanjem binarne matrice [ bigInput.txt ].\n" << "  2 Prosljedjivanjem slike [ imageInput.ppm ].\n";
	std::cin >> in;

	int width = 0;
	int height = 0;
	int imageSize=0;
	size_t bytes;
	size_t bytes1;
	Pixel* image = nullptr;
	int* currentState = nullptr;
	int* nextState = nullptr;
		
	switch (in)
	{	
		case 1:
		{
			width = 256;
			height = 256;
			imageSize = width * height;
			bytes = width * height * sizeof(Pixel);
			bytes1 = width * height * sizeof(int);
			image = (Pixel*)malloc(bytes);
			currentState = (int*)malloc(bytes1);
			nextState = (int*)malloc(bytes1);

			std::ifstream readFile("bigInput.txt", std::ios_base::in);

			for (int i = 0; i < imageSize; i++)
			{
				// Matricu nula i jedinica preslikavam u sliku
				// Bijela polja su jedinice, to su zive celije
				// Crna polja su nule, to su mrtve celije

				readFile >> currentState[i];
				if (currentState[i] == 0) //crna
				{
					image[i].r = 0;
					image[i].g = 0;
					image[i].b = 0;
				}
				if (currentState[i] == 1) //bijela
				{
					image[i].r = 255;
					image[i].g = 255;
					image[i].b = 255;
				}
			}
			readFile.close();

			// Ucitanu matricu piksela upisujem u fajl - sliku
			writeImage("image.ppm", image, width, height);

			break;
		}

		case 2 :
		{
			FILE* fp = fopen("imageInput.ppm", "rb"); /* b - binary mode */
			if (!fscanf(fp, "P6\n%d %d\n255\n", &width, &height)) {
				throw "error";
			}

			imageSize = width * height;
			bytes = width * height * sizeof(Pixel);
			bytes1 = width * height * sizeof(int);

			image = (Pixel*)malloc(bytes);
			currentState = (int*)malloc(bytes1);
			nextState = (int*)malloc(bytes1);			
		
			fread(image, sizeof(Pixel), (size_t)width * (size_t)height, fp);
			fclose(fp);
					
			//std::cout << "\n" << width << "x" << height << "\n";

			for (int i = 0; i < imageSize; i++)
			{
				if (image[i].r == 0 && image[i].b == 0 && image[i].g == 0)
					currentState[i] = 0;
				else
					currentState[i] = 1;
			}

			/* Ispis ucitane matrice
			 
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					std::cout << currentState[j * width + i] << " ";
				}
				std::cout << "\n";
			}*/

			break;
		}
		default:
		{
			std::cout << "Neispravan unos!";
			return 1;
		}			
	}

	
	//int width = 10;
	//int height = 10;	
	//int imageSize = width * height;
	
	//size_t bytes = width * height * sizeof(Pixel);
	//size_t bytes1 = width * height * sizeof(int);

	// slika
	 //image = (Pixel*)malloc(bytes); 
	// trenutno stanje
	 //currentState = (int*)malloc(bytes1); 
	// naredno stanje
	 //nextState = (int*)malloc(bytes1); 


	// Ucitam matricu nula i jedinica iz fajla
	//std::ifstream readFile("input.txt", std::ios_base::in);
	//
	//for (int i = 0; i < imageSize; i++)
	//{
	//	// Matricu nula i jedinica preslikavam u sliku
	//	// Bijela polja su jedinice, to su zive celije
	//	// Crna polja su nule, to su mrtve celije

	//	readFile >> currentState[i];
	//	if (currentState[i] == 0) //crna
	//	{
	//		image[i].r = 0;
	//		image[i].g = 0;
	//		image[i].b = 0;
	//	}
	//	if (currentState[i] == 1) //bijela
	//	{
	//		image[i].r = 255;
	//		image[i].g = 255;
	//		image[i].b = 255;
	//	}
	//}
	//readFile.close();

	//// Ucitanu matricu piksela upisujem u fajl - sliku
	//writeImage("image.ppm", image, width, height);		
	
	std::cout << "\n" <<"=== Unesite rezim rada: ===\n";
	std::cout << "  1 GameOfLife u 10 generacija...\n";
	std::cout << "  2 Prelazak na proizvoljnu iteraciju igre...\n";

	int mode=1;

	std::cin >> mode;

	int iteration = 0;

	switch (mode)
	{
		case 2:
		{
			std::cout << "Unesite broj iteracije od koje zelite krenuti (<10): \n";
			std::cin >> iteration;
		}
		case 1:
		{	
			for (int i = 0; i < 10; i++)
			{
				std::cout <<  "..";

				// Alociramo memoriju na uredjaju:

				// slika
				cl_mem d_image;		
				// binarna matrica koja predstavlja trenutno stanje
				cl_mem d_currentState;
				// binarna matrica koja predstavlja naredno stanje
				cl_mem d_nextState;

				cl_platform_id cpPlatform;	      // OpenCL platform
				cl_device_id device_id;           // device ID
				cl_context context;               // context
				cl_command_queue queue;           // command queue
				cl_program program;               // program
				cl_kernel kernel;                 // kernel

				// Niz za globalnu velicinu i niz za lokalnu velicinu
				size_t globalSize[2], localSize[2];	
				cl_int err;

				// Number of work items in each local work group
				localSize[0] = localSize[1] = 2;

				// Number of total work items - localSize must be devisor
				globalSize[0] = width;
				globalSize[1] = height;

				// Bind to platform
				err = clGetPlatformIDs(1, &cpPlatform, NULL);

				// Get ID for the device
				err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL); 

				// Create a context  
				context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

				// Create a command queue 
				queue = clCreateCommandQueue(context, device_id, 0, &err);

				// Ucitavanje kernela
				char* kernelSource = readKernelSource("gameOfLife.cl");

				// Create the compute program from the source buffer
				program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &err);

				// Build the program executable 
				err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

				if (err)
				{
					// Determine the size of the log
					size_t log_size;
					clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

					// Allocate memory for the log
					char* log = (char*)malloc(log_size);

					// Get the log
					clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

					// Print the log
					printf("%s\n", log);

					free(log);
				}
				
				// Create the compute kernel in the program we wish to run
				kernel = clCreateKernel(program, "gameOfLife", &err);
		

				// Create the input and output arrays in device memory for our calculation
				// Alociramo memoriju na uredjaju:
				// izlazna slika
				d_image = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL); 
				// trenutno stanje - binarna matrica
				d_currentState = clCreateBuffer(context, CL_MEM_READ_WRITE, bytes1, NULL, NULL);
				// naredno izracunato stanje
				d_nextState = clCreateBuffer(context, CL_MEM_READ_WRITE, bytes1, NULL, NULL);

				//Write our data set into the input array in device memory
				// Upisivanje matrica sa hosta na uredjaj

				err = clEnqueueWriteBuffer(queue, d_image, CL_TRUE, 0, bytes, image, 0, NULL, NULL); 
				err = clEnqueueWriteBuffer(queue, d_currentState, CL_TRUE, 0, bytes1, currentState, 0, NULL, NULL);
				err = clEnqueueWriteBuffer(queue, d_nextState, CL_TRUE, 0, bytes1, nextState, 0, NULL, NULL);

				// Set the arguments to our compute kernel
				// Postavljamo argumente kernela

				err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_image);		
				err |= clSetKernelArg(kernel, 1, sizeof(int), &width);
				err |= clSetKernelArg(kernel, 2, sizeof(int), &height);
				err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &d_currentState);		
				err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &d_nextState);


				clFinish(queue);

				// Execute the kernel over the entire range of the data set  
				// Pokrecemo kernel
				err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);

				// Wait for the command queue to get serviced before reading back results
				clFinish(queue);

				// Read the results from the device
				// Kopiramo rezultate sa uredjaja na host
				clEnqueueReadBuffer(queue, d_image, CL_TRUE, 0, bytes, image, 0, NULL, NULL);
				clEnqueueReadBuffer(queue, d_currentState, CL_TRUE, 0, bytes1, currentState, 0, NULL, NULL);
				clEnqueueReadBuffer(queue, d_nextState, CL_TRUE, 0, bytes1, nextState, 0, NULL, NULL);

				clFinish(queue);

				const std::string outFile = std::string("generation") + std::to_string(i + 1) + std::string(".ppm");  
				
				if (i >= iteration)
					writeImage(outFile.c_str(), image, width, height);

				// Naredno stanje u ovoj iteraciji predstavlja trenutno stanje za narednu iteraciju
				for (int i = 0; i < imageSize; i++)
					currentState[i] = nextState[i];

				// release OpenCL resources
				clReleaseMemObject(d_image);
				clReleaseMemObject(d_currentState);
				clReleaseMemObject(d_nextState);
				clReleaseProgram(program);
				clReleaseKernel(kernel);
				clReleaseCommandQueue(queue);
				clReleaseContext(context);

				// release host memory
				free(kernelSource);
			}
			break;
		}
		default:
		{
			std::cout << "Neispravan unos.\n";
		}
	}

	std::cout << "\n";

	// Oslobadjanje memorije
	free(image);  
	free(currentState);
	free(nextState);
	system("pause");
	return 0;
}
/*
int* subSegment(int width, int height, int* hostArray)
{
	int subSegmentWidth;
	int subSegmentHeight;
	size_t bytes = width * height * sizeof(int);
	
	//uzimamo pocetni piksel i na osnovu njega generisemo ostatak slike u desno i na dole
	std::cout << "Dohvatanje podsegmenta. Unesite vrijednost pocetne kordinate (_ , _) - (0,0) za citavu sliku. ";
	std::cin >> subSegmentWidth;
	std::cin >> subSegmentHeight;
	size_t bytes1 = (width - subSegmentWidth) * (height - subSegmentHeight) * sizeof(int);
	int *subArray = (int*)malloc(bytes1);
	
	if (subSegmentWidth == 0 && subSegmentHeight == 0)
	{
		subArray = nullptr;
		return subArray;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cl_mem deviceSubSegmentArray1;
	cl_mem deviceSubSegmentArray2;
	cl_platform_id cpPlatform;		// OpenCL platform
	cl_device_id device_id;           // device ID
	cl_context context;               // context
	cl_command_queue queue;           // command queue
	cl_program program;               // program
	cl_kernel kernel;
	cl_int err;// kernel

	size_t globalSize[2], localSize[2];	//niz za globalnu velicinu
	globalSize[0] = width - subSegmentWidth;
	globalSize[1] = height - subSegmentHeight;
	localSize[0] = 8;
	localSize[1] = 8;

	err = clGetPlatformIDs(1, &cpPlatform, NULL);
	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
	queue = clCreateCommandQueue(context, device_id, 0, &err);
	char* kernelSource = readKernelSource("OpenClFile.cl");
	program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &err);
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	kernel = clCreateKernel(program, "subSegment", &err);
	deviceSubSegmentArray1 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);
	deviceSubSegmentArray2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes1, NULL, NULL);

	err = clEnqueueWriteBuffer(queue, deviceSubSegmentArray1, CL_TRUE, 0, bytes, hostArray, 0, NULL, NULL);

	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &deviceSubSegmentArray1);	 //argumenti kernela
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &deviceSubSegmentArray2);

	err |= clSetKernelArg(kernel, 1, sizeof(int), &subSegmentWidth);
	err |= clSetKernelArg(kernel, 2, sizeof(int), &subSegmentHeight);
	err |= clSetKernelArg(kernel, 1, sizeof(int), &width);
	err |= clSetKernelArg(kernel, 2, sizeof(int), &height);
	clFinish(queue);
	err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
	clFinish(queue);
	clEnqueueReadBuffer(queue, deviceSubSegmentArray1, CL_TRUE, 0, bytes, hostArray, 0, NULL, NULL);
	clEnqueueReadBuffer(queue, deviceSubSegmentArray2, CL_TRUE, 0, bytes, subArray, 0, NULL, NULL);
	clFinish(queue);

	clReleaseMemObject(deviceSubSegmentArray1);
	clReleaseMemObject(deviceSubSegmentArray2);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	free(kernelSource);

	return subArray;
}
*/