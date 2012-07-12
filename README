ciUI is an Block for Cinder (version 0.8.4) that easily allows for the creation of user interfaces aka GUIs. ciUI also takes care of widget layout, spacing, font loading, saving and loading settings, and callbacks. ciUI can be easily customized (colors, font & widget sizes, padding, layout, etc).

ciUI contains a collection of minimally designed graphical user interface (GUI) widgets:

Widgets in ciUI:

- Buttons & Toggles (multi-image & single image buttons, label buttons, toggle matrix, radio toggles, Drop Down List)
- Sliders (multi-image, minimal, bilabel, FPS, circle, rotary, range, vertical, horizontal)
- Number Dials
- 2D pads
- Labels, FPS labels
- Text Input Area
- Waveform & Spectrum Graphs
- Moving Plot/Graph
- Image Display, Image Color Sampler

This library allows for rapid UI design and development. Widget layout is semi-automatic, and can be easily customized. ciUI is a GL based GUI and uses Cinder's drawings calls to render its widgets. It integrates into Cinder projects very easily since it follow’s Cinder’s Block ideology. There are many examples included in the download that show how to add widgets, customize their placement, get values from different types of widgets, set widget values, add callback functions for gui events, saving and loading settings, and more.

This UI library was inspired & influenced by: 

- controlP5 (http://www.sojamo.de/libraries/controlP5/) 
- GLV (http://mat.ucsb.edu/glv/)
- SimpleGUI (http://marcinignac.com/blog/simplegui/)
- Apple's User Interface 

ciUI was / is developed by Reza Ali (www.syedrezaali.com || syed.reza.ali@gmail.com || @rezaali). It has been tested on OSX  and iOS. It should work on windows, but I haven't tested it yet. ciUI is open source under an MIT License, therefore you can use it in commercial and non-commercial projects. If you do end up using it, please let me know. I would love to see examples of it out there in the wild. If you plan to use it in a commercial project please consider donating to help support this Block and future releases/fixes/features (http://www.syedrezaali.com/blog/?p=2366).

In terms of performance, I haven't compared it to other GUIs out there, but since its written using Cinder drawing calls and uses Boost for its callbacks, it runs fast (on my laptop (Apple Macbook Pro 2009)).

TUTORIAL: 

This tutorial will provide step by step instructions on how to integrate ciUI with a project your are working on, for simplicity I will assume we are starting from an empty project. For this we are going to be assuming you are using Cinder 0.8.4 for OSX, however these instructions should be easily adaptable for iOS.

1. After downloading ciUI, place it in your Cinder Blocks folder. 

2. Create a new project that is an example of the Cinder emptyExample (found in the apps/examples folder in OF).

3. Open the project in xCode. 

4. Drag the src (Blocks/ciUI/src) folder within ciUI into the Blocks folder in the xCode project (located on the left side of OF). You can rename this ciUI to make things neat. 

5. When prompted "Choose options for adding these files", just press "finish." Then add ofxXmlSettings to the Blocks folder inside your xCode project. ciUI uses this Block to save and load settings from XML files. 

6. Now we must import some resources that ciUI will use into the data folder of the project your are working on. The easiest way is to go to one the examples in the ciUI Block folder and copy the bin/data/GUI folder into your project's data folder "bin/data/".

Note: The last step is critical so ciUI can find the proper resources for rendering font. 

7. Now go back to xCode and include "ciUI.h" in your testApp.h file.

8. Then in your testApp.h file, create a new ciUICanvas object by typing "ciUICanvas *gui;" within the testApp class. In addition create two functions:

void exit(); 
void guiEvent(ciUIEventArgs &e);

9. Switch over to your testApp.cpp file and define the exit and guiEvent functions: 

void testApp::exit()
{
	
}

void testApp::guiEvent(ciUIEventArgs &e)
{
	
}

10. Within the setup function we are going to initialize the gui object and add widgets to it. So one way to do that is: 

gui = new ciUICanvas(0,0,320,320);		//ciUICanvas(float x, float y, float width, float height)		

Note: The arguments define the GUI's top left corner position and width and height. If no arguments are passed then the GUI will position itself on top of the window and the back of the GUI won't be drawn. 

11. In the exit function we have to delete the gui after we are done using the application. But before this we want to tell the GUI to save the current values in the widgets to an XML file. Thus your exit function should look like: 

void testApp::exit()
{
    gui->saveSettings("GUI/guiSettings.xml");     
    delete gui; 
}

12. We are now going to add widgets to the GUI: 

gui->addWidgetDown(new ciUILabel("ciUI TUTORIAL", OFX_UI_FONT_LARGE)); 
gui->addWidgetDown(new ciUISlider(304,16,0.0,255.0,100.0,"BACKGROUND VALUE")); 
ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent); 
gui->loadSettings("GUI/guiSettings.xml"); 

Note: The second to last line adds a listener/callback, so the gui knows what function to call once a widget is triggered or interacted with by the user, don't worry if its doesn't make too much sense right now, you'll get the hang of it. The last line tells the gui to load settings (widget values from a saved XML file, if the file isn't present it uses the default value of the widgets). 

13. Now to the guiEvent function, we need to react to the user input. The argument of the guiEvent function, ciUIEventArgs &e, contains the widget which was modified. To access the widget we do the following:

void testApp::guiEvent(ciUIEventArgs &e)
{
    if(e.widget->getName() == "BACKGROUND VALUE")	
    {
        ciUISlider *slider = (ciUISlider *) e.widget;    
        ofBackground(slider->getScaledValue());
    }   
}

Note: The if statement checks to see which widget was triggered. The way it does that is a string comparison. If the widget is the "BACKGROUND VALUE" slider widget, then case the widget to a slider and retrieve its value by the "getScaledValue()" function. 

Note: A more practical example might save the value from the slider and use it else where the program. 

14. Lets add a Toggle to toggle the window between fullscreen and window mode. In the setup function add another widget after the other widgets: 

gui->addWidgetDown(new ciUIToggle(32, 32, false, "FULLSCREEN"));

Note: if we placed this function before the other addWidgetDown calls then the Toggle would be placed above the slider and able. Lets keep things neat and put in under the slider. 

15. We have to now respond to the "FULLSCREEN" toggle widget so we add more functionality to our guiEvent function. In the end it should look like:

void testApp::guiEvent(ciUIEventArgs &e)
{
    if(e.widget->getName() == "BACKGROUND VALUE")
    {
        ciUISlider *slider = (ciUISlider *) e.widget;    
        ofBackground(slider->getScaledValue());
    }
    else if(e.widget->getName() == "FULLSCREEN")
    {
        ciUIToggle *toggle = (ciUIToggle *) e.widget;
        ofSetFullscreen(toggle->getValue()); 
    }    
}

Notes: So you can see adding other kinds of widgets and reacting to them are done in a very similar manner. Explore the examples to check out how to access some of the more complex widgets, I hope you'll see thats its pretty intuitive. I tried my best to limit the amount of code that needs to be written, however I kept it open enough so people can be expressive with it. 

If you lost your way somewhere in the tutorial, don't worry the whole project is included in the examples in the ciUI Block folder!