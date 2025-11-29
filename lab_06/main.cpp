#include <iostream>
#include "guess_server.h"

using namespace std;

void start_guess_number_server() {
    GuessServer s = play_guess_server("");

    cout << "Try to guess the secret number (1–100)" << endl;

    while (s.in_progress()) {
        cout << "Your guess: ";
        int user;
        cin >> user;

        s.set_request(user);

        s.move_next();
        int response = s.current_value_safety();

        if (response == 0) {
            cout << "Correct!" << endl;
            break;
        } else if (response < 0) {
            cout << "Too small!" << endl;
        } else {
            cout << "Too big!" << endl;
        }
    }
}

int main() {
    start_guess_number_server();
}
