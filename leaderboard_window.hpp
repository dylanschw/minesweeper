#ifndef LEADERBOARD_WINDOW_HPP
#define LEADERBOARD_WINDOW_HPP

#include <string>

// Opens the leaderboard window, displays the contents of "files/leaderboard.txt",
// and highlights the current player's score with an asterisk (*).
void showLeaderboard(const std::string& currentPlayer);

#endif // LEADERBOARD_WINDOW_HPP
