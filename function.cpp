#include "bloomfilter.cpp"
#include <fstream>
#include <string>
#include <sstream>

struct Account
{
    string username;
    string password;
};

bool isValidPassword(string password)
{
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char c : password)
    {
        if (isupper(c))
            hasUpper = true;
        else if (islower(c))
            hasLower = true;
        else if (isdigit(c))
            hasDigit = true;
        else if (!isalnum(c))
            hasSpecial = true;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

bool isWeakPassword(BloomFilter wpassbf, string password, string weakPass_file)
{
    //Look for weak password in bloom filter first
    if (!isInBF(password, wpassbf))
        return false;

    //If the weak password is found in the bloom filter, check in the file
    ifstream weakPassFile(weakPass_file);
    if (!weakPassFile)
    {
        cout << "Unable to open " << weakPass_file << endl;
        return false;
    }
    string line;
    while (getline(weakPassFile, line))
    {
        if (line == password)
            return true;
    }
    return false;
}

vector<Account> getDatabase(string account_file)
{
    vector<Account> accs;
    ifstream ifs(account_file);
    if (!ifs)
    {
        cout << "Unable to open " << account_file << endl;
        return accs;
    }
    string line;
    int i = 0;
    Account temp;
    while (getline(ifs, line))
    {
        if (line != "")
        {
            stringstream ss {line};
            ss >> temp.username;
            ss >> temp.password;
            i++;
            accs.push_back(temp);
        }
    }
    ifs.close();
    return accs;
}

BloomFilter generateUsernameFilter(vector<Account> accs)
{
    BloomFilter bf;
    int start_prime = 31;
    GenerateFuncCoefficient(bf, start_prime);
    for (int i = 0; i < accs.size(); i++)
        addToBF(accs[i].username, bf);
    
    return bf;
}

BloomFilter generateWPassFilter(string weakPass_file)
{
    BloomFilter bf;
    int start_prime = 31;
    GenerateFuncCoefficient(bf, start_prime);
    ifstream ifs(weakPass_file);
    if (!ifs)
    {
        cout << "Unable to open " << weakPass_file << endl;
        return bf;
    }
    string line;
    while (getline(ifs, line))
        addToBF(line, bf);
    ifs.close();

    return bf;
}

bool registerAccount(Account account, vector<Account> &accs , string account_file, string weakPass_file, BloomFilter &accbf, BloomFilter wpassbf)
{
    // Check username and password constraints
    if (account.username.length() <= 5 || account.username.length() >= 10)
    {
        cout << "Username must be between 6 and 9 characters long.\n";
        return false;
    }

    if (account.username.find(' ') != string::npos)
    {
        cout << "Username must not contain spaces.\n";
        return false;
    }

    if (account.password.length() <= 10 || account.password.length() >= 20)
    {
        cout << "Password must be between 11 and 19 characters long.\n";
        return false;
    }

    if (account.password.find(' ') != string::npos)
    {
        cout << "Password must not contain spaces.\n";
        return false;
    }

    if (account.password == account.username)
    {
        cout << "Password cannot be the same as the username.\n";
        return false;
    }

    if (!isValidPassword(account.password))
    {
        cout << "Password must include uppercase, lowercase, numbers, and special characters.\n";
        return false;
    }

    if (isWeakPassword(wpassbf, account.password, weakPass_file))
    {
        cout << "Password is too weak.\n";
        return false;
    }

    // Check if the username is not the same as any registered usernames
    if (isInBF(account.username, accbf))   //First, check if the username exist in bloom filter
    {   //If yes, the username probably exists in the database. Linear search the database
        for (int i = 0; i < accs.size(); i++)
        {
            if (accs[i].username == account.username)
            {
                cout << "Username " << account.username << " is already registered." << endl;
                return false;
            }
        }
    }

    // If all constraints are met, store the account in the Database
    ofstream ofs(account_file, ios::app);
    if (!ofs)
    {
        cout << "Unable to open " << account_file << endl;
        return false;
    }
    ofs << account.username << " " << account.password << endl;
    ofs.close();

    //Update vector
    accs.push_back(account);

    //Update bloom filter
    addToBF(account.username, accbf);

    return true;
}

bool multiplyRegistration(vector<Account> &accs, string signup_file, string account_file, string fail_file, string weakPass_file , BloomFilter &accbf, BloomFilter wpassbf)
{
    ifstream ifs(signup_file);
    if (!ifs)
    {
        cout << "Unable to open " << signup_file << endl;
        return false;
    }
    ofstream ofs(fail_file, ios::app);
    if (!ofs)
    {
        cout << "Unable to open " << fail_file << endl;
        return false;
    }
    string line;
    Account temp;
    while (getline(ifs, line))
    {
        stringstream ss {line};
        ss >> temp.username;
        ss >> temp.password;
        if (!registerAccount(temp, accs , account_file, weakPass_file, accbf, wpassbf)) //If the registration fails, output the account to fail.txt
            ofs << temp.username << " " << temp.password << endl;
        else
            cout << "Account " << temp.username << " successfully registered." << endl;
    }
    ifs.close();
    ofs.close();
    return true;
}

bool logIn(Account acc, vector<Account> accs, BloomFilter bf)
{
    // Check username constraints
    if (acc.username.length() <= 5 || acc.username.length() >= 10)
    {
        cout << "Username must be between 6 and 9 characters long.\n";
        return false;
    }

    if (acc.username.find(' ') != string::npos)
    {
        cout << "Username must not contain spaces.\n";
        return false;
    }
    
    //Look the username up in the bloom filter
    if (!isInBF(acc.username, bf))  //If it doesn't exist in bf, it 100% doesn't exist in database
    {
        cout << "Username does not exist." << endl;
        return false;
    }

    //If there is a chance that the username exist, (linear) search the database
    for (int i = 0; i < accs.size(); i++)
    {
        if (accs[i].username == acc.username)
        {
            if (accs[i].password == acc.password)
            {
                cout << "Login sucessful." << endl;
                return true;
            }
            else
            {
                cout << "Password does not match." << endl;
                return false;
            }
        }
    }

    //If username wasn't found in database, return false
    cout << "Username does not exist." << endl;
    return false;
}

bool changePassword(Account &acc, vector<Account> &accs , string newpassword , string account_file, string weakPass_file, BloomFilter wpassbf)
{
    if (newpassword.length() <= 10 || newpassword.length() >= 20)
    {
        cout << "Password must be between 11 and 19 characters long." << endl;
        return false;
    }

    if (newpassword.find(' ') != string::npos)
    {
        cout << "Password must not contain spaces.\n";
        return false;
    }

    if (newpassword == acc.username)
    {
        cout << "Password cannot be the same as the username." << endl;
        return false;
    }

    if (newpassword == acc.password)
    {
        cout << "New passowrd cannot be the same as old password." << endl;
        return false;
    }

    if (!isValidPassword(newpassword))
    {
        cout << "Password must include uppercase, lowercase, numbers, and special characters." << endl;
        return false;
    }

    if (isWeakPassword(wpassbf, newpassword, weakPass_file))
    {
        cout << "Password is too weak." << endl;
        return false;
    }

    acc.password = newpassword;
    ofstream ofs(account_file);
    if (!ofs)
    {
        cout << "Unable to open " << account_file << endl;
        return false;
    }
    for (int i = 0; i < accs.size(); i++)
    {
        //Change passowrd in the database
        if (accs[i].username == acc.username)
            accs[i].password = acc.password;
        //Update the database file
        ofs << accs[i].username << " " << accs[i].password << endl;
    }
    ofs.close();
    return true;
}
