#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciUISlidersApp : public AppBasic {
public:
    void prepareSettings( Settings *settings ); 	
    void setup();
	void update();
	void draw();
    void shutDown();
        
    void guiEvent(ciUIEvent *event);    
    
    float guiWidth; 
    float guiHeight; 
    float dim; 
    float length; 
    
    float red, green, blue; 
    ciUICanvas *gui;     
};

void ciUISlidersApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768 , 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUISlidersApp::setup()
{
    guiWidth = getWindowWidth()*.5; 
    guiHeight= getWindowHeight(); 
    dim = guiWidth*.0625; 
    length = guiWidth-CI_UI_GLOBAL_WIDGET_SPACING*2;
    red = 0.35; 
    green = 0.8; 
    blue = 0.95; 
    
    gui = new ciUICanvas(0,0,guiWidth, guiHeight);
	
    gui->addWidgetDown(new ciUILabel("SLIDER WIDGETS", CI_UI_FONT_LARGE));         
    gui->addWidgetDown(new ciUISpacer(length, 1)); 
    gui->addWidgetDown(new ciUIFPSSlider(length, dim, 0, 120, 60.0, "FPS SLIDER"));
	gui->addWidgetDown(new ciUILabel("NORMAL SLIDER", CI_UI_FONT_MEDIUM)); 	
    gui->addWidgetDown(new ciUISlider(length,dim, 0.0, 1.0, red, "RED")); 
	gui->addWidgetDown(new ciUILabel("MINIMAL SLIDER", CI_UI_FONT_MEDIUM)); 	
    gui->addWidgetDown(new ciUIMinimalSlider(length, dim, 0.0, 1.0, green, "GREEN", CI_UI_FONT_MEDIUM));
	gui->addWidgetDown(new ciUILabel("BILABEL SLIDER", CI_UI_FONT_MEDIUM)); 	    
    gui->addWidgetDown(new ciUIBiLabelSlider(length, 0.0, 1.0, blue, "BLUE", "LESS BLUE", "MORE BLUE", CI_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ciUILabel("RANGE SLIDER", CI_UI_FONT_MEDIUM)); 
	gui->addWidgetDown(new ciUIRangeSlider(length,dim, 0.0, 255.0, 50.0, 100.0, "RSLIDER")); 
    gui->addWidgetDown(new ciUISpacer(length, 1)); 
    gui->addWidgetDown(new ciUILabel("VERTICAL SLIDERS", CI_UI_FONT_MEDIUM));     
    gui->addWidgetDown(new ciUISlider(dim,80, 0.0, 255.0, 100, "1")); 	
	gui->addWidgetRight(new ciUISlider(dim,80, 0.0, 255.0, 150, "2")); 
	gui->addWidgetRight(new ciUISlider(dim,80, 0.0, 255.0, 200, "3")); 
	gui->addWidgetRight(new ciUIRangeSlider(dim, 80, 0, 255.0, 100.0, 200.0, "4")); 
    gui->addWidgetDown(new ciUISpacer(length, 1)); 
	gui->addWidgetDown(new ciUILabel("ROTARY SLIDER", CI_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ciUIRotarySlider(dim*4.0, 0.0, 100.0, 25.0, "CSLIDER 2")); 
	gui->addWidgetRight(new ciUICircleSlider(dim*4, 0.0, 1.0, green, "CIRCLE", CI_UI_FONT_MEDIUM));
	gui->addWidgetNorthOf(new ciUILabel("CIRCLE SLIDER", CI_UI_FONT_MEDIUM), "CIRCLE");         
    gui->addWidgetSouthOf(new ciUISpacer(length, 1), "CSLIDER 2");
    gui->addWidgetDown(new ciUILabel("2D PAD", CI_UI_FONT_MEDIUM)); 
	gui->addWidgetDown(new ciUI2DPad(length,80, Vec2f((length)*.5,80*.5), "PAD")); 	
    gui->addWidgetDown(new ciUISpacer(length, 1));     
	gui->addWidgetDown(new ciUILabel("IMAGE SLIDER", CI_UI_FONT_MEDIUM)); 
    gui->addWidgetDown(new ciUIImageSlider(304, dim, 0.0, 1.0, red, "slider.png", "RED"));     
    
    gui->registerUIEvents(this, &ciUISlidersApp::guiEvent);
}

void ciUISlidersApp::update()
{
    gui->update(); 
}

void ciUISlidersApp::draw()
{
	gl::clear( Color( red, green, blue ) ); 
    gui->draw();
}

void ciUISlidersApp::shutDown()
{
    delete gui; 
}

void ciUISlidersApp::guiEvent(ciUIEvent *event)
{
    string name = event->widget->getName(); 
    
    if(name == "RED")
    {
        ciUISlider *s = ( ciUISlider * ) event->widget; 
        red = s->getScaledValue(); 
    }
	if(name == "RED")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		red = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}
	else if(name == "GREEN")
	{
		ciUIMinimalSlider *slider = (ciUIMinimalSlider *) event->widget; 
		green = slider->getScaledValue(); 
        cout << "value: " << slider->getScaledValue() << endl; 
	}	
	else if(name == "BLUE")
	{
		ciUIBiLabelSlider *slider = (ciUIBiLabelSlider *) event->widget; 
		blue = slider->getScaledValue(); 		
        cout << "value: " << slider->getScaledValue() << endl;         
	}  
    else if(name == "RSLIDER")
    {
        ciUIRangeSlider *rslider = (ciUIRangeSlider *) event->widget; 
        cout << "valuelow: " << rslider->getScaledValueLow() << endl; 
        cout << "valuehigh: " << rslider->getScaledValueHigh() << endl;   
    }
    else if(name == "PAD")
    {
        ciUI2DPad *pad = (ciUI2DPad *) event->widget; 
        cout << "value x: " << pad->getScaledValue().x << endl; 
        cout << "value y: " << pad->getScaledValue().y << endl;         
    }
    else if(name == "CSLIDER" || name == "CSLIDER 2")
    {
        ciUIRotarySlider *rotslider = (ciUIRotarySlider *) event->widget; 
        cout << "value: " << rotslider->getScaledValue() << endl; 
    }    
}

CINDER_APP_BASIC( ciUISlidersApp, RendererGl )