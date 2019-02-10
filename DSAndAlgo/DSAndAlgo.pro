TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

TEMPLATE = lib
android {
    TEMPLATE = app  # for Android this is an "app"
}


HEADERS += \
    SegmentSet/segment.hpp \
    SegmentSet/segment_set.hpp
