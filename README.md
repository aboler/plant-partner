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

## Completed Work

- Ability to control GPIO Pins w/ ESP32.
  - i.e. controlling an external light source
  - Existing interfaces that allow modular control for project's purposes
- ADC data acquisition w/ ESP32
  - i.e. reading in value from photoresistor
  - Existing interfaces that allow modular control for project's purposes
- Developed basic structure to collect plant data from sensors
- Breadboard implementation of one of our hardware/software
- Frontend initial skeleton set up for mobile Android app interface
  - Configured Flutter environment and Android Emulator
  - Built a persistent bottom navigation bar (Tasks, Home, Settings)
  - Developed placeholder screens for each tab
  - Tested UI flow and verified emulator functionality
- Implemented locally-hosted database and backend API source code with basic functions
  - Set up MongoDB database that can be hosted locally, currently stores one JSON data model
  - Created Backend App source code with an API via Express.JS that allows for communication with the backend server and database
  - Organized the source code into folders for function controllers, JSON data models, and CRUD routes for easy addition of new functionality


## Project Architecture

The ESP32 board is set up to interface with sensors to collect real-time environmental and system data. Currently, it is only configured to interact with a photoresistor. The sensor data is then processed to inform and schedule the operation of actuators thus, automating plant care mechanisms. The ESP32 also manages communication with external circuits that drive the systems with these actuators. As of now, this only involves an external LED.

All sensor readings are to be logged in a database and displayed on a mobile application, providing users with remote monitoring and control capabilities. This aspect is still in development, so currently their interactions have not been implemented.

### File Structure Summary

- ``microsrc`` : directory containing relevant ESP32 code
  - ``src`` : directory containing all developed code for the program and a CMakeLists.txt to help compile
    - ``main`` : contains main program for ESP32 and CMakeLists.txt compiling developed code
    - ``dataTypes`` : contains code defining common data types created for the project, like plantData
    - ``peripherals`` : code for modularizing intercations with certain peripheral systems, like ADC and GPIO
  - ``submodules`` : directory containing other repositories utilized within the main code
    - ``esp-idf`` : submodule containing examples and ESP32 interfaces in c

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

- ``frontEndSrc`` : directory containing relevant frontend Android code
  - ``lib``: directory containing dart code for screen components
    - ``main``: app entry point, launches navigation shell
    - `nav`: bottom navigation bar and routing
    - ``navPages``: directory containing the task, home, settings placeholder screens

- ``Documentation`` : directory containing relevant project documentation
  - ``Circuits`` : directory containing relevant circuits created during development
  - ``Class Submissions`` : directory containing a file with the link to the Google Drive folder containing all of our report work/submissions
  - ``Contributions`` : directory containing all members' timesheet records
  - ``Datasheets`` : directory containing schematics to reference for our current materials being used
  - ``Diagrams`` : directory containing relevant diagrams created during development
  - ``Materials`` : directory containing documentation related to materials, such as which items were purchased and from where
  - ``Research`` : directory containing research collected during the project, such as how to set up a coding environment for the ESP32
  - ``pcbDesign`` : directory containing work dones towards designing/building our project's PCB

## Current Bugs:

The system is in a development state with only base-level functionality implemented. No bugs reported
