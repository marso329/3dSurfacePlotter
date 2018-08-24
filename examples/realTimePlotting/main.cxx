
#include "SurfacePlotter.h"
#include <thread>
#include <functional>
#include  <chrono>
#include "RBOXClient.h"
#include "RBOXFrameStructure.h"
using namespace SurfacePlotting;
std::size_t bunches=3564;
std::size_t turns =4096;



using namespace RBOX;

void addData(Surface<int16_t>* surface,RBOXFrameStructure* data){
surface->addRow(data->getData(),DOWN);

}

void updateSurface(Surface<int16_t>* surface){
	RBOX::RBOXClient* client=new RBOX::RBOXClient("Plotting_test_opengl");
	std::vector<std::string> subscriptions={"test0"};
	std::function<void(RBOX::RBOXFrameStructure*)> bindedFunction =std::bind( addData, surface, std::placeholders::_1 );
	client->addSubscription (subscriptions,bindedFunction);
	client->setSeverity(RBOX::CLIENTSEVERITY::MEDIUM);
	client->start();
	int16_t* data=reinterpret_cast<int16_t*>(malloc(bunches*sizeof(int16_t)));
	float angle=0;
	float toAdd=2*PI/bunches*512;
	while(true){
		float x;
		//std::cin>>x;
		for(std::size_t i=0;i<bunches;i++){
			*(data+i)=static_cast<int16_t>(16000*(cos(angle)+1));
			//*(data+i)=x;
			angle+=toAdd;
		}
		surface->addRow(data,DOWN);
		std::this_thread::sleep_for (std::chrono::milliseconds(16));
		angle+=0.33;
	}
}

int main(){
SurfacePlotter* test=new SurfacePlotter();
									//rows X columns
 Surface<int16_t>* surface=test->addSurface<int16_t>(turns,bunches);
 int16_t* data=reinterpret_cast<int16_t*>(malloc(turns*bunches*sizeof(int16_t)));
 for(std::size_t i=0;i<turns*bunches;i++){

	 *(data+i)=0;
	 }
surface->addData(data);
	surface->setPixelsPerColumn(0.01);
//surface->setPixelsPerRow(0.001);
free(data);

std::thread temp= std::thread(std::bind(updateSurface,surface));

//surface->addRow(data+4,shift=UP);
test->show();
}
