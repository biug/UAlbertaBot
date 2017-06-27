#include "UCTSearch.h"

using namespace SparCraft;
using namespace std;
void perm(int num, int m, vector<vector<int> > & dst)
{

	if (num == 1)
	{
		for (int j = 0; j < m; j++)
		{
			vector<int> temp2;
			temp2.push_back(j);
			dst.push_back(temp2);
		}

	}
	else
	{
		perm(num - 1, m, dst);
		vector<vector<int> >  temp = dst;
		dst.clear();
		for (int i = 0; i < temp.size(); i++)
		{
			for (int j = 0; j < m; j++)
			{
				vector<int> temp2 = temp[i];
				temp2.push_back(j);
				dst.push_back(temp2);
			}
		}

	}

}

UCTSearch::UCTSearch(const UCTSearchParameters & params) 
	: _params(params)
    , _memoryPool(NULL)
{
    for (size_t p(0); p<Constants::Num_Players; ++p)
    {
        // set ordered move script player objects
        for (size_t s(0); s<_params.getOrderedMoveScripts().size(); ++s)
        {
            _allScripts[p].push_back(AllPlayers::getPlayerPtr(p, _params.getOrderedMoveScripts()[s]));
        }

        // set player model objects
        if (_params.playerModel(p) != PlayerModels::None)
        {
            _playerModels[p] = AllPlayers::getPlayerPtr(p, _params.playerModel(p));
        }
    }
	

}

void UCTSearch::setMemoryPool(UCTMemoryPool * pool)
{
    _memoryPool = pool;
}

void UCTSearch::doSearch(GameState & initialState, std::vector<UnitAction> & move)
{
    Timer t;
    t.start();
	//MoveArray                               moves;
	//std::cout << 11111 << endl;
	//initialState.generateMoves(moves, Players::Player_None);
	//_allScripts[Players::Player_None][0]->getMoves(initialState, moves, _actionVec);
	//	for (int i = 0; i < _actionVec.size(); i++)
	//		_actionVec[i]._scriptIndex = 0;
	//	std::cout << 11111 << endl;
    _rootNode = UCTNode(NULL, Players::Player_None, SearchNodeType::RootNode, _actionVec, _params.maxChildren(), _memoryPool ? _memoryPool->alloc() : NULL);
    // do the required number of traversals
	size_t traversals(0);
    for (traversals=0; traversals < _params.maxTraversals(); ++traversals)
    {
        GameState state(initialState);
        traverse(_rootNode, state);

        if (traversals && (traversals % 5 == 0))
        {
            if (_params.timeLimit() && (t.getElapsedTimeInMilliSec() >= _params.timeLimit()))
            {
                break;
            }
        }

        _results.traversals++;

        //printSubTree(_rootNode, initialState, "__uct.txt");
        //system("\"C:\\Program Files (x86)\\Graphviz2.30\\bin\\dot.exe\" < __uct.txt -Tpng > uct.png");
    }
	
    // choose the move to return
    if (_params.rootMoveSelectionMethod() == UCTMoveSelect::HighestValue)
    {
        move = _rootNode.bestUCTValueChild(true, _params).getMove();
    }
    else if (_params.rootMoveSelectionMethod() == UCTMoveSelect::MostVisited)
    {
        move = _rootNode.mostVisitedChild().getMove();
    }

    if (_params.graphVizFilename().length() > 0)
    {
        //printSubTree(_rootNode, initialState, _params.graphVizFilename());
        //system("\"C:\\Program Files (x86)\\Graphviz2.30\\bin\\dot.exe\" < __uct.txt -Tpng > uct.png");
    }

    double ms = t.getElapsedTimeInMilliSec();
    _results.timeElapsed = ms;
    //printf("Search took %lf ms\n", ms);
    //printf("Hello\n");
}

const bool UCTSearch::searchTimeOut()
{
	return (_params.timeLimit() && (_searchTimer.getElapsedTimeInMilliSec() >= _params.timeLimit()));
}

const bool UCTSearch::terminalState(GameState & state, const size_t & depth) const
{
	return (depth <= 0 || state.isTerminal());
}

void UCTSearch::generateOrderedMoves(GameState & state, MoveArray & moves, const IDType & playerToMove)
{
	_orderedMoves.clear();


	//std::cout << (int)playerToMove <<std::endl; 
	//for (size_t idx(0); idx < moves.numUnits();idx++)
	//	std::cout <<"numMoves: "<< (int)moves.numMoves(idx) << std::endl;
	//system("pause");
	// if we are using opponent modeling, get the move and then return, we don't want to put any more moves in
    if (_params.playerModel(playerToMove) != PlayerModels::None)
	{
        // put the vector into the ordered moves array
        _orderedMoves.add(std::vector<UnitAction>());

        // generate the moves into that vector
		_playerModels[playerToMove]->getMoves(state, moves, _orderedMoves[0]);
		
		return;
	}

	// if we are using script move ordering, insert the script moves we want
    if (_params.moveOrderingMethod() == MoveOrderMethod::ScriptFirst)
    {
        for (size_t s(0); s<_params.getOrderedMoveScripts().size(); s++)
	    {
            std::vector<UnitAction> moveVec;
		    _allScripts[playerToMove][s]->getMoves(state, moves, moveVec);
			for (int i = 0; i < moveVec.size(); i++)
				moveVec[i]._scriptIndex = s;
		    _orderedMoves.add(moveVec);
	    }
    }
	if (_params.moveOrderingMethod() == MoveOrderMethod::ScriptAll)
	{
		std::vector<std::vector<UnitAction>> moveVecAll;
		for (size_t s(0); s<_params.getOrderedMoveScripts().size(); s++)
		{
			IDType player, enemyplayer;
			std::vector<UnitAction> moveVec;
			_allScripts[playerToMove][s]->getMoves(state, moves, moveVec);
			moveVecAll.push_back(moveVec);
			_orderedMoves.add(moveVec);
		}
		vector<vector<int>> dst;
		perm(moves.numUnits(), moveVecAll.size(),dst);

		for (int s(1); s < dst.size()-1; s++)
		{
			std::vector<UnitAction> moveVec;
			//vector<int> idx = dst[s];
			for (size_t t(0); t < moves.numUnits(); t++)
			{
				int scriptIdx = dst[s][t];
				UnitAction cScript = moveVecAll[scriptIdx][t];
				if (cScript.unit() != t)
					system("pause");
				moveVec.push_back(cScript);
			}
			int is_exist = 1;
			for (size_t t(0); t < _orderedMoves.size(); t++)
			{
				int is_same = 1;
				for (int z = 0; z < moveVec.size(); z++)
				{
					if (!(moveVec[z] == _orderedMoves[t][z]))
					{
						is_same = 0;
						break;
					}
				}
				if (is_same == 0)
				{
					is_exist = 0;
					break;
				}

			}
			if (is_exist==0)
			_orderedMoves.add(moveVec);
		}

		/*std::cout << _orderedMoves.size() << " " << dst.size() << endl;
		system("pause");*/
		

			/*for (size_t t(0); t < moveVec.size(); t++)
				std::cout << (int)moveVec[t].unit() << " " << moveVec[t].moveString() << std::endl;

			std::cout << "============================" << std::endl;
			
			_orderedMoves.add(moveVec);*/
		
		//system("pause");
	}
	
}
const size_t UCTSearch::getChildNodeType(UCTNode & parent, const GameState & prevState) const
{
    if (!prevState.bothCanMove())
    {
        return SearchNodeType::SoloNode;
    }
    else
    {
        if (parent.getNodeType() == SearchNodeType::RootNode)
        {
            return SearchNodeType::FirstSimNode;
        }
        else if (parent.getNodeType() == SearchNodeType::SoloNode)
        {
            return SearchNodeType::FirstSimNode;
        }
        else if (parent.getNodeType() == SearchNodeType::SecondSimNode)
        {
            return SearchNodeType::FirstSimNode;
        }
        else if (parent.getNodeType() == SearchNodeType::FirstSimNode)
        {
            return SearchNodeType::SecondSimNode;
        }
    }

    return SearchNodeType::Default;
}

const bool UCTSearch::getNextMove(IDType playerToMove, MoveArray & moves, const size_t & moveNumber, std::vector<UnitAction> & actionVec)
{
    if (moveNumber > _params.maxChildren())
    {
        return false;
    }

    // if this move is beyond the first, check to see if we are only using a single move
    if (moveNumber == 1)
    {
        // if we are player modeling, we should have only generated the first move
        if (_params.playerModel(playerToMove) != PlayerModels::None)
	    {
            // so return false
		    return false;
	    }
    }

    actionVec.clear();

	// if this move should be from the ordered list, return it from the list
	if (moveNumber < _orderedMoves.size())
	{
        actionVec.assign(_orderedMoves[moveNumber].begin(), _orderedMoves[moveNumber].end());
        return true;
	}
	// otherwise return the next move vector starting from the beginning
	else
	{
		if (_params.moveOrderingMethod() == MoveOrderMethod::ScriptAll)
		{
			return false;
			if (moves.hasMoreMoves())
			{
				moves.getNextMoveVec(actionVec);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (moves.hasMoreMoves())
			{
				moves.getNextMoveVec(actionVec);
				return true;
			}
			else
			{
				return false;
			}
		}
        
	}
}

const IDType UCTSearch::getPlayerToMove(UCTNode & node, const GameState & state) const
{
	const IDType whoCanMove(state.whoCanMove());

	// if both players can move
	if (whoCanMove == Players::Player_Both)
	{
        // pick the first move based on our policy
		const IDType policy(_params.playerToMoveMethod());
		const IDType maxPlayer(_params.maxPlayer());

        // the max player always chooses at the root
        if (isRoot(node))
        {
            return maxPlayer;
        }

        // the type of node this is
        const IDType nodeType = node.getNodeType();

        // the 2nd player in a sim move is always the enemy of the first
        if (nodeType == SearchNodeType::FirstSimNode)
        {
            return state.getEnemy(node.getPlayer());
        }
        // otherwise use our policy to see who goes first in a sim move state
        else
        {
		    if (policy == SparCraft::PlayerToMove::Alternate)
		    {
			    return state.getEnemy(node.getPlayer());
		    }
		    else if (policy == SparCraft::PlayerToMove::Not_Alternate)
		    {
			    return node.getPlayer();
		    }
		    else if (policy == SparCraft::PlayerToMove::Random)
		    {
			    return rand() % 2;
		    }

            // we should never get to this state
		    System::FatalError("UCT Error: Nobody can move for some reason");
		    return Players::Player_None;
        }
	}
	else
	{
		return whoCanMove;
	}
}

UCTNode & UCTSearch::UCTNodeSelect(UCTNode & parent)
{
    UCTNode *   bestNode    = NULL;
    bool        maxPlayer   = isRoot(parent) || (parent.getChild(0).getPlayer() == _params.maxPlayer());
    double      bestVal     = maxPlayer ? std::numeric_limits<double>::min() : std::numeric_limits<double>::max();
         
    // loop through each child to find the best node
    for (size_t c(0); c < parent.numChildren(); ++c)
    {
        UCTNode & child = parent.getChild(c);

		double currentVal(0);
	
        // if we have visited this node already, get its UCT value
		if (child.numVisits() > 0)
		{
			double winRate    = (double)child.numWins() / (double)child.numVisits();
            double uctVal     = _params.cValue() * sqrt( log( (double)parent.numVisits() ) / ( child.numVisits() ) );
			currentVal        = maxPlayer ? (winRate + uctVal) : (winRate - uctVal);
            
            child.setUCTVal(currentVal);
		}
		else
		{
            // if we haven't visited it yet, return it and visit immediately
			return child;
		}

        // choose the best node depending on max or min player
        if (maxPlayer)
        {
            if (currentVal > bestVal)
            {
                bestVal             = currentVal;
			    bestNode            = &child;
            }
        }
        else if (currentVal < bestVal)
        {
            bestVal             = currentVal;
			bestNode            = &child;
        }
	}

    return *bestNode;
}

void UCTSearch::updateState(UCTNode & node, GameState & state, bool isLeaf)
{
    // if it's the first sim move with children, or the root node
    if ((node.getNodeType() != SearchNodeType::FirstSimNode) || isLeaf)
    {
        // if this is a second sim node
        if (node.getNodeType() == SearchNodeType::SecondSimNode)
        {
            // make the parent's moves on the state because they haven't been done yet
            state.makeMoves(node.getParent()->getMove());
        }

        // do the current node moves and call finished moving
        state.makeMoves(node.getMove());
        state.finishedMoving();
    }
}
StateEvalScore UCTSearch::traverse(UCTNode & node, GameState & currentState)
{
    StateEvalScore playoutVal;

    _results.totalVisits++;

    // if we haven't visited this node yet, do a playout
    if (node.numVisits() == 0)
    {
        // update the status of the current state with this node's moves
        //updateState(node, currentState, !node.hasChildren());
        updateState(node, currentState, true);
		
        // do the playout
		if (1==0)//_params.moveOrderingMethod() == MoveOrderMethod::ScriptAll)// && node.getParent()!=NULL)
		{
			StateEvalScore  score(0,0);
			for (size_t idx1(0); idx1 < _params.getOrderedMoveScripts().size(); ++idx1)
			{
				for (size_t idx2(0); idx2 < _params.getOrderedMoveScripts().size(); ++idx2)
				{
					score = currentState.eval(_params.maxPlayer(), _params.evalMethod(), _params.getOrderedMoveScripts()[idx1], _params.getOrderedMoveScripts()[idx2]);
					if (score>playoutVal)
						playoutVal = score;
				}
			}
			


			//std::cout << 1 << std::endl;
		//	const IDType player(getPlayerToMove(node, currentState));
		//	const IDType enemyPlayer(currentState.getEnemy(player));
		//	UnitScriptData currentScriptData;
		//	for (size_t unitIndex(0); unitIndex < currentState.numUnits(player); ++unitIndex)
		//	{
		//		currentScriptData.setUnitScript(currentState.getUnit(player, unitIndex), _params.getOrderedMoveScripts()[0]);
		//	}
		//	for (size_t unitIndex(0); unitIndex < currentState.numUnits(enemyPlayer); ++unitIndex)
		//	{
		//		currentScriptData.setUnitScript(currentState.getUnit(enemyPlayer, unitIndex), _params.getOrderedMoveScripts()[0]);
		//	}

		///*	_params.getOrderedMoveScripts();
		//	_playerScriptPortfolio.push_back(PlayerModels::NOKDPS);
		//	_playerScriptPortfolio.push_back(PlayerModels::KiterDPS);*/
		//	for (size_t unitIndex(0); unitIndex < currentState.numUnits(player); ++unitIndex)
		//	{
		//		//std::cout << (int)unitIndex << std::endl;
		//		const Unit & unit(currentState.getUnit(player, unitIndex));
		//		//std::cout << node.getMove().size() << std::endl;
		//		for (size_t sIndex(0); sIndex < node.getMove().size(); ++sIndex)
		//		{
		//			/*std::cout << (int) unitIndex << " " << (int)currentState.numUnits(player) << " " << (int)player << " ssss ";
		//			std::cout << (int)node.getMove()[sIndex]._unit << " " << (int) node.getMove().size() << " " << (int)node.getMove()[sIndex]._player << std::endl;
		//			system("pause");*/
		//			if (node.getMove()[sIndex]._unit == unitIndex && node.getMove()[sIndex]._player == player)
		//			{
		//				
		//				
		//				size_t idx = node.getMove()[sIndex]._scriptIndex;
		//				std::cout << _params.getOrderedMoveScripts()[idx] << endl;
		//				currentScriptData.setUnitScript(unit, _params.getOrderedMoveScripts()[idx]);
		//			
		//				break;
		//			}
		//			
		//			
		//		}
		//	}
		//	for (size_t unitIndex(0); unitIndex < currentState.numUnits(enemyPlayer); ++unitIndex)
		//	{
		//		//std::cout << (int)unitIndex << std::endl;
		//		const Unit & unit(currentState.getUnit(enemyPlayer, unitIndex));
		//		for (size_t sIndex(0); sIndex < node.getMove().size(); ++sIndex)
		//		{
		//			if (node.getMove()[sIndex]._unit == unitIndex && node.getMove()[sIndex]._player == enemyPlayer)
		//			{
		//				currentScriptData.setUnitScript(unit, node.getMove()[sIndex]._scriptIndex);
		//				break;
		//			}


		//		}
		//	}
		//
		//	playoutVal = eval(player, currentState, currentScriptData);
			
			
		}
		else
		{
			playoutVal = currentState.eval(_params.maxPlayer(), _params.evalMethod(), _params.simScript(Players::Player_One), _params.simScript(Players::Player_Two));
		}
        

        _results.nodesVisited++;
    }
    // otherwise we have seen this node before
    else
    {
        // update the state for a non-leaf node
        updateState(node, currentState, false);

        if (currentState.isTerminal())
        {


            playoutVal = currentState.eval(_params.maxPlayer(), EvaluationMethods::LTD2);
        }
        else
        {
            // if the children haven't been generated yet
            if (!node.hasChildren())
            {
                generateChildren(node, currentState);
            }

            UCTNode & next = UCTNodeSelect(node);
            playoutVal = traverse(next, currentState);
        }
    }

    node.incVisits();
    
    if (playoutVal.val() > 0)
    {
        node.addWins(1);
    }
    else if (playoutVal.val() == 0)
    {
        node.addWins(0.5);
    }

    return playoutVal;
}

// generate the children of state 'node'
// state is the GameState after node's moves have been performed
void UCTSearch::generateChildren(UCTNode & node, GameState & state)
{
    // figure out who is next to move in the game
    const IDType playerToMove(getPlayerToMove(node, state));

    // generate all the moves possible from this state
	state.generateMoves(_moveArray, playerToMove);
    _moveArray.shuffleMoveActions();

    // generate the 'ordered moves' for move ordering
    generateOrderedMoves(state, _moveArray, playerToMove);

    // for each child of this state, add a child to the current node
	size_t child(0);
    for (child=0; (child < _params.maxChildren()) && getNextMove(playerToMove, _moveArray, child, _actionVec); ++child)
    {
        // add the child to the tree
        node.addChild(&node, playerToMove, getChildNodeType(node, state), _actionVec, _params.maxChildren(), _memoryPool ? _memoryPool->alloc() : NULL);
        _results.nodesCreated++;
    }
	/*std::cout << child << std::endl;
	system("pause")*/
	
}

StateEvalScore UCTSearch::performPlayout(GameState & state)
{
    GameState copy(state);
    copy.finishedMoving();

    return copy.eval(_params.maxPlayer(), _params.evalMethod(), _params.simScript(Players::Player_One), _params.simScript(Players::Player_Two));
}

const bool UCTSearch::isRoot(UCTNode & node) const
{
    return &node == &_rootNode;
}

void UCTSearch::printSubTree(UCTNode & node, GameState s, std::string filename)
{
    std::ofstream out(filename.c_str());

    GraphViz::Graph G("g");
    G.set("bgcolor", "#ffffff");

    printSubTreeGraphViz(node, G, s);

    G.print(out);
}

void UCTSearch::printSubTreeGraphViz(UCTNode & node, GraphViz::Graph & g, GameState state)
{
    if (node.getNodeType() == SearchNodeType::FirstSimNode && node.hasChildren())
    {
        // don't make any moves if it is a first simnode
    }
    else
    {
        if (node.getNodeType() == SearchNodeType::SecondSimNode)
        {
            state.makeMoves(node.getParent()->getMove());
        }

        state.makeMoves(node.getMove());
        state.finishedMoving();
    }

    std::stringstream label;
    std::stringstream move;

    for (size_t a(0); a<node.getMove().size(); ++a)
    {
        move << node.getMove()[a].moveString() << "\\n";
    }

    if (node.getMove().size() == 0)
    {
        move << "root";
    }

    std::string firstSim = SearchNodeType::getName(node.getNodeType());

    Unit p1 = state.getUnit(0,0);
    Unit p2 = state.getUnit(1,0);

    label   << move.str() 
            << "\\nVal: "       << node.getUCTVal() 
            << "\\nWins: "      << node.numWins() 
            << "\\nVisits: "    << node.numVisits() 
            << "\\nChildren: "  << node.numChildren() 
            << "\\n"            << firstSim
            << "\\nPtr: "       << &node
            << "\\n---------------"
            << "\\nFrame: " << state.getTime()
            << "\\nHP: " << p1.currentHP() << "  " << p2.currentHP()
            << "\\nAtk: " << p1.nextAttackActionTime() << "  " << p2.nextAttackActionTime()
            << "\\nMove: " << p1.nextMoveActionTime() << "  " << p2.nextMoveActionTime()
            << "\\nPrev: " << p1.previousActionTime() << "  " << p2.previousActionTime();
    
    std::string fillcolor       ("#aaaaaa");

    if (node.getPlayer() == Players::Player_One)
    {
        fillcolor = "#ff0000";
    }
    else if (node.getPlayer() == Players::Player_Two)
    {
        fillcolor = "#00ff00";
    }
    
    GraphViz::Node n(getNodeIDString(node));
    n.set("label",      label.str());
    n.set("fillcolor",  fillcolor);
    n.set("color",      "#000000");
    n.set("fontcolor",  "#000000");
    n.set("style",      "filled,bold");
    n.set("shape",      "box");
    g.addNode(n);

    // recurse for each child
    for (size_t c(0); c<node.numChildren(); ++c)
    {
        UCTNode & child = node.getChild(c);
        if (child.numVisits() > 0)
        {
            GraphViz::Edge edge(getNodeIDString(node), getNodeIDString(child));
            g.addEdge(edge);
            printSubTreeGraphViz(child, g, state);
        }
    }
}
 
std::string UCTSearch::getNodeIDString(UCTNode & node)
{
    std::stringstream ss;
    ss << (unsigned long long)&node;
    return ss.str();
}

UCTSearchResults & UCTSearch::getResults()
{
    return _results;
}
StateEvalScore UCTSearch::eval(const IDType & player, const GameState & state, UnitScriptData & playerScriptsChosen)
{
	const IDType enemyPlayer(state.getEnemy(player));

	Game g(state, 200);

	g.playIndividualScripts(playerScriptsChosen);

	return g.getState().eval(player, SparCraft::EvaluationMethods::LTD2);
}