#include <iostream>
#include <limits> 
#include "user.h" 

using namespace std;

enum PrimaryPrompt{LOGIN, REGISTER, EXIT, MAIN_PROMPT};
enum subUserPrompt{USER_VERIFICATION, LOGIN_MENU};
enum featurePrompt{WHOAMI, LIST_CONTACT, ADD_CONTACT, SEEK_CONTACT, DELETE_CONTACT, LOGOUT, LOGIN_MENU_PROMPT}; 

void editPrompt(User &other){
    cout << "Do you want to edit contact with name: " << other.getName() << " and phone: " << other.getPhone() << " ? (y/n): ";
    char choice;
    cin >> choice;
    if(choice == 'n' || choice == 'N') {
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        return; 
    }
    
    string name, phone;
    cout << "Enter updated name: "; 
    getline(cin >> ws, name); 
    cout << "Enter updated phone: "; 
    cin >> phone;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    other.setName(name);
    other.setPhone(phone);

    cout << "Contact info updated locally. Will be saved when main user updates contact list." << endl;
    return;
}

int main() {
    PrimaryPrompt prompt = MAIN_PROMPT;
    subUserPrompt sub_login_prompt = USER_VERIFICATION;
    featurePrompt login_menu_prompt = LOGIN_MENU_PROMPT;
    
    const string serialname = "user_data.txt"; 
    
    vector<User> users = load_users(serialname);
    
    if(users.size() > 0){
        int maxid = max_user_id(users);
        User::setNextId(maxid + 1); 
        cout<< "Next available user ID will be: " << User::getId() << endl;
    } else {
        User::resetId(); 
    }

    User user; 
    
    while (true) {
        switch (prompt) {
            case MAIN_PROMPT:
                cout << "\n--- MAIN MENU ---" << endl;
                cout << "1. Login" << endl;
                cout << "2. Register" << endl;
                cout << "3. Exit" << endl;
                int choice;
                if (!(cin >> choice)) {
                    cout << "Invalid input. Please enter a number." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    prompt = MAIN_PROMPT;
                    break;
                }
                if (choice < 1 || choice > 3) {
                     cout << "Invalid choice." << endl;
                     prompt = MAIN_PROMPT;
                     break;
                }
                prompt = static_cast<PrimaryPrompt>(choice - 1);
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                break;
            case LOGIN:{
                switch (sub_login_prompt){
                    case USER_VERIFICATION:{
                        string name, phone;
                        cout << "\n--- LOGIN ---" << endl;
                        cout << "Name: "; 
                        getline(cin >> ws, name); 
                        cout << "Phone: "; 
                        cin >> phone;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

                        bool isFound = false;
                        for(const auto& u: users){
                            if((u.getName() == name) && (phone == u.getPhone())){
                                user = u; 
                                isFound = true;
                                break;
                            }
                        }

                        if(!isFound){
                            cout << "Entered info is not recognized!" << endl;
                            prompt = MAIN_PROMPT;
                            sub_login_prompt = USER_VERIFICATION;
                        } else {
                            if (user.getLoginState() == 1) {
                                cout << "User already logged in!" << endl;
                            }
                            user.setLoginState(1); 
                            
                            for(auto &u: users){
                                if(u.getThisId() == user.getThisId()){
                                    u.setLoginState(1);
                                    break;
                                }
                            }

                            cout << "Login successful!" << endl;
                            sub_login_prompt = LOGIN_MENU;
                            prompt = LOGIN;
                        }
                        break;
                    }
                    case LOGIN_MENU:{
                        switch(login_menu_prompt){
                            case LOGIN_MENU_PROMPT:
                                cout << "\n--- USER MENU (" << user.getName() << ") ---" << endl;
                                cout << "1. WHO AM I" << endl;
                                cout << "2. List contact" << endl;
                                cout << "3. Add contact" << endl;
                                cout << "4. Seek & Edit contact" << endl;
                                cout << "5. Delete contact" << endl; 
                                cout << "6. Logout" << endl;         
                                
                                if (!(cin >> choice)) {
                                    cout << "Invalid input. Please enter a number." << endl;
                                    cin.clear();
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    login_menu_prompt = LOGIN_MENU_PROMPT;
                                    break;
                                }
                                if (choice < 1 || choice > 6) { 
                                     cout << "Invalid choice." << endl;
                                     login_menu_prompt = LOGIN_MENU_PROMPT;
                                     break;
                                }
                                login_menu_prompt = static_cast<featurePrompt>(choice - 1);
                                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                                break;
                            case WHOAMI:
                                cout << "\n--- WHO AM I ---" << endl;
                                cout << "ID: " << user.getThisId() << endl;
                                cout << "Name: " << user.getName() << endl;
                                cout << "Phone: " << user.getPhone() << endl;
                                login_menu_prompt = LOGIN_MENU_PROMPT;
                                cout << endl;
                                break;
                            case LIST_CONTACT:
                                cout << "\n--- LIST CONTACT ---" << endl;
                                user.printContact();
                                login_menu_prompt = LOGIN_MENU_PROMPT;
                                break;
                            case ADD_CONTACT:
                                cout << "\n--- ADD CONTACT ---" << endl;
                                user.addContactPrompt(users);
                                login_menu_prompt = LOGIN_MENU_PROMPT;
                                break;
                            case SEEK_CONTACT:{
                                string query;
                                cout << "\n--- SEEK & EDIT CONTACT ---" << endl;
                                cout << "Please enter phone number / name to search: ";
                                getline(cin >> ws, query); 
                                
                                User foundUser = user.seekContact(query);
                                
                                if(foundUser.getThisId() != 0 || foundUser.getName() != "" ){
                                    cout << "Contact found: " << foundUser.getName() << ", " << foundUser.getPhone() << endl;
                                    
                                    User oldContact = foundUser; 
                                    
                                    editPrompt(foundUser); 
                                    
                                    user.updateContact(oldContact, foundUser); 
                                } else {
                                    cout << "Contact matching '" << query << "' is not found." << endl;
                                }
                                cout << endl;
                                login_menu_prompt = LOGIN_MENU_PROMPT;
                                break;
                            }
                            case DELETE_CONTACT: 
                                user.deleteContactPrompt();
                                login_menu_prompt = LOGIN_MENU_PROMPT;
                                break;

                            case LOGOUT:
                                for(auto &u: users){
                                    if(u.getThisId() == user.getThisId()){
                                        u.setLoginState(0);
                                        break;
                                    }
                                }

                                prompt = MAIN_PROMPT;
                                sub_login_prompt = USER_VERIFICATION;
                                login_menu_prompt = LOGIN_MENU_PROMPT;
                                user = User(); 
                                cout << "\nYou have been logged out!" << endl;
                                break;
                        }
                    }
                }
                break;
            }
            case REGISTER:{
                string name, phone;
                cout << "\n--- REGISTER ---" << endl;
                cout << "Name: "; 
                getline(cin >> ws, name); 
                cout << "Phone: "; 
                cin >> phone;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                
                bool isFound = false;
                for(const auto& u: users){
                    if( (phone == u.getPhone())){
                        isFound = true;
                        break;
                    }
                }

                if(!isFound){
                    User newUser(name, phone); 
                    
                    users.push_back(newUser); 
                    user = newUser; 
                    user.setLoginState(1); 
                    
                    cout << "Registration is successful! ID: " << newUser.getThisId() << endl;
                    
                    prompt = LOGIN;
                    sub_login_prompt = LOGIN_MENU;
                    break;
                } else {
                    cout << "Phone number already exists! Returning to main menu." << endl;
                    prompt = MAIN_PROMPT;
                }
                break;

            }
            case EXIT:
                cout << "\n--- EXITING PROGRAM ---" << endl;
                if(users.size()>0){
                    if(save_users(users, serialname)) {
                        cout << "User data saved successfully." << endl;
                    } else {
                        cout << "ERROR: Failed to save user data!" << endl;
                    }
                } else {
                    cout << "No user data to save." << endl;
                }
                cout << "Goodbye!" << endl;
                return 0; 
        }
    }
    return 0;
}