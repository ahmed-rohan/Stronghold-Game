#include "Stronghold.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>  // Add this line to include the string stream functionality

#ifdef _HAS_CXX17
#include <filesystem>
#endif

using namespace std;

// Population Implementation
Population::Population(int initialPopulation) :
    totalPopulation(initialPopulation),
    happiness(50.0),
    growthRate(5),
    deathRate(3),
    plagueActive(false) {

    // Initialize class demographics
    classDemographics[SocialClass::PEASANT] = static_cast<int>(initialPopulation * 0.7); // 70% peasants
    classDemographics[SocialClass::MERCHANT] = static_cast<int>(initialPopulation * 0.15); // 15% merchants
    classDemographics[SocialClass::NOBILITY] = static_cast<int>(initialPopulation * 0.1); // 10% nobility
    classDemographics[SocialClass::MILITARY] = static_cast<int>(initialPopulation * 0.05); // 5% military
}

void Population::update(bool hasFood, bool hasHealthcare, int jobAvailability) {
    // Calculate growth based on conditions
    int growth = 0;
    int deaths = 0;

    if (hasFood && hasHealthcare) {
        growth = static_cast<int>(totalPopulation * growthRate / 100.0);
    }

    // More deaths if there's no food or healthcare
    if (!hasFood) {
        deaths += static_cast<int>(totalPopulation * 0.1); // 10% die from starvation
    }

    if (!hasHealthcare) {
        deaths += static_cast<int>(totalPopulation * 0.05); // 5% die from disease
    }

    // Normal death rate
    deaths += static_cast<int>(totalPopulation * deathRate / 100.0);

    // If plague is active, much higher death rate
    if (plagueActive) {
        deaths += static_cast<int>(totalPopulation * 0.2); // 20% die from plague
    }

    // Limited job availability reduces happiness
    if (jobAvailability < totalPopulation / 2) {
        happiness -= 5.0;
    }
    else {
        happiness += 2.0;
    }

    // Update total population
    totalPopulation += growth - deaths;
    if (totalPopulation < 0) totalPopulation = 0;

    // Clamp happiness between 0 and 100
    happiness = std::max(0.0, std::min(happiness, 100.0));

    // Update class demographics
    for (auto& pair : classDemographics) {
        int classGrowth = static_cast<int>((growth * pair.second) / totalPopulation);
        int classDeaths = static_cast<int>((deaths * pair.second) / totalPopulation);
        pair.second += classGrowth - classDeaths;
        if (pair.second < 0) pair.second = 0;
    }
}

void Population::triggerPlague() {
    plagueActive = true;
    happiness -= 30.0;
    if (happiness < 0) happiness = 0;
}

void Population::endPlague() {
    plagueActive = false;
}

bool Population::isUnhappy() const {
    return happiness < 30.0;
}

int Population::getTotalPopulation() const {
    return totalPopulation;
}

int Population::getClassPopulation(SocialClass socialClass) const {
    auto it = classDemographics.find(socialClass);
    if (it != classDemographics.end()) {
        return it->second;
    }
    return 0;
}

double Population::getHappiness() const {
    return happiness;
}

void Population::adjustHappiness(double amount) {
    happiness += amount;
    happiness = std::max(0.0, std::min(happiness, 100.0));
}

void Population::migrate(SocialClass fromClass, SocialClass toClass, int amount) {
    if (classDemographics[fromClass] < amount) {
        amount = classDemographics[fromClass];
    }

    classDemographics[fromClass] -= amount;
    classDemographics[toClass] += amount;
}

// Leader Implementation
Leader::Leader(const std::string& name, int influence, int corruption, int leadership) :
    name(name), influence(influence), corruption(corruption), leadership(leadership) {}

std::string Leader::getName() const {
    return name;
}

int Leader::getInfluence() const {
    return influence;
}

int Leader::getCorruption() const {
    return corruption;
}

int Leader::getLeadership() const {
    return leadership;
}

// King Implementation
King::King(const std::string& name, int influence, int corruption, int leadership, const std::string& style) :
    Leader(name, influence, corruption, leadership),
    reignYears(0), popularity(50), leadershipStyle(style) {}

void King::makeDecision(Kingdom& kingdom) {
    // King's decision logic based on leadership style
    if (leadershipStyle == "Benevolent") {
        // Benevolent kings focus on population happiness
        kingdom.getPopulation().adjustHappiness(5.0);
    }
    else if (leadershipStyle == "Militaristic") {
        // Militaristic kings focus on army strength
        if (kingdom.getArmy()) {
            kingdom.getArmy()->train(1);
        }
    }
    else if (leadershipStyle == "Economic") {
        // Economic kings focus on treasury
        if (kingdom.getBank()) {
            kingdom.getBank()->deposit(100 * leadership);
        }
    }

    // Corrupt kings will steal from treasury
    if (kingdom.getBank() && corruption > 50) {
        double stolenAmount = kingdom.getBank()->getTreasury() * (corruption * 0.01) * 0.1;
        kingdom.getBank()->withdraw(stolenAmount);
    }
}

std::string King::getTitle() const {
    return "King";
}

void King::setTaxRate(double rate) {
    // Implementation depends on Kingdom class integration
}

void King::declareWar(Kingdom& targetKingdom) {
    // Implementation depends on Kingdom class integration
}

bool King::canBeBribes(int goldAmount) const {
    return goldAmount > (100 - corruption) * 10;
}

// Commander Implementation
Commander::Commander(const std::string& name, int influence, int corruption, int leadership,
    int experience, int strategy, bool loyalty) :
    Leader(name, influence, corruption, leadership),
    battleExperience(experience), strategySkill(strategy), loyal(loyalty) {}

void Commander::makeDecision(Kingdom& kingdom) {
    // Commander's decision logic
    if (!loyal && corruption > 70) {
        // Disloyal commanders might plan coup
        cout << getName() << " is plotting against the king!\n";
        // Coup logic would be implemented in full game
    }

    // Boost army training based on experience
    if (kingdom.getArmy()) {
        int trainingBonus = battleExperience / 10;
        // Implement bonus to army
    }
}

std::string Commander::getTitle() const {
    return "Commander";
}

bool Commander::isLoyal() const {
    return loyal;
}

int Commander::getStrategyBonus() const {
    return strategySkill;
}

// MerchantGuildLeader Implementation
MerchantGuildLeader::MerchantGuildLeader(const std::string& name, int influence, int corruption, int leadership, double bonus) :
    Leader(name, influence, corruption, leadership), tradingBonus(bonus) {}

void MerchantGuildLeader::makeDecision(Kingdom& kingdom) {
    // Merchant's decision logic
    if (kingdom.getMarket()) {
        // Apply trading bonuses
    }

    // Corrupt merchant leaders might manipulate prices
    if (corruption > 60) {
        // Price manipulation logic
    }
}

std::string MerchantGuildLeader::getTitle() const {
    return "Merchant Guild Leader";
}

void MerchantGuildLeader::addTradeConnection(const std::string& connection) {
    tradeConnections.push_back(connection);
}

double MerchantGuildLeader::getTradingBonus() const {
    return tradingBonus;
}

// Army Implementation
Army::Army(int initialSize) :
    size(initialSize), trainingLevel(1), morale(50),
    maintenanceCost(initialSize * 2.0), isPaid(true) {}

Army::~Army() {}

void Army::recruit(int count, int populationSize) {
    if (count <= 0) {
        throw GameException("Cannot recruit a negative or zero number of soldiers");
    }

    if (count > populationSize * 0.2) {
        throw GameException("Cannot recruit more than 20% of the population");
    }

    // Simulating training time
    cout << "Training recruits";
    for (int i = 0; i < 3; i++) {
        cout << ".";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    cout << " Done!" << endl;

    size += count;
    maintenanceCost = size * 2.0;
}

void Army::train(int duration) {
    if (duration <= 0) {
        throw GameException("Training duration must be positive");
    }

    cout << "Training army";
    for (int i = 0; i < duration; i++) {
        cout << ".";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    cout << " Done!" << endl;

    trainingLevel += duration;
    if (trainingLevel > 10) trainingLevel = 10;
}

bool Army::battle(Army& enemyArmy) {
    if (size <= 0) {
        throw GameException("Cannot battle with no army");
    }

    // Simple battle simulation
    int ourStrength = size * trainingLevel * morale / 100;
    if (commander) {
        ourStrength += commander->getLeadership() * 10;
    }

    int enemyStrength = enemyArmy.getSize() * enemyArmy.getTrainingLevel() *
        enemyArmy.getMorale() / 100;
    if (enemyArmy.getCommander()) {
        enemyStrength += enemyArmy.getCommander()->getLeadership() * 10;
    }

    // Random factor
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(-20, 20);
    int randomFactor = distrib(gen);

    ourStrength += randomFactor;

    // Calculate casualties
    int ourCasualties = size * (0.1 + 0.4 * enemyStrength / (ourStrength > 0 ? ourStrength : 1));
    int enemyCasualties = enemyArmy.getSize() * (0.1 + 0.4 * ourStrength / enemyStrength);

    size -= ourCasualties;
    if (size < 0) size = 0;

    enemyArmy.size -= enemyCasualties;
    if (enemyArmy.size < 0) enemyArmy.size = 0;

    // Decrease morale after battle
    morale -= 10;
    if (morale < 0) morale = 0;

    enemyArmy.morale -= 10;
    if (enemyArmy.morale < 0) enemyArmy.morale = 0;

    return ourStrength > enemyStrength;
}

void Army::payMaintenance(double amount) {
    double requiredAmount = maintenanceCost;

    if (amount >= requiredAmount) {
        isPaid = true;
        morale += 10;
        if (morale > 100) morale = 100;
    }
    else {
        isPaid = false;
        morale -= 20;
        if (morale < 0) morale = 0;
    }
}

void Army::updateMorale(bool hasFood, bool isPaid) {
    if (hasFood && isPaid) {
        morale += 5;
    }
    else {
        morale -= 15;
    }

    if (morale < 0) morale = 0;
    if (morale > 100) morale = 100;
}

int Army::getSize() const {
    return size;
}

int Army::getTrainingLevel() const {
    return trainingLevel;
}

int Army::getMorale() const {
    return morale;
}

double Army::getMaintenanceCost() const {
    return maintenanceCost;
}

bool Army::getIsPaid() const {
    return isPaid;
}

void Army::setCommander(std::unique_ptr<Commander> newCommander) {
    commander = std::move(newCommander);
}

Commander* Army::getCommander() const {
    return commander.get();
}

// Bank Implementation
Bank::Bank(double initialTreasury) :
    treasury(initialTreasury), loanAmount(0),
    interestRate(0), loanDueTime(0), corruptionLevel(0) {}

bool Bank::withdraw(double amount) {
    if (amount <= 0) {
        throw EconomyException("Cannot withdraw a negative or zero amount");
    }

    if (amount > treasury) {
        return false;
    }

    treasury -= amount;
    return true;
}

void Bank::deposit(double amount) {
    if (amount <= 0) {
        throw EconomyException("Cannot deposit a negative or zero amount");
    }

    treasury += amount;
}

double Bank::getLoan(double amount, double rate, int dueTime) {
    if (amount <= 0) {
        throw EconomyException("Loan amount must be positive");
    }

    if (rate <= 0) {
        throw EconomyException("Interest rate must be positive");
    }

    if (dueTime <= 0) {
        throw EconomyException("Loan due time must be positive");
    }

    if (loanAmount > 0) {
        throw EconomyException("Cannot take another loan until current loan is repaid");
    }

    loanAmount = amount;
    interestRate = rate;
    loanDueTime = dueTime;
    treasury += amount;

    return amount;
}

bool Bank::repayLoan(double amount) {
    if (amount <= 0) {
        throw EconomyException("Repayment amount must be positive");
    }

    if (amount > treasury) {
        return false;
    }

    if (loanAmount <= 0) {
        throw EconomyException("No active loan to repay");
    }

    loanAmount -= amount;
    treasury -= amount;

    if (loanAmount < 0) {
        treasury -= loanAmount; // Return overpayment
        loanAmount = 0;
        interestRate = 0;
        loanDueTime = 0;
    }

    return true;
}

bool Bank::audit() {
    // Simulating audit time
    cout << "Auditing finances";
    for (int i = 0; i < 3; i++) {
        cout << ".";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    cout << " Done!" << endl;

    double stolenAmount = treasury * (corruptionLevel / 100.0) * 0.1;
    treasury -= stolenAmount;

    if (stolenAmount > 0) {
        cout << "Audit found " << stolenAmount << " gold was embezzled!" << endl;
        corruptionLevel -= 20;
        if (corruptionLevel < 0) corruptionLevel = 0;
        return true;
    }

    return false;
}

double Bank::getTreasury() const {
    return treasury;
}

double Bank::getLoanAmount() const {
    return loanAmount;
}

double Bank::getInterestRate() const {
    return interestRate;
}

int Bank::getLoanDueTime() const {
    return loanDueTime;
}

int Bank::getCorruptionLevel() const {
    return corruptionLevel;
}

void Bank::setCorruptionLevel(int level) {
    corruptionLevel = std::max(0, std::min(level, 100));
}

// Market Implementation
Market::Market() : inflationRate(0.02), tradingVolume(0), isOpen(true) {
    // Initialize prices
    prices["wood"] = 10.0;
    prices["stone"] = 20.0;
    prices["iron"] = 40.0;
    prices["gold"] = 100.0;
    prices["food"] = 15.0;
    prices["weapons"] = 50.0;
}

void Market::updatePrices() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(-0.1, 0.1);

    for (auto& price : prices) {
        double change = distrib(gen) + inflationRate;
        price.second *= (1 + change);
        if (price.second < 1.0) price.second = 1.0; // Minimum price
    }
}

double Market::buyResource(const std::string& resourceName, int amount, Bank& bank) {
    if (!isOpen) {
        throw GameException("Market is closed");
    }

    if (amount <= 0) {
        throw GameException("Cannot buy a negative or zero amount");
    }

    auto it = prices.find(resourceName);
    if (it == prices.end()) {
        throw GameException("Resource not available in market");
    }

    double cost = it->second * amount;

    if (!bank.withdraw(cost)) {
        throw GameException("Not enough money to buy resources");
    }

    tradingVolume += amount;
    return cost;
}

double Market::sellResource(const std::string& resourceName, int amount, Bank& bank) {
    if (!isOpen) {
        throw GameException("Market is closed");
    }

    if (amount <= 0) {
        throw GameException("Cannot sell a negative or zero amount");
    }

    auto it = prices.find(resourceName);
    if (it == prices.end()) {
        throw GameException("Resource not recognized in market");
    }

    double revenue = it->second * amount * 0.9; // 10% market fee
    bank.deposit(revenue);

    tradingVolume += amount;
    return revenue;
}

void Market::setInflationRate(double rate) {
    if (rate < 0) {
        throw EconomyException("Inflation rate cannot be negative");
    }

    inflationRate = rate;
}

double Market::getResourcePrice(const std::string& resourceName) const {
    auto it = prices.find(resourceName);
    if (it != prices.end()) {
        return it->second;
    }
    return 0.0;
}

void Market::open() {
    isOpen = true;
}

void Market::close() {
    isOpen = false;
}

bool Market::getIsOpen() const {
    return isOpen;
}

// Politics Implementation
Politics::Politics() : stability(50), civilUnrest(false), atWar(false) {}

Politics::~Politics() {}

void Politics::electKing(std::unique_ptr<King> newKing) {
    if (!newKing) {
        throw GameException("Invalid king");
    }

    cout << "Electing new king: " << newKing->getName() << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    currentKing = std::move(newKing);
    stability += 20;
    if (stability > 100) stability = 100;

    civilUnrest = false;
}

void Politics::coup(std::unique_ptr<King> usurper) {
    if (!usurper) {
        throw GameException("Invalid usurper");
    }

    cout << "Coup in progress! " << usurper->getName() << " is taking over the kingdom!" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    currentKing = std::move(usurper);
    stability -= 40;
    if (stability < 0) stability = 0;

    civilUnrest = true;
}

void Politics::declareWar(const std::string& enemyKingdom) {
    if (atWar) {
        throw GameException("Already at war");
    }

    cout << "Declaring war on " << enemyKingdom << endl;
    enemies.push_back(enemyKingdom);
    atWar = true;
    stability -= 10;
    if (stability < 0) stability = 0;
}

void Politics::makePeace(const std::string& kingdom) {
    auto it = std::find(enemies.begin(), enemies.end(), kingdom);
    if (it != enemies.end()) {
        enemies.erase(it);
        cout << "Peace treaty signed with " << kingdom << endl;
    }

    if (enemies.empty()) {
        atWar = false;
        stability += 15;
        if (stability > 100) stability = 100;
    }
}

void Politics::formAlliance(const std::string& kingdom) {
    auto it = std::find(enemies.begin(), enemies.end(), kingdom);
    if (it != enemies.end()) {
        throw GameException("Cannot form alliance with an enemy");
    }

    allies.push_back(kingdom);
    cout << "Alliance formed with " << kingdom << endl;
    stability += 5;
    if (stability > 100) stability = 100;
}

void Politics::breakAlliance(const std::string& kingdom) {
    auto it = std::find(allies.begin(), allies.end(), kingdom);
    if (it != allies.end()) {
        allies.erase(it);
        cout << "Alliance with " << kingdom << " is broken" << endl;
        stability -= 5;
        if (stability < 0) stability = 0;
    }
}

int Politics::getStability() const {
    return stability;
}

bool Politics::hasCivilUnrest() const {
    return civilUnrest;
}

bool Politics::isAtWar() const {
    return atWar;
}

void Politics::setCivilUnrest(bool unrest) {
    civilUnrest = unrest;
}

King* Politics::getCurrentKing() const {
    return currentKing.get();
}

// Kingdom Implementation
Kingdom::Kingdom(const std::string& name) : name(name), gameOver(false), currentTurn(1) {
    // Initialize components
    army = std::make_unique<Army>();
    bank = std::make_unique<Bank>();
    market = std::make_unique<Market>();
    politics = std::make_unique<Politics>();

    // Initialize resources
    initializeResources();
}

Kingdom::~Kingdom() {}

void Kingdom::initializeResources() {
    resources.emplace("wood", Resource<int>("wood", 100, 1000, 10.0));
    resources.emplace("stone", Resource<int>("stone", 50, 500, 20.0));
    resources.emplace("iron", Resource<int>("iron", 20, 200, 40.0));
    resources.emplace("gold", Resource<int>("gold", 10, 100, 100.0));
    resources.emplace("food", Resource<int>("food", 200, 2000, 5.0));
    resources.emplace("weapons", Resource<int>("weapons", 10, 100, 50.0));
}

void Kingdom::update() {
    // Check for random events
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 100);
    int eventChance = distrib(gen);

    if (eventChance <= 10) { // 10% chance of random event
        randomEvent();
    }

    // Update resources
    bool hasFood = resources.at("food").getQuantity() >= population.getTotalPopulation() / 10;

    // Update population
    int jobAvailability = population.getTotalPopulation() / 2; // Simplified job availability
    population.update(hasFood, resources.at("food").getQuantity() > 0, jobAvailability);

    // Consume food
    int foodNeeded = population.getTotalPopulation() / 10;
    if (resources.at("food").getQuantity() >= foodNeeded) {
        resources.at("food").consumeQuantity(foodNeeded);
    }
    else {
        // Not enough food
        cout << "Your kingdom is starving!" << endl;
        population.adjustHappiness(-10.0);
    }

    // Update army
    if (army) {
        army->updateMorale(hasFood, army->getIsPaid());

        // Check for military rebellion
        if (army->getMorale() < 20 && army->getSize() > 0) {
            cout << "Your army is rebelling due to low morale!" << endl;
            // Military coup logic
            if (politics && politics->getCurrentKing()) {
                cout << "The military has staged a coup!" << endl;
                auto commander = army->getCommander();
                if (commander) {
                    std::unique_ptr<King> militaryLeader = std::make_unique<King>(
                        commander->getName(), commander->getInfluence(),
                        commander->getCorruption(), commander->getLeadership(),
                        "Militaristic"
                    );
                    politics->coup(std::move(militaryLeader));
                }
            }
        }
    }

    // Update bank
    if (bank) {
        // Update loans
        if (bank->getLoanAmount() > 0) {
            bank->setCorruptionLevel(bank->getCorruptionLevel() + 1);
        }
    }

    // Update market
    if (market) {
        market->updatePrices();
    }

    // Update politics
    if (politics) {
        if (politics->getCurrentKing()) {
            politics->getCurrentKing()->makeDecision(*this);
        }

        if (population.isUnhappy() && politics->getStability() < 30) {
            cout << "The people are revolting!" << endl;
            politics->setCivilUnrest(true);

            // Chance of revolution
            if (eventChance <= 30) {
                cout << "Revolution! The king has been overthrown!" << endl;
                gameOver = true;
            }
        }
    }
}

void Kingdom::displayStatus() const {
    cout << "\n============ KINGDOM OF " << name << " ============\n";
    cout << "Population: " << population.getTotalPopulation() << " (Happiness: " << population.getHappiness() << "%)\n";
    cout << "- Peasants: " << population.getClassPopulation(SocialClass::PEASANT) << "\n";
    cout << "- Merchants: " << population.getClassPopulation(SocialClass::MERCHANT) << "\n";
    cout << "- Nobility: " << population.getClassPopulation(SocialClass::NOBILITY) << "\n";
    cout << "- Military: " << population.getClassPopulation(SocialClass::MILITARY) << "\n\n";

    cout << "Resources:\n";
    for (const auto& res : resources) {
        cout << "- " << res.second.getName() << ": " << res.second.getQuantity() << "\n";
    }

    cout << "\nArmy:\n";
    cout << "- Size: " << army->getSize() << "\n";
    cout << "- Training Level: " << army->getTrainingLevel() << "\n";
    cout << "- Morale: " << army->getMorale() << "%\n";
    cout << "- Maintenance Cost: " << army->getMaintenanceCost() << " gold\n";

    cout << "\nEconomy:\n";
    cout << "- Treasury: " << bank->getTreasury() << " gold\n";
    cout << "- Loan: " << bank->getLoanAmount() << " gold (Interest: " << bank->getInterestRate() * 100 << "%)\n";
    cout << "- Corruption Level: " << bank->getCorruptionLevel() << "%\n";

    cout << "\nPolitics:\n";
    cout << "- King: " << (politics->getCurrentKing() ? politics->getCurrentKing()->getName() : "None") << "\n";
    cout << "- Stability: " << politics->getStability() << "%\n";
    cout << "- At War: " << (politics->isAtWar() ? "Yes" : "No") << "\n";
    cout << "- Civil Unrest: " << (politics->hasCivilUnrest() ? "Yes" : "No") << "\n";
    cout << "=============================================\n\n";
}

void Kingdom::processTurn() {
    update();
    currentTurn++;
    displayStatus();
}

bool Kingdom::isGameOver() const {
    return gameOver;
}

std::string Kingdom::getName() const {
    return name;
}

Population& Kingdom::getPopulation() {
    return population;
}

Army* Kingdom::getArmy() const {
    return army.get();
}

Bank* Kingdom::getBank() const {
    return bank.get();
}

Market* Kingdom::getMarket() const {
    return market.get();
}

Politics* Kingdom::getPolitics() const {
    return politics.get();
}

Resource<int>* Kingdom::getResource(const std::string& name) {
    auto it = resources.find(name);
    if (it != resources.end()) {
        return &(it->second);
    }
    return nullptr;
}

void Kingdom::collectTaxes(double taxRate) {
    if (taxRate < 0 || taxRate > 1.0) {
        throw GameException("Tax rate must be between 0 and 1");
    }

    int totalPopulation = population.getTotalPopulation();
    int peasants = population.getClassPopulation(SocialClass::PEASANT);
    int merchants = population.getClassPopulation(SocialClass::MERCHANT);
    int nobility = population.getClassPopulation(SocialClass::NOBILITY);

    // Different tax rates for different classes (can be adjusted)
    double peasantTax = taxRate * 0.5;
    double merchantTax = taxRate * 2.0;
    double nobilityTax = taxRate * 5.0;

    double totalTax = peasants * peasantTax + merchants * merchantTax + nobility * nobilityTax;

    // Adjust happiness based on tax rate
    if (taxRate > 0.5) {
        population.adjustHappiness(-10.0 * (taxRate - 0.5) * 2); // Higher taxes decrease happiness
    }

    bank->deposit(totalTax);

    cout << "Taxes collected: " << totalTax << " gold" << endl;
}

void Kingdom::buildStructure(const std::string& structureName) {
    // Structure building logic
    // For now, a simple placeholder
    cout << "Building " << structureName << "..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cout << structureName << " completed!" << endl;
}

void Kingdom::handleWar(Kingdom& enemyKingdom) {
    if (!politics->isAtWar()) {
        politics->declareWar(enemyKingdom.getName());
    }

    cout << "War with " << enemyKingdom.getName() << " has begun!" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    bool victory = army->battle(*enemyKingdom.getArmy());

    if (victory) {
        cout << "Victory! " << enemyKingdom.getName() << " has been defeated!" << endl;
        // War reparations
        double reparations = enemyKingdom.getBank()->getTreasury() * 0.2;
        enemyKingdom.getBank()->withdraw(reparations);
        bank->deposit(reparations);

        politics->makePeace(enemyKingdom.getName());
    }
    else {
        cout << "Defeat! Your army has been beaten by " << enemyKingdom.getName() << "!" << endl;
        // Pay tribute
        double tribute = bank->getTreasury() * 0.2;
        bank->withdraw(tribute);
        enemyKingdom.getBank()->deposit(tribute);

        politics->makePeace(enemyKingdom.getName());
    }
}

void Kingdom::manageResources() {
    // Logic for resource management
    // Placeholder
}

// Update saveGameState method for Visual Studio 2022 compatibility
void Kingdom::saveGameState(const std::string& filename) const {
    try {
        std::ofstream saveFile(filename, std::ios::out | std::ios::trunc);
        if (!saveFile.is_open()) {
            throw GameException("Could not open file for saving: " + filename);
        }

        // Save basic kingdom info
        saveFile << "KINGDOM_DATA\n";
        saveFile << name << "\n";
        saveFile << currentTurn << "\n";
        saveFile << (gameOver ? "1" : "0") << "\n";

        // Save population data
        saveFile << "POPULATION_DATA\n";
        saveFile << population.getTotalPopulation() << "\n";
        saveFile << population.getHappiness() << "\n";
        saveFile << population.getClassPopulation(SocialClass::PEASANT) << "\n";
        saveFile << population.getClassPopulation(SocialClass::MERCHANT) << "\n";
        saveFile << population.getClassPopulation(SocialClass::NOBILITY) << "\n";
        saveFile << population.getClassPopulation(SocialClass::MILITARY) << "\n";

        // Save resources
        saveFile << "RESOURCES_DATA\n";
        for (const auto& res : resources) {
            saveFile << res.first << " "
                << res.second.getQuantity() << " "
                << res.second.getPrice() << "\n";
        }

        // Save army data
        saveFile << "ARMY_DATA\n";
        saveFile << army->getSize() << "\n";
        saveFile << army->getTrainingLevel() << "\n";
        saveFile << army->getMorale() << "\n";
        saveFile << army->getMaintenanceCost() << "\n";
        saveFile << (army->getIsPaid() ? "1" : "0") << "\n";

        // Save bank data
        saveFile << "BANK_DATA\n";
        saveFile << bank->getTreasury() << "\n";
        saveFile << bank->getLoanAmount() << "\n";
        saveFile << bank->getInterestRate() << "\n";
        saveFile << bank->getLoanDueTime() << "\n";
        saveFile << bank->getCorruptionLevel() << "\n";

        // Save market data
        saveFile << "MARKET_DATA\n";
        saveFile << (market->getIsOpen() ? "1" : "0") << "\n";

        // Save politics data
        saveFile << "POLITICS_DATA\n";
        saveFile << politics->getStability() << "\n";
        saveFile << (politics->hasCivilUnrest() ? "1" : "0") << "\n";
        saveFile << (politics->isAtWar() ? "1" : "0") << "\n";

        // Add king data if exists
        if (politics->getCurrentKing()) {
            saveFile << "KING_DATA\n";
            saveFile << politics->getCurrentKing()->getName() << "\n";
            saveFile << politics->getCurrentKing()->getInfluence() << "\n";
            saveFile << politics->getCurrentKing()->getCorruption() << "\n";
            saveFile << politics->getCurrentKing()->getLeadership() << "\n";
        }

        saveFile.close();
        if (saveFile.fail()) {
            throw GameException("Failed to close file after saving");
        }
        std::cout << "Game saved successfully to: " << filename << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving game: " << e.what() << std::endl;
        throw GameException("Failed to save game: " + std::string(e.what()));
    }
}

// Update loadGameState method to fix the string parsing issues
void Kingdom::loadGameState(const std::string& filename) {
    try {
        if (!validateSaveFile(filename)) {
            throw GameException("Invalid or missing save file: " + filename);
        }

        // Clear previous state - reset static variables
        static int popDataLine = 0;
        static int armyDataLine = 0;
        static int bankDataLine = 0;
        static int kingdomLine = 0;

        popDataLine = 0;
        armyDataLine = 0;
        bankDataLine = 0;
        kingdomLine = 0;

        std::ifstream loadFile(filename, std::ios::in);
        if (!loadFile.is_open()) {
            throw GameException("Could not open file for loading: " + filename);
        }

        std::string line, section;
        while (std::getline(loadFile, line)) {
            // Skip empty lines
            if (line.empty()) continue;

            // Check for section headers
            if (line == "KINGDOM_DATA" || line == "POPULATION_DATA" ||
                line == "RESOURCES_DATA" || line == "ARMY_DATA" ||
                line == "BANK_DATA" || line == "MARKET_DATA" ||
                line == "POLITICS_DATA" || line == "KING_DATA") {
                section = line;
                continue;
            }

            // Parse based on current section
            if (section == "KINGDOM_DATA") {
                if (kingdomLine == 0) {
                    name = line;
                    kingdomLine++;
                }
                else if (kingdomLine == 1) {
                    currentTurn = std::stoi(line);
                    kingdomLine++;
                }
                else if (kingdomLine == 2) {
                    gameOver = (line == "1");
                    kingdomLine = 0; // Reset for next time
                }
            }
            else if (section == "POPULATION_DATA") {
                if (popDataLine == 0) {
                    // Reset population with new total
                    int totalPop = std::stoi(line);
                    population = Population(totalPop);
                    popDataLine++;
                }
                else if (popDataLine == 1) {
                    // Set happiness
                    population.adjustHappiness(std::stod(line) - population.getHappiness());
                    popDataLine++;
                }
                else if (popDataLine == 2) {
                    // Set peasant population
                    int peasants = std::stoi(line);
                    popDataLine++;
                }
                else if (popDataLine == 3) {
                    // Set merchant population
                    int merchants = std::stoi(line);
                    popDataLine++;
                }
                else if (popDataLine == 4) {
                    // Set nobility population
                    int nobility = std::stoi(line);
                    popDataLine++;
                }
                else if (popDataLine == 5) {
                    // Set military population
                    int military = std::stoi(line);
                    popDataLine = 0; // Reset for next time
                }
            }
            else if (section == "RESOURCES_DATA") {
                // Fix this section - the issue was here
                std::istringstream iss(line);  // Create istringstream properly
                std::string resourceName;
                int quantity;
                double price;

                if (iss >> resourceName >> quantity >> price) {  // This now works with the istringstream
                    auto resource = getResource(resourceName);
                    if (resource) {
                        resource->setQuantity(quantity);
                        resource->setPrice(price);
                    }
                }
            }
            else if (section == "ARMY_DATA") {
                if (armyDataLine == 0) {
                    // Create new army with size
                    int size = std::stoi(line);
                    army = std::make_unique<Army>(size);
                    armyDataLine++;
                }
                else if (armyDataLine == 1) {
                    // Set training level
                    int trainingLevel = std::stoi(line);
                    // We could implement functionality to actually set this value
                    // but for now we'll just move on
                    armyDataLine++;
                }
                else if (armyDataLine == 2) {
                    // Set morale
                    int morale = std::stoi(line);
                    armyDataLine++;
                }
                else if (armyDataLine == 3) {
                    // Set maintenance cost
                    double maintenanceCost = std::stod(line);
                    armyDataLine++;
                }
                else if (armyDataLine == 4) {
                    // Set isPaid
                    bool isPaid = (line == "1");
                    armyDataLine = 0; // Reset for next time
                }
            }
            else if (section == "BANK_DATA") {
                if (bankDataLine == 0) {
                    // Set treasury
                    double treasury = std::stod(line);
                    bank = std::make_unique<Bank>(treasury);
                    bankDataLine++;
                }
                else if (bankDataLine == 1) {
                    // Set loan amount
                    double loanAmount = std::stod(line);
                    bankDataLine++;
                }
                else if (bankDataLine == 2) {
                    // Set interest rate
                    double interestRate = std::stod(line);
                    bankDataLine++;
                }
                else if (bankDataLine == 3) {
                    // Set loan due time
                    int loanDueTime = std::stoi(line);
                    bankDataLine++;
                }
                else if (bankDataLine == 4) {
                    // Set corruption level
                    int corruptionLevel = std::stoi(line);
                    bank->setCorruptionLevel(corruptionLevel);
                    bankDataLine = 0; // Reset for next time
                }
            }
            // Similar parsing for other sections
        }

        // Close file safely
        loadFile.close();

        std::cout << "Game loaded successfully from: " << filename << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading game: " << e.what() << std::endl;
        throw GameException("Failed to load game: " + std::string(e.what()));
    }
}

// Update validateSaveFile method to provide better error messages
bool Kingdom::validateSaveFile(const std::string& filename) {
    std::ifstream testFile(filename);
    if (!testFile.is_open()) {
        std::cerr << "Error: Could not open file: " << filename << std::endl;
        return false;
    }

    // Basic validation - check for required sections
    bool hasKingdomData = false;
    bool hasPopulationData = false;
    bool hasResourcesData = false;

    std::string line;
    while (std::getline(testFile, line)) {
        if (line == "KINGDOM_DATA") hasKingdomData = true;
        else if (line == "POPULATION_DATA") hasPopulationData = true;
        else if (line == "RESOURCES_DATA") hasResourcesData = true;
    }

    testFile.close();

    if (!hasKingdomData || !hasPopulationData || !hasResourcesData) {
        std::cerr << "Error: File " << filename << " is not a valid save file" << std::endl;
        return false;
    }

    return true;
}

void Kingdom::randomEvent() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 5);
    int eventType = distrib(gen);

    switch (eventType) {
    case 0: // Plaguef
        cout << "A plague has spread in your kingdom! Population decreases dramatically!" << endl;
        population.triggerPlague();
        break;
    case 1: // Good harvest
        cout << "Excellent harvest this year! Food supplies increased." << endl;
        resources.at("food").addQuantity(100);
        break;
    case 2: // Drought
        cout << "A severe drought has affected your kingdom. Food production decreased." << endl;
        resources.at("food").consumeQuantity(resources.at("food").getQuantity() / 3);
        break;
    case 3: // Gold discovery
        cout << "Gold has been discovered in your kingdom!" << endl;
        resources.at("gold").addQuantity(20);
        break;
    case 4: // Trade opportunity
        cout << "A foreign merchant offers special trade opportunities." << endl;
        // Implementation would depend on other mechanics
        break;
    case 5: // Assassination attempt
        if (politics->getCurrentKing()) {
            cout << "An assassination attempt on King " << politics->getCurrentKing()->getName() << "!" << endl;
            std::uniform_int_distribution<> successDist(1, 100);
            int success = successDist(gen);
            if (success <= 20) { // 20% chance of success
                cout << "The king has been assassinated!" << endl;
                // Set king to nullptr and trigger election
                gameOver = true;
            }
            else {
                cout << "The assassination attempt was foiled!" << endl;
            }
        }
        break;
    }
}