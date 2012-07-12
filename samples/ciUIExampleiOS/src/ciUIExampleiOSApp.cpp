#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/CinderMath.h"
#include "cinder/Perlin.h"
#include "cinder/Timeline.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "Resources.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciUIExampleiOSApp : public AppCocoaTouch 
{    
public:
    void prepareSettings( Settings *settings ); 
    void setup();
    void update(); 
    void draw();
    void shutDown();
    
    void guiEvent(ciUIEvent *event);    
    
    void setupGUI1(); 
    void setupGUI2(); 
    
    //Vars 
    float bgColorR; 
    float bgColorG; 
    float bgColorB;     
    

    ciUICanvas *gui;  
    ciUICanvas *gui1;  
    float menuWidth; 
    float menuHeight; 
    float widgetWidth;
    float dim; 
};

void ciUIExampleiOSApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768, 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUIExampleiOSApp::setup()
{
    gl::enableAlphaBlending(); 
	setFpsSampleInterval(1.0); 
    menuWidth = getWindowWidth()/2.0; 
    menuHeight = getWindowHeight();
    widgetWidth = menuWidth - CI_UI_GLOBAL_WIDGET_SPACING*2;
    dim = menuWidth*.125;
    bgColorR = 0.90f;     
    bgColorG = 0.08f;     
    bgColorB = 0.33f;      
    
    setupGUI1(); 
    setupGUI2(); 
}

void ciUIExampleiOSApp::update()
{
    gui->update();  
    gui1->update();
}

void ciUIExampleiOSApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( bgColorR, bgColorG, bgColorB ) );    

    gui->draw();        
    gui1->draw();    
}

void ciUIExampleiOSApp::shutDown()
{
    delete gui; 
    delete gui1; 
}

void ciUIExampleiOSApp::guiEvent(ciUIEvent *event)
{
    string name = event->widget->getName(); 
    cout << name << endl; 
    if(name == "THEME SELECTOR")
    {
        ciUISlider *slider = (ciUISlider *) event->widget; 
        int theme = slider->getScaledValue(); 
        gui->setTheme(theme); 
        gui1->setTheme(theme); 
    }    
    else if(name == "RED")
    {
        ciUISlider *slider = (ciUISlider *) event->widget; 
        bgColorR = slider->getScaledValue(); 
    }
    else if(name == "GREEN")
    {
        ciUISlider *slider = (ciUISlider *) event->widget; 
        bgColorG = slider->getScaledValue(); 
    }
    else if(name == "BLUE")
    {
        ciUISlider *slider = (ciUISlider *) event->widget; 
        bgColorB = slider->getScaledValue(); 
    }    
    else if(name == "TEXT INPUT")
    {
        ciUITextInput *tf = (ciUITextInput *) event->widget; 
        switch(tf->getTriggerType())
        {
            case CI_UI_TEXTINPUT_ON_ENTER:
                cout << "ENTER TRIGGER: " << tf->getTextString() << endl; 
                break; 
            case CI_UI_TEXTINPUT_ON_FOCUS:
                cout << "ON FOCUS TRIGGER: " << tf->getTextString() << endl;               
                break; 
            case CI_UI_TEXTINPUT_ON_UNFOCUS:
                cout << "ON UNFOCUS TRIGGER: " << tf->getTextString() << endl;                                  
                break;                 
        }               
    }
}

void ciUIExampleiOSApp::setupGUI1()
{
    float w = widgetWidth; 
    float h = dim; 
    float spacerHeight = 2; 
    gui = new ciUICanvas(0,0,menuWidth,menuHeight);    
    gui->addWidgetDown(new ciUILabel("CINDER UI: CIUI", CI_UI_FONT_LARGE));
    gui->addWidgetDown(new ciUISpacer(w, spacerHeight));            
    gui->addWidgetDown(new ciUILabel("BY REZA ALI @rezaali", CI_UI_FONT_SMALL));
    gui->addWidgetDown(new ciUILabel("SLIDERS", CI_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ciUILabel("CHANGE THEMES:", CI_UI_FONT_MEDIUM));
    ((ciUISlider *)gui->addWidgetDown(new ciUISlider(w, h, 0, 44, 0.0, "THEME SELECTOR")))->setIncrement(1.0);    
    gui->addWidgetDown(new ciUISlider(w, h, 0, 1.0, bgColorR, "RED"));    
    gui->addWidgetDown(new ciUISlider(w, h, 0, 1.0, bgColorG, "GREEN"));
    gui->addWidgetDown(new ciUISlider(w, h, 0, 1.0, bgColorB, "BLUE"));
    gui->addWidgetDown(new ciUIRangeSlider(w, h, 0, 100, 25, 75, "RANGE SLIDER"));        
    gui->addWidgetDown(new ciUIMinimalSlider(w, h, 0, 1.00, 0.50, "MINIMAL SLIDER"));        
    gui->addWidgetDown(new ciUIBiLabelSlider(w, h, 0, 1, .5, "BILABEL", "BILABEL", "SLIDER"));
    gui->addWidgetDown(new ciUIFPSSlider(w, h, 0, 120.00, 0.50, "FPS SLIDER"));        
    gui->addWidgetDown(new ciUISpacer(w, spacerHeight));        
    gui->autoSizeToFitWidgets();
    gui->registerUIEvents(this, &ciUIExampleiOSApp::guiEvent); 
}

void ciUIExampleiOSApp::setupGUI2()
{
    float w = widgetWidth; 
    float h = dim;
    float spacerHeight = 2; 
    gui1 = new ciUICanvas(gui->getRect()->getX()+gui->getRect()->getWidth(),0,menuWidth,menuHeight);
    gui1->addWidgetDown(new ciUILabel("MORE WIDGETS", CI_UI_FONT_LARGE));
    gui1->addWidgetDown(new ciUISpacer(w, spacerHeight));        
    gui1->addWidgetDown(new ciUI2DPad(w, w*.5, Vec2f(w*.5,w*.25), "2DPAD"), CI_UI_ALIGN_LEFT);
    gui1->addWidgetDown(new ciUISpacer(w, spacerHeight));        
    gui1->addWidgetDown(new ciUILabel("NUMBER DIALER", CI_UI_FONT_MEDIUM));    
    gui1->addWidgetDown(new ciUINumberDialer(-10000.0, 10000.0, 10000.0, 4, "NUMBER DIALER", CI_UI_FONT_MEDIUM));    
    gui1->addWidgetDown(new ciUISpacer(w, spacerHeight), CI_UI_ALIGN_LEFT);        
    gui1->addWidgetDown(new ciUILabel("BUTTONS & TOGGLES", CI_UI_FONT_MEDIUM));
    gui1->addWidgetDown(new ciUIButton(h,h,false, "BUTTON", CI_UI_FONT_SMALL));
    gui1->addWidgetRight(new ciUIToggle(h,h,false, "TOGGLE", CI_UI_FONT_SMALL));    
    gui1->addWidgetDown(new ciUIMultiImageButton(h, h, false, "toggle.png", "MULTI-IMAGE BUTTON", CI_UI_FONT_SMALL));
    gui1->addWidgetDown(new ciUIMultiImageToggle(h, h, true, "toggle.png", "MULTI-IMAGE TOGGLE", CI_UI_FONT_SMALL));    
    gui1->addWidgetDown(new ciUILabelButton(w, false, "LABEL BUTTON", CI_UI_FONT_MEDIUM, h));
    gui1->addWidgetDown(new ciUILabelButton(false, "BUTTON AUTO SIZE", CI_UI_FONT_SMALL));    
    gui1->addWidgetDown(new ciUILabelToggle(w, false, "LABEL TOGGLE", CI_UI_FONT_MEDIUM, h));        
    gui1->addWidgetDown(new ciUILabelToggle(false, "TOGGLE AUTO SIZE", CI_UI_FONT_SMALL));        
    gui1->autoSizeToFitWidgets();
    gui1->registerUIEvents(this, &ciUIExampleiOSApp::guiEvent);     
}

CINDER_APP_COCOA_TOUCH( ciUIExampleiOSApp, RendererGl )
