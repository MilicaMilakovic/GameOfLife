struct Pixel
{	
	unsigned char r, g ,b;
};

bool blinker(__global int* currentState, __global int* nextState, __global struct Pixel* image, int i, int j, int width, int height)
{
	//if (i == 0 || i == width - 1 || j == 0 || j == width - 1)
	//	return false;

	if (j + 1 > width || i + 1 > width || j - 1 < 0 || i - 1 < 0)
		return false;
	
	int alive1 = 0, alive2 = 0;

	if(currentState[j * width + i] == 1 && nextState[j * width + i] == 1)
		for (int tempI = -1; tempI < 2; tempI++)
		{
			for (int tempJ = -1; tempJ < 2; tempJ++)
			{
				alive1 += currentState[(j + tempJ) * width + i + tempI];
				alive2 += nextState[(j + tempJ) * width + i + tempI];
			}			
		}

	if (alive1 == 3 && alive2 == 3)		
	{
		// Horizontalno tri u trenutnom stanju i verikalno tri u sljedecem
		if ((currentState[j * width + i] == 1 && currentState[j * width + i - 1] == 1 && currentState[j * width + i + 1] == 1) &&
			(nextState[j * width + i] == 1 && nextState[(j - 1) * width + i] == 1 && nextState[(j + 1) * width + i] == 1))
			return true;

		// Vertikalno tri u ovom i horizontalno tri u sljedecem
		if ((currentState[j * width + i] == 1 && currentState[(j - 1) * width + i] == 1 && currentState[(j + 1) * width + i] == 1) &&
			(nextState[j * width + i] == 1 && nextState[j * width + i - 1] == 1 && nextState[j * width + i + 1] == 1))
			return true;
	}
	return false;
}

__kernel void gameOfLife(__global struct Pixel* image, const int width, const int height, __global int* currentState, __global int* nextState) 
{
	int i = get_global_id(0);
	int j = get_global_id(1);	


	int livingNeighbours = 0;

	// Racunanje koliko zivih celija se nalazi u okolini trenutnog polja
	for (int tempI = -1; tempI < 2; tempI++)
		for (int tempJ = -1; tempJ < 2; tempJ++)
			livingNeighbours +=currentState[(j+tempJ)*width + i + tempI];

	// Oduzimamo vrijednost polja za kojeg gledamo zive komsije, njega ne treba racunati
	livingNeighbours -=currentState[j*width + i];

	// Provjera GOL uslova:

	// Ziv i ima manje od 2 zive komsije - umire (underpopulation)
	if((currentState[j*width + i] == 1) && (livingNeighbours < 2))
	{
			nextState[j*width + i] = 0;
			image[j*width + i].r = 0;
			image[j*width + i].g = 0;
			image[j*width + i].b = 0;
	}
	// Ziv sa vise od tri komsije - umire (overpopulation)
	else if((currentState[j*width + i] == 1) && (livingNeighbours > 3))
	{
			nextState[j*width + i] = 0;
			image[j*width + i].r = 0;
			image[j*width + i].g = 0;
			image[j*width + i].b = 0;
	}
	// Mrtav sa tacno tri zive celije - zivi (reproduction)
	else if((currentState[j*width + i] == 0) && (livingNeighbours == 3))
	{		
		nextState[j*width + i] = 1;
		image[j*width + i].g = 255;
		image[j * width + i].r = 255;
		image[j * width + i].b = 255;
	}
	// Ostaje ista u sljedecoj generaciji
	else
	{
		nextState[j*width + i] = currentState[j*width + i];	
	}	

	// U slucaju da se detektuje oscilatorni obrazac blinker, centralna celija bice obojena u crveno
	if (blinker(currentState, nextState, image, i, j, width, height) == true)
	{
		image[j * width + i].r = 255;
		image[j * width + i].g = 0;
		image[j * width + i].b = 0;
	}
}
