cmake_minimum_required(VERSION 3.3)
project(RTC_GLO_TP1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp arret.cpp arret.h auxiliaires.cpp auxiliaires.h coordonnees.cpp coordonnees.h DonneesGTFS.cpp DonneesGTFS.h ligne.cpp ligne.h station.cpp station.h voyage.cpp voyage.h)
add_executable(RTC_GLO_TP1 ${SOURCE_FILES})