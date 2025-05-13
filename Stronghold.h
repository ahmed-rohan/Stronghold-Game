#ifndef STRONGHOLD_H
#define STRONGHOLD_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>

namespace std {
    // Forward declarations
    class Kingdom;
    class Population;
    class Army;
    class Bank;
    class Market;
    class Politics;
    class Leader;

    // Exception classes
    class GameException : public exception {
    private:
        string message;
    public:
        GameException(const string& msg) : message(msg) {}
        const char* what() const noexcept override {
            return message.c_str();
        }
    };

    class ResourceException : public GameException {
    public:
        ResourceException(const string& msg) : GameException("Resource Error: " + msg) {}
    };

    class EconomyException : public GameException {
    public:
        EconomyException(const string& msg) : GameException("Economy Error: " + msg) {}
    };

    // Template class for resources
    template <typename T>
    class Resource {
    private:
        string name;
        T quantity;
        T maxQuantity;
        double price;

    public:
        Resource() : name(""), quantity(0), maxQuantity(0), price(0.0) {}

        Resource(const string& name, T initialQuantity, T maxQty, double initialPrice)
            : name(name), quantity(initialQuantity), maxQuantity(maxQty), price(initialPrice) {}

        void setQuantity(T q) {
            if (q < 0) {
                throw ResourceException("Cannot set negative quantity for " + name);
            }
            if (q > maxQuantity) {
                throw ResourceException("Exceeds maximum storage capacity for " + name);
            }
            quantity = q;
        }

        T getQuantity() const { return quantity; }

        void addQuantity(T amount) {
            if (quantity + amount > maxQuantity) {
                throw ResourceException("Not enough storage for " + name);
            }
            quantity += amount;
        }

        bool consumeQuantity(T amount) {
            if (quantity < amount) {
                return false;
            }
            quantity -= amount;
            return true;
        }

        double getPrice() const { return price; }
        void setPrice(double newPrice) { price = newPrice; }
        string getName() const { return name; }
    };

    // Social class enumeration
    enum class SocialClass {
        PEASANT,
        MERCHANT,
        NOBILITY,
        MILITARY
    };

    // Population class to handle demographics
    class Population {
    private:
        int totalPopulation;
        map<SocialClass, int> classDemographics;
        double happiness;
        int growthRate;
        int deathRate;
        bool plagueActive;

    public:
        Population(int initialPopulation = 1000);
        void update(bool hasFood, bool hasHealthcare, int jobAvailability);
        void triggerPlague();
        void endPlague();
        bool isUnhappy() const;
        int getTotalPopulation() const;
        int getClassPopulation(SocialClass socialClass) const;
        double getHappiness() const;
        void adjustHappiness(double amount);
        void migrate(SocialClass fromClass, SocialClass toClass, int amount);
    };

    // Base Leader class
    class Leader {
    protected:
        string name;
        int influence;
        int corruption;
        int leadership;

    public:
        Leader(const string& name, int influence, int corruption, int leadership);
        virtual ~Leader() = default;
        virtual void makeDecision(Kingdom& kingdom) = 0;
        virtual string getTitle() const = 0;
        string getName() const;
        int getInfluence() const;
        int getCorruption() const;
        int getLeadership() const;
    };

    // King class derived from Leader
    class King : public Leader {
    private:
        int reignYears;
        int popularity;
        string leadershipStyle;

    public:
        King(const string& name, int influence, int corruption, int leadership,
            const string& style);
        void makeDecision(Kingdom& kingdom) override;
        string getTitle() const override;
        void setTaxRate(double rate);
        void declareWar(Kingdom& targetKingdom);
        bool canBeBribes(int goldAmount) const;
    };

    // Commander class derived from Leader
    class Commander : public Leader {
    private:
        int battleExperience;
        int strategySkill;
        bool loyal;

    public:
        Commander(const string& name, int influence, int corruption, int leadership,
            int experience, int strategy, bool loyalty);
        void makeDecision(Kingdom& kingdom) override;
        string getTitle() const override;
        bool isLoyal() const;
        int getStrategyBonus() const;
    };

    // MerchantGuildLeader class derived from Leader
    class MerchantGuildLeader : public Leader {
    private:
        double tradingBonus;
        vector<string> tradeConnections;

    public:
        MerchantGuildLeader(const string& name, int influence, int corruption, int leadership,
            double bonus);
        void makeDecision(Kingdom& kingdom) override;
        string getTitle() const override;
        void addTradeConnection(const string& connection);
        double getTradingBonus() const;
    };

    // Army class
    class Army {
    private:
        int size;
        int trainingLevel;
        int morale;
        double maintenanceCost;
        bool isPaid;
        unique_ptr<Commander> commander;

    public:
        Army(int initialSize = 0);
        ~Army();
        void recruit(int count, int populationSize);
        void train(int duration);
        bool battle(Army& enemyArmy);
        void payMaintenance(double amount);
        void updateMorale(bool hasFood, bool isPaid);
        int getSize() const;
        int getTrainingLevel() const;
        int getMorale() const;
        double getMaintenanceCost() const;
        bool getIsPaid() const;
        void setCommander(unique_ptr<Commander> newCommander);
        Commander* getCommander() const;
    };

    // Bank class
    class Bank {
    private:
        double treasury;
        double loanAmount;
        double interestRate;
        int loanDueTime;
        int corruptionLevel;

    public:
        Bank(double initialTreasury = 1000.0);
        bool withdraw(double amount);
        void deposit(double amount);
        double getLoan(double amount, double rate, int dueTime);
        bool repayLoan(double amount);
        bool audit();
        double getTreasury() const;
        double getLoanAmount() const;
        double getInterestRate() const;
        int getLoanDueTime() const;
        int getCorruptionLevel() const;
        void setCorruptionLevel(int level);
    };

    // Market class
    class Market {
    private:
        map<string, double> prices;
        double inflationRate;
        int tradingVolume;
        bool isOpen;

    public:
        Market();
        void updatePrices();
        double buyResource(const string& resourceName, int amount, Bank& bank);
        double sellResource(const string& resourceName, int amount, Bank& bank);
        void setInflationRate(double rate);
        double getResourcePrice(const string& resourceName) const;
        void open();
        void close();
        bool getIsOpen() const;
    };

    // Politics class
    class Politics {
    private:
        unique_ptr<King> currentKing;
        int stability;
        bool civilUnrest;
        bool atWar;
        vector<string> allies;
        vector<string> enemies;

    public:
        Politics();
        ~Politics();
        void electKing(unique_ptr<King> newKing);
        void coup(unique_ptr<King> usurper);
        void declareWar(const string& enemyKingdom);
        void makePeace(const string& kingdom);
        void formAlliance(const string& kingdom);
        void breakAlliance(const string& kingdom);
        int getStability() const;
        bool hasCivilUnrest() const;
        bool isAtWar() const;
        void setCivilUnrest(bool unrest);
        King* getCurrentKing() const;
    };

    // Kingdom class - the main game class
    class Kingdom {
    private:
        string name;
        Population population;
        unique_ptr<Army> army;
        unique_ptr<Bank> bank;
        unique_ptr<Market> market;
        unique_ptr<Politics> politics;
        map<string, Resource<int>> resources;
        bool gameOver;
        int currentTurn;

        // Helper methods
        void randomEvent();

    public:
        Kingdom(const string& name);
        ~Kingdom();

        void initializeResources();
        void update();
        void displayStatus() const;
        void processTurn();
        bool isGameOver() const;
        int getCurrentTurn() const { return currentTurn; }

        // Simplified methods that would handle files in a full implementation
        void saveGameState(const string& filename) const;
        void loadGameState(const string& filename);
        static bool validateSaveFile(const string& filename);

        // Getters
        string getName() const;
        Population& getPopulation();
        Army* getArmy() const;
        Bank* getBank() const;
        Market* getMarket() const;
        Politics* getPolitics() const;
        Resource<int>* getResource(const string& name);

        // Game actions
        void collectTaxes(double taxRate);
        void buildStructure(const string& structureName);
        void handleWar(Kingdom& enemyKingdom);
        void manageResources();
    };
}  // namespace std

#endif // STRONGHOLD_H