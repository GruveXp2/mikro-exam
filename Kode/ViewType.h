#ifndef VIEWTYPE_H
#define VIEWTYPE_H

enum class ViewType {
    TIME,
    CLIMATE,
    WEATHER,
    NEWS,
    COUNT, // this is the count of normal views. this is so that when you do next/prev view, it will only cycle thru the main views, all other must be accessed directly
    SET_LOCATION
};

#endif //VIEWTYPE_H