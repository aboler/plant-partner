# plant-partner

Plant Partner repo. for CEN4907c

- **Contributors**: Nicholas Cimino, Aspen Boler, Sabrina Luu, Carter Kreis, Fernando Amado-Pupo
- [Project link](https://github.com/users/aboler/projects/1)

### Notes for adding code and time reports

- Please create a new branch to develop off of main
  - main remains as a minimum viable product branch available
  - Allows PR's to be made for adding to dev so group members can review code made
- Add reports for any changes/development work done in a day in main branch
  - Make reports in Documentation/Reports in relevant timesheet

## Meeting Times

- Meet weekly for ~15 minutes after every client meeting to ensure we're staying on track

## Completed Work

### Hardware Release

Please see ``Documentation/Diagrams``, ``Documentation/Circuits``, and ``Documentation/pcbDesign`` for current hardware designed, built, and used for testing

### Software Release

For ESP32 (in VSCode):

1. Run ``git clone recurse-submodules git@github.com:aboler/plant-partner.git``
   - This will clone the base plant-partner and esp-idf Github repositories using ssh
2. Download the ESP-IDF Extension
3. After you install the extension, the  should appear in the VS Code Activity bar (left side set of icons). When you click the Espressif icon, you can see a list of basic commands provided by this extension.
4. From the command list, select Configure ESP-IDF Extension or press F1 and type “Configure ESP-IDF Extension”. After, choose the ESP-IDF: Configure ESP-IDF Extension option.
5. Choose Express. Select Github for the download server.
6. For “Select ESP-IDF version” select “Find ESP-IDF in your system”
7. For “Enter ESP-IDF directory (IDF_PATH):” click the folder icon and within the Github Repository select /plant-partner/microSrc/submodules/esp-idf
8. For “Enter ESP-IDF Tools directory (IDF_TOOLS_PATH):” select the default location or wherever you prefer
   - Make sure that IDF_TOOLS_PATH had no spaces and is not in the Github Repo
9. If your operating system is MacOS/Linux, choose the system Python executable to create ESP-IDF virtual environment inside ESP-IDF Tools and install ESP-IDF Python package there
10. Click Install
11. If everything is installed correctly, you will see a message that all settings have been configured

In Powershell or a Linux terminal preferably:

1. Run ``cd microSrc``
2. Connect an ESP-32 into computer
3. Setup ESP-IDF and flash ESP-32 by running ``python esp.py -p PORT``
   - You may need to use python3 instead of python
   - PORT will be something like COM3 for Windows and /dev/ttyUSB0 for Ubuntu

For Backend + Database:

1. Navigate to ``backEndSrc/``
2. Start up the backend server by running ``npm start``, server will be running once these messages are displayed on the terminal
   - "Connected to MongoDB"
   - "Server running on port 8000"

*For Frontend Application*

In Bash or Powershell

1. Run `cd frontEndSrc`
2. Run `cd frontend`
3. Run `flutter emulators –launch Medium_Phone_API_36.1`
4. Run `flutter pub get`
   * This command is to pull the dependencies
5. Run `flutter run`

### Pre-Alpha Build

- Ability to control GPIO Pins w/ ESP32.
  - i.e. controlling an external light source
  - Existing interfaces that allow modular control for project's purposes
- ADC data acquisition w/ ESP32
  - i.e. reading in value from photoresistor
  - Existing interfaces that allow modular control for project's purposes
- Developed basic structure to collect plant data from sensors
  - 3 uint16_t variables for collecting light, moisture, and nutrient data
- Breadboard implementation of our hardware/software
  - External photoresistor/LED circuit
- Frontend initial skeleton set up for mobile Android app interface
  - Configured Flutter environment and Android Emulator
  - Built a persistent bottom navigation bar (Tasks, Home, Settings)
  - Developed placeholder screens for each tab
  - Tested UI flow and verified emulator functionality
- Implemented locally-hosted database and backend API source code with basic functions
  - Set up MongoDB database that can be hosted locally, currently stores one JSON data model
  - Created Backend App source code with an API via Express.JS that allows for communication with the backend server and database
  - Organized the source code into folders for function controllers, JSON data models, and CRUD routes for easy addition of new functionality

### Prototype Build

- Ability to configure input GPIO Pins w/ ESP32
- Ability to generate a PWM signal
  - i.e. controlling a water pump omtor
- Modularized ADC data acquisition
  - Allows for configuration for both photoresistor and mositure sensor
- Modified breadboard implementation
  - Developed switch circuit to determine when to sample connected sensors/actuate components
  - e.g. water pump control, photoresistor and moisture sensing, LED actuation
- Ability to connect ESP32 to Wi-Fi
  - Allows new plant data to be sent to the database
- Created a test plant on the database
- Connected application and database so application can receive and display plant data to the Home tab
- Created a update button that allows application to display newly updated data from the database
- Beginnings of a PCB to replace external wiring has been started and posted to the repository
- Migrate database to the cloud via MongoDB atlas to allow for easier testing

## Project Architecture

The ESP32 board is set up to interface with sensors to collect real-time environmental and system data. Currently, it is configure to interact with a photoresistor and moisture sensor. The sensor data is then processed to inform and schedule the operation of actuators thus, automating plant care mechanisms. The ESP32 also manages communication with external circuits that drive the systems with these actuators. As of now, an external switch circuit is set up to control sensor sampling and component actuation. Switch0 is responsible for sampling the photoresistor and updating the LED if its detected to be too dark, Switch1 samples the moisture sensor, Switch2 actuates the water pump motor, and Switch3 posts plant data to the database to mimic receiving commands from the application.

The sensor readings can be reported to the database and displayed on a mobile application, providing users with remote monitoring. To update the data shown on the application, users must press the update button available in the Home tab. In the future, commands will be able to be sent and received from the application. However, this aspect is still in development and has not been implemented.

### File Structure Summary

- ``microsrc`` : directory containing relevant ESP32 code

  - ``scripts`` : directory containing scripts necessary for setting up ESP-IDF dependencies and environment for device
  - ``src`` : directory containing all developed code for the program and a CMakeLists.txt to help compile
    - ``main`` : contains main program for ESP32 and CMakeLists.txt compiling developed code
    - ``dataTypes`` : contains code defining common data types created for the project, like plantData
    - ``peripherals`` : code for modularizing intercations with certain peripheral systems, like ADC, GPIO, and PWM
    - ``wifi`` : code for implementing wifi communication in the esp32
  - ``submodules`` : directory containing other repositories utilized within the main code
    - ``esp-idf`` : submodule containing examples and ESP32 interfaces in c
  - ``esp.py`` : Python script which can setup, build, flash, and/or monitor ESP-32
- ``backEndSrc`` : directory containing relevant backend code

  - ``controller`` : directory containing code for controllers that implement backend API CRUD functions for the server
    - ``sensorController.js`` : code for implementing CRUD functions that work with JSON data of the Sensor model
  - ``model`` : directory containing code for JSON data models to be used by the backend server and MongoDB database
    - ``sensorModel.js`` : code for creating the JSON Sensor data model
  - ``routes`` : directory containing code for the API's routes that allow CRUD functions to be accessed via URI in an HTTP request
    - ``sensorRoute.js`` : code for the routes that make use of functions that work with the Sensor JSON data model
  - ``node_modules`` : directory containing the node.js dependencies needed to create the backend code
  - ``.env`` : file that holds environment variables to be used by the main code of the backend server, app.js
  - ``app.js`` : the main code of the backend server, connecting to the mongoDB database, linking the routes, and locally hosting server
  - ``package-lock.json`` : the package-lock file for all the node package dependencies needed for the backend server
  - ``package.json`` : the package file listing the basic details and dependencies of the backend server project
- ``frontEndSrc`` : directory containing all Flutter/Dart source code for the mobile application.

  - ``lib``: root directory for all application logic, user interface screens, and services.
    - ``main``: application entry point; initializes the app and launches the bottom navigation shell
    - `nav.dart/dashboard_nav.dart`: bottom navigation bar widget and screen routing logic
    - ``views/``: directory containing all primary UI pages
      - `homeView.dart`: displays live plant sensor data from MongoDB and includes refresh functionality.
      - `taskView.dart/settingsView.dart:` placeholder screen for task/scheduling and user settings
      - `addPlantView.dart`: screen for adding new plants (prototype stage)
    - `services/`: directory for backend communication modules
      - `remote_service.dart`: handles HTTP requests to the backend
    - `plant.dart`: types data model used to decode JSON plant objects from the database
- ``Documentation`` : directory containing relevant project documentation

  - ``Circuits`` : directory containing relevant circuits created during development
  - ``ClassSubmissions`` : directory containing a file with the link to the Google Drive folder containing all of our report work/submissions
  - ``Contributions`` : directory containing all members' timesheet records
  - ``Datasheets`` : directory containing schematics to reference for our current materials being used
  - ``Diagrams`` : directory containing relevant diagrams created during development
  - ``Materials`` : directory containing documentation related to materials, such as which items were purchased and from where
  - ``Research`` : directory containing research collected during the project, such as how to set up a coding environment for the ESP32
  - ``pcbDesign`` : directory containing work dones towards designing/building our project's PCB

## How to Use esp.py

PORT: This is USB port that the ESP-32 is connected to. For Windows, it should be COM3 or something similar. For Ubuntu, it should be /dev/ttyUSB0 or something similar. Be aware, that if the Port is not automatically detected (usually Windows) you may have to install a driver being Silicon Labs CP210x or FTDI.

- Default: ``python esp.py -p PORT``
  - This will run the necessary setup scripts to install the ESP-IDF dependencies then build, flash, and monitor the ESP-32 at PORT
- Build: ``python esp.py build``
  - This will only build the current code under microSrc/src
- Flash: ``python esp.py flash -p PORT``
  - This will build and flash the current code under microSrc/src onto the ESP-32 at the connected Port
- Monitor: ``python esp.py monitor -p PORT``
  - This will monitor the ESP-32 at the connected Port and fill the terminal with any outputs from the ESP-32
- Clean: ``python esp.py clean``
  - This will delete any build artifacts

N.B. You may have to use python3 instead of python

## Current Bugs:

The system is in a development state with only base-level functionality implemented. No bugs reported
