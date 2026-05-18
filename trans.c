#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCTS 100
#define EMPTY 0

// clientData structure definition
struct clientData
{
    unsigned int acctNum;      // account number
    char lastName[15];         // account last name
    char firstName[10];        // account first name
    double balance;            // account balance

    // Added fields
    char mobile[12];           // e.g., 9876543210
    char accType[10];          // "Savings" or "Current"
    int minBalance;            // minimum allowed balance
    int transCount;            // number of transactions
};

// Global array for in-memory search / sort
struct clientData accountList[MAX_ACCTS];
int count = 0; // number of active accounts

// Function prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void loadAccounts(FILE *fPtr);          // FIX: takes FILE* parameter
void searchAccount(FILE *fPtr);         // FIX: takes FILE* parameter (was using global cfPtr)
void sortAccountsByAcct(FILE *fPtr);    // FIX: takes FILE* parameter (was using global cfPtr)
void displaySorted(void);
void printAccountDetails(const struct clientData *c);
void verifyAccountsFile(void);          // FIX: now defined at bottom

int main(int argc, char *argv[])
{
    FILE *cfPtr;               // credit.dat file pointer
    unsigned int choice;       // user's choice

    // Open binary random-access file
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        fprintf(stderr, "%s: File could not be opened.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Main menu loop
    while ((choice = enterChoice()) != 7)  // 7 = exit
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;
        case 2:
            updateRecord(cfPtr);
            break;
        case 3:
            newRecord(cfPtr);
            break;
        case 4:
            deleteRecord(cfPtr);
            break;
        case 5:
            searchAccount(cfPtr);       // FIX: pass cfPtr explicitly
            break;
        case 6:
            sortAccountsByAcct(cfPtr);  // FIX: pass cfPtr explicitly
            displaySorted();
            break;
        default:
            puts("Incorrect choice.");
            break;
        }
    }

    fclose(cfPtr);
    verifyAccountsFile();
    printf("Program ended.\n");
    return 0;
}

// Display menu and return user choice
unsigned int enterChoice(void)
{
    printf("\nTransaction Processing System\n");
    printf("1 - Store accounts to 'accounts.txt'\n");
    printf("2 - Update account balance\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Search account (number/name)\n");
    printf("6 - Sort and display accounts\n");
    printf("7 - Exit\n");
    printf("Enter choice: ");

    unsigned int choice;
    if (scanf("%u", &choice) != 1)
    {
        while (getchar() != '\n');
        return 0;
    }
    return choice;
}

// Create formatted text file accounts.txt
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client = {0, "", "", 0.0, "", "", 0, 0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File 'accounts.txt' could not be created.");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr, "%-6s%-16s%-11s%10s   Mobile        Type   MinBal  TCount\n",
            "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-6u%-16s%-11s%10.2f   %-11s %-7s  %d       %d\n",
                    client.acctNum, client.lastName, client.firstName,
                    client.balance, client.mobile, client.accType,
                    client.minBalance, client.transCount);
        }
    }

    fclose(writePtr);
    printf("Accounts written to 'accounts.txt'.\n");
}

// Load all active accounts into memory array
// FIX: was called with undeclared global cfPtr inside other functions
void loadAccounts(FILE *fPtr)
{
    rewind(fPtr);
    struct clientData client;
    count = 0;

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            accountList[count++] = client;
        }
    }
}

// FIX: added FILE *fPtr parameter — cfPtr was not in scope here before
void searchAccount(FILE *fPtr)
{
    loadAccounts(fPtr);

    int choice;
    printf("Search by: 1 - Account number, 2 - Name\n");
    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input.\n");
        return;
    }

    if (choice == 1)
    {
        unsigned int num;
        printf("Enter account number: ");
        if (scanf("%u", &num) != 1)
        {
            printf("Invalid input.\n");
            return;
        }

        for (int i = 0; i < count; i++)
        {
            if (accountList[i].acctNum == num)
            {
                printAccountDetails(&accountList[i]);
                return;
            }
        }
        printf("Account #%u not found.\n", num);
    }
    else if (choice == 2)
    {
        char name[30];
        printf("Enter first or last name: ");
        if (scanf("%29s", name) != 1)
        {
            printf("Invalid input.\n");
            return;
        }

        int found = 0;
        for (int i = 0; i < count; i++)
        {
            if (strcmp(accountList[i].firstName, name) == 0 ||
                strcmp(accountList[i].lastName,  name) == 0)
            {
                printAccountDetails(&accountList[i]);
                found = 1;
            }
        }
        if (!found)
        {
            printf("No account found with name '%s'.\n", name);
        }
    }
    else
    {
        printf("Invalid choice.\n");
    }
}

// FIX: added FILE *fPtr parameter — cfPtr was not in scope here before
void sortAccountsByAcct(FILE *fPtr)
{
    loadAccounts(fPtr);
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - 1 - i; j++)
        {
            if (accountList[j].acctNum > accountList[j + 1].acctNum)
            {
                struct clientData temp = accountList[j];
                accountList[j] = accountList[j + 1];
                accountList[j + 1] = temp;
            }
        }
    }
}

// Display sorted list
void displaySorted(void)
{
    if (count == 0)
    {
        printf("No accounts to display.\n");
        return;
    }

    printf("Sorted accounts (by account number):\n");
    for (int i = 0; i < count; i++)
    {
        printAccountDetails(&accountList[i]);
    }
}

// Print one account's details
void printAccountDetails(const struct clientData *c)
{
    printf("Acct: %u | Name: %s %s | Balance: %.2f | Mobile: %s | Type: %s | MinBal: %d | Trans: %d\n",
           c->acctNum, c->firstName, c->lastName, c->balance,
           c->mobile, c->accType, c->minBalance, c->transCount);
}

// Update balance with minimum balance check
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0, "", "", 0, 0};

    printf("Enter account to update (1-100): ");
    if (scanf("%u", &account) != 1 || account == 0 || account > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Error reading record.\n");
        return;
    }

    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printAccountDetails(&client);

    printf("Enter charge (+) or payment (-): ");
    if (scanf("%lf", &transaction) != 1)
    {
        printf("Invalid amount.\n");
        return;
    }

    double newBal = client.balance + transaction;
    if (newBal < client.minBalance)
    {
        printf("Cannot go below minimum balance of %d.\n", client.minBalance);
        return;
    }

    client.balance = newBal;
    client.transCount++;

    printf("New details:\n");
    printAccountDetails(&client);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    if (fwrite(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Error writing record.\n");
    }
}

// Create and insert new account
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, "", "", 0, 0};
    unsigned int accountNum;

    printf("Enter new account number (1-100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum == 0 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) == 1 &&
        client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
        return;
    }

    printf("Enter lastname, firstname, balance\n? ");
    if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance) != 3)
    {
        printf("Invalid data.\n");
        return;
    }
    if (client.balance < 0.0)
    {
        printf("Negative balance not allowed.\n");
        return;
    }

    printf("Enter mobile (10 digits): ");
    if (scanf("%11s", client.mobile) != 1)
    {
        printf("Invalid mobile.\n");
        return;
    }

    printf("Enter account type (Savings/Current): ");
    if (scanf("%9s", client.accType) != 1)
    {
        printf("Invalid account type.\n");
        return;
    }

    printf("Enter minimum balance: ");
    if (scanf("%d", &client.minBalance) != 1 || client.minBalance < 0)
    {
        printf("Invalid minimum balance.\n");
        return;
    }

    client.acctNum = accountNum;
    client.transCount = 0;

    fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fwrite(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Error writing record.\n");
        return;
    }

    printf("Account #%u created.\n", client.acctNum);
}

// FIX: deleteRecord was incomplete (cut off). Full implementation added.
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0, "", "", 0, 0};
    unsigned int accountNum;

    printf("Enter account number to delete (1-100): ");
    if (scanf("%u", &accountNum) != 1 || accountNum == 0 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fread(&client, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Error reading record.\n");
        return;
    }

    if (client.acctNum == EMPTY)
    {
        printf("Account #%u does not exist.\n", accountNum);
        return;
    }

    // Show account being deleted
    printf("Deleting the following account:\n");
    printAccountDetails(&client);

    // Overwrite with blank record
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    if (fwrite(&blankClient, sizeof(struct clientData), 1, fPtr) != 1)
    {
        printf("Error deleting record.\n");
        return;
    }

    printf("Account #%u deleted successfully.\n", accountNum);
}

// FIX: verifyAccountsFile was declared but never defined
void verifyAccountsFile(void)
{
    FILE *fPtr = fopen("accounts.txt", "r");
    if (fPtr == NULL)
    {
        printf("Note: 'accounts.txt' was not generated this session.\n");
        return;
    }

    char line[256];
    int lines = 0;
    while (fgets(line, sizeof(line), fPtr) != NULL)
        lines++;

    fclose(fPtr);
    printf("Verification: 'accounts.txt' exists with %d line(s) (including header).\n", lines);
}