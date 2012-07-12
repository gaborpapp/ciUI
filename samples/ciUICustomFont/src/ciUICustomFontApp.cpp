#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciUICustomFontApp : public AppBasic {
public:
    void prepareSettings( Settings *settings ); 	
    void setup();
	void update();
	void draw();
    void shutDown();
    
    void keyDown( KeyEvent event); 
    void guiEvent(ciUIEvent *event);    
    
    float guiWidth; 
    float guiHeight; 
    float dim; 
    float length; 
    
    ColorA backgroundColor; 
    bool drawPadding; 
    ciUICanvas *gui;     
};

void ciUICustomFontApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768 , 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUICustomFontApp::setup()
{
    guiWidth = getWindowWidth()*.5; 
    guiHeight= getWindowHeight(); 
    dim = guiWidth*.0625; 
    length = guiWidth-CI_UI_GLOBAL_WIDGET_SPACING*2;
    backgroundColor.r = 0.75; 
    backgroundColor.g = 0.75; 
    backgroundColor.b = 0.75; 
    drawPadding = false; 
    
    gui = new ciUICanvas(0,0,guiWidth, guiHeight);

    gui->setFont("faucet.ttf");                     //This loads a new font and sets the GUI font
    gui->setFontSize(CI_UI_FONT_LARGE, 36);            //These call are optional, but if you want to resize the LARGE, MEDIUM, and SMALL fonts, here is how to do it. 
    gui->setFontSize(CI_UI_FONT_MEDIUM, 24);           
    gui->setFontSize(CI_UI_FONT_SMALL, 16);            //SUPER IMPORTANT NOTE: CALL THESE FUNTIONS BEFORE ADDING ANY WIDGETS, THIS AFFECTS THE SPACING OF THE GUI
    
    gui->addWidgetDown(new ciUILabel("CIUI: CUSTOM FONT", CI_UI_FONT_LARGE));    
    gui->addWidgetDown(new ciUISpacer(length, 2)); 
    gui->addWidgetDown(new ciUILabel("BACKGROUND CONTROL", CI_UI_FONT_MEDIUM));     
    gui->addWidgetDown(new ciUISlider(length, dim, 0.0, 1.0, backgroundColor.r, "BGR")); 
    gui->addWidgetDown(new ciUISlider(length, dim, 0.0, 1.0, backgroundColor.g, "BGG")); 
    gui->addWidgetDown(new ciUISlider(length, dim, 0.0, 1.0, backgroundColor.b, "BGB"));     
    
    gui->addWidgetDown(new ciUISpacer(length, 2)); 
    gui->addWidgetDown(new ciUILabelToggle(drawPadding, "DRAW PADDING", CI_UI_FONT_MEDIUM)); 
    gui->addWidgetDown(new ciUILabelToggle(true, "SPACER COLOR", CI_UI_FONT_MEDIUM));     
    
    gui->addWidgetDown(new ciUISpacer(length, 2)); 
    gui->addWidgetDown(new ciUILabel("HIDE & SHOW GUI", CI_UI_FONT_MEDIUM));     
    gui->addWidgetDown(new ciUILabel("BY PRESSING 'g'", CI_UI_FONT_MEDIUM));     
    gui->addWidgetDown(new ciUISpacer(length, 2)); 
    gui->autoSizeToFitWidgets();
    
    gui->registerUIEvents(this, &ciUICustomFontApp::guiEvent);
}

void ciUICustomFontApp::update()
{
    gui->update(); 
}

void ciUICustomFontApp::draw()
{
	gl::clear( Color( backgroundColor.r, backgroundColor.g, backgroundColor.b ) ); 
    gui->draw();
}

void ciUICustomFontApp::shutDown()
{
    delete gui; 
}

void ciUICustomFontApp::keyDown( KeyEvent event)
{
    if(event.getChar() == 'g')
    {
        gui->toggleVisible(); 
    }
}

void ciUICustomFontApp::guiEvent(ciUIEvent *event)
{
    string name = event->widget->getName(); 
	
    if(name == "BGR")
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
    else if(name == "DRAW PADDING")
    {
        ciUILabelToggle *toggle = (ciUILabelToggle *) event->widget; 
        drawPadding = toggle->getValue();     
        gui->setDrawWidgetPadding(drawPadding);             
    }    
    else if(name == "SPACER COLOR")
    {
        ciUILabelToggle *toggle = (ciUILabelToggle *) event->widget;         
        
        vector<ciUIWidget*> spacers = gui->getWidgetsOfType(CI_UI_WIDGET_SPACER); 
        
        for(int i = 0; i < spacers.size(); i++)        
        {
            ciUISpacer *spacer = (ciUISpacer *) spacers[i]; 
            spacer->setToggleColor(toggle->getValue()); 
        }
    }
}

CINDER_APP_BASIC( ciUICustomFontApp, RendererGl )
