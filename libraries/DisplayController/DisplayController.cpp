#include "DisplayController.h"

//window types:
// display data
// or menus
DisplayController::DisplayController(KWH018ST01_4WSPI tft, HardwareSerial &serial) : myTFT(tft), _serial(&serial) {


    menuWindows = new Window[MAX_MENU_WINDOWS];
    dataWindows = new DataWindow[MAX_DATA_WINDOWS];

    // menuWindows = new wind_info_t[9];
    // currentWindowCount = 0;
    // menuCols = 0;
    // menuRows = 0;
    // currentSelectedWindow = 0;
    // lastSelectedWindow = 0;


    RED = myTFT.rgbTo18b(255, 0, 0);
    GREEN = myTFT.rgbTo18b(0, 255, 0);
    BLUE = myTFT.rgbTo18b(0, 0, 255);
    WHITE = myTFT.rgbTo18b(255, 255, 255);
    ORANGE = myTFT.rgbTo18b(255, 128, 0);
    defaultColor = myTFT.rgbTo18b(0, 0, 0);

    // menu windows
    for(int i = 0; i < MAX_MENU_WINDOWS; i++) {
        Window wind;
        wind_info_t pw;
        myTFT.setWindowDefaults(&pw);
        wind.wind = pw;
        menuWindows[i] = wind;
    }

    // log windows
    // float dX = XMAX * 1.0 / 2;
    
    float dY = YMAX * 1.0 / MAX_DATA_WINDOWS;
    for(int i = 0; i < MAX_DATA_WINDOWS; i++) {
        DataWindow d;
        // "Compass"  |  123deg
        //    w             wd
        // wind_info_t w;
        wind_info_t wd;

        // myTFT.setWindowDefaults(&w);
        myTFT.setWindowDefaults(&wd);

        wd.xMin = dataSeparatorX + 1;
        wd.xMax = XMAX - 1;
        wd.yMin = (int)(dY * i);
        wd.yMax = (int)(dY * i + dY - 1);

        d.wind = wd;
        d.isDirty = 1;
        dataWindows[i] = d;
    }

    myTFT.setWindowDefaults(&labelsWindow);
    labelsWindow.xMin = 0;
    labelsWindow.xMax = dataSeparatorX;
    labelsWindow.yMin = 0;
    labelsWindow.yMax = YMAX - 1;

    myTFT.pCurrentWindow = &labelsWindow;
    myTFT.setCurrentWindowColorSequence((color_t)&defaultColor);
    
    displayState = Menu;
}

void DisplayController::begin(uint8_t DC_PIN, uint8_t CS_PIN, uint8_t PWM_PIN, SPIClass SPI_PORT, uint32_t SPI_SPEED) {
    myTFT.begin(DC_PIN, CS_PIN, PWM_PIN, SPI_PORT, SPI_SPEED);
    myTFT.clearDisplay();

    // creating menu windows
    createWindows(2, 3);

    // menu windows
    for(int i = 0; i < MAX_MENU_WINDOWS; i++) {
        myTFT.pCurrentWindow = &(menuWindows[i].wind);
        myTFT.setCurrentWindowColorSequence((color_t)&defaultColor);
    }

    // log windows
    for(int i = 0; i < MAX_DATA_WINDOWS; i++) {
        myTFT.pCurrentWindow = &(dataWindows[i].wind);
        myTFT.setCurrentWindowColorSequence((color_t)&defaultColor);
    }
    myTFT.setCurrentWindowColorSequence((color_t)&defaultColor);
    // myTFT.setCurrentWindowMemory((color_t)screenMem, 128*160);
    
}

DisplayController& DisplayController::createWindows(int cols, int rows) {
    if(cols * rows > 9 || cols < 1 || rows < 1) {
        _serial->println("To many windows, max 9");
        return *this;
    }

    menuCols = cols;
    menuRows = rows;
    currentWindowCount = rows * cols;
    currentSelectedWindow = 0;
    lastSelectedWindow = 0;
    justSwitchedWindow = 1;

    float dX = XMAX * 1.0 / cols;
    float dY = YMAX * 1.0 / rows;

    for(int r = 0; r < rows; r++) {
        for(int c = 0; c < cols; c++) {
            wind_info_t w = menuWindows[r * cols + c].wind;
            w.xMax = (int)(dX * c + dX - 1);
            w.xMin = (int)(dX * c);
            w.yMin = (int)(dY * r);
            w.yMax = (int)(dY * r + dY - 1);
            // pointers don't work???
            menuWindows[r * cols + c].wind = w;
            menuWindows[r * cols + c].data = nullptr;
            menuWindows[r * cols + c].onClick = nullptr;
        }
    }

    return *this;
}

DisplayController& DisplayController::addOnClickToWindow(int index, Action onClick) {
    menuWindows[index].onClick = onClick;
    return *this;
}

int DisplayController::isWithinBounds(int x, int y, wind_info_t wind) {
    if(x < wind.xMin || x > wind.xMax) return 0;
    if(y < wind.yMin || y > wind.yMax) return 0;
    return 1;
}

int DisplayController::handleJoystickAxis(int x, int y) {
    // if(p != 0) {
    //     handleJoystickInputUtil(IN_CLICK);
    // }
    // else {
    if(x < 100){
        return handleJoystickInputUtil(IN_DOWN);
    }
    else if(x > 500) {
        return handleJoystickInputUtil(IN_UP);
    }
    else if(y < 100) {
        return handleJoystickInputUtil(IN_RIGHT);
    }
    else if(y > 500) {
        return handleJoystickInputUtil(IN_LEFT);
    }
    else {
        lastJoystickInput = IN_NONE;
        return 0;
    }
    // }
}

int DisplayController::handleJoystickInputUtil(JoystickInputType inputType) {
    // if(lastJoystickInput != IN_NONE) return 0;
    if(inputType == lastJoystickInput) return 0;
    
    lastJoystickInput = inputType;
    return onJoystickInput(inputType);
}

int DisplayController::onJoystickInput(JoystickInputType inputType) {
    //...
    // for(int i = currentWindowCount - 1; i >= 0; i--) {
    //     if(isWithinBounds(cursorPos.x, cursorPos.y, menuWindows[i].wind) && menuWindows[i].onClick) {
    //         menuWindows[i].onClick();
    //         return 1;
    //     }
    // }

    if(displayState != Menu) return;

    switch(inputType) {
        case IN_CLICK:
            if(currentSelectedWindow < currentWindowCount && menuWindows[currentSelectedWindow].onClick != nullptr) {
                // call the onClick method
                (*(menuWindows[currentSelectedWindow].onClick))();
                return 1;
            }

            return 0;

        case IN_DOWN:
            if(currentSelectedWindow + menuCols < currentWindowCount) {
                currentSelectedWindow += menuCols;
                // updateDisplay();
            }
            return 1;
        
        case IN_UP:
            if(currentSelectedWindow - menuCols >= 0) {
                currentSelectedWindow -= menuCols;
                // updateDisplay();
            }
            return 1;

        case IN_RIGHT:
        
            // if(currentSelectedWindow + 1 < currentWindowCount) {
            if((currentSelectedWindow + 1) % menuCols != 0) {
                currentSelectedWindow++;
                // updateDisplay();
            }
            return 1;

        case IN_LEFT:
            // if(currentSelectedWindow - 1 >= 0) {
            if((currentSelectedWindow - 1 + menuCols) % menuCols != menuCols - 1) {
                currentSelectedWindow--;
                // updateDisplay();
            }
            return 1;
        
        default:
            return 0;
    }
}


// Menu

void DisplayController::switchToMenuWindow() {
    displayState = Menu;
    justSwitchedWindow = 1;
}

// count = labels.length
void DisplayController::switchToMenuWindow(char **labels, int count) {
    menuLabelCount = count;
    for(int i = 0; i < currentWindowCount && i < count; i++) {
        menuWindows[i].data = labels[i];
    }

    switchToMenuWindow();
}


// Log

void DisplayController::switchToLogWindow() {
    displayState = Log;
    justSwitchedWindow = 1;
}

// count = labels.length
void DisplayController::switchToLogWindow(char **labels, int count) {
    dataLabels = labels;
    dataLabelCount = count;
    switchToLogWindow();
}

void DisplayController::updateLogValue(int index, char *value) {
    dataWindows[index].isDirty = 1;
    dataWindows[index].data = value;
}


// TFT printing utilities

void DisplayController::printInWindow(wind_info_t *wind, char *text, int reset) {
    if(text == nullptr) return;
    if(reset) {
        myTFT.resetTextCursor(wind);
    }

    myTFT.pCurrentWindow = wind;
    myTFT.print(text);
}

void DisplayController::printInWindowAtPosition(wind_info_t *wind, char *text, int x, int y) {
    if(text == nullptr) return;
    myTFT.pCurrentWindow = wind;
    myTFT.setTextCursor(x, y, wind);
    myTFT.print(text);
}

void DisplayController::printInCurrentWindowAtPosition(char *text, int x, int y) {
    printInWindowAtPosition(myTFT.pCurrentWindow, text, x, y);
}



void DisplayController::updateDisplay() {
    // _serial->println("update");
    // Draw cursor and erase last cursor position
    // myTFT.pixel(cursorPos.x, cursorPos.y, (color_t)&RED);
    // myTFT.pixel(lastCursorPos.x, lastCursorPos.y, (color_t)&defaultColor);
    // lastCursorPos = cursorPos;  
    // hyperdisplayDefaultWindow 

    switch(displayState) {
        case Menu:
            if(justSwitchedWindow) {
                justSwitchedWindow = 0;
                myTFT.clearDisplay();
                
                // redraw all menu windows
                for(int i = 0; i < currentWindowCount; i++) {
                    myTFT.pCurrentWindow = &(menuWindows[i].wind);
                    int x = myTFT.pCurrentWindow->xMax - myTFT.pCurrentWindow->xMin;
                    int y = myTFT.pCurrentWindow->yMax - myTFT.pCurrentWindow->yMin;

                    // red window if selected
                    myTFT.setCurrentWindowColorSequence(i == currentSelectedWindow ? ((color_t)&RED) : ((color_t)&WHITE));
                    myTFT.rectangle(0, 0, x, y);
                    printInCurrentWindowAtPosition(i >= menuLabelCount ? nullptr : menuWindows[i].data, 2, 2);
                }
            }

            if(currentSelectedWindow != lastSelectedWindow) {
                // make the last window white
                myTFT.pCurrentWindow = &(menuWindows[lastSelectedWindow].wind);
                myTFT.fillWindow((color_t)&defaultColor);
                myTFT.setCurrentWindowColorSequence((color_t)&WHITE);

                int x = myTFT.pCurrentWindow->xMax - myTFT.pCurrentWindow->xMin;
                int y = myTFT.pCurrentWindow->yMax - myTFT.pCurrentWindow->yMin;
                myTFT.rectangle(0, 0, x, y);
                printInCurrentWindowAtPosition(lastSelectedWindow >= menuLabelCount ? nullptr : menuWindows[lastSelectedWindow].data, 2, 2);

                // make the current window red
                myTFT.pCurrentWindow = &(menuWindows[currentSelectedWindow].wind);
                myTFT.fillWindow((color_t)&defaultColor);
                myTFT.setCurrentWindowColorSequence((color_t)&RED);

                x = myTFT.pCurrentWindow->xMax - myTFT.pCurrentWindow->xMin;
                y = myTFT.pCurrentWindow->yMax - myTFT.pCurrentWindow->yMin;
                myTFT.rectangle(0, 0, x, y);
                printInCurrentWindowAtPosition(currentSelectedWindow >= menuLabelCount ? nullptr : menuWindows[currentSelectedWindow].data, 2, 2);
            }

            lastSelectedWindow = currentSelectedWindow;

            break;

        case Log:
            if(justSwitchedWindow) {
                justSwitchedWindow = 0;
                myTFT.clearDisplay();
                myTFT.resetTextCursor(&labelsWindow);
                myTFT.pCurrentWindow = &labelsWindow;
                myTFT.fillWindow((color_t)&defaultColor);

                for(int i = 0; i < MAX_DATA_WINDOWS && i < dataLabelCount; i++) {
                    myTFT.setCurrentWindowColorSequence((color_t)&WHITE);
                    printInCurrentWindowAtPosition(dataLabels[i], 1, dataWindows[i].wind.yMin);
                }
            }

            // for each data window
            //  if dirty update value
            for(int i = 0; i < MAX_DATA_WINDOWS; i++) {
                if(dataWindows[i].isDirty) {
                    dataWindows[i].isDirty = 0;
                    myTFT.pCurrentWindow = &(dataWindows[i].wind);
                    myTFT.fillWindow((color_t)&defaultColor);
                    
                    if(dataWindows[i].data != nullptr) {
                        // _serial->println(dataWindows[i].data);
                        _serial->println(dataWindows[i].wind.yMin);
                        myTFT.setCurrentWindowColorSequence((color_t)&RED);
                        // printInCurrentWindowAtPosition(dataWindows[i].data, dataSeparatorX + 3, dataWindows[i].wind.yMin); //i * YMAX * 1.0 / MAX_DATA_WINDOWS
                        printInCurrentWindowAtPosition(dataWindows[i].data, 0, 0);
                    }
                }
            }

        break;
    }
}