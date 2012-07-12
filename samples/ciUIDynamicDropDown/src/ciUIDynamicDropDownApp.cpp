#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "ciUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciUIDynamicDropDownApp : public AppBasic {
public:
    void prepareSettings( Settings *settings ); 	
    void setup();
	void update();
	void draw();
    void shutDown();
    
    void keyDown( KeyEvent event );
    void guiEvent(ciUIEvent *event);    
    
    float guiWidth; 
    float guiHeight; 
    float dim; 
    float length; 
    
    float bgValue; 
    ciUICanvas *gui;     
    ciUIDropDownList *ddl; 
    
};

void ciUIDynamicDropDownApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 768 , 1024 );
    settings->setFrameRate( 60.0f );
}

void ciUIDynamicDropDownApp::setup()
{
    guiWidth = getWindowWidth()*.5; 
    guiHeight= getWindowHeight(); 
    dim = guiWidth*.0625; 
    length = guiWidth-CI_UI_GLOBAL_WIDGET_SPACING*2;
    bgValue = 0.75; 
    
    gui = new ciUICanvas(0,0,guiWidth, guiHeight);
    gui->addWidgetDown(new ciUILabel("CIUI: DYNAMIC DROP DOWN", CI_UI_FONT_LARGE));
    gui->addWidgetDown(new ciUILabel("PRESS '1' TO ADD TO LIST", CI_UI_FONT_MEDIUM)); 
    gui->addWidgetDown(new ciUILabel("PRESS '2' TO DELETE FROM LIST", CI_UI_FONT_MEDIUM));     
    gui->addWidgetDown(new ciUILabel("PRESS '3' TO DELETE ALL IN LIST", CI_UI_FONT_MEDIUM));  
    gui->addWidgetDown(new ciUILabel("PRESS '4' TO OPEN/CLOSE THE LIST", CI_UI_FONT_MEDIUM));      
    vector<string> names; 
    names.push_back("ONE");    names.push_back("TWO");    names.push_back("THREE");    names.push_back("FOUR");    names.push_back("FIVE");
    ddl = new ciUIDropDownList(length, "DYNAMIC DROP DOWN", names, CI_UI_FONT_MEDIUM); 
    ddl->setAllowMultiple(true);
    gui->addWidgetDown(ddl);
    
    gui->registerUIEvents(this, &ciUIDynamicDropDownApp::guiEvent);
}

void ciUIDynamicDropDownApp::update()
{
    gui->update(); 
}

void ciUIDynamicDropDownApp::draw()
{
	gl::clear( Color( bgValue, bgValue, bgValue ) ); 
    gui->draw();
}

void ciUIDynamicDropDownApp::shutDown()
{
    delete gui; 
}

void ciUIDynamicDropDownApp::guiEvent(ciUIEvent *event)
{
    string name = event->widget->getName(); 
    
    cout << "WIDGET NAME: " << name << endl; 
    
    if(name == "DYNAMIC DROP DOWN")
    {
        ciUIDropDownList *ddlist = (ciUIDropDownList *) event->widget;
        vector<ciUIWidget *> &selected = ddlist->getSelected(); 
        for(int i = 0; i < selected.size(); i++)
        {
            cout << "SELECTED: " << selected[i]->getName() << endl; 
        }
    }
}

void ciUIDynamicDropDownApp::keyDown(KeyEvent event)
{
    switch (event.getChar())     
    {
        case 'p':
            gui->setDrawWidgetPadding(true);
            break;
        case 'P':
            gui->setDrawWidgetPadding(false);
            break;
            
        case '1':
            ddl->addToggle("NEW TOGGLE");
            break;
            
        case '2':
            ddl->removeToggle("NEW TOGGLE");
            break;
            
        case '3':
            ddl->clearToggles();
            break;

        case '4':
        {
            if(ddl->isOpen())
            {
                ddl->close();    
            }
            else 
            {
                ddl->open();
            }
        }   
            break;
 
        default:
            break;
    } 
}

CINDER_APP_BASIC( ciUIDynamicDropDownApp, RendererGl )
