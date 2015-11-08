#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofHideCursor();
    _video.setDeviceID(1);
    _video.setDesiredFrameRate(60);
    _video.initGrabber(ofGetWidth(), ofGetHeight());
    _currentFilter = 0;
    
    
    ofTrueTypeFont::setGlobalDpi(72);
    
    synth.load("camerashutter.wav");
    synth.setVolume(0.75f);
    
    contdown.load("verdana.ttf", 600, true, true);
    frames.resize(15);
    for (int i = 0; i < 15; ++i){
        frames[i].load("frames/"+ofToString(i + 1) + ".png");
    }
    frameIndex=0;
    
    timer=0;
    timingContdown=0;
    time=0;
    
    m_shoot = false;
    
    m_fbo.allocate(720,ofGetHeight(),GL_RGB);
    m_fbo.begin();
    ofClear(255,255,255, 0);
    m_fbo.end();
    
    m_fboFrame.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
    m_fboFrame.begin();
    ofClear(255,255,255, 0);
    m_fboFrame.end();
    
    pix.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);

    frameSize.set(600,ofGetHeight());
    
    // Basic filter examples
    //_filters.push_back(new HalftoneFilter(_video.getWidth(), _video.getHeight(), 0.01));
    _filters.push_back(new CrosshatchFilter(_video.getWidth(), _video.getHeight()));
    _filters.push_back(new KuwaharaFilter(6));
    _filters.push_back(new SobelEdgeDetectionFilter(_video.getWidth(), _video.getHeight()));
    _filters.push_back(new BilateralFilter(_video.getWidth(), _video.getHeight()));
    _filters.push_back(new SketchFilter(_video.getWidth(), _video.getHeight()));
    _filters.push_back(new DilationFilter(_video.getWidth(), _video.getHeight()));
    _filters.push_back(new XYDerivativeFilter(_video.getWidth(), _video.getHeight()));
    _filters.push_back(new ZoomBlurFilter());
    _filters.push_back(new SmoothToonFilter(_video.getWidth(), _video.getHeight()));
    _filters.push_back(new CGAColorspaceFilter());
    _filters.push_back(new ErosionFilter(_video.getWidth(), _video.getHeight()));
    _filters.push_back(new LookupFilter(_video.getWidth(), _video.getHeight(), "img/lookup_amatorka.png"));
    _filters.push_back(new LookupFilter(_video.getWidth(), _video.getHeight(), "img/lookup_miss_etikate.png"));
    _filters.push_back(new LookupFilter(_video.getWidth(), _video.getHeight(), "img/lookup_soft_elegance_1.png"));
    _filters.push_back(new PosterizeFilter(8));
    _filters.push_back(new LaplacianFilter(_video.getWidth(), _video.getHeight(), ofVec2f(1, 1)));
    _filters.push_back(new PixelateFilter(_video.getWidth(), _video.getHeight()));

    
    // blending examples
    
    ofImage wes = ofImage("img/wes.jpg");

    _filters.push_back(new ExclusionBlendFilter(wes.getTexture()));
    
    // here's another unimaginative filter chain
    
    FilterChain * watercolorChain = new FilterChain(_video.getWidth(), _video.getHeight(), "Monet");
    watercolorChain->addFilter(new KuwaharaFilter(9));
    watercolorChain->addFilter(new LookupFilter(_video.getWidth(), _video.getHeight(), "img/lookup_miss_etikate.png"));
    watercolorChain->addFilter(new BilateralFilter(_video.getWidth(), _video.getHeight()));
    watercolorChain->addFilter(new PoissonBlendFilter("img/canvas_texture.jpg", _video.getWidth(), _video.getHeight(), 2.0));

    _filters.push_back(watercolorChain);
    
    
    
    
    // getting printer list
    vector<string> printerList = printer.getPrinterList();
    int i;
    cout << "--- Printer List" << endl;
    for (i = 0; i < printerList.size(); i++) {
        cout << printerList[i] << endl;
    }
    cout << "--- End Printer List" << endl;
    
    
    // Get default printer
    string defaultPrinterName = printer.getDefaultPrinterName();
    cout << "Default Printer: " << defaultPrinterName << endl;
    
    
    // set printer name which you want to use...
    printer.setPrinterName(defaultPrinterName);
    
    
    // set print options....... see also http://www.cups.org/documentation.php/doc-1.5/options.html
    // or, set printer default option from http://localhost:631/printers/
    printer.addOption("media", "A5");
    printer.addOption("resolution", "300dpi"); // one particularly useful option to know about.
    
    
    // You can also use parseOptions to set printer options as with a command line interface
    // http://www.cups.org/documentation.php/options.html
    //	 printer.parseOptions("fit-to-page media=A4,Tray1");
    
    
    // if necessary.......
    printer.setJobTitle("stampfie");
    
    
    setupArduino();
}


//--------------------------------------------------------------
void ofApp::setupArduino() {
    
    // this should be set to whatever com port your serial device is connected to.
    // (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
    // arduino users check in arduino app....
    int baud = 9600;
    serial.setup("/dev/cu.usbmodem1421", baud); //open the first device
    //serial.setup("COM4", baud); // windows example
    //serial.setup("/dev/tty.usbserial-A4001JEC", baud); // mac osx example
    //serial.setup("/dev/ttyUSB0", baud); //linux example
    
    nTimesRead = 0;
    nBytesRead = 0;
    readTime = 0;
    memset(bytesReadString, 0, 4);

}

//--------------------------------------------------------------
void ofApp::update(){
    _video.update();
    
    updateArduino();
    
    printer.updatePrinterInfo();
    
    timer=ofGetElapsedTimeMillis();
    
    time = 9-int(((timer- timingContdown)/1000));
    
    if (time<6 && m_shoot==true){
        path="images/image-" +ofGetTimestampString() +".jpg";
        m_fbo.readToPixels(pix);
        ofSaveImage(pix,path);
        m_imageOutput.load(path);
        m_imageOutput.mirror(false,true);
        printer.printImage(path);
        m_shoot=false;
    }
    else if(time==0){
        _currentFilter = ofRandom(0, _filters.size());
        frameIndex=ofRandom(0, 15);
    }
    pix.clear();
}

//--------------------------------------------------------------
void ofApp::updateArduino(){
    

        
        
        // (2) read
        // now we try to read 3 bytes
        // since we might not get them all the time 3 - but sometimes 0, 6, or something else,
        // we will try to read three bytes, as much as we can
        // otherwise, we may have a "lag" if we don't read fast enough
        // or just read three every time. now, we will be sure to
        // read as much as we can in groups of three...

        nTimesRead = 0;
        nBytesRead = 0;
        int nRead  = 0;  // a temp variable to keep count per read
        
        unsigned char bytesReturned[3];
        
        memset(bytesReadString, 0, 4);
        memset(bytesReturned, 0, 3);
        
        while( (nRead = serial.readBytes( bytesReturned, 3)) > 0){
            nTimesRead++;
            nBytesRead = nRead;
        };
        
        memcpy(bytesReadString, bytesReturned, 3);
        
        readTime = ofGetElapsedTimef();
    
    if (nBytesRead > 0 && ((ofGetElapsedTimef() - readTime) < 0.5f) && m_shoot==false){
        m_shoot=true;
        timingContdown=ofGetElapsedTimeMillis();
    }
    
}


//--------------------------------------------------------------
void ofApp::draw(){
    m_fbo.begin();
    ofClear(0,0,0,0);
    ofBackground(0, 0, 0);
    ofSetColor(255);

    ofPushMatrix();
    ofScale(-1, 1);
    ofTranslate(-_video.getWidth()+280, 0);
    _filters[_currentFilter]->begin();
    
    if(int(time<6 && time>0)){
        if(m_imageOutput.isAllocated()){
        m_imageOutput.draw(280,0);
        }
    }else{
        _video.draw(0,0);
    }
    _filters[_currentFilter]->end();
    ofPopMatrix();
    frames[frameIndex].draw(ofGetWidth()*0.5 - 640,0, 720,ofGetHeight());
    if(time<=9 && time>6){
        contdown.drawString(ofToString(time-6), ofGetWidth()*0.5-175-280, ofGetHeight()*0.5+200);
    }
    else if((9-(timer- timingContdown)/1000)>5.7 && (9-(timer- timingContdown)/1000)<5.9){
        ofFill();
        ofDrawRectangle(-280, 0, ofGetWidth(), ofGetHeight());
        synth.play();
    }
    //    ofDrawBitmapString( _filters[_currentFilter]->getName() + " Filter\n(press SPACE to change filters)", ofPoint(40, 20));
    m_fbo.end();

    m_fboFrame.begin();
    ofSetColor(0);
    ofFill();
    ofDrawRectangle(0,0,ofGetWidth(),ofGetHeight());
    m_fboFrame.end();
    

    m_fboFrame.draw(0,0);
    m_fbo.draw(ofGetWidth()*0.5-360,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'c' && m_shoot==false)
    {
        m_shoot=true;
        timingContdown=ofGetElapsedTimeMillis();
    }
    
    /*if (buttonState=="0" && m_shoot==false)
    {
        m_shoot=true;
        timingContdown=ofGetElapsedTimeMillis();
    }*/
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
