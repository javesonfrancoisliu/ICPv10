#include <stdio.h>      // Includes standard input/output functions like printf and scanf.
#include <stdlib.h>     // Includes standard library functions like memory allocation (malloc, free).
#include <string.h>     // Includes string handling functions like strcpy and strcmp.
#include <ctype.h>      // Includes character type functions like isdigit, isalpha, etc.

#include "FileHandling.h"                // Includes your custom functions for handling files.
#include "FormatHandling.h"              // Includes your custom functions for formatting data.
#include "ProductManagement.h"           // Includes your functions for managing products.
#include "InventoryStockManagement.h"    // Includes your functions for managing inventory.
#include "CategorySupplierManagement.h"  // Includes your functions for categories and suppliers.
#include "CustomerTransactionManagement.h" // Includes your functions for customers and transactions.

#define ADMIN_FILE "admins.txt" // Defines a constant for the admin data filename.

void mainSystemMenu(int *loggedInStatus, int *keepRunningApp, const char *currentAdminID); // Declares the main menu function.
int handleLogin(char *loggedInAdminID); // Declares the function to handle admin login.
int verifyAdminCredentials(const char *adminID, const char *password); // Declares the function to check admin ID and password.
void getStringInput(const char *prompt, char *buffer, int buffer_size); // Declares a helper function to get string input safely.
int getIntegerInput(const char *prompt); // Declares a helper function to get integer input safely.

int main() // The main function where the program starts execution.
{
    checkFileExist(ADMIN_FILE); // Ensures the admin file exists before starting.
    checkFileExist("inventory.txt"); // Ensures the inventory file exists.
    checkFileExist("categories.txt"); // Ensures the categories file exists.
    checkFileExist("suppliers.txt"); // Ensures the suppliers file exists.
    checkFileExist("customers.txt"); // Ensures the customers file exists.
    checkFileExist("transactions.txt"); // Ensures the transactions file exists.

    int loggedIn = 0; // Initializes login status to 0 (false).
    int keepRunningApp = 1; // Initializes a flag to 1 (true) to keep the application running.
    char currentAdminID[MAX_ID_LENGTH] = {0}; // Creates a character array to store the logged-in admin's ID.

    printf("Welcome to the Integrated Management System!\n"); // Prints a welcome message to the user.

    do // Starts a loop that continues as long as keepRunningApp is true.
    {
        if (!loggedIn) // Checks if no user is currently logged in.
        {
            loggedIn = handleLogin(currentAdminID); // Calls the login function to attempt a login.
            if (!loggedIn) // Checks if the login attempt failed.
            {
                keepRunningApp = 0; // Sets the flag to 0 (false) to exit the application.
            }
        }

        if (loggedIn && keepRunningApp) // Checks if a user is logged in and the app should keep running.
        {
            mainSystemMenu(&loggedIn, &keepRunningApp, currentAdminID); // Calls the main menu function.
        }

    } while (keepRunningApp); // The loop continues until keepRunningApp becomes 0.

    freeAllLists(); // Calls a function to free any allocated memory before exiting.
    printf("\nSystem shutting down. Thank you!\n"); // Prints a shutdown message.
    return 0; // Returns 0 to indicate the program finished successfully.
}


void mainSystemMenu(int *loggedInStatus, int *keepRunningApp, const char *currentAdminID) // Function to display and handle the main menu.
{
    int choice; // Declares a variable to store the user's menu choice.
    do // Starts a loop to repeatedly show the menu.
    {
        printf("\n--- Main System Menu ---\n"); // Prints the menu title.
        printf("1. Product Management\n"); // Prints menu option 1.
        printf("2. Inventory and Stock Management\n"); // Prints menu option 2.
        printf("3. Category and Supplier Management\n"); // Prints menu option 3.
        printf("4. User and Transaction Management\n"); // Prints menu option 4.
        printf("0. Logout\n"); // Prints menu option 0 for logging out.
        printf("--------------------------\n"); // Prints a separator line.

        choice = getIntegerInput("Enter your choice: "); // Prompts for and gets the user's choice.

        switch (choice) // Starts a switch statement to handle the user's choice.
        {
        case 1: // If the user chose 1.
            productManagementMenu(); // Calls the product management menu function.
            break; // Exits the switch statement.
        case 2: // If the user chose 2.
            inventory_stock_main(); // Calls the inventory and stock management function.
            break; // Exits the switch statement.
        case 3: // If the user chose 3.
            category_supplier_main(); // Calls the category and supplier management function.
            break; // Exits the switch statement.
        case 4: // If the user chose 4.
            user_transaction_main(currentAdminID); // Calls the user and transaction management function.
            break; // Exits the switch statement.
        case 0: // If the user chose 0.
            printf("\nLogging out user %s...\n", currentAdminID); // Prints a logout message with the admin's ID.
            *loggedInStatus = 0; // Sets the logged-in status to 0 (false).
            break; // Exits the switch statement.
        default: // If the user entered an invalid choice.
            printf("Invalid choice. Please enter a number between 0 and 4.\n"); // Prints an error message.
        }

        if (choice != 0) // Checks if the user's choice was not to log out.
        {
            printf("\nPress Enter to return to the Main Menu..."); // Prompts the user to press Enter.
            int c; // Declares a character variable to clear the input buffer.
            while ((c = getchar()) != '\n' && c != EOF); // Waits for the Enter key to be pressed.
        }

    } while (choice != 0); // The loop continues until the user chooses 0 to log out.
}

int handleLogin(char *loggedInAdminID) // Function to manage the user login process.
{
    char inputID[MAX_ID_LENGTH]; // Creates a character array for the admin ID input.
    char inputPassword[MAX_PASSWORD_LENGTH]; // Creates a character array for the password input.
    int attempts = 0; // Initializes the login attempt counter to 0.
    const int MAX_ATTEMPTS = 3; // Sets the maximum number of login attempts to 3.

    while (attempts < MAX_ATTEMPTS) // Starts a loop that runs as long as attempts are less than the maximum.
    {
        printf("\n--- System Login (Attempt %d of %d) ---\n", attempts + 1, MAX_ATTEMPTS); // Shows the current attempt number.
        getStringInput("Enter Admin ID: ", inputID, sizeof(inputID)); // Prompts for and gets the admin ID.
        getStringInput("Enter Password: ", inputPassword, sizeof(inputPassword)); // Prompts for and gets the password.

        if (verifyAdminCredentials(inputID, inputPassword)) // Checks if the entered ID and password are correct.
        {
            printf("\nLogin successful! Welcome, %s.\n", inputID); // Prints a success message.
            strcpy(loggedInAdminID, inputID); // Copies the successful ID to the main program's variable.
            return 1; // Returns 1 to indicate a successful login.
        }
        else // If the credentials were not correct.
        {
            printf("\nInvalid Admin ID or Password.\n"); // Prints an error message.
            attempts++; // Increments the attempt counter.
        }
    }

    printf("\nMaximum login attempts reached. Access denied.\n"); // Informs the user that they have failed too many times.
    return 0; // Returns 0 to indicate a failed login.
}

int verifyAdminCredentials(const char *adminID, const char *password) // Function to check credentials against the admin file.
{
    FILE *file = fopen(ADMIN_FILE, "r"); // Opens the admin file in "read" mode.
    if (file == NULL) // Checks if the file failed to open.
    {
        printf("Critical Error: Cannot open admin file for verification.\n"); // Prints a critical error message.
        return 0; // Returns 0 to indicate failure.
    }

    Admin admin; // Declares a variable of type Admin struct to hold data from the file.
    char line[512]; // Creates a character array to read each line from the file.

    while (fgets(line, sizeof(line), file)) // Reads the file line by line until the end.
    {
        // Parses the line to extract the admin ID and password, skipping other fields.
        if (sscanf(line, "%[^,],%*[^,],%*[^,],%*[^,],%*[^,],%s", admin.adminID, admin.password) == 2)
        {
            if (strcmp(admin.adminID, adminID) == 0 && strcmp(admin.password, password) == 0) // Compares the file's credentials with the user's input.
            {
                fclose(file); // Closes the file.
                return 1; // Returns 1 to indicate the credentials match.
            }
        }
    }
    fclose(file); // Closes the file if no match was found.
    return 0; // Returns 0 to indicate no matching credentials were found.
}

void getStringInput(const char *prompt, char *buffer, int buffer_size) // A safe function to get a line of text from the user.
{
    printf("%s", prompt); // Prints the prompt message to the screen.
    if (fgets(buffer, buffer_size, stdin) != NULL) // Reads a line of input from the keyboard, including the newline.
    {
        buffer[strcspn(buffer, "\n")] = 0; // Finds the newline character and replaces it with a null terminator.
    }
    else // If there was an error reading input.
    {
        buffer[0] = '\0'; // Ensures the buffer is an empty string.
    }
}

int getIntegerInput(const char *prompt) // A safe function to get an integer from the user.
{
    char buffer[100]; // Creates a character array to temporarily store the user's input.
    int value; // Declares a variable to store the converted integer value.
    char term; // Declares a character to check for extra characters after the number.

    printf("%s", prompt); // Prints the prompt message to the screen.
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) // Reads the entire line of input from the user.
    {
        // Tries to convert the input to an integer and checks if there are any trailing characters.
        if (sscanf(buffer, "%d%c", &value, &term) == 2 && term == '\n')
        {
            return value; // Returns the valid integer value.
        }
    }
    return -1; // Returns -1 to indicate the input was not a valid integer.
}