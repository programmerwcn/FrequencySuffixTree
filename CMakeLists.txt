#cmake_minimum_required(VERSION 3.15)
project(untitled)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled main.cpp rstartreeold.h rstartreeold.cpp storage.cpp storage.h util.cpp util.h FrequentPhraseDetection.cpp FrequentPhraseDetection.h pcta.cpp pcta.h)