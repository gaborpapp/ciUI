#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciUIScrollableApp : public AppBasic {
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
    
    ciUIScrollableCanvas *gui;     
};

void ciUIScrollableApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768 , 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUIScrollableApp::setup()
{    
    guiWidth = getWindowWidth()*.5; 
    guiHeight= getWindowHeight(); 
    dim = guiWidth*.0625; 
    length = guiWidth-CI_UI_GLOBAL_WIDGET_SPACING*2;
    
    gui = new ciUIScrollableCanvas(0,0,guiWidth, guiHeight);        
    gui->setScrollableDirections(false, true);
    gui->addWidgetDown(new ciUILabel("CIUI: SCROLLABLE CANVAS", CI_UI_FONT_LARGE));
    gui->addWidgetDown(new ciUISpacer(length, 2));      
    
    for(int i = 0; i < 30; i++)
    {
        ostringstream ss;
        ss << i;
        gui->addWidgetDown(new ciUISlider(length, dim, 0, 100, i*3.3, "SLIDER " + ss.str() ));
        gui->addWidgetDown(new ciUISpacer(length, 2));
    }
    
    vector<ciUIWidget*> spacers = gui->getWidgetsOfType(CI_UI_WIDGET_SPACER);
    
    for(int i = 0; i < spacers.size(); i++)
    {
        ciUISpacer *s = (ciUISpacer *) spacers[i];
        s->toggleVisible();
    }

    gui->autoSizeToFitWidgets();	             
    gui->registerUIEvents(this, &ciUIScrollableApp::guiEvent);
}

void ciUIScrollableApp::update()
{
    gui->update(); 
}

void ciUIScrollableApp::draw()
{
	gl::clear( ColorA( 0.75, 0.75, 0.75 ) );     
    gui->draw();
}

void ciUIScrollableApp::shutDown()
{
    delete gui; 
}

void ciUIScrollableApp::guiEvent(ciUIEvent *event)
{
	string name = event->widget->getName(); 
	cout << name << endl; 
}

void ciUIScrollableApp::keyDown( KeyEvent event )
{
    if(event.getChar() == 'h')
    {
        gui->toggleVisible(); 
    }
}

CINDER_APP_BASIC( ciUIScrollableApp, RendererGl )
