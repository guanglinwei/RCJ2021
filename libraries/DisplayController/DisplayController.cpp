#include "DisplayController.h"

//window types:
// display data
// or menus
DisplayController::DisplayController(KWH018ST01_4WSPI tft, HardwareSerial &serial) : myTFT(tft), _serial(&serial) {


    windows = new Window[MAX_MENU_WINDOWS];
    dataWindows = new DataWindow[MAX_DATA_WINDOWS];
    // windows = new wind_info_t[9];
    current_window_count = 0;
    window_cols = 0;
    current_selected_window = 0;
    last_selected_window = 0;

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
        wind.pWind = &pw;
        wind.onClick = nullptr;
        windows[i] = wind;
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

        // w.xMin = 0;
        // w.xMax = dataSeparatorX;
        // w.yMin = (int)(dY * i);
        // w.yMax = (int)(dY * i + dY - 1);

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

    // for(int i = 0; i < 2; i++) {
    //     myTFT.setWindowDefaults(&windows[i]);
    //     myTFT.pCurrentWindow = &windows[i];
    //     myTFT.setCurrentWindowColorSequence((color_t)&defaultColor);
    //     myTFT.setWindowMemory(&windows[i], screenMem[i], 128*160/2);

    // }
    // windows[0].xMax = 63;
    // windows[0].yMax = 79;
    // windows[1].xMin = 64;
    // windows[1].xMax = 127;
    // windows[1].yMin = 80;
    // windows[1].yMax = 179;
    myTFT.clearDisplay();

    // menu windows
    for(int i = 0; i < MAX_MENU_WINDOWS; i++) {
        myTFT.pCurrentWindow = windows[i].pWind;
        myTFT.setCurrentWindowColorSequence((color_t)&defaultColor);
    }

    // log windows
    for(int i = 0; i < MAX_DATA_WINDOWS; i++) {
        myTFT.pCurrentWindow = &(dataWindows[i].wind);
        myTFT.setCurrentWindowColorSequence((color_t)&defaultColor);
    }
    myTFT.setCurrentWindowColorSequence((color_t)&defaultColor);
    _serial->println("begin");
    // myTFT.setCurrentWindowMemory((color_t)screenMem, 128*160);
    
}

DisplayController& DisplayController::createWindows(int cols, int rows) {
    if(cols * rows > 9) return *this;

    window_cols = cols;
    current_window_count = rows * cols;

    float dX = XMAX * 1.0 / cols;
    float dY = YMAX * 1.0 / rows;

    for(int r = 0; r < rows; r++) {
        for(int c = 0; c < cols; c++) {
            // TODO: does this change the actual struct
            wind_info_t *w = windows[r * cols + c].pWind;
            w->xMax = (int)(dX * c + dX - 1);
            w->xMin = (int)(dX * c);
            w->yMin = (int)(dY * r);
            w->yMax = (int)(dY * r + dY - 1);
            
        }
    }

    return *this;
}

DisplayController& DisplayController::addOnClickToWindow(int index, Action onClick) {
    windows[index].onClick = onClick;
    return *this;
}

int DisplayController::isWithinBounds(int x, int y, wind_info_t wind) {
    if(x < wind.xMin || x > wind.xMax) return 0;
    if(y < wind.yMin || y > wind.yMax) return 0;
    return 1;
}

int DisplayController::onJoystickClicked() {
    //...
    // for(int i = current_window_count - 1; i >= 0; i--) {
    //     if(isWithinBounds(cursorPos.x, cursorPos.y, windows[i].pWind) && windows[i].onClick) {
    //         windows[i].onClick();
    //         return 1;
    //     }
    // }

    if(current_selected_window < current_window_count) {
        (*(windows[current_selected_window].onClick))();
        return 1;
    }
    
    return 0;
}

void DisplayController::switchToLogWindow() {
    displayState = Log;
    just_switched_window = 1;
}

void DisplayController::switchToLogWindow(char **labels) {
    dataLabels = labels;
    switchToLogWindow();
}

void DisplayController::updateLogValue(int index, char *value) {
    dataWindows[index].isDirty = 1;
    dataWindows[index].data = value;
}

void DisplayController::printInWindow(wind_info_t *wind, char *text, int reset) {
    if(reset) {
        myTFT.resetTextCursor(wind);
    }

    myTFT.pCurrentWindow = wind;
    myTFT.print(text);
}

void DisplayController::printInWindowAtPosition(wind_info_t *wind, char *text, int x, int y) {
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


    // log window
    // if just switched print the labels
    if(displayState == Log) {
        if(just_switched_window) {
            _serial->println("just switched");
            just_switched_window = 0;
            myTFT.clearDisplay();
            myTFT.resetTextCursor(&labelsWindow);
            myTFT.pCurrentWindow = &labelsWindow;
            myTFT.fillWindow((color_t)&defaultColor);

            for(int i = 0; i < MAX_DATA_WINDOWS; i++) {
                _serial->println(dataLabels[i]);
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
    }
}