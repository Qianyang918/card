#pragma once

#include <unordered_map>
#include <vector>

#include "CardTypes.h"

class CardGameManager
{
public:
    // Initialize manager state.
    CardGameManager();

    // Build demo data for table, reserve and bottom piles.
    void setupDemo();

    // Get all table card ids.
    const std::vector<int>& getTableCardIds() const;
    // Get all reserve pile card ids.
    const std::vector<int>& getStockCardIds() const;
    // Get all bottom pile history card ids.
    const std::vector<int>& getHandCardIds() const;
    // Get readonly card data by id.
    const CardData* getCard(int cardId) const;
    // Get writable card data by id.
    CardData* getCard(int cardId);
    // Get current top card id in bottom pile.
    int getHandTopCardId() const;
    // Get remaining reserve pile count.
    int getRemainingStockCount() const;
    // Get next drawable reserve card id.
    int getNextStockCardId() const;

    // Check whether a table card can move to bottom pile.
    bool canMoveTableCardToHand(int cardId) const;
    // Check whether a table card is selectable.
    bool canSelectTableCard(int cardId) const;
    // Move a table card to bottom pile.
    MoveResult moveTableCardToHand(int cardId);

    // Check whether reserve pile still has cards.
    bool canDrawFromStock() const;
    // Draw one reserve card to bottom pile.
    MoveResult drawFromStock();

    // Check whether undo is available.
    bool canUndo() const;
    // Undo the last move.
    UndoResult undo();

private:
    // Create one card with initial state.
    CardData createCard(int value, CardSuit suit, CardZone zone, bool faceUp, int column, int row) const;
    // Check whether two values are adjacent.
    bool isAdjacentValue(int lhs, int rhs) const;
    // Check whether a table card is unblocked and selectable.
    bool isTableCardSelectable(const CardData& card) const;
    // Collect table cards that should flip face up.
    std::vector<int> collectCardsToFlip() const;
    // Push one card onto the bottom pile.
    void moveCardToHand(CardData& card);

private:
    int _nextCardId;
    int _handTopCardId;
    std::unordered_map<int, CardData> _cards;
    std::vector<int> _tableCardIds;
    std::vector<int> _stockCardIds;
    std::vector<int> _handCardIds;
    std::vector<MoveRecord> _history;
};
