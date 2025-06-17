#ifndef PRODUCT_MANAGEMENT_H // If PRODUCT_MANAGEMENT_H is not defined,
#define PRODUCT_MANAGEMENT_H // Define PRODUCT_MANAGEMENT_H to prevent multiple inclusions.

#include <stdio.h> // Includes standard input/output functions.
#include <string.h> // Includes string handling functions.
#include <stdlib.h> // Includes standard library functions like atof and atoi.
#include <ctype.h> // Includes functions to check character types.

#include "FileHandling.h" // Includes your custom file handling definitions.
#include "FormatHandling.h" // Includes your custom format handling definitions.

#define INVENTORY_FILE "inventory.txt" // Defines a constant for the inventory filename.
#define CATEGORIES_FILE "categories.txt" // Defines a constant for the categories filename.

// A private helper function to get a non-empty string from the user with validation.
static inline void getValidString(char *outputBuffer, int bufferSize, const char *prompt)
{
    char tempBuffer[256]; // Creates a temporary buffer to hold raw user input.
    do // Starts a loop that continues until valid input is received.
    {
        printf("%s: ", prompt); // Prints the prompt message to the user.
        if (fgets(tempBuffer, sizeof(tempBuffer), stdin) == NULL) // Reads a line of input safely.
        {
            printf("Error reading input. Please try again.\n"); // Handles potential input errors.
            tempBuffer[0] = '\0'; // Resets the buffer to be empty.
            continue; // Skips the rest of the loop and tries again.
        }
        tempBuffer[strcspn(tempBuffer, "\n")] = 0; // Removes the trailing newline character from the input.

        if (tempBuffer[0] == '\0') // Checks if the input is empty.
        {
            printf("Input cannot be empty. Please try again.\n"); // Shows an error if the input is empty.
        }
        else if (strlen(tempBuffer) >= (unsigned int)bufferSize) // Checks if the input is too long for the destination buffer.
        {
            printf("Input too long. Maximum %d characters allowed. Please try again.\n", bufferSize - 1); // Shows an error for oversized input.
        }
        else // If input is valid.
        {
            strcpy(outputBuffer, tempBuffer); // Copies the valid input to the final output buffer.
            return; // Exits the function since valid input has been captured.
        }
    } while (1); // This loop runs forever until `return` is called.
}

// A private helper function to get a validated integer from the user.
static inline int getValidIntegerInput(const char *prompt, int allowZero, int allowNegative)
{
    char buffer[100]; // Creates a buffer to hold the string input.
    int value; // Declares a variable to store the converted integer.
    int isValid; // Declares a flag to track if the input is valid.

    do // Starts a loop that continues until the input is valid.
    {
        isValid = 1; // Resets the validation flag to true for each new attempt.
        getValidString(buffer, sizeof(buffer), prompt); // Gets a non-empty string from the user.
        value = intChecker(buffer); // Calls a custom function to convert the string to an integer.

        if (value == 0 && strcmp(buffer, "0") != 0) // Checks if conversion failed (e.g., input was "abc").
        {
            printf("Invalid integer format. Please enter a whole number.\n"); // Shows an error for non-integer input.
            isValid = 0; // Sets the flag to false to repeat the loop.
        }
        else if (!allowZero && value == 0 && strcmp(buffer, "0") == 0) // Checks if zero was entered but is not allowed.
        {
            printf("Zero is not allowed for this field. Please try again.\n"); // Shows an error if zero is disallowed.
            isValid = 0; // Sets the flag to false.
        }
        else if (!allowNegative && value < 0) // Checks if a negative number was entered but is not allowed.
        {
            printf("Negative numbers are not allowed for this field. Please try again.\n"); // Shows an error for negative input.
            isValid = 0; // Sets the flag to false.
        }
    } while (!isValid); // The loop continues as long as the input is not valid.
    return value; // Returns the successfully validated integer.
}

// A private helper function to get a validated float from the user.
static inline float getValidFloatInput(const char *prompt, int allowZero, int allowNegative)
{
    char buffer[100]; // Creates a buffer to hold the string input.
    float value; // Declares a variable to store the converted float.
    int isValid; // Declares a flag to track if the input is valid.

    do // Starts a loop that continues until the input is valid.
    {
        isValid = 1; // Resets the validation flag to true for each new attempt.
        getValidString(buffer, sizeof(buffer), prompt); // Gets a non-empty string from the user.
        value = floatChecker(buffer); // Calls a custom function to convert the string to a float.

        // Checks if conversion failed (e.g., input was "abc").
        if (value == 0.0f && !(strcmp(buffer, "0") == 0 || strcmp(buffer, "0.0") == 0 || strcmp(buffer, ".0") == 0 || strcmp(buffer, "0.") == 0))
        {
            printf("Invalid float format. Please enter a number (e.g., 123.45).\n"); // Shows an error for non-float input.
            isValid = 0; // Sets the flag to false.
        }
        // Checks if zero was entered but is not allowed.
        else if (!allowZero && value == 0.0f && (strcmp(buffer, "0") == 0 || strcmp(buffer, "0.0") == 0 || strcmp(buffer, ".0") == 0 || strcmp(buffer, "0.") == 0))
        {
            printf("Zero is not allowed for this field. Please try again.\n"); // Shows an error if zero is disallowed.
            isValid = 0; // Sets the flag to false.
        }
        else if (!allowNegative && value < 0.0f) // Checks if a negative number was entered but is not allowed.
        {
            printf("Negative numbers are not allowed for this field. Please try again.\n"); // Shows an error for negative input.
            isValid = 0; // Sets the flag to false.
        }
    } while (!isValid); // The loop continues as long as the input is not valid.
    return value; // Returns the successfully validated float.
}

// A private helper function to print the fields of a single inventory product.
static inline void printInventoryFields(const Inventory *product)
{
    if (product == NULL) // Checks if the provided product pointer is null.
    {
        printf("Error: Cannot print details of a NULL product.\n"); // Prints an error if the product is null.
        return; // Exits the function to prevent a crash.
    }
    printf("Product ID   : %s\n", product->productID); // Prints the product's ID.
    printf("Category ID  : %s\n", product->categoryID); // Prints the product's category ID.
    printf("Name         : %s\n", product->name); // Prints the product's name.
    printf("Price        : %.2f\n", product->price); // Prints the product's price, formatted to 2 decimal places.
    printf("Quantity     : %d\n", product->quantity); // Prints the product's quantity.
    printf("Description  : %s\n", product->description); // Prints the product's description.
}

// A private helper function to find a product by its ID and load its details.
static inline int getProductDetails_local(const char *productID, Inventory *productOut)
{
    FILE *file = fopen(INVENTORY_FILE, "r"); // Opens the inventory file in read mode.
    if (file == NULL) // Checks if the file failed to open.
    {
        return 0; // Returns 0 (failure) if the file cannot be opened.
    }

    Inventory item_buffer; // Creates a temporary struct to hold data read from the file.
    char tempPriceStr[50]; // Creates a temporary string for the price.
    char tempQuantityStr[50]; // Creates a temporary string for the quantity.

    // Reads the file line by line, parsing the comma-separated values.
    while (fscanf(file, "%10[^,],%10[^,],%50[^,],%49[^,],%49[^,],%200[^\n]\n",
                  item_buffer.productID, item_buffer.categoryID, item_buffer.name,
                  tempPriceStr, tempQuantityStr, item_buffer.description) == 6)
    {
        if (strcmp(item_buffer.productID, productID) == 0) // Compares the product ID from the file with the one we're searching for.
        {
            strcpy(productOut->productID, item_buffer.productID); // Copies the product ID to the output struct.
            strcpy(productOut->categoryID, item_buffer.categoryID); // Copies the category ID.
            strcpy(productOut->name, item_buffer.name); // Copies the name.
            productOut->price = atof(tempPriceStr); // Converts the price string to a float and assigns it.
            productOut->quantity = atoi(tempQuantityStr); // Converts the quantity string to an integer and assigns it.
            strcpy(productOut->description, item_buffer.description); // Copies the description.
            fclose(file); // Closes the file.
            return 1; // Returns 1 (success) because the product was found.
        }
    }
    fclose(file); // Closes the file if the loop finishes (product not found).
    return 0; // Returns 0 (failure) because the product was not found.
}

// A private helper function to display all available categories and let the user select one.
static inline int displayAndSelectCategory(char *selectedCategoryID)
{
    FILE *file = fopen(CATEGORIES_FILE, "r"); // Opens the categories file in read mode.
    if (file == NULL) // Checks if the file failed to open.
    {
        printf("Error: Could not open categories file '%s'.\n", CATEGORIES_FILE); // Prints an error message.
        return 0; // Returns 0 (failure).
    }

    Category categoriesList[MAX_RECORDS]; // Creates an array of structs to hold all categories.
    int categoryCount = 0; // Initializes a counter for the number of categories found.

    printf("\n--- Available Categories ---\n"); // Prints a title for the list.
    // Reads all categories from the file into the array.
    while (categoryCount < MAX_RECORDS &&
           fscanf(file, "%10[^,],%50[^,],%200[^\n]\n",
                  categoriesList[categoryCount].categoryID,
                  categoriesList[categoryCount].name,
                  categoriesList[categoryCount].description) == 3)
    {
        printf("%d. %s - %s\n", categoryCount + 1, categoriesList[categoryCount].categoryID, categoriesList[categoryCount].name); // Prints each category as a numbered option.
        categoryCount++; // Increments the category counter.
    }
    fclose(file); // Closes the categories file.

    if (categoryCount == 0) // Checks if no categories were found.
    {
        printf("No categories available. Please add categories first.\n"); // Informs the user that no categories exist.
        return 0; // Returns 0 (failure).
    }

    int choice; // Declares a variable for the user's numeric choice.
    do // Starts a loop to get a valid selection from the user.
    {
        choice = getValidIntegerInput("Select Category (by number, 0 to Cancel)", 1, 0); // Asks the user for their choice.
        if (choice == 0) // Checks if the user wants to cancel the operation.
        {
            printf("Category selection cancelled.\n"); // Confirms cancellation.
            return 0; // Returns 0 to indicate cancellation.
        }
        if (choice >= 1 && choice <= categoryCount) // Checks if the choice is a valid number in the list.
        {
            strcpy(selectedCategoryID, categoriesList[choice - 1].categoryID); // Copies the chosen category's ID to the output variable.
            return 1; // Returns 1 (success).
        }
        else // If the choice is out of the valid range.
        {
            printf("Invalid selection. Please enter a number between 1 and %d, or 0 to cancel.\n", categoryCount); // Prints an error message.
        }
    } while (1); // The loop continues until a valid choice is made or the user cancels.
}

// A private helper function to display all available products and let the user select one.
static inline int displayAndSelectProductID(char *selectedProductID)
{
    FILE *file = fopen(INVENTORY_FILE, "r"); // Opens the inventory file in read mode.
    if (file == NULL) // Checks if the file failed to open.
    {
        printf("Error: Could not open inventory file '%s'.\n", INVENTORY_FILE); // Prints an error message.
        return 0; // Returns 0 (failure).
    }

    Inventory productsList[MAX_RECORDS]; // Creates an array of structs to hold all products.
    int productCount = 0; // Initializes a counter for the number of products found.
    char tempPriceStr[50], tempQuantityStr[50]; // Creates temporary strings for price and quantity during parsing.

    printf("\n--- Available Products ---\n"); // Prints a title for the list.
    // Reads all products from the file into the array.
    while (productCount < MAX_RECORDS &&
           fscanf(file, "%10[^,],%10[^,],%50[^,],%49[^,],%49[^,],%200[^\n]\n",
                  productsList[productCount].productID,
                  productsList[productCount].categoryID,
                  productsList[productCount].name,
                  tempPriceStr,
                  tempQuantityStr,
                  productsList[productCount].description) == 6)
    {
        printf("%d. %s - %s\n", productCount + 1, productsList[productCount].productID, productsList[productCount].name); // Prints each product as a numbered option.
        productCount++; // Increments the product counter.
    }
    fclose(file); // Closes the inventory file.

    if (productCount == 0) // Checks if no products were found.
    {
        printf("No products available in inventory.\n"); // Informs the user that no products exist.
        return 0; // Returns 0 (failure).
    }

    int choice; // Declares a variable for the user's numeric choice.
    do // Starts a loop to get a valid selection from the user.
    {
        choice = getValidIntegerInput("Select Product (by number, 0 to Cancel)", 1, 0); // Asks the user for their choice.
        if (choice == 0) // Checks if the user wants to cancel the operation.
        {
            printf("Product selection cancelled.\n"); // Confirms cancellation.
            return 0; // Returns 0 to indicate cancellation.
        }
        if (choice >= 1 && choice <= productCount) // Checks if the choice is a valid number in the list.
        {
            strcpy(selectedProductID, productsList[choice - 1].productID); // Copies the chosen product's ID to the output variable.
            return 1; // Returns 1 (success).
        }
        else // If the choice is out of the valid range.
        {
            printf("Invalid selection. Please enter a number between 1 and %d, or 0 to cancel.\n", productCount); // Prints an error message.
        }
    } while (1); // The loop continues until a valid choice is made or the user cancels.
}

// A private helper function to write a new product record to the inventory file.
static inline void addNewProduct_local(const Inventory *newProduct)
{
    checkFileExist(INVENTORY_FILE); // Ensures the inventory file exists before trying to write to it.
    FILE *file = fopen(INVENTORY_FILE, "a"); // Opens the inventory file in "append" mode to add to the end.
    if (file == NULL) // Checks if the file failed to open.
    {
        printf("CRITICAL ERROR: Could not open inventory file for writing.\n"); // Prints a critical error message.
        return; // Exits the function to prevent further errors.
    }

    // Writes the new product's data as a new, comma-separated line in the file.
    fprintf(file, "%s,%s,%s,%.2f,%d,%s\n",
            newProduct->productID,
            newProduct->categoryID,
            newProduct->name,
            newProduct->price,
            newProduct->quantity,
            newProduct->description);

    fclose(file); // Closes the file to save the changes.
    printf("Inventory data added successfully.\n"); // Prints a success confirmation message.
}

// A function to guide the user through adding a new product.
static inline void addNewProduct()
{
    printf("\n--- Add New Product ---\n"); // Prints the title for the "Add Product" screen.
    Inventory newProduct; // Creates a new Inventory struct to hold the product's details.

    const char *generatedID_ptr = generateID(INVENTORY_FILE, "PROD0000"); // Calls a function to generate a new, unique product ID.
    if (strstr(generatedID_ptr, "Error") != NULL) // Checks if the ID generation resulted in an error.
    {
        printf("Error generating new Product ID: %s\n", generatedID_ptr); // Prints the error message from the ID generator.
        return; // Exits the function.
    }
    strcpy(newProduct.productID, generatedID_ptr); // Copies the newly generated ID into the product struct.
    printf("Generated Product ID: %s\n", newProduct.productID); // Shows the new ID to the user.

    if (!displayAndSelectCategory(newProduct.categoryID)) // Asks the user to select a category for the new product.
    {
        printf("Product addition aborted.\n"); // Informs the user that the process was cancelled.
        return; // Exits if no category was selected.
    }

    getValidString(newProduct.name, MAX_NAME_LENGTH, "Enter Product Name"); // Prompts for and gets the product name.
    newProduct.price = getValidFloatInput("Enter Product Price", 0, 0); // Prompts for and gets the product price.
    newProduct.quantity = getValidIntegerInput("Enter Product Quantity", 1, 0); // Prompts for and gets the product quantity.
    getValidString(newProduct.description, MAX_DESCRIPTION_LENGTH, "Enter Product Description"); // Prompts for and gets the product description.

    addNewProduct_local(&newProduct); // Calls the helper function to save the new product to the file.

    printf("\n--- Product Added Successfully ---\n"); // Prints a success header.
    Inventory addedProductDetails; // Creates a temporary struct to re-read the product details for display.
    if (getProductDetails_local(newProduct.productID, &addedProductDetails)) // Fetches the details of the product just added.
    {
        printInventoryFields(&addedProductDetails); // Prints the full details of the new product for confirmation.
    }
}

// A function to guide the user through updating an existing product's information.
static inline void updateProductInfo()
{
    printf("\n--- Update Product Information ---\n"); // Prints the title for the "Update Product" screen.
    char productIDToUpdate[MAX_ID_LENGTH]; // Creates a buffer to store the ID of the product to update.
    Inventory currentProduct; // Creates a struct to hold the product's current details.

    if (!displayAndSelectProductID(productIDToUpdate)) // Asks the user to select which product they want to update.
    {
        printf("Product update aborted.\n"); // Informs the user that the process was cancelled.
        return; // Exits if no product was selected.
    }

    do // Starts a loop to allow updating multiple fields on the same product.
    {
        if (!getProductDetails_local(productIDToUpdate, &currentProduct)) // Fetches the latest details for the selected product.
        {
            printf("Error: Product with ID '%s' not found.\n", productIDToUpdate); // Prints an error if the product can't be found.
            return; // Exits the function.
        }

        printf("\nCurrent details for Product ID %s:\n", currentProduct.productID); // Shows the user which product they are editing.
        printInventoryFields(&currentProduct); // Prints the current details of the product.

        printf("\nWhich information to update? (Enter 0 to Finish)\n"); // Asks the user which field to edit.
        printf("1. Category ID\n2. Name\n3. Price\n4. Quantity\n5. Description\n"); // Displays the editable fields.

        int fieldChoice = getValidIntegerInput("Enter your choice (1-5, 0 to Finish)", 1, 0); // Gets the user's choice.

        if (fieldChoice == 0) break; // If the user enters 0, exit the editing loop.

        char newValueBuffer[MAX_DESCRIPTION_LENGTH]; // A buffer to hold the new value as a string.
        const char *attributeToUpdate = NULL; // A pointer to store the name of the attribute being updated.

        switch (fieldChoice) // Handles the user's choice of which field to update.
        {
        case 1: // If the user chose to update the Category.
            attributeToUpdate = "categoryID"; // Sets the attribute name for the update function.
            if (!displayAndSelectCategory(newValueBuffer)) continue; // Asks the user to select a new category; skips if they cancel.
            break; // Exits the switch.
        case 2: // If the user chose to update the Name.
            attributeToUpdate = "name"; // Sets the attribute name.
            getValidString(newValueBuffer, MAX_NAME_LENGTH, "Enter new Product Name"); // Gets the new product name from the user.
            break; // Exits the switch.
        case 3: // If the user chose to update the Price.
            attributeToUpdate = "price"; // Sets the attribute name.
            sprintf(newValueBuffer, "%.2f", getValidFloatInput("Enter new Price", 0, 0)); // Gets a new float price and converts it to a string.
            break; // Exits the switch.
        case 4: // If the user chose to update the Quantity.
            attributeToUpdate = "quantity"; // Sets the attribute name.
            sprintf(newValueBuffer, "%d", getValidIntegerInput("Enter new Quantity", 1, 0)); // Gets a new integer quantity and converts it to a string.
            break; // Exits the switch.
        case 5: // If the user chose to update the Description.
            attributeToUpdate = "description"; // Sets the attribute name.
            getValidString(newValueBuffer, MAX_DESCRIPTION_LENGTH, "Enter new Description"); // Gets the new description from the user.
            break; // Exits the switch.
        default: // If the choice was not valid.
            printf("Invalid field choice.\n"); // Prints an error message.
            continue; // Skips the rest of the loop and starts over.
        }

        if (attributeToUpdate) // Checks if an attribute was successfully chosen for an update.
        {
            updateDataInventory(INVENTORY_FILE, attributeToUpdate, productIDToUpdate, newValueBuffer); // Calls the generic update function to modify the file.
            printf("\n--- Field Updated Successfully ---\n"); // Prints a success message.
        }

        char editAnotherField[10]; // A buffer for the user's 'y/n' choice.
        getValidString(editAnotherField, sizeof(editAnotherField), "Update another field for this product? (y/n)"); // Asks the user if they want to edit another field.
        if (editAnotherField[0] != 'y' && editAnotherField[0] != 'Y') break; // If the answer is not 'y' or 'Y', exit the loop.

    } while (1); // The loop runs forever until the user decides to stop editing.
    printf("Finished editing Product ID %s.\n", productIDToUpdate); // Prints a final message when done.
}

// A function to guide the user through deleting a product.
static inline void deleteProduct()
{
    printf("\n--- Delete Product from Inventory ---\n"); // Prints the title for the "Delete Product" screen.
    char productIDToDelete[MAX_ID_LENGTH]; // A buffer to hold the ID of the product to delete.
    Inventory productToDeleteDetails; // A struct to hold the details of the product for confirmation.

    if (!displayAndSelectProductID(productIDToDelete)) // Asks the user to select which product to delete.
    {
        printf("Product deletion aborted.\n"); // Informs the user that the process was cancelled.
        return; // Exits if no product was selected.
    }

    if (!getProductDetails_local(productIDToDelete, &productToDeleteDetails)) // Fetches the details of the selected product.
    {
        printf("Error: Product with ID '%s' not found.\n", productIDToDelete); // Prints an error if the product doesn't exist.
        return; // Exits the function.
    }

    printf("\nYou are about to delete the following product:\n"); // Warns the user about the deletion.
    printInventoryFields(&productToDeleteDetails); // Shows the details of the product being deleted.

    char confirmation[10]; // A buffer for the user's confirmation.
    getValidString(confirmation, sizeof(confirmation), "Are you sure? (yes/no)"); // Asks for final confirmation.

    if (strcmp(confirmation, "yes") == 0) // Checks if the user confirmed with "yes".
    {
        deleteDataInventory(INVENTORY_FILE, productIDToDelete); // Calls the function to delete the product's record from the file.
    }
    else // If the user did not type "yes".
    {
        printf("Deletion cancelled.\n"); // Confirms that the deletion was cancelled.
    }
}

// A function to let the user select and view the details of one specific product.
static inline void viewSpecificProductDetails()
{
    printf("\n--- View Specific Product Details ---\n"); // Prints the title for the screen.
    char productIDToView[MAX_ID_LENGTH]; // A buffer to hold the ID of the product to view.

    if (!displayAndSelectProductID(productIDToView)) // Asks the user to select a product.
    {
        printf("View product details aborted.\n"); // Informs the user that the process was cancelled.
        return; // Exits if no product was selected.
    }

    Inventory product; // A struct to hold the product's details.
    if (getProductDetails_local(productIDToView, &product)) // Fetches the details of the selected product.
    {
        printf("\nDetails for Product ID: %s\n", product.productID); // Prints a header for the details.
        printInventoryFields(&product); // Prints all the fields of the product.
    }
}

// A function to view all products currently in the inventory.
static inline void viewAllProducts_local()
{
    printf("\n--- All Products in Inventory ---\n"); // Prints the title for the screen.
    FILE *file = fopen(INVENTORY_FILE, "r"); // Opens the inventory file in read mode.
    if (file == NULL) // Checks if the file failed to open.
    {
        printf("Inventory is empty or file '%s' cannot be opened.\n", INVENTORY_FILE); // Prints an error/info message.
        return; // Exits the function.
    }

    Inventory item_buffer; // A temporary struct to hold data for each product as it's read.
    char tempPriceStr[50]; // A temporary string for the price.
    char tempQuantityStr[50]; // A temporary string for the quantity.
    int count = 0; // A counter for the number of products displayed.

    // Reads the file line by line, parsing each product record.
    while (fscanf(file, "%10[^,],%10[^,],%50[^,],%49[^,],%49[^,],%200[^\n]\n",
                  item_buffer.productID, item_buffer.categoryID, item_buffer.name,
                  tempPriceStr, tempQuantityStr, item_buffer.description) == 6)
    {
        count++; // Increments the product counter.
        item_buffer.price = atof(tempPriceStr); // Converts the price from a string to a float.
        item_buffer.quantity = atoi(tempQuantityStr); // Converts the quantity from a string to an integer.

        printf("\n--- Product %d ---\n", count); // Prints a header for each product.
        printInventoryFields(&item_buffer); // Prints the details of the current product.
    }

    fclose(file); // Closes the file.
    if (count == 0) // Checks if no products were found.
        printf("\nNo products found in inventory.\n"); // Informs the user if the inventory is empty.
    else // If products were found.
        printf("\nTotal products displayed: %d\n", count); // Prints the total number of products shown.
}

// The main menu for all product-related operations.
static inline void productManagementMenu()
{
    checkFileExist(INVENTORY_FILE); // Ensures the inventory file exists.
    checkFileExist(CATEGORIES_FILE); // Ensures the categories file exists.

    int choice; // A variable to hold the user's menu choice.
    do // Starts the menu loop.
    {
        printf("\n\n--- Product Management Menu ---\n"); // Prints the menu title.
        printf("1. Add New Product\n"); // Menu option 1.
        printf("2. Update Product Information\n"); // Menu option 2.
        printf("3. Delete Product\n"); // Menu option 3.
        printf("4. View Specific Product Details\n"); // Menu option 4.
        printf("5. View All Products\n"); // Menu option 5.
        printf("0. Back to Main Menu\n"); // Menu option 0.
        printf("---------------------------------\n"); // Prints a separator line.

        choice = getValidIntegerInput("Enter your choice", 1, 0); // Gets a valid integer choice from the user.

        switch (choice) // Handles the user's choice.
        {
        case 1: addNewProduct(); break; // Calls the add product function.
        case 2: updateProductInfo(); break; // Calls the update product function.
        case 3: deleteProduct(); break; // Calls the delete product function.
        case 4: viewSpecificProductDetails(); break; // Calls the view specific product function.
        case 5: viewAllProducts_local(); break; // Calls the view all products function.
        case 0: printf("Returning to Main Menu...\n"); break; // Informs the user they are returning.
        default: printf("Invalid choice. Please try again.\n"); break; // Handles invalid numeric choices.
        }

        if (choice != 0) // If the user is not exiting the menu.
        {
            printf("\nPress Enter to continue..."); // Prompts the user to press Enter.
            getchar(); // Waits for a single character input to pause the screen.
        }
    } while (choice != 0); // The loop continues until the user chooses 0.
}

#endif // Marks the end of the PRODUCT_MANAGEMENT_H header guard.