
#include "SurfacePlotter.h"
#include <thread>
#include <functional>
#include  <chrono>

using namespace SurfacePlotting;

void updateSurface(Surface<float>* surface){
	float* data=reinterpret_cast<float*>(malloc(3564*sizeof(float)));
	float angle=0;
	float toAdd=2*PI/3564;
	while(true){
		for(std::size_t i=0;i<3564;i++){
			*(data+i)=16000*cos(angle);
			angle+=toAdd;
		}
		surface->addRow(data,UP);
		std::this_thread::sleep_for (std::chrono::milliseconds(10));



	}
}

int main(){
SurfacePlotter* test=new SurfacePlotter();
									//rows X columns
 Surface<float>* surface=test->addSurface<float>(3564,4096);
 float* data=reinterpret_cast<float*>(malloc(3564*4096*sizeof(float)));
 for(std::size_t i=0;i<3564*4096;i++){

	 *(data+i)=0;
	 }
surface->addData(data);
surface->setPixelsPerColumn(0.001);
surface->setPixelsPerRow(0.001);
free(data);

std::thread temp= std::thread(std::bind(updateSurface,surface));

//surface->addRow(data+4,shift=UP);
test->show();
}
