# KavaNest Devices
![Cover Image](https://i.imgur.com/iRd1q1m.png)

This is the physical `Devices` portion of my KavaNest home automation system which is made up of the following components
* [Client](https://github.com/InvisiBug/kavanest-client)
* [API](https://github.com/InvisiBug/kavanest-api)
* [Controller (Skippy)](https://github.com/InvisiBug/kavanest-controller)
* [Device Listener (Scraper)](https://github.com/InvisiBug/kavanest-scraper)
* Devices

## Technologies used
![C++](https://img.shields.io/badge/C++-00599C.svg?&logo=c++&logoColor=white)&nbsp;
![Arduino](https://img.shields.io/badge/-Arduino-00979D?&logo=Arduino&logoColor=white)&nbsp;
![MQTT](https://img.shields.io/badge/MQTT-3C5280?logo=eclipse-mosquitto)&nbsp;

# What is this component (Devices)
This part of the system hold the code for the physical devices that form the system. Most use the ESP8266 chip and have been programmed using the arduino system, specifically PlatformIo.
The devices communicate with the system via an MQTT layer

# What is KavaNest
The system, I’m calling KavaNest is a zone based heating system that controls my house. It allows me to control the temperature of each room independently of the others, meaning I can heat just my office during the day and turn the heating on in the living room in the evening so it’s nice and warm when people get in from work.

## The aim of the system
- Every room can be heated independently of the others
- A central place to control various lights around my house
- Act as a timer for my heated matress in the winter
- Historical data viewable in Grafana
- A way to access my computer when I'm away from home
- All built and run internally to my home network, no phoning home to nasty companies that will sell your data

## How the system works
Each room has a temperature sensor and a thermostatic radiator valve which allows the system to monitor the temperature of the rooms and direct hot water to specific radiators independently of the others. A schedule controller looks at the desired temperature versus the actual temperature and activates the heating if it's needed. At this point, the boiler switches on, the radiator valves in there rooms that don't need heat all close, and the valve in the room that requires heat opens.

## The components of the system

* Physical devices like temperature sensors and radiator valves connect to my home wifi and communicate via MQTT.

* A Node.js app listens to the MQTT messages and updates a mongo store thats keeps track of the state of the system, this node app also sends push messages to the client when something changes.

* A GraphQL API is used to interface with the system which reads and updates the data in the mongo store along with communicating with the physical devices via the MQTT network.

* Another Node.js app acts at the automation controller for the system. It uses the GraphQL API to read the current system state and update anything that needs changing like radiator valves.

* A React client app is used to interact with the system, which has been designed for mobile and desktop use. The client app mainly uses the GraphQL API, along with push messages that are sent to the client, via a web socket connection, every time something changes with the system.

* There is a Grafana dashboard that trends various data points like room temperatures and radiator valve states. This historical data has proven useful for optimizing the system, and diagnosing issues if anything strange is happening. Prometheus is used to monitor the different Raspberry Pi’s in my cluster as well as some other devices all of which have their own status dashboard.

* Along side all of this is a test environment with a simulator that replicates the physical devices. This test environment is used when I want to try out new features, this way my house mates don’t have to go without heating when I break something.

* The system is fully dockerized and is capable of being run on pretty much any platform. I have it running on my K3s cluster consisting of 3 Raspberry Pi 4Bs. Running things locally means that everything will work so long as the wifi signal is present, with no need for internet access and no company selling your data.
