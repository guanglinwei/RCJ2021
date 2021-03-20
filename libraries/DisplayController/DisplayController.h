#ifndef DisplayController_h
#define DisplayController_h

#include "HyperDisplay_KWH018ST01_4WSPI.h"
#include "Arduino.h"

typedef void (*Action)();

// TODO: get rid of point

/*
    How to use:

    serial.begin(...)
    displaycontroller.begin(...)

    To log values:
    displayController.switchToLogWindow(labels: string[]);
    displayController.updateLogValue(index, str);

    To update screen:
    displayController.updateDisplay();
*/
class DisplayController
{
    public:
        DisplayController(KWH018ST01_4WSPI tft, HardwareSerial &serial);

        void begin(uint8_t DC_PIN, uint8_t CS_PIN, uint8_t PWM_PIN, SPIClass SPI_PORT, uint32_t SPI_SPEED);
        DisplayController &createWindows(int cols, int rows);
        DisplayController &addOnClickToWindow(int index, Action onClick);
        // window with data
        void switchToLogWindow();
        void switchToLogWindow(char **dataLabels);
        void updateLogValue(int index, char *value);

        void printInWindow(wind_info_t *wind, char *text, int reset = 0);
        void printInWindowAtPosition(wind_info_t *wind, char *text, int x, int y);
        void printInCurrentWindowAtPosition(char *text, int x, int y);

        void updateDisplay();

    private:
        KWH018ST01_4WSPI myTFT;
        HardwareSerial *_serial;
        // ILI9163C_color_18_t **screenMem;

        struct Window {
            wind_info_t *pWind;
            Action onClick;
        };

        struct DataWindow {
            wind_info_t wind;
            int isDirty;
            // int updatable;
            char *data = nullptr;
        };

        struct Point {
            int x;
            int y;
            Point() : x(0), y(0) {}
            Point(int _x, int _y) : x(_x), y(_y) {}
            
        };

        enum DisplayState { 
            Menu,
            Log
        } displayState;

        // wind_info_t *windows;
        //menu
        Window *windows;
        // log window
        // "dataLabel"   |  dataWindow
        // "Compass"     |  123deg
        DataWindow *dataWindows;
        char **dataLabels;
        wind_info_t labelsWindow;
        int dataSeparatorX = XMAX / 2;

        int MAX_MENU_WINDOWS = 9;
        int MAX_DATA_WINDOWS = 4;

        // void createWindows()
        int current_window_count;
        // number of cols of windows
        int window_cols;

        int current_selected_window;
        int last_selected_window;
        int just_switched_window;

        ILI9163C_color_18_t defaultColor;
        ILI9163C_color_18_t RED;
        ILI9163C_color_18_t GREEN;
        ILI9163C_color_18_t BLUE;
        ILI9163C_color_18_t WHITE;
        ILI9163C_color_18_t ORANGE;

        int isWithinBounds(int x, int y, wind_info_t wind);
        static const int XMAX = 128;
        static const int YMAX = 160;

        Point cursorPos = Point();
        Point lastCursorPos = Point();
        int showCursor;
        int onJoystickClicked();


};

#endif