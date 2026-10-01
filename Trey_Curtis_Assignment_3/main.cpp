/******************************************************************************

Welcome to GDB Online.
  GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby, 
  C#, OCaml, VB, Perl, Swift, Prolog, Javascript, Pascal, COBOL, HTML, CSS, JS
  Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
//EECS 348 Assignment 3
//C++ program that prioritzies emails for a busy company CEO
//Collaborators: Ethan Church
// Input:
// A text file containing EMAIL, NEXT, READ, and COUNT commands.
// Output:
// Displays the highest-priority email and the number of
// unread emails when requested.
//Code from
//Trey Curtis and Gemini
//Creation Date: (9/29/2026)
//Revision Date (9/29/2026)
#include <iostream> 
#include <string> 
#include <vector> 
#include <sstream> 
#include <fstream> 
#include <stdexcept> 

// stores the information for one email
struct Email {
    std::string senderCategory; // stores who sent the email
    std::string subject; // stores the email subject
    std::string date; // stores the date
    int categoryRank; // stores the priority number
    int dateValue; // stores the date as a number

    // makes a new email and calculates its priority
    Email(const std::string& cat, const std::string& subj, const std::string& d)
        : senderCategory(cat), subject(subj), date(d) { // sets the email values
        
        // gives each sender type a priority number
        if (senderCategory == "Boss") categoryRank = 5; // boss gets highest priority
        else if (senderCategory == "Subordinate") categoryRank = 4; // subordinate priority
        else if (senderCategory == "Peer") categoryRank = 3; // peer priority
        else if (senderCategory == "ImportantPerson") categoryRank = 2; // important person priority
        else if (senderCategory == "OtherPerson") categoryRank = 1; // other person priority
        else categoryRank = 0; // unknown sender gets lowest priority

        // changes the date into a number
        if (date.length() >= 10) { // makes sure the date is long enough
            int month = std::stoi(date.substr(0, 2)); // gets the month
            int day = std::stoi(date.substr(3, 2)); // gets the day
            int year = std::stoi(date.substr(6, 4)); // gets the year
            dateValue = year * 10000 + month * 100 + day; // puts the date into one number
        } else {
            dateValue = 0; // uses 0 if the date is not valid
        }
    }

    // compares two emails to see which one has more priority
    bool operator>(const Email& other) const {
        // compares the sender priority first
        if (categoryRank != other.categoryRank) {
            return categoryRank > other.categoryRank; // higher rank means higher priority
        }

        // if they have the same rank then compare the dates
        return dateValue > other.dateValue; // newer date gets priority
    }
};

// max heap used for organizing the emails
class MaxHeap {
private:
    std::vector<Email> heap; // stores all the emails

    // moves an email up the heap
    void heapifyUp(size_t index) {
        while (index > 0) { // keeps going until it reaches the top
            size_t parent = (index - 1) / 2; // finds the parent
            if (heap[index] > heap[parent]) { // checks if the child has more priority
                std::swap(heap[index], heap[parent]); // swaps the two emails
                index = parent; // moves up to the parent
            } else {
                break; // stops if the heap is in the right order
            }
        }
    }

    // moves an email down the heap
    void heapifyDown(size_t index) {
        size_t size = heap.size(); // gets the number of emails
        while (true) { // keeps checking until it is in the right spot
            size_t largest = index; // starts with the current email
            size_t left = 2 * index + 1; // finds the left child
            size_t right = 2 * index + 2; // finds the right child

            if (left < size && heap[left] > heap[largest]) { // checks the left child
                largest = left; // left child becomes the highest
            }

            if (right < size && heap[right] > heap[largest]) { // checks the right child
                largest = right; // right child becomes the highest
            }

            if (largest != index) { // checks if a swap is needed
                std::swap(heap[index], heap[largest]); // swaps the emails
                index = largest; // moves down to the new position
            } else {
                break; // stops if everything is in order
            }
        }
    }

public:
    // adds a new email to the heap
    void insert(const Email& email) {
        heap.push_back(email); // adds the email to the end
        heapifyUp(heap.size() - 1); // moves it to the right spot
    }

    // checks if the heap has no emails
    bool isEmpty() const {
        return heap.empty(); // returns true if there are no emails
    }

    // gets the number of emails
    size_t size() const {
        return heap.size(); // returns the heap size
    }

    // gets the email with the highest priority
    const Email& peek() const {
        if (isEmpty()) { // checks if there are no emails
            throw std::out_of_range("Cannot peek on an empty heap."); // gives an error if empty
        }

        return heap[0]; // returns the first email
    }

    // removes the highest priority email
    void extractMax() {
        if (isEmpty()) return; // does nothing if there are no emails

        heap[0] = heap.back(); // moves the last email to the front
        heap.pop_back(); // removes the old last email

        if (!isEmpty()) { // checks if there are still emails
            heapifyDown(0); // fixes the heap order
        }
    }
};

// removes extra spaces from strings
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n"); // finds the first real character
    if (first == std::string::npos) return ""; // returns empty if there is nothing there

    size_t last = str.find_last_not_of(" \t\r\n"); // finds the last real character
    return str.substr(first, (last - first + 1)); // returns the cleaned string
}

// starts the main program
int main(int argc, char* argv[]) {
    // chooses the input file
    std::string filename = (argc > 1) ? argv[1] : "input.txt"; // uses the argument or input.txt

    std::ifstream inputFile(filename); // opens the input file
    std::istream& input = inputFile.is_open() ? inputFile : std::cin; // uses the file or keyboard input

    MaxHeap emailQueue; // creates the email heap
    std::string line; // stores each line of input

    // reads the input one line at a time
    while (std::getline(input, line)) {
        line = trim(line); // removes extra spaces
        if (line.empty()) continue; // skips empty lines

        // checks if the command starts with EMAIL
        if (line.rfind("EMAIL", 0) == 0) {
            std::string data = trim(line.substr(5)); // gets the email information
            std::stringstream ss(data); // makes the data easier to split

            std::string sender, subject, date; // stores the email fields

            // gets the sender subject and date
            if (std::getline(ss, sender, ',') &&
                std::getline(ss, subject, ',') &&
                std::getline(ss, date)) {
                
                // creates the email and puts it into the heap
                emailQueue.insert(Email(trim(sender), trim(subject), trim(date)));
            }
        }

        // checks if the command is COUNT
        else if (line == "COUNT") {
            std::cout << "There are " << emailQueue.size() << " emails to read.\n"; // prints email count
        }

        // checks if the command is NEXT
        else if (line == "NEXT") {
            if (!emailQueue.isEmpty()) { // makes sure there is an email
                const Email& top = emailQueue.peek(); // gets the highest priority email
                std::cout << "Next email:\n"; // prints the next email heading
                std::cout << "Sender: " << top.senderCategory << "\n"; // prints the sender
                std::cout << "Subject: " << top.subject << "\n"; // prints the subject
                std::cout << "Date: " << top.date << "\n"; // prints the date
            }
        }

        // checks if the command is READ
        else if (line == "READ") {
            if (!emailQueue.isEmpty()) { // makes sure there is an email
                emailQueue.extractMax(); // removes the highest priority email
            }
        }
    }

    return 0; // ends the program
}

