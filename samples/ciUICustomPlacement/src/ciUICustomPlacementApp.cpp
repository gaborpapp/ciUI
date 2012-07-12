#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciUICustomPlacementApp : public AppBasic {
public:
    void prepareSettings( Settings *settings ); 	
    void setup();
	void update();
	void draw();
    void shutDown();
    
    void guiEvent(ciUIEvent *event);    
    
    void setupGUI1(); 
    void setupGUI2(); 
    
    float guiWidth; 
    float guiHeight; 
    float dim; 
    float length; 
    
    ColorA background; 
    ciUICanvas *gui1;     
    ciUICanvas *gui2;         
};

void ciUICustomPlacementApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768 , 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUICustomPlacementApp::setup()
{
    guiWidth = getWindowWidth()*.5; 
    guiHeight= getWindowHeight(); 
    dim = guiWidth*.0625; 
    length = guiWidth-CI_UI_GLOBAL_WIDGET_SPACING*2;
    background.r = 1.00; 
    background.g = 0.16; 
    background.b = 0.23;     
    
    setupGUI1(); 
    setupGUI2();
    
}

void ciUICustomPlacementApp::setupGUI1()
{
    gui1 = new ciUICanvas();
    gui1->setDrawBack(false);     
    gui1->addWidgetDown(new ciUILabel("CUSTOM WIDGET LAYOUT", CI_UI_FONT_LARGE), CI_UI_ALIGN_RIGHT);
    gui1->addWidget(new ciUILabel(360, 300, "2D PAD", CI_UI_FONT_MEDIUM)); 				
    gui1->addWidgetSouthOf(new ciUI2DPad(320,190, Vec2f(320*.5,190*.5), "PAD"), "2D PAD"); 	
    gui1->addWidgetWestOf(new ciUITextInput(160, "TEXT INPUT", "Input Text", CI_UI_FONT_LARGE), "PAD"); 
    gui1->addWidgetEastOf(new ciUISlider(dim,190, 0.0, 255.0, 150, "0"), "PAD"); 
    gui1->addWidgetNorthOf(new ciUIToggle(dim, dim, false, "D_GRID"), "TEXT INPUT");      
    gui1->addWidgetWestOf(new ciUIToggle(dim, dim, true, "D2"), "D_GRID"); 
    gui1->addWidgetNorthOf(new ciUISlider(320,dim, 0.0, 1.0, background.g, "GREEN"), "D2"); 
    gui1->addWidgetNorthOf(new ciUISlider(320,dim, 0.0, 1.0, background.b, "BLUE"), "GREEN"); 
    gui1->addWidgetLeft(new ciUIToggle(dim, dim, true, "NEW")); 
    gui1->addWidgetSouthOf(new ciUISlider(320,dim, 0.0, 1.0, background.r, "RED"), "PAD");     
    gui1->registerUIEvents(this, &ciUICustomPlacementApp::guiEvent);    
}

void ciUICustomPlacementApp::setupGUI2()
{
    //this gui will show how to use functions: autoSizeToFitWidgets() & centerWidgetsHorizontallyOnCanvas() 
    gui2 = new ciUICanvas();
    gui2->addWidgetRight(new ciUIImageToggle(64,64,false,loadResource("DVD.png"),"DVD"));
    gui2->addWidgetRight(new ciUISpacer(2,64));
    gui2->addWidgetRight(new ciUIImageToggle(64,64,false,loadResource("House.png"),"House"));
    gui2->addWidgetRight(new ciUISpacer(2,64));
    gui2->addWidgetRight(new ciUIImageToggle(64,64,false,loadResource("Images.png"),"Images"));
    gui2->addWidgetRight(new ciUISpacer(2,64));
    gui2->addWidgetRight(new ciUIImageToggle(64,64,false,loadResource("Music.png"),"Music"));
    gui2->addWidgetRight(new ciUISpacer(2,64));
    gui2->addWidgetRight(new ciUIImageToggle(64,64,false,loadResource("Play.png"),"Play"));
    gui2->addWidgetRight(new ciUISpacer(2,64));
    gui2->addWidgetRight(new ciUIImageToggle(64,64,false,loadResource("Recycle.png"),"Recycle"));
    gui2->addWidgetRight(new ciUISpacer(2,64));
    gui2->addWidgetRight(new ciUIImageToggle(64,64,false,loadResource("System Preferences.png"),"System Preferences"));
    gui2->addWidgetRight(new ciUISpacer(2,64));
    gui2->addWidgetRight(new ciUIImageToggle(64,64,false,loadResource("WiFi.png"),"WiFi"));    
    gui2->autoSizeToFitWidgets();               //this function will fit the canvas's rect to fit all the widgets perfectly with padding
    gui2->setDrawBack(true);
    gui2->setColorBack(ColorA(1,1,1,1));
    float h = gui2->getRect()->getHeight();     
    gui2->getRect()->setY(getWindowHeight()-h); //reposition to the bottom of the screen since we know the exact height of the canvas
    gui2->getRect()->setWidth(getWindowWidth());//set the canvas' width to the size of the window
    gui2->centerWidgetsHorizontallyOnCanvas();  //this function centers the widgets horizontally on the canvas' rect
                                                //check out centerWidgetsVerticallyOnCanvas() for the vertical equivalent
                                                //there is also centerWidgets() which essentially does both in one shot
    
    vector<ciUIWidget *> spacers = gui2->getWidgetsOfType(CI_UI_WIDGET_SPACER);    
    for(int i = 0; i < spacers.size(); i++)     //the function above can be used to get all the widgets of a certain type from a canvas
    {
        ciUISpacer *s = (ciUISpacer *) spacers[i]; 
        s->toggleColor();
    }
}

void ciUICustomPlacementApp::update()
{
    gui1->update(); 
    gui2->update();     
}

void ciUICustomPlacementApp::draw()
{
	gl::clear( Color( background.r, background.g, background.b ) ); 
    gui1->draw();
    gui2->draw();    
}

void ciUICustomPlacementApp::shutDown()
{
    delete gui1; 
    delete gui2;     
}

void ciUICustomPlacementApp::guiEvent(ciUIEvent *event)
{
    string name = event->widget->getName(); 
	if(name == "RED")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		background.r = slider->getScaledValue(); 
	}
	else if(name == "GREEN")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		background.g = slider->getScaledValue(); 
	}	
	else if(name == "BLUE")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		background.b = slider->getScaledValue(); 		
	}
}

CINDER_APP_BASIC( ciUICustomPlacementApp, RendererGl )