#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "Resources.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciUIButtonsApp : public AppBasic {
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
    ciUICanvas *gui;     
};

void ciUIButtonsApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768 , 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUIButtonsApp::setup()
{
    guiWidth = getWindowWidth()*.5; 
    guiHeight= getWindowHeight(); 
    dim = guiWidth*.0625; 
    length = guiWidth-CI_UI_GLOBAL_WIDGET_SPACING*2;
    bgValue = 0.75; 
    
    gui = new ciUICanvas(0,0,guiWidth, guiHeight);
    gui->addWidgetDown(new ciUILabel("CIUI: BUTTONS & TOGGLES", CI_UI_FONT_LARGE));

    gui->addWidgetDown(new ciUISpacer(length, 2));
    gui->addWidgetDown(new ciUILabel("SIMPLE BUTTONS & TOGGLES", CI_UI_FONT_MEDIUM));    
    gui->addWidgetDown(new ciUIButton(dim*2,dim*2,false, "BUTTON", CI_UI_FONT_SMALL));
    gui->addWidgetRight(new ciUIToggle(dim*2,dim*2,false, "TOGGLE", CI_UI_FONT_SMALL));    
    
    gui->addWidgetDown(new ciUISpacer(length, 2), CI_UI_ALIGN_LEFT);    
    gui->addWidgetDown(new ciUILabel("SINGLE IMAGE BUTTONS & TOGGLES", CI_UI_FONT_MEDIUM));        
    gui->addWidgetDown(new ciUIImageButton(dim*2,dim*2, false, loadResource(PLAY_IMAGE), "PLAY"));
    gui->addWidgetRight(new ciUIImageToggle(dim*2,dim*2, false, loadResource(PLAY_IMAGE), "PLAY"));    
    
    gui->addWidgetDown(new ciUISpacer(length, 2), CI_UI_ALIGN_LEFT);     
    gui->addWidgetDown(new ciUILabel("MULTI-IMAGE BUTTONS & TOGGLES", CI_UI_FONT_MEDIUM));        
    gui->addWidgetDown(new ciUIMultiImageButton(dim*2, dim*2, false, "toggle.png", "BTN", CI_UI_FONT_SMALL));
    gui->addWidgetRight(new ciUIMultiImageToggle(dim*2, dim*2, true, "toggle.png", "TGL", CI_UI_FONT_SMALL));
    
    gui->addWidgetDown(new ciUISpacer(length, 2));    
    gui->addWidgetDown(new ciUILabelButton(length, false, "LABEL BUTTON LGE", CI_UI_FONT_LARGE));
    gui->addWidgetDown(new ciUILabelButton(length, false, "LABEL BUTTON MED", CI_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ciUILabelButton(length, false, "LABEL BUTTON SML", CI_UI_FONT_SMALL));
        
    gui->addWidgetDown(new ciUILabelToggle(length, false, "LABEL TOGGLE LGE", CI_UI_FONT_LARGE));        
    gui->addWidgetDown(new ciUILabelToggle(length, false, "LABEL TOGGLE MED", CI_UI_FONT_MEDIUM));        
    gui->addWidgetDown(new ciUILabelToggle(length, false, "LABEL TOGGLE SML", CI_UI_FONT_SMALL));            
    
    gui->addWidgetDown(new ciUILabelButton(false, "BUTTON AUTO SIZE", CI_UI_FONT_SMALL));
    gui->addWidgetRight(new ciUILabelToggle(false, "TOGGLE AUTO SIZE", CI_UI_FONT_SMALL));        
    
    gui->addWidgetDown(new ciUISpacer(length, 2), CI_UI_ALIGN_LEFT);
    vector<string> names;     names.push_back("RAD 1");    names.push_back("RAD 2");    
    gui->addWidgetDown(new ciUIRadio(dim,dim, "RADIO HORIZONTAL", names, CI_UI_ORIENTATION_HORIZONTAL));
    vector<string> names2;     names2.push_back("RADIO 1");    names2.push_back("RADIO 2");     
    gui->addWidgetDown(new ciUIRadio(dim,dim, "RADIO VERTICAL", names2, CI_UI_ORIENTATION_VERTICAL));    
    
    ((ciUIToggleMatrix *)gui->addWidgetDown(new ciUIToggleMatrix(dim, dim, 4, 5, "TOGGLE MATRIX")))->setAllowMultiple(true);    //true by default
    ((ciUIToggleMatrix *)gui->addWidgetRight(new ciUIToggleMatrix(dim, dim, 4, 5, "RADIO MATRIX")))->setAllowMultiple(false);    
    
    gui->registerUIEvents(this, &ciUIButtonsApp::guiEvent);
}

void ciUIButtonsApp::update()
{
    gui->update(); 
}

void ciUIButtonsApp::draw()
{
	gl::clear( Color( bgValue, bgValue, bgValue ) ); 
    gui->draw();
}

void ciUIButtonsApp::shutDown()
{
    delete gui; 
}

void ciUIButtonsApp::guiEvent(ciUIEvent *event)
{
    string name = event->widget->getName(); 
    if(name == "BACKGROUND")
    {
        ciUISlider *s = ( ciUISlider * ) event->widget; 
        bgValue = s->getScaledValue(); 
    }
    else if(name == "FULLSCREEN")
    {
        ciUIToggle *t = ( ciUIToggle * ) event->widget; 
        setFullScreen(t->getValue()); 
    }
}

CINDER_APP_BASIC( ciUIButtonsApp, RendererGl )
