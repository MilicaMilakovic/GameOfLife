struct Pixel
{	
	unsigned char r, g ,b;
};

bool middle(int i, int j, int width, int height, __global int *array) //pomocne funkcije za prepoznavanje oscilatornog obrazca
{
	int temp=0;
	for (int tempI = -2; tempI<3; tempI++)
		for (int tempJ = -2; tempJ<3; tempJ++)
			temp+=array[(j+tempJ)*width + i + tempI];
	if(temp == 3)
		return true;
	else
		return false;
}
int leftOne(int i, int j, int width, int height, __global int *array)
{
	int temp=0;
	for(int tempI = -1; tempI<4; tempI++)
		for(int tempJ = -2; tempJ<3; tempJ++)
			temp+=array[(j+tempJ)*width + i + tempI];
	if(temp == 3)
		return true;
	else 
		return false;
}
int rightOne(int i, int j, int width, int height, __global int *array)
{
	int temp=0;
	for(int tempI = -3; tempI<2; tempI ++)
		for(int tempJ = -2; tempJ<3; tempJ++)
			temp+=array[(j+tempJ)*width + i + tempI];
	if(temp == 3)
		return true;
	else 
		return false;
}
int upperMiddle(int i, int j, int width, int height, __global int *array)
{
	int temp=0;
	for(int tempI=-2; tempI<3; tempI++)
		for(int tempJ=-1; tempJ<4; tempJ++)
			temp+=array[(j+tempJ)*width + i + tempI];
	if(temp == 3)
		return true;
	else 
		return false;
}
int bottomMiddle(int i, int j, int width, int height, __global int *array)
{
	int temp=0;
	for(int tempI=-2; tempI<3; tempI++)
		for(int tempJ=-3; tempJ<2; tempJ++)
			temp+=array[(j+tempJ)*width + i + tempI];
	if(temp == 3)
		return true;
	else 
		return false;
}

bool oscilator(int i, int j, int width, int height, __global struct Pixel *matrix, __global int *array, __global int *array1)  //provjeravanje oscilatornog obrasca na osnovu celija koje okruzuju dati oblik
{
	if ((i > 2) && (i < width - 2) && (j > 2) && (j < height - 2))
	{
		bool triCase=false;
		int temp = 0 ;
		if (array[j*width + i]==1 && (array[j*width + i - 1]==1 && array[j*width + i + 1]==1) || (array[(j-1)*width + i]==1 && array[(j+1)*width + i]==1))
			{
				triCase = middle(i, j, width, height, array);
				return triCase;
			}
		else if (array[j*width + i - 1]==1 && array[(j-1)*width + i - 1]==1 && array[(j+1)*width + i - 1]==1)
			{
				triCase = rightOne(i, j, width, height, array);
				return triCase;
			}
		else if (array[j*width + i + 1]==1 && array[(j-1)*width + i + 1]==1 && array[(j+1)*width + i + 1]==1)
			{
				triCase = leftOne(i, j, width, height, array);
				return triCase;
			}
		else if (array[(j+1)*width + i]==1 && array[(j+1)*width + i + 1]==1 && array[(j+1)*width + i - 1]==1)
			{
				triCase = upperMiddle(i, j, width, height, array);
				return triCase;
			}
		else if (array[(j-1)*width + i]==1 && array[(j-1)*width + i + 1]==1 && array[(j-1)*width + i - 1]==1)
		{
			triCase = bottomMiddle(i, j, width, height, array);
				return triCase;
		}
		else
			return false;
	}
	else
		return false;
}

__kernel void gameOfLife(__global struct Pixel* matrix, const int width, const int height, __global int* array, __global int* array1) // 2 pomocna niza od 0 i 1
{
	int i = get_global_id(0);
	int j = get_global_id(1);
	int iL = get_local_id(0);
	int jL = get_local_id(1);

	if (i == 0 || i == width - 1 || j == 0 || j == width - 1)
	{
		array1[j*width + i] = array[j*width + i];
		return;
	}

	int alive = 0;
	for (int tempI = -1; tempI < 2; tempI++)
		for (int tempJ = -1; tempJ < 2; tempJ++)
			alive+=array[(j+tempJ)*width + i + tempI];
	alive-=array[j*width + i];


	if((array[j*width + i] == 1) && (alive < 2))
		{
			array1[j*width + i] = 0;
			matrix[j*width + i].r = 0;
			matrix[j*width + i].g = 0;
			matrix[j*width + i].b = 0;
		}
	else if((array[j*width + i] == 1) && (alive > 3))
		{
			array1[j*width + i] = 0;
			matrix[j*width + i].r = 0;
			matrix[j*width + i].g = 0;
			matrix[j*width + i].b = 0;
		}
	else if((array[j*width + i] == 0) && (alive == 3))	
		{
			if (oscilator(i, j, width, height, matrix, array, array1)==true)
				{
					matrix[j*width + i].r = 0;
					matrix[j*width + i].b = 0;
				}
			else 
				{	
					matrix[j*width + i].r = 255;
					matrix[j*width + i].b = 255;
				}
			array1[j*width + i] = 1;
			matrix[j*width + i].g = 255;
		}
	else
		{
			array1[j*width + i] = array[j*width + i];
			if (oscilator(i, j, width, height, matrix, array, array1)==true)
				{
					matrix[j*width + i].r = 0;
					matrix[j*width + i].g = 255;
					matrix[j*width + i].b = 0;
				}
			else if (matrix[j*width + i].g==255 && oscilator(i, j, width, height, matrix, array, array1)==false)
				{
					matrix[j*width + i].r = 255;
					matrix[j*width + i].g = 255;
					matrix[j*width + i].b = 255;
				}
		}	
}

__kernel void subSegment(__global int* array1, __global int* array2, const int subSegmentWidth, const int subSegmentHeight, const int width, const int height)
{
    int i = get_global_id(0);
	int j = get_global_id(1);
	int lI = get_local_id(0);
	int lJ = get_local_id(1);

	array2[j*subSegmentWidth + i] = array1[j*width+subSegmentHeight + i + subSegmentWidth];
}