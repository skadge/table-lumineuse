La Table Lumineuse
==================

This repository holds the C++ source code of the light table I've build for my
daughters.

![Picture of the table](doc/table1.jpg)

It is meant to run on a Raspberry PI and controls a [LED
strip](http://www.adafruit.com/product/306) as well as speakers.

The whole thing can be controlled from any device (tablet, phone) by connecting
to its web page. It exposes a simple REST API (see
[handler.cpp](src/handler.cpp) for the exact list of support actions), so it
would be easy to integrate it with other applications.


Dependencies
------------

- a webserver for the HTML interface
- SFML 2+ for audio support
