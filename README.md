# plant-partner
Plant Partner repo. for CEN4907c
- [Project link](https://github.com/users/aboler/projects/1)
- Contributors: Nicholas Cimino, Aspen Boler, Sabrina Luu, Carter Kreis, Fernando Amado-Pupo

## Notes for adding code
- Please create a new branch to develop off of
  - main remains as a minimum viable product branch available
  - Allows PR's to be made for adding to dev so group members can review code made

## Notes for reports
- Add reports for any changes/development work done in a day in main branch
  - Make reports in Documentation/Reports in relevant timesheet

##Completed Work
  - Basic interactive Android app interface
  - Ability to control GPIO Pins w/ ESP32.
  - ADC data acquisition w/ESP32
  - Breadboard implementation of one of our hardware/software 

## Project Architecture
The ESP32 board interfaces with multiple sensors to collect real-time environmental and system data. This data is processed to inform and schedule the operation of actuators thus, automating the control of p. 
The ESP32 also manages communication with external circuits that drive the pumping systems for both water and fertilizer. Additionally, all sensor readings are logged in a database and displayed on a mobile application, providing users with remote monitoring and control capabilities.

##Current Bugs:
The system is in a skeleton state with only base-level functionality implemented. No bugs reported
