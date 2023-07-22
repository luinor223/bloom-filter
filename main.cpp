#include "function.cpp"

int main()
{
    string account_file = "Account.txt";
    string weakpass_file = "WeakPass.txt";
    string signup_file = "SignUp.txt";
    string fail_file = "Fail.txt";
    vector<Account> accs = getDatabase(account_file);
    BloomFilter bf = generateBloomFilter(accs);
    bool menu = true;
    Account acc;
    bool loggedIn = false;
    while (menu)
    {
        int choice;
        if (!loggedIn)
        {
            cout << "==== Account Management System ====" << endl;
            cout << "1. Registration" << endl;
            cout << "2. Multiple Registrations" << endl;
            cout << "3. Login" << endl;
            cout << "4. Quit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();
            switch (choice)
            {
                case 1:
                    cout << "---Register---" << endl;
                    cout << "Enter username: ";
                    getline(cin, acc.username);
                    cout << "Enter password: ";
                    getline(cin, acc.password);
                    if (registerAccount(acc, accs, account_file, weakpass_file, bf))
                        cout << "Registration succesful." << endl;
                    else
                        cout << "Registration failed" << endl;
                    cout << endl;
                    acc.username = "";
                    acc.password = "";
                    break;
                case 2:
                    cout << "---Multiple Registrations---" << endl;
                    multiplyRegistration(accs, signup_file, account_file, fail_file, weakpass_file, bf);
                    cout << "Done." << endl << endl;
                    break;
                case 3:
                    cout << "---Login---" << endl;
                    cout << "Enter username: ";
                    getline(cin, acc.username);
                    cout << "Enter passowrd: ";
                    getline(cin, acc.password);
                    if (logIn(acc, accs, bf))
                    {
                        loggedIn = true;
                        cout << "Login Successful." << endl;
                    }
                    else
                    {
                        cout << "Login Failed." << endl;
                        acc.username = "";
                        acc.password = "";
                    }
                    cout << endl;
                    break;
                case 4:
                    menu = false;
                    break;
                default:
                    cout << "Invalid command!" << endl << endl;
                    break;
            }
        }

        else
        {
            cout << "==== Welcome, " << acc.username << "! ====" << endl;
            cout << "1. Change Password" << endl;
            cout << "2. Log out" << endl;
            cout << "3. Quit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore(); 
            string newpassword;
            switch (choice)
            {
                case 1:
                    cout << "Enter your new password: ";
                    getline(cin, newpassword);
                    if (changePassword(acc, accs, newpassword, account_file, weakpass_file))
                        cout << "Password changed successfully." << endl;
                    else
                        cout << "Failed to change password." << endl;
                    cout << endl;
                    break;
                case 2:
                    cout << "Logging out..." << endl << endl;
                    loggedIn = false;
                    acc.username = "";
                    acc.password = "";
                    break;
                case 3:
                    cout << "Quiting..." << endl;
                    menu = false;
                    break;
                default:
                    cout << "Invalid command!" << endl << endl;
                    break;
            }
        }
    }

    delete[] bf.FuncCoef;
    return 0;
}