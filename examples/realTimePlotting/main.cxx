
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
	client->setSeverity(RBOX::CLIENTSEVERITY::LOW);

	client->start();
}

int main(){
	//create plotter
	SurfacePlotter* test=new SurfacePlotter();
	//create surface	
	Surface<int16_t>* surface=test->addSurface<int16_t>(turns,bunches);
	//data to initialize it with 	
	int16_t* data=reinterpret_cast<int16_t*>(calloc(turns*bunches,sizeof(int16_t)));
	surface->addData(data);
	surface->setPixelsPerColumn(0.01);
	surface->setYScale(20);
	surface->setPixelsPerRow(0.01);
	free(data);
	//start RBOXClient thread
	std::thread temp= std::thread(std::bind(updateSurface,surface));
	//show the data	
	test->show();
}
