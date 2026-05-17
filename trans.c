// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_RECORDS 100
#define DATA_FILE "credit.dat"
#define TEXT_FILE "accounts.txt"

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void displayAccounts(FILE *fPtr);
void searchRecord(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    // fopen opens the file; exits if file cannot be opened
    if ((cfPtr = fopen(DATA_FILE, "rb+")) == NULL)
    {
        // Try creating the file if it doesn't exist
        if ((cfPtr = fopen(DATA_FILE, "wb+")) == NULL)
        {
            printf("%s: File could not be opened or created.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        else
        {
            struct clientData blankClient = {0, "", "", 0.0};
            for (unsigned int i = 1; i <= MAX_RECORDS; ++i)
            {
                if (fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr) != 1)
                {
                    printf("Error writing to file during initialization.\n");
                    exit(EXIT_FAILURE);
                }
            }
            rewind(cfPtr); // sets pointer to beginning of file
        }
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        // create text file from record file
        case 1:
            textFile(cfPtr);
            break;
        // update record
        case 2:
            updateRecord(cfPtr);
            break;
        // create record
        case 3:
            newRecord(cfPtr);
            break;
        // delete existing record
        case 4:
            deleteRecord(cfPtr);
            break;
        // display all accounts
        case 5:
            displayAccounts(cfPtr);
            break;
        // search account by last name
        case 6:
            searchRecord(cfPtr);
            break;
        // display if user does not select valid choice
        default:
            puts("Incorrect choice");
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
} // end main

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    int result;     // used to test whether fread read any bytes
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};

    // fopen opens the file; exits if file cannot be opened
    if ((writePtr = fopen(TEXT_FILE, "w")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (!feof(readPtr))
        {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);

            // write single record to text file
            if (result != 0 && client.acctNum != 0)
            {
                fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                        client.balance);
            } // end if
        }     // end while

        fclose(writePtr); // fclose closes the file
    }                     // end else
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    // create clientData with no information
    struct clientData client = {0, "", "", 0.0};

    // obtain number of account to update
    printf("Enter account to update ( 1 - %u ): ", MAX_RECORDS);
    if (scanf("%u", &account) != 1)
    {
        puts("Invalid input.");
        { int c; while ((c = getchar()) != '\n' && c != EOF); } // clear buffer safely
        return;
    }
    if (account < 1 || account > MAX_RECORDS)
    {
        printf("Account number must be between 1 and %u.\n", MAX_RECORDS);
        return;
    }

    // move file pointer to correct record in file
    if (fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET) != 0)
    {
        puts("Error seeking to record.");
        return;
    }
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account does not exist
    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
    }
    else
    { // update record
        printf("%-6u%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // request transaction amount from user
        printf("%s", "Enter charge ( + ) or payment ( - ): ");
        if (scanf("%lf", &transaction) != 1)
        {
            puts("Invalid transaction amount.");
            { int c; while ((c = getchar()) != '\n' && c != EOF); } // clear buffer safely
            return;
        }
        client.balance += transaction; // update record balance

        printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // move file pointer to correct record in file
        // move back by 1 record length
        if (fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR) != 0)
        {
            puts("Error seeking to record.");
            return;
        }
        // write updated record over old record in file
        if (fwrite(&client, sizeof(struct clientData), 1, fPtr) != 1)
        {
            puts("Error writing updated record to file.");
        }
    } // end else
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                       // stores record read from file
    struct clientData blankClient = {0, "", "", 0}; // blank client
    unsigned int accountNum;                        // account number

    // obtain number of account to delete
    printf("Enter account number to delete ( 1 - %u ): ", MAX_RECORDS);
    if (scanf("%u", &accountNum) != 1)
    {
        puts("Invalid input.");
        { int c; while ((c = getchar()) != '\n' && c != EOF); } // clear buffer safely
        return;
    }
    if (accountNum < 1 || accountNum > MAX_RECORDS)
    {
        printf("Account number must be between 1 and %u.\n", MAX_RECORDS);
        return;
    }

    // move file pointer to correct record in file
    if (fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET) != 0)
    {
        puts("Error seeking to record.");
        return;
    }
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if record does not exist
    if (client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
    } // end if
    else
    { // delete record
        // move file pointer to correct record in file
        if (fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET) != 0)
        {
            puts("Error seeking to record.");
            return;
        }
        // replace existing record with blank record
        if (fwrite(&blankClient, sizeof(struct clientData), 1, fPtr) != 1)
        {
            puts("Error writing to file.");
        }
    } // end else
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; // account number

    // obtain number of account to create
    printf("Enter new account number ( 1 - %u ): ", MAX_RECORDS);
    if (scanf("%u", &accountNum) != 1)
    {
        puts("Invalid input.");
        { int c; while ((c = getchar()) != '\n' && c != EOF); } // clear buffer safely
        return;
    }
    if (accountNum < 1 || accountNum > MAX_RECORDS)
    {
        printf("Account number must be between 1 and %u.\n", MAX_RECORDS);
        return;
    }

    // move file pointer to correct record in file
    if (fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET) != 0)
    {
        puts("Error seeking to record.");
        return;
    }
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account already exists
    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
    } // end if
    else
    { // create record
        // user enters last name, first name and balance
        printf("%s", "Enter lastname, firstname, balance\n? ");
        if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance) != 3)
        {
            puts("Invalid input format.");
            { int c; while ((c = getchar()) != '\n' && c != EOF); } // clear buffer safely
            return;
        }

        client.acctNum = accountNum;
        // move file pointer to correct record in file
        if (fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET) != 0)
        {
            puts("Error seeking to record.");
            return;
        }
        // insert record in file
        if (fwrite(&client, sizeof(struct clientData), 1, fPtr) != 1)
        {
            puts("Error writing record to file.");
        }
    } // end else
} // end function newRecord

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; // variable to store user's choice
    int result;              // to test if scanf read correctly
    // display available options
    printf("\nEnter your choice\n"
           "1 - store a formatted text file of accounts called\n"
           "    \"%s\" for printing\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - display all accounts in console\n"
           "6 - search account by last name\n"
           "7 - end program\n? ", TEXT_FILE);

    result = scanf("%u", &menuChoice); // receive choice from user
    if (result != 1)
    {
        { int c; while ((c = getchar()) != '\n' && c != EOF); } // clear the invalid input from buffer safely
        return 0;                  // return an invalid choice to force another iteration
    }
    return menuChoice;
} // end function enterChoice

// display formatted accounts in the console
void displayAccounts(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    int result;

    rewind(fPtr); // sets pointer to beginning of file
    printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

    // read records from random-access file
    while (!feof(fPtr))
    {
        result = fread(&client, sizeof(struct clientData), 1, fPtr);

        // display record
        if (result != 0 && client.acctNum != 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
        }
    }
    printf("\n");
} // end function displayAccounts

// search for an account by last name
void searchRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    char searchName[15];
    int result;
    int found = 0;

    printf("Enter last name to search: ");
    if (scanf("%14s", searchName) != 1)
    {
        puts("Invalid input.");
        { int c; while ((c = getchar()) != '\n' && c != EOF); } // clear buffer safely
        return;
    }

    rewind(fPtr); // sets pointer to beginning of file
    printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

    // read records from random-access file
    while (!feof(fPtr))
    {
        result = fread(&client, sizeof(struct clientData), 1, fPtr);

        if (result != 0 && client.acctNum != 0 && strcmp(client.lastName, searchName) == 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No accounts found with last name '%s'.\n", searchName);
    }
    printf("\n");
} // end function searchRecord