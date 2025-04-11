#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "1920"
#define DB_NAME "InventoryManagement"

// Function Prototypes
void adminMenu(MYSQL *conn);
void staffMenu(MYSQL *conn);
void viewInventory(MYSQL *conn);
void addProduct(MYSQL *conn);
void updateStock(MYSQL *conn);
void deleteProduct(MYSQL *conn);
void registerUser(MYSQL *conn);
int userLogin(MYSQL *conn, char *role);

// User Login
int userLogin(MYSQL *conn, char *role) {
    char username[50], password[50], query[256];
    printf("\nEnter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    sprintf(query, "SELECT role FROM Users WHERE username='%s' AND password='%s'", username, password);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        strcpy(role, row[0]); // Get the user's role
        mysql_free_result(result);
        return 1;
    } else {
        mysql_free_result(result);
        return 0;
    }
}

// Register User
void registerUser(MYSQL *conn) {
    char username[50], password[50], email[100], role[10], query[256];
    printf("\nEnter new username: ");
    scanf("%s", username);
    printf("Enter new password: ");
    scanf("%s", password);
    printf("Enter email: ");
    scanf("%s", email);
    printf("Enter role (admin/staff): ");
    scanf("%s", role);

    sprintf(query, "INSERT INTO Users (username, password, email, role) VALUES ('%s', '%s', '%s', '%s')", username, password, email, role);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
    } else {
        printf("Registration successful!\n");
    }
}

// View Inventory (Formatted Table)
void viewInventory(MYSQL *conn) {
    if (mysql_query(conn, "SELECT * FROM Products")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row;
    printf("\n+------------+----------------+----------------+------------+------------+\n");
    printf("| Product ID |   Name          | Category       | Quantity   |   Price    |\n");
    printf("+------------+----------------+----------------+------------+------------+\n");
    while ((row = mysql_fetch_row(result))) {
        printf("| %-10s | %-14s | %-14s | %-10s | $%-9s |\n", row[0], row[1], row[2], row[3], row[4]);
    }
    printf("+------------+----------------+----------------+------------+------------+\n");
    mysql_free_result(result);
}

// Add Product (Admin Functionality)
void addProduct(MYSQL *conn) {
    char name[100], category[100], query[256];
    int quantity;
    float price;

    printf("\nEnter Product Name: ");
    scanf("%s", name);
    printf("Enter Product Category: ");
    scanf("%s", category);
    printf("Enter Quantity: ");
    scanf("%d", &quantity);
    printf("Enter Price: ");
    scanf("%f", &price);

    sprintf(query, "INSERT INTO Products (name, category, quantity, price) VALUES ('%s', '%s', %d, %.2f)", name, category, quantity, price);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
    } else {
        printf("Product added successfully!\n");
    }
}

// Update Stock (Admin/Staff Functionality)
void updateStock(MYSQL *conn) {
    int productId, quantity;
    char query[256];

    printf("\nEnter Product ID to update: ");
    scanf("%d", &productId);
    printf("Enter New Quantity: ");
    scanf("%d", &quantity);

    sprintf(query, "UPDATE Products SET quantity = %d WHERE productId = %d", quantity, productId);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
    } else {
        if (mysql_affected_rows(conn) > 0) {
            printf("Stock updated successfully!\n");
        } else {
            printf("No product found with ID %d.\n", productId);
        }
    }
}

// Delete Product (Admin Functionality)
void deleteProduct(MYSQL *conn) {
    int productId;
    char query[256];

    printf("\nEnter Product ID to delete: ");
    scanf("%d", &productId);

    sprintf(query, "DELETE FROM Products WHERE productId = %d", productId);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
    } else {
        if (mysql_affected_rows(conn) > 0) {
            printf("Product deleted successfully!\n");
        } else {
            printf("No product found with ID %d.\n", productId);
        }
    }
}

// Admin Menu
void adminMenu(MYSQL *conn) {
    int choice;
    while (1) {
        printf("\nADMIN MENU:\n1. View Inventory\n2. Add Product\n3. Update Stock\n4. Delete Product\n5. Register User\n6. Logout\nEnter choice: ");
        scanf("%d", &choice);
        if (choice == 1) viewInventory(conn);
        else if (choice == 2) addProduct(conn);
        else if (choice == 3) updateStock(conn);
        else if (choice == 4) deleteProduct(conn);
        else if (choice == 5) registerUser(conn);
        else if (choice == 6) return;
        else printf("Invalid choice.\n");
    }
}

// Staff Menu
void staffMenu(MYSQL *conn) {
    int choice;
    while (1) {
        printf("\nSTAFF MENU:\n1. View Inventory\n2. Update Stock\n3. Logout\nEnter choice: ");
        scanf("%d", &choice);
        if (choice == 1) viewInventory(conn);
        else if (choice == 2) updateStock(conn);
        else if (choice == 3) return;
        else printf("Invalid choice.\n");
    }
}

// Main Function
int main() {
    MYSQL *conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    int choice;
    char role[10];
    while (1) {
        printf("\n********************************** INVENTORY MANAGEMENT SYSTEM **********************************\n");
        printf("\nMAIN MENU:\n1. Login\n2. Register\n3. Exit\nEnter choice: ");
        scanf("%d", &choice);
        if (choice == 1) {
            if (userLogin(conn, role)) {
                if (strcmp(role, "admin") == 0) adminMenu(conn);
                else if (strcmp(role, "staff") == 0) staffMenu(conn);
            } else {
                printf("Login failed. Invalid username or password.\n");
            }
        } else if (choice == 2) registerUser(conn);
        else if (choice == 3) {
            printf("Thank You..Have a Good Day!");
            break;
        } else printf("Invalid choice.\n");
    }

    mysql_close(conn);
    return 0;
}
