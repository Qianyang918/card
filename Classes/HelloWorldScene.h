/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include <unordered_map>
#include <vector>

#include "cocos2d.h"
#include "CardGameManager.h"

class HelloWorld : public cocos2d::Scene
{
public:
    // Initialize scene and UI.
    virtual bool init() override;

    // Create and return the scene instance.
    static cocos2d::Scene* scene();

    CREATE_FUNC(HelloWorld);

private:
    // Create a single card node and its touch handler.
    cocos2d::Node* createCardNode(int cardId);
    // Rebuild all card nodes and refresh the scene.
    void rebuildAllCards();
    // Refresh one card according to its current zone.
    void refreshCardVisual(int cardId);
    // Refresh the bottom pile and keep only the top card visible.
    void refreshHandPile();
    // Refresh the reserve pile.
    void refreshStockPile();
    // Refresh the status text.
    void refreshStatusText(const std::string& text);
    // Try moving a table card onto the bottom pile.
    void tryMoveTableCard(int cardId);
    // Draw one card from reserve pile to bottom pile.
    void drawFromStock();
    // Undo the last move.
    void undoLastMove();

    // Get a card position in the table area.
    cocos2d::Vec2 getTableCardPosition(const CardData& card) const;
    // Get a card position in the reserve pile.
    cocos2d::Vec2 getStockCardPosition(int index) const;
    // Get the top card position in the bottom pile.
    cocos2d::Vec2 getHandCardPosition(int handIndex) const;
    // Get the clickable rectangle of a card node.
    cocos2d::Rect getCardTouchRect(cocos2d::Node* node) const;

    // Get the suit image path.
    std::string getSuitFile(CardSuit suit) const;
    // Get the big number image path.
    std::string getBigNumberFile(const CardData& card) const;
    // Get the small number image path.
    std::string getSmallNumberFile(const CardData& card) const;
    // Check whether the suit is red.
    bool isRedSuit(CardSuit suit) const;
    // Convert card value to display text.
    std::string valueToString(int value) const;

private:
    CardGameManager _gameManager;
    cocos2d::Node* _tableRoot;
    cocos2d::Node* _stockRoot;
    cocos2d::Node* _handRoot;
    cocos2d::Label* _statusLabel;
    cocos2d::Label* _handTitleLabel;
    cocos2d::MenuItemLabel* _undoButton;
    std::unordered_map<int, cocos2d::Node*> _cardNodes;
};

#endif // __HELLOWORLD_SCENE_H__
