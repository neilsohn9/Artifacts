#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

struct User {
    string username, password;
};

struct Vehicle {
    int id;
    string make, model;
    int year;
    double priceKsh;
};

struct Order {
    string username;
    int vehicleId;
};

// Pre-loaded vehicles (no manual input)
vector<Vehicle> loadVehicles() {
    vector<Vehicle> vehicles = {
        {1, "Toyota", "Axio", 2015, 900000},
        {2, "Honda", "Fit", 2018, 850000},
        {3, "Mazda", "Demio", 2016, 780000},
        {4, "Subaru", "Impreza", 2014, 1100000},
        {5, "Nissan", "Note", 2017, 820000}
    };
    return vehicles;
}

// Register user
void registerUser() {
    User u;
    cout << "\n--- Register ---\n";
    cout << "Enter username: ";
    cin >> u.username;
    cout << "Enter password: ";
    cin >> u.password;

    ofstream outFile("users.txt", ios::app);
    outFile << u.username << " " << u.password << endl;
    outFile.close();
    cout << "Registration successful!\n";
}

// Login
bool loginUser(User& loggedInUser) {
    string uname, pass;
    cout << "\n--- Login ---\n";
    cout << "Enter username: ";
    cin >> uname;
    cout << "Enter password: ";
    cin >> pass;

    ifstream inFile("users.txt");
    User u;
    while (inFile >> u.username >> u.password) {
        if (u.username == uname && u.password == pass) {
            loggedInUser = u;
            cout << "Login successful!\n";
            return true;
        }
    }
    inFile.close();
    cout << "Login failed. Invalid credentials.\n";
    return false;
}

// Proceed to payment
void proceedToPayment(const User& user, const Vehicle& vehicle) {
    int payChoice;
    cout << "\nProceed to pay Ksh " << vehicle.priceKsh << " for " << vehicle.make << " " << vehicle.model << "?\n";
    cout << "1. Yes\n2. No\nChoose: ";
    cin >> payChoice;

    if (payChoice == 1) {
        ofstream payFile("payments.txt", ios::app);
        payFile << user.username << " " << vehicle.id << " " << vehicle.priceKsh << endl;
        payFile.close();
        cout << "Payment successful! Your order is being processed.\n";
    } else {
        cout << "You chose not to pay now. You can pay later from the menu.\n";
    }
}

// View vehicles
void viewVehicles(const vector<Vehicle>& vehicles) {
    cout << fixed << setprecision(2);
    cout << "\n--- Vehicle Listings (Prices in Ksh) ---\n";
    cout << left << setw(5) << "ID" << setw(10) << "Make" << setw(10) << "Model"
         << setw(6) << "Year" << setw(12) << "Price" << endl;
    cout << "--------------------------------------------\n";
    for (const auto& v : vehicles) {
        cout << left << setw(5) << v.id << setw(10) << v.make << setw(10) << v.model
             << setw(6) << v.year << setw(12) << v.priceKsh << endl;
    }
}

// Order vehicle
void orderVehicle(const User& user, const vector<Vehicle>& vehicles) {
    int id;
    cout << "\nEnter Vehicle ID to order: ";
    cin >> id;

    bool found = false;
    for (const auto& v : vehicles) {
        if (v.id == id) {
            found = true;
            ofstream outFile("orders.txt", ios::app);
            outFile << user.username << " " << id << endl;
            outFile.close();
            cout << "Order placed for " << v.make << " " << v.model << " - Ksh " << v.priceKsh << "\n";
            proceedToPayment(user, v);
            break;
        }
    }
    if (!found) {
        cout << "Vehicle not found.\n";
    }
}

// View user's order
void viewMyOrder(const User& user, const vector<Vehicle>& vehicles) {
    ifstream inFile("orders.txt");
    Order o;
    bool found = false;
    while (inFile >> o.username >> o.vehicleId) {
        if (o.username == user.username) {
            for (const auto& v : vehicles) {
                if (v.id == o.vehicleId) {
                    cout << "\nYour Order:\n";
                    cout << v.make << " " << v.model << " (" << v.year << ") - Ksh " << v.priceKsh << endl;
                    found = true;
                    break;
                }
            }
        }
    }
    inFile.close();
    if (!found) {
        cout << "You have no active orders.\n";
    }
}

// View payment status
void viewPaymentStatus(const User& user, const vector<Vehicle>& vehicles) {
    ifstream inFile("payments.txt");
    string uname;
    int vid;
    double amount;
    bool found = false;

    while (inFile >> uname >> vid >> amount) {
        if (uname == user.username) {
            for (const auto& v : vehicles) {
                if (v.id == vid) {
                    cout << "\nPayment Found:\n";
                    cout << "Vehicle: " << v.make << " " << v.model << " - Paid: Ksh " << amount << endl;
                    found = true;
                    break;
                }
            }
        }
    }
    inFile.close();
    if (!found) {
        cout << "No payment made yet.\n";
    }
}

// Cancel order
void cancelOrder(const User& user) {
    ifstream inFile("orders.txt");
    ofstream tempFile("temp.txt");
    Order o;
    bool cancelled = false;

    while (inFile >> o.username >> o.vehicleId) {
        if (o.username != user.username) {
            tempFile << o.username << " " << o.vehicleId << endl;
        } else {
            cancelled = true;
        }
    }
    inFile.close();
    tempFile.close();
    remove("orders.txt");
    rename("temp.txt", "orders.txt");

    if (cancelled) {
        cout << "Order cancelled successfully.\n";
    } else {
        cout << "No order to cancel.\n";
    }
}

// After login menu
void userMenu(const User& user) {
    vector<Vehicle> vehicles = loadVehicles();
    int choice;
    do {
        cout << "\n--- Menu ---\n";
        cout << "1. View Vehicles\n2. Order Vehicle\n3. View My Order\n4. Cancel Order\n5. View Payment Status\n6. Logout\nChoose: ";
        cin >> choice;

        switch (choice) {
            case 1: viewVehicles(vehicles); break;
            case 2: viewVehicles(vehicles); orderVehicle(user, vehicles); break;
            case 3: viewMyOrder(user, vehicles); break;
            case 4: cancelOrder(user); break;
            case 5: viewPaymentStatus(user, vehicles); break;
            case 6: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 6);
}

// Main
int main() {
    int choice;
    do {
        cout << "\n=== Car Marketplace ===\n";
        cout << "1. Register\n2. Login\n3. Exit\nChoose: ";
        cin >> choice;

        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            User currentUser;
            if (loginUser(currentUser)) {
                userMenu(currentUser);
            }
        } else if (choice == 3) {
            cout << "Goodbye!\n";
        } else {
            cout << "Invalid choice.\n";
        }
    } while (choice != 3);

    return 0;
}
