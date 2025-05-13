#include "Stronghold.h"
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <limits>
#include <cctype>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <algorithm>
#include <regex>
#include <fstream> // For file handling

using namespace std;

// Helper Functions
void clearScreen() {
    try {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    catch (const std::exception& e) {
        cerr << "Error clearing screen: " << e.what() << endl;
    }
}

string toLowerCase(const string& str) {
    try {
        string result = str;
        for (char& c : result) {
            c = std::tolower(c);
        }
        return result;
    }
    catch (const std::exception& e) {
        cerr << "Error in toLowerCase: " << e.what() << endl;
        return str; // Return original string on error
    }
}

// Function to check if string contains only alphabets and spaces
bool isValidName(const string& name) {
    if (name.empty()) return false;

    return std::all_of(name.begin(), name.end(), [](char c) {
        return std::isalpha(c) || std::isspace(c);
        });
}

// Function to get name input (only alphabets and spaces)
string getNameInput(const string& prompt) {
    string value;
    cout << prompt;
    while (true) {
        getline(cin, value);
        if (isValidName(value)) {
            return value;
        }
        cout << "Invalid input. Names can only contain letters and spaces. Try again: ";
    }
}

// Function to get integer input within range
int getRangedIntInput(const string& prompt, int min, int max) {
    int value;
    string input;
    cout << prompt << " (" << min << "-" << max << "): ";

    while (true) {
        getline(cin, input);

        // Check if input contains only digits
        bool validDigits = !input.empty() && std::all_of(input.begin(), input.end(), ::isdigit);

        if (!validDigits) {
            cout << "Invalid input. Please enter only numbers: ";
            continue;
        }

        try {
            value = stoi(input);
            if (value >= min && value <= max) {
                return value;
            }
            else {
                cout << "Input must be between " << min << " and " << max << ". Try again: ";
            }
        }
        catch (const std::exception&) {
            cout << "Invalid number. Try again: ";
        }
    }
}

// Function to get double input within range
double getRangedDoubleInput(const string& prompt, double min, double max) {
    double value;
    string input;
    cout << prompt << " (" << min << "-" << max << "): ";

    while (true) {
        getline(cin, input);

        // Check if input is a valid floating point number format
        std::regex pattern("^[0-9]*\\.?[0-9]+$");
        if (!std::regex_match(input, pattern)) {
            cout << "Invalid input. Please enter only numeric values: ";
            continue;
        }

        try {
            value = stod(input);
            if (value >= min && value <= max) {
                return value;
            }
            else {
                cout << "Input must be between " << min << " and " << max << ". Try again: ";
            }
        }
        catch (const std::exception&) {
            cout << "Invalid number. Try again: ";
        }
    }
}

// Legacy input functions preserved for backward compatibility
int getIntInput(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

double getDoubleInput(const string& prompt) {
    double value;
    cout << prompt;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

string getStringInput(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

// Game initialization function
unique_ptr<Kingdom> initializeGame() {
    try {
        clearScreen();
        cout << "===============================================\n";
        cout << "                STRONGHOLD\n";
        cout << "            A Kingdom Simulator\n";
        cout << "===============================================\n\n";

        cout << "Welcome, noble ruler. You are about to embark on a\n";
        cout << "challenging journey to build and maintain a thriving\n";
        cout << "medieval kingdom against all odds.\n\n";

        string kingdomName = getNameInput("Enter the name of your kingdom: ");
        string rulerName = getNameInput("Enter your name, the king: ");

        cout << "\nInitializing " << kingdomName << " under the rule of King " << rulerName << "...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));

        auto kingdom = std::make_unique<Kingdom>(kingdomName);

        // Create and elect first king
        std::unique_ptr<King> king = std::make_unique<King>(
            rulerName, 50, 20, 50, "Benevolent"
        );
        kingdom->getPolitics()->electKing(std::move(king));

        return kingdom;
    }
    catch (const GameException& e) {
        cerr << "Game initialization error: " << e.what() << endl;
        throw; // Re-throw to let main handle it
    }
    catch (const std::exception& e) {
        cerr << "Unexpected error during initialization: " << e.what() << endl;
        throw GameException("Failed to initialize game: " + string(e.what()));
    }
}

// Display main menu
void displayMainMenu() {
    try {
        cout << "\n============= MAIN MENU =============\n";
        cout << "1. View Kingdom Status\n";
        cout << "2. Manage Resources\n";
        cout << "3. Manage Army\n";
        cout << "4. Manage Economy\n";
        cout << "5. Manage Politics\n";
        cout << "6. Advance Turn\n";
        cout << "7. Save Game\n";
        cout << "8. Load Game\n";
        cout << "9. Exit Game\n";
        cout << "====================================\n";
        cout << "Enter your choice: ";
    }
    catch (const std::exception& e) {
        cerr << "Error displaying menu: " << e.what() << endl;
    }
}

// Resource management menu
void manageResources(Kingdom& kingdom) {
    while (true) {
        try {
            clearScreen();
            cout << "\n========== RESOURCE MANAGEMENT ==========\n";

            // Display current resources
            cout << "Current Resources:\n";
            auto woodResource = kingdom.getResource("wood");
            auto stoneResource = kingdom.getResource("stone");
            auto ironResource = kingdom.getResource("iron");
            auto goldResource = kingdom.getResource("gold");
            auto foodResource = kingdom.getResource("food");
            auto weaponsResource = kingdom.getResource("weapons");

            if (woodResource) cout << "1. Wood: " << woodResource->getQuantity() << "\n";
            if (stoneResource) cout << "2. Stone: " << stoneResource->getQuantity() << "\n";
            if (ironResource) cout << "3. Iron: " << ironResource->getQuantity() << "\n";
            if (goldResource) cout << "4. Gold: " << goldResource->getQuantity() << "\n";
            if (foodResource) cout << "5. Food: " << foodResource->getQuantity() << "\n";
            if (weaponsResource) cout << "6. Weapons: " << weaponsResource->getQuantity() << "\n";

            cout << "\nActions:\n";
            cout << "7. Buy Resources\n";
            cout << "8. Sell Resources\n";
            cout << "9. Gather Resources\n";
            cout << "10. Back to Main Menu\n";
            cout << "=========================================\n";

            int choice = getRangedIntInput("Enter your choice", 7, 10);

            try {
                if (choice == 10) {
                    break;
                }
                else if (choice == 7) {
                    // Buy resources
                    string resourceOptions = "wood, stone, iron, gold, food, weapons";
                    string resourceName;
                    bool validResource = false;

                    while (!validResource) {
                        resourceName = getStringInput("Enter resource name to buy (" + resourceOptions + "): ");
                        resourceName = toLowerCase(resourceName);
                        if (resourceName == "wood" || resourceName == "stone" || resourceName == "iron" ||
                            resourceName == "gold" || resourceName == "food" || resourceName == "weapons") {
                            validResource = true;
                        }
                        else {
                            cout << "Invalid resource name. Please choose from: " << resourceOptions << endl;
                        }
                    }

                    int amount = getRangedIntInput("Enter amount to buy", 1, 1000);

                    double cost = kingdom.getMarket()->buyResource(resourceName, amount, *kingdom.getBank());
                    auto resource = kingdom.getResource(resourceName);
                    if (resource) {
                        resource->addQuantity(amount);
                        cout << "Purchased " << amount << " " << resourceName << " for " << cost << " gold\n";
                    }
                    else {
                        throw GameException("Resource not found");
                    }
                }
                else if (choice == 8) {
                    // Sell resources
                    string resourceOptions = "wood, stone, iron, gold, food, weapons";
                    string resourceName;
                    bool validResource = false;

                    while (!validResource) {
                        resourceName = getStringInput("Enter resource name to sell (" + resourceOptions + "): ");
                        resourceName = toLowerCase(resourceName);
                        if (resourceName == "wood" || resourceName == "stone" || resourceName == "iron" ||
                            resourceName == "gold" || resourceName == "food" || resourceName == "weapons") {
                            validResource = true;
                        }
                        else {
                            cout << "Invalid resource name. Please choose from: " << resourceOptions << endl;
                        }
                    }

                    auto resource = kingdom.getResource(resourceName);
                    if (resource) {
                        int maxAmount = resource->getQuantity();
                        if (maxAmount <= 0) {
                            throw GameException("You don't have any " + resourceName + " to sell");
                        }

                        int amount = getRangedIntInput("Enter amount to sell", 1, maxAmount);

                        double revenue = kingdom.getMarket()->sellResource(resourceName, amount, *kingdom.getBank());
                        resource->consumeQuantity(amount);
                        cout << "Sold " << amount << " " << resourceName << " for " << revenue << " gold\n";
                    }
                    else {
                        throw GameException("Resource not found");
                    }
                }
                else if (choice == 9) {
                    // Gather resources - simulates resource production
                    cout << "Gathering resources...\n";

                    // This would be a more complex system in a full game
                    // For now, just add some basic resources
                    if (woodResource) woodResource->addQuantity(10);
                    if (stoneResource) stoneResource->addQuantity(5);
                    if (ironResource) ironResource->addQuantity(2);
                    if (foodResource) foodResource->addQuantity(20);

                    cout << "Resources gathered!\n";
                }
            }
            catch (const GameException& e) {
                cout << "\nError: " << e.what() << endl;
            }
            catch (const std::exception& e) {
                cout << "\nUnexpected error: " << e.what() << endl;
            }

            cout << "\nPress Enter to continue...";
            cin.get();
        }
        catch (const std::exception& e) {
            cerr << "Fatal error in resource management: " << e.what() << endl;
            cout << "\nPress Enter to return to main menu...";
            cin.get();
            break;
        }
    }
}

// Army management menu
void manageArmy(Kingdom& kingdom) {
    while (true) {
        try {
            clearScreen();
            cout << "\n========== ARMY MANAGEMENT ==========\n";

            Army* army = kingdom.getArmy();
            if (army) {
                cout << "Army Status:\n";
                cout << "- Size: " << army->getSize() << "\n";
                cout << "- Training Level: " << army->getTrainingLevel() << "\n";
                cout << "- Morale: " << army->getMorale() << "%\n";
                cout << "- Maintenance Cost: " << army->getMaintenanceCost() << " gold\n";

                Commander* commander = army->getCommander();
                if (commander) {
                    cout << "- Commander: " << commander->getName() << "\n";
                    cout << "  - Leadership: " << commander->getLeadership() << "\n";
                    cout << "  - Strategy: " << commander->getStrategyBonus() << "\n";
                    cout << "  - Loyalty: " << (commander->isLoyal() ? "Loyal" : "Questionable") << "\n";
                }
                else {
                    cout << "- No Commander\n";
                }
            }
            else {
                cout << "No army available.\n";
            }

            cout << "\nActions:\n";
            cout << "1. Recruit Soldiers\n";
            cout << "2. Train Army\n";
            cout << "3. Pay Maintenance\n";
            cout << "4. Appoint Commander\n";
            cout << "5. Go to War (simulation)\n";
            cout << "0. Back to Main Menu\n";
            cout << "======================================\n";

            int choice = getRangedIntInput("Enter your choice", 0, 5);

            try {
                if (choice == 0) {
                    break;
                }
                else if (choice == 1) {
                    // Recruit soldiers
                    int population = kingdom.getPopulation().getTotalPopulation();
                    int maxRecruit = population / 2;
                    int count = getRangedIntInput("Enter number of soldiers to recruit", 1, maxRecruit);

                    army->recruit(count, population);

                    // Update population
                    kingdom.getPopulation().migrate(
                        SocialClass::PEASANT,
                        SocialClass::MILITARY,
                        std::min(count, kingdom.getPopulation().getClassPopulation(SocialClass::PEASANT))
                    );

                    cout << "Recruited " << count << " soldiers!\n";
                }
                else if (choice == 2) {
                    // Train army
                    int duration = getRangedIntInput("Enter training duration (in seconds)", 1, 5);

                    army->train(duration);
                    cout << "Army training complete!\n";
                }
                else if (choice == 3) {
                    // Pay maintenance
                    double maintenanceCost = army->getMaintenanceCost();
                    cout << "Army maintenance costs " << maintenanceCost << " gold\n";

                    if (kingdom.getBank()->withdraw(maintenanceCost)) {
                        army->payMaintenance(maintenanceCost);
                        cout << "Paid army maintenance successfully!\n";
                    }
                    else {
                        cout << "Not enough money to pay maintenance!\n";
                        army->payMaintenance(0);
                    }
                }
                else if (choice == 4) {
                    // Appoint commander
                    string name = getNameInput("Enter commander name: ");

                    int influence = getRangedIntInput("Enter influence", 1, 100);
                    int corruption = getRangedIntInput("Enter corruption", 1, 100);
                    int leadership = getRangedIntInput("Enter leadership", 1, 100);
                    int experience = getRangedIntInput("Enter battle experience", 1, 100);
                    int strategy = getRangedIntInput("Enter strategy skill", 1, 100);

                    string loyaltyStr;
                    bool loyal = false;
                    bool validInput = false;

                    while (!validInput) {
                        loyaltyStr = getStringInput("Is commander loyal? (yes/no): ");
                        loyaltyStr = toLowerCase(loyaltyStr);
                        if (loyaltyStr == "yes" || loyaltyStr == "no") {
                            validInput = true;
                            loyal = (loyaltyStr == "yes");
                        }
                        else {
                            cout << "Invalid input. Please enter 'yes' or 'no'.\n";
                        }
                    }

                    std::unique_ptr<Commander> commander = std::make_unique<Commander>(
                        name, influence, corruption, leadership, experience, strategy, loyal
                    );

                    army->setCommander(std::move(commander));
                    cout << "Commander appointed successfully!\n";
                }
                else if (choice == 5) {
                    // Simulate war
                    cout << "Simulating war...\n";

                    try {
                        // Create enemy kingdom for simulation
                        Kingdom enemyKingdom("Enemy Kingdom");
                        enemyKingdom.getArmy()->recruit(100, 1000);

                        kingdom.handleWar(enemyKingdom);
                    }
                    catch (const GameException& e) {
                        cout << "War simulation failed: " << e.what() << endl;
                    }
                }
            }
            catch (const GameException& e) {
                cout << "\nError: " << e.what() << endl;
            }
            catch (const std::exception& e) {
                cout << "\nUnexpected error: " << e.what() << endl;
            }

            cout << "\nPress Enter to continue...";
            cin.get();
        }
        catch (const std::exception& e) {
            cerr << "Fatal error in army management: " << e.what() << endl;
            cout << "\nPress Enter to return to main menu...";
            cin.get();
            break;
        }
    }
}

// Economy management menu
void manageEconomy(Kingdom& kingdom) {
    while (true) {
        try {
            clearScreen();
            cout << "\n========== ECONOMY MANAGEMENT ==========\n";

            Bank* bank = kingdom.getBank();
            if (bank) {
                cout << "Economy Status:\n";
                cout << "- Treasury: " << bank->getTreasury() << " gold\n";
                cout << "- Loan: " << bank->getLoanAmount() << " gold\n";
                if (bank->getLoanAmount() > 0) {
                    cout << "  - Interest Rate: " << bank->getInterestRate() * 100 << "%\n";
                    cout << "  - Due Time: " << bank->getLoanDueTime() << " turns\n";
                }
                cout << "- Corruption Level: " << bank->getCorruptionLevel() << "%\n";
            }
            else {
                cout << "Banking system not available.\n";
            }

            cout << "\nMarket Prices:\n";
            Market* market = kingdom.getMarket();
            if (market) {
                cout << "- Wood: " << market->getResourcePrice("wood") << " gold\n";
                cout << "- Stone: " << market->getResourcePrice("stone") << " gold\n";
                cout << "- Iron: " << market->getResourcePrice("iron") << " gold\n";
                cout << "- Gold: " << market->getResourcePrice("gold") << " gold\n";
                cout << "- Food: " << market->getResourcePrice("food") << " gold\n";
                cout << "- Weapons: " << market->getResourcePrice("weapons") << " gold\n";
            }

            cout << "\nActions:\n";
            cout << "1. Collect Taxes\n";
            cout << "2. Take Loan\n";
            cout << "3. Repay Loan\n";
            cout << "4. Audit Finances\n";
            cout << "5. Adjust Market (Open/Close)\n";
            cout << "0. Back to Main Menu\n";
            cout << "=======================================\n";

            int choice = getRangedIntInput("Enter your choice", 0, 5);

            try {
                if (choice == 0) {
                    break;
                }
                else if (choice == 1) {
                    // Collect taxes
                    double taxRate = getRangedDoubleInput("Enter tax rate", 0.0, 1.0);

                    kingdom.collectTaxes(taxRate);
                    cout << "Taxes collected!\n";
                }
                else if (choice == 2) {
                    // Take loan
                    if (bank->getLoanAmount() > 0) {
                        throw GameException("You already have an outstanding loan");
                    }

                    double amount = getRangedDoubleInput("Enter loan amount", 1.0, 10000.0);
                    double rate = getRangedDoubleInput("Enter interest rate", 0.01, 0.5);
                    int dueTime = getRangedIntInput("Enter due time (in turns)", 1, 50);

                    bank->getLoan(amount, rate, dueTime);
                    cout << "Loan taken successfully!\n";
                }
                else if (choice == 3) {
                    // Repay loan
                    if (bank->getLoanAmount() <= 0) {
                        cout << "No active loans to repay.\n";
                    }
                    else {
                        double maxAmount = std::min(bank->getLoanAmount(), bank->getTreasury());
                        if (maxAmount <= 0) {
                            throw GameException("You don't have any gold to repay the loan");
                        }

                        double amount = getRangedDoubleInput("Enter repayment amount", 0.1, maxAmount);

                        if (bank->repayLoan(amount)) {
                            cout << "Loan repayment successful!\n";
                            if (bank->getLoanAmount() <= 0) {
                                cout << "Loan fully repaid!\n";
                            }
                        }
                        else {
                            cout << "Not enough money to repay loan!\n";
                        }
                    }
                }
                else if (choice == 4) {
                    // Audit finances
                    bool foundCorruption = bank->audit();
                    if (foundCorruption) {
                        cout << "Corruption detected and reduced!\n";
                    }
                    else {
                        cout << "No corruption detected in your finances.\n";
                    }
                }
                else if (choice == 5) {
                    // Adjust market
                    string action;
                    bool validInput = false;

                    while (!validInput) {
                        action = getStringInput("Open or close market? (open/close): ");
                        action = toLowerCase(action);
                        if (action == "open" || action == "close") {
                            validInput = true;
                        }
                        else {
                            cout << "Invalid input. Please enter 'open' or 'close'.\n";
                        }
                    }

                    if (action == "open") {
                        market->open();
                        cout << "Market is now open!\n";
                    }
                    else {
                        market->close();
                        cout << "Market is now closed!\n";
                    }
                }
            }
            catch (const GameException& e) {
                cout << "\nError: " << e.what() << endl;
            }
            catch (const std::exception& e) {
                cout << "\nUnexpected error: " << e.what() << endl;
            }

            cout << "\nPress Enter to continue...";
            cin.get();
        }
        catch (const std::exception& e) {
            cerr << "Fatal error in economy management: " << e.what() << endl;
            cout << "\nPress Enter to return to main menu...";
            cin.get();
            break;
        }
    }
}

// Politics management menu
void managePolitics(Kingdom& kingdom) {
    while (true) {
        try {
            clearScreen();
            cout << "\n========== POLITICS MANAGEMENT ==========\n";

            Politics* politics = kingdom.getPolitics();
            if (politics) {
                cout << "Political Status:\n";
                cout << "- Current King: " << (politics->getCurrentKing() ? politics->getCurrentKing()->getName() : "None") << "\n";
                if (politics->getCurrentKing()) {
                    cout << "  - Leadership: " << politics->getCurrentKing()->getLeadership() << "\n";
                    cout << "  - Influence: " << politics->getCurrentKing()->getInfluence() << "\n";
                    cout << "  - Corruption: " << politics->getCurrentKing()->getCorruption() << "\n";
                }
                cout << "- Stability: " << politics->getStability() << "%\n";
                cout << "- Civil Unrest: " << (politics->hasCivilUnrest() ? "Yes" : "No") << "\n";
                cout << "- At War: " << (politics->isAtWar() ? "Yes" : "No") << "\n";
            }
            else {
                cout << "Political system not available.\n";
            }

            cout << "\nActions:\n";
            cout << "1. Elect New King\n";
            cout << "2. Form Alliance\n";
            cout << "3. Break Alliance\n";
            cout << "4. Declare War\n";
            cout << "5. Make Peace\n";
            cout << "0. Back to Main Menu\n";
            cout << "=======================================\n";

            int choice = getRangedIntInput("Enter your choice", 0, 5);

            try {
                if (choice == 0) {
                    break;
                }
                else if (choice == 1) {
                    // Elect new king
                    string name = getNameInput("Enter name for new king: ");

                    int influence = getRangedIntInput("Enter influence", 1, 100);
                    int corruption = getRangedIntInput("Enter corruption", 1, 100);
                    int leadership = getRangedIntInput("Enter leadership", 1, 100);

                    string style;
                    bool validStyle = false;

                    while (!validStyle) {
                        style = getStringInput("Enter leadership style (Benevolent/Militaristic/Economic): ");
                        if (style == "Benevolent" || style == "Militaristic" || style == "Economic") {
                            validStyle = true;
                        }
                        else {
                            cout << "Invalid leadership style. Please enter Benevolent, Militaristic, or Economic.\n";
                        }
                    }

                    std::unique_ptr<King> newKing = std::make_unique<King>(
                        name, influence, corruption, leadership, style
                    );

                    politics->electKing(std::move(newKing));
                    cout << "New king elected!\n";
                }
                else if (choice == 2) {
                    // Form alliance
                    string kingdomName = getNameInput("Enter kingdom name to form alliance with: ");

                    if (kingdomName == kingdom.getName()) {
                        throw GameException("Cannot form alliance with your own kingdom");
                    }

                    politics->formAlliance(kingdomName);
                    cout << "Alliance formed with " << kingdomName << "!\n";
                }
                else if (choice == 3) {
                    // Break alliance
                    string kingdomName = getNameInput("Enter kingdom name to break alliance with: ");

                    politics->breakAlliance(kingdomName);
                    cout << "Alliance with " << kingdomName << " broken!\n";
                }
                else if (choice == 4) {
                    // Declare war
                    if (politics->isAtWar()) {
                        throw GameException("Already at war with another kingdom");
                    }

                    string kingdomName = getNameInput("Enter kingdom name to declare war on: ");

                    if (kingdomName == kingdom.getName()) {
                        throw GameException("Cannot declare war on your own kingdom");
                    }

                    politics->declareWar(kingdomName);
                    cout << "War declared on " << kingdomName << "!\n";
                }
                else if (choice == 5) {
                    // Make peace
                    if (!politics->isAtWar()) {
                        throw GameException("Not currently at war with any kingdom");
                    }

                    string kingdomName = getNameInput("Enter kingdom name to make peace with: ");

                    politics->makePeace(kingdomName);
                    cout << "Peace made with " << kingdomName << "!\n";
                }
            }
            catch (const GameException& e) {
                cout << "\nError: " << e.what() << endl;
            }
            catch (const std::exception& e) {
                cout << "\nUnexpected error: " << e.what() << endl;
            }

            cout << "\nPress Enter to continue...";
            cin.get();
        }
        catch (const std::exception& e) {
            cerr << "Fatal error in politics management: " << e.what() << endl;
            cout << "\nPress Enter to return to main menu...";
            cin.get();
            break;
        }
    }
}

// Update saveGame function for Visual Studio 2022
void saveGame(Kingdom& kingdom) {
    try {
        string filename = getNameInput("Enter filename to save game: ");

        // Add a default extension if none provided
        if (filename.find('.') == string::npos) {
            filename += ".sav";
        }

        cout << "Saving game to \"" << filename << "\"...\n";
        kingdom.saveGameState(filename);
        cout << "Game saved successfully!\n";
    }
    catch (const GameException& e) {
        cout << "Save error: " << e.what() << endl;
    }
    catch (const std::exception& e) {
        cout << "Unexpected error while saving: " << e.what() << endl;
    }

    cout << "Press Enter to continue...";
    cin.get();
}

// Update loadGame function to improve error handling
unique_ptr<Kingdom> loadGame() {
    try {
        string filename = getNameInput("Enter filename to load game: ");

        // Add a default extension if none provided
        if (filename.find('.') == string::npos) {
            filename += ".sav";
        }

        cout << "Loading game from \"" << filename << "\"...\n";

        // Check if file exists before attempting to validate or load
        std::ifstream checkFile(filename);
        if (!checkFile.is_open()) {
            cout << "Error: File \"" << filename << "\" does not exist or cannot be opened" << endl;
            cout << "Make sure the file exists in the current directory." << endl;
            cout << "Press Enter to continue...";
            cin.get();
            return nullptr;
        }
        checkFile.close();

        if (!Kingdom::validateSaveFile(filename)) {
            cout << "Error: \"" << filename << "\" is not a valid save file" << endl;
            cout << "Press Enter to continue...";
            cin.get();
            return nullptr;
        }

        // Create a new kingdom with a temporary name and then load the saved data
        auto kingdom = make_unique<Kingdom>("TempKingdom");
        kingdom->loadGameState(filename);

        cout << "Game loaded successfully!\n";
        cout << "Press Enter to continue...";
        cin.get();

        return kingdom;
    }
    catch (const GameException& e) {
        cout << "Load error: " << e.what() << endl;
    }
    catch (const std::exception& e) {
        cout << "Unexpected error while loading: " << e.what() << endl;
    }

    cout << "Press Enter to continue...";
    cin.get();
    return nullptr;
}

// Main game loop
int main() {
    try {
        // Seed random number generator
        srand(static_cast<unsigned int>(time(nullptr)));

        // Initialize game
        std::unique_ptr<Kingdom> kingdom;
        try {
            kingdom = initializeGame();
        }
        catch (const std::exception& e) {
            cerr << "Failed to initialize game: " << e.what() << endl;
            return 1;
        }

        bool running = true;

        // Main game loop
        while (running && !kingdom->isGameOver()) {
            try {
                clearScreen();
                kingdom->displayStatus();
                displayMainMenu();

                int choice;
                string input;
                bool validChoice = false;

                while (!validChoice) {
                    getline(cin, input);

                    // Check if input is a single digit
                    if (input.length() == 1 && isdigit(input[0])) {
                        choice = input[0] - '0';  // Convert char to int
                        if (choice >= 0 && choice <= 9) {
                            validChoice = true;
                        }
                        else {
                            cout << "Invalid choice. Please enter a number between 1-9 (or 0): ";
                        }
                    }
                    else {
                        cout << "Invalid input. Please enter a single digit: ";
                    }
                }

                try {
                    switch (choice) {
                    case 1: // View kingdom status
                        clearScreen();
                        kingdom->displayStatus();
                        cout << "Press Enter to continue...";
                        cin.get();
                        break;

                    case 2: // Manage resources
                        manageResources(*kingdom);
                        break;

                    case 3: // Manage army
                        manageArmy(*kingdom);
                        break;

                    case 4: // Manage economy
                        manageEconomy(*kingdom);
                        break;

                    case 5: // Manage politics
                        managePolitics(*kingdom);
                        break;

                    case 6: // Advance turn
                        try {
                            cout << "\nAdvancing to next turn...\n";
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                            kingdom->processTurn();
                            cout << "Turn processed successfully!\n";
                        }
                        catch (const GameException& e) {
                            cout << "Error processing turn: " << e.what() << endl;
                        }
                        cout << "Press Enter to continue...";
                        cin.get();
                        break;

                    case 7: // Save game
                        saveGame(*kingdom);
                        break;

                    case 8: // Load game
                    {
                        auto loadedKingdom = loadGame();
                        if (loadedKingdom) {
                            kingdom = std::move(loadedKingdom);
                        }
                    }
                    break;

                    case 9: // Exit game
                        running = false;
                        break;

                    default:
                        cout << "Invalid choice. Please try again.\n";
                        cout << "Press Enter to continue...";
                        cin.get();
                    }
                }
                catch (const GameException& e) {
                    cout << "\nError: " << e.what() << endl;
                    cout << "Press Enter to continue...";
                    cin.get();
                }
                catch (const std::exception& e) {
                    cout << "\nUnexpected error: " << e.what() << endl;
                    cout << "Press Enter to continue...";
                    cin.get();
                }

                // Check for game over condition
                if (kingdom->isGameOver()) {
                    try {
                        clearScreen();
                        cout << "\n===============================================\n";
                        cout << "                GAME OVER\n";
                        cout << "===============================================\n\n";

                        cout << "Your reign has come to an end!\n";
                        cout << "Final kingdom status:\n";
                        kingdom->displayStatus();

                        cout << "Press Enter to exit...";
                        cin.get();
                    }
                    catch (const std::exception& e) {
                        cerr << "Error displaying game over screen: " << e.what() << endl;
                    }
                }
            }
            catch (const std::exception& e) {
                cerr << "Fatal error in game loop: " << e.what() << endl;
                cout << "\nPress Enter to try to continue...";
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cin.get();
            }
        }

        cout << "\nThank you for playing Stronghold!\n";
        return 0;
    }
    catch (const std::exception& e) {
        cerr << "CRITICAL ERROR: " << e.what() << endl;
        cout << "The game has encountered a critical error and must exit." << endl;
        cout << "Press Enter to exit...";
        cin.get();
        return 1;
    }
}