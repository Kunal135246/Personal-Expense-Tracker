/*
 * ============================================================
 *  PERSONAL EXPENSE TRACKER  -  C++ Mini Project
 * ============================================================
 *  Classes   : Category, Expense, User, Report
 *  Modules   : Expense Entry, Reports (Monthly Summary)
 *  OOP used  : Classes & Objects, Encapsulation
 *              (all data members private, accessed only via
 *               public getters/setters and member functions)
 *
 *  Compile   : g++ -std=c++11 expense_tracker.cpp -o expense_tracker
 *  Run       : ./expense_tracker      (Windows: expense_tracker.exe)
 * ============================================================
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <limits>
#include <algorithm>

using namespace std;

/* ------------------------------------------------------------
 *  Helper functions (date validation & safe input)
 * ---------------------------------------------------------- */
bool isLeapYear(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int daysInMonth(int m, int y) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && isLeapYear(y)) return 29;
    return days[m - 1];
}

bool isValidDate(int d, int m, int y) {
    if (y < 1900 || y > 2100) return false;
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > daysInMonth(m, y)) return false;
    return true;
}

string monthName(int m) {
    static const string names[] = {"January", "February", "March", "April",
                                   "May", "June", "July", "August",
                                   "September", "October", "November", "December"};
    return (m >= 1 && m <= 12) ? names[m - 1] : "Invalid";
}

int readInt(const string &prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid input. Please enter a whole number.\n";
    }
}

double readDouble(const string &prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Invalid input. Please enter a number.\n";
    }
}

string readLine(const string &prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

/* ============================================================
 *  CLASS: Category
 *  Represents a spending category (Food, Travel, Rent ...)
 * ========================================================== */
class Category {
private:
    int id;
    string name;

public:
    Category() : id(0), name("") {}
    Category(int id, const string &name) : id(id), name(name) {}

    // Getters
    int getId() const { return id; }
    string getName() const { return name; }

    // Setters
    void setName(const string &n) { name = n; }
};

/* ============================================================
 *  CLASS: Expense
 *  Represents a single expense record.
 *  All data is private (Encapsulation); validation is done in
 *  the setters so an Expense can never hold invalid data.
 * ========================================================== */
class Expense {
private:
    int id;
    double amount;
    string description;
    int categoryId;
    int day, month, year;

public:
    Expense()
        : id(0), amount(0.0), description(""), categoryId(0),
          day(1), month(1), year(2000) {}

    Expense(int id, double amount, const string &desc, int catId,
            int d, int m, int y)
        : id(id), amount(0.0), description(desc), categoryId(catId),
          day(1), month(1), year(2000) {
        setAmount(amount);
        setDate(d, m, y);
    }

    // Getters
    int getId() const { return id; }
    double getAmount() const { return amount; }
    string getDescription() const { return description; }
    int getCategoryId() const { return categoryId; }
    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    // Setters with validation
    bool setAmount(double a) {
        if (a <= 0) return false;
        amount = a;
        return true;
    }

    bool setDate(int d, int m, int y) {
        if (!isValidDate(d, m, y)) return false;
        day = d;
        month = m;
        year = y;
        return true;
    }

    void setDescription(const string &d) { description = d; }
    void setCategoryId(int c) { categoryId = c; }

    string getDateString() const {
        ostringstream oss;
        oss << setfill('0') << setw(2) << day << "-"
            << setw(2) << month << "-" << year;
        return oss.str();
    }
};

/* ============================================================
 *  CLASS: User
 *  Owns the user's categories and expenses.
 *  Provides the "Expense Entry" module (add / delete / list)
 *  plus file persistence.
 * ========================================================== */
class User {
private:
    string name;
    vector<Category> categories;
    vector<Expense> expenses;
    int nextCategoryId;
    int nextExpenseId;

public:
    User(const string &name = "Guest")
        : name(name), nextCategoryId(1), nextExpenseId(1) {}

    // ---------- Getters / Setters ----------
    string getName() const { return name; }
    void setName(const string &n) { name = n; }
    const vector<Category> &getCategories() const { return categories; }
    const vector<Expense> &getExpenses() const { return expenses; }

    // ---------- Category management ----------
    bool categoryExists(const string &catName) const {
        for (size_t i = 0; i < categories.size(); i++) {
            string a = categories[i].getName(), b = catName;
            transform(a.begin(), a.end(), a.begin(), ::tolower);
            transform(b.begin(), b.end(), b.begin(), ::tolower);
            if (a == b) return true;
        }
        return false;
    }

    bool isValidCategoryId(int id) const {
        for (size_t i = 0; i < categories.size(); i++)
            if (categories[i].getId() == id) return true;
        return false;
    }

    string getCategoryName(int id) const {
        for (size_t i = 0; i < categories.size(); i++)
            if (categories[i].getId() == id) return categories[i].getName();
        return "Unknown";
    }

    bool addCategory(const string &catName) {
        if (catName.empty() || categoryExists(catName)) return false;
        categories.push_back(Category(nextCategoryId++, catName));
        return true;
    }

    void loadDefaultCategories() {
        const char *defaults[] = {"Food", "Transport", "Rent", "Shopping",
                                  "Entertainment", "Bills", "Health", "Other"};
        for (int i = 0; i < 8; i++) addCategory(defaults[i]);
    }

    void displayCategories() const {
        cout << "\n  Categories:\n";
        for (size_t i = 0; i < categories.size(); i++)
            cout << "   " << categories[i].getId() << ". "
                 << categories[i].getName() << "\n";
    }

    // ---------- Expense Entry module ----------
    bool addExpense(double amount, const string &desc, int catId,
                    int d, int m, int y) {
        if (amount <= 0 || !isValidCategoryId(catId) || !isValidDate(d, m, y))
            return false;
        expenses.push_back(Expense(nextExpenseId++, amount, desc, catId, d, m, y));
        return true;
    }

    bool deleteExpense(int id) {
        for (size_t i = 0; i < expenses.size(); i++) {
            if (expenses[i].getId() == id) {
                expenses.erase(expenses.begin() + i);
                return true;
            }
        }
        return false;
    }

    void displayAllExpenses() const {
        if (expenses.empty()) {
            cout << "\n  No expenses recorded yet.\n";
            return;
        }
        cout << "\n  " << string(78, '=') << "\n";
        cout << "  " << left << setw(5) << "ID" << setw(13) << "Date"
             << setw(16) << "Category" << setw(28) << "Description"
             << right << setw(12) << "Amount" << "\n";
        cout << "  " << string(78, '-') << "\n";
        double total = 0;
        for (size_t i = 0; i < expenses.size(); i++) {
            const Expense &e = expenses[i];
            string desc = e.getDescription();
            if (desc.length() > 26) desc = desc.substr(0, 23) + "...";
            cout << "  " << left << setw(5) << e.getId()
                 << setw(13) << e.getDateString()
                 << setw(16) << getCategoryName(e.getCategoryId())
                 << setw(28) << desc
                 << right << setw(12) << fixed << setprecision(2) << e.getAmount() << "\n";
            total += e.getAmount();
        }
        cout << "  " << string(78, '-') << "\n";
        cout << "  " << left << setw(62) << "TOTAL" << right << setw(16)
             << fixed << setprecision(2) << total << "\n";
        cout << "  " << string(78, '=') << "\n";
    }

    // ---------- File persistence ----------
    // File format:
    //   USER|name
    //   CAT|id|name
    //   EXP|id|amount|categoryId|day|month|year|description
    bool saveToFile(const string &filename) const {
        ofstream out(filename.c_str());
        if (!out) return false;
        out << "USER|" << name << "\n";
        for (size_t i = 0; i < categories.size(); i++)
            out << "CAT|" << categories[i].getId() << "|"
                << categories[i].getName() << "\n";
        for (size_t i = 0; i < expenses.size(); i++) {
            const Expense &e = expenses[i];
            out << "EXP|" << e.getId() << "|" << e.getAmount() << "|"
                << e.getCategoryId() << "|" << e.getDay() << "|"
                << e.getMonth() << "|" << e.getYear() << "|"
                << e.getDescription() << "\n";
        }
        return true;
    }

    bool loadFromFile(const string &filename) {
        ifstream in(filename.c_str());
        if (!in) return false;

        categories.clear();
        expenses.clear();
        nextCategoryId = nextExpenseId = 1;

        string line;
        while (getline(in, line)) {
            vector<string> parts;
            stringstream ss(line);
            string token;
            // split on '|' but keep the description (last field) intact
            while (parts.size() < 7 && getline(ss, token, '|')) parts.push_back(token);
            string rest;
            if (!parts.empty() && parts[0] == "EXP") getline(ss, rest);

            if (parts.empty()) continue;
            if (parts[0] == "USER" && parts.size() >= 2) {
                name = parts[1];
            } else if (parts[0] == "CAT" && parts.size() >= 3) {
                int id = atoi(parts[1].c_str());
                categories.push_back(Category(id, parts[2]));
                nextCategoryId = max(nextCategoryId, id + 1);
            } else if (parts[0] == "EXP" && parts.size() >= 7) {
                int id = atoi(parts[1].c_str());
                double amt = atof(parts[2].c_str());
                int cat = atoi(parts[3].c_str());
                int d = atoi(parts[4].c_str());
                int m = atoi(parts[5].c_str());
                int y = atoi(parts[6].c_str());
                expenses.push_back(Expense(id, amt, rest, cat, d, m, y));
                nextExpenseId = max(nextExpenseId, id + 1);
            }
        }
        return true;
    }
};

/* ============================================================
 *  CLASS: Report
 *  The "Reports" module. Reads a User's data (read-only) and
 *  produces summaries without being able to modify anything.
 * ========================================================== */
class Report {
private:
    const User &user;

    // Returns total spent in a given month/year
    double monthTotal(int month, int year) const {
        double total = 0;
        const vector<Expense> &ex = user.getExpenses();
        for (size_t i = 0; i < ex.size(); i++)
            if (ex[i].getMonth() == month && ex[i].getYear() == year)
                total += ex[i].getAmount();
        return total;
    }

public:
    Report(const User &u) : user(u) {}

    void monthlySummary(int month, int year) const {
        const vector<Expense> &ex = user.getExpenses();
        map<int, double> byCategory;
        int count = 0;
        double total = 0;
        const Expense *highest = NULL;

        for (size_t i = 0; i < ex.size(); i++) {
            if (ex[i].getMonth() == month && ex[i].getYear() == year) {
                byCategory[ex[i].getCategoryId()] += ex[i].getAmount();
                total += ex[i].getAmount();
                count++;
                if (highest == NULL || ex[i].getAmount() > highest->getAmount())
                    highest = &ex[i];
            }
        }

        cout << "\n  " << string(60, '=') << "\n";
        cout << "   MONTHLY SUMMARY - " << monthName(month) << " " << year
             << "   (User: " << user.getName() << ")\n";
        cout << "  " << string(60, '=') << "\n";

        if (count == 0) {
            cout << "   No expenses found for this month.\n";
            cout << "  " << string(60, '=') << "\n";
            return;
        }

        cout << "   " << left << setw(18) << "Category" << right << setw(12)
             << "Amount" << setw(10) << "Share" << "   Chart\n";
        cout << "  " << string(60, '-') << "\n";

        for (map<int, double>::const_iterator it = byCategory.begin();
             it != byCategory.end(); ++it) {
            double pct = (it->second / total) * 100.0;
            int bars = static_cast<int>(pct / 5.0);   // 1 '#' per 5%
            cout << "   " << left << setw(18) << user.getCategoryName(it->first)
                 << right << setw(12) << fixed << setprecision(2) << it->second
                 << setw(8) << setprecision(1) << pct << "%   "
                 << string(bars, '#') << "\n";
        }

        cout << "  " << string(60, '-') << "\n";
        cout << "   Total expenses  : " << count << "\n";
        cout << "   Total spent     : " << fixed << setprecision(2) << total << "\n";
        cout << "   Daily average   : " << fixed << setprecision(2)
             << total / daysInMonth(month, year) << "\n";
        if (highest != NULL) {
            cout << "   Highest expense : " << highest->getAmount() << "  ("
                 << highest->getDescription() << ", "
                 << user.getCategoryName(highest->getCategoryId()) << " on "
                 << highest->getDateString() << ")\n";
        }

        // Compare with previous month
        int pm = (month == 1) ? 12 : month - 1;
        int py = (month == 1) ? year - 1 : year;
        double prev = monthTotal(pm, py);
        if (prev > 0) {
            double change = ((total - prev) / prev) * 100.0;
            cout << "   vs " << monthName(pm) << "     : "
                 << (change >= 0 ? "+" : "") << fixed << setprecision(1)
                 << change << "% ("
                 << (change >= 0 ? "spent more" : "spent less") << ")\n";
        }
        cout << "  " << string(60, '=') << "\n";
    }

    void yearlyOverview(int year) const {
        cout << "\n  " << string(40, '=') << "\n";
        cout << "   YEARLY OVERVIEW - " << year << "\n";
        cout << "  " << string(40, '=') << "\n";
        double grand = 0;
        for (int m = 1; m <= 12; m++) {
            double t = monthTotal(m, year);
            grand += t;
            cout << "   " << left << setw(14) << monthName(m) << right
                 << setw(14) << fixed << setprecision(2) << t << "\n";
        }
        cout << "  " << string(40, '-') << "\n";
        cout << "   " << left << setw(14) << "TOTAL" << right << setw(14)
             << fixed << setprecision(2) << grand << "\n";
        cout << "  " << string(40, '=') << "\n";
    }
};

/* ============================================================
 *  Menu-driven interface
 * ========================================================== */
void addExpenseUI(User &user) {
    cout << "\n  --- Add New Expense ---\n";
    double amount = readDouble("  Amount            : ");
    if (amount <= 0) {
        cout << "  [!] Amount must be greater than zero.\n";
        return;
    }
    string desc = readLine("  Description       : ");

    user.displayCategories();
    int cat = readInt("  Category ID       : ");
    if (!user.isValidCategoryId(cat)) {
        cout << "  [!] Invalid category ID.\n";
        return;
    }

    int d = readInt("  Day   (1-31)      : ");
    int m = readInt("  Month (1-12)      : ");
    int y = readInt("  Year  (e.g. 2026) : ");

    if (user.addExpense(amount, desc, cat, d, m, y))
        cout << "  [OK] Expense added successfully.\n";
    else
        cout << "  [!] Could not add expense. Check the date and try again.\n";
}

void deleteExpenseUI(User &user) {
    user.displayAllExpenses();
    if (user.getExpenses().empty()) return;
    int id = readInt("\n  Enter Expense ID to delete: ");
    if (user.deleteExpense(id))
        cout << "  [OK] Expense deleted.\n";
    else
        cout << "  [!] No expense with that ID.\n";
}

void addCategoryUI(User &user) {
    string name = readLine("\n  New category name: ");
    if (user.addCategory(name))
        cout << "  [OK] Category added.\n";
    else
        cout << "  [!] Category is empty or already exists.\n";
}

void summaryUI(const User &user) {
    int m = readInt("\n  Month (1-12)      : ");
    int y = readInt("  Year  (e.g. 2026) : ");
    if (m < 1 || m > 12 || y < 1900 || y > 2100) {
        cout << "  [!] Invalid month or year.\n";
        return;
    }
    Report report(user);
    report.monthlySummary(m, y);
}

void yearlyUI(const User &user) {
    int y = readInt("\n  Year (e.g. 2026): ");
    if (y < 1900 || y > 2100) {
        cout << "  [!] Invalid year.\n";
        return;
    }
    Report report(user);
    report.yearlyOverview(y);
}

void showMenu(const User &user) {
    cout << "\n  ==========================================\n";
    cout << "     PERSONAL EXPENSE TRACKER\n";
    cout << "     User: " << user.getName() << "\n";
    cout << "  ==========================================\n";
    cout << "    1. Add Expense\n";
    cout << "    2. View All Expenses\n";
    cout << "    3. Delete Expense\n";
    cout << "    4. Add Category\n";
    cout << "    5. View Categories\n";
    cout << "    6. Monthly Summary Report\n";
    cout << "    7. Yearly Overview\n";
    cout << "    8. Save & Exit\n";
    cout << "  ==========================================\n";
}

int main() {
    const string DATA_FILE = "expense_data.txt";
    User user;

    if (user.loadFromFile(DATA_FILE)) {
        cout << "\n  Welcome back, " << user.getName() << "! Data loaded.\n";
    } else {
        string name = readLine("\n  Welcome! Enter your name: ");
        if (name.empty()) name = "Guest";
        user.setName(name);
        user.loadDefaultCategories();
        cout << "  Profile created with default categories.\n";
    }

    int choice;
    do {
        showMenu(user);
        choice = readInt("  Enter your choice: ");

        switch (choice) {
            case 1: addExpenseUI(user); break;
            case 2: user.displayAllExpenses(); break;
            case 3: deleteExpenseUI(user); break;
            case 4: addCategoryUI(user); break;
            case 5: user.displayCategories(); break;
            case 6: summaryUI(user); break;
            case 7: yearlyUI(user); break;
            case 8:
                if (user.saveToFile(DATA_FILE))
                    cout << "\n  Data saved to '" << DATA_FILE << "'. Goodbye!\n";
                else
                    cout << "\n  [!] Could not save data. Goodbye!\n";
                break;
            default:
                cout << "  [!] Invalid choice. Please select 1-8.\n";
        }
    } while (choice != 8);

    return 0;
}