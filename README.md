# ETE3007-IRMAP

Infrared Map Constructor Robot

## Custom Libraries

The project includes the class `IRSensor` in the form of header and cpp files placed in the root directory of the main ino file. The reason they are placed there (i.e. not in their own directory somewhere else for a better organization) is to let the compiler find the definition directly (and not have to tinker with the include path).

If you want to use the `IRSensor` class in your own project, you can place them in the same directory as your ino file (and, of course, import them at the top of the sketch using `#include "IRSensor.h"`). For Arduino IDE users, you can also put the two files in a folder with the same name, zip it, then include it from the **Sketch > Include Library > Add .ZIP Library...** menu.

## VS Code Configuration

For VS Code users, we recommend some extensions and settings for a comfortable development experience. Settings refer to files in the `.vscode` folder, and are to be _added_ to existing settings if any, not _replace_ them.

### Arduino

#### Extensions

-   [Arduino](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) (Microsoft)
-   [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) (Microsoft)

#### Settings

-   `arduino.json`
    ```json
    {
        "board": "esp8266:esp8266:d1_mini",
        "sketch": "irmap-wemos\\irmap-wemos.ino"
    }
    ```
-   `c_cpp_properties.json`

    Absolute paths are default ones—they may be different on your machine. Don't forget to change `{USER}` to your username.

    ```json
    {
        "configurations": [
            {
                "includePath": [
                    "C:\\Users\\{USER}\\AppData\\Local\\Arduino15\\packages\\esp8266\\hardware\\esp8266\\2.7.0\\**"
                ],
                "forcedInclude": [
                    "C:\\Users\\{USER}\\AppData\\Local\\Arduino15\\packages\\esp8266\\hardware\\esp8266\\2.7.0\\cores\\esp8266\\Arduino.h",
                    "C:\\Users\\{USER}\\AppData\\Local\\Arduino15\\packages\\esp8266\\hardware\\esp8266\\2.7.0\\variants\\d1_mini\\pins_arduino.h"
                ]
            }
        ]
    }
    ```

-   `settings.json`

    These settings are optional. They aim to conform to the standard Arduino code style.

    ```json
    {
        "[ino]": {
            "editor.tabSize": 2
        },
        "C_Cpp.clang_format_style": "{BreakBeforeBraces: Attach}"
    }
    ```

### Python

#### Extensions

-   [Anaconda Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-python.anaconda-extension-pack) (Microsoft)
-   [Python](https://marketplace.visualstudio.com/items?itemName=ms-python.python) (Microsoft)
-   [Python Docstring Generator](https://marketplace.visualstudio.com/items?itemName=njpwerner.autodocstring) (Nils Werner)
