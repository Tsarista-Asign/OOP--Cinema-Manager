#ifndef MOVIE_MANAGER_H
#define MOVIE_MANAGER_H

#include "DataManager.h"
#include "Movie.h"
#include <vector>
#include <string>

class Showtime;

class MovieManager {
private:
    DataManager& data_manager;
    std::vector<Movie>& movies;
    const std::vector<Showtime>& showtimes;

public:
    MovieManager(DataManager& dm, std::vector<Movie>& m, const std::vector<Showtime>& s);
    void loadMovies();
    void displayMovies() const;
    void addMovie();
    void editMovie();
    void deleteMovie();
};

#endif // MOVIE_MANAGER_H