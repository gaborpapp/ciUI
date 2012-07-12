/********************************************************************************** 
 
 Copyright (C) 2012 Syed Reza Ali (www.syedrezaali.com)
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 **********************************************************************************/

#ifndef CIUI_CANVAS
#define CIUI_CANVAS

#include "cinder/app/App.h"

#if defined( CINDER_COCOA_TOUCH )
#include "cinder/app/AppCocoaTouch.h"
#endif
#include "cinder/app/AppBasic.h"

#include "cinder/Xml.h"
#include "cinder/Utilities.h"
#include "cinder/Font.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/gl/gl.h"
#include "ciUIWidget.h"

#include <vector>
#include <map>

class ciUICanvas : public ciUIWidget
{    
public:	
    ~ciUICanvas() 
    {
		delete GUIevent; 
		for(int i = 0; i < widgets.size(); i++)
		{
			ciUIWidget *w = widgets[i]; 
			delete w; 
		}
		widgets.clear();             
    }
    
    ciUICanvas(float x, float y, float w, float h) 
    {
        rect = new ciUIRectangle(x,y,w,h);        
        init(w, h);
    }

    ciUICanvas(float x, float y, float w, float h, ciUICanvas *sharedResources) 
    {
        rect = new ciUIRectangle(x,y,w,h);        
        init(w, h, sharedResources);
    }
    
    ciUICanvas() 
    {
        float w = ci::app::getWindowWidth();
        float h = ci::app::getWindowHeight();
        rect = new ciUIRectangle(0,0,w,h); 
        init(w, h);
        setDrawBack(false); 
    }

    ciUICanvas(ciUICanvas *sharedResources) 
    {
        float w = ci::app::getWindowWidth();
        float h = ci::app::getWindowHeight();
        rect = new ciUIRectangle(0,0,w,h); 
        init(w, h, sharedResources);
        setDrawBack(false); 
    }
    
    void init(int w, int h, ciUICanvas *sharedResources = NULL)
    {
        name = "CI_UI_WIDGET_CANVAS"; 
		kind = CI_UI_WIDGET_CANVAS; 

#if defined( CINDER_COCOA_TOUCH )
        mApp = (app::AppCocoaTouch *) app::App::get();                 
#else
        mApp = app::App::get();                 
#endif 

		enabled = false; 		
		enable(); 
		
		enable_highlight_outline = false; 
		enable_highlight_fill = false; 
        
		GUIevent = new ciUIEvent(this); 
        
		paddedRect = new ciUIRectangle(-padding, -padding, w+padding*2.0, h+padding*2.0);
		paddedRect->setParent(rect);

        if(sharedResources != NULL)
        {
            hasSharedResources = true; 
            
            font_large = sharedResources->getFontLarge();
            font_medium = sharedResources->getFontMedium();
            font_small = sharedResources->getFontSmall();
            
            fontName = sharedResources->getFontName();
        }
        else
        {
            hasSharedResources = false;             
            fontName = CI_UI_FONT_NAME;
            setFont(fontName);         
        }
                
		font = font_medium; 
		lastAdded = NULL; 
        uniqueIDs = 0;         
        widgetSpacing = CI_UI_GLOBAL_WIDGET_SPACING; 
        hasKeyBoard = false; 
    }    
    
    void saveSettings(string fileName)
    {
        try
        {
            XmlTree settings( "Settings", "" );
            for(int i = 0; i < widgetsWithState.size(); i++)
            {                
                XmlTree widget( "Widget", "" );
                widget.push_back( XmlTree( "Kind", numToString(widgetsWithState[i]->getKind(),0) ) );
                widget.push_back( XmlTree( "Name", widgetsWithState[i]->getName() ) );
                writeSpecificWidgetData(widgetsWithState[i], widget); 
                settings.push_back( widget );
            }
            settings.write( writeFile( "settings/"+fileName , true) );        
        }
        catch (Exception e)
        {
            cout << "CIUI: Could not save file: " << fileName << endl; 
        }
    }
    
    void writeSpecificWidgetData(ciUIWidget *widget, XmlTree &XML)
    {
        int kind = widget->getKind();        
        switch (kind) {
            case CI_UI_WIDGET_IMAGETOGGLE:    
            case CI_UI_WIDGET_MULTIIMAGETOGGLE: 
            case CI_UI_WIDGET_LABELTOGGLE:                
            case CI_UI_WIDGET_TOGGLE:
            {
                ciUIToggle *toggle = (ciUIToggle *) widget; 
                XML.push_back( XmlTree( "Value", numToString( toggle->getValue() ? 1 : 0) ) );                
            }
                break;
                
            case CI_UI_WIDGET_MULTIIMAGESLIDER_H:
            case CI_UI_WIDGET_MULTIIMAGESLIDER_V:                                 
            case CI_UI_WIDGET_IMAGESLIDER_H:
            case CI_UI_WIDGET_IMAGESLIDER_V:
            case CI_UI_WIDGET_BILABELSLIDER:
            case CI_UI_WIDGET_CIRCLESLIDER:
            case CI_UI_WIDGET_MINIMALSLIDER:                
            case CI_UI_WIDGET_SLIDER_H:
            case CI_UI_WIDGET_SLIDER_V:
            {
                ciUISlider *slider = (ciUISlider *) widget; 
                XML.push_back( XmlTree( "Value", numToString( slider->getScaledValue() ) )  );
            }
                break;

            case CI_UI_WIDGET_RSLIDER_H:
            case CI_UI_WIDGET_RSLIDER_V:
            {
                ciUIRangeSlider *rslider = (ciUIRangeSlider *) widget; 
                XML.push_back( XmlTree( "HighValue", numToString( rslider->getScaledValueHigh() ) ) ); 
                XML.push_back( XmlTree( "LowValue", numToString( rslider->getScaledValueLow() ) ) );              
            }
                break;

            case CI_UI_WIDGET_NUMBERDIALER:
            {
                ciUINumberDialer *numdialer = (ciUINumberDialer *) widget; 
                XML.push_back( XmlTree( "Value", numToString( numdialer->getValue() ) ) );
            }
                break;

            case CI_UI_WIDGET_2DPAD:
            {
                ciUI2DPad *pad = (ciUI2DPad *) widget; 
                XML.push_back( XmlTree( "XValue", numToString( pad->getScaledValue().x ) ) ); 
                XML.push_back( XmlTree( "YValue", numToString( pad->getScaledValue().y ) ) );            
            }
                break;

            case CI_UI_WIDGET_TEXTINPUT:
            {
                ciUITextInput *textInput = (ciUITextInput *) widget; 
                XML.push_back( XmlTree( "Value", textInput->getTextString() ) );                 
            }
                break;
                                
            case CI_UI_WIDGET_ROTARYSLIDER:
            {
                ciUIRotarySlider *rotslider = (ciUIRotarySlider *) widget;
                XML.push_back( XmlTree( "Value", numToString( rotslider->getScaledValue() ) ) ); 
            }
                break;
                
            case CI_UI_WIDGET_IMAGESAMPLER:
            {
                ciUIImageSampler *imageSampler = (ciUIImageSampler *) widget;                 
                XML.push_back( XmlTree( "XValue", numToString( imageSampler->getValue().x ) ) ); 
                XML.push_back( XmlTree( "YValue", numToString( imageSampler->getValue().y ) ) );                
                XML.push_back( XmlTree( "RColor", numToString( imageSampler->getColor().r ) ) );                               
                XML.push_back( XmlTree( "GColor", numToString( imageSampler->getColor().g ) ) );                               
                XML.push_back( XmlTree( "BColor", numToString( imageSampler->getColor().b ) ) );                                               
                XML.push_back( XmlTree( "AColor", numToString( imageSampler->getColor().a ) ) );                               
            }
                break;

            default:
                break;
        }
    }

    void loadSettings(string fileName)
    {
        try
        {
            XmlTree XML( loadFile( "settings/"+fileName ) );         

            XmlTree settings = XML.getChild( "Settings" );        
            for( XmlTree::Iter item = settings.begin(); item != settings.end(); ++item )
            {        
                string wName = item->getChild("Name").getValue(); 
                ciUIWidget *widget = getWidget(wName); 
                if(widget != NULL)
                {
                    loadSpecificWidgetData(widget, *item); 
                    triggerEvent(widget); 
                }                     
            }
        }
        catch (Exception e)
        {
            cout << "CIUI: Could not load file: " << fileName << endl; 
        }
        hasKeyBoard = false;                
    }


    void loadSpecificWidgetData(ciUIWidget *widget, XmlTree &XML)
    {
        int kind = XML.getChild("Kind").getValue<int>();       
        switch (kind) 
        {
            case CI_UI_WIDGET_IMAGETOGGLE:    
            case CI_UI_WIDGET_MULTIIMAGETOGGLE: 
            case CI_UI_WIDGET_LABELTOGGLE:                
            case CI_UI_WIDGET_TOGGLE:
            {
                ciUIToggle *toggle = (ciUIToggle *) widget; 
                int value = XML.getChild("Value").getValue<int>();
                toggle->setValue((value ? 1 : 0)); 
            }
                break;

            case CI_UI_WIDGET_MULTIIMAGESLIDER_H:
            case CI_UI_WIDGET_MULTIIMAGESLIDER_V:                 
            case CI_UI_WIDGET_IMAGESLIDER_H:
            case CI_UI_WIDGET_IMAGESLIDER_V:                
            case CI_UI_WIDGET_BILABELSLIDER:    
            case CI_UI_WIDGET_CIRCLESLIDER:               
            case CI_UI_WIDGET_MINIMALSLIDER:
            case CI_UI_WIDGET_SLIDER_H:
            case CI_UI_WIDGET_SLIDER_V:
            {
                ciUISlider *slider = (ciUISlider *) widget; 
                float value = XML.getChild("Value").getValue<float>();
                slider->setValue(value); 
            }
                break;
                
            case CI_UI_WIDGET_RSLIDER_H:
            case CI_UI_WIDGET_RSLIDER_V:
            {
                ciUIRangeSlider *rslider = (ciUIRangeSlider *) widget; 
                float valueHigh = XML.getChild("HighValue").getValue<float>();
                float valueLow = XML.getChild("LowValue").getValue<float>();
                rslider->setValueHigh(valueHigh);
                rslider->setValueLow(valueLow);
            }
                break;
                
            case CI_UI_WIDGET_NUMBERDIALER:
            {
                ciUINumberDialer *numdialer = (ciUINumberDialer *) widget; 
                float value = XML.getChild("Value").getValue<float>();
                numdialer->setValue(value);                 
            }
                break;
                
            case CI_UI_WIDGET_2DPAD:
            {
                ciUI2DPad *pad = (ciUI2DPad *) widget; 
                float valueX = XML.getChild("XValue").getValue<float>();
                float valueY = XML.getChild("YValue").getValue<float>();
                pad->setValue(Vec2f(valueX, valueY)); 
            }
                break;
                
            case CI_UI_WIDGET_TEXTINPUT:
            {
                ciUITextInput *textInput = (ciUITextInput *) widget; 
                string value = XML.getChild("Value").getValue();
                textInput->setTextString(value); 
            }
                break;                
                
            case CI_UI_WIDGET_ROTARYSLIDER:
            {
                ciUIRotarySlider *rotslider = (ciUIRotarySlider *) widget;
                float value = XML.getChild("Value").getValue<float>();
                rotslider->setValue(value); 
            }
                break;
                
            case CI_UI_WIDGET_IMAGESAMPLER:
            {
                ciUIImageSampler *imageSampler = (ciUIImageSampler *) widget; 
                float valueX = XML.getChild("XValue").getValue<float>();
                float valueY = XML.getChild("YValue").getValue<float>();
                
                float r = XML.getChild("RColor").getValue<float>();
                float g = XML.getChild("GColor").getValue<float>();
                float b = XML.getChild("BColor").getValue<float>();
                float a = XML.getChild("AColor").getValue<float>();
                
                imageSampler->setValue(Vec2f(valueX, valueY));
                imageSampler->setColor(ColorA(r,g,b,a));
            }
                break;
                
            default:
                break;
        }        
    }

    
    gl::TextureFontRef getFontLarge()
    {
        return font_large;
    }    
    
    gl::TextureFontRef getFontMedium()
    {
        return font_medium;
    }

    gl::TextureFontRef getFontSmall()
    {
        return font_small;
    }

    bool setFont(string filename)
    {
        bool large = updateFont(CI_UI_FONT_LARGE, filename, CI_UI_FONT_LARGE_SIZE);
        bool medium = updateFont(CI_UI_FONT_MEDIUM, filename, CI_UI_FONT_MEDIUM_SIZE);
        bool small = updateFont(CI_UI_FONT_SMALL, filename, CI_UI_FONT_SMALL_SIZE);
        bool successful = large && medium && small;
        if( successful ) fontName = filename;
        return successful;
    }
    
    string getFontName()
    {
        return fontName; 
    }
    
    void setFontSize(ciUIWidgetFontType _kind, int _size, int _resolution = CI_UI_FONT_RESOLUTION)
    {
        switch(_kind)
        {
            case CI_UI_FONT_LARGE:                                              
                fontLarge = Font( loadResource(fontName), _size);                
                font_large = gl::TextureFont::create(fontLarge);                
                break; 

            case CI_UI_FONT_MEDIUM:
                fontMedium = Font( loadResource(fontName), _size);                
                font_medium = gl::TextureFont::create(fontMedium);                
                
                break; 

            case CI_UI_FONT_SMALL:
                fontSmall = Font( loadResource(fontName), _size);                
                font_small = gl::TextureFont::create(fontSmall);                
                
                break; 
        }
    }
    
    void setWidgetSpacing(float _widgetSpacing)
    {
        widgetSpacing = _widgetSpacing; 
    }
    
    float getWidgetSpacing()
    {
        return widgetSpacing;
    }
   
    bool isEnabled()
	{
		return enabled; 
	}
	
    void setVisible(bool _visible)
    {
        visible = _visible; 
        if(visible)
        {
            enable();
        }
        else
        {
            disable();
        }
    }
    
    
	void toggleVisible()
	{
		if(isEnabled())
		{
			disable(); 
		}
		else {
			enable(); 
		}
	}

    bool hasKeyboardFocus()
    {
        return hasKeyBoard; 
    }
	
	void enable()
	{
        if(!isEnabled())
        {            
            enabled = true; 
#if defined( CINDER_COCOA_TOUCH )
            enableTouchEventCallbacks();
#else
            enableMouseEventCallbacks();
            enableKeyEventCallbacks();            
#endif         
        }
	}
	
	void disable()
	{
        if(isEnabled())
        {                    
            enabled = false;       
#if defined( CINDER_COCOA_TOUCH )
            disableTouchEventCallbacks();
#else
            disableMouseEventCallbacks();    
            disableKeyEventCallbacks();
#endif
        }
    }
		
#if defined( CINDER_COCOA_TOUCH )
	
	//Touch Callbacks
    void enableTouchEventCallbacks()
    {
        mCbTouchesBegan = mApp->registerTouchesBegan(this, &ciUICanvas::canvasTouchesBegan); 
        mCbTouchesMoved = mApp->registerTouchesMoved(this, &ciUICanvas::canvasTouchesMoved); 
        mCbTouchesEnded = mApp->registerTouchesEnded(this, &ciUICanvas::canvasTouchesEnded); 
    }	

	void disableTouchEventCallbacks()
    {
        mApp->unregisterTouchesBegan( mCbTouchesBegan );
        mApp->unregisterTouchesMoved( mCbTouchesMoved );
        mApp->unregisterTouchesEnded( mCbTouchesEnded );
    }	
	
#else
	
	//Mouse Callbacks
    void enableMouseEventCallbacks()
    {
        mCbMouseDown = mApp->registerMouseDown( this, &ciUICanvas::canvasMouseDown );
        mCbMouseUp = mApp->registerMouseUp( this, &ciUICanvas::canvasMouseUp );	
        mCbMouseMove = mApp->registerMouseMove( this, &ciUICanvas::canvasMouseMove );
        mCbMouseDrag = mApp->registerMouseDrag( this, &ciUICanvas::canvasMouseDrag );	
    }

	//Mouse Callbacks
    void disableMouseEventCallbacks()
    {
        mApp->unregisterMouseDown( mCbMouseDown );
        mApp->unregisterMouseUp( mCbMouseUp );	
        mApp->unregisterMouseMove( mCbMouseMove );
        mApp->unregisterMouseDrag( mCbMouseDrag );	
    }	

    //KeyBoard Callbacks
	void enableKeyEventCallbacks()
	{
        mCbKeyDown = mApp->registerKeyDown( this, &ciUICanvas::canvasKeyDown );
        mCbKeyUp = mApp->registerKeyUp( this, &ciUICanvas::canvasKeyUp );
	}

	//KeyBoard Callbacks
	void disableKeyEventCallbacks()
	{
        mApp->unregisterKeyDown( mCbKeyDown ); 
        mApp->unregisterKeyUp( mCbKeyUp );         
	}
    
#endif	    
    virtual void update()
    {		
        if(enabled)
        {           
            for(int i = 0; i < widgets.size(); i++)
            {
                widgets[i]->update(); 	
            }		
        }
    }
     
    void draw()
    {
        if(enabled)
        {                   
            glDisable(GL_DEPTH_TEST);       
            glDisable(GL_LIGHTING);
            gl::enableAlphaBlending(); 
            
            glLineWidth(1.5f);
            
            drawPadded();
            
            drawPaddedOutline(); 
            
            drawBack(); 
            
            drawFill(); 
            
            drawFillHighlight(); 
            
            drawOutline(); 
            
            drawOutlineHighlight();

            for(int i = widgets.size()-1; i >= 0; i--)
            {
                if(widgets[i]->isVisible())
                {
                    widgets[i]->draw(); 	
                }
            }
            
            glDisable(GL_DEPTH_TEST); 
        }
    }
    
    void exit()
    {

    }	
        
#if defined( CINDER_COCOA_TOUCH )

	virtual bool canvasTouchesBegan(TouchEvent event) 
	{		
        for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) 
        {                
            if(rect->inside(touchIt->getX(), touchIt->getY()))
            {
                for(int i = 0; i < widgets.size(); i++)
                {
                    if(widgets[i]->isVisible())	widgets[i]->touchesBegan(event); 
                }
            }
        }
        return false;         
	}
    
	virtual bool canvasTouchesMoved(TouchEvent event) 
	{        
        for(int i = 0; i < widgets.size(); i++)
        {
            if(widgets[i]->isVisible())	widgets[i]->touchesMoved(event);
        }
        return false;         
    }
    
	virtual bool canvasTouchesEnded(TouchEvent event) 
	{
        for(int i = 0; i < widgets.size(); i++)
        {
            if(widgets[i]->isVisible())	widgets[i]->touchesEnded(event); 
        }
        return false;         
	}
    	
#else	
    
	virtual bool canvasMouseMove( MouseEvent event ) 
    {
        if(rect->inside(event.getX(), event.getY()))
        {
			for(int i = 0; i < widgets.size(); i++)
			{                
				if(widgets[i]->isVisible()) widgets[i]->mouseMove(event.getX(), event.getY()); 
			}
		}	
        return false; 
    }
    
    virtual bool canvasMouseDrag( MouseEvent event ) 
    {		
        for(int i = 0; i < widgets.size(); i++)
        {
            if(widgets[i]->isVisible())	widgets[i]->mouseDrag(event.getX(), event.getY(), event.isRightDown()); 
        }     
        return false;         
    }
    
    virtual bool canvasMouseDown( MouseEvent event ) 
    {
        if(rect->inside(event.getX(), event.getY()))
        {
			for(int i = 0; i < widgets.size(); i++)
			{
				if(widgets[i]->isVisible()) widgets[i]->mouseDown(event.getX(), event.getY(), event.isRightDown()); 
			}
		}		
        return false;         
    }
    
    virtual bool canvasMouseUp( MouseEvent event ) 
    {		        
        for(int i = 0; i < widgets.size(); i++)
        {
            if(widgets[i]->isVisible()) widgets[i]->mouseUp(event.getX(), event.getY(), event.isRightDown()); 
        }    
       return false;  
    }
	    
#endif	

    virtual bool canvasKeyDown( KeyEvent event )
    {
        for(int i = 0; i < widgets.size(); i++)
		{
			widgets[i]->keyDown(event);
		}
        return false; 
    }

    virtual bool canvasKeyUp( KeyEvent event )
    {
		for(int i = 0; i < widgets.size(); i++)
		{
			widgets[i]->keyUp(event);
		}
        return false; 
    }
	
    bool isHit(int x, int y)
    {
        if(isEnabled())
        {
            return rect->inside(x, y);
        }
        else
        {
            return false; 
        }
    }
    
    ciUIWidget *getWidgetHit(float x, float y)
    {
        if(isEnabled() && rect->inside(x, y))
        {
            for(int i = 0; i < widgets.size(); i++)
            {
                if(widgets[i]->isHit(x, y))
                {
                    return widgets[i]; 
                }
            }
            return NULL;
        }
        else
        {
            return NULL; 
        }        
    }
    

    void stateChange()
    {        
        switch (state) 
		{
            case CI_UI_STATE_NORMAL:
            {            
                draw_fill_highlight = false;             
                draw_outline_highlight = false;             
            }
                break;
            case CI_UI_STATE_OVER:
            {
                draw_fill_highlight = false;        
				if(enable_highlight_outline)
				{
					draw_outline_highlight = true;                         
				}
            }
                break;
            case CI_UI_STATE_DOWN:
            {
				if(enable_highlight_fill)
				{
					draw_fill_highlight = true;     				
				}
                draw_outline_highlight = false;             
            }
                break;
            case CI_UI_STATE_SUSTAINED:
            {
                draw_fill_highlight = false;            
                draw_outline_highlight = false;                         
            }
                break;            
                
            default:
                break;
        }        
    }
    
    void autoSizeToFitWidgets()
    {        
        float maxWidth = 0;
        float maxHeight = 0;

        for(int i = 0; i < widgets.size(); i++)
        {
            if(widgets[i]->isVisible())
            {
                ciUIRectangle* wr = widgets[i]->getRect(); 
                float widgetwidth = wr->getRawX()+wr->getWidth();

                float widgetheight = wr->getRawY()+wr->getHeight();
                
                if(widgetwidth > maxWidth)
                {
                    maxWidth = wr->getRawX()+widgets[i]->getPaddingRect()->getWidth();
                }                        
                if(widgetheight > maxHeight)
                {
                    maxHeight = wr->getRawY()+widgets[i]->getPaddingRect()->getHeight();                                                                        
                }        
            }
        }
        
        rect->setWidth(maxWidth);
        rect->setHeight(maxHeight);
        paddedRect->setWidth(rect->getWidth()+padding*2.0);
        paddedRect->setHeight(rect->getHeight()+padding*2.0);        
    }
    
    void centerWidgetsOnCanvas(bool centerHorizontally=true, bool centerVertically=true)
    {            
        float xMin = 0; 
        float yMin = 0;
        
        float xMax = 0; 
        float yMax = 0;
        
        float w = 0; 
        float h = 0; 
        
        for(int i = 0; i < widgets.size(); i++)
        {
//            if(widgets[i]->isVisible())
//            {
                ciUIRectangle* wr = widgets[i]->getRect(); 
                if(wr->getRawX() < xMin)
                {
                    xMin = wr->getRawX(); 
                }
                if((wr->getRawX() + wr->getWidth()) > xMax)
                {
                    xMax = (wr->getRawX() + wr->getWidth()); 
                }
                
                if(wr->getRawY() < yMin)
                {
                    yMin = wr->getRawY(); 
                }
                if((wr->getRawY() + wr->getHeight()) > yMax)
                {
                    yMax = (wr->getRawY() + wr->getHeight()); 
                }                                                                    
//            }
        }     
        
        w = xMax - xMin;
        h = yMax - yMin;            
        
        float moveDeltaX = rect->getHalfWidth() - w*.5; 
        float moveDeltaY = rect->getHalfHeight() - h*.5;
                
        for(int i = 0; i < widgets.size(); i++)
        {
            if(widgets[i]->isVisible() && !(widgets[i]->isEmbedded()))
            {
                ciUIRectangle* wr = widgets[i]->getRect(); 
                if(centerHorizontally) wr->setX(wr->getRawX()+ moveDeltaX-padding);             
                if(centerVertically) wr->setY(wr->getRawY()+ moveDeltaY-padding);                             
            }
        }           
//        addWidget(new ciUISpacer(xMin+moveDeltaX, yMin+moveDeltaY, w, h));
    }    
    
    void centerWidgetsHorizontallyOnCanvas()
    {
        centerWidgetsOnCanvas(true, false);
    }

    void centerWidgetsVerticallyOnCanvas()
    {
        centerWidgetsOnCanvas(false, true);
    }
    
    void centerWidgets()
    {
        centerWidgetsOnCanvas(); 
    }
    
    void removeWidget(ciUIWidget *widget)
    {
//        cout << endl; 
//        cout << "Widget to find: " << widget->getName() << endl; 
//        cout << endl; 
        
        //for the map
        map<string, ciUIWidget*>::iterator it;        
        it=widgets_map.find(widget->getName());
        if(it != widgets_map.end())
        {
            widgets_map.erase(it);     
        }
                
        for(int i = 0; i < widgetsWithState.size(); i++)
        {
            ciUIWidget *other = widgetsWithState[i]; 
            if(widget->getName() == other->getName())
            {
                widgetsWithState.erase(widgetsWithState.begin()+i);                
                break; 
            }
        }
        vector<ciUIWidget *>::iterator wit;
        //for all the widgets 
        for(wit=widgets.begin(); wit != widgets.end(); wit++)
        {
            ciUIWidget *other = *wit;                   
            if(widget->getName() == other->getName())
            { 
                widgets.erase(wit);                             
                break; 
            }
        }
        
        if(widget->hasLabel())
        {
            ciUIWidgetWithLabel *wwl = (ciUIWidgetWithLabel *) widget; 
            ciUILabel *label = wwl->getLabelWidget();
            removeWidget(label);
        }        

        delete widget;
    }    

    void addWidget(ciUIWidget *widget)
	{
        if(widget->getKind() == CI_UI_WIDGET_LABEL)
		{
			ciUILabel *label = (ciUILabel *) widget;
			setLabelFont(label); 
		}
		else if(widget->getKind() == CI_UI_WIDGET_SLIDER_H || widget->getKind() == CI_UI_WIDGET_SLIDER_V || widget->getKind() == CI_UI_WIDGET_BILABELSLIDER || widget->getKind() == CI_UI_WIDGET_MINIMALSLIDER || widget->getKind() == CI_UI_WIDGET_CIRCLESLIDER || widget->getKind() == CI_UI_WIDGET_IMAGESLIDER_H || widget->getKind() == CI_UI_WIDGET_IMAGESLIDER_V || widget->getKind() == CI_UI_WIDGET_MULTIIMAGESLIDER_H || widget->getKind() == CI_UI_WIDGET_MULTIIMAGESLIDER_V)           
		{
			ciUISlider *slider = (ciUISlider *) widget;
			ciUILabel *label = (ciUILabel *) slider->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 				

            if(widget->getKind() == CI_UI_WIDGET_BILABELSLIDER)
            {
                ciUIBiLabelSlider *biSlider = (ciUIBiLabelSlider *) widget;
                ciUILabel *rlabel = (ciUILabel *) biSlider->getRightLabel();
                setLabelFont(rlabel); 			
                pushbackWidget(rlabel); 				                
            }
            
            widgetsWithState.push_back(widget);                         
		}
		else if(widget->getKind() == CI_UI_WIDGET_2DPAD)		
		{
			ciUI2DPad *pad = (ciUI2DPad *) widget;
			ciUILabel *label = (ciUILabel *) pad->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 				

            widgetsWithState.push_back(widget);             
		}		
		else if(widget->getKind() == CI_UI_WIDGET_IMAGE)		
		{
			ciUIImage *image = (ciUIImage *) widget;
			ciUILabel *label = (ciUILabel *) image->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 				
		}	
		else if(widget->getKind() == CI_UI_WIDGET_IMAGESAMPLER)		
		{
			ciUIImage *image = (ciUIImage *) widget;
			ciUILabel *label = (ciUILabel *) image->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 				
            
            widgetsWithState.push_back(widget);                                     
		}	        
		else if(widget->getKind() == CI_UI_WIDGET_RSLIDER_H || widget->getKind() == CI_UI_WIDGET_RSLIDER_V)
		{
			ciUIRangeSlider *rslider = (ciUIRangeSlider *) widget;
			ciUILabel *label = (ciUILabel *) rslider->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 			
            
            widgetsWithState.push_back(widget);                         
		}		
		else if(widget->getKind() == CI_UI_WIDGET_ROTARYSLIDER)
		{
			ciUIRotarySlider *rslider = (ciUIRotarySlider *) widget;
			ciUILabel *label = (ciUILabel *) rslider->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 				
            
            widgetsWithState.push_back(widget);                         
		}		
		else if(widget->getKind() == CI_UI_WIDGET_BUTTON || widget->getKind() == CI_UI_WIDGET_TOGGLE || widget->getKind() ==  CI_UI_WIDGET_LABELBUTTON || widget->getKind() == CI_UI_WIDGET_LABELTOGGLE || widget->getKind() == CI_UI_WIDGET_MULTIIMAGEBUTTON || widget->getKind() == CI_UI_WIDGET_MULTIIMAGETOGGLE)
		{
			ciUIButton *button = (ciUIButton *) widget;
			ciUILabel *label = (ciUILabel *) button->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 		
            if(widget->getKind() != CI_UI_WIDGET_BUTTON && widget->getKind() != CI_UI_WIDGET_LABELBUTTON && widget->getKind() != CI_UI_WIDGET_MULTIIMAGEBUTTON)
            {
                widgetsWithState.push_back(widget);                         
            }
		}
        else if(widget->getKind() == CI_UI_WIDGET_DROPDOWNLIST)            
        { 
			ciUIDropDownList *list = (ciUIDropDownList *) widget;
			ciUILabel *label = (ciUILabel *) list->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 		
            
            vector<ciUILabelToggle *> toggles = list->getToggles(); 
			for(int i = 0; i < toggles.size(); i++)
			{
				ciUILabelToggle *t = toggles[i]; 
				ciUILabel *l2 = (ciUILabel *) t->getLabel();
				setLabelFont(l2); 	
                pushbackWidget(l2); 					
                pushbackWidget(t); 

                widgetsWithState.push_back(t);             
			}            
        }
		else if(widget->getKind() == CI_UI_WIDGET_TEXTINPUT)
		{
			ciUITextInput *textinput = (ciUITextInput *) widget;
			ciUILabel *label = (ciUILabel *) textinput->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 	
            
            widgetsWithState.push_back(widget);             
		}		
		else if(widget->getKind() == CI_UI_WIDGET_NUMBERDIALER)
		{
			ciUINumberDialer *numberDialer = (ciUINumberDialer *) widget;
			ciUILabel *label = (ciUILabel *) numberDialer->getLabel();
			setLabelFont(label); 			
			pushbackWidget(label); 			
            
            widgetsWithState.push_back(widget);                         
		}		        
		else if(widget->getKind() == CI_UI_WIDGET_RADIO)
		{
			ciUIRadio *radio = (ciUIRadio *) widget;
			ciUILabel *label = (ciUILabel *) radio->getLabel();			
			setLabelFont(label); 
			pushbackWidget(label); 				
			
			vector<ciUIToggle *> toggles = radio->getToggles(); 
			
			for(int i = 0; i < toggles.size(); i++)
			{
				ciUIToggle *t = toggles[i]; 
				ciUILabel *l2 = (ciUILabel *) t->getLabel();
				setLabelFont(l2); 	
				pushbackWidget(t); 
				pushbackWidget(l2);
                
                widgetsWithState.push_back(t);                             
			}
		}
        else if(widget->getKind() == CI_UI_WIDGET_TOGGLEMATRIX)
		{
			ciUIToggleMatrix *matrix = (ciUIToggleMatrix *) widget;
			ciUILabel *label = (ciUILabel *) matrix->getLabel();			
			setLabelFont(label); 
			pushbackWidget(label); 				
			
			vector<ciUIToggle *> toggles = matrix->getToggles(); 
			
			for(int i = 0; i < toggles.size(); i++)
			{
				ciUIToggle *t = toggles[i]; 
				ciUILabel *l2 = (ciUILabel *) t->getLabel();
				setLabelFont(l2); 	
				pushbackWidget(t); 
				pushbackWidget(l2); 			
                
                widgetsWithState.push_back(t);                             
			}
		}
		else if(widget->getKind() == CI_UI_WIDGET_FPS)
		{
			ciUILabel *fps = (ciUILabel *) widget;
			setLabelFont(fps); 
		}
        else if(widget->getKind() == CI_UI_WIDGET_IMAGETOGGLE)
        {
            widgetsWithState.push_back(widget);              
        }
	
		widget->setRectParent(this->rect); 		
		widget->setParent(this); 
		pushbackWidget(widget); 	
	}
    
	ciUIWidget* addWidgetDown(ciUIWidget *widget, ciUIWidgetAlignment align = CI_UI_ALIGN_LEFT, bool reAdd = false)
	{
        if(!reAdd) addWidget(widget); 
        ciUIRectangle *widgetRect = widget->getRect();         
		if(lastAdded != NULL)
		{
			ciUIRectangle *lastPaddedRect = lastAdded->getPaddingRect(); 
			widgetRect->setY(lastPaddedRect->getY()+lastPaddedRect->getHeight()-rect->getY()+widgetSpacing); 
		}
		else 
		{
			widgetRect->setY(widgetSpacing);
		}
        switch(align)
        {
            case CI_UI_ALIGN_TOP: 
                
                break; 
            case CI_UI_ALIGN_BOTTOM:
                
                break;
                
            case CI_UI_ALIGN_LEFT: 
                widgetRect->setX(widgetSpacing);             
                break;                    
            case CI_UI_ALIGN_FREE: 
                
                break; 
            case CI_UI_ALIGN_RIGHT: 
                widgetRect->setX(rect->getWidth()-widgetRect->getWidth()-widgetSpacing); 
                break;                     
        }                           
		lastAdded = widget; 
		return widget;
	}
    
	ciUIWidget* addWidgetUp(ciUIWidget *widget, ciUIWidgetAlignment align = CI_UI_ALIGN_LEFT, bool reAdd = false)
	{
        if(!reAdd) addWidget(widget); 
        ciUIRectangle *widgetRect = widget->getRect();                 
		if(lastAdded != NULL)
		{
			ciUIRectangle *lastPaddedRect = lastAdded->getPaddingRect(); 
            ciUIRectangle *widgetPaddedRect = widget->getPaddingRect();                                     
			widgetRect->setY(lastPaddedRect->getY()-widgetPaddedRect->getHeight()-rect->getY());                                     
		}
		else 
		{
			widgetRect->setY(widgetSpacing);             
		}
        switch(align)
        {
            case CI_UI_ALIGN_TOP: 
                
                break; 
            case CI_UI_ALIGN_BOTTOM:
                
                break;
                
            case CI_UI_ALIGN_LEFT: 
                widgetRect->setX(widgetSpacing);             
                break;                    
            case CI_UI_ALIGN_FREE: 
                
                break; 
            case CI_UI_ALIGN_RIGHT: 
                widgetRect->setX(rect->getWidth()-widgetRect->getWidth()-widgetSpacing); 
                break;                     
        }          
		lastAdded = widget; 
		return widget;
	}    
    
	ciUIWidget* addWidgetRight(ciUIWidget *widget, ciUIWidgetAlignment align = CI_UI_ALIGN_FREE, bool reAdd = false)
	{
        if(!reAdd) addWidget(widget); 
        ciUIRectangle *widgetRect = widget->getRect();                         
		if(lastAdded != NULL)
		{
			ciUIRectangle *lastRect = lastAdded->getRect(); 
			ciUIRectangle *lastPaddedRect = lastAdded->getPaddingRect();                         
			
            widgetRect->setX(lastPaddedRect->getX()+lastPaddedRect->getWidth()-rect->getX()+widgetSpacing); 
			widgetRect->setY(lastRect->getY()-rect->getY()); 
		}
		else 
		{
			widgetRect->setX(widgetSpacing); 
            widgetRect->setY(widgetSpacing);                         
		}
        switch(align)
        {
            case CI_UI_ALIGN_LEFT: 
                widgetRect->setX(widgetSpacing);             
                break;                    
            case CI_UI_ALIGN_FREE: 
                
                break; 
            case CI_UI_ALIGN_RIGHT: 
                widgetRect->setX(rect->getWidth()-widgetRect->getWidth()-widgetSpacing); 
                break;                     
            case CI_UI_ALIGN_TOP: 
                widgetRect->setY(widgetSpacing);                         
                break;                     
            case CI_UI_ALIGN_BOTTOM: 
                widgetRect->setY(rect->getHeight()-widgetRect->getHeight()-widgetSpacing); 
                break;                                     
        }                  
        lastAdded = widget; 
        return widget; 	
	}
    
    ciUIWidget* addWidgetLeft(ciUIWidget *widget, ciUIWidgetAlignment align = CI_UI_ALIGN_FREE, bool reAdd = false)
	{
        if(!reAdd) addWidget(widget);     
        ciUIRectangle *widgetRect = widget->getRect(); 
		if(lastAdded != NULL)
		{
			ciUIRectangle *lastRect = lastAdded->getRect(); 
			ciUIRectangle *lastPaddedRect = lastAdded->getPaddingRect();                         
            ciUIRectangle *widgetPaddedRect = widget->getPaddingRect();                         
			
            widgetRect->setX(lastPaddedRect->getX()-widgetPaddedRect->getWidth()-rect->getX()); 
			widgetRect->setY(lastRect->getY()-rect->getY()); 
		}
		else 
		{
			widgetRect->setX(widgetSpacing); 
            widgetRect->setY(widgetSpacing);                         
		}
        switch(align)
        {
            case CI_UI_ALIGN_LEFT: 
                widgetRect->setX(widgetSpacing);             
                break;                    
            case CI_UI_ALIGN_FREE: 
                
                break; 
            case CI_UI_ALIGN_RIGHT: 
                widgetRect->setX(rect->getWidth()-widgetRect->getWidth()-widgetSpacing); 
                break;        
            case CI_UI_ALIGN_TOP: 
                widgetRect->setY(widgetSpacing);                         
                break;                     
            case CI_UI_ALIGN_BOTTOM: 
                widgetRect->setY(rect->getHeight()-widgetRect->getHeight()-widgetSpacing); 
                break;                                                     
        }                          
        lastAdded = widget;  	
        return widget;
	}    
    
    ciUIWidget* addWidgetSouthOf(ciUIWidget *widget, string referenceName, bool reAdd = false)
	{
        if(!reAdd) addWidget(widget); 
        ciUIWidget *referenceWidget = getWidget(referenceName);
		if(referenceWidget != NULL)
		{
			ciUIRectangle *referencePaddedRect = referenceWidget->getPaddingRect(); 
			ciUIRectangle *referenceRect = referenceWidget->getRect();             
			ciUIRectangle *widgetRect = widget->getRect(); 
            
			widgetRect->setY(referencePaddedRect->getY()+referencePaddedRect->getHeight()-rect->getY()+widgetSpacing); 
            widgetRect->setX(referenceRect->getX()-rect->getX());   
		}
		else 
		{
			ciUIRectangle *widgetRect = widget->getRect(); 
			widgetRect->setY(widgetSpacing); 
		}
        lastAdded = widget;  		
        return widget;
    }    
    
    ciUIWidget* addWidgetNorthOf(ciUIWidget *widget, string referenceName, bool reAdd = false)
	{
        if(!reAdd) addWidget(widget); 
        ciUIWidget *referenceWidget = getWidget(referenceName);
		if(referenceWidget != NULL)
		{
			ciUIRectangle *referencePaddedRect = referenceWidget->getPaddingRect(); 
			ciUIRectangle *referenceRect = referenceWidget->getRect();             
			ciUIRectangle *widgetRect = widget->getRect(); 
            ciUIRectangle *widgetPaddedRect = widget->getPaddingRect();             
            
			widgetRect->setY(referencePaddedRect->getY()-widgetPaddedRect->getHeight()-rect->getY());             
            widgetRect->setX(referenceRect->getX()-rect->getX());   
		}
		else 
		{
			ciUIRectangle *widgetRect = widget->getRect(); 
			widgetRect->setY(widgetSpacing); 
		}
        lastAdded = widget;  	        
        return widget;
    }      
    
    ciUIWidget* addWidgetWestOf(ciUIWidget *widget, string referenceName, bool reAdd = false)
	{
        if(!reAdd) addWidget(widget);      
        ciUIWidget *referenceWidget = getWidget(referenceName);
		if(referenceWidget != NULL)
		{
			ciUIRectangle *referencePaddedRect = referenceWidget->getPaddingRect(); 
			ciUIRectangle *referenceRect = referenceWidget->getRect();             			
            ciUIRectangle *widgetRect = widget->getRect(); 
            ciUIRectangle *widgetPaddedRect = widget->getPaddingRect();             
            
            widgetRect->setY(referenceRect->getY()-rect->getY());                         
            widgetRect->setX(referencePaddedRect->getX()-rect->getX()-widgetPaddedRect->getWidth());   
		}
		else 
		{			
            ciUIRectangle *currentRect = widget->getRect(); 			
            currentRect->setY(widgetSpacing); 
		}
        lastAdded = widget;  	
        return widget;
    }        
    
    ciUIWidget* addWidgetEastOf(ciUIWidget *widget, string referenceName, bool reAdd = false)
	{
        if(!reAdd) addWidget(widget);     
        ciUIWidget *referenceWidget = getWidget(referenceName);
		if(referenceWidget != NULL)
		{
			ciUIRectangle *referencePaddedRect = referenceWidget->getPaddingRect(); 
			ciUIRectangle *referenceRect = referenceWidget->getRect();             			
            ciUIRectangle *widgetRect = widget->getRect(); 	            
            widgetRect->setY(referenceRect->getY()-rect->getY());                        
            widgetRect->setX(referencePaddedRect->getX()-rect->getX()+referencePaddedRect->getWidth()+widgetSpacing); 
		}
		else 
		{			
            ciUIRectangle *currentRect = widget->getRect(); 			
            currentRect->setY(widgetSpacing); 
		}
        lastAdded = widget;  	
        return widget;
    }         
    
    void resetPlacer()
    {
        lastAdded = NULL; 
    }
    
    void setPlacer(ciUIWidget *referenceWidget)
    {
        lastAdded = referenceWidget; 
    }
    
	void setLabelFont(ciUILabel *label)
	{
		switch(label->getSize())
		{
			case CI_UI_FONT_LARGE:
				label->setFont(font_large); 					
				break; 
			case CI_UI_FONT_MEDIUM:
				label->setFont(font_medium); 					
				break; 
			case CI_UI_FONT_SMALL:
				label->setFont(font_small); 					
				break; 					
		}		
	}
	
	void triggerEvent(ciUIWidget *child)
	{        
        checkForKeyFocus(child); 
		GUIevent->widget = child; 		
        uiEventCallbackMgr.call( GUIevent ); 
	}
    
    void setUIColors(ColorA &cb, ColorA &co, ColorA &coh, ColorA &cf, ColorA &cfh, ColorA &cp, ColorA &cpo)
    {
        setWidgetColor(CI_UI_WIDGET_COLOR_BACK, cb);
        setWidgetColor(CI_UI_WIDGET_COLOR_OUTLINE, co);                
        setWidgetColor(CI_UI_WIDGET_COLOR_OUTLINE_HIGHLIGHT, coh);
        setWidgetColor(CI_UI_WIDGET_COLOR_FILL, cf);                
        setWidgetColor(CI_UI_WIDGET_COLOR_FILL_HIGHLIGHT, cfh);
        setWidgetColor(CI_UI_WIDGET_COLOR_PADDED, cp);
        setWidgetColor(CI_UI_WIDGET_COLOR_PADDED_OUTLINE, cpo);
        
        setColorBack(cb);
        setColorOutline(co);
        setColorOutlineHighlight(coh);
        setColorFill(cf);
        setColorFillHighlight(cfh);
        setColorPadded(cp);
        setColorPaddedOutline(cpo); 
    }
	    
    void setTheme(int theme)
    {
        switch(theme)
        {
            case CI_UI_THEME_DEFAULT:
            {
                ColorA cb = CI_UI_COLOR_BACK; 
                ColorA co = CI_UI_COLOR_OUTLINE; 
                ColorA coh = CI_UI_COLOR_OUTLINE_HIGHLIGHT;
                ColorA cf = CI_UI_COLOR_FILL; 
                ColorA cfh = CI_UI_COLOR_FILL_HIGHLIGHT;
                ColorA cp = CI_UI_COLOR_PADDED;
                ColorA cpo = CI_UI_COLOR_PADDED_OUTLINE;
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_HACKER:
            {
                ColorA cb = ColorA( 0.294118, 0, 0.0588235, 0.196078 );
                ColorA co = ColorA( 0.254902, 0.239216, 0.239216, 0.392157 );
                ColorA coh = ColorA( 0.294118, 0, 0.0588235, 0.784314 );
                ColorA cf = ColorA( 0.784314, 1, 0, 0.784314 );
                ColorA cfh = ColorA( 0.980392, 0.00784314, 0.235294, 1 );
                ColorA cp = ColorA( 0.0156863, 0, 0.0156863, 0.392157 );
                ColorA cpo = ColorA( 0.254902, 0.239216, 0.239216, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

                
        case CI_UI_THEME_HIPSTER:
            {
                ColorA cb = ColorA( 0.607843, 0.6, 0.509804, 0.196078 );
                ColorA co = ColorA( 0.231373, 0.392157, 0.501961, 0.392157 );
                ColorA coh = ColorA( 0.607843, 0.6, 0.509804, 0.784314 );
                ColorA cf = ColorA( 1, 0.52549, 0.0666667, 0.784314 );
                ColorA cfh = ColorA( 0.0313725, 0.101961, 0.188235, 1 );
                ColorA cp = ColorA( 0.196078, 0.25098, 0.352941, 0.392157 );
                ColorA cpo = ColorA( 0.231373, 0.392157, 0.501961, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_DIETER:
            {
                ColorA cb = ColorA( 0.803922, 0.741176, 0.682353, 0.196078 );
                ColorA co = ColorA( 0.478431, 0.356863, 0.243137, 0.392157 );
                ColorA coh = ColorA( 0.803922, 0.741176, 0.682353, 0.784314 );
                ColorA cf = ColorA( 0.980392, 0.294118, 0, 0.784314 );
                ColorA cfh = ColorA( 0.980392, 0.980392, 0.980392, 1 );
                ColorA cp = ColorA( 0.121569, 0.121569, 0.121569, 0.392157 );
                ColorA cpo = ColorA( 0.478431, 0.356863, 0.243137, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_BARBIE:
            {
                ColorA cb = ColorA( 0.92549, 0, 0.54902, 0.196078 );
                ColorA co = ColorA( 0, 0, 0, 0.392157 );
                ColorA coh = ColorA( 0, 0.678431, 0.937255, 0.784314 );
                ColorA cf = ColorA( 0.92549, 0, 0.54902, 0.784314 );
                ColorA cfh = ColorA( 1, 0.94902, 0, 1 );
                ColorA cp = ColorA( 0, 0, 0, 0.392157 );
                ColorA cpo = ColorA( 0, 0.678431, 0.937255, 0.784314 ); 
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_WINDOWS:
            {
                ColorA cb = ColorA( 0.0470588, 0.0588235, 0.4, 0.196078 );
                ColorA co = ColorA( 0.0431373, 0.0627451, 0.54902, 0.392157 );
                ColorA coh = ColorA( 0.0470588, 0.0588235, 0.4, 0.784314 );
                ColorA cf = ColorA( 0.054902, 0.305882, 0.678431, 0.784314 );
                ColorA cfh = ColorA( 0.0627451, 0.498039, 0.788235, 1 );
                ColorA cp = ColorA( 0.027451, 0.0352941, 0.239216, 0.392157 );
                ColorA cpo = ColorA( 0.0431373, 0.0627451, 0.54902, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_MACOSX:
            {
                ColorA cb = ColorA( 0, 0.678431, 0.937255, 0.196078 );
                ColorA co = ColorA( 1, 0.94902, 0, 0.392157 );
                ColorA coh = ColorA( 0, 0, 0, 0.784314 );
                ColorA cf = ColorA( 0, 0.678431, 0.937255, 0.784314 );
                ColorA cfh = ColorA( 0.92549, 0, 0.54902, 1 );
                ColorA cp = ColorA( 1, 0.94902, 0, 0.392157 );
                ColorA cpo = ColorA( 0, 0, 0, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_ZOOLANDER:
            {
                ColorA cb = ColorA( 0.160784, 0.133333, 0.121569, 0.196078 );
                ColorA co = ColorA( 0.0745098, 0.454902, 0.490196, 0.392157 );
                ColorA coh = ColorA( 0.160784, 0.133333, 0.121569, 0.784314 );
                ColorA cf = ColorA( 0.988235, 0.207843, 0.298039, 0.784314 );
                ColorA cfh = ColorA( 0.988235, 0.968627, 0.772549, 1 );
                ColorA cp = ColorA( 0.0392157, 0.74902, 0.737255, 0.392157 );
                ColorA cpo = ColorA( 0.0745098, 0.454902, 0.490196, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_VEGAN2:
            {
                ColorA cb = ColorA( 0.745098, 0.94902, 0.00784314, 0.196078 );
                ColorA co = ColorA( 0.533333, 0.768627, 0.145098, 0.392157 );
                ColorA coh = ColorA( 0.745098, 0.94902, 0.00784314, 0.784314 );
                ColorA cf = ColorA( 0.917647, 0.992157, 0.901961, 0.784314 );
                ColorA cfh = ColorA( 0.105882, 0.403922, 0.419608, 1 );
                ColorA cp = ColorA( 0.317647, 0.584314, 0.282353, 0.392157 );
                ColorA cpo = ColorA( 0.533333, 0.768627, 0.145098, 0.784314 );                
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_BERLIN:
            {
                ColorA cb = ColorA( 0.6, 0.894118, 1, 0.196078 );
                ColorA co = ColorA( 0.294118, 0.34902, 0.419608, 0.392157 );
                ColorA coh = ColorA( 0.6, 0.894118, 1, 0.784314 );
                ColorA cf = ColorA( 0.968627, 0.309804, 0.309804, 0.784314 );
                ColorA cfh = ColorA( 1, 0.231373, 0.231373, 1 );
                ColorA cp = ColorA( 0.105882, 0.12549, 0.14902, 0.392157 );
                ColorA cpo = ColorA( 0.294118, 0.34902, 0.419608, 0.784314 ); 
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_METALGEAR:
            {
                ColorA cb = ColorA( 0.2, 0.172549, 0.172549, 0.294118 );
                ColorA co = ColorA( 0.0980392, 0.101961, 0.141176, 0.392157 );
                ColorA coh = ColorA( 0.2, 0.172549, 0.172549, 0.784314 );
                ColorA cf = ColorA( 0.980392, 0.396078, 0.341176, 0.784314 );
                ColorA cfh = ColorA( 1, 1, 1, 1 );
                ColorA cp = ColorA( 0, 0, 0, 0.392157 );
                ColorA cpo = ColorA( 0.0980392, 0.101961, 0.141176, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_TEALLIME:
            {
                ColorA cb = ColorA( 0.105882, 0.403922, 0.419608, 0.294118 );
                ColorA co = ColorA( 0.917647, 0.992157, 0.901961, 0.392157 );
                ColorA coh = ColorA( 0.105882, 0.403922, 0.419608, 0.784314 );
                ColorA cf = ColorA( 0.317647, 0.584314, 0.282353, 0.784314 );
                ColorA cfh = ColorA( 0.533333, 0.768627, 0.145098, 1 );
                ColorA cp = ColorA( 0.745098, 0.94902, 0.00784314, 0.392157 );
                ColorA cpo = ColorA( 0.917647, 0.992157, 0.901961, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_VEGAN:
            {
                ColorA cb = ColorA( 0.317647, 0.584314, 0.282353, 0.294118 );
                ColorA co = ColorA( 0.105882, 0.403922, 0.419608, 0.392157 );
                ColorA coh = ColorA( 0.317647, 0.584314, 0.282353, 0.784314 );
                ColorA cf = ColorA( 0.533333, 0.768627, 0.145098, 0.784314 );
                ColorA cfh = ColorA( 0.745098, 0.94902, 0.00784314, 1 );
                ColorA cp = ColorA( 0.917647, 0.992157, 0.901961, 0.392157 );
                ColorA cpo = ColorA( 0.105882, 0.403922, 0.419608, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_RUSTIC:
            {
                ColorA cb = ColorA( 0.768627, 0.713725, 0.427451, 0.294118 );
                ColorA co = ColorA( 0.968627, 0.427451, 0.235294, 0.392157 );
                ColorA coh = ColorA( 0.768627, 0.713725, 0.427451, 0.784314 );
                ColorA cf = ColorA( 0.835294, 0.152941, 0.0196078, 0.784314 );
                ColorA cfh = ColorA( 0.941176, 0.827451, 0.466667, 1 );
                ColorA cp = ColorA( 0.952941, 0.909804, 0.894118, 0.392157 );
                ColorA cpo = ColorA( 0.968627, 0.427451, 0.235294, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                            
            case CI_UI_THEME_MIDNIGHT:
            {
                ColorA cb = ColorA( 0.0431373, 0.282353, 0.419608, 0.294118 );
                ColorA co = ColorA( 0.811765, 0.941176, 0.619608, 0.392157 );
                ColorA coh = ColorA( 0.0431373, 0.282353, 0.419608, 0.784314 );
                ColorA cf = ColorA( 0.231373, 0.52549, 0.52549, 0.784314 );
                ColorA cfh = ColorA( 0.47451, 0.741176, 0.603922, 1 );
                ColorA cp = ColorA( 0.658824, 0.858824, 0.658824, 0.392157 );
                ColorA cpo = ColorA( 0.811765, 0.941176, 0.619608, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_MINBLUE:
            {
                ColorA cb = ColorA( 0.996078, 0.976471, 0.941176, 0.294118 );
                ColorA co = ColorA( 0.690196, 0.972549, 1, 0.392157 );
                ColorA coh = ColorA( 0.996078, 0.976471, 0.941176, 0.784314 );
                ColorA cf = ColorA( 0, 0.737255, 0.819608, 0.784314 );
                ColorA cfh = ColorA( 0.462745, 0.827451, 0.870588, 1 );
                ColorA cp = ColorA( 0.682353, 0.909804, 0.984314, 0.392157 );
                ColorA cpo = ColorA( 0.690196, 0.972549, 1, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                            
            case CI_UI_THEME_LIMESTONE:
            {
                ColorA cb = ColorA( 0.423529, 0.564706, 0.52549, 0.294118 );
                ColorA co = ColorA( 0.988235, 0.329412, 0.388235, 0.392157 );
                ColorA coh = ColorA( 0.423529, 0.564706, 0.52549, 0.784314 );
                ColorA cf = ColorA( 0.662745, 0.8, 0.0941176, 0.784314 );
                ColorA cfh = ColorA( 0.811765, 0.286275, 0.423529, 1 );
                ColorA cp = ColorA( 0.921569, 0.917647, 0.737255, 0.392157 );
                ColorA cpo = ColorA( 0.988235, 0.329412, 0.388235, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;                  
                
            case CI_UI_THEME_SPEARMINT:
            {
                ColorA cb = ColorA( 0.0980392, 0.54902, 0.0352941, 0.294118 );
                ColorA co = ColorA( 1, 0.772549, 0.372549, 0.392157 );
                ColorA coh = ColorA( 0.0980392, 0.54902, 0.0352941, 0.784314 );
                ColorA cf = ColorA( 0.862745, 0.980392, 0.980392, 0.784314 );
                ColorA cfh = ColorA( 0.937255, 0.345098, 0.552941, 1 );
                ColorA cp = ColorA( 0.996078, 0.662745, 0.0705882, 0.392157 );
                ColorA cpo = ColorA( 1, 0.772549, 0.372549, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;                  
                
            case CI_UI_THEME_MINPINK:
            {
                ColorA cb = ColorA( 0.862745, 0.980392, 0.980392, 0.294118 );
                ColorA co = ColorA( 0.0980392, 0.54902, 0.0352941, 0.392157 );
                ColorA coh = ColorA( 0.862745, 0.980392, 0.980392, 0.784314 );
                ColorA cf = ColorA( 0.937255, 0.345098, 0.552941, 0.784314 );
                ColorA cfh = ColorA( 0.996078, 0.662745, 0.0705882, 1 );
                ColorA cp = ColorA( 1, 0.772549, 0.372549, 0.392157 );
                ColorA cpo = ColorA( 0.0980392, 0.54902, 0.0352941, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_PEPTOBISMOL:
            {
                ColorA cb = ColorA( 0.87451, 0.0823529, 0.101961, 0.294118 );
                ColorA co = ColorA( 0, 0.854902, 0.235294, 0.392157 );
                ColorA coh = ColorA( 0.87451, 0.0823529, 0.101961, 0.784314 );
                ColorA cf = ColorA( 0.956863, 0.952941, 0.156863, 0.784314 );
                ColorA cfh = ColorA( 0.992157, 0.52549, 0.0117647, 1 );
                ColorA cp = ColorA( 0, 0.796078, 0.905882, 0.392157 );
                ColorA cpo = ColorA( 0, 0.854902, 0.235294, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_BILEBLUE:
            {
                ColorA cb = ColorA( 0.992157, 0.52549, 0.0117647, 0.294118 );
                ColorA co = ColorA( 0.956863, 0.952941, 0.156863, 0.392157 );
                ColorA coh = ColorA( 0.992157, 0.52549, 0.0117647, 0.784314 );
                ColorA cf = ColorA( 0, 0.796078, 0.905882, 0.784314 );
                ColorA cfh = ColorA( 0, 0.854902, 0.235294, 1 );
                ColorA cp = ColorA( 0.87451, 0.0823529, 0.101961, 0.392157 );
                ColorA cpo = ColorA( 0.956863, 0.952941, 0.156863, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_COOLCLAY:
            {
                ColorA cb = ColorA( 0.6, 0.894118, 1, 0.294118 );
                ColorA co = ColorA( 0.294118, 0.34902, 0.419608, 0.392157 );
                ColorA coh = ColorA( 0.6, 0.894118, 1, 0.784314 );
                ColorA cf = ColorA( 0.968627, 0.309804, 0.309804, 0.784314 );
                ColorA cfh = ColorA( 1, 0.231373, 0.231373, 1 );
                ColorA cp = ColorA( 0.105882, 0.12549, 0.14902, 0.392157 );
                ColorA cpo = ColorA( 0.294118, 0.34902, 0.419608, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_BLUEBLUE:
            {
                ColorA cb = ColorA( 0, 0.678431, 0.937255, 0.294118 );
                ColorA co = ColorA( 1, 0.94902, 0, 0.392157 );
                ColorA coh = ColorA( 0, 0, 0, 0.784314 );
                ColorA cf = ColorA( 0, 0.678431, 0.937255, 0.784314 );
                ColorA cfh = ColorA( 0.92549, 0, 0.54902, 1 );
                ColorA cp = ColorA( 1, 0.94902, 0, 0.392157 );
                ColorA cpo = ColorA( 0, 0, 0, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_PINKPANTHER:
            {
                ColorA cb = ColorA( 0.92549, 0, 0.54902, 0.294118 );
                ColorA co = ColorA( 0, 0, 0, 0.392157 );
                ColorA coh = ColorA( 0, 0.678431, 0.937255, 0.784314 );
                ColorA cf = ColorA( 0.92549, 0, 0.54902, 0.784314 );
                ColorA cfh = ColorA( 1, 0.94902, 0, 1 );
                ColorA cp = ColorA( 0, 0, 0, 0.392157 );
                ColorA cpo = ColorA( 0, 0.678431, 0.937255, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_MAROON:
            {
                ColorA cb = ColorA( 0.396078, 0.588235, 0.619608, 0.294118 );
                ColorA co = ColorA( 0.858824, 0.85098, 0.823529, 0.392157 );
                ColorA coh = ColorA( 0.396078, 0.588235, 0.619608, 0.784314 );
                ColorA cf = ColorA( 0.670588, 0.0784314, 0.172549, 0.784314 );
                ColorA cfh = ColorA( 0.741176, 0.858824, 0.870588, 1 );
                ColorA cp = ColorA( 0.803922, 0.831373, 0.423529, 0.392157 );
                ColorA cpo = ColorA( 0.858824, 0.85098, 0.823529, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
       
            case CI_UI_THEME_PINKLATTE:
            {
                ColorA cb = ColorA( 0.854902, 0.847059, 0.654902, 0.294118 );
                ColorA co = ColorA( 0.498039, 0.780392, 0.686275, 0.392157 );
                ColorA coh = ColorA( 0.854902, 0.847059, 0.654902, 0.784314 );
                ColorA cf = ColorA( 1, 0.239216, 0.498039, 0.784314 );
                ColorA cfh = ColorA( 1, 0.619608, 0.615686, 1 );
                ColorA cp = ColorA( 0.247059, 0.721569, 0.686275, 0.392157 );
                ColorA cpo = ColorA( 0.498039, 0.780392, 0.686275, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_MINGREEN:
            {
                ColorA cb = ColorA( 1, 1, 1, 0.294118 );
                ColorA co = ColorA( 0.94902, 0.901961, 0.760784, 0.392157 );
                ColorA coh = ColorA( 1, 1, 1, 0.784314 );
                ColorA cf = ColorA( 0.435294, 0.74902, 0.635294, 0.784314 );
                ColorA cfh = ColorA( 0.74902, 0.721569, 0.682353, 1 );
                ColorA cp = ColorA( 0.94902, 0.780392, 0.466667, 0.392157 );
                ColorA cpo = ColorA( 0.94902, 0.901961, 0.760784, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_HELLOYELLOW:
            {
                ColorA cb = ColorA( 1, 0.827451, 0, 0.294118 );
                ColorA co = ColorA( 0.290196, 0.729412, 0.690196, 0.392157 );
                ColorA coh = ColorA( 0.596078, 0.129412, 0, 0.784314 );
                ColorA cf = ColorA( 1, 0.827451, 0, 0.784314 );
                ColorA cfh = ColorA( 1, 0.960784, 0.619608, 1 );
                ColorA cp = ColorA( 0.290196, 0.729412, 0.690196, 0.392157 );
                ColorA cpo = ColorA( 0.596078, 0.129412, 0, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_TEALTEAL:
            {
                ColorA cb = ColorA( 0.290196, 0.729412, 0.690196, 0.294118 );
                ColorA co = ColorA( 1, 0.827451, 0, 0.392157 );
                ColorA coh = ColorA( 1, 0.960784, 0.619608, 0.784314 );
                ColorA cf = ColorA( 0.290196, 0.729412, 0.690196, 0.784314 );
                ColorA cfh = ColorA( 0.596078, 0.129412, 0, 1 );
                ColorA cp = ColorA( 1, 0.827451, 0, 0.392157 );
                ColorA cpo = ColorA( 1, 0.960784, 0.619608, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_RUSTICORANGE:
            {
                ColorA cb = ColorA( 0.419608, 0.333333, 0.188235, 0.294118 );
                ColorA co = ColorA( 0.192157, 0.188235, 0.258824, 0.392157 );
                ColorA coh = ColorA( 0.419608, 0.333333, 0.188235, 0.784314 );
                ColorA cf = ColorA( 1, 0.427451, 0.141176, 0.784314 );
                ColorA cfh = ColorA( 1, 0.921569, 0.419608, 1 );
                ColorA cp = ColorA( 0.164706, 0.529412, 0.196078, 0.392157 );
                ColorA cpo = ColorA( 0.192157, 0.188235, 0.258824, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_TEALSALMON:
            {
                ColorA cb = ColorA( 0.305882, 0.521569, 0.533333, 0.294118 );
                ColorA co = ColorA( 0.219608, 0.270588, 0.231373, 0.392157 );
                ColorA coh = ColorA( 0.305882, 0.521569, 0.533333, 0.784314 );
                ColorA cf = ColorA( 1, 0.27451, 0.329412, 0.784314 );
                ColorA cfh = ColorA( 1, 0.835294, 0.415686, 1 );
                ColorA cp = ColorA( 1, 0.996078, 0.827451, 0.392157 );
                ColorA cpo = ColorA( 0.219608, 0.270588, 0.231373, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_CITRUSBLUE:
            {
                ColorA cb = ColorA( 0.223529, 0.556863, 0.713725, 0.294118 );
                ColorA co = ColorA( 0.133333, 0.407843, 0.533333, 0.392157 );
                ColorA coh = ColorA( 0.223529, 0.556863, 0.713725, 0.784314 );
                ColorA cf = ColorA( 1, 0.635294, 0, 0.784314 );
                ColorA cfh = ColorA( 1, 0.839216, 0, 1 );
                ColorA cp = ColorA( 1, 0.960784, 0, 0.392157 );
                ColorA cpo = ColorA( 0.133333, 0.407843, 0.533333, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_LIMEPURPLE:
            {
                ColorA cb = ColorA( 0.341176, 0.211765, 1, 0.294118 );
                ColorA co = ColorA( 0.14902, 0.14902, 0.14902, 0.392157 );
                ColorA coh = ColorA( 0.341176, 0.211765, 1, 0.784314 );
                ColorA cf = ColorA( 0.905882, 1, 0.211765, 0.784314 );
                ColorA cfh = ColorA( 1, 0.211765, 0.435294, 1 );
                ColorA cp = ColorA( 0.137255, 0.454902, 0.870588, 0.392157 );
                ColorA cpo = ColorA( 0.14902, 0.14902, 0.14902, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_LIMESTONE2:
            {
                ColorA cb = ColorA( 0.396078, 0.384314, 0.45098, 0.294118 );
                ColorA co = ColorA( 0.34902, 0.729412, 0.662745, 0.392157 );
                ColorA coh = ColorA( 0.396078, 0.384314, 0.45098, 0.784314 );
                ColorA cf = ColorA( 0.847059, 0.945098, 0.443137, 0.784314 );
                ColorA cfh = ColorA( 0.988235, 1, 0.85098, 1 );
                ColorA cp = ColorA( 0.25098, 0.0705882, 0.172549, 0.392157 );
                ColorA cpo = ColorA( 0.34902, 0.729412, 0.662745, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                                
            case CI_UI_THEME_COOLPURPLE:
            {
                ColorA cb = ColorA( 0.14902, 0.537255, 0.913725, 0.294118 );
                ColorA co = ColorA( 0.0431373, 0.964706, 0.576471, 0.392157 );
                ColorA coh = ColorA( 0.14902, 0.537255, 0.913725, 0.784314 );
                ColorA cf = ColorA( 0.913725, 0.101961, 0.615686, 0.784314 );
                ColorA cfh = ColorA( 0.964706, 0.713725, 0.0431373, 1 );
                ColorA cp = ColorA( 0.964706, 0.94902, 0.0431373, 0.392157 );
                ColorA cpo = ColorA( 0.0431373, 0.964706, 0.576471, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_GRAYRED:
            {
                ColorA cb = ColorA( 0.160784, 0.133333, 0.121569, 0.294118 );
                ColorA co = ColorA( 0.0745098, 0.454902, 0.490196, 0.392157 );
                ColorA coh = ColorA( 0.160784, 0.133333, 0.121569, 0.784314 );
                ColorA cf = ColorA( 0.988235, 0.207843, 0.298039, 0.784314 );
                ColorA cfh = ColorA( 0.988235, 0.968627, 0.772549, 1 );
                ColorA cp = ColorA( 0.0392157, 0.74902, 0.737255, 0.392157 );
                ColorA cpo = ColorA( 0.0745098, 0.454902, 0.490196, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_METALGEAR2:
            {
                ColorA cb = ColorA( 0.803922, 0.741176, 0.682353, 0.294118 );
                ColorA co = ColorA( 0.478431, 0.356863, 0.243137, 0.392157 );
                ColorA coh = ColorA( 0.803922, 0.741176, 0.682353, 0.784314 );
                ColorA cf = ColorA( 0.980392, 0.294118, 0, 0.784314 );
                ColorA cfh = ColorA( 0.980392, 0.980392, 0.980392, 1 );
                ColorA cp = ColorA( 0.121569, 0.121569, 0.121569, 0.392157 );
                ColorA cpo = ColorA( 0.478431, 0.356863, 0.243137, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_LIGHTPINK:
            {
                ColorA cb = ColorA( 0.619608, 0.117647, 0.298039, 0.294118 );
                ColorA co = ColorA( 0.560784, 0.560784, 0.560784, 0.392157 );
                ColorA coh = ColorA( 0.619608, 0.117647, 0.298039, 0.784314 );
                ColorA cf = ColorA( 0.92549, 0.92549, 0.92549, 0.784314 );
                ColorA cfh = ColorA( 1, 0.0666667, 0.407843, 1 );
                ColorA cp = ColorA( 0.145098, 0.00784314, 0.0588235, 0.392157 );
                ColorA cpo = ColorA( 0.560784, 0.560784, 0.560784, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_MINPINK2:
            {
                ColorA cb = ColorA( 0.92549, 0.92549, 0.92549, 0.294118 );
                ColorA co = ColorA( 0.619608, 0.117647, 0.298039, 0.392157 );
                ColorA coh = ColorA( 0.92549, 0.92549, 0.92549, 0.784314 );
                ColorA cf = ColorA( 1, 0.0666667, 0.407843, 0.784314 );
                ColorA cfh = ColorA( 0.145098, 0.00784314, 0.0588235, 1 );
                ColorA cp = ColorA( 0.560784, 0.560784, 0.560784, 0.392157 );
                ColorA cpo = ColorA( 0.619608, 0.117647, 0.298039, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_MAXPINK:
            {
                ColorA cb = ColorA( 1, 0.0784314, 0.341176, 0.294118 );
                ColorA co = ColorA( 0.0392157, 0.0392157, 0.0392157, 0.392157 );
                ColorA coh = ColorA( 0.890196, 0.964706, 1, 0.784314 );
                ColorA cf = ColorA( 1, 0.0784314, 0.341176, 0.784314 );
                ColorA cfh = ColorA( 1, 0.847059, 0.490196, 1 );
                ColorA cp = ColorA( 0.0392157, 0.0392157, 0.0392157, 0.392157 );
                ColorA cpo = ColorA( 0.890196, 0.964706, 1, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            case CI_UI_THEME_MINYELLOW:
            {
                ColorA cb = ColorA( 0.898039, 0.894118, 0.854902, 0.294118 );
                ColorA co = ColorA( 0.847059, 0.823529, 0.6, 0.392157 );
                ColorA coh = ColorA( 0.898039, 0.894118, 0.854902, 0.784314 );
                ColorA cf = ColorA( 0.960784, 0.878431, 0.219608, 0.784314 );
                ColorA cfh = ColorA( 0.0901961, 0.0862745, 0.360784, 1 );
                ColorA cp = ColorA( 0.745098, 0.74902, 0.619608, 0.392157 );
                ColorA cpo = ColorA( 0.847059, 0.823529, 0.6, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_MINLIME:
            {
                ColorA cb = ColorA( 0.960784, 0.882353, 0.886275, 0.294118 );
                ColorA co = ColorA( 0.882353, 0.717647, 0.929412, 0.392157 );
                ColorA coh = ColorA( 0.960784, 0.882353, 0.886275, 0.784314 );
                ColorA cf = ColorA( 0.72549, 0.870588, 0.317647, 0.784314 );
                ColorA cfh = ColorA( 0.819608, 0.890196, 0.537255, 1 );
                ColorA cp = ColorA( 0.878431, 0.282353, 0.568627, 0.392157 );
                ColorA cpo = ColorA( 0.882353, 0.717647, 0.929412, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_MINORANGE:
            {
                ColorA cb = ColorA( 0.8, 0.8, 0.8, 0.294118 );
                ColorA co = ColorA( 0.435294, 0.435294, 0.435294, 0.392157 );
                ColorA coh = ColorA( 0.8, 0.8, 0.8, 0.784314 );
                ColorA cf = ColorA( 1, 0.392157, 0, 0.784314 );
                ColorA cfh = ColorA( 1, 1, 1, 1 );
                ColorA cp = ColorA( 0.2, 0.2, 0.2, 0.392157 );
                ColorA cpo = ColorA( 0.435294, 0.435294, 0.435294, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_GRAYDAY:
            {
                ColorA cb = ColorA( 0.694118, 0.776471, 0.8, 0.294118 );
                ColorA co = ColorA( 1, 1, 1, 0.392157 );
                ColorA coh = ColorA( 0.0784314, 0.0784314, 0.0784314, 0.784314 );
                ColorA cf = ColorA( 0.694118, 0.776471, 0.8, 0.784314 );
                ColorA cfh = ColorA( 1, 0.937255, 0.368627, 1 );
                ColorA cp = ColorA( 1, 1, 1, 0.392157 );
                ColorA cpo = ColorA( 0.0784314, 0.0784314, 0.0784314, 0.784314 );
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  

            case CI_UI_THEME_MINBLACK:
            {
                ColorA cb = ColorA( 1, 1, 1, 0.294118 );
                ColorA co = ColorA( 0.819608, 0.905882, 0.317647, 0.392157 );
                ColorA coh = ColorA( 1, 1, 1, 0.784314 );
                ColorA cf = ColorA( 0, 0, 0, 0.784314 );
                ColorA cfh = ColorA( 0.14902, 0.678431, 0.894118, 1 );
                ColorA cp = ColorA( 0.301961, 0.737255, 0.913725, 0.392157 );
                ColorA cpo = ColorA( 0.819608, 0.905882, 0.317647, 0.784314 );                                
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break;  
                
            default:
            {
                ColorA cb = CI_UI_COLOR_BACK; 
                ColorA co = CI_UI_COLOR_OUTLINE; 
                ColorA coh = CI_UI_COLOR_OUTLINE_HIGHLIGHT;
                ColorA cf = CI_UI_COLOR_FILL; 
                ColorA cfh = CI_UI_COLOR_FILL_HIGHLIGHT;
                ColorA cp = CI_UI_COLOR_PADDED;
                ColorA cpo = CI_UI_COLOR_PADDED_OUTLINE;
                setUIColors( cb, co, coh, cf, cfh, cp, cpo );                                 
            }
                break; 
        }
    }
    
    
	void setWidgetColor(int _target, ColorA _color)
	{
		switch (_target) 
		{
			case CI_UI_WIDGET_COLOR_BACK:
				for(int i = 0; i < widgets.size(); i++)
				{
					widgets[i]->setColorBack(_color); 
				}				
				break;

			case CI_UI_WIDGET_COLOR_OUTLINE:
				for(int i = 0; i < widgets.size(); i++)
				{
					widgets[i]->setColorOutline(_color); 
				}				
				break;
			
			case CI_UI_WIDGET_COLOR_OUTLINE_HIGHLIGHT:
				for(int i = 0; i < widgets.size(); i++)
				{
					widgets[i]->setColorOutlineHighlight(_color); 
				}				
				break;
			
			case CI_UI_WIDGET_COLOR_FILL:
				for(int i = 0; i < widgets.size(); i++)
				{
					widgets[i]->setColorFill(_color); 
				}				
				break;
			
			case CI_UI_WIDGET_COLOR_FILL_HIGHLIGHT:
				for(int i = 0; i < widgets.size(); i++)
				{
					widgets[i]->setColorFillHighlight(_color); 
				}					
				break;
                
			case CI_UI_WIDGET_COLOR_PADDED:
				for(int i = 0; i < widgets.size(); i++)
				{
					widgets[i]->setColorPadded(_color); 
				}					
				break;
                
			case CI_UI_WIDGET_COLOR_PADDED_OUTLINE:
				for(int i = 0; i < widgets.size(); i++)
				{
					widgets[i]->setColorPaddedOutline(_color); 
				}					
				break;                
				
			default:
				break;
		}
	}
    
    ciUIWidget *getWidget(string _name)
	{
		return widgets_map[_name]; 
	}
	
    void removeWidget(string _name)    
    {
        ciUIWidget *toDelete = getWidget(_name);
        if(toDelete != NULL)
        {
            removeWidget(toDelete);
        }    
    }
    
	void setDrawPadding(bool _draw_padded_rect)
	{
		draw_padded_rect = _draw_padded_rect; 
	}

    void setDrawWidgetPadding(bool _draw_padded_rect)
    {
		for(int i = 0; i < widgets.size(); i++)
		{
			widgets[i]->setDrawPadding(_draw_padded_rect); 
		}		        
    }

	void setDrawPaddingOutline(bool _draw_padded_rect_outline)
	{
		draw_padded_rect_outline = _draw_padded_rect_outline; 
	}

    void setDrawWidgetPaddingOutline(bool _draw_padded_rect_outline)
	{
		for(int i = 0; i < widgets.size(); i++)
		{
			widgets[i]->setDrawPaddingOutline(_draw_padded_rect_outline); 
		}		
	}

    
    vector<ciUIWidget*> getWidgets()
    {
        return widgets;
    }
    
    vector<ciUIWidget*> getWidgetsOfType(ciUIWidgetType type)
    {
        vector<ciUIWidget*> widgetToReturn; 
        for(int i = 0; i < widgets.size(); i++)
		{
            if(widgets[i]->getKind() == type)
            {
                widgetToReturn.push_back(widgets[i]);
            }
		}	 
        return widgetToReturn;                                         
    }

    template<typename T> ci::CallbackId registerUIEvents( T *obj, void (T::*callback)( ciUIEvent *event ) )
    {
        return uiEventCallbackMgr.registerCb( std::bind1st( std::mem_fun( callback ), obj ) );
    }
    
    void unregisterUIEvents( ci::CallbackId id ) 
    {
        uiEventCallbackMgr.unregisterCb( id ); 
    }

	
protected:    
    
    void pushbackWidget(ciUIWidget *widget)
    {
        widget->setID(uniqueIDs); 
        uniqueIDs++;
        widgets.push_back(widget);    
		widgets_map[widget->getName()] = widget;                             
    }
    
    ci::CallbackMgr<void(ciUIEvent*)> uiEventCallbackMgr;
    
    
#if defined( CINDER_COCOA_TOUCH )
    app::AppCocoaTouch *mApp;
    ci::CallbackId mCbTouchesBegan, mCbTouchesMoved, mCbTouchesEnded; 
#else
    app::App *mApp;    	
    ci::CallbackId mCbMouseDown, mCbMouseDrag, mCbMouseUp, mCbMouseMove;
    ci::CallbackId mCbKeyDown, mCbKeyUp;
#endif 
    
    gl::TextureFontRef font_large; 	
    gl::TextureFontRef font_medium; 		
    gl::TextureFontRef font_small;
 	
    Font fontLarge; 
    Font fontMedium; 
    Font fontSmall;     
    
    
	ciUIEvent *GUIevent; 
    int state; 
    bool hasSharedResources;
    
    map<string, ciUIWidget*> widgets_map;     
	vector<ciUIWidget*> widgets; 
	vector<ciUIWidget*> widgetsWithState;     
	ciUIWidget *lastAdded; 
	ciUIWidget *activeFocusedWidget; 
	bool enable_highlight_outline; 
	bool enable_highlight_fill; 	
	bool enabled; 
    int uniqueIDs; 
    bool hasKeyBoard; 
    
    float widgetSpacing; 
    
    string fontName;

    bool updateFont(ciUIWidgetFontType _kind, string filename, int fontsize) 
    {
        bool success = true;
        switch(_kind)
        {
            case CI_UI_FONT_LARGE:              
                fontLarge = Font( loadResource(filename), fontsize);                
                font_large = gl::TextureFont::create(fontLarge);
                break; 
                
            case CI_UI_FONT_MEDIUM:
                fontMedium = Font( loadResource(filename), fontsize);                
                font_medium = gl::TextureFont::create(fontMedium);
                
                break; 
                
            case CI_UI_FONT_SMALL:
                fontSmall = Font( loadResource(filename), fontsize);                
                font_small = gl::TextureFont::create(fontSmall);

                break; 

        }
        return success;
    }
    
    void checkForKeyFocus(ciUIWidget *child)
    {
        if(child->getKind() == CI_UI_WIDGET_TEXTINPUT)
        {
            ciUITextInput *textinput = (ciUITextInput *) child;         
            switch(textinput->getTriggerType())
            {
                case CI_UI_TEXTINPUT_ON_FOCUS:
                {
                    cout << "has focus" << endl; 
                    hasKeyBoard = true;         
                }
                    break; 
                    
                case CI_UI_TEXTINPUT_ON_ENTER:
                {
                    cout << "has focus" << endl;                     
                    hasKeyBoard = true; 
                }        
                    break; 
                    
                case CI_UI_TEXTINPUT_ON_UNFOCUS:
                {
                    cout << "lost focus" << endl;  
                    hasKeyBoard = false; 
                }        
                    break; 
                    
                default:
                {
                    hasKeyBoard = false; 
                }
                    break;                     
            }
        }
    }
    
};

#endif

