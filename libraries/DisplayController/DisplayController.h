#ifndef DisplayController_h
#define DisplayController_h

#include "HyperDisplay_KWH018ST01_4WSPI.h"
#include "Arduino.h"

typedef void (*Action)();

/*
    How to use:

    serial.begin(...)
    displaycontroller.begin(...)

    To log values:
    displayController.switchToLogWindow(labels: string[], count: labels.length);
    displayController.updateLogValue(index, str);

    To update screen:
    displayController.updateDisplay();
*/
class DisplayController
{
    public:
        /**
         * DisplayController constructor.
         * @param tft TFT display object.
         * @param serial reference to Arduino serial object.
         */
        DisplayController(KWH018ST01_4WSPI tft, HardwareSerial &serial);
        /**
         * Sets up initial state of DisplayController. This must be called before using any other functions.
        */
        void begin(uint8_t DC_PIN, uint8_t CS_PIN, uint8_t PWM_PIN, SPIClass SPI_PORT, uint32_t SPI_SPEED);

        /**
         * Creates windows on the menu screen.
         * @param cols the number of columns.
         * @param rows the number of rows.
         * @return *this
         */
        DisplayController &createWindows(int cols, int rows);
        /**
         * Set the function to be called when a menu window is clicked.
         * @param index the index of the window.
         * @param onClick the function to be called.
         * @return *this
         */
        DisplayController &setOnClickForWindow(int index, Action onClick);

        /**
         * Switch the display state to Menu.
         * @see DisplayState
         */
        void switchToMenuWindow();
        /**
         * Switch the display state to Menu and labels the menu windows.
         * @param labels the text labels as a pointer to a char pointer.
         * @param count the number of labels.
         * @see DisplayState
         */
        void switchToMenuWindow(char **labels, int count);
        
        /**
         * Switch the display state to Log.
         * @see DisplayState
         */
        void switchToLogWindow();
        /**
         * Switch the display state to Log and labels each of the data.
         * @param dataLabels the text labels as a pointer to a char pointer.
         * @param count the number of labels.
         * @see DisplayState
         */
        void switchToLogWindow(char **dataLabels, int count);
        /**
         * Change the value of a data point on the log window.
         * @param index the index of the data point.
         * @param value the value of the data.
         */
        void updateLogValue(int index, char *value);

        /**
         * Print some text in a certain window.
         * @param wind a pointer to the window to print in.
         * @param text the text to print.
         * @param reset false (0) or true (1). If true, reset the text cursor.
         */
        void printInWindow(wind_info_t *wind, char *text, int reset = 0);
         /**
         * Print some text in a certain window at a position.
         * @param wind a pointer to the window to print in.
         * @param text the text to print.
         * @param x the x-coordinate to print at.
         * @param y the y-coordinate to print at.
         */
        void printInWindowAtPosition(wind_info_t *wind, char *text, int x, int y);
         /**
         * Print some text in the currently selected window.
         * @param text the text to print.
         * @param x the x-coordinate to print at.
         * @param y the y-coordinate to print at.
         */
        void printInCurrentWindowAtPosition(char *text, int x, int y);
        
        /**
         * An enum.
         * The type of input given from a joystick.
         */
        enum JoystickInputType {
            IN_CLICK,
            IN_UP,
            IN_RIGHT,
            IN_DOWN,
            IN_LEFT,
            IN_NONE
        } joystickInput;
        /**
         * Change the selected window or perform the selected action dictated by an input.
         * @param inputType the joystick input.
         * @see JoystickInputType
         * @return 1 if the input successfully performed an action, 0 otherwise.
         */
        int onJoystickInput(JoystickInputType inputType);
        /**
         * Handle a joystick input given the x and y readings.
         * @param x the joystick x value.
         * @param y the joystick y value.
         * @return 1 if successfully handled input, 0 otherwise.
         * @see handleJoystickInputUtil()
         */
        int handleJoystickAxis(int x, int y);
        /**
         * A helper function that only performs if the joystick input changed.
         * @param inputType the joystick input.
         * @see handleJoystickAxis()
         */
        int handleJoystickInputUtil(JoystickInputType inputType);

        /**
         * Update the display to match the current state. Usually called every frame.
         */ 
        void updateDisplay();

    private:
        KWH018ST01_4WSPI myTFT;
        HardwareSerial *_serial;
        // ILI9163C_color_18_t **screenMem;

        struct Window {
            wind_info_t wind;
            Action onClick = nullptr;
            char *data = nullptr;
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
            Menu, /**< Displays a grid of windows with labels. When highlighting a window and clicking the joystick, call the function assigned to that window. */
            Log /**< View and debug values. Used to validate sensor inputs and check logging. */
        } displayState;

        Window *menuWindows;
        int menuLabelCount;
        // log window
        // "dataLabel"   |  dataWindow
        // "Compass"     |  123deg
        DataWindow *dataWindows;
        char **dataLabels;
        int dataLabelCount;
        wind_info_t labelsWindow;
        int dataSeparatorX = XMAX / 2;

        int MAX_MENU_WINDOWS = 9;
        int MAX_DATA_WINDOWS = 4;

        int currentWindowCount;
        // number of cols/rows of windows
        int menuCols;
        int menuRows;

        // selected menu window
        int currentSelectedWindow;
        int lastSelectedWindow;

        int justSwitchedWindow;

        ILI9163C_color_18_t defaultColor;
        ILI9163C_color_18_t RED;
        ILI9163C_color_18_t GREEN;
        ILI9163C_color_18_t BLUE;
        ILI9163C_color_18_t WHITE;
        ILI9163C_color_18_t ORANGE;

        int isWithinBounds(int x, int y, wind_info_t wind);
        static const int XMAX = 128;
        static const int YMAX = 160;

        JoystickInputType lastJoystickInput = IN_NONE;

};

#endif