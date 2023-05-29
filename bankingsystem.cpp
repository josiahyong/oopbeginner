#include<iostream>
#include<fstream>
#include<cstdlib>
#include<map> //store a list of accounts with acc no. attached so we can lookup

#define MINIMUM_DEPOSIT 50

class CannotWithdraw{}; //deal with exception when balance lesser than withdraw amount
class IdNoExist{};  //deal with exceptions for when id does not exist in the bank

class Account{
    private:
        long acc_id;
        float balance;
        std::string fname;
        std::string lname;
    public:
        static long next_acc_id;
        Account(){} //default constructor
        Account(std::string fn, std::string ln, float balance);
        void withdraw(float amount);
        void deposit(float amount);
        long getAccID(){return acc_id;}
        static long getNextAccID(){return next_acc_id;}
        static void setLastAccID(long id);
        float getBalance(){return balance;}
        std::string getFirstName(){return fname;}
        std::string getLastName(){return lname;}
        friend std::ifstream & operator>>(std::ifstream &ifs, Account &a);
        friend std::ofstream & operator<<(std::ofstream &ofs, Account &a);
        friend std::ostream & operator<<(std::ostream &o, Account &a);
};

class Bank{
    private:
        //map of accounts with their ids
        std::map<long,Account>accounts_list; //(id, Account)
    public:
        Bank();
        Account OpenAcc(std::string fn, std::string ln, float bal);
        Account CheckBalance(long id);
        Account Deposit(long id, float bal);
        Account Withdraw(long id,float bal);
        void CloseAccount(long id);
        void ShowAllAccounts();
        void CheckValidID(long id);
        ~Bank(); //destructor
};

int main()
{
    Bank b;
    Account a;
    int flag=1;
    int choice; std::string fn; std::string ln; float bal; long id; float amount; 
    std::cout<<"\n**WELCOME TO NO.1 BANK**"<<std::endl;
    

    do{
        std::cout<<"\n\t 0: Open Account"<<std::endl;
        std::cout<<"\t 1: Check Balance"<<std::endl;
        std::cout<<"\t 2: Deposit"<<std::endl;
        std::cout<<"\t 3: Withdraw"<<std::endl;
        std::cout<<"\t 4: Close Account"<<std::endl;
        std::cout<<"\t 5: Show All Accounts"<<std::endl;
        std::cout<<"\t 6: Exit"<<std::endl;
        std::cout<<"\nEnter your choice: ";

        std::cin>>choice;

        try{
            if(std::cin.fail())
            {
                throw 1;
            }
        }
        catch(int e){
            std::cout<<"Invalid input!"<<std::endl;
            break;
        }
        
        switch(choice){
            case 0: //Open Account
                std::cout<<"\n Enter first name: ";
                std::cin>>fn;
                std::cout<<"\n Enter last name: ";
                std::cin>>ln;
                std::cout<<"\n Enter initial balance: ";
                std::cin>>bal;
                a=b.OpenAcc(fn,ln,bal);
                std::cout<<"Account Created!"<<std::endl;
                std::cout<<a;
                break;
            case 1: //Check Balance
                std::cout<<"Enter Account ID: "; 
                std::cin>>id;
                try{
                    a=b.CheckBalance(id);
                }
                catch(IdNoExist){
                    std::cout<<"Account ID does not exist!"<<std::endl;
                    break;
                }
                std::cout<<"Here are the details of your account:\n";
                std::cout<<a;
                break;
            case 2: //Deposit
                std::cout<<"Enter Account ID: "; 
                std::cin>>id;
                try{
                    b.CheckValidID(id);
                }
                catch(IdNoExist){
                    std::cout<<"Account ID does not exist!"<<std::endl;
                    break;
                }
                std::cout<<"\nEnter the amount you wish to deposit: ";
                std::cin>>amount;
                if(amount<MINIMUM_DEPOSIT){
                    std::cout<<"Minimum amount is $50!"<<std::endl;
                    break;
                }
                a=b.Deposit(id,amount);
                std::cout<<"Amount Deposited!\n";
                std::cout<<a;
                break;
            case 3: //Withdraw
                std::cout<<"Enter Account ID: ";
                std::cin>>id;
                try{ 
                    b.CheckValidID(id);
                }
                catch(IdNoExist){
                    std::cout<<"Account ID does not exist!"<<std::endl;
                    break;
                }
                std::cout<<"\nEnter amount you wish to withdraw: ";
                std::cin>>amount;
                //throw error if amount to withdraw is more than existing amount in account
                try{
                    a=b.Withdraw(id,amount);
                }
                catch(CannotWithdraw){
                    std::cout<<"\nNot enough funds to withdraw!"<<std::endl;
                    break;
                }
                std::cout<<"Amount Withdrawn!\n";
                std::cout<<a;
                break;
            case 4: //Close Account
                std::cout<<"Enter Account ID: ";
                std::cin>>id;
                try{
                    b.CheckValidID(id);
                }
                catch(IdNoExist){
                    std::cout<<"Account ID does not exist!"<<std::endl;
                    break;
                }
                b.CloseAccount(id);
                std::cout<<"Account Closed!"<<std::endl;
                break;
            case 5: //Show All Accounts
                b.ShowAllAccounts();
                break;
            case 6: //Exit
                break;
            default: 
                std::cout<<"\n\tInvalid input!"<<std::endl;
                break;
        }
    }while(choice!=6);


    return 0;
}

long Account::next_acc_id=0; //keep track of next available id to assign to new account

std::ofstream & operator<<(std::ofstream &ofs, Account &a)
{
    ofs<<a.acc_id<<std::endl;
    ofs<<a.fname<<std::endl;
    ofs<<a.lname<<std::endl;
    ofs<<a.balance<<std::endl;
    return ofs;
}
std::ifstream & operator>>(std::ifstream &ifs, Account &a)
{
    ifs>>a.acc_id>>a.fname>>a.lname>>a.balance;
    return ifs;
}
std::ostream & operator<<(std::ostream &o, Account &a)
{
    o<<"\nAccount ID: "<<a.acc_id<<"\nFirst Name: "<<a.fname<<"\nLast Name: "<<a.lname<<"\nCurrent Balance: "<<a.balance<<std::endl;
    return o;
}

Account::Account(std::string fn, std::string ln, float bal)
{   
    next_acc_id++;
    fname=fn;
    lname=ln;
    balance=bal;
    acc_id=next_acc_id;
}

void Account::setLastAccID(long id)
{
    next_acc_id=id;
}

void Account::deposit(float amount)
{
    balance+=amount;
}

void Account::withdraw(float amount)
{
    if(balance-amount<0){
        throw CannotWithdraw();
    }
    balance-=amount;
}

Bank::Bank()
{
    Account acc;
    std::ifstream infile("Accounts.txt");
    if(!infile){
        return;
    }
    while(!infile.eof())
    {
        infile>>acc;
        accounts_list.insert(std::pair<long,Account>(acc.getAccID(),acc));
    }
    Account::setLastAccID(acc.getAccID());
    infile.close();
}

Bank::~Bank()
{
    std::ofstream outfile;
    outfile.open("Accounts.txt",std::ios::trunc);
    std::map<long,Account>::iterator itr;
    for(itr=accounts_list.begin();itr!=accounts_list.end();itr++)
    {
        outfile<<itr->second;
    }
    outfile.close();
}

Account Bank::CheckBalance(long id)
{   
    std::map<long,Account>::iterator itr=accounts_list.find(id);
    if(itr==accounts_list.end()){
        throw IdNoExist();
    }
    return itr->second;
}


Account Bank::OpenAcc(std::string fn, std::string ln, float bal)
{
    Account new_acc(fn,ln,bal);
    std::ofstream outfile;
    outfile.open("Accounts.txt",std::ios::trunc);
    //write all the accounts back into the file first
    accounts_list.insert(std::pair<long,Account>(new_acc.getAccID(),new_acc));
    std::map<long,Account>::iterator itr;
    for(itr=accounts_list.begin();itr!=accounts_list.end();itr++)
    {
        outfile<<itr->second;
    }
    outfile.close();
    return new_acc;
}

Account Bank::Deposit(long id,float bal)
{
    std::map<long,Account>::iterator itr=accounts_list.find(id);
    (itr->second).deposit(bal);
    return itr->second;
}

Account Bank::Withdraw(long id, float bal)
{
    std::map<long,Account>::iterator itr=accounts_list.find(id);
    (itr->second).withdraw(bal);
    return itr->second;
}

void Bank::CloseAccount(long id)
{
    std::map<long,Account>::iterator itr=accounts_list.find(id);
    accounts_list.erase(itr);
}
void Bank::ShowAllAccounts()
{
    std::map<long,Account>::iterator itr;
    for(itr=accounts_list.begin();itr!=accounts_list.end();itr++)
    {
        std::cout<<itr->second;
    }
}


void Bank::CheckValidID(long id)
{
    std::map<long,Account>::iterator itr=accounts_list.find(id);
    if(itr==accounts_list.end())
    {
        throw IdNoExist();
    }
}

