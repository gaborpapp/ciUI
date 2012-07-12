#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciUISaveLoadSettingsApp : public AppBasic {
public:
    void prepareSettings( Settings *settings ); 	
    void setup();
	void update();
	void draw();
    void shutDown();
    
    void keyDown( KeyEvent event ); 
    void guiEvent( ciUIEvent *event );    
    
    float guiWidth; 
    float guiHeight; 
    float dim; 
    float length; 
    
    float red, green, blue, alpha, radius, resolution; 
    bool drawFill; 
    ColorA backgroundColor; 
    Vec2f position; 
    ciUICanvas *gui;     
};

void ciUISaveLoadSettingsApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768 , 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUISaveLoadSettingsApp::setup()
{
    backgroundColor.r = 0.5; 
    backgroundColor.g = 0.5; 
    backgroundColor.b = 0.5;  
    
    alpha = .8; 
    radius = 150; 
    drawFill = true;     
    backgroundColor = ColorA(233/255.0, 52.0/255.0, 27.0/255.0); 
    resolution = 30; 
    position = Vec2f(getWindowWidth()*.5, getWindowHeight()*.5); 
    
    guiWidth = getWindowWidth()*.5; 
    guiHeight= getWindowHeight(); 
    dim = guiWidth*.0625; 
    length = guiWidth-CI_UI_GLOBAL_WIDGET_SPACING*2;
    
    gui = new ciUICanvas(0,0,guiWidth, guiHeight);
	gui->addWidgetDown(new ciUILabel("CIUI: SAVE & LOAD SETTINGS", CI_UI_FONT_LARGE)); 
    gui->addWidgetDown(new ciUILabel("PRESS 's' TO SAVE SETTINGS", CI_UI_FONT_MEDIUM));     
    gui->addWidgetDown(new ciUILabel("PRESS 'l' TO LOAD SETTINGS", CI_UI_FONT_MEDIUM));         
    gui->addWidgetDown(new ciUISpacer(length, 2));    
    gui->addWidgetDown(new ciUILabel("BACKGROUND CONTROL", CI_UI_FONT_MEDIUM));     
    gui->addWidgetDown(new ciUISlider(length/3.0 - CI_UI_GLOBAL_WIDGET_SPACING, dim, 0, 1.0, backgroundColor.r, "BGR")); 
    gui->addWidgetRight(new ciUISlider(length/3.0 - CI_UI_GLOBAL_WIDGET_SPACING, dim, 0, 1.0, backgroundColor.g, "BGG")); 
    gui->addWidgetRight(new ciUISlider(length/3.0 - CI_UI_GLOBAL_WIDGET_SPACING, dim, 0, 1.0, backgroundColor.b, "BGB"));     
    gui->addWidgetDown(new ciUILabel("CIRCLE CONTROL", CI_UI_FONT_MEDIUM));     
    gui->addWidgetDown(new ciUISlider(length/3.0 - CI_UI_GLOBAL_WIDGET_SPACING, dim, 0.0, 1.0, red, "RED")); 
	gui->addWidgetRight(new ciUISlider(length/3.0 - CI_UI_GLOBAL_WIDGET_SPACING, dim, 0.0, 1.0, green, "GREEN")); 
    gui->addWidgetRight(new ciUISlider(length/3.0 - CI_UI_GLOBAL_WIDGET_SPACING, dim, 0.0, 1.0, blue, "BLUE")); 	    
    gui->addWidgetSouthOf(new ciUISlider(length,dim, 0.0, 1.0, alpha, "ALPHA"), "RED"); 	        
    gui->addWidgetDown(new ciUISlider(length,dim, 0.0, 600.0, radius, "RADIUS")); 
	gui->addWidgetDown(new ciUISlider(length,dim, 3, 60, resolution, "RESOLUTION")); 
    gui->addWidgetDown(new ciUILabelToggle(length, drawFill, "DRAW FILL", CI_UI_FONT_MEDIUM)); 
    float padWidth = length; 
    float padHeight = length*((float)getWindowHeight()/(float)getWindowWidth()); 
    
    gui->addWidgetDown(new ciUI2DPad(padWidth, padHeight, Vec2f(padWidth*.5, padHeight*.5), "POSITION")); 
    
    
    gui->registerUIEvents(this, &ciUISaveLoadSettingsApp::guiEvent);
}

void ciUISaveLoadSettingsApp::update()
{
    gui->update(); 
}

void ciUISaveLoadSettingsApp::draw()
{
	gl::clear( backgroundColor ); 
    
    gl::color(ColorA(red, green, blue, alpha)); 
    if(drawFill)
    {
        gl::drawSolidCircle(position, radius, resolution);
    }
    else
    {
        gl::drawStrokedCircle(position, radius, resolution);
    }
    
    gui->draw();
}

void ciUISaveLoadSettingsApp::shutDown()
{
    delete gui; 
}

void ciUISaveLoadSettingsApp::guiEvent(ciUIEvent *event)
{
	string name = event->widget->getName(); 
	
	if(name == "RED")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		red = slider->getScaledValue(); 
	}
	else if(name == "GREEN")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		green = slider->getScaledValue(); 
	}	
	else if(name == "BLUE")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		blue = slider->getScaledValue(); 		
	}
	else if(name == "BGR")
	{
		ciUISlider *rslider = (ciUISlider *) event->widget; 
		backgroundColor.r = rslider->getScaledValue(); 		
	}    
	else if(name == "BGG")
	{
		ciUISlider *rslider = (ciUISlider *) event->widget; 
		backgroundColor.g = rslider->getScaledValue(); 		
	}    
	else if(name == "BGB")
	{
		ciUISlider *rslider = (ciUISlider *) event->widget; 
		backgroundColor.b = rslider->getScaledValue(); 		
	}    
	else if(name == "ALPHA")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		alpha = slider->getScaledValue(); 		
	}
	else if(name == "RADIUS")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		radius = slider->getScaledValue(); 		
	}
	else if(name == "RESOLUTION")
	{
		ciUISlider *slider = (ciUISlider *) event->widget; 
		resolution = slider->getScaledValue();          //gets the value from the specified range, otherwise you can get a normalized value (0.0 -> 1.0); 
	}    
    else if(name == "POSITION")
	{
		ciUI2DPad *pad = (ciUI2DPad *) event->widget; 
		position.x = pad->getPercentValue().x*getWindowWidth(); 
		position.y = pad->getPercentValue().y*getWindowHeight();
	}    
    else if(name == "DRAW FILL")
    {
        ciUILabelToggle *toggle = (ciUILabelToggle *) event->widget; 
        drawFill = toggle->getValue();     
    }    
}

void ciUISaveLoadSettingsApp::keyDown( KeyEvent event )
{
    if(event.getChar() == 's')
    {
        gui->saveSettings("guiSettings.xml"); 
    }
    else if(event.getChar() == 'l')
    {
        gui->loadSettings("guiSettings.xml"); 
    }

}

CINDER_APP_BASIC( ciUISaveLoadSettingsApp, RendererGl )
