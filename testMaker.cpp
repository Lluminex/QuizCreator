//=============================================================================
// Name        : testMaker.cpp
// Author      : Levi Hoffer
// Description : Allows user to create a quiz and then take that quiz
// Compile     : g++ testMaker.cpp (in linux)
// Run         : ./a.out
// Credit      : Used chatGPT to help with limit error
//             : Used youTube video (https://www.youtube.com/watch?v=eiugg-Nb40A)
//             : to learn about class implementation
//=============================================================================
//#define UNIT_TESTING // UNCOMMENT FOR TESTING!!
#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <memory>
#include <cassert>
//#include <vector> //no arrays are used in this code
#include <iomanip>
#include <algorithm> // Include algorithm for std::find
//#include <unordered_map>

using namespace std;


// Struct representing a choice in the linked list of choices for MCQs
struct ChoiceNode {
    char letter; //stores letter
    string choiceText;           // Text of the answer choice

    ChoiceNode* next; // Pointer to the next choice

    // Constructor for initializing a choice with given text
    explicit ChoiceNode(char c, const string& text) : letter(c), choiceText(text), next(nullptr) {}
};


// Struct representing a question in the quiz
struct Question {
    int id;                     // Unique ID for the question
    string type;                // Type of question (e.g., mcq, tf, wr)
    string text;                // Question text
    //vector<string> choices;     // Answer choices (for MCQs)
    ChoiceNode* choicesHead; //answer choices  for MCQs
    string correctAnswer;       // Correct answer
    double points;              // Points awarded for this question
    shared_ptr<Question> next;  // Pointer to the next question in the linked list
    string studentAnswer; //The student's answer 0_0

    // Constructor to initialize a question with a given ID and default values
    explicit Question(int questionId) : id(questionId), choicesHead(nullptr), points(0.0), next(nullptr) {}

    void addChoice(char c, const string& text) {
        ChoiceNode* newAnswer = new ChoiceNode(c, text);
        if (choicesHead == nullptr) {
            choicesHead = newAnswer;
        } else {
            ChoiceNode* temp = choicesHead;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newAnswer;
        }
    }

    void printQuestion() {
        ChoiceNode* temp = choicesHead;
        while (temp != nullptr) {
            std::cout << temp->letter << ". " << temp->choiceText << std::endl;
            temp = temp->next;
        }
        std::cout << "Correct answer: " << correctAnswer << std::endl;
    }

    void resetAnswers() {
        ChoiceNode* current = choicesHead;
        while (current != nullptr) {
            ChoiceNode* nextNode = current->next; // Save the next node
            delete current;                        // Delete current node
            current = nextNode;                    // Move to next node
        }

        // Ensure head is a nullptr after clearing
        choicesHead = nullptr;
    }
};


// Class representing the quiz and containing operations to manage questions
class Quiz {
private:
    shared_ptr<Question> head;  // Head pointer of the linked list of questions
    int questionCount;          // Counter for the number of questions in the quiz
    double totalPoints;         // Sum of all points for all questions
    double score;               //sum of student points

public:
    // Constructor initializing the quiz with no questions
    Quiz() : head(nullptr), questionCount(0), totalPoints(0.0), score(0.0) {}
    bool errorMessage = false;
    double getScore() {
        return score;
    }
    // Function to clear the input stream, handling input errors
    void clearInput() {
        cin.clear();
        if (cin.peek() == '\n') {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    //check if no questions submitted
    bool check1() {
        if(questionCount == 0) {
            cout << "Warning - the number of questions to be asked must equal to or be larger than 1." << endl;
            return false;
        }
        return true;
    }
    //check for null ptr;
    bool check2() {
       if(!check1()) {
           if(head == nullptr) {
               cout << "[Linked list set to NULL]" << endl;
               return false;
           }
           cout << "[Linked list not set to NULL]" << endl;
           return true;

       }
        return true;
    }

private:
    // Helper function to get validated input from the user
    //const vector<string>& validOptions = {}
    string getInput(const string& prompt, string type) {
        string input;
        while (true) {
            cout << prompt;
            cin >> input;
            clearInput();

            // Convert input to lowercase for case-insensitive comparison
            for (auto& c : input) c = tolower(c);
            int typeNum = -1;
            if(type == "create") {
                typeNum = 1;
            }
            else if(type == "TF") {
                typeNum = 2;
            }
            else if(type == "MCQ") {
                typeNum = 3;
            }

            switch(typeNum) {
                case 1:
                    if(input == "mcq" || input == "tf" || input == "wr") {
                        return input;
                    }
                break;
                case 2:
                    if(input == "true" || input == "false") {
                        return input;
                    }
                break;
                case 3:
                    if(isalpha(input[0])) {
                        return input;
                    }
                break;
                default:
                    return input;
            }

            cout << "[Invalid input, please try again!]" << endl;
        }
    }

    // Modified getInput function to handle integer actions
    int getActionInput(const string& prompt) {
        string input;
        int action;
        while (true) {
            cout << prompt;
            getline(cin, input);  // Use getline to avoid newline issues
            try {
                action = stoi(input);  // Convert string to integer
                return action;
            } catch (const invalid_argument&) {
                cout << "[Unknown input, please try again]" << endl;
            }
        }
    }

    // Helper function to get a validated point value for the question
    double getPointsInput() {
        double points;
        while (true) {
            cout << "Enter point value: ";
            cin >> points;
            if (!cin.fail() && points >= 0) break;
            cout << "[Invalid input, please try again!]" << endl;
            clearInput();
        }
        clearInput();
        return points;
    }

    // Function to add a new question to the end of the linked list
    void addQuestionNode(shared_ptr<Question> newQuestion) {
        if (!head) {  // If list is empty, new question is the head
            head = newQuestion;
        } else {  // Traverse to the end of the list and append the new question
            auto temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newQuestion;
        }
        questionCount++;            // Increment question count
        totalPoints += newQuestion->points;  // Add to total points
    }

    // Function to retrieve a question by its ID
    shared_ptr<Question> getQuestionById(int id) {
        auto temp = head;
        while (temp) {
            if (temp->id == id) return temp;
            temp = temp->next;
        }
        return nullptr;  // Return nullptr if question not found
    }


    // Function to create a multiple-choice question
    void createMCQ(shared_ptr<Question> question) {
        question->type = "mcq";
        cout << "Enter a question: ";
        getline(cin, question->text);

        // Collect answer choices for the MCQ
        string choice;
        for (char option = 'A'; option <= 'Z'; ++option) {
            cout << "Enter choice " << option << ": ";
            getline(cin, choice);
            if (choice == "quit()") break;
            //question->choices.push_back(string(1, option) + ". " + choice);
            question -> addChoice(option, choice);
        }

        // Get the correct answer and point value
        question->correctAnswer = getInput("Select correct answer: ", "MCQ");
        question->points = getPointsInput();
        cout << "Question saved." << endl;
    }

    // Function to create a true/false question
    void createTF(shared_ptr<Question> question) {
        question->type = "tf";
        cout << "Enter a question: ";
        getline(cin, question->text);

        // Get correct answer and point value
        question->correctAnswer = getInput("Select correct answer [true/false]: ", "TF");
        question->points = getPointsInput();
        cout << "Question saved." << endl;
    }

    // Function to create a written response question
    void createWR(shared_ptr<Question> question) {
        question->type = "wr";
        cout << "Enter a question: ";
        getline(cin, question->text);

        // Get correct answer and point value
        cout << "Type correct answer: ";
        getline(cin, question->correctAnswer);
        question->points = getPointsInput();
        cout << "Question saved." << endl;
    }

    // Function to edit an existing question's properties
    void editQuestion(shared_ptr<Question> question) {
        cout << "===============================\n";
        cout << "=== QUESTION " << question->id << " SAVED VALUES ===\n";
        cout << "===============================\n";
        cout << "1. Type: " << question->type << "\n";
        cout << "2. Question: " << question->text << "\n";

        // Display choices if question is an MCQ

        if (question->type == "mcq") {
            cout << "3. Answer choices:\n";

            ChoiceNode* temp = question -> choicesHead;
            while (temp != nullptr) {
                std::cout << temp->letter << ". " << temp->choiceText << std::endl;
                temp = temp->next;
            }
            /*
            for (const auto& choice : question->choices) {
                cout << choice << endl;
            }*/
            cout << "4. Correct answer: " << question->correctAnswer << "\n";
        } else {
            cout << "3. Correct answer: " << question->correctAnswer << "\n";
        }

        // Edit specific properties based on user input
        int option;
        do {
            cout << "Type a number to edit, or type -1 to quit: ";
            if (!(cin >> option)) { clearInput(); option = -1; }
            clearInput();
            switch (option) {
                case 1:
                    cout << "Type of question [mcq/tf/wr]: ";
                cin >> question->type;
                break;
                case 2:
                    cout << "Enter a new question: ";
                getline(cin, question->text);
                break;
                case 3:
                    if (question->type == "mcq") {
                        //question->choices.clear();
                        question -> resetAnswers();
                        // Collect answer choices for the MCQ
                        string choice;
                        for (char option = 'A'; option <= 'Z'; ++option) {
                            cout << "Enter choice " << option << ": ";
                            getline(cin, choice);
                            if (choice == "quit()") break;
                            //question->choices.push_back(string(1, option) + ". " + choice);
                            question -> addChoice(option, choice);
                        }
                    } else {
                        cout << "Enter correct answer: ";
                        getline(cin, question->correctAnswer);
                    }
                break;
                case 4:
                    if (question->type == "mcq") {
                        cout << "Select correct answer: ";
                        getline(cin, question->correctAnswer);
                    }
                break;
                default:
                    if (option != -1) cout << "[Unknown option, please try again]" << endl;
            }
        } while (option != -1);
    }

    // Function to delete a question by its ID
    void deleteQuestion(int id) {
        shared_ptr<Question> temp = head, prev = nullptr;

        // Search for the question to delete
        while (temp && temp->id != id) {
            prev = temp;
            temp = temp->next;
        }

        // Handle case where question is not found
        if (!temp) {
            cout << "[Question not found]" << endl;
            return;
        }

        // Remove question from the linked list
        if (!prev) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        totalPoints -= temp->points;
        questionCount--;
        cout << "Question " << id << " deleted." << endl;
    }

    void displayQuestion(shared_ptr<Question> question) {
        cout << "Question " << question -> id << ": " << question -> text << endl;
        if(question -> type == "mcq") {
            //if mcq, must also display the answer choices

            auto temp = question -> choicesHead;
            while (temp != nullptr) {
                std::cout << temp->letter << ". " << temp->choiceText << std::endl;
                temp = temp->next;
            }
            /*
            for(const auto& choice : question -> choices) {
                cout << choice << endl;
            }*/
        }
        string answer;
        cout << "Your answer: ";
        cin >> answer;
        // Convert input to lowercase for case-insensitive comparison
        for (auto& c : answer) c = tolower(c);
        question -> studentAnswer = answer;
        clearInput();

    }

    bool allQuestionAnswer() {
        auto current = head;  // Start at the head of the linked list
        while (current != nullptr) {
            if (current->studentAnswer.empty()) {  // Check if the answer is empty
                return false;  // Found an unanswered question, return false
            }
            current = current->next;  // Move to the next question in the list
        }
        return true;  // All questions have answers, return true
    }

    //return the next unanswered question
    shared_ptr<Question> getNextUnansweredQuestion() {
        auto temp = head;
        while (temp) {
            if (temp->studentAnswer.empty()){ return temp;}//if student answer for question is empty, return that
            temp = temp->next;//if not, then go to next node
        }
        return nullptr;
    }

public:
    void auto_createQuestion(bool correct, bool answered) {
        auto newQuestion = make_shared<Question>(1);
        newQuestion -> type = "wr";
        newQuestion -> text = "How long was the shortest war on record?";
        newQuestion -> correctAnswer = "38";
        newQuestion -> points = 100;
        if (correct && answered) {
            newQuestion -> studentAnswer = "38";
        }
        else if (answered) {
            newQuestion -> studentAnswer = "85";
        }
        addQuestionNode(newQuestion);

    }
    // Public interface for creating a question based on type
    void createQuestion() {
        auto newQuestion = make_shared<Question>(questionCount + 1);
        cout << "=== Question " << questionCount + 1 << " ===" << endl;

        string type = getInput("Type of question [mcq/tf/wr]: ", "create");//"mcq", "tf", "wr"
        if (type == "mcq") createMCQ(newQuestion);
        else if (type == "tf") createTF(newQuestion);
        else if (type == "wr") createWR(newQuestion);
        addQuestionNode(newQuestion);
    }

    // Public interface to edit an existing question
    void editQuestion() {
        int id;
        cout << "Select a question to edit, or type quit() [" << 1 << "-" << questionCount << "]: ";
        cin >> id;
        auto question = getQuestionById(id);
        if (question) editQuestion(question);
        else cout << "[Question not found]" << endl;
    }

    // Public interface to delete a question by its ID
    void deleteQuestion() {
        int id;
        cout << "Select a question to delete [" << 1 << "-" << questionCount << "]: ";
        cin >> id;
        deleteQuestion(id);
    }

    // Public interface to delete all questions
    void deleteQuestionTest() {
        head = nullptr;
        questionCount = 0;
    }


    // Displays a summary log of the quiz session
    void displaySessionLog() {
        cout << "=== SESSION LOG ===" << endl;
        cout << "Total questions: " << questionCount << endl;
        cout << fixed << setprecision(2) << "Total point values: " << totalPoints << endl;
    }

    bool promptUserEmpty(shared_ptr<Question> question) {

        do{cout << "Do you want to?\n";
            cout << "\t1. Go to next question\n";
            cout << "\t2. Jump to question\n";
            cout << "\t3. Submit\n";
            cout << "Select an action: ";
            int action;
            //getline(cin, user_answer);
            cin >> action;
            clearInput();
            // Execute action based on user's choice
            switch (action) {
                case 1:
                    // Move to the next unanswered question
                        question = getNextUnansweredQuestion();
                if (!question) {return false;}//if question still null, break loop

                displayQuestion(question);


                return true;

                case 2:
                    int id;
                cout << "Jump to question [" << 1 << "-" << questionCount << "]: ";
                cin >> id;
                question = getQuestionById(id);
                if (question) {
                    displayQuestion(question);
                    return true;
                }
                cout << "[Question not found]" << endl;

                break;
                case 3:
                    return false;
                default:
                    cout << "[Unknown input, please try again]" << endl;
            }
        }while(true);


    }

    bool promptUser(shared_ptr<Question> question) {
        string newAnswer;
    do {
        cout << "Do you want to?\n";
        cout << "\t1. Edit this Answer\n";
        cout << "\t2. Go to next question\n";
        cout << "\t3. Jump to question\n";
        cout << "\t4. Submit\n";
        cout << "Select an action: ";
        int action;
        cin >> action;
        clearInput();
        // Execute action based on user's choice
        switch (action) {
            case 1:
                cout << "Your new answer: ";
                getline(cin, newAnswer);
                // Convert input to lowercase for case-insensitive comparison
                for (auto& c : newAnswer) c = tolower(c);
                question -> studentAnswer = newAnswer;
                clearInput();
                return true;
            case 2:
                // Move to the next unanswered question
                question = getNextUnansweredQuestion();
                if (!question) {return false;}//if question still null, break loop

                displayQuestion(question);


                return true;
                break;
            case 3:
                int id;
                cout << "Jump to question [" << 1 << "-" << questionCount << "]: ";
                cin >> id;
                question = getQuestionById(id);
                if (question) {
                    displayQuestion(question);
                    return true;
                }
                cout << "[Question not found]" << endl;

                break;
            case 4:
                return false;

            default:
                cout << "[Unknown input, please try again]" << endl;
        }
    }while(true);


    }

    void conductQuiz(){
        shared_ptr<Question> question = head;
        bool cont = true;

        while(cont) {
            if(allQuestionAnswer()) {
               cout << "All questions answered. ";
            }
            if(question && question->studentAnswer.empty()) {
               cont = promptUserEmpty(question);
            }
            else {
               cont = promptUser(question);
            }
        }
    }

    void submitTest() {
        for (auto temp = head; temp; temp = temp->next) {
            if (temp->studentAnswer.empty()) {
                cout << "[Unanswered questions detected. Submit anyway? (y/n)] ";
                errorMessage = true;

            }
        }
    }

    void submit() {
        for (auto temp = head; temp; temp = temp->next) {
            if (temp->studentAnswer.empty()) {
                cout << "[Unanswered questions detected. Submit anyway? (y/n)] ";
                errorMessage = true;
                string choice;
                cin >> choice;
                if (choice == "n") return;
                else break;
            }
        }

        //double score = 0;
        cout << "\n/!\\ Assessment Complete.\n=== SESSION LOG ===" << endl;
        for (auto temp = head; temp; temp = temp->next) {
            cout << "Question " << temp->id << ": " << temp->correctAnswer << endl;
            cout << "Your answer: " << temp->studentAnswer << endl;
            if (temp->correctAnswer == temp->studentAnswer) score += temp->points;
        }
        cout << fixed << setprecision(2) << "Final score: " << score << "/" << totalPoints << endl;
    }

};

int main() {
#ifdef UNIT_TESTING
    // Running the unit tests
    cout << "***This is a Debugging Version ***" << endl << endl;
    Quiz testQuiz;

    //Unit test 1
    //check if there are zero question in the test bank
    cout << "Unit Test Case 1: Ask no question. The program should give a warning message." << endl;
    assert(!testQuiz.check1());
    cout << "\nCase 1 Passed" << endl << endl;

    //unit test 2.1
    //check for wrong answer
    cout << "Unit Test Case 2.1: Ask 1 question in the linked list. The tester enters an incorrect answer." << endl;
    testQuiz.auto_createQuestion(false, true);
    testQuiz.submit();
    assert(testQuiz.getScore() == 0);
    cout << "\nCase 2 Passed" << endl << endl;

    Quiz test2;
    //Unit test 2.2
    //check for right answer
    cout << "Unit Test Case 2.2: Ask 1 question in the linked list. The tester enters a correct answer." << endl;
    test2.auto_createQuestion(true, true);
    test2.submit();
    assert(test2.getScore() == 100);
    cout << "\nCase 2.2 Passed" << endl << endl;

    //Unit test 3
    //test if no questions are answered
    Quiz test3;
    cout << "Unit Test Case 3: Check what happens when no questions answered" << endl;
    test3.auto_createQuestion(true, false);
    test3.submitTest();
    assert(test3.errorMessage);
    cout << "\n\nCase 3 Passed" << endl << endl;

    //Unit test 4
    //test if you delete all questions
    Quiz test4;
    cout << "Unit Test Case 4: Check if you can delete all questions created" << endl;
    test4.auto_createQuestion(true, false);
    test4.deleteQuestionTest();
    assert(!test4.check2());
    cout << "\nCase 4 Passed" << endl << endl;

    cout << "***End of the Debugging Version ***" << endl << endl;



#else
    Quiz quiz;   // Create an instance of the Quiz class
    bool cont = true;

    // Main loop for quiz management
    while (cont) {
        cout << "Do you want to?\n";
        cout << "\t1. Create new question\n";
        cout << "\t2. Edit question\n";
        cout << "\t3. Delete question\n";
        cout << "\t4. Finish\n";
        cout << "Select an action: ";
        int action;
        cin >> action;
        quiz.clearInput();

        // Execute action based on user's choice
        switch (action) {
            case 1:
                quiz.createQuestion();
                break;
            case 2:
                quiz.editQuestion();
                break;
            case 3:
                quiz.deleteQuestion();
                break;
            case 4:
                quiz.displaySessionLog();
                if(quiz.check1()) {
                    cont = false;
                }
                break;
            default:
                cout << "[Unknown input, please try again]" << endl;
        }
    }
    bool test = false;
    bool loop = true;
    string user_answer = "";
    do {
        cout << "/!\\ Begin assessment? [y/n]: ";
        getline(cin, user_answer);
        if (user_answer.at(0) == 'y' || user_answer.at(0) == 'Y') {
            test = true;
            loop = false;
        }
        else if (user_answer.at(0) == 'n' || user_answer.at(0) == 'N') {
            test = false;
            loop = false;
        }
        else {
            cout << "Invalid Response" << endl;
            user_answer = "";
        }
    }while(loop);

    if(test) {
        quiz.conductQuiz();
        quiz.submit();
    }

    cout << "*** Thank you for using the testing service. Goodbye! ***" << endl;
#endif




    return 0;
}