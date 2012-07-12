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

class ciUIAllWidgetsApp : public AppBasic 
{    
public:
    void prepareSettings( Settings *settings ); 
    Renderer* prepareRenderer(); 
    void setup();
    void update(); 
    void draw();
    void shutDown();
    
	void keyDown( KeyEvent event );
	void keyUp( KeyEvent event );

    void guiEvent(ciUIEvent *event);    
    
    void setupGUI1(); 
    void setupGUI2(); 
    void setupGUI3(); 
    
    //Vars 
    float bgColorR; 
    float bgColorG; 
    float bgColorB;     
    
    Perlin myPerlin;
    gl::Texture mImage;
    Surface mImageSurface; 
    float *buffer; 
    ciUIMovingGraph *mvg; 
    ciUICanvas *gui;  
    ciUICanvas *gui1;  
    ciUICanvas *gui2;      
    float menuWidth; 
    float menuHeight; 
    float widgetWidth;
    float dim; 
};

Renderer* ciUIAllWidgetsApp::prepareRenderer()
{
    return new RendererGl( RendererGl::AA_MSAA_2 ); 
}

void ciUIAllWidgetsApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768, 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUIAllWidgetsApp::setup()
{
    gl::enableAlphaBlending(); 
	setFpsSampleInterval(1.0); 

    mImage = loadImage( loadResource( SAMPLE_IMAGE ) ); 
    mImageSurface = loadImage (loadResource( SAMPLE_IMAGE ) );
    buffer = new float[128];
    menuWidth = getWindowWidth()/3.0; 
    menuHeight = getWindowHeight();
    widgetWidth = menuWidth - CI_UI_GLOBAL_WIDGET_SPACING*2;
    dim = menuWidth*.125;
    bgColorR = 0.90f;     
    bgColorG = 0.08f;     
    bgColorB = 0.33f;      
    
    setupGUI1(); 
    setupGUI2(); 
    setupGUI3();     
}

void ciUIAllWidgetsApp::update()
{
    for(int i = 0; i < 128; i++)
    {
        buffer[i] = myPerlin.noise((float)i/128.0, timeline().getCurrentTime());
    }
    gui->update();    
    mvg->addPoint(getAverageFps());
}

void ciUIAllWidgetsApp::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( bgColorR, bgColorG, bgColorB ) );    
    gui->draw();
    gui1->draw();
    gui2->draw();    
}

void ciUIAllWidgetsApp::shutDown()
{
    delete[] buffer; 
    delete gui; 
    delete gui1; 
    delete gui2;     
}

void ciUIAllWidgetsApp::keyDown( KeyEvent event )
{
    if(gui2->hasKeyboardFocus())        
    {
        return; 
    }
    else
    {    
        if( event.getChar() == 'f' )
        {        
            setFullScreen( ! isFullScreen() );
        }
        else if(event.getChar() == 'p')
        {
            gui->setDrawWidgetPadding(true);
            gui1->setDrawWidgetPadding(true);
            gui2->setDrawWidgetPadding(true);        
        }
        else if(event.getChar() == 'P')
        {
            gui->setDrawWidgetPadding(false);
            gui1->setDrawWidgetPadding(false);
            gui2->setDrawWidgetPadding(false);        
        }
        else if(event.getChar() == '1')
        {
            gui->toggleVisible();
        }
        else if(event.getChar() == '2')
        {
            gui1->toggleVisible();        
        }
        else if(event.getChar() == '3')
        {
            gui2->toggleVisible();        
        }
        else if(event.getChar() == 's')
        {
            gui->saveSettings("ciUIGUISettings.xml");
            gui1->saveSettings("ciUIGUI1Settings.xml");
            gui2->saveSettings("ciUIGUI2Settings.xml");        
        }
        else if(event.getChar() == 'l')
        {
            gui->loadSettings("ciUIGUISettings.xml");        
            gui1->loadSettings("ciUIGUI1Settings.xml");        
            gui2->loadSettings("ciUIGUI2Settings.xml");                
        }
    }
}

void ciUIAllWidgetsApp::keyUp( KeyEvent event )
{
}

void ciUIAllWidgetsApp::guiEvent(ciUIEvent *event)
{
    string name = event->widget->getName(); 
    cout << name << endl; 
    if(name == "RED")
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

void ciUIAllWidgetsApp::setupGUI1()
{
    float w = widgetWidth; 
    float h = dim; 
    float spacerHeight = 2; 
    gui = new ciUICanvas(0,0,menuWidth,menuHeight);
    
    gui->addWidgetDown(new ciUILabel("CINDER UI: CIUI", CI_UI_FONT_LARGE));
    gui->addWidgetDown(new ciUILabel("BY REZA ALI @rezaali", CI_UI_FONT_SMALL));
    gui->addWidgetDown(new ciUILabel("SLIDERS", CI_UI_FONT_MEDIUM));
    
    gui->addWidgetDown(new ciUISlider(w, h, 0, 1.0, bgColorR, "RED"));    
    gui->addWidgetDown(new ciUISlider(w, h, 0, 1.0, bgColorG, "GREEN"));
    gui->addWidgetDown(new ciUISlider(w, h, 0, 1.0, bgColorB, "BLUE"));
    gui->addWidgetDown(new ciUIImageSlider(w, 24, 0.0, 1.0, .5, "slider.png", "IMAGE SLIDER"));
    gui->addWidgetDown(new ciUIRangeSlider(w, h, 0, 100, 25, 75, "RANGE SLIDER"));        
    gui->addWidgetDown(new ciUIMinimalSlider(w, h, 0, 1.00, 0.50, "MINIMAL SLIDER"));        
    gui->addWidgetDown(new ciUIBiLabelSlider(w, h, 0, 1, .5, "BILABEL", "BILABEL", "SLIDER"));
    gui->addWidgetDown(new ciUIFPSSlider(w, h, 0, 120.00, 0.50, "FPS SLIDER"));        
    gui->addWidgetDown(new ciUISpacer(w, spacerHeight));        
    ((ciUICircleSlider *)     gui->addWidgetDown(new ciUICircleSlider(w/2 - CI_UI_GLOBAL_WIDGET_SPACING,0.0, 1.0, 0.5, "NS")))->setInputDirection(CI_UI_DIRECTION_NORTHSOUTH);
    ((ciUICircleSlider *)     gui->addWidgetRight(new ciUICircleSlider(w/2 - CI_UI_GLOBAL_WIDGET_SPACING,0.0, 1.0, 0.5, "SN")))->setInputDirection(CI_UI_DIRECTION_SOUTHNORTH);
    ((ciUICircleSlider *)     gui->addWidgetSouthOf(new ciUICircleSlider(w/2 - CI_UI_GLOBAL_WIDGET_SPACING,0.0, 1.0, 0.5, "EW"), "NS"))->setInputDirection(CI_UI_DIRECTION_EASTWEST);
    ((ciUICircleSlider *)     gui->addWidgetRight(new ciUICircleSlider(w/2 - CI_UI_GLOBAL_WIDGET_SPACING,0.0, 1.0, 0.5, "WE")))->setInputDirection(CI_UI_DIRECTION_WESTEAST);
    gui->registerUIEvents(this, &ciUIAllWidgetsApp::guiEvent); 
}

void ciUIAllWidgetsApp::setupGUI2()
{
    float w = widgetWidth; 
    float h = dim;
    float spacerHeight = 2; 
    gui1 = new ciUICanvas(gui->getRect()->getX()+gui->getRect()->getWidth(),0,menuWidth,menuHeight);
    gui1->addWidgetDown(new ciUILabel("MORE WIDGETS", CI_UI_FONT_LARGE));
    gui1->addWidgetDown(new ciUISpacer(w, spacerHeight));        
    gui1->addWidgetDown(new ciUI2DPad(w, w*.5, Vec2f(w*.5,w*.25), "2DPAD"), CI_UI_ALIGN_LEFT);
    gui1->addWidgetDown(new ciUISpacer(w, spacerHeight));        
    gui1->addWidgetDown(new ciUIRotarySlider(120, 0, 100.0, 50.0, "ROTARY"));
    gui1->addWidgetRight(new ciUISlider(h,120,0.0,100.0,25.0,"1"));
    gui1->addWidgetRight(new ciUIRangeSlider(h,120,0.0,100.0,50.0,75.0,"5"));    
    
    gui1->addWidgetDown(new ciUISpacer(w, spacerHeight), CI_UI_ALIGN_LEFT);        
    gui1->addWidgetDown(new ciUILabel("NUMBER DIALER", CI_UI_FONT_MEDIUM));    
    gui1->addWidgetDown(new ciUINumberDialer(-10000.0, 10000.0, 10000.0, 4, "NUMBER DIALER", CI_UI_FONT_MEDIUM));    
    gui1->addWidgetDown(new ciUISpacer(w, spacerHeight), CI_UI_ALIGN_LEFT);        
    gui1->addWidgetDown(new ciUILabel("BUTTONS & TOGGLES", CI_UI_FONT_MEDIUM));
    gui1->addWidgetDown(new ciUIButton(h,h,false, "BUTTON", CI_UI_FONT_SMALL));
    gui1->addWidgetRight(new ciUIToggle(h,h,false, "TOGGLE", CI_UI_FONT_SMALL));    
    gui1->addWidgetDown(new ciUIMultiImageButton(h, h, false, "toggle.png", "MULTI-IMAGE BUTTON", CI_UI_FONT_SMALL));
    gui1->addWidgetDown(new ciUIMultiImageToggle(h, h, true, "toggle.png", "MULTI-IMAGE TOGGLE", CI_UI_FONT_SMALL));
    
    gui1->addWidgetDown(new ciUILabelButton(w, false, "LABEL BUTTON", CI_UI_FONT_MEDIUM));
    gui1->addWidgetDown(new ciUILabelButton(false, "BUTTON AUTO SIZE", CI_UI_FONT_SMALL));
    
    gui1->addWidgetDown(new ciUILabelToggle(w, false, "LABEL TOGGLE", CI_UI_FONT_MEDIUM));        
    gui1->addWidgetDown(new ciUILabelToggle(false, "TOGGLE AUTO SIZE", CI_UI_FONT_SMALL));        
    
    //gui1->setDrawBack(false);
    gui1->registerUIEvents(this, &ciUIAllWidgetsApp::guiEvent);     
}

void ciUIAllWidgetsApp::setupGUI3()
{
    vector<float> mbuffer; 
    for(int i = 0; i < 128; i++)
    {
        buffer[i] = myPerlin.noise((float)i/128.0);
        mbuffer.push_back(0);        
    }
    
    float w = widgetWidth; 
    float h = dim;
    float spacerHeight = 2;     
    gui2 = new ciUICanvas(gui1->getRect()->getX()+gui1->getRect()->getWidth(),0,menuWidth,menuHeight);
    gui2->addWidgetDown(new ciUILabel("zZOMG EVEN MORE!!", CI_UI_FONT_LARGE));    
    
    gui2->addWidgetDown(new ciUISpacer(w, spacerHeight));
    vector<string> names;     names.push_back("RAD 1");    names.push_back("RAD 2");    
    gui2->addWidgetDown(new ciUIRadio(h,h, "RADIO HORIZONTAL", names, CI_UI_ORIENTATION_HORIZONTAL));    
    vector<string> names2;     names2.push_back("RADIO 1");    names2.push_back("RADIO 2");     names2.push_back("RADIO 3");        
    gui2->addWidgetDown(new ciUIRadio(h,h, "RADIO VERTICAL", names2, CI_UI_ORIENTATION_VERTICAL));    
    
    
    gui2->addWidgetDown(new ciUISpacer(w, spacerHeight));   
    gui2->addWidgetDown(new ciUITextInput(w, "TEXT INPUT", "TEXT INPUT FIELD", CI_UI_FONT_MEDIUM));
    gui2->addWidgetDown(new ciUILabel("WAVEFORM PLOT", CI_UI_FONT_SMALL));        
    gui2->addWidgetDown(new ciUIWaveform(w, 30, buffer, 128, -1.0, 1.0, "WAVEFORM"));  
    gui2->addWidgetDown(new ciUILabel("SPECTRUM PLOT", CI_UI_FONT_SMALL));        
    gui2->addWidgetDown(new ciUISpectrum(w, 30, buffer, 128, -1.0, 1.0, "SPECTRUM"));  
    gui2->addWidgetDown(new ciUISpacer(w, spacerHeight));   
    gui2->addWidgetDown(new ciUILabel("MOVING PLOT (USING FPS)", CI_UI_FONT_SMALL));    
    mvg = (ciUIMovingGraph *) gui2->addWidgetDown(new ciUIMovingGraph(w, 30, mbuffer, 128, 0, 120, "MOVING GRAPH"));    
    
    gui2->addWidgetDown(new ciUIToggleMatrix(h, h, 3,3, "TOGGLE MTX"));    
    ((ciUIToggleMatrix *) gui2->addWidgetRight(new ciUIToggleMatrix(h, h, 3,3, "RADIO MTX")))->setAllowMultiple(false);
    
    gui2->addWidgetSouthOf(new ciUIImage(w/2 - CI_UI_GLOBAL_WIDGET_SPACING,w/2 - CI_UI_GLOBAL_WIDGET_SPACING,  &mImage, "IMAGE VIEW"), "TOGGLE MTX");
    gui2->addWidgetRight(new ciUIImageSampler(w/2 - CI_UI_GLOBAL_WIDGET_SPACING,w/2 - CI_UI_GLOBAL_WIDGET_SPACING, &mImageSurface, "IMAGE SAMPLER"));
    vector<string> items; items.push_back("ITEM 1"); items.push_back("ITEM 2");  items.push_back("ITEM 3");  items.push_back("ITEM 4");  
    gui2->addWidgetSouthOf(new ciUIDropDownList(w, "DROP DOWN LIST", names, CI_UI_FONT_MEDIUM), "IMAGE VIEW");    
    //gui2->setDrawBack(false);    
    gui2->registerUIEvents(this, &ciUIAllWidgetsApp::guiEvent);     
}


CINDER_APP_BASIC( ciUIAllWidgetsApp, RendererGl )