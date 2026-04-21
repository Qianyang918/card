#pragma once

#include <vector>

enum class CardSuit
{
    Club,
    Diamond,
    Heart,
    Spade
};

enum class CardZone
{
    Table,
    Stock,
    Hand
};

enum class MoveType
{
    None,
    TableToHand,
    StockToHand
};

struct CardData
{
    int id = 0;
    int value = 0;
    CardSuit suit = CardSuit::Club;
    CardZone zone = CardZone::Table;
    bool faceUp = false;
    bool removed = false;
    int column = 0;
    int row = 0;
    std::vector<int> blockedBy;
};

struct MoveRecord
{
    MoveType type = MoveType::None;
    int movedCardId = 0;
    int previousHandTopCardId = 0;
    std::vector<int> flippedCardIds;
};

struct MoveResult
{
    bool success = false;
    MoveType type = MoveType::None;
    int movedCardId = 0;
    int previousHandTopCardId = 0;
    std::vector<int> flippedCardIds;
};

struct UndoResult
{
    bool success = false;
    MoveType undoneType = MoveType::None;
    int movedCardId = 0;
    int restoredHandTopCardId = 0;
    std::vector<int> revertedFaceDownCardIds;
};
