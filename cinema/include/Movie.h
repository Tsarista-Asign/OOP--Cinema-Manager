#ifndef MOVIE_H
#define MOVIE_H

#include "DataEntity.h"
#include <string>

class Movie : public DataEntity {
public:
    std::string id;
    std::string title;
    int duration;

    Movie(const json& j);
    void display() const override;
    json toJson() const override;
};

#endif // MOVIE_H