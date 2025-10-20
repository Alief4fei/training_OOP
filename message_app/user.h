#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm> // Untuk std::max, std::remove_if
#include <limits>    // Untuk cin.ignore
#include <sstream>   // Untuk std::to_string

using namespace std;

// Forward declaration
class User;

// Deklarasi fungsi helper
inline bool save_users(const std::vector<User>& users, const std::string& filename);
inline std::vector<User> load_users(const std::string& filename);
inline int max_user_id(const std::vector<User>& users);

// --- FUNGSI SERIALISASI STRING (Helper Global) ---
inline void write_string(std::ostream& os, const std::string& str) {
    size_t len = str.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        os.write(str.data(), len);
    }
}

inline void read_string(std::istream& is, std::string& str) {
    size_t len = 0;
    if (!is.read(reinterpret_cast<char*>(&len), sizeof(len))) {
        str.clear();
        return; 
    }
    if (len > 0) {
        str.resize(len);
        if (!is.read(str.data(), len)) {
            str.clear();
        }
    } else {
        str.clear();
    }
}
// ---------------------------------------------------

class User{
private:
    string name;
    string phone;
    int loginState;
    int id; 
    static inline int sequenceCounter = 100; 
    vector<User> contact;

    // Helper untuk membuat nama file kontak yang konsisten
    string getContactFilename() const {
        return to_string(this->id) + "_" + this->phone + "_contact_data.txt";
    }

public:
    // --- FUNGSI SERIALISASI MANUAL ---
    void write_to_stream(std::ostream& os) const {
        write_string(os, name);
        write_string(os, phone);
        os.write(reinterpret_cast<const char*>(&loginState), sizeof(loginState));
        os.write(reinterpret_cast<const char*>(&id), sizeof(id));
    }

    void read_from_stream(std::istream& is) {
        read_string(is, name);
        read_string(is, phone);
        if (!is.read(reinterpret_cast<char*>(&loginState), sizeof(loginState))) return;
        if (!is.read(reinterpret_cast<char*>(&id), sizeof(id))) return;
        
        contact.clear();
    }
    // ----------------------------------------
private:
   
    bool validateContact(string phone, vector<User> users){
        bool isRegistered = false;
        
        for(const auto& u : users){ 
            if(u.getPhone() == phone){
                isRegistered = true;
                break;
            }
        }

        if(!isRegistered){
            cout << "Phone number is NOT registered in the system." << endl;
            return true; // GAGAL (karena tidak terdaftar)
        }
        
        string contact_filename = getContactFilename(); 
        contact = load_users(contact_filename); 
        
        for(const auto& c : contact){ 
            if(c.getPhone() == phone){
                cout << "Phone number already exists in your contact list!" << endl;
                return true; // GAGAL (karena duplikat)
            }
        }

        cout << "Contact validation successful (registered & not duplicate)." << endl;
        return false; // SUKSES
    }

    bool addContact(User user){
        contact.push_back(user);
        cout << user.name << " is successfully added to contacts." << endl;
        string contact_filename = getContactFilename(); 
        return save_users(contact, contact_filename);
    }

public:
    // Konstruktor Default
    User(){
        contact = vector<User>();
        id = 0; 
    }

    // Konstruktor untuk Register User Baru
    User(string name, string phone): name(name), phone(phone) {
        this->id = sequenceCounter++; 
        loginState = 0;
        contact = vector<User>();
    }

    void addContactPrompt(vector<User> users){
        string name, phone;
        while(true){
            cout << "Add new contact name: ";
            getline(cin >> ws, name); // Mengizinkan spasi
            cout << "Add " << name << " phone: ";
            cin >> phone;
            
            if(validateContact(phone, users)){
                char choice;
                cout << "Do you want to Repeat adding contact? (y/n (will exit) ): ";
                cin >> choice;
                if(choice == 'n' || choice == 'N') {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    return; 
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue; 
            }
            break; 
        }
        
        User newContact; 
        newContact.setName(name);
        newContact.setPhone(phone);
        
        addContact(newContact);
        cout << endl;
    }

    void printContact(){
        int i = 1;
        cout << "Contact List" << endl;
        string contact_filename = getContactFilename();
        contact = load_users(contact_filename);
        if(contact.size() == 0){
            cout << "No contact found!" << endl;
            return;
        }
        for(const auto& user : contact){
            cout << i++ << ". " << user.name << ", " << user.phone << endl;
        }
        cout << endl;
    }

    User seekContact(string query){
        string contact_filename = getContactFilename();
        contact = load_users(contact_filename);
        
        for(const auto& user : contact){
            if(user.getPhone() == query || user.getName() == query){
                return user; 
            }
        }
        return User(); 
    }

    void updateContact(const User& oldContact, const User& newContact){
        string contact_filename = getContactFilename();
        contact = load_users(contact_filename);
        
        bool found = false;
        for(auto &user: contact){
            if(user.getName() == oldContact.getName() && user.getPhone() == oldContact.getPhone()){
                user.setName(newContact.getName());
                user.setPhone(newContact.getPhone());
                found = true;
                break;
            }
        }

        if(found){
            if (save_users(contact, contact_filename)) {
                cout << "Contact has been updated and saved successfully!" << endl;
            } else {
                cout << "ERROR: Failed to save updated contact list!" << endl;
            }
        } else {
            cout << "Contact is not found in the list (using old name/phone as key)!" << endl;
        }
    }

    void deleteContact(const string& query) {
        string contact_filename = getContactFilename();
        contact = load_users(contact_filename);

        bool found = false;
        
        auto it = std::remove_if(contact.begin(), contact.end(), 
            [&query, &found](const User& u) {
                if (u.getName() == query || u.getPhone() == query) {
                    found = true;
                    return true; 
                }
                return false;
            });

        contact.erase(it, contact.end());

        if(found){
            if (save_users(contact, contact_filename)) {
                cout << "Contact matching '" << query << "' has been deleted and saved successfully!" << endl;
            } else {
                cout << "ERROR: Failed to save updated contact list after deletion!" << endl;
            }
        } else {
            cout << "Contact matching '" << query << "' is not found in the list!" << endl;
        }
    }

    void deleteContactPrompt(){
        string query;
        cout << "\n--- DELETE CONTACT ---" << endl;
        cout << "Enter contact name or phone number to delete: ";
        getline(cin >> ws, query); 
        
        deleteContact(query);
        cout << endl;
    }


    // --- GETTER & SETTER ---
    string getName() const {return name;}
    string getPhone() const {return phone;}
    int getThisId() const {return id;}
    void setName(string name){this->name = name;}
    void setPhone(string phone){this->phone = phone;}
    int getLoginState() const {return loginState;}
    void setLoginState(int state){ loginState = state;}
    static int getId(){return sequenceCounter;}
    static void setNextId(int nextId){ sequenceCounter = nextId;}
    static void resetId(){
        sequenceCounter = 100;
    }
};

// --------------------------------------------------------
// IMPLEMENTASI FUNGSI GLOBAL save_users dan load_users
// --------------------------------------------------------

inline bool save_users(const std::vector<User>& users, const std::string& filename) { 
    std::ofstream ofs(filename, std::ios::out | std::ios::trunc); 
    if (!ofs.is_open()) {
        cerr << "Error: Failed to open file for writing: " << filename << endl;
        return false;
    }

    size_t count = users.size();
    ofs.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& user : users) {
        user.write_to_stream(ofs);
    }
    
    return ofs.good();
}

inline std::vector<User> load_users(const std::string& filename) {
    std::vector<User> users;
    std::ifstream ifs(filename, std::ios::in);
    if (!ifs.is_open()) return users; 

    size_t count = 0;
    if (!ifs.read(reinterpret_cast<char*>(&count), sizeof(count))) return users;
    
    for (size_t i = 0; i < count; ++i) {
        User u;
        u.read_from_stream(ifs);
        if (!ifs.good()) {
            std::cerr << "Warning: failed to read user data." << std::endl;
            break;
        }
        users.push_back(u);
    }
    return users;
}

inline int max_user_id(const std::vector<User>& users) {
    int maxid = 0;
    for (const auto& u : users) {
        maxid = std::max(maxid, u.getThisId());
    }
    return maxid;
}