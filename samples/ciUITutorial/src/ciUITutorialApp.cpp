#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciUITutorialApp : public AppBasic {
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
    
    float bgValue;
    bool bgToggle; 
    ciUICanvas *gui;     
};

void ciUITutorialApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768 , 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUITutorialApp::setup()
{
    guiWidth = getWindowWidth()*.5; 
    guiHeight= getWindowHeight(); 
    dim = guiWidth*.0625; 
    length = guiWidth-CI_UI_GLOBAL_WIDGET_SPACING*2;
    bgValue = 0.75; 
    bgToggle = false; 
    
    gui = new ciUICanvas( 0, 0, guiWidth, guiHeight );
    gui->addWidgetDown(new ciUILabel("CIUI TUTORIAL", CI_UI_FONT_LARGE));    
    gui->addWidgetDown(new ciUISlider(length, dim, 0.0, 1.0, bgValue, "BACKGROUND"));
    gui->addWidgetDown(new ciUIToggle(dim, dim, false, "FULLSCREEN"));    
    gui->autoSizeToFitWidgets();           
    gui->registerUIEvents(this, &ciUITutorialApp::guiEvent);
    gui->setTheme(CI_UI_THEME_MINBLACK);
}

void ciUITutorialApp::update()
{
    gui->update(); 
}

void ciUITutorialApp::draw()
{
    gl::clear( Color( bgValue, bgValue, bgValue ) ); 
    gui->draw();    
}

void ciUITutorialApp::shutDown()
{
    cout << "deleting gui" << endl; 
    delete gui; 
}

void ciUITutorialApp::guiEvent(ciUIEvent *event)
{
    string name = event->widget->getName(); 
    cout << "GOT EVENT FROM: " << name << endl; 
    if(name == "FULLSCREEN")
    {
        ciUIToggle *t = (ciUIToggle * ) event->widget; 
        setFullScreen(t->getValue()); 
    }
    else if(name == "BACKGROUND")
    {
        ciUISlider *s = (ciUISlider *) event->widget; 
        bgValue = s->getScaledValue();
    }
}

CINDER_APP_BASIC( ciUITutorialApp, RendererGl )